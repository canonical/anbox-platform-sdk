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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "anbox-platform-sdk/plugin.h"
#include "anbox-platform-sdk/public_api.h"

#include <chrono>
#include <fstream>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

#include <stdint.h>
#include <dlfcn.h>
#include <gelf.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/input.h>

namespace chrono = std::chrono;

namespace {
constexpr const char* anbox_initialize_platform_name{"anbox_initialize"};
constexpr const char* anbox_deinitialize_platform_name{"anbox_deinitialize"};
constexpr const char* anbox_platform_get_audio_processor_name{"anbox_platform_get_audio_processor"};
constexpr const char* anbox_platform_get_input_processor_name{"anbox_platform_get_input_processor"};
constexpr const char* anbox_platform_get_graphics_processor_name{"anbox_platform_get_graphics_processor"};
constexpr const char* anbox_platform_get_sensor_processor_name{"anbox_platform_get_sensor_processor"};
constexpr const char* anbox_platform_get_anbox_proxy_name{"anbox_platform_get_anbox_proxy"};
constexpr const char* anbox_platform_get_gps_processor_name{"anbox_platform_get_gps_processor"};
constexpr const char* anbox_platform_get_camera_processor_name{"anbox_platform_get_camera_processor"};
constexpr const char* anbox_platform_ready_name{"anbox_platform_ready"};
constexpr const char* anbox_platform_wait_until_ready_name{"anbox_platform_wait_until_ready"};
constexpr const char* anbox_platform_get_config_item_name{"anbox_platform_get_config_item"};
constexpr const char* anbox_platform_stop_name{"anbox_platform_stop"};
constexpr const char* anbox_platform_handle_event_name{"anbox_platform_handle_event"};
constexpr const char* anbox_audio_processor_process_data_name{"anbox_audio_processor_process_data"};
constexpr const char* anbox_audio_processor_write_data_name{"anbox_audio_processor_write_data"};
constexpr const char* anbox_audio_processor_read_data_name{"anbox_audio_processor_read_data"};
constexpr const char* anbox_audio_processor_standby_name{"anbox_audio_processor_standby"};
constexpr const char* anbox_audio_processor_need_silence_on_standby_name{"anbox_audio_processor_need_silence_on_standby"};
constexpr const char* anbox_input_processor_read_event_name{"anbox_input_processor_read_event"};
constexpr const char* anbox_input_processor_inject_event_name{"anbox_input_processor_inject_event"};
constexpr const char* anbox_graphics_processor_initialize_name{"anbox_graphics_processor_initialize"};
constexpr const char* anbox_graphics_processor_begin_frame_name{"anbox_graphics_processor_begin_frame"};
constexpr const char* anbox_graphics_processor_finish_frame_name{"anbox_graphics_processor_finish_frame"};
constexpr const char* anbox_graphics_processor_create_display_name{"anbox_graphics_processor_create_display"};
constexpr const char* anbox_sensor_processor_supported_sensors_name{"anbox_sensor_processor_supported_sensors"};
constexpr const char* anbox_sensor_processor_read_data_name{"anbox_sensor_processor_read_data"};
constexpr const char* anbox_sensor_processor_inject_data_name{"anbox_sensor_processor_inject_data"};
constexpr const char* anbox_proxy_set_change_screen_orientation_callback_name{"anbox_proxy_set_change_screen_orientation_callback"};
constexpr const char* anbox_proxy_set_change_display_density_callback_name{"anbox_proxy_set_change_display_density_callback"};
constexpr const char* anbox_proxy_set_change_display_size_callback_name{"anbox_proxy_set_change_display_size_callback"};
constexpr const char* anbox_proxy_set_trigger_action_callback_name{"anbox_proxy_set_trigger_action_callback"};
constexpr const char* anbox_proxy_send_message_name{"anbox_proxy_send_message"};
constexpr const char* anbox_gps_processor_read_data_name{"anbox_gps_processor_read_data"};
constexpr const char* anbox_gps_processor_inject_data_name{"anbox_gps_processor_inject_data"};
constexpr const char* anbox_camera_processor_get_device_specs_name{"anbox_camera_processor_get_device_specs"};
constexpr const char* anbox_camera_processor_open_device_name{"anbox_camera_processor_open_device"};
constexpr const char* anbox_camera_processor_close_device_name{"anbox_camera_processor_close_device"};
constexpr const char* anbox_camera_processor_read_frame_name{"anbox_camera_processor_read_frame"};
constexpr const char* anbox_camera_processor_inject_frame_name{"anbox_camera_processor_inject_frame"};

constexpr const int timeout_in_secs{5};
constexpr const int event_numbers{1000};
constexpr const int event_value_numbers{100};
constexpr const int big_chunk_size{4096};
constexpr const int small_chunk_size{4096};
constexpr const int sensor_data_numbers{1000};
constexpr const int gps_data_numbers{1000};
constexpr const int gnss_constellation_type_count{6};
constexpr const int gnss_measurement_state_count{16};
constexpr const int gnss_clocks_count{6};
constexpr const int supported_sensor_count{8};
constexpr const int supported_gps_data_count{3};
constexpr const int audio_buffer_length{1024};
constexpr const int video_frame_count{100};
constexpr const uint32_t android_minimum_density{72};

static void print_usage() {
  std::cerr << "Usage: anbox-platform-tester [GTEST options] <path to platform .so>" << std::endl;
}

static const char* platform_path{nullptr};

template <typename type>
type generate_random_number(type min, type max) {
  return min + static_cast<type>(rand()) / (static_cast<type>(RAND_MAX / (max - min)));
}

bool is_valid_sensor_type(AnboxSensorType sensor_type) {
  switch (sensor_type) {
    case AnboxSensorType::ACCELERATION:
    case AnboxSensorType::GYROSCOPE:
    case AnboxSensorType::MAGNETOMETER:
    case AnboxSensorType::ORIENTATION:
    case AnboxSensorType::TEMPERATURE:
    case AnboxSensorType::PROXIMITY:
    case AnboxSensorType::LIGHT:
    case AnboxSensorType::PRESSURE:
    case AnboxSensorType::HUMIDITY:
      return true;
    default:
      return false;
  }
}

bool is_valid_gps_data_type(AnboxGpsDataType gps_type) {
  switch (gps_type) {
    case AnboxGpsDataType::GGA:
    case AnboxGpsDataType::RMC:
    case AnboxGpsDataType::GNSSv1:
      return true;
    default:
      return false;
  }
}

int rmc_current_date() {
  time_t rawtime;
  struct tm* timeinfo;
  std::string s(7, '\0');
  time (&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(&s[0], 7,"%g%m%d",timeinfo);
  return std::stoi(s);
}

long current_datetime_in_ns() {
  auto now = std::chrono::system_clock::now();
  auto now_ns = std::chrono::time_point_cast<std::chrono::nanoseconds>(now).time_since_epoch();
  return now_ns.count();
}

bool is_multiple_axis_sensor(AnboxSensorType sensor_type) {
  switch (sensor_type) {
    case AnboxSensorType::ACCELERATION:
    case AnboxSensorType::GYROSCOPE:
    case AnboxSensorType::MAGNETOMETER:
    case AnboxSensorType::ORIENTATION:
      return true;
    default:
      return false;
  }
}

class SensorDataGenerator {
 public:
   SensorDataGenerator() = default;
   ~SensorDataGenerator() = default;

   int generate(AnboxSensorData* sensor_data, float min = -1.0, float max = 1.0) {
     const auto enum_index = generate_random_number<int>(0, supported_sensor_count);
     const auto sensor_type = static_cast<AnboxSensorType>(1 << enum_index);
     if (!is_valid_sensor_type(sensor_type))
      return -1;

     sensor_data->sensor_type = sensor_type;
     const auto multiple_axis_sensor = is_multiple_axis_sensor(sensor_type);
     if (multiple_axis_sensor) {
       sensor_data->values[0] = generate_random_number<float>(min, max);
       sensor_data->values[1] = generate_random_number<float>(min, max);
       sensor_data->values[2] = generate_random_number<float>(min, max);
     } else
       sensor_data->values[0] = generate_random_number<float>(min, max);

     return 0;
   }
};

class GpsDataGenerator {
 public:
   GpsDataGenerator() = default;
   ~GpsDataGenerator() = default;

   int generate(AnboxGpsData* gps_data) {
     const auto enum_index = generate_random_number<int>(1, supported_gps_data_count + 1);
     const auto data_type = static_cast<AnboxGpsDataType>(1 << enum_index);
     if (!is_valid_gps_data_type(data_type))
      return -1;

     gps_data->data_type = data_type;
     switch (data_type) {
       case AnboxGpsDataType::GGA:
         gps_data->data_type = data_type;
         gps_data->gga_data.time = std::time(0) * 1000; // timestamp in milliseconds
         gps_data->gga_data.latitude = generate_random_number<double>(-90.0, 90.0);
         gps_data->gga_data.latitudeHemi = generate_random_number<int>(0, 2) == 0 ? 'N':'S';
         gps_data->gga_data.longitude = generate_random_number<double>(-90.0, 90.0);
         gps_data->gga_data.longitudeHemi = generate_random_number<int>(0, 2) == 0 ? 'E':'W';
         gps_data->gga_data.altitude = generate_random_number<double>(-100.0, 1000.0);
         gps_data->gga_data.altitudeUnit = 'M';
         gps_data->gga_data.horizontalAccuracy = generate_random_number<float>(0, 10.0);
         gps_data->gga_data.verticalAccuracy = generate_random_number<float>(0, 10.0);
         break;
       case AnboxGpsDataType::RMC:
         gps_data->rmc_data.time =  std::time(0) * 1000;
         gps_data->rmc_data.status = generate_random_number<int>(0, 2) == 0 ? 'A':'V';
         gps_data->rmc_data.latitude = generate_random_number<double>(-90.0, 90.0);
         gps_data->rmc_data.latitudeHemi =  generate_random_number<int>(0, 2) == 0 ? 'N':'S';
         gps_data->rmc_data.longitude = generate_random_number<double>(-90.0, 90.0);
         gps_data->rmc_data.longitudeHemi =  generate_random_number<int>(0, 2) == 0 ? 'E':'W';
         gps_data->rmc_data.speed = generate_random_number<float>(0, 100.0);
         gps_data->rmc_data.bearing = generate_random_number<float>(-90, 90.0);
         gps_data->rmc_data.date = rmc_current_date();
         gps_data->rmc_data.horizontalAccuracy = generate_random_number<float>(0, 10.0);
         gps_data->rmc_data.verticalAccuracy = generate_random_number<float>(0, 10.0);
         break;
       case AnboxGpsDataType::GNSSv1:
         gps_data->gnss_data.measurement_count = generate_random_number<int>(0, 10);
         gps_data->gnss_data.clock.time_ns = current_datetime_in_ns();
         gps_data->gnss_data.clock.full_bias_ns = current_datetime_in_ns();
         gps_data->gnss_data.clock.bias_ns = current_datetime_in_ns();
         gps_data->gnss_data.clock.bias_uncertainty_ns = current_datetime_in_ns();
         gps_data->gnss_data.clock.drift_nsps = generate_random_number<float>(0.0, 10.0);
         gps_data->gnss_data.clock.drift_uncertainty_nsps = generate_random_number<float>(0.0, 10.0);
         gps_data->gnss_data.clock.hw_clock_discontinuity_count = generate_random_number<int>(0, 10);
         gps_data->gnss_data.clock.flags = static_cast<GnssClockFlags>(
                               generate_random_number<int>(0, gnss_clocks_count));

         for (int i = 0; i < gps_data->gnss_data.measurement_count; i++) {
           gps_data->gnss_data.measurements[i].svid = generate_random_number<int>(0, 1000);
           gps_data->gnss_data.measurements[i].constellation = static_cast<GnssConstellationType>(
                               generate_random_number<int>(1, gnss_constellation_type_count + 1));
           gps_data->gnss_data.measurements[i].state = static_cast<GnssMeasurementState>(
                               generate_random_number<int>(1, gnss_measurement_state_count + 1));
           gps_data->gnss_data.measurements[i].received_sv_time_in_ns = current_datetime_in_ns();
           gps_data->gnss_data.measurements[i].received_sv_time_uncertainty_in_ns = current_datetime_in_ns();
           gps_data->gnss_data.measurements[i].c_n0_dbhz = generate_random_number<float>(0.0, 100.0);
           gps_data->gnss_data.measurements[i].pseudorange_rate_mps =
                               generate_random_number<float>(0.0, 10.0);
           gps_data->gnss_data.measurements[i].pseudorange_rate_uncertainty_mps =
                               generate_random_number<float>(0.0, 10.0);
           gps_data->gnss_data.measurements[i].carrier_frequency_hz =
                               generate_random_number<float>(0.0, 10.0);
         }
         break;
       default:
         return -1;
     }

     return 0;
   }
};

class RandomDataGenerator {
 public:
   RandomDataGenerator() {
     // Read "/dev/urandom" to generate random data.
     urnd_fd_ = open("/dev/urandom", O_RDONLY);
   }

   ~RandomDataGenerator() {
     if (urnd_fd_ >= 0)
       close(urnd_fd_);
   }

   size_t generate(uint8_t* data, size_t size) {
     auto bytes_read = read(urnd_fd_, data, size);
     if (bytes_read < 0)
       return 0;
     return bytes_read;
   }
 private:
  int urnd_fd_ = -1;
};

class BasePlatformTest : public ::testing::Test {
 public:
  void SetUp() override {
    handle_ = dlopen(platform_path, RTLD_NOW);
    ASSERT_NE(nullptr, handle_);
    elf_fd_ = open(platform_path, O_RDONLY);
    ASSERT_NE(-1, elf_fd_);
  }

  void TearDown() override {
    if (handle_)
      dlclose(handle_);
    handle_ = nullptr;
    if (elf_fd_  != -1)
      close(elf_fd_);
    elf_fd_ = -1;
  }

 protected:
  template <typename function>
  function export_symbol(const char* symbol_name) {
    return reinterpret_cast<function>(dlsym(handle_, symbol_name));
  }

  void load_descriptor(AnboxPlatformDescriptor** descriptor) {
    // ASSERT_xx can only be used in functions that return void.
    // The descriptor is crucial for anbox to detect and verify
    // an external platform plugin.
    // So we give the descriptor pointer back via a parameter.
    auto version = elf_version(EV_CURRENT);
    ASSERT_NE(EV_NONE, version);

    auto elf = elf_begin(elf_fd_, ELF_C_READ, nullptr);
    ASSERT_NE(nullptr, elf);

    auto kind = elf_kind(elf);
    ASSERT_EQ(kind, ELF_K_ELF);

    size_t shstrndx;
    auto ret = elf_getshdrstrndx(elf, &shstrndx);
    ASSERT_EQ(0, ret);

    GElf_Shdr shdr;
    char* name = nullptr;
    Elf_Data* data = nullptr;
    Elf_Scn* scn = nullptr;
    while ((scn = elf_nextscn(elf, scn)) != nullptr) {
      auto elf_shadr = gelf_getshdr(scn, &shdr);
      ASSERT_EQ(&shdr, elf_shadr);

      name = elf_strptr(elf, shstrndx, shdr.sh_name);
      ASSERT_NE(nullptr, name);

      if (!strcmp(name, ANBOX_PLATFORM_DESCRIPTOR_SECTION) &&
          (data = elf_getdata(scn, data)))
        *descriptor = reinterpret_cast<AnboxPlatformDescriptor*>(data->d_buf);
    }
  }

 private:
  void* handle_ = nullptr;
  int elf_fd_ = -1;
};

class VideoFrameGenerator {
 public:
   VideoFrameGenerator() = default;
   ~VideoFrameGenerator() = default;

   int generate(AnboxVideoFrame& frame, uint32_t width, uint32_t height,
       const AnboxVideoColorSpaceFormat& format) {
     memset(&frame, 0, sizeof(AnboxVideoFrame));
     switch(format) {
     case VIDEO_FRAME_FORMAT_YUV420: {
       uint32_t num_of_pixels = width * height;
       uint32_t num_of_yuv = num_of_pixels / 4;
       frame.size = num_of_pixels * 3 / 2;
       frame.data = reinterpret_cast<uint8_t*>(malloc(frame.size));

       // Fill the frame with a white color
       const uint8_t rgb[3] = {0xff, 0xff, 0xff};
       uint8_t yuv[3];
       rgb_to_yuv(rgb, yuv);

       uint8_t* y = frame.data;
       uint8_t* u = y + num_of_pixels;
       uint8_t* v = u + num_of_yuv;
       ::memset(y, yuv[0], num_of_pixels);
       ::memset(u, yuv[1], num_of_yuv);
       ::memset(v, yuv[2], num_of_yuv);
     }
     break;
     case VIDEO_FRAME_FORMAT_RGBA: {
       frame.size = width * height * 4;
       frame.data = reinterpret_cast<uint8_t*>(malloc(frame.size));

       // Fill the frame with a white color
       ::memset(frame.data, 0xff, frame.size);
     }
     break;
     default:
     return -1;
     }

     return 0;
   }
 private:
  void rgb_to_yuv(const uint8_t* rgb, uint8_t* yuv) {
    const auto r = rgb[0];
    const auto g = rgb[1];
    const auto b = rgb[2];
    yuv[0] = ((66 * r + 129 * g + 25 * b + 128) >> 8) + 16;
    yuv[1] = ((-38 * r - 74 * g + 112 * b + 128) >> 8) + 128;
    yuv[2] = ((112 * r - 94 * g - 18 * b + 128) >> 8) + 128;
  }
};

class PlatformBehaviorTest : public BasePlatformTest {
 public:
  void SetUp() override {
    BasePlatformTest::SetUp();

    create_platform = export_symbol<AnboxInitializePlatformFunc>(
                anbox_initialize_platform_name);
    ASSERT_NE(nullptr, create_platform);
    release_platform = export_symbol<AnboxReleasePlatformFunc>(
                anbox_deinitialize_platform_name);
    ASSERT_NE(nullptr, release_platform);
    get_audio_processor = export_symbol<AnboxPlatformGetAudioProcessorFunc>(
                anbox_platform_get_audio_processor_name);
    ASSERT_NE(nullptr, get_audio_processor);
    get_input_processor = export_symbol<AnboxPlatformGetInputProcessorFunc>(
                anbox_platform_get_input_processor_name);
    ASSERT_NE(nullptr, get_input_processor);
    get_config_item = export_symbol<AnboxPlatformGetConfigItemFunc>(
                anbox_platform_get_config_item_name);
    ASSERT_NE(nullptr, get_config_item);
    ready = export_symbol<AnboxPlatformReadyFunc>(
                anbox_platform_ready_name);
    ASSERT_NE(nullptr, ready);
    wait_until_ready = export_symbol<AnboxPlatformWaitUntilReadyFunc>(
                anbox_platform_wait_until_ready_name);
    ASSERT_NE(nullptr, wait_until_ready);
    get_proxy = export_symbol<AnboxPlatformGetAnboxProxyFunc>(
                anbox_platform_get_anbox_proxy_name);
    ASSERT_NE(nullptr, get_proxy);
    stop = export_symbol<AnboxPlatformStopFunc>(
                anbox_platform_stop_name);
    ASSERT_NE(nullptr, stop);
    handle_event = export_symbol<AnboxPlatformHandleEventFunc>(
                anbox_platform_handle_event_name);
    ASSERT_NE(nullptr, handle_event);

    load_descriptor(&descriptor);
    ASSERT_NE(nullptr, descriptor);
  }

 protected:
  AnboxInitializePlatformFunc create_platform{nullptr};
  AnboxReleasePlatformFunc release_platform{nullptr};
  AnboxPlatformReadyFunc ready{nullptr};
  AnboxPlatformWaitUntilReadyFunc wait_until_ready{nullptr};
  AnboxPlatformGetConfigItemFunc get_config_item{nullptr};
  AnboxPlatformGetAudioProcessorFunc get_audio_processor{nullptr};
  AnboxPlatformGetInputProcessorFunc get_input_processor{nullptr};
  AnboxPlatformGetGraphicsProcessorFunc get_graphics_processor{nullptr};
  AnboxPlatformGetSensorProcessorFunc get_sensor_processor{nullptr};
  AnboxPlatformGetGpsProcessorFunc get_gps_processor{nullptr};
  AnboxPlatformGetCameraProcessorFunc get_camera_processor{nullptr};
  AnboxPlatformGetAnboxProxyFunc get_proxy{nullptr};
  AnboxPlatformStopFunc stop{nullptr};
  AnboxPlatformHandleEventFunc handle_event{nullptr};

  AnboxPlatformDescriptor* descriptor{nullptr};
};

class PlatformInputProcessorTest : public PlatformBehaviorTest {
 public:
  void SetUp() override {
    PlatformBehaviorTest::SetUp();

    platform = create_platform(nullptr);
    ASSERT_NE(nullptr, platform);
    if (ready(platform) == false)
      ASSERT_EQ(0, wait_until_ready(platform));

    input_processor_read_event = export_symbol<AnboxInputProcessorReadEventFunc>(
                anbox_input_processor_read_event_name);
    ASSERT_NE(nullptr, input_processor_read_event);
    input_processor_inject_event = export_symbol<AnboxInputProcessorInjectEventFunc>(
                anbox_input_processor_inject_event_name);
    ASSERT_NE(nullptr, input_processor_inject_event);
  }

  void TearDown() override {
    if (platform)
      release_platform(platform) ;
    PlatformBehaviorTest::TearDown();
  }

 protected:
  AnboxPlatform* platform{nullptr};
  AnboxInputProcessorReadEventFunc input_processor_read_event{nullptr};
  AnboxInputProcessorInjectEventFunc input_processor_inject_event{nullptr};
};

class PlatformAudioProcessorTest : public PlatformBehaviorTest {
public:
 void SetUp() override {
   PlatformBehaviorTest::SetUp();

   platform = create_platform(nullptr);
   ASSERT_NE(nullptr, platform);
   if (ready(platform) == false)
     ASSERT_EQ(0, wait_until_ready(platform));

   audio_processor_process_data = export_symbol<AnboxAudioProcessorProcessDataFunc>(
               anbox_audio_processor_process_data_name);
   ASSERT_NE(nullptr, audio_processor_process_data);

   audio_processor_write_data = export_symbol<AnboxAudioProcessorWriteDataFunc>(
               anbox_audio_processor_write_data_name);
   ASSERT_NE(nullptr, audio_processor_write_data);

   audio_processor_read_data = export_symbol<AnboxAudioProcessorReadDataFunc>(
               anbox_audio_processor_read_data_name);
   ASSERT_NE(nullptr, audio_processor_read_data);

   audio_processor_standby = export_symbol<AnboxAudioProcessorStandbyFunc>(
               anbox_audio_processor_standby_name);
   ASSERT_NE(nullptr, audio_processor_standby);

   audio_processor_need_silence_on_standby = export_symbol<AnboxAudioProcessorNeedSilenceOnStandbyFunc>(
               anbox_audio_processor_need_silence_on_standby_name);
   ASSERT_NE(nullptr, audio_processor_need_silence_on_standby);
 }

 void TearDown() override {
   if (platform)
     release_platform(platform);
   PlatformBehaviorTest::TearDown();
 }
 protected:
  AnboxPlatform* platform{nullptr};
  AnboxAudioProcessorProcessDataFunc audio_processor_process_data{nullptr};
  AnboxAudioProcessorWriteDataFunc audio_processor_write_data{nullptr};
  AnboxAudioProcessorReadDataFunc audio_processor_read_data{nullptr};
  AnboxAudioProcessorStandbyFunc audio_processor_standby{nullptr};
  AnboxAudioProcessorNeedSilenceOnStandbyFunc audio_processor_need_silence_on_standby{nullptr};
};

class PlatformGraphicsProcessorTest : public PlatformBehaviorTest {
public:
 void SetUp() override {
   PlatformBehaviorTest::SetUp();

   platform = create_platform(nullptr);
   ASSERT_NE(nullptr, platform);
   if (ready(platform) == false)
     ASSERT_EQ(0, wait_until_ready(platform));

   get_graphics_processor = export_symbol<AnboxPlatformGetGraphicsProcessorFunc>(
                  anbox_platform_get_graphics_processor_name);
   ASSERT_NE(nullptr, get_graphics_processor);

   graphics_processor_initialize = export_symbol<AnboxGraphicsProcessorInitializeFunc>(
               anbox_graphics_processor_initialize_name);
   ASSERT_NE(nullptr, graphics_processor_initialize);

   graphics_processor_begin_frame = export_symbol<AnboxGraphicsProcessorBeginFrameFunc>(
               anbox_graphics_processor_begin_frame_name);
   ASSERT_NE(nullptr, graphics_processor_begin_frame);

   graphics_processor_finish_frame = export_symbol<AnboxGraphicsProcessorFinishFrameFunc>(
               anbox_graphics_processor_finish_frame_name);
   ASSERT_NE(nullptr, graphics_processor_finish_frame);

  graphics_processor_create_display = export_symbol<AnboxGraphicsProcessorCreateDisplayFunc>(
               anbox_graphics_processor_create_display_name);
   ASSERT_NE(nullptr, graphics_processor_create_display);
 }

 void TearDown() override {
   if (platform)
     release_platform(platform);
   PlatformBehaviorTest::TearDown();
 }
 protected:
  AnboxPlatform* platform{nullptr};
  AnboxGraphicsProcessorInitializeFunc graphics_processor_initialize{nullptr};
  AnboxGraphicsProcessorBeginFrameFunc graphics_processor_begin_frame{nullptr};
  AnboxGraphicsProcessorFinishFrameFunc graphics_processor_finish_frame{nullptr};
  AnboxGraphicsProcessorCreateDisplayFunc graphics_processor_create_display{nullptr};
};

class PlatformSensorProcessorTest : public PlatformBehaviorTest {
 public:
  void SetUp() override {
    PlatformBehaviorTest::SetUp();

    platform = create_platform(nullptr);
    ASSERT_NE(nullptr, platform);
    if (ready(platform) == false)
      ASSERT_EQ(0, wait_until_ready(platform));

    get_sensor_processor = export_symbol<AnboxPlatformGetSensorProcessorFunc>(
		    anbox_platform_get_sensor_processor_name);
    ASSERT_NE(nullptr, get_sensor_processor);

    sensor_processor_supported_sensors = export_symbol<AnboxSensorProcessorSupportedSensorsFunc>(
                   anbox_sensor_processor_supported_sensors_name);
    ASSERT_NE(nullptr, sensor_processor_supported_sensors);
    sensor_processor_read_data = export_symbol<AnboxSensorProcessorReadDataFunc>(
                   anbox_sensor_processor_read_data_name);
    ASSERT_NE(nullptr, sensor_processor_read_data);
    sensor_processor_inject_data = export_symbol<AnboxSensorProcessorInjectDataFunc>(
                   anbox_sensor_processor_inject_data_name);
    ASSERT_NE(nullptr, sensor_processor_inject_data);
  }

  void TearDown() override {
    if (platform)
      release_platform(platform);
    PlatformBehaviorTest::TearDown();
  }

  bool valid_sensor_data(const AnboxSensorData& data, float min, float max) {
    // Check the internal values in the union
    if (!is_valid_sensor_type(data.sensor_type))
      return false;

    if (data.values[0] < min || data.values[0] > max)
      return false;

    if (is_multiple_axis_sensor(data.sensor_type)
        && ((data.values[1] < min || data.values[1] > max)
        || (data.values[2] < min || data.values[2] > max)))
      return false;

    // Explicitly check each sensor specific data
    switch (data.sensor_type) {
      case AnboxSensorType::ACCELERATION:
        return data.acceleration.axis.x > min && data.acceleration.axis.x < max
            && data.acceleration.axis.y > min && data.acceleration.axis.y < max
            && data.acceleration.axis.z > min && data.acceleration.axis.z < max;
      case AnboxSensorType::GYROSCOPE:
        return data.gyroscope.axis.x > min && data.gyroscope.axis.x < max
            && data.gyroscope.axis.y > min && data.gyroscope.axis.y < max
            && data.gyroscope.axis.z > min && data.gyroscope.axis.z < max;
      case AnboxSensorType::MAGNETOMETER:
        return data.magnetic.axis.x > min && data.magnetic.axis.x < max
            && data.magnetic.axis.y > min && data.magnetic.axis.y < max
            && data.magnetic.axis.z > min && data.magnetic.axis.z < max;
      case AnboxSensorType::ORIENTATION:
        return data.orientation.angle.azimuth > min && data.orientation.angle.azimuth < max
            && data.orientation.angle.pitch > min && data.orientation.angle.pitch < max
            && data.orientation.angle.roll > min && data.orientation.angle.roll < max;
      case AnboxSensorType::TEMPERATURE:
        return data.temperature > min && data.temperature < max;
      case AnboxSensorType::PROXIMITY:
        return data.proximity > min && data.proximity < max;
      case AnboxSensorType::LIGHT:
        return data.light > min && data.light < max;
      case AnboxSensorType::PRESSURE:
        return data.pressure > min && data.pressure < max;
      case AnboxSensorType::HUMIDITY:
        return data.humidity > min && data.humidity < max;
      default:
        return false;
    }
  }

 protected:
  AnboxPlatform* platform{nullptr};
  AnboxSensorProcessorSupportedSensorsFunc sensor_processor_supported_sensors{nullptr};
  AnboxSensorProcessorReadDataFunc sensor_processor_read_data{nullptr};
  AnboxSensorProcessorInjectDataFunc sensor_processor_inject_data{nullptr};
};

class PlatformProxyTest : public PlatformBehaviorTest {
 public:
  void SetUp() override {
    PlatformBehaviorTest::SetUp();

    platform = create_platform(nullptr);
    ASSERT_NE(nullptr, platform);
    if (ready(platform) == false)
      ASSERT_EQ(0, wait_until_ready(platform));

    get_proxy = export_symbol<AnboxPlatformGetAnboxProxyFunc>(
            anbox_platform_get_anbox_proxy_name);
    ASSERT_NE(nullptr, get_proxy);

    proxy_set_change_screen_orientation_callback = export_symbol<AnboxProxySetChangeScreenOrientationCallbackFunc>(
                   anbox_proxy_set_change_screen_orientation_callback_name);
    ASSERT_NE(nullptr, proxy_set_change_screen_orientation_callback);
    proxy_set_change_display_density_callback = export_symbol<AnboxProxySetChangeDisplayDensityCallbackFunc>(
                   anbox_proxy_set_change_display_density_callback_name);
    ASSERT_NE(nullptr, proxy_set_change_display_density_callback);
    proxy_set_change_display_size_callback = export_symbol<AnboxProxySetChangeDisplaySizeCallbackFunc>(
                   anbox_proxy_set_change_display_size_callback_name);
    ASSERT_NE(nullptr, proxy_set_change_display_size_callback);
    proxy_send_message = export_symbol<AnboxProxySendMessageFunc>(
                   anbox_proxy_send_message_name);
    ASSERT_NE(nullptr, proxy_send_message);
    proxy_set_trigger_action_callback = export_symbol<AnboxProxySetTriggerActionCallbackFunc>(
                   anbox_proxy_set_trigger_action_callback_name);
    ASSERT_NE(nullptr, proxy_set_trigger_action_callback);
  }

  void TearDown() override {
    if (platform)
      release_platform(platform);
    PlatformBehaviorTest::TearDown();
  }

 protected:
  AnboxPlatform* platform{nullptr};
  AnboxProxySetChangeScreenOrientationCallbackFunc proxy_set_change_screen_orientation_callback{nullptr};
  AnboxProxySetChangeDisplayDensityCallbackFunc proxy_set_change_display_density_callback{nullptr};
  AnboxProxySetChangeDisplaySizeCallbackFunc proxy_set_change_display_size_callback{nullptr};
  AnboxProxySetTriggerActionCallbackFunc proxy_set_trigger_action_callback{nullptr};
  AnboxProxySendMessageFunc proxy_send_message{nullptr};
};

class PlatformGpsProcessorTest : public PlatformBehaviorTest {
 public:
  void SetUp() override {
    PlatformBehaviorTest::SetUp();

    platform = create_platform(nullptr);
    ASSERT_NE(nullptr, platform);
    if (ready(platform) == false)
      ASSERT_EQ(0, wait_until_ready(platform));

    get_gps_processor = export_symbol<AnboxPlatformGetGpsProcessorFunc>(
                   anbox_platform_get_gps_processor_name);
    ASSERT_NE(nullptr, get_gps_processor);

    gps_processor_read_data = export_symbol<AnboxGpsProcessorReadDataFunc>(
                   anbox_gps_processor_read_data_name);
    ASSERT_NE(nullptr, gps_processor_read_data);
    gps_processor_inject_data = export_symbol<AnboxGpsProcessorInjectDataFunc>(
                   anbox_gps_processor_inject_data_name);
    ASSERT_NE(nullptr, gps_processor_inject_data);
  }

  void TearDown() override {
    if (platform)
      release_platform(platform) ;
    PlatformBehaviorTest::TearDown();
  }

protected:
 AnboxPlatform* platform{nullptr};
 AnboxGpsProcessorReadDataFunc gps_processor_read_data{nullptr};
 AnboxGpsProcessorInjectDataFunc gps_processor_inject_data{nullptr};
};

class PlatformCameraProcessorTest : public PlatformBehaviorTest {
 public:
  void SetUp() override {
    PlatformBehaviorTest::SetUp();

    platform = create_platform(nullptr);
    ASSERT_NE(nullptr, platform);
    if (ready(platform) == false)
      ASSERT_EQ(0, wait_until_ready(platform));

    get_camera_processor = export_symbol<AnboxPlatformGetCameraProcessorFunc>(
                   anbox_platform_get_camera_processor_name);
    ASSERT_NE(nullptr, get_camera_processor);

    camera_processor_get_device_specs = export_symbol<AnboxCameraProcessorGetDeviceSpecsFunc>(
                   anbox_camera_processor_get_device_specs_name);
    ASSERT_NE(nullptr, camera_processor_get_device_specs);

    camera_processor_read_frame = export_symbol<AnboxCameraProcessorReadFrameFunc>(
                   anbox_camera_processor_read_frame_name);
    ASSERT_NE(nullptr, camera_processor_read_frame);
    camera_processor_inject_frame = export_symbol<AnboxCameraProcessorInjectFrameFunc>(
                   anbox_camera_processor_inject_frame_name);
    ASSERT_NE(nullptr, camera_processor_inject_frame);

    camera_processor_open_device = export_symbol<AnboxCameraProcessorOpenDeviceFunc>(
                   anbox_camera_processor_open_device_name);
    ASSERT_NE(nullptr, camera_processor_open_device);
    camera_processor_close_device = export_symbol<AnboxCameraProcessorCloseDeviceFunc>(
                   anbox_camera_processor_close_device_name);
    ASSERT_NE(nullptr, camera_processor_close_device);
  }

  void TearDown() override {
    if (platform)
      release_platform(platform) ;
    PlatformBehaviorTest::TearDown();
  }

  void RenderFrame(const AnboxCameraProcessor* processor) {
    AnboxVideoFrame frame;
    int ret = camera_processor_read_frame(processor, &frame, -1);
    EXPECT_EQ(ret, 0);
    EXPECT_GT(frame.size, 0);
    EXPECT_NE(frame.data, nullptr);

    std::this_thread::sleep_for(std::chrono::milliseconds{1000 / 30});
    EXPECT_NE(frame.data, nullptr);
    free(frame.data);
  }

  void OpenCamera(const AnboxCameraProcessor* processor) {
    AnboxCameraSpec* camera_specs{nullptr};
    size_t camera_specs_len{0};
    auto ret = camera_processor_get_device_specs(processor, &camera_specs,
        &camera_specs_len);
    EXPECT_EQ(ret, 0);
    EXPECT_NE(camera_specs, nullptr);
    EXPECT_GT(camera_specs_len, 0);

    // Use the first camera spec for testing
    ret = camera_processor_open_device(processor, camera_specs[0], CAMERA_ORIENTATION_LANDSCAPE);
    EXPECT_EQ(ret, 0);
  }

protected:
 AnboxPlatform* platform{nullptr};
 AnboxCameraProcessorGetDeviceSpecsFunc camera_processor_get_device_specs{nullptr};
 AnboxCameraProcessorReadFrameFunc camera_processor_read_frame{nullptr};
 AnboxCameraProcessorInjectFrameFunc camera_processor_inject_frame{nullptr};
 AnboxCameraProcessorOpenDeviceFunc camera_processor_open_device{nullptr};
 AnboxCameraProcessorCloseDeviceFunc camera_processor_close_device{nullptr};
};
} // namespace

TEST_F(BasePlatformTest, ExportsMandatorySymbols) {
  auto create_platform = export_symbol<AnboxInitializePlatformFunc>(
              anbox_initialize_platform_name);
  ASSERT_NE(nullptr, create_platform);
  auto release_platform = export_symbol<AnboxReleasePlatformFunc>(
              anbox_deinitialize_platform_name);
  ASSERT_NE(nullptr, release_platform);
  auto audio_processor = export_symbol<AnboxPlatformGetAudioProcessorFunc>(
              anbox_platform_get_audio_processor_name);
  ASSERT_NE(nullptr, audio_processor);
  auto input_processor = export_symbol<AnboxPlatformGetInputProcessorFunc>(
              anbox_platform_get_input_processor_name);
  ASSERT_NE(nullptr, input_processor);
  auto graphics_processor = export_symbol<AnboxPlatformGetGraphicsProcessorFunc>(
              anbox_platform_get_graphics_processor_name);
  ASSERT_NE(nullptr, graphics_processor);
  auto ready = export_symbol<AnboxPlatformReadyFunc>(
              anbox_platform_ready_name);
  ASSERT_NE(nullptr, ready);
  auto wait_until_ready = export_symbol<AnboxPlatformWaitUntilReadyFunc>(
              anbox_platform_wait_until_ready_name);
  ASSERT_NE(nullptr, wait_until_ready);
  auto anbox_platform_get_config_item = export_symbol<AnboxPlatformGetConfigItemFunc>(
              anbox_platform_get_config_item_name);
  ASSERT_NE(nullptr, anbox_platform_get_config_item);
  auto audio_processor_process_data = export_symbol<AnboxAudioProcessorProcessDataFunc>(
              anbox_audio_processor_process_data_name);
  ASSERT_NE(nullptr, audio_processor_process_data);
  auto input_processor_read_event = export_symbol<AnboxInputProcessorReadEventFunc>(
              anbox_input_processor_read_event_name);
  ASSERT_NE(nullptr, input_processor_read_event);
  auto input_processor_inject_event = export_symbol<AnboxInputProcessorInjectEventFunc>(
              anbox_input_processor_inject_event_name);
  ASSERT_NE(nullptr, input_processor_inject_event);
}

TEST_F(BasePlatformTest, ExposesPlatformDescriptor) {
  AnboxPlatformDescriptor *descriptor = nullptr;
  load_descriptor(&descriptor);
  ASSERT_NE(nullptr, descriptor);
  ASSERT_EQ(descriptor->platform_version, ANBOX_PLATFORM_VERSION);
  uint32_t major, minor, patch;
  anbox_platform_get_versions(descriptor->platform_version, major, minor, patch);
  ASSERT_EQ(major, ANBOX_PLATFORM_MAJOR_VERSION);
  ASSERT_EQ(minor, ANBOX_PLATFORM_MINOR_VERSION);
  ASSERT_EQ(patch, ANBOX_PLATFORM_PATCH_VERSION);
  ASSERT_GE(strlen(descriptor->description), 0);
  ASSERT_GE(strlen(descriptor->vendor), 0);
  ASSERT_GE(strlen(descriptor->name), 0);
}

TEST_F(PlatformBehaviorTest, CanCreateAndReleasePlatformWithoutOptions) {
  auto platform = create_platform(nullptr);
  ASSERT_NE(nullptr, platform);
  if (ready(platform) == false)
    ASSERT_EQ(0, wait_until_ready(platform));
  release_platform(platform);
}

TEST_F(PlatformBehaviorTest, CanCreateAndReleasePlatform) {
  AnboxPlatformConfiguration configuration;
  auto platform = create_platform(&configuration);
  ASSERT_NE(nullptr, platform);
  if (ready(platform) == false)
    ASSERT_EQ(0, wait_until_ready(platform));
  release_platform(platform);
}

TEST_F(PlatformBehaviorTest, SupportsAllMandatoryFeatures) {
  AnboxPlatformConfiguration configuration;
  auto platform = create_platform(&configuration);
  ASSERT_NE(nullptr, platform);
  if (ready(platform) == false)
    ASSERT_EQ(0, wait_until_ready(platform));

  char egl_driver[MAX_STRING_LENGTH] = {'\0'};
  auto ret = get_config_item(platform,
                             AnboxPlatformConfigurationKey::EGL_DRIVER_PATH,
                             egl_driver,
                             MAX_STRING_LENGTH);
  EXPECT_THAT(ret, 0);
  EXPECT_THAT(egl_driver, ::testing::MatchesRegex("^/.*/[a-zA-Z][a-zA-Z0-9]+.so"));

  char glesv1_driver[MAX_STRING_LENGTH] = {'\0'};
  ret = get_config_item(platform,
                        AnboxPlatformConfigurationKey::OPENGL_ES1_CM_DRIVER_PATH,
                        glesv1_driver,
                        MAX_STRING_LENGTH);
  EXPECT_THAT(ret, 0);
  EXPECT_THAT(glesv1_driver, ::testing::MatchesRegex("^/.*/[a-zA-Z0-9]+.so"));

  char glesv2_driver[MAX_STRING_LENGTH] = {'\0'};
  ret = get_config_item(platform,
                        AnboxPlatformConfigurationKey::OPENGL_ES2_DRIVER_PATH,
                        glesv2_driver,
                        MAX_STRING_LENGTH);
  EXPECT_THAT(ret, 0);
  EXPECT_THAT(glesv2_driver, ::testing::MatchesRegex("^/.*/[a-zA-Z][a-zA-Z0-9]+.so"));

  AnboxDisplaySpec display_spec;
  memset(&display_spec, 0, sizeof(display_spec));
  ret = get_config_item(platform,
                        AnboxPlatformConfigurationKey::DISPLAY_SPEC,
                        &display_spec,
                        sizeof(display_spec));
  if (ret < 0) {
    AnboxDisplaySpec2 display_spec2;
    memset(&display_spec2, 0, sizeof(display_spec2));
    ret = get_config_item(platform,
                          AnboxPlatformConfigurationKey::DISPLAY_SPEC2,
                          &display_spec2,
                          sizeof(display_spec2));
    EXPECT_GT(display_spec2.width, 0);
    EXPECT_GT(display_spec2.height, 0);
    EXPECT_GE(display_spec2.density, 0);
  } else {
    EXPECT_GT(display_spec.width, 0);
    EXPECT_GT(display_spec.height, 0);
    EXPECT_GE(display_spec.density, 0);
  }

  AnboxAudioSpec audio_spec;
  memset(&audio_spec, 0, sizeof(audio_spec));
  ret = get_config_item(platform,
                        AnboxPlatformConfigurationKey::AUDIO_SPEC,
                        &audio_spec,
                        sizeof(audio_spec));
  EXPECT_THAT(ret, 0);
  EXPECT_GT(audio_spec.freq, 0);
  EXPECT_GT(audio_spec.samples, 0);
  EXPECT_GT(audio_spec.channels, 0);
  EXPECT_NE(audio_spec.format, AUDIO_FORMAT_INVALID);

  release_platform(platform);
}

TEST_F(PlatformBehaviorTest, FetchInValidConfigItem) {
  AnboxPlatformConfiguration configuration;
  auto platform = create_platform(&configuration);
  ASSERT_NE(nullptr, platform);
  if (ready(platform) == false)
    ASSERT_EQ(0, wait_until_ready(platform));

  char glesv2_driver[MAX_STRING_LENGTH] = {'\0'};
  auto ret = get_config_item(platform,
                             (AnboxPlatformConfigurationKey)(0xffff),
                             glesv2_driver,
                             MAX_STRING_LENGTH);
  ASSERT_EQ(ret, -EINVAL);

  release_platform(platform);
}

TEST_F(PlatformBehaviorTest, ProvidesDisplaySpec) {
  AnboxPlatformConfiguration configuration;
  auto platform = create_platform(&configuration);
  ASSERT_NE(nullptr, platform);
  if (ready(platform) == false)
    ASSERT_EQ(0, wait_until_ready(platform));

  // The platform needs to provide one of the two display spec struct versions
  AnboxDisplaySpec spec1;
  auto ret = get_config_item(platform, DISPLAY_SPEC, &spec1, sizeof(AnboxDisplaySpec));
  if (ret < 0) {
    AnboxDisplaySpec2 spec2;
    ret = get_config_item(platform, DISPLAY_SPEC2, &spec2, sizeof(AnboxDisplaySpec2));
    ASSERT_EQ(ret, 0);
  }

  release_platform(platform);
}

TEST_F(PlatformInputProcessorTest, CanReadEventInBlockMode) {
  const auto input_processor = get_input_processor(platform);
  EXPECT_NE(nullptr, input_processor);

  int ret = input_processor_inject_event(input_processor, AnboxInputEvent{KEYBOARD, 0, EV_KEY,  KEY_ENTER, 0});
  EXPECT_EQ(ret, 0);
  AnboxInputEvent input_ev;
  ret = input_processor_read_event(input_processor, &input_ev, -1);
  EXPECT_EQ(ret, 0);
  EXPECT_EQ(input_ev.device_type, KEYBOARD);
  EXPECT_EQ(input_ev.device_id, 0);
  EXPECT_EQ(input_ev.type, EV_KEY);
  EXPECT_EQ(input_ev.code, KEY_ENTER);
  EXPECT_EQ(input_ev.value, 0);
}

TEST_F(PlatformInputProcessorTest, CanReadEventInNonblockMode) {
  const auto input_processor = get_input_processor(platform);
  ASSERT_NE(nullptr, input_processor);

  AnboxInputEvent input_ev;
  int ret = input_processor_read_event(input_processor, &input_ev, 0);
  EXPECT_EQ(ret, -EIO); // return immediately as no event in the queue at this moment.

  ret = input_processor_inject_event(input_processor, AnboxInputEvent{KEYBOARD, 0, EV_KEY, KEY_ENTER, 0});
  EXPECT_EQ(ret, 0);
  ret = input_processor_read_event(input_processor, &input_ev, 0);
  EXPECT_EQ(ret, 0);
  EXPECT_EQ(input_ev.device_type, KEYBOARD);
  EXPECT_EQ(input_ev.device_id, 0);
  EXPECT_EQ(input_ev.type, EV_KEY);
  EXPECT_EQ(input_ev.code, KEY_ENTER);
  EXPECT_EQ(input_ev.value, 0);
}

TEST_F(PlatformInputProcessorTest, CanReadMultipleEvents) {
  const auto input_processor = get_input_processor(platform);
  ASSERT_NE(nullptr, input_processor);

  for (size_t n = 0; n < event_numbers; n++) {
    auto type_index = n % (EV_MAX - EV_KEY);
    auto code_index = n % (KEY_UNKNOWN - KEY_RESERVED);
    auto value_index = n % event_value_numbers;
    int ret = input_processor_inject_event(input_processor, AnboxInputEvent{KEYBOARD, 0,
                                                             (uint16_t)(EV_KEY + type_index),
                                                             (uint16_t)(KEY_RESERVED + code_index),
                                                             (int32_t)value_index});
    EXPECT_EQ(ret, 0);
  }

  for (size_t n = 0; n < event_numbers; n++) {
    auto type_index = n % (EV_MAX - EV_KEY);
    auto code_index = n % (KEY_UNKNOWN - KEY_RESERVED);
    auto value_index = n % event_value_numbers;
    AnboxInputEvent input_ev;
    int ret = input_processor_read_event(input_processor, &input_ev, 1000);
    EXPECT_EQ(ret, 0);
    EXPECT_EQ(input_ev.device_type, KEYBOARD);
    EXPECT_EQ(input_ev.device_id, 0);
    EXPECT_EQ(input_ev.type, EV_KEY + type_index);
    EXPECT_EQ(input_ev.code, KEY_RESERVED + code_index);
    EXPECT_EQ(input_ev.value, value_index);
  }

  // The event queue is empty now, so any call to read_event must error out after 1s timeout.
  AnboxInputEvent input_ev;
  int ret = input_processor_read_event(input_processor, &input_ev, 1000);
  EXPECT_EQ(ret, -EIO);
}

TEST_F(PlatformInputProcessorTest, ReadEventBlocksWhenNoEventAvailable) {
  const auto input_processor = get_input_processor(platform);
  ASSERT_NE(nullptr, input_processor);

  int ret = input_processor_inject_event(input_processor, AnboxInputEvent{KEYBOARD, 0, EV_KEY, KEY_ENTER, 0});
  ASSERT_EQ(ret, 0);
  auto fut = std::async(
    std::launch::async, [&]() {
      AnboxInputEvent input_ev;
      ret = input_processor_read_event(input_processor, &input_ev, -1);
      EXPECT_EQ(ret, 0);
      EXPECT_EQ(input_ev.device_type, KEYBOARD);
      EXPECT_EQ(input_ev.device_id, 0);
      EXPECT_EQ(input_ev.type, EV_KEY);
      EXPECT_EQ(input_ev.code, KEY_ENTER);
      EXPECT_EQ(input_ev.value, 0);
  });

  ASSERT_EQ(fut.wait_for(chrono::seconds(1)), std::future_status::ready);

  // The event queue is empty now, so any call to read_event will block until next available
  // event occurs.
  fut = std::async(
    std::launch::async, [&]() {
      AnboxInputEvent input_ev;
      ret = input_processor_read_event(input_processor, &input_ev, -1);
      EXPECT_EQ(ret, 0);
      EXPECT_EQ(input_ev.device_type, KEYBOARD);
      EXPECT_EQ(input_ev.device_id, 0);
      EXPECT_EQ(input_ev.type, EV_KEY);
      EXPECT_EQ(input_ev.code, KEY_ENTER);
      EXPECT_EQ(input_ev.value, 0);
  });

  ASSERT_EQ(fut.wait_for(chrono::seconds(1)), std::future_status::timeout);
  // We insert one event in the queue to unblock the pending event request.
  ret = input_processor_inject_event(input_processor, AnboxInputEvent{KEYBOARD, 0, EV_KEY, KEY_ENTER, 0});
  EXPECT_EQ(ret, 0);
}

TEST_F(PlatformInputProcessorTest, ReadEventDoesNotForwardInvalidEvents) {
  const auto input_processor = get_input_processor(platform);
  ASSERT_NE(nullptr, input_processor);

  int ret = input_processor_inject_event(input_processor, AnboxInputEvent{KEYBOARD, 0, EV_MAX, KEY_RESERVED, 0});
  ASSERT_EQ(ret, 0);
  AnboxInputEvent input_ev;
  ret = input_processor_read_event(input_processor, &input_ev, -1);
  ASSERT_EQ(ret, -EIO);
}

TEST_F(PlatformAudioProcessorTest, CanWriteAudioData) {
  const auto audio_processor = get_audio_processor(platform);
  ASSERT_NE(nullptr, audio_processor);

  // Feed random pcm data for a while(5s).
  std::atomic_bool finished{false};
  RandomDataGenerator pcm_generator;
  auto fut = std::async(
    std::launch::async, [&]() {
    uint8_t buf[big_chunk_size];
    while (!finished) {
      auto read_size = pcm_generator.generate(buf, sizeof(buf));
      EXPECT_LT(0, read_size);
      auto written_size = audio_processor_write_data(audio_processor, buf, read_size);
      EXPECT_EQ(read_size, written_size);
    }
  });

  if (fut.wait_for(std::chrono::seconds(timeout_in_secs)) == std::future_status::timeout) {
    finished = true;
  };
}

TEST_F(PlatformAudioProcessorTest, CanWriteAudioDataWithSmallChunk) {
  const auto audio_processor = get_audio_processor(platform);
  ASSERT_NE(nullptr, audio_processor);

  // Feed random pcm data for a while(5s).
  std::atomic<bool> finished{false};
  RandomDataGenerator pcm_generator;
  auto fut = std::async(
    std::launch::async, [&]() {
    uint8_t buf[small_chunk_size];
    while (!finished) {
      auto read_size = pcm_generator.generate(buf, sizeof(buf));
      EXPECT_LT(0, read_size);
      auto written_size = audio_processor_write_data(audio_processor, buf, read_size);
      EXPECT_EQ(read_size, written_size);
    }
  });

  if (fut.wait_for(std::chrono::seconds(timeout_in_secs)) == std::future_status::timeout) {
    finished = true;
  };
}

TEST_F(PlatformAudioProcessorTest, WriteAudioDataWithFlakyData) {
  const auto audio_processor = get_audio_processor(platform);
  ASSERT_NE(nullptr, audio_processor);

  int written_size = audio_processor_write_data(audio_processor, nullptr, 1);
  EXPECT_EQ(written_size, -EIO);

  written_size = audio_processor_write_data(audio_processor, nullptr, 0);
  EXPECT_EQ(written_size, -EIO);
}

TEST_F(PlatformAudioProcessorTest, CanReadAudioData) {
  const auto audio_processor = get_audio_processor(platform);
  ASSERT_NE(nullptr, audio_processor);

  uint8_t data[audio_buffer_length];
  int read_size = audio_processor_read_data(audio_processor, data, audio_buffer_length);
  EXPECT_LT(0, read_size);
}

TEST_F(PlatformAudioProcessorTest, CanDoAudioStandby) {
  const auto audio_processor = get_audio_processor(platform);
  ASSERT_NE(nullptr, audio_processor);

  int ret = audio_processor_standby(audio_processor, AUDIO_OUTPUT_STREAM);
  EXPECT_EQ(0, ret);

  ret = audio_processor_standby(audio_processor, AUDIO_INPUT_STREAM);
  EXPECT_EQ(0, ret);
}

TEST_F(PlatformAudioProcessorTest, ReadAudioDataWithFlakyData) {
  const auto audio_processor = get_audio_processor(platform);
  ASSERT_NE(nullptr, audio_processor);

  int read_size = audio_processor_write_data(audio_processor, nullptr, 1);
  EXPECT_EQ(read_size, -EIO);

  read_size = audio_processor_write_data(audio_processor, nullptr, 0);
  EXPECT_EQ(read_size, -EIO);
}

TEST_F(PlatformGraphicsProcessorTest, CanBeInitialized) {
  const auto graphics_processor = get_graphics_processor(platform);
  ASSERT_NE(nullptr, graphics_processor);

  AnboxGraphicsConfiguration configuration;
  configuration.native_display = EGL_DEFAULT_DISPLAY;
  configuration.native_window = 0;
  configuration.output_flip_mode = FLIP_MODE_NONE;
  configuration.texture_format = TEXTURE_FORMAT_RGBA;
  configuration.avoid_pbuffers = true;

  int ret = graphics_processor_initialize(graphics_processor, &configuration);
  EXPECT_EQ(ret, 0);
}

TEST_F(PlatformGraphicsProcessorTest, ProcessesSingleFrame) {
  const auto graphics_processor = get_graphics_processor(platform);
  ASSERT_NE(nullptr, graphics_processor);

  graphics_processor_begin_frame(graphics_processor);
  graphics_processor_finish_frame(graphics_processor);
}

TEST_F(PlatformSensorProcessorTest, CanReadMultipleSensorData) {
  const auto sensor_processor = get_sensor_processor(platform);
  ASSERT_NE(nullptr, sensor_processor);

  const auto min_value = -10.0;
  const auto max_value = 10.0;
  SensorDataGenerator sensor_data_generator;
  for (size_t n = 0; n < sensor_data_numbers; n++) {
    AnboxSensorData sensor_data;
    int ret = sensor_data_generator.generate(&sensor_data, min_value, max_value);
    EXPECT_EQ(ret, 0);
    ret = sensor_processor_inject_data(sensor_processor, sensor_data);
    EXPECT_EQ(ret, 0);
  }

  for (size_t n = 0; n < sensor_data_numbers; n++) {
    AnboxSensorData sensor_data;
    int ret = sensor_processor_read_data(sensor_processor, &sensor_data, 1000);
    EXPECT_EQ(ret, 0);
    EXPECT_TRUE(valid_sensor_data(sensor_data, min_value, max_value));
  }

  // The data queue is empty now, so any call to read_data must error out after 1s timeout.
  AnboxSensorData sensor_data;
  int ret = sensor_processor_read_data(sensor_processor, &sensor_data, 1000);
  EXPECT_EQ(ret, -EIO);
}

TEST_F(PlatformSensorProcessorTest, ReadDataBlocksWhenNoEventAvailable) {
  const auto sensor_processor = get_sensor_processor(platform);
  ASSERT_NE(nullptr, sensor_processor);

  const auto min_value = -10.0;
  const auto max_value = 10.0;
  SensorDataGenerator sensor_data_generator;
  AnboxSensorData sensor_data;
  int ret = sensor_data_generator.generate(&sensor_data, min_value, max_value);
  EXPECT_EQ(ret, 0);

  ret = sensor_processor_inject_data(sensor_processor, sensor_data);
  ASSERT_EQ(ret, 0);
  auto fut = std::async(
    std::launch::async, [&]() {
      AnboxSensorData data;
      ret = sensor_processor_read_data(sensor_processor, &data, -1);
      EXPECT_EQ(ret, 0);
      EXPECT_TRUE(valid_sensor_data(data, min_value, max_value));
  });

  ASSERT_EQ(fut.wait_for(chrono::seconds(1)), std::future_status::ready);

  // The data queue is empty now, so any call to read_event will block until next available
  // sensor data arrival.
  fut = std::async(
    std::launch::async, [&]() {
      AnboxSensorData data;
      ret = sensor_processor_read_data(sensor_processor, &data, -1);
      EXPECT_EQ(ret, 0);
      EXPECT_TRUE(valid_sensor_data(data, min_value, max_value));
  });

  ASSERT_EQ(fut.wait_for(chrono::seconds(1)), std::future_status::timeout);
  // We add one sensor data in the queue to unblock the pending sensor data request.
  ret = sensor_processor_inject_data(sensor_processor, sensor_data);
  EXPECT_EQ(ret, 0);
}

TEST_F(PlatformSensorProcessorTest, ReadDataDoesNotForwardInvalidEvents) {
  const auto sensor_processor = get_sensor_processor(platform);
  ASSERT_NE(nullptr, sensor_processor);

  SensorDataGenerator sensor_data_generator;
  AnboxSensorData data;
  int ret = sensor_data_generator.generate(&data);
  ASSERT_EQ(ret, 0);

  data.sensor_type = static_cast<AnboxSensorType>(1 << (supported_sensor_count + 1));
  ret = sensor_processor_inject_data(sensor_processor, data);
  ASSERT_EQ(ret, 0);

  AnboxSensorData sensor_data;
  ret = sensor_processor_read_data(sensor_processor, &sensor_data, -1);
  ASSERT_EQ(ret, -EIO);
}

TEST_F(PlatformSensorProcessorTest, CanReadDataInBlockMode) {
  const auto sensor_processor = get_sensor_processor(platform);
  ASSERT_NE(nullptr, sensor_processor);

  SensorDataGenerator sensor_data_generator;
  AnboxSensorData data;
  int ret = sensor_data_generator.generate(&data);
  ASSERT_EQ(ret, 0);

  ret = sensor_processor_inject_data(sensor_processor, data);
  EXPECT_EQ(ret, 0);
  AnboxSensorData sensor_data;

  ret = sensor_processor_read_data(sensor_processor, &sensor_data, -1);
  EXPECT_EQ(ret, 0);
  EXPECT_TRUE(valid_sensor_data(data, -1.0, 1.0));
}

TEST_F(PlatformSensorProcessorTest, CanReadDataInNonblockMode) {
  const auto sensor_processor = get_sensor_processor(platform);
  ASSERT_NE(nullptr, sensor_processor);

  AnboxSensorData sensor_data;
  int ret = sensor_processor_read_data(sensor_processor, &sensor_data, 0);
  EXPECT_EQ(ret, -EIO); // return immediately as no data in the queue at this moment.

  SensorDataGenerator sensor_data_generator;
  AnboxSensorData data;
  ret = sensor_data_generator.generate(&data);
  ASSERT_EQ(ret, 0);

  ret = sensor_processor_inject_data(sensor_processor, data);
  EXPECT_EQ(ret, 0);

  ret = sensor_processor_read_data(sensor_processor, &sensor_data, 0);
  EXPECT_EQ(ret, 0);
  EXPECT_TRUE(valid_sensor_data(data, -1.0, 1.0));
}

TEST_F(PlatformProxyTest, CanInvokeCallbackFunctionsWhenSet) {
  const auto anbox_proxy = get_proxy(platform);
  ASSERT_NE(nullptr, anbox_proxy);

  // Register dummy callback functions
  auto change_screen_orientation_callback = [](AnboxScreenOrientationType orientation_type,
                                               void* user_data) {
    (void) user_data;
    if (orientation_type < PORTRAIT || orientation_type > LANDSCAPE_REVERSED)
      return -EINVAL;
    return 0;
  };

  proxy_set_change_screen_orientation_callback(anbox_proxy, change_screen_orientation_callback, platform);
  // The actual display callback invocation functions won't be exported symbols
  // as the caller would be the platform plugin rather than Anbox.
  // To mock the invocation of callback functions to a platform via anbox proxy.
  // We access the underlying the instance of AnboxProxy here.
  const auto proxy = anbox_proxy->instance;
  auto ret = proxy->change_screen_orientation(PORTRAIT);
  EXPECT_EQ(ret, 0);
  ret = proxy->change_screen_orientation(LANDSCAPE);
  EXPECT_EQ(ret, 0);
  ret = proxy->change_screen_orientation(PORTRAIT_REVERSED);
  EXPECT_EQ(ret, 0);
  ret = proxy->change_screen_orientation(LANDSCAPE_REVERSED);
  EXPECT_EQ(ret, 0);
  ret = proxy->change_screen_orientation(static_cast<AnboxScreenOrientationType>(LANDSCAPE_REVERSED + 1));
  EXPECT_EQ(ret, -EINVAL);

  auto change_display_density_callback = [](uint32_t density, void* user_data) {
    (void) user_data;
    if (density < android_minimum_density)
      return -EINVAL;
    return 0;
  };
  proxy_set_change_display_density_callback(anbox_proxy, change_display_density_callback, platform);
  ret = proxy->change_display_density(android_minimum_density - 1);
  EXPECT_EQ(ret, -EINVAL);
  ret = proxy->change_display_density(android_minimum_density + 1);
  EXPECT_EQ(ret, 0);
  ret = proxy->change_display_density(android_minimum_density + 10);
  EXPECT_EQ(ret, 0);
  ret = proxy->change_display_density(android_minimum_density + 100);
  EXPECT_EQ(ret, 0);

  auto change_display_size_callback = [](uint32_t width, uint32_t height, void* user_data) {
    (void) user_data;
    if (width == 0 || height == 0)
      return -EINVAL;
    return 0;
  };
  proxy_set_change_display_size_callback(anbox_proxy, change_display_size_callback, platform);
  ret = proxy->change_display_size(0, 720);
  EXPECT_EQ(ret, -EINVAL);
  ret = proxy->change_display_size(1280, 0);
  EXPECT_EQ(ret, -EINVAL);
  ret = proxy->change_display_size(1280, 720);
  EXPECT_EQ(ret, 0);

  auto trigger_action_callback = [](const char* name, const char **args,
      size_t args_len, void* user_data) {
    (void) user_data;
    if (!name)
      return -EINVAL;
    if ((!args && args_len != 0) || (args && args_len == 0))
      return -EINVAL;
    return 0;
  };

  const char* action{"launch-activity"};
  const char* args{"com.canonical.anboxtestapp"};
  proxy_set_trigger_action_callback(anbox_proxy, trigger_action_callback, platform);
  ret = proxy->trigger_action(nullptr, &args, 1);
  EXPECT_EQ(ret, -EINVAL);
  ret = proxy->trigger_action(action, &args, 0);
  EXPECT_EQ(ret, -EINVAL);
  ret = proxy->trigger_action(action, nullptr, 1);
  EXPECT_EQ(ret, -EINVAL);
  ret = proxy->trigger_action(action, &args, 1);
  EXPECT_EQ(ret, 0);
}

TEST_F(PlatformProxyTest, InvokeCallbackFunctionsWhenNotSet) {
  const auto anbox_proxy = get_proxy(platform);
  ASSERT_NE(nullptr, anbox_proxy);

  const auto proxy = anbox_proxy->instance;
  auto ret = proxy->change_screen_orientation(PORTRAIT);
  EXPECT_EQ(ret, -EINVAL);

  ret = proxy->change_display_density(android_minimum_density);
  EXPECT_EQ(ret, -EINVAL);

  ret = proxy->change_display_size(1280, 720);
  EXPECT_EQ(ret, -EINVAL);
}

TEST_F(PlatformProxyTest, SendMessageWhenSet) {
  const auto anbox_proxy = get_proxy(platform);
  ASSERT_NE(nullptr, anbox_proxy);

  const auto proxy = anbox_proxy->instance;
  char type[] = "update";
  char data[] = "invoke-camera";
  auto ret = proxy->send_message(
        type, sizeof(type), data, sizeof(data));
  EXPECT_EQ(ret, 0);

  ret = proxy->send_message(
        nullptr, sizeof(type), data, sizeof(data));
  EXPECT_EQ(ret, -EINVAL);

  ret = proxy->send_message(
        type, 0, data, sizeof(data));
  EXPECT_EQ(ret, -EINVAL);

  ret = proxy->send_message(
        type, sizeof(type), nullptr, sizeof(data));
  EXPECT_EQ(ret, -EINVAL);

  ret = proxy->send_message(
        type, sizeof(type), data, 0);
  EXPECT_EQ(ret, -EINVAL);
}

TEST_F(PlatformProxyTest, SendMessageWhenNotSet) {
  const auto anbox_proxy = get_proxy(platform);
  ASSERT_NE(nullptr, anbox_proxy);

  const auto proxy = anbox_proxy->instance;
  char type[] = "invoke-camera";
  char data[] = "update";
  auto ret = proxy->send_message(
        type, sizeof(type), data, sizeof(data));
  EXPECT_EQ(ret, -EIO);
}

TEST_F(PlatformGpsProcessorTest, CanReadMultipleGpsData) {
  const auto gps_processor = get_gps_processor(platform);
  ASSERT_NE(nullptr, gps_processor);

  GpsDataGenerator gps_data_generator;
  for (size_t n = 0; n < gps_data_numbers; n++) {
    AnboxGpsData gps_data;
    int ret = gps_data_generator.generate(&gps_data);
    EXPECT_EQ(ret, 0);
    ret = gps_processor_inject_data(gps_processor, gps_data);
    EXPECT_EQ(ret, 0);
  }

  for (size_t n = 0; n < gps_data_numbers; n++) {
    AnboxGpsData gps_data;
    int ret = gps_processor_read_data(gps_processor, &gps_data, 1000);
    EXPECT_EQ(ret, 0);
  }

  // The data queue is empty now, so any call to read_data must error out after 1s timeout.
  AnboxGpsData gps_data;
  int ret = gps_processor_read_data(gps_processor, &gps_data, 1000);
  EXPECT_EQ(ret, -EIO);
}

TEST_F(PlatformGpsProcessorTest, ReadDataBlocksWhenNoDataAvailable) {
  const auto gps_processor = get_gps_processor(platform);
  ASSERT_NE(nullptr, gps_processor);

  AnboxGpsData gps_data;
  GpsDataGenerator gps_data_generator;
  int ret = gps_data_generator.generate(&gps_data);
  EXPECT_EQ(ret, 0);

  ret = gps_processor_inject_data(gps_processor, gps_data);
  ASSERT_EQ(ret, 0);
  auto fut = std::async(
    std::launch::async, [&]() {
      AnboxGpsData data;
      ret = gps_processor_read_data(gps_processor, &data, -1);
      EXPECT_EQ(ret, 0);
  });

  ASSERT_EQ(fut.wait_for(chrono::seconds(1)), std::future_status::ready);

  // The data queue is empty now, so any call to read_data will block until
  // next available gps data arrival.
  fut = std::async(
    std::launch::async, [&]() {
      AnboxGpsData data;
      ret = gps_processor_read_data(gps_processor, &data, -1);
      EXPECT_EQ(ret, 0);
  });

  ASSERT_EQ(fut.wait_for(chrono::seconds(1)), std::future_status::timeout);
  // We add one gps data in the queue to unblock the pending gps data request.
  ret = gps_processor_inject_data(gps_processor, gps_data);
  EXPECT_EQ(ret, 0);
}

TEST_F(PlatformGpsProcessorTest, ReadDataDoesNotForwardInvalidEvents) {
  const auto gps_processor = get_gps_processor(platform);
  ASSERT_NE(nullptr, gps_processor);

  GpsDataGenerator gps_data_generator;
  AnboxGpsData data;
  int ret = gps_data_generator.generate(&data);
  ASSERT_EQ(ret, 0);

  data.data_type = static_cast<AnboxGpsDataType>(1 << (supported_gps_data_count + 1));
  ret = gps_processor_inject_data(gps_processor, data);
  ASSERT_EQ(ret, 0);

  AnboxGpsData gps_data;
  ret = gps_processor_read_data(gps_processor, &gps_data, -1);
  ASSERT_EQ(ret, -EIO);
}

TEST_F(PlatformGpsProcessorTest, CanReadDataInBlockMode) {
  const auto gps_processor = get_gps_processor(platform);
  ASSERT_NE(nullptr, gps_processor);

  GpsDataGenerator gps_data_generator;
  AnboxGpsData data;
  int ret = gps_data_generator.generate(&data);
  ASSERT_EQ(ret, 0);

  ret = gps_processor_inject_data(gps_processor, data);
  EXPECT_EQ(ret, 0);

  AnboxGpsData gps_data;
  ret = gps_processor_read_data(gps_processor, &gps_data, -1);
  EXPECT_EQ(ret, 0);
}

TEST_F(PlatformGpsProcessorTest, CanReadDataInNonblockMode) {
  const auto gps_processor = get_gps_processor(platform);
  ASSERT_NE(nullptr, gps_processor);

  AnboxGpsData gps_data;
  int ret = gps_processor_read_data(gps_processor, &gps_data, 0);
  EXPECT_EQ(ret, -EIO); // return immediately as no data in the queue at this moment.

  GpsDataGenerator gps_data_generator;
  AnboxGpsData data;
  ret = gps_data_generator.generate(&data);
  ASSERT_EQ(ret, 0);

  ret = gps_processor_inject_data(gps_processor, data);
  EXPECT_EQ(ret, 0);

  ret = gps_processor_read_data(gps_processor, &gps_data, 0);
  EXPECT_EQ(ret, 0);
}

TEST_F(PlatformCameraProcessorTest, CannotReadFramesWhenCameraIsNotOpen) {
  auto camera_processor = get_camera_processor(platform);
  ASSERT_NE(nullptr, camera_processor);

  AnboxVideoFrame frame;
  int ret = camera_processor_read_frame(camera_processor, &frame, -1);
  EXPECT_EQ(ret, -EIO);
}

TEST_F(PlatformCameraProcessorTest, CannotReadFramesAfterCameraIsClosed) {
  auto camera_processor = get_camera_processor(platform);
  ASSERT_NE(nullptr, camera_processor);

  OpenCamera(camera_processor);

  VideoFrameGenerator video_frame_generator;
  AnboxVideoFrame frame;
  int ret = video_frame_generator.generate(frame, 1280, 720, VIDEO_FRAME_FORMAT_YUV420);
  EXPECT_EQ(ret, 0);
  ret = camera_processor_inject_frame(camera_processor, frame);
  EXPECT_EQ(ret, 0);

  AnboxVideoFrame frame_1;
  ret = camera_processor_read_frame(camera_processor, &frame_1, -1);
  EXPECT_EQ(frame_1.size, frame.size);
  EXPECT_NE(frame_1.data, nullptr);

  ret = camera_processor_close_device(camera_processor);
  EXPECT_EQ(ret, 0);

  AnboxVideoFrame frame_2;
  ret = camera_processor_read_frame(camera_processor, &frame_2, -1);
  EXPECT_EQ(ret, -EIO);
}

TEST_F(PlatformCameraProcessorTest, CanReadMultipleVideoFrames) {
  const auto camera_processor = get_camera_processor(platform);
  ASSERT_NE(nullptr, camera_processor);

  OpenCamera(camera_processor);

  VideoFrameGenerator video_frame_generator;
  for (size_t n = 0; n < video_frame_count; n++) {
    VideoFrameGenerator video_frame_generator;
    AnboxVideoFrame frame;
    int ret = video_frame_generator.generate(frame, 1280, 720, VIDEO_FRAME_FORMAT_YUV420);
    EXPECT_EQ(ret, 0);
    ret = camera_processor_inject_frame(camera_processor, frame);
    EXPECT_EQ(ret, 0);
  }

  for (size_t n = 0; n < video_frame_count; n++)
    RenderFrame(camera_processor);

  // The frame queue is empty now, so any call to read_frame must error out after 1s timeout.
  AnboxVideoFrame frame_1;
  int ret = camera_processor_read_frame(camera_processor, &frame_1, 1000);
  EXPECT_EQ(ret, -EIO);
}

TEST_F(PlatformCameraProcessorTest, ReadFrameBlocksWhenNoFrameAvailable) {
  const auto camera_processor = get_camera_processor(platform);
  ASSERT_NE(nullptr, camera_processor);

  OpenCamera(camera_processor);

  VideoFrameGenerator video_frame_generator;
  AnboxVideoFrame frame;
  int ret = video_frame_generator.generate(frame, 1280, 720, VIDEO_FRAME_FORMAT_YUV420);
  EXPECT_EQ(ret, 0);

  ret = camera_processor_inject_frame(camera_processor, frame);
  EXPECT_EQ(ret, 0);

  auto fut = std::async(
    std::launch::async, [&]() {
      AnboxVideoFrame frame;
      ret = camera_processor_read_frame(camera_processor, &frame, -1);
      EXPECT_EQ(ret, 0);
  });

  ASSERT_EQ(fut.wait_for(chrono::seconds(1)), std::future_status::ready);

  // The frame queue is empty now, so any call to read_frame will block until
  // next available video arrival.
  fut = std::async(
    std::launch::async, [&]() {
      RenderFrame(camera_processor);
  });

  ASSERT_EQ(fut.wait_for(chrono::seconds(1)), std::future_status::timeout);
  // We add one video frame in the queue to unblock the pending video frame request.
  ret = camera_processor_inject_frame(camera_processor, frame);
  EXPECT_EQ(ret, 0);
}

TEST_F(PlatformCameraProcessorTest, ReadFrameDoesNotForwardInvalidFrames) {
  const auto camera_processor = get_camera_processor(platform);
  ASSERT_NE(nullptr, camera_processor);

  OpenCamera(camera_processor);

  typedef std::function<void(AnboxVideoFrame* frame)> CustomizeFrameInjection;
  auto inject_video_frame = [=](uint32_t width, uint32_t height,
                                AnboxVideoColorSpaceFormat format,
                                CustomizeFrameInjection customize_injection = nullptr) {
    AnboxVideoFrame frame;

    VideoFrameGenerator video_frame_generator;
    auto ret = video_frame_generator.generate(frame, width, height, format);
    EXPECT_EQ(ret, 0);

    if (customize_injection)
      customize_injection(&frame);

    ret = camera_processor_inject_frame(camera_processor, frame);
    EXPECT_EQ(ret, 0);

    return frame;
  };

  auto read_video_frame = [=]() {
    AnboxVideoFrame video_frame;
    auto ret = camera_processor_read_frame(camera_processor, &video_frame, -1);
    EXPECT_EQ(ret, -EIO);
  };

  inject_video_frame(1280, 720, VIDEO_FRAME_FORMAT_YUV420,
      [](AnboxVideoFrame* frame){
        frame->size = 0;
      });
  read_video_frame();

  inject_video_frame(1280, 720, VIDEO_FRAME_FORMAT_YUV420,
      [](AnboxVideoFrame* frame){
        free(frame->data);
        frame->data = nullptr;
      });
  read_video_frame();

  inject_video_frame(1280, 720, VIDEO_FRAME_FORMAT_YUV420,
      [](AnboxVideoFrame* frame){
        frame->size = 1280 * 720;
      });
  read_video_frame();
}

TEST_F(PlatformCameraProcessorTest, CanReadFrameInBlockMode) {
  const auto camera_processor = get_camera_processor(platform);
  ASSERT_NE(nullptr, camera_processor);

  OpenCamera(camera_processor);

  VideoFrameGenerator video_frame_generator;
  AnboxVideoFrame frame;
  int ret = video_frame_generator.generate(frame, 1280, 720, VIDEO_FRAME_FORMAT_YUV420);
  EXPECT_EQ(ret, 0);

  ret = camera_processor_inject_frame(camera_processor, frame);
  EXPECT_EQ(ret, 0);

  RenderFrame(camera_processor);
}

TEST_F(PlatformCameraProcessorTest, CanReadFrameInNonblockMode) {
  const auto camera_processor = get_camera_processor(platform);
  ASSERT_NE(nullptr, camera_processor);

  OpenCamera(camera_processor);

  AnboxVideoFrame video_frame;
  int ret = camera_processor_read_frame(camera_processor, &video_frame, 0);
  EXPECT_EQ(ret, -EIO); // return immediately as no video frame in the queue yet.

  VideoFrameGenerator video_frame_generator;
  AnboxVideoFrame frame;
  ret = video_frame_generator.generate(frame, 1280, 720, VIDEO_FRAME_FORMAT_YUV420);
  EXPECT_EQ(ret, 0);

  ret = camera_processor_inject_frame(camera_processor, frame);
  EXPECT_EQ(ret, 0);

  RenderFrame(camera_processor);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);

  if (argc != 2) {
    std::cerr << "ERROR: Invalid number of arguments provided" << std::endl;
    print_usage();
    return EXIT_FAILURE;
  }

  platform_path = argv[1];
  if (!std::ifstream(platform_path).good()) {
    std::cerr << "ERROR: Platform module at path "
              << platform_path << " does not exist" << std::endl;
    return EXIT_FAILURE;
  }

  return RUN_ALL_TESTS();
}
