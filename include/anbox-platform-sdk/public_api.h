// Anbox - The Android in a Box runtime environment
// Copyright 2018 Canonical Ltd.  All rights reserved.

#ifndef ANBOX_SDK_INTERNAL_PUBLIC_API_H_
#define ANBOX_SDK_INTERNAL_PUBLIC_API_H_

#include "anbox-platform-sdk/platform.h"

/**
 * @brief Initialize a platform instance.
 *
 * Every platform implementation has to export a symbol named
 * 'anbox_initialize' implementing the AnboxInitializePlatformFunc
 * function prototype.
 *
 **/
typedef AnboxPlatform* (*AnboxInitializePlatformFunc)(const AnboxPlatformConfiguration* configuration);

/**
 * @brief Release a platform instance.
 *
 * Every platform implementation has to export a symbol named
 * 'anbox_deinitialize' implementing the AnboxReleasePlatformFunc
 * function prototype.
 *
 **/
typedef void (*AnboxReleasePlatformFunc)(AnboxPlatform* platform);

/**
 * @brief Retrieve the platform audio processor instance.
 *
 * Every platform implementation has to export a symbol named
 * 'anbox_platform_get_audio_processor' implementing the AnboxPlatformGetAudioProcessorFunc
 * function prototype.
 *
 **/
typedef const AnboxAudioProcessor* (*AnboxPlatformGetAudioProcessorFunc)(const AnboxPlatform* platform);

/**
 * @brief Retrieve the platform input processor instance.
 *
 * Every platform implementation has to export a symbol named
 * 'anbox_platform_get_input_processor' implementing the AnboxPlatformGetInputProcessorFunc
 * function prototype.
 *
 **/
typedef const AnboxInputProcessor* (*AnboxPlatformGetInputProcessorFunc)(const AnboxPlatform* platform);

/**
 * @brief Retrieve the platform graphics processor instance.
 *
 * Every platform implementation has to export a symbol named
 * 'anbox_platform_get_graphics_processor' implementing the AnboxPlatformGetGraphicsProcessorFunc
 * function prototype.
 *
 **/
typedef const AnboxGraphicsProcessor* (*AnboxPlatformGetGraphicsProcessorFunc)(const AnboxPlatform* platform);

/**
 * @brief Retrieve the platform sensor processor instance.
 *
 * Every platform implementation has to export a symbol named
 * 'anbox_platform_get_sensor_processor' implementing the AnboxPlatformGetSensorProcessorFunc
 * function prototype.
 *
 **/
typedef const AnboxSensorProcessor* (*AnboxPlatformGetSensorProcessorFunc)(const AnboxPlatform* platform);

/**
 * @brief Retrieve the platform gps processor instance.
 *
 * Every platform implementation has to export a symbol named
 * 'anbox_platform_get_gps_processor' implementing the AnboxPlatformGetGpsProcessorFunc
 * function prototype.
 *
 **/
typedef const AnboxGpsProcessor* (*AnboxPlatformGetGpsProcessorFunc)(const AnboxPlatform* platform);

/**
 * @brief Retrieve the platform anbox proxy.
 *
 * Every platform implementation has to export a symbol named
 * 'anbox_platform_get_anbox_proxy' implementing the AnboxPlatformGetAnboxProxyFunc
 * function prototype.
 *
 **/
typedef const AnboxProxy* (*AnboxPlatformGetAnboxProxyFunc)(const AnboxPlatform* platform);

/**
 * @brief Query the platform for its ready status
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::Platform::ready
 *
 **/
typedef bool (*AnboxPlatformReadyFunc)(const AnboxPlatform* platform);

/**
 * @brief Wait for platform plugin to be initialized
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::Platform::wait_until_ready
 *
 **/
typedef int (*AnboxPlatformWaitUntilReadyFunc)(const AnboxPlatform* platform);

/**
 * @brief Retrieve the configuration options provided by platform plugin
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::Platform::get_config_item
 *
 **/
typedef int (*AnboxPlatformGetConfigItemFunc)(const AnboxPlatform* platform,
                                              AnboxPlatformConfigurationKey key,
                                              void* data, size_t data_size);

/**
 * @brief Ask the platform to stop any pending work it has to prepare for Anbox to terminate.
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::Platform::stop
 *
 **/
typedef int (*AnboxPlatformStopFunc)(const AnboxPlatform* platform);

/**
 * @brief Process a chunk of audio data.
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::AudioProcessor::process_data
 *
 **/
typedef size_t (*AnboxAudioProcessorProcessDataFunc)(const AnboxAudioProcessor* audio_processor,
                                                     const uint8_t* data,
                                                     size_t size);

/**
 * @brief Write a chunk of audio data.
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::AudioProcessor::write_data
 *
 **/
typedef ssize_t (*AnboxAudioProcessorWriteDataFunc)(const AnboxAudioProcessor* audio_processor,
                                                    const uint8_t* data,
                                                    size_t size);

/**
 * @brief Read a chunk of audio data.
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::AudioProcessor::read_data
 *
 **/
typedef ssize_t (*AnboxAudioProcessorReadDataFunc)(const AnboxAudioProcessor* audio_processor,
                                                   uint8_t* data,
                                                   size_t size);

/**
 * @brief Notify the platform when an audio stream is in standby mode.
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::AudioProcessor::standby
 *
 **/
