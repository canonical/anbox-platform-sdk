// Anbox - The Android in a Box runtime environment
// Copyright 2018 Canonical Ltd.  All rights reserved.

#ifndef ANBOX_PLATFORM_SDK_PLATFORM_H_
#define ANBOX_PLATFORM_SDK_PLATFORM_H_

#include "anbox-platform-sdk/common.h"
#include "anbox-platform-sdk/version.h"
#include "anbox-platform-sdk/audio_format.h"
#include "anbox-platform-sdk/audio_processor.h"
#include "anbox-platform-sdk/input_processor.h"
#include "anbox-platform-sdk/sensor_processor.h"
#include "anbox-platform-sdk/gps_processor.h"
#include "anbox-platform-sdk/graphics_processor.h"
#include "anbox-platform-sdk/anbox_proxy.h"

#include <stdint.h>
#include <stddef.h>
#include <linux/input.h>

#include <EGL/egl.h>

#define MAX_STRING_LENGTH 256

/**
 * @brief A particular elf section that Anbox needs various information
 * from the platform plugin to appear in.
 */
#define ANBOX_PLATFORM_DESCRIPTOR_SECTION ".anbox_platform_descriptor_section"

/**
 * @brief AnboxDisplaySpec describes properties of the virtual
 * display Anbox creates for rendering.
 */
typedef struct {
  /**
   * Width of the display
   *
   * Needs to be greater than `0`.
   **/
  uint32_t width;

  /**
   * Height of the display
   *
   * Needs to be greater than `0`.
   **/
  uint32_t height;

  /**
   * Density of the display.
   *
   * Takes the same values as defined in https://developer.android.com/reference/android/util/DisplayMetrics.html
   *
   * If `0` is supplied a default density of `160` will be used.
   **/
  uint32_t density;
} AnboxDisplaySpec;

/**
 * @brief AnboxDisplaySpec2 describes properties of the virtual
 * display Anbox creates for rendering.
 */
typedef struct {
  /**
   * Width of the display
   *
   * Needs to be greater than `0`.
   **/
  uint32_t width;

  /**
   * Height of the display
   *
   * Needs to be greater than `0`.
   **/
  uint32_t height;

  /**
   * Density of the display.
   *
   * Takes the same values as defined in https://developer.android.com/reference/android/util/DisplayMetrics.html
   *
   * If `0` is supplied a default density of `160` will be used.
   **/
  uint32_t density;

  /**
   * Target frame rate of the display.
   *
   * Defaults to 60
   **/
  uint32_t fps;
} AnboxDisplaySpec2;


/**
 * @brief The audio input/output format from anbox.
 *
 *  E.g. AnboxAudioSpec spec{44100, AUDIO_FORMAT_PCM_16_BIT, 2, 4096};
 *       The above spec indicates the audio data is sampled 44100 times per second,
 *       16-bit pcm audio format, two channel with 4kb buffer size in samples.
 */
struct AnboxAudioSpec {
  /** The number of samples of audio per second. */
  uint32_t freq;
  /** The audio data format. */
  AnboxAudioFormat format;
  /** The number of audio signal channels. */
  uint8_t channels;
  /** The audio buffer size in samples. */
  uint16_t samples;
};

/**
 * @brief AnboxPlatformConfigurationKey specifies configuration items which
 * allow to influence the behavior and configuration of Anbox.
 */
typedef enum {
  /**
   * Path to the platform implementation of the EGL shared library.
   *
   * Providing a valid path is mandatory.
   *
   * The value of this configuration item is of type `const char*`.
   */
  EGL_DRIVER_PATH = 0,

  /**
   * Path to the platform implementation of the OpenGL ES 1.x CM shared library.
   *
   * Providing a valid path is mandatory.
   *
   * The value of this configuration item is of type `const char*`.
   */
  OPENGL_ES1_CM_DRIVER_PATH = 1,

  /**
   * Path to the platform implementation of the OpenGL ES 2.x shared library.
   *
   * Providing a valid path is mandatory.
   *
   * The value of this configuration item is of type `const char*`.
   */
  OPENGL_ES2_DRIVER_PATH = 2,

  /**
   * Specification of parameters of the virtual display Anbox creates.
   *
   * Internally Anbox will setup a virtual display for Android to render onto.
   * The specification provided with this configuration item details the
   * dimension, density and other relevant parameters of the the created virtual
   * display.
   *
   * The value of this configuration is of type `AnboxDisplaySpec`
   * @note: deprecated, it exists only for backward compatibility. Please use DISPLAY_SPEC2 instead.
   */
  DISPLAY_SPEC = 3,

  /**
   * Specification of parameters of the audio output Anbox creates.
   *
   * Internally Anbox will setup audio server for Android to connect to.
   * The specification provided with this configuration item details the
   * audio format, channel, sample rate and other relevant parameters of
   * the created audio server.
   *
   * The value of this configuration is of type `AnboxAudioSpec`
   * @note: deprecated, it exists only for backward compatibility. Please use AUDIO_OUTPUT_SPEC instead.
   */
  AUDIO_SPEC = 4,

   /**
   * Specification of parameters of the virtual display Anbox creates.
   *
   * Internally Anbox will setup a virtual display for Android to render onto.
   * The specification provided with this configuration item details the
   * dimension, density and other relevant parameters of the the created virtual
   * display.
   *
   * The value of this configuration is of type `AnboxDisplaySpec2`
   */
  DISPLAY_SPEC2 = 5,

  /**
   * Specification of parameters of the audio input Anbox creates.
   *
   * Internally Anbox will setup audio server for Android to connect to.
   * The specification provided with this configuration item details the
   * audio format, channel, sample rate and other relevant parameters of
   * the created audio server.
   *
   * The value of this configuration is of type `AnboxAudioSpec`
   */
  AUDIO_INPUT_SPEC = 6,
} AnboxPlatformConfigurationKey;

/**
 * @brief AnboxPlatformConfiguration holds platform configuration options which
 * Anbox supplies to the platform implementation at initialization time.
 */
struct AnboxPlatformConfiguration {
};

/**
 * @brief AnboxPlatformDescriptor provides information about the implemented platform.
 */
struct AnboxPlatformDescriptor {
  /** The name of the platform plugin. */
  const char name[MAX_STRING_LENGTH];
  /** The name of the vendor implementing the plugin. */
  const char vendor[MAX_STRING_LENGTH];
  /** The description of the platform plugin. */
  const char description[MAX_STRING_LENGTH];
  /** The Anbox platform API version. */
  uint32_t platform_version;
};

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
   * @brief Retrieve the platform anbox proxy.
   *
   * A platform is supposed to have only a single anbox proxy instance at all time.
   *
   * @return a valid AnboxProxy instance, otherwise NULL when an error occured.
   * or the platform does not support to handle the system behavior change via callbacks.
   */
  virtual AnboxProxy* anbox_proxy() { return &anbox_proxy_; }

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

 private:
  AnboxProxy  anbox_proxy_;
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
