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

#ifndef ANBOX_PLATFORM_SDK_PLATFORM_H_
#define ANBOX_PLATFORM_SDK_PLATFORM_H_

#include "anbox-platform-sdk/types.h"
#include "anbox-platform-sdk/common.h"
#include "anbox-platform-sdk/version.h"
#include "anbox-platform-sdk/audio_processor.h"
#include "anbox-platform-sdk/input_processor.h"
#include "anbox-platform-sdk/sensor_processor.h"
#include "anbox-platform-sdk/gps_processor.h"
#include "anbox-platform-sdk/graphics_processor.h"
#include "anbox-platform-sdk/camera_processor.h"
#include "anbox-platform-sdk/anbox_proxy.h"
#include "anbox-platform-sdk/video_decoder.h"
#include "anbox-platform-sdk/vhal_connector.h"

namespace anbox {

/**
 * @brief Platform defines the custom Anbox platform implemented by a plugin.
 *
 * It supports the following features:
 *     - Process the audio data that is passed by Anbox from the Anroid container.
 *     - Delivery of input events to the Android container.
 *     - Loading a custom Open GL ES/EGL driver implementation.
 */
class Platform {
 public:
  Platform() = default;
  virtual ~Platform() = default;
  Platform(const Platform &) = delete;
  Platform& operator=(const Platform &) = delete;

  /**
   * @brief Retrieve the platform audio processor instance.
   *
   * A platform is supposed to have only a single audio processor
   * instance at all time.
   *
   * @return a valid AudioProcessor instance, otherwise NULL when an error occured
   * or the platform does not support audio processing.
   */
  virtual AudioProcessor* audio_processor() = 0;

  /**
   * @brief Retrieve the platform input processor instance.
   *
   * A platform is supposed to have only a single input processor
   * instance at all time.
   *
   * @return a valid InputProcessor instance, otherwise NULL when an error occured.
   * or the platform does not support input processing.
   */
  virtual InputProcessor* input_processor() = 0;

    /**
   * @brief Retrieve the platform graphics processor instance.
   *
   * A platform is supposed to have only a single graphics processor
   * instance at all time. Providing a graphics processor is not
   * mandatory for a platform.
   *
   * @return a valid GraphicsProcessor instance, otherwise NULL when an error occured.
   * or the platform does not support graphics processing.
   */
  virtual GraphicsProcessor* graphics_processor() { return nullptr; }

  /**
   * @brief Retrieve the platform sensor processor instance.
   *
   * A platform is supposed to have only a single sensor processor
   * instance at all time.
   *
   * @return a valid SensorProcessor instance, otherwise NULL when an error occured.
   * or the platform does not support sensor processing.
   */
  virtual SensorProcessor* sensor_processor() { return nullptr; }

  /**
   * @brief Retrieve the platform gps processor instance.
   *
   * A platform is supposed to have only a single gps processor
   * instance at all time.
   *
   * @return a valid SensorProcessor instance, otherwise NULL when an error occured.
   * or the platform does not support gps processing.
   */
  virtual GpsProcessor* gps_processor() { return nullptr; }

  /**
   * @brief Retrieve the platform camera processor instance.
   *
   * A platform is supposed to have only a single camera processor
   * instance at all time.
   *
   * @return a valid CameraProcessor instance, otherwise NULL when an error occured.
   * or the platform does not support posting a video frame to a camera application
   * running in Android container.
   */
  virtual CameraProcessor* camera_processor() { return nullptr; }

  /**
   * @brief Retrieve the platform anbox proxy.
   *
   * A platform is supposed to have only a single anbox proxy instance at all time.
   *
   * @return a valid AnboxProxy instance, otherwise NULL when an error occured.
   * or the platform does not support to handle the system behavior change via callbacks.
   */
  virtual AnboxProxy* anbox_proxy() { return &anbox_proxy_; }

  /**
   * @brief Create a video decoder instances for a codec of the given name
   *
   * @return a valid VideoDecoder instance, otherwise NULL when an error occurred or
   * if video decoding is not supported by the platform.
   */
  virtual VideoDecoder* create_video_decoder(AnboxVideoCodecType codec_type) {
    (void) codec_type;
    return nullptr;
  }

  /**
   * @brief Retrieve the platform vhal connector instance.
   *
   * A platform is supposed to have only a single vhal connector
   * instance at all time.
   *
   * @return a valid VhalConnector instance, otherwise NULL when an error occured,
   * or the platform does not support connecting to the vhal service
   * running in Android container.
   */
  virtual VhalConnector* vhal_connector() { return &vhal_connector_; }

