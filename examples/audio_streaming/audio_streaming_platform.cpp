/*
 * This file is part of Anbox Platform SDK
 *
 * Copyright 2021 Canonical Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "anbox-platform-sdk/plugin.h"

#include <chrono>
#include <future>
#include <mutex>
#include <queue>
#include <iostream>
#include <stdexcept>
#include <thread>

#include <string.h>
#include <math.h>
#include <limits.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

namespace chrono = std::chrono;

#ifndef SYSTEM_LIBDIR
#define SYSTEM_LIBDIR
#endif

namespace {
// This will load the ANGLE based Null OpenGL driver implementation which the Anbox
// runtime includes by default. It will not provide any rendered pixels but is
// sufficient for first tests.
constexpr const char* opengl_es1_cm_driver_path = SYSTEM_LIBDIR "/anbox/angle/libGLESv1_CM.so";
constexpr const char* opengl_es2_driver_path = SYSTEM_LIBDIR  "/anbox/angle/libGLESv2.so";
constexpr const char* egl_driver_path = SYSTEM_LIBDIR  "/anbox/angle/libEGL.so";

constexpr const char* output_url = "rtp://127.0.0.1:37777";
constexpr int max_audio_buffer_size = 50 * 1024;
constexpr int frame_duration = 100;
} // namespace

namespace anbox {
struct AudioBuffer {
  uint8_t data[max_audio_buffer_size];
  std::atomic<uint32_t> size{0};
  std::mutex mutex;
};

struct Context {
  AVFormatContext* format_context{nullptr};
  AVCodecContext*  codec_context{nullptr};
  AVStream* stream{nullptr};
  AVFrame*  frame{nullptr};
  AVPacket  pkt;
  uint8_t*  frame_buffer{nullptr};
  size_t    frame_buffer_size{0};
  uint32_t  pts_index{0};
};

static AVSampleFormat audio_format_to_av_sample_format(AnboxAudioFormat format) {
  switch (format) {
  case AUDIO_FORMAT_PCM_8_BIT:
    return AV_SAMPLE_FMT_U8;
  case AUDIO_FORMAT_PCM_16_BIT:
    return AV_SAMPLE_FMT_S16;
  case AUDIO_FORMAT_PCM_32_BIT:
    return AV_SAMPLE_FMT_S32;
  case AUDIO_FORMAT_PCM_FLOAT:
    return AV_SAMPLE_FMT_FLT;
  case AUDIO_FORMAT_PCM_8_24_BIT:
  case AUDIO_FORMAT_PCM_24_BIT_PACKED:
  default:
    return AV_SAMPLE_FMT_NONE;
  }
}

static const char* audio_format_to_string(AnboxAudioFormat format) {
  switch (format){
  case AUDIO_FORMAT_INVALID:
    return "AUDIO_FORMAT_INVALID";
  case AUDIO_FORMAT_DEFAULT:
    return "AUDIO_FORMAT_DEFAULT";
  case AUDIO_FORMAT_PCM_16_BIT:
    return "AUDIO_FORMAT_PCM_16_BIT";
  case AUDIO_FORMAT_PCM_8_BIT:
    return "AUDIO_FORMAT_PCM_8_BIT";
  case AUDIO_FORMAT_PCM_32_BIT:
    return "AUDIO_FORMAT_PCM_32_BIT";
  case AUDIO_FORMAT_PCM_8_24_BIT:
    return "AUDIO_FORMAT_PCM_8_24_BIT";
  case AUDIO_FORMAT_PCM_FLOAT:
    return "AUDIO_FORMAT_PCM_FLOAT";
  case AUDIO_FORMAT_PCM_24_BIT_PACKED:
    return "AUDIO_FORMAT_PCM_24_BIT_PACKED";
  };
}

static int audio_channel_to_av_channel_format(uint8_t channels) {
  switch (channels) {
  case 1:
    return AV_CH_LAYOUT_MONO;
  case 2:
    return AV_CH_LAYOUT_STEREO;
  default:
    return -EINVAL;
  }
}


static int audio_encode(AVCodecContext *codec_context, AVPacket *packet, AVFrame *frame, int *encoded) {
  auto ret = avcodec_send_frame(codec_context, frame);
  if (ret < 0)
    return ret;

  *encoded = 0;
  do {
    ret = avcodec_receive_packet(codec_context, packet);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
      break;
    } else if (ret < 0) {
      std::cerr << "Failed to do audio encode" << std::endl;
      return ret;
    } else {
      *encoded = 1;
    }
  } while (ret > 0);
  return 0;
}

class AudioStreamingPlatformAudioProcessor : public AudioProcessor {
 public:
  AudioStreamingPlatformAudioProcessor(const AnboxAudioSpec& audio_spec);
  ~AudioStreamingPlatformAudioProcessor() override;

  size_t process_data(const uint8_t* data, size_t size) override;
  ssize_t write_data(const uint8_t* data, size_t size) override;
  ssize_t read_data(uint8_t* data, size_t size) override;

 private:
  int configure_audio(const AnboxAudioSpec& audio_spec);
  void process_audio_data();
  int flush_encoder();
  void close_audio_processor();

  AudioBuffer audio_buffer_;
  std::atomic_bool finished_{false};
  std::thread process_thread_;
  std::unique_ptr<Context> context_{nullptr};
};

AudioStreamingPlatformAudioProcessor::AudioStreamingPlatformAudioProcessor(
   const AnboxAudioSpec& audio_spec) {
  // Launch the audio processing thread once audio output is configured.
  if (configure_audio(audio_spec) == 0)
    process_thread_ = std::thread(&AudioStreamingPlatformAudioProcessor::process_audio_data, this);
  else
    std::cerr << "Failed to create audio processor " << strerror(errno) << std::endl;
}


AudioStreamingPlatformAudioProcessor::~AudioStreamingPlatformAudioProcessor() {
  finished_.store(true);
  if (process_thread_.joinable())
    process_thread_.join();

  close_audio_processor();
  if (!context_)
    context_.reset();
}

size_t AudioStreamingPlatformAudioProcessor::process_data(const uint8_t* data, size_t size) {
  (void) data;
  (void) size;
  return 0;
}

ssize_t AudioStreamingPlatformAudioProcessor::write_data(const uint8_t* data, size_t size) {
  if (!data || size == 0)
    return -EIO;

  // Sleep for a while if the audio buffer is full
  // and let the audio process thread do its work.
  while ((audio_buffer_.size + size) >= max_audio_buffer_size)
      std::this_thread::sleep_for(std::chrono::seconds(1));

  std::unique_lock<std::mutex> lock(audio_buffer_.mutex);
  memcpy(audio_buffer_.data + audio_buffer_.size,  data, size);
  audio_buffer_.size += size;

  return size;
}

ssize_t AudioStreamingPlatformAudioProcessor::read_data(uint8_t* data, size_t size) {
  if (!data || size == 0)
    return -EIO;

  // Generate a simple sine wave
  for (size_t i = 0; i < size; i++)
    data[i] = static_cast<uint8_t>(round(UCHAR_MAX / 2 + UCHAR_MAX * sin(2*M_PI*i)));

  return size;
}

int AudioStreamingPlatformAudioProcessor::configure_audio(const AnboxAudioSpec& audio_spec) {
  // Register all codecs and formats.
  av_register_all();

  // Initialize the network components.
  avformat_network_init();

  int channel_layout  = 0;
  AVSampleFormat sample_fmt = AV_SAMPLE_FMT_NONE;
  auto codec = avcodec_find_encoder(AV_CODEC_ID_MP3);
  if (!codec) {
    std::cerr << "Failed to find the encoder." << std::endl;
    return -ENOENT;
  }

  sample_fmt = audio_format_to_av_sample_format(audio_spec.format);
  if (sample_fmt == AV_SAMPLE_FMT_NONE) {
    std::cerr << "Unsupported audio format: " << audio_format_to_string(audio_spec.format) << std::endl;
    return -ENOENT;
  }

  channel_layout = audio_channel_to_av_channel_format(audio_spec.channels);
  if (channel_layout == -EINVAL) {
    std::cerr << "Unsupported audio channel: " << audio_spec.channels << std::endl;
    return -ENOENT;
  }

  auto codec_context = avcodec_alloc_context3(codec);
  if (!codec_context) {
    std::cerr << "Failed to allocate codec context." << std::endl;
    return -ENOMEM;
  }

  codec_context->bit_rate = 64000;
  codec_context->sample_rate = audio_spec.freq;
  codec_context->sample_fmt = sample_fmt;
  codec_context->channel_layout = channel_layout;
  codec_context->channels = av_get_channel_layout_nb_channels(codec_context->channel_layout);
  codec_context->codec_type = AVMEDIA_TYPE_AUDIO;
  if (avcodec_open2(codec_context, codec, nullptr) < 0) {
    std::cerr << "Failed to open codec context." << std::endl;
    avcodec_close(codec_context);
    return -EIO;
  }

  AVFormatContext* format_context = nullptr;
  avformat_alloc_output_context2(&format_context, nullptr, "rtp", output_url);
  if (!format_context) {
    std::cerr << "Failed to allocate output context." << output_url << std::endl;
    avcodec_close(codec_context);
    return -ENOMEM;
  }

  auto stream = avformat_new_stream(format_context, codec);
  if (!stream){
    std::cerr << "Failed to create new stream" << std::endl;
    avcodec_close(codec_context);
    avformat_free_context(format_context);
    return -EIO;
  }

  // Copy codec context to the output stream.
  avcodec_parameters_from_context(stream->codecpar, codec_context);

  // We setup a local RTP audio streaming at "rtp://127.0.0.1:37777";
  // When the audio streaming is running, people could open the stream
  // by running `ffplay rtp://127.0.0.1:37777`.
  if (!(format_context->oformat->flags & AVFMT_NOFILE)) {
    if (avio_open(&format_context->pb, output_url, AVIO_FLAG_WRITE) < 0) {
      std::cerr << "Failed to setup RTP live streaming at: " << output_url << std::endl;
      avcodec_close(codec_context);
      avformat_free_context(format_context);
      return -EIO;
    }
  }

  AVDictionary* metadata = nullptr;
  av_dict_set(&metadata, "anbox-platform", "platform-audio-streaming", 0);
  format_context->metadata = metadata;
  auto ret = avformat_write_header(format_context, nullptr);
  if (ret < 0) {
    std::cerr << "Failed to write header" << std::endl;
    avcodec_close(codec_context);
    avformat_free_context(format_context);
    return -EIO;
  }

  auto frame = av_frame_alloc();
  if (!frame) {
    std::cerr << "Failed to allocate frame." << std::endl;
    avcodec_close(codec_context);
    avformat_free_context(format_context);
    return -ENOMEM;
  }
  frame->nb_samples = codec_context->frame_size;
  frame->format = codec_context->sample_fmt;
  frame->channels = codec_context->channels;

  auto size = av_samples_get_buffer_size(nullptr,
                                         frame->channels,
                                         frame->nb_samples,
                                         static_cast<AVSampleFormat>(frame->format),
                                         0);
  auto frame_buffer = reinterpret_cast<uint8_t *>(av_malloc(size));
  ret = avcodec_fill_audio_frame(frame,
                                 frame->channels,
                                 static_cast<AVSampleFormat>(frame->format),
                                 reinterpret_cast<const uint8_t*>(frame_buffer),
                                 size,
                                 0);
  if (ret < 0) {
    std::cerr << "Failed to fill audio frame." << std::endl;
    avcodec_close(codec_context);
    avformat_free_context(format_context);
    av_free(frame);
    av_free(frame_buffer);
    return -ENOMEM;
  }

  // Keep the critical data as the private members in audio processor for resources release on close.
  context_ = std::make_unique<Context>();
  context_->format_context = format_context;
  context_->frame = frame;
  context_->stream = stream;
  context_->frame_buffer = frame_buffer;
  context_->frame_buffer_size = size;
  context_->codec_context = codec_context;

  av_new_packet(&context_->pkt, size);

  return 0;
}

void AudioStreamingPlatformAudioProcessor::process_audio_data() {
  if (!context_)
    return;

  const auto frame_buffer_size = context_->frame_buffer_size;
  auto format_context = context_->format_context;
  auto frame = context_->frame;
  int encoded{0};
  while (!finished_) {
    std::unique_lock<std::mutex> lock(audio_buffer_.mutex);
    if (audio_buffer_.size > frame_buffer_size) {
      memcpy(context_->frame_buffer, audio_buffer_.data, frame_buffer_size);
      frame->data[0] = context_->frame_buffer;
      frame->pts = (context_->pts_index++) * frame_duration;

      auto ret = audio_encode(context_->codec_context, &context_->pkt, frame, &encoded);
      if(ret < 0)
        continue;

      if (encoded==1){
        context_->pkt.stream_index = context_->stream->index;
        av_interleaved_write_frame(format_context, &context_->pkt);
        av_packet_unref(&context_->pkt);
      }

      audio_buffer_.size -= frame_buffer_size;
      memmove(audio_buffer_.data, audio_buffer_.data + frame_buffer_size, audio_buffer_.size);
    }
  }
}

int AudioStreamingPlatformAudioProcessor::flush_encoder() {
  if (!context_)
    return -EINVAL;

  int ret, encoded{0};
  while(true) {
    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = nullptr;
    pkt.size = 0;
    ret = audio_encode(context_->codec_context, &pkt, nullptr, &encoded);
    if (ret < 0)
      break;
    if (!encoded) {
      ret = 0;
      break;
    }
    ret = av_interleaved_write_frame(context_->format_context, &pkt);
    if (ret < 0)
      break;
  }
  return ret;
}

void AudioStreamingPlatformAudioProcessor::close_audio_processor() {
  if (!context_)
    return;

  if (flush_encoder() < 0)
    std::cerr << "Failed to flush audio encoder." << std::endl;

  auto format_context = context_->format_context;
  if (!format_context)
    return;

  av_write_trailer(format_context);

  // Release resources.
  auto stream = context_->stream;
  if (stream) {
    avcodec_close(context_->codec_context);
    av_free(context_->frame);
    av_free(context_->frame_buffer);
  }

  avio_close(format_context->pb);
  avformat_free_context(format_context);
}

class AudioStreamingPlatformInputProcessor : public InputProcessor {
  public:
    AudioStreamingPlatformInputProcessor() {}
    ~AudioStreamingPlatformInputProcessor() override = default;

    int read_event(AnboxInputEvent* event, int timeout) override;
    int inject_event(AnboxInputEvent event) override;
  private:
    std::queue<AnboxInputEvent> event_queue_;
    std::mutex mutex_;
};

int AudioStreamingPlatformInputProcessor::inject_event(AnboxInputEvent event) {
  std::unique_lock<std::mutex> lock(mutex_);
  event_queue_.push(event);

  return 0;
}

int AudioStreamingPlatformInputProcessor::read_event(AnboxInputEvent* event, int timeout) {
  // Create a event loop thread with std::async asynchronous function.
  // The event loop thread handle the input messages from the queue.
  // The execution is blocked at future::get until
  // 1. The next event becomes available..
  // 2. A default AnboxInputEvent instance is returned when timeout is triggered
  //    and the event queue is empty at the same time.
  std::atomic_bool finished{false};
  if (timeout == 0)
    finished.store(true);
  auto fut = std::async(
    std::launch::async, [&]() {
      do {
        std::lock_guard<std::mutex> lock(mutex_);
        if (event_queue_.size() > 0){
          auto ev = event_queue_.front();
          event_queue_.pop();
          return ev;
        }
      } while(!finished);
      return AnboxInputEvent{KEYBOARD, 0, EV_MAX, 0, 0};
    });

  if (timeout > 0 && fut.wait_for(chrono::milliseconds(timeout)) == std::future_status::timeout)
    finished.store(true);

  auto new_event = fut.get();
  if (new_event.type >= EV_SYN && new_event.type < EV_MAX) {
    event->device_type = new_event.device_type;
    event->device_id = new_event.device_id;
    event->type = new_event.type;
    event->code = new_event.code;
    event->value = new_event.value;
    return 0;
  }

  return -EIO;
}

class AudioStreamingPlatformGraphicsProcessor : public GraphicsProcessor {
 public:
  AudioStreamingPlatformGraphicsProcessor() {}
  ~AudioStreamingPlatformGraphicsProcessor() override = default;

  int initialize(AnboxGraphicsConfiguration* configuration) override;
  void begin_frame() override;
  void finish_frame() override;
};

int AudioStreamingPlatformGraphicsProcessor::initialize(AnboxGraphicsConfiguration* configuration) {
  (void) configuration;
  return 0;
}

void AudioStreamingPlatformGraphicsProcessor::begin_frame() {}

void AudioStreamingPlatformGraphicsProcessor::finish_frame() {}


class AudioStreamingPlatformProxy : public AnboxProxy {
 public:
  AudioStreamingPlatformProxy() {}
  ~AudioStreamingPlatformProxy() override = default;

  int send_message(const char* type, size_t type_size,
                   const char* data, size_t data_size) override;
};

int AudioStreamingPlatformProxy::send_message(
    const char* type, size_t type_size,
    const char* data, size_t data_size) {
  if (type == nullptr || type_size == 0 ||
      data == nullptr || data_size == 0)
    return -EINVAL;

  std::cout << "Send message type: " << std::string(type, type_size)
            << " data: " << std::string(data, data_size) << std::endl;
  return 0;
}

class AudioStreamingPlatform : public anbox::Platform {
 public:
  AudioStreamingPlatform(const AnboxPlatformConfiguration* configuration) :
    audio_processor_(std::make_unique<AudioStreamingPlatformAudioProcessor>(audio_out_spec_)),
    input_processor_(std::make_unique<AudioStreamingPlatformInputProcessor>()),
    graphics_processor_(std::make_unique<AudioStreamingPlatformGraphicsProcessor>()),
    anbox_proxy_(std::make_unique<AudioStreamingPlatformProxy>()) {
      (void) configuration;
    }
  ~AudioStreamingPlatform() override = default;

  AudioProcessor* audio_processor() override;
  InputProcessor* input_processor() override;
  GraphicsProcessor* graphics_processor() override;
  AnboxProxy* anbox_proxy() override;
  bool ready() const override;
  int wait_until_ready() override;
  int get_config_item(AnboxPlatformConfigurationKey key, void* data, size_t data_size) override;

 private:
  AnboxDisplaySpec display_spec_{1280, 720, 0};
  AnboxAudioSpec audio_out_spec_{44100, AUDIO_FORMAT_PCM_16_BIT, 1, 4096};
  AnboxAudioSpec audio_in_spec_{44100, AUDIO_FORMAT_PCM_16_BIT, 1, 4096};
  const std::unique_ptr<AudioStreamingPlatformAudioProcessor> audio_processor_;
  const std::unique_ptr<AudioStreamingPlatformInputProcessor> input_processor_;
  const std::unique_ptr<AudioStreamingPlatformGraphicsProcessor> graphics_processor_;
  const std::unique_ptr<AudioStreamingPlatformProxy> anbox_proxy_;
};

AudioProcessor* AudioStreamingPlatform::audio_processor() {
  return audio_processor_.get();
}

InputProcessor* AudioStreamingPlatform::input_processor() {
  return input_processor_.get();
}

GraphicsProcessor* AudioStreamingPlatform::graphics_processor() {
  return graphics_processor_.get();
}

AnboxProxy* AudioStreamingPlatform::anbox_proxy() {
  return anbox_proxy_.get();
}

bool AudioStreamingPlatform::ready() const {
  return true;
}

int AudioStreamingPlatform::wait_until_ready() {
  return 0;
}

int AudioStreamingPlatform::get_config_item(AnboxPlatformConfigurationKey key,
                                            void* data, size_t data_size) {
  if (!data)
    return -EINVAL;

  auto provide_str_value = [data, data_size](const char* value) -> int {
    const size_t value_size = strlen(value);
    if (value_size > data_size)
      return -ENOMEM;

    if (value)
      memcpy(data, reinterpret_cast<const void*>(value), value_size);

    return 0;
  };

  switch (key) {
  case EGL_DRIVER_PATH:
    return provide_str_value(egl_driver_path);
  case OPENGL_ES1_CM_DRIVER_PATH:
    return provide_str_value(opengl_es1_cm_driver_path);
  case OPENGL_ES2_DRIVER_PATH:
    return provide_str_value(opengl_es2_driver_path);
  case DISPLAY_SPEC: {
    if (data_size != sizeof(AnboxDisplaySpec))
      return -ENOMEM;

    auto spec = reinterpret_cast<AnboxDisplaySpec*>(data);
    memcpy(spec, &display_spec_, sizeof(AnboxDisplaySpec));
    break;
  }
  case AUDIO_SPEC: {
    if (data_size != sizeof(AnboxAudioSpec))
      return -ENOMEM;

    auto spec = reinterpret_cast<AnboxAudioSpec*>(data);
    memcpy(spec, &audio_out_spec_, sizeof(AnboxAudioSpec));
    break;
  }
  case AUDIO_INPUT_SPEC: {
    if (data_size != sizeof(AnboxAudioSpec))
      return -ENOMEM;

    auto spec = reinterpret_cast<AnboxAudioSpec*>(data);
    memcpy(spec, &audio_in_spec_, sizeof(AnboxAudioSpec));
    break;
  }
  default:
    return -EINVAL;
  }

  return 0;
}
} // namespace anbox

ANBOX_PLATFORM_PLUGIN_DESCRIBE(anbox::AudioStreamingPlatform, "audio_streaming", "Canonical", "An audio streaming platform plugin with libav")
