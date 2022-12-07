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

class SensorPlatformAudioProcessor : public AudioProcessor {
 public:
  SensorPlatformAudioProcessor() {}
  ~SensorPlatformAudioProcessor() override = default;

  size_t process_data(const uint8_t* data, size_t size) override;
};

size_t SensorPlatformAudioProcessor::process_data(const uint8_t* data, size_t size) {
  (void) data;
  (void) size;

  return 0;
}

class SensorPlatformInputProcessor : public InputProcessor {
  public:
    SensorPlatformInputProcessor() {}
    ~SensorPlatformInputProcessor() override = default;

    int read_event(AnboxInputEvent* event, int timeout) override;
    int inject_event(AnboxInputEvent event) override;
};

int SensorPlatformInputProcessor::read_event(AnboxInputEvent* event, int timeout = -1) {
  (void) event;
  (void) timeout;

  return 0;
}

int SensorPlatformInputProcessor::inject_event(AnboxInputEvent event) {
  (void) event;

  return 0;
}

class SensorPlatformSensorProcessor : public SensorProcessor {
  public:
    SensorPlatformSensorProcessor() {}
    ~SensorPlatformSensorProcessor() override = default;

    AnboxSensorType supported_sensors() const override;
    int read_data(AnboxSensorData* data, int timeout) override;
    int inject_data(AnboxSensorData data) override;
  private:
    std::queue<AnboxSensorData> data_queue_;
    std::mutex mutex_;
};

AnboxSensorType SensorPlatformSensorProcessor::supported_sensors() const {
  return static_cast<AnboxSensorType>(AnboxSensorType::ACCELERATION | AnboxSensorType::TEMPERATURE);
}

int SensorPlatformSensorProcessor::inject_data(AnboxSensorData data) {
  std::unique_lock<std::mutex> lock(mutex_);
  data_queue_.push(data);
  return 0;
}

int SensorPlatformSensorProcessor::read_data(AnboxSensorData* data, int timeout) {
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
      return AnboxSensorData{AnboxSensorType::NONE};
    });

  if (timeout > 0 && fut.wait_for(chrono::milliseconds(timeout)) == std::future_status::timeout)
    finished.store(true);

  auto new_sensor_data = fut.get();
  data->sensor_type = new_sensor_data.sensor_type;
  switch (new_sensor_data.sensor_type) {
    case AnboxSensorType::ACCELERATION:
      data->acceleration.axis.x = new_sensor_data.acceleration.axis.x;
      data->acceleration.axis.y = new_sensor_data.acceleration.axis.y;
      data->acceleration.axis.z = new_sensor_data.acceleration.axis.z;
      return 0;
    case AnboxSensorType::GYROSCOPE:
      data->gyroscope.axis.x = new_sensor_data.gyroscope.axis.x;
      data->gyroscope.axis.y = new_sensor_data.gyroscope.axis.y;
      data->gyroscope.axis.z = new_sensor_data.gyroscope.axis.z;
      return 0;
    case AnboxSensorType::MAGNETOMETER:
      data->magnetic.axis.x = new_sensor_data.magnetic.axis.x;
      data->magnetic.axis.y = new_sensor_data.magnetic.axis.y;
      data->magnetic.axis.z = new_sensor_data.magnetic.axis.z;
      return 0;
    case AnboxSensorType::ORIENTATION:
      data->orientation.angle.azimuth = new_sensor_data.orientation.angle.azimuth;
      data->orientation.angle.pitch = new_sensor_data.orientation.angle.pitch;
      data->orientation.angle.roll = new_sensor_data.orientation.angle.roll;
      return 0;
    case AnboxSensorType::TEMPERATURE:
      data->temperature = new_sensor_data.temperature;
      return 0;
    case AnboxSensorType::PROXIMITY:
      data->proximity = new_sensor_data.proximity;
      return 0;
    case AnboxSensorType::LIGHT:
      data->light = new_sensor_data.light;
      return 0;
    case AnboxSensorType::PRESSURE:
      data->pressure = new_sensor_data.pressure;
      return 0;
    case AnboxSensorType::HUMIDITY:
      data->humidity = new_sensor_data.humidity;
      return 0;
    default:
      return -EIO;
  }
}

class SensorPlatform : public anbox::Platform {
 public:
  SensorPlatform(const AnboxPlatformConfiguration* configuration) :
    audio_processor_(std::make_unique<SensorPlatformAudioProcessor>()),
    input_processor_(std::make_unique<SensorPlatformInputProcessor>()),
    sensor_processor_(std::make_unique<SensorPlatformSensorProcessor>()) {
      (void) configuration;
    }
  ~SensorPlatform() override = default;

  AudioProcessor* audio_processor() override;
  InputProcessor* input_processor() override;
  SensorProcessor* sensor_processor() override;
  bool ready() const override;
  int wait_until_ready() override;
  int get_config_item(AnboxPlatformConfigurationKey key, void* data, size_t data_size) override;

 private:
  AnboxDisplaySpec display_spec_{1280, 720, 0};
  AnboxAudioSpec audio_spec_{44100, AUDIO_FORMAT_PCM_16_BIT, 1, 4096};
  const std::unique_ptr<SensorPlatformAudioProcessor> audio_processor_;
  const std::unique_ptr<SensorPlatformInputProcessor> input_processor_;
  const std::unique_ptr<SensorPlatformSensorProcessor> sensor_processor_;
};

AudioProcessor* SensorPlatform::audio_processor() {
  return audio_processor_.get();
}

InputProcessor* SensorPlatform::input_processor() {
  return input_processor_.get();
}

SensorProcessor* SensorPlatform::sensor_processor() {
  return sensor_processor_.get();
}

bool SensorPlatform::ready() const {
  return true;
}

int SensorPlatform::wait_until_ready() {
  return 0;
}

int SensorPlatform::get_config_item(AnboxPlatformConfigurationKey key,
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

ANBOX_PLATFORM_PLUGIN_DESCRIBE(anbox::SensorPlatform, "sensor", "Canonical", "A sensor platform plugin")