typedef int (*AnboxAudioProcessorStandbyFunc)(const AnboxAudioProcessor* audio_processor,
                                              AnboxAudioStreamType type);

/**
 * @brief Produce a silent audio stream on need while the audio output stream goes into the standby state
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::AudioProcessor::need_silence_on_standby
 *
 **/
typedef bool (*AnboxAudioProcessorNeedSilenceOnStandbyFunc)(const AnboxAudioProcessor* audio_processor);

/**
 * @brief Read next available input event.
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::InputProcessor::read_event
 *
 **/
typedef int (*AnboxInputProcessorReadEventFunc)(const AnboxInputProcessor* input_processor,
                                                AnboxInputEvent* event,
                                                int timeout);

/**
 * @brief Inject an input event into AnboxPlatform
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::InputProcessor::inject_event
 *
 **/
typedef int (*AnboxInputProcessorInjectEventFunc)(const AnboxInputProcessor* input_processor,
                                                  AnboxInputEvent event);

/**
    * @brief Initialize the graphics processor
    *
    * The function prototype for C API function which stands for
    * the C++ method of anbox::GraphicsProcessor::initialize
    **/
typedef int (*AnboxGraphicsProcessorInitializeFunc)(const AnboxGraphicsProcessor* graphics_processor,
                                                    AnboxGraphicsConfiguration* configuration);

/**
 * @brief Begin a new frame
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::GraphicsProcessor::begin_frame
 *
 **/
typedef void (*AnboxGraphicsProcessorBeginFrameFunc)(const AnboxGraphicsProcessor* graphics_processor);

/**
 * @brief Finish the currently rendered frame
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::GraphicsProcessor::finish_frame
 *
 **/
typedef void (*AnboxGraphicsProcessorFinishFrameFunc)(const AnboxGraphicsProcessor* graphics_processor);

/**
 * @brief Create an offscreen EGL surface
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::GraphicsProcessor::create_offscreen_surface
 *
 **/
typedef EGLSurface (*AnboxGraphicsProcessorCreateOffscreenSurfaceFunc)(const AnboxGraphicsProcessor* graphics_processor,
                                                                       EGLDisplay display,
                                                                       EGLConfig config,
                                                                       const EGLint* attribs);

/**
 * @brief Destroy an offscreen EGL surface
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::GraphicsProcessor::destroy_offscreen_surface
 *
 **/
typedef bool (*AnboxGraphicsProcessorDestroyOffscreenSurfaceFunc)(const AnboxGraphicsProcessor* graphics_processor,
                                                                  EGLDisplay display,
                                                                  EGLSurface surface);

/**
 * @brief Sensors supported by the platform
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::SensorProcessor::supported_sensors
 *
 **/
typedef AnboxSensorType (*AnboxSensorProcessorSupportedSensorsFunc)(const AnboxSensorProcessor* sensor_processor);

/**
 * @brief Read next available sensor data.
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::SensorProcessor::read_data
 *
 **/
typedef int (*AnboxSensorProcessorReadDataFunc)(const AnboxSensorProcessor* sensor_processor,
                                                AnboxSensorData* data,
                                                int timeout);

/**
 * @brief Inject a sensor data into AnboxPlatform
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::SensorProcessor::inject_data
 *
 **/
typedef int (*AnboxSensorProcessorInjectDataFunc)(const AnboxSensorProcessor* sensor_processor,
                                                  AnboxSensorData data);

/**
 * @brief Set the change screen orientation callback function
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::AnboxProxy::set_change_screen_orientation_callback
 *
 **/
typedef int (*AnboxProxySetChangeScreenOrientationCallbackFunc)(const AnboxProxy* anbox_proxy,
                                                                const AnboxChangeScreenOrientationCallback& callback,
                                                                void* user_data);

/**
 * @brief Set the change display density callback function
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::AnboxProxy::set_change_display_density_callback
 *
 **/
typedef int (*AnboxProxySetChangeDisplayDensityCallbackFunc)(const AnboxProxy* anbox_proxy,
                                                             const AnboxChangeDisplayDensityCallback& callback,
                                                             void* user_data);

/**
 * @brief Set the change display size callback function
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::AnboxProxy::set_change_display_size_callback
 *
 **/
typedef int (*AnboxProxySetChangeDisplaySizeCallbackFunc)(const AnboxProxy* anbox_proxy,
                                                          const AnboxChangeDisplaySizeCallback& callback,
                                                          void* user_data);

/**
 * @brief Request Gps processor to start forwarding the GPS data to Android container.
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::GpsProcessor::start
 *
 **/
typedef int (*AnboxGpsProcessorStartFunc)(const AnboxGpsProcessor* gps_processor);

/**
 * @brief Request Gps processor to stop forwarding the GPS data to Android container
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::GpsProcessor::stop
 *
 **/
typedef int (*AnboxGpsProcessorStopFunc)(const AnboxGpsProcessor* gps_processor);

/**
 * @brief Read next available gps data.
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::GpsProcessor::read_data
 *
 **/
typedef int (*AnboxGpsProcessorReadDataFunc)(const AnboxGpsProcessor* gps_processor,
                                             AnboxGpsData* data,
                                             int timeout);

/**
 * @brief Inject a gps data into AnboxPlatform
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::SensorProcessor::inject_data
 *
 **/
typedef int (*AnboxGpsProcessorInjectDataFunc)(const AnboxGpsProcessor* gps_processor,
                                               AnboxGpsData data);

#endif
