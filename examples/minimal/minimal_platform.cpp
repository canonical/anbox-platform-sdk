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

#include <stdexcept>
#include <string.h>
#include <memory>

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

class MinimalPlatformAudioProcessor : public AudioProcessor {
 public:
  MinimalPlatformAudioProcessor() {}
  ~MinimalPlatformAudioProcessor() override = default;

  size_t process_data(const uint8_t* data, size_t size) override;
};

size_t MinimalPlatformAudioProcessor::process_data(const uint8_t* data, size_t size) {
  (void) data;
  (void) size;

  return 0;
}

class MinimalPlatformInputProcessor : public InputProcessor {
  public:
    MinimalPlatformInputProcessor() {}
    ~MinimalPlatformInputProcessor() override = default;

    int read_event(AnboxInputEvent* event, int timeout) override;
    int inject_event(AnboxInputEvent event) override;
};

int MinimalPlatformInputProcessor::read_event(AnboxInputEvent* event, int timeout = -1) {
  (void) event;
  (void) timeout;

  return 0;
}

int MinimalPlatformInputProcessor::inject_event(AnboxInputEvent event) {
  (void) event;

  return 0;
}

class MinimalGraphicsProcessor : public GraphicsProcessor {
 public:
  MinimalGraphicsProcessor() {}
  ~MinimalGraphicsProcessor() override = default;
};

class MinimalPlatform : public anbox::Platform {
 public:
  MinimalPlatform(const AnboxPlatformConfiguration* configuration) :
    graphics_processor_(std::make_unique<MinimalGraphicsProcessor>()),
    audio_processor_(std::make_unique<MinimalPlatformAudioProcessor>()),
    input_processor_(std::make_unique<MinimalPlatformInputProcessor>()) {
      (void) configuration;
    }
  ~MinimalPlatform() override = default;

  GraphicsProcessor* graphics_processor() override;
  AudioProcessor* audio_processor() override;
  InputProcessor* input_processor() override;
  bool ready() const override;
  int wait_until_ready() override;
  int get_config_item(AnboxPlatformConfigurationKey key, void* data, size_t data_size) override;

 private:
  AnboxDisplaySpec2 display_spec_ = {1280, 720, 160, 60};
  AnboxAudioSpec audio_spec_ = {48000, AUDIO_FORMAT_PCM_16_BIT, 2, 4096};
  const std::unique_ptr<MinimalGraphicsProcessor> graphics_processor_;
  const std::unique_ptr<MinimalPlatformAudioProcessor> audio_processor_;
  const std::unique_ptr<MinimalPlatformInputProcessor> input_processor_;
};

GraphicsProcessor* MinimalPlatform::graphics_processor() {
  return graphics_processor_.get();
}

AudioProcessor* MinimalPlatform::audio_processor() {
  return audio_processor_.get();
}

InputProcessor* MinimalPlatform::input_processor() {
  return input_processor_.get();
}

bool MinimalPlatform::ready() const {
  return true;
}

int MinimalPlatform::wait_until_ready() {
  return 0;
}

int MinimalPlatform::get_config_item(AnboxPlatformConfigurationKey key, void* data, size_t data_size) {
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
  default:
    return -EINVAL;
  }

  return 0;
}
} // namespace anbox

ANBOX_PLATFORM_PLUGIN_DESCRIBE(anbox::MinimalPlatform, "minimal", "Canonical", "A minimal platform plugin")
