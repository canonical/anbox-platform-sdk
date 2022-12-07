/*
 * This file is part of Anbox Platform SDK
 *
 * Copyright 2022 Canonical Ltd.
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

#include <stdexcept>
#include <string.h>
#include <memory>

#ifndef SYSTEM_LIBDIR
#define SYSTEM_LIBDIR
#endif

namespace {
constexpr const char* opengl_es1_cm_driver_path = SYSTEM_LIBDIR "/libGLESv1_CM.so.1";
constexpr const char* opengl_es2_driver_path = SYSTEM_LIBDIR "/libGLESv2.so.2";
constexpr const char* egl_driver_path = SYSTEM_LIBDIR "/libEGL.so.1";
} // namespace

namespace anbox {
class NvidiaAudioProcessor : public AudioProcessor {
 public:
  NvidiaAudioProcessor() {}
  ~NvidiaAudioProcessor() override = default;

  size_t process_data(const uint8_t* data, size_t size) override;
};

size_t NvidiaAudioProcessor::process_data(const uint8_t* data, size_t size) {
  (void) data;
  (void) size;

  return 0;
}

class NvidiaInputProcessor : public InputProcessor {
  public:
    NvidiaInputProcessor() {}
    ~NvidiaInputProcessor() override = default;

    int read_event(AnboxInputEvent* event, int timeout) override;
    int inject_event(AnboxInputEvent event) override;
};

int NvidiaInputProcessor::read_event(AnboxInputEvent* event, int timeout = -1) {
  (void) event;
  (void) timeout;

  return 0;
}

int NvidiaInputProcessor::inject_event(AnboxInputEvent event) {
  (void) event;

  return 0;
}

class NvidiaGraphicsProcessor : public GraphicsProcessor {
 public:
  NvidiaGraphicsProcessor() {}
  ~NvidiaGraphicsProcessor() override = default;

  int initialize(AnboxGraphicsConfiguration* configuration) override {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
    configuration->native_display = EGL_DEFAULT_DISPLAY;
#pragma GCC diagnostic pop
    configuration->native_window = 0;
    configuration->output_flip_mode = FLIP_MODE_VERTICAL;
    // The NVIDIA GL driver supports pbuffers so lets use them
    configuration->avoid_pbuffers = false;
    configuration->texture_format = AnboxGraphicsTextureFormat::TEXTURE_FORMAT_RGBA;

    return 0;
  }

  void begin_frame() override {
  }

  void finish_frame() override {
  }
};

class NvidiaPlatform : public anbox::Platform {
 public:
  NvidiaPlatform(const AnboxPlatformConfiguration* configuration) :
    audio_processor_(std::make_unique<NvidiaAudioProcessor>()),
    input_processor_(std::make_unique<NvidiaInputProcessor>()),
    graphics_processor_(std::make_unique<NvidiaGraphicsProcessor>()) {
      (void) configuration;
    }
  ~NvidiaPlatform() override = default;


  AudioProcessor* audio_processor() override;
  InputProcessor* input_processor() override;
  GraphicsProcessor* graphics_processor() override;
  bool ready() const override;
  int wait_until_ready() override;
  int get_config_item(AnboxPlatformConfigurationKey key, void* data, size_t data_size) override;

 private:
  AnboxDisplaySpec2 display_spec_ = {1280, 720, 160, 60};
  AnboxAudioSpec audio_spec_ = {48000, AUDIO_FORMAT_PCM_16_BIT, 2, 4096};
  const std::unique_ptr<NvidiaAudioProcessor> audio_processor_;
  const std::unique_ptr<NvidiaInputProcessor> input_processor_;
  const std::unique_ptr<NvidiaGraphicsProcessor> graphics_processor_;
};

AudioProcessor* NvidiaPlatform::audio_processor() {
  return audio_processor_.get();
}

InputProcessor* NvidiaPlatform::input_processor() {
  return input_processor_.get();
}

GraphicsProcessor* NvidiaPlatform::graphics_processor() {
  return graphics_processor_.get();
}

bool NvidiaPlatform::ready() const {
  return true;
}

int NvidiaPlatform::wait_until_ready() {
  return 0;
}

int NvidiaPlatform::get_config_item(AnboxPlatformConfigurationKey key, void* data, size_t data_size) {
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
  case DISPLAY_SPEC2: {
    if (data_size != sizeof(AnboxDisplaySpec2))
      return -ENOMEM;

    auto spec = reinterpret_cast<AnboxDisplaySpec2*>(data);
    memcpy(spec, &display_spec_, sizeof(AnboxDisplaySpec2));
    break;
  }
  case AUDIO_SPEC: {
    if (data_size != sizeof(AnboxAudioSpec))
      return -ENOMEM;

    auto spec = reinterpret_cast<AnboxAudioSpec*>(data);
    memcpy(spec, &audio_spec_, sizeof(AnboxAudioSpec));
    break;
  }
  case EGL_DRIVER_PATH:
    return provide_str_value(egl_driver_path);
  case OPENGL_ES1_CM_DRIVER_PATH:
    return provide_str_value(opengl_es1_cm_driver_path);
  case OPENGL_ES2_DRIVER_PATH:
    return provide_str_value(opengl_es2_driver_path);
  case GRAPHICS_IMPLEMENTATION_TYPE: {
    AnboxGraphicsImplementationType* type = reinterpret_cast<AnboxGraphicsImplementationType*>(data);

    // For NVIDIA GPUs Anbox currently only supports host rendering
    *type = ANBOX_GRAPHICS_IMPLEMENTATION_TYPE_HOST_RENDERING;

    break;
  }
  default:
    return -EINVAL;
  }

  return 0;
}
} // namespace anbox

ANBOX_PLATFORM_PLUGIN_DESCRIBE(anbox::NvidiaPlatform, "nvidia", "Canonical", "A platform plugin for NVIDIA GPUs")
