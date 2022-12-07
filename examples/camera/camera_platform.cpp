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
#include <atomic>
#include <future>
#include <queue>
#include <iostream>
#include <memory>
#include <mutex>
#include <string.h>


#define RETURN_ON_ERROR(frame)          \
  do {                                  \
    if (frame.data)                     \
      free(frame.data);                 \
    return -EIO;                        \
  } while (0)

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
} // namespace

namespace anbox {

class CameraPlatformAudioProcessor : public AudioProcessor {
 public:
  CameraPlatformAudioProcessor() {}
  ~CameraPlatformAudioProcessor() override = default;

  size_t process_data(const uint8_t* data, size_t size) override;
};

size_t CameraPlatformAudioProcessor::process_data(const uint8_t* data, size_t size) {
  (void) data;
  (void) size;

  return 0;
}

class CameraPlatformInputProcessor : public InputProcessor {
  public:
    CameraPlatformInputProcessor() {}
    ~CameraPlatformInputProcessor() override = default;

    int read_event(AnboxInputEvent* event, int timeout) override;
    int inject_event(AnboxInputEvent event) override;
};

int CameraPlatformInputProcessor::read_event(AnboxInputEvent* event, int timeout = -1) {
  (void) event;
  (void) timeout;

  return 0;
}

int CameraPlatformInputProcessor::inject_event(AnboxInputEvent event) {
  (void) event;

  return 0;
}

class CameraPlatformCameraProcessor : public CameraProcessor {
  public:
    CameraPlatformCameraProcessor() {}
    ~CameraPlatformCameraProcessor() override = default;

    int get_device_specs(AnboxCameraSpec** specs, size_t *specs_len) override;
    int open_device(AnboxCameraSpec spec, AnboxCameraOrientation orientation) override;
    int close_device() override;
    int read_frame(AnboxVideoFrame* frame, int timeout) override;
    int inject_frame(AnboxVideoFrame frame) override;

