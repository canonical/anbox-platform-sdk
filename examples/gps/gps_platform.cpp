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
class GpsPlatformAudioProcessor : public AudioProcessor {
 public:
  GpsPlatformAudioProcessor() {}
  ~GpsPlatformAudioProcessor() override = default;

  size_t process_data(const uint8_t* data, size_t size) override;
};

size_t GpsPlatformAudioProcessor::process_data(const uint8_t* data, size_t size) {
  (void) data;
  (void) size;

  return 0;
}

class GpsPlatformInputProcessor : public InputProcessor {
  public:
    GpsPlatformInputProcessor() {}
    ~GpsPlatformInputProcessor() override = default;

    int read_event(AnboxInputEvent* event, int timeout) override;
    int inject_event(AnboxInputEvent event) override;
};

int GpsPlatformInputProcessor::read_event(AnboxInputEvent* event, int timeout = -1) {
  (void) event;
  (void) timeout;

  return 0;
}

int GpsPlatformInputProcessor::inject_event(AnboxInputEvent event) {
  (void) event;

  return 0;
}

class GpsPlatformGpsProcessor : public GpsProcessor {
  public:
    GpsPlatformGpsProcessor() {}
    ~GpsPlatformGpsProcessor() override = default;

    int read_data(AnboxGpsData* data, int timeout) override;
    int inject_data(AnboxGpsData data) override;
  private:
    std::queue<AnboxGpsData> data_queue_;
    std::mutex mutex_;
};

int GpsPlatformGpsProcessor::inject_data(AnboxGpsData data) {
  std::unique_lock<std::mutex> lock(mutex_);
  data_queue_.push(data);
  return 0;
}

int GpsPlatformGpsProcessor::read_data(AnboxGpsData* data, int timeout) {
  std::atomic_bool finished{false};
  if (timeout == 0)
    finished.store(true);
  auto fut = std::async(
    std::launch::async, [&]() {
      do {
        std::lock_guard<std::mutex> lock(mutex_);
        if (data_queue_.size() > 0){
          auto dt = data_queue_.front();
          data_queue_.pop();
          return dt;
        }
      } while(!finished);
      return AnboxGpsData{AnboxGpsDataType::Unknown};
    });

  if (timeout > 0 && fut.wait_for(chrono::milliseconds(timeout)) == std::future_status::timeout)
    finished.store(true);

  auto new_gps_data = fut.get();
  switch (new_gps_data.data_type) {
    case AnboxGpsDataType::GGA:
    case AnboxGpsDataType::RMC:
    case AnboxGpsDataType::GNSSv1:
      memcpy(data, &new_gps_data, sizeof(AnboxGpsData));
      return 0;
    default:
      return -EIO;
  }
}

class GpsGraphicsProcessor : public GraphicsProcessor {
 public:
  GpsGraphicsProcessor() {}
  ~GpsGraphicsProcessor() override = default;
};

class GpsPlatform : public anbox::Platform {
 public:
  GpsPlatform(const AnboxPlatformConfiguration* configuration) :
    graphics_processor_(std::make_unique<GpsGraphicsProcessor>()),
    audio_processor_(std::make_unique<GpsPlatformAudioProcessor>()),
    input_processor_(std::make_unique<GpsPlatformInputProcessor>()),
    gps_processor_(std::make_unique<GpsPlatformGpsProcessor>()) {
      (void) configuration;
    }
  ~GpsPlatform() override = default;

  GraphicsProcessor* graphics_processor() override;
  AudioProcessor* audio_processor() override;
  InputProcessor* input_processor() override;
  GpsProcessor* gps_processor() override;
  bool ready() const override;
  int wait_until_ready() override;
  int get_config_item(AnboxPlatformConfigurationKey key, void* data, size_t data_size) override;

 private:
  AnboxDisplaySpec display_spec_{1280, 720, 0};
  AnboxAudioSpec audio_spec_{44100, AUDIO_FORMAT_PCM_16_BIT, 1, 4096};
  const std::unique_ptr<GpsGraphicsProcessor> graphics_processor_;
  const std::unique_ptr<GpsPlatformAudioProcessor> audio_processor_;
  const std::unique_ptr<GpsPlatformInputProcessor> input_processor_;
  const std::unique_ptr<GpsPlatformGpsProcessor> gps_processor_;
};

GraphicsProcessor* GpsPlatform::graphics_processor() {
  return graphics_processor_.get();
}

AudioProcessor* GpsPlatform::audio_processor() {
  return audio_processor_.get();
}

InputProcessor* GpsPlatform::input_processor() {
  return input_processor_.get();
}

GpsProcessor* GpsPlatform::gps_processor() {
  return gps_processor_.get();
}

bool GpsPlatform::ready() const {
  return true;
}

int GpsPlatform::wait_until_ready() {
  return 0;
}

int GpsPlatform::get_config_item(AnboxPlatformConfigurationKey key,
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

    auto spec = reinterpret_cast<AnboxDisplaySpec2*>(data);
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

ANBOX_PLATFORM_PLUGIN_DESCRIBE(anbox::GpsPlatform, "gps", "Canonical", "A gps platform plugin")