  /**
   * @brief Query the platform for its ready status
   *
   * A platform is supposed to have different status during initialization
   *
   * @return true if the platform is fully initialized, otherwise false.
   */
  virtual bool ready() const = 0;

  /**
   * @brief Wait for platform plugin to be initialized.
   *
   * This function allows the caller to wait until the plugin platform has fully initialized all internal components.
   * Once the platform plugin reports it is ready Anbox will start the Android container.
   *
   * @return 0 on success, a negative error code otherwise.
   */
  virtual int wait_until_ready() = 0;

  /**
   * @brief Retrieve the configuration options provided by platform plugin.
   *
   * This function provides a way for anbox to read a configuration option from the plugin.
   * @param key plugin configuration option key.
   * @param data pointer stores the address of the configuration key value.
   * @param data_size size of the memory the `data` pointer points to
   * @return 0 on success, a negative error code otherwise. Possible are -EINVAL
   * (invalid arguments supplied) and -ENOMEM (provided memory is not large enough
   * to store the value of the configuration item).
   */
  virtual int get_config_item(AnboxPlatformConfigurationKey key, void* data, size_t data_size) = 0;

  /**
   * @brief Ask the platform to stop any pending work it has to prepare for Anbox to terminate.
   *
   * Anbox will wait for the function to return before it finishes its termination process. The
   * function is called very early on before the Android container is stopped.
   *
   * The platform is expected to stop its processors and don't provide any further data to
   * Anbox. Any further data provided to Anbox after the function has returned will not be
   * considered.
   *
   * @return 0 on success, a negative error code otherwise. Possible is -EINVAL (invalid arguments
   * supplied).
   */
  virtual int stop() { return 0; }

  /**
   * @brief Handle an event sending from Anbox
   *
   * Some operations (E.g. trigger an action) or initialization routine of a platform may
   * require Android container fully booted or all essential components of Anbox are fully initialized.
   *
   * This enables a platform to get notified when an event was fired from Anbox and perform
   * one specific operation when a certain event is received.
   */
  virtual void handle_event(AnboxEventType type) { (void)type; }

  /**
   * @brief Set the configuration options by Anbox to the platform.
   *
   * This function provides a way for anbox to write a configuration option to the plugin.
   *
   * @param key plugin configuration option key.
   * @param data pointer stores the address of the configuration key value.
   * @param data_size size of the memory the `data` pointer points to
   * @return 0 on success, a negative error code otherwise. Possible are -EINVAL
   * (invalid arguments supplied) and -ENOMEM (provided memory is not large enough
   * to store the value of the configuration item).
   */
  virtual int set_config_item(AnboxPlatformConfigurationKey key, void* data, size_t data_size) {
    (void)key;
    (void)data;
    (void)data_size;
    return 0;
  }

  /**
   * @brief Register an external event tracing implementation withe platform.
   *
   * @param get_category_enabled_callback Callback the platform can use to determine if a certain tracing category is enabled
   * @param add_event_callback  Callback to submit a tracing event to the Anbox runtime
   */
  virtual void setup_event_tracer(
    AnboxTracerGetCategoryEnabledFunc get_category_enabled_callback,
    AnboxTracerAddEventFunc add_event_callback) {
    (void) get_category_enabled_callback;
    (void) add_event_callback;
  }

 private:
  AnboxProxy  anbox_proxy_;
  VhalConnector vhal_connector_;
};
}

/**
 * @brief A descriptor that describes the basic information of the platform plugin.
 *
 * This macro enables registering and unregistering a platform plugin, adding an additional
 * section which allows Anbox to mainly verify platform API version at plugin compiling time.
 * It comes with a few necessary plugin information too, such as the vendor name.
 * Every platform implementation has to instantiate a platform descriptor which declares the
 * implemented platform plugin and describes the following things.
 *     - name: The name of the platform
 *     - vendor: The vendor name of the platform.
 *     - description: The description of the platform.
 *
 **/
#define ANBOX_PLATFORM_PLUGIN_DESCRIBE(platform_type, name, vendor, description) \
  AnboxPlatformDescriptor anbox_platform_descriptor __attribute((section(ANBOX_PLATFORM_DESCRIPTOR_SECTION))) = \
    { name, vendor, description, ANBOX_PLATFORM_VERSION }; \
extern "C" { \
  ANBOX_EXPORT AnboxPlatform* anbox_initialize(const AnboxPlatformConfiguration* configuration) { \
    auto platform = std::make_unique<platform_type>(configuration); \
    return anbox_platform_plugin_register(std::move(platform)); \
  } \
  ANBOX_EXPORT void anbox_deinitialize(AnboxPlatform* platform) { \
      anbox_platform_plugin_unregister(platform); \
  } \
}
#endif