  private:
    std::queue<AnboxVideoFrame> frame_queue_;
    std::mutex mutex_;
    AnboxCameraSpec select_camera_spec_{VIDEO_FRAME_FORMAT_UNKNOWN, CAMERA_FACING_MODE_REAR, 0, 0, 0};
    AnboxCameraOrientation current_camera_orientation_;
};

int CameraPlatformCameraProcessor::get_device_specs(AnboxCameraSpec** specs, size_t *specs_len) {
  const auto specs_length = 2;
  AnboxCameraSpec *camera_specs = new AnboxCameraSpec[specs_length];
  for (size_t i = 0; i < specs_length; i++) {
    camera_specs[i].format = VIDEO_FRAME_FORMAT_YUV420;
    camera_specs[i].fps = 30;
    camera_specs[i].facing_mode = i % 2 == 0 ? CAMERA_FACING_MODE_REAR
      : CAMERA_FACING_MODE_FRONT;
    camera_specs[i].width = 1280;
    camera_specs[i].height = 720;
  }
  *specs = camera_specs;
  *specs_len = specs_length;
  return 0;
}

int CameraPlatformCameraProcessor::open_device(AnboxCameraSpec spec, AnboxCameraOrientation orientation) {
  select_camera_spec_ = spec;
  current_camera_orientation_ = orientation;
  return 0;
}

int CameraPlatformCameraProcessor::close_device() {
  select_camera_spec_ = AnboxCameraSpec{VIDEO_FRAME_FORMAT_UNKNOWN, CAMERA_FACING_MODE_REAR, 0, 0, 0};
  return 0;
}

int CameraPlatformCameraProcessor::inject_frame(AnboxVideoFrame frame) {
  std::unique_lock<std::mutex> lock(mutex_);
  frame_queue_.push(frame);
  return 0;
}

int CameraPlatformCameraProcessor::read_frame(AnboxVideoFrame* frame, int timeout) {
  if (frame == NULL)
    return -EINVAL;

  if (select_camera_spec_.format == VIDEO_FRAME_FORMAT_UNKNOWN)
    return -EIO;

  std::atomic_bool finished{false};
  if (timeout == 0)
    finished.store(true);
  auto fut = std::async(
    std::launch::async, [&]() {
      do {
        std::lock_guard<std::mutex> lock(mutex_);
        if (frame_queue_.size() > 0){
          auto dt = frame_queue_.front();
          frame_queue_.pop();
          return dt;
        }
      } while(!finished);
      return AnboxVideoFrame{nullptr, 0};
    });

  if (timeout > 0 && fut.wait_for(chrono::milliseconds(timeout)) == std::future_status::timeout)
    finished.store(true);

  const auto new_frame = fut.get();
  if (new_frame.data == NULL || new_frame.size == 0)
    RETURN_ON_ERROR(new_frame);

  switch (select_camera_spec_.format) {
  case AnboxVideoColorSpaceFormat::VIDEO_FRAME_FORMAT_RGBA:
    if (new_frame.size != select_camera_spec_.width * select_camera_spec_.height * 4)
      RETURN_ON_ERROR(new_frame);
  break;
  case AnboxVideoColorSpaceFormat::VIDEO_FRAME_FORMAT_YUV420:
    if (new_frame.size !=  select_camera_spec_.width * select_camera_spec_.height * 3 / 2)
      RETURN_ON_ERROR(new_frame);
  break;
  default:
    RETURN_ON_ERROR(new_frame);
  }

  //NOTE: to avoid extra video frame copy, here we do a shadow copy
  //for the underlying video buffer. It's the caller's responsibility
  //to release the resource after using the video frame.
  frame->data = new_frame.data;
  frame->size = new_frame.size;
  return 0;
}

class CameraGraphicsProcessor : public GraphicsProcessor {
 public:
  CameraGraphicsProcessor() {}
  ~CameraGraphicsProcessor() override = default;
};

class CameraPlatform : public anbox::Platform {
 public:
  CameraPlatform(const AnboxPlatformConfiguration* configuration) :
    graphics_processor_(std::make_unique<CameraGraphicsProcessor>()),
    audio_processor_(std::make_unique<CameraPlatformAudioProcessor>()),
    input_processor_(std::make_unique<CameraPlatformInputProcessor>()),
    camera_processor_(std::make_unique<CameraPlatformCameraProcessor>()) {
      (void) configuration;
    }
  ~CameraPlatform() override = default;

  GraphicsProcessor* graphics_processor() override;
  AudioProcessor* audio_processor() override;
  InputProcessor* input_processor() override;
  CameraProcessor* camera_processor() override;
  bool ready() const override;
  int wait_until_ready() override;
  int get_config_item(AnboxPlatformConfigurationKey key, void* data, size_t data_size) override;

 private:
  AnboxDisplaySpec display_spec_{1280, 720, 0};
  AnboxAudioSpec audio_spec_{44100, AUDIO_FORMAT_PCM_16_BIT, 1, 4096};
  const std::unique_ptr<CameraGraphicsProcessor> graphics_processor_;
  const std::unique_ptr<CameraPlatformAudioProcessor> audio_processor_;
  const std::unique_ptr<CameraPlatformInputProcessor> input_processor_;
  const std::unique_ptr<CameraPlatformCameraProcessor> camera_processor_;
};

GraphicsProcessor* CameraPlatform::graphics_processor() {
  return graphics_processor_.get();
}

AudioProcessor* CameraPlatform::audio_processor() {
  return audio_processor_.get();
}

InputProcessor* CameraPlatform::input_processor() {
  return input_processor_.get();
}

CameraProcessor* CameraPlatform::camera_processor() {
  return camera_processor_.get();
}

bool CameraPlatform::ready() const {
  return true;
}

int CameraPlatform::wait_until_ready() {
  return 0;
}

int CameraPlatform::get_config_item(AnboxPlatformConfigurationKey key,
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
    memcpy(spec, &audio_spec_, sizeof(AnboxAudioSpec));
    break;
  }
  default:
    return -EINVAL;
  }

  return 0;
}
} // namespace anbox

ANBOX_PLATFORM_PLUGIN_DESCRIBE(anbox::CameraPlatform, "camera", "Canonical", "A camera platform plugin")
