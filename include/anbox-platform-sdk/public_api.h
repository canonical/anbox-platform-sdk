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
 * @brief Retrieve the platform camera processor instance.
 *
 * Every platform implementation has to export a symbol named
 * 'anbox_platform_get_camera_processor' implementing the AnboxPlatformGetCameraProcessorFunc
 * function prototype.
 *
 **/
typedef const AnboxCameraProcessor* (*AnboxPlatformGetCameraProcessorFunc)(const AnboxPlatform* platform);

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
 * @brief Create a new video decoder for the given codec
 *
 * Every platform implementation has to export a symbol named
 * 'anbox_platform_create_video_decoder' implementing the AnboxPlatformCreateVideoDecoder
 * function prototype.
 */
typedef AnboxVideoDecoder* (*AnboxPlatformCreateVideoDecoderFunc)(const AnboxPlatform* platform, AnboxVideoCodecType codec_type);

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
 * @brief Set the configuration options by Anbox to the platform
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::Platform::set_config_item
 *
 **/
typedef int (*AnboxPlatformSetConfigItemFunc)(const AnboxPlatform* platform,
                                              AnboxPlatformConfigurationKey key,
                                              void* data, size_t data_size);

/**
 * @brief Register an external event tracing implementation with the platform
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::Platform::setup_tevent_tracer
 *
 **/
typedef void (*AnboxPlatformSetupEventTracerFunc)(const AnboxPlatform* platform,
                                                  AnboxTracerGetCategoryEnabledFunc get_category_enabled_func,
                                                  AnboxTracerAddEventFunc add_event_func);

/**
 * @brief Ask the platform to stop any pending work it has to prepare for Anbox to terminate.
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::Platform::stop
 *
 **/
typedef int (*AnboxPlatformStopFunc)(const AnboxPlatform* platform);

/**
 * @brief Handle an event fired from Anbox by a platform
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::Platform::handle_event
 *
 **/
typedef void (*AnboxPlatformHandleEventFunc)(const AnboxPlatform* platform, AnboxEventType type);

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
 * @brief Notify the platform when an audio stream is in activation mode.
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::AudioProcessor::activate
 *
 **/
typedef int (*AnboxAudioProcessorActivateFunc)(const AnboxAudioProcessor* audio_processor,
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
 * @brief Create an EGL display
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::GraphicsProcessor::create_display
 **/
typedef EGLDisplay (*AnboxGraphicsProcessorCreateDisplayFunc)(const AnboxGraphicsProcessor* graphics_processor);

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
 * @brief Present the given buffer to a display or other output
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::GraphicsProcessor::present
 */
typedef bool (*AnboxGraphicsProcessorPresentFunc)(const AnboxGraphicsProcessor* graphics_processor,
                                                  AnboxGraphicsBuffer* buffer,
                                                  AnboxCallback* callback);

/**
 * @brief Present the given buffer to a display or other output
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::GraphicsProcessor::present
 */
typedef bool (*AnboxGraphicsProcessorPresent2Func)(const AnboxGraphicsProcessor* graphics_processor,
                                                  AnboxGraphicsBuffer2* buffer,
                                                  AnboxCallback* callback);

/*
 * @brief Create a new graphics buffer
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::GraphicsProcessor::create_buffer
 */
typedef bool (*AnboxGraphicsProcessorCreateBufferFunc)(const AnboxGraphicsProcessor* graphics_processor,
                                                       uint32_t width, uint32_t height, uint32_t format,
                                                       uint32_t usage, AnboxGraphicsBuffer2** buffer);


/*
 * @brief Set the vsync callback
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::GraphicsProcessor::set_vsync_callback
 */
typedef bool (*AnboxGraphicsProcessorSetVsyncCallbackFunc)(
  const AnboxGraphicsProcessor* graphics_processor,
  const AnboxVsyncCallback& callback, void* user_data);

/**
 * @brief Sensors supported by the platform
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::SensorProcessor::supported_sensors
 *
 **/
typedef AnboxSensorType (*AnboxSensorProcessorSupportedSensorsFunc)(const AnboxSensorProcessor* sensor_processor);

/**
 * @brief Activate or deactivate a specific sensor
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::SensorProcessor::activate_sensor
 *
 **/
typedef int (*AnboxSensorProcessorActivateSensorFunc)(const AnboxSensorProcessor* sensor_processor,
                                                      const AnboxSensorType type,
                                                      bool on);


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
 * @brief Send a message from Anbox to the platform
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::AnboxProxy::send_message
 *
 **/
typedef int (*AnboxProxySendMessageFunc)(const AnboxProxy* anbox_proxy,
                                         const char* type,
                                         size_t type_size,
                                         const char* data,
                                         size_t data_size);

/**
 * @brief Allows the platform to trigger an action within the Android system
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::AnboxProxy::set_trigger_action_callback
 *
 **/
typedef int (*AnboxProxySetTriggerActionCallbackFunc)(const AnboxProxy* proxy,
                                                      const AnboxTriggerActionCallback& callback,
                                                      void* user_data);

/**
 * @brief Allows the platform to create ADB connection
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::AnboxProxy::set_create_adb_connection_callback
 *
 **/
typedef int (*AnboxProxySetCreateAdbConnectionCallbackFunc)(const AnboxProxy* proxy,
                                                            const AnboxCreateADBConnectionCallback& callback,
                                                            void* user_data);

/**
 * @brief Allows the platform to disconnect ADB connection
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::AnboxProxy::set_disconnect_adb_connection_callback
 *
 **/
typedef int (*AnboxProxySetDisconnectAdbConnectionCallbackFunc)(const AnboxProxy* proxy,
                                                                const AnboxDisconnectADBConnectionCallback& callback,
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

/**
 * @brief Open a camera device.
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::CameraProcessor::get_device_specs
 *
 **/
typedef int (*AnboxCameraProcessorGetDeviceSpecsFunc)(const AnboxCameraProcessor* camera_processor,
                                                      AnboxCameraSpec** specs,
                                                      size_t *length);

/**
 * @brief Open a camera device.
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::CameraProcessor::open_device
 *
 **/
typedef int (*AnboxCameraProcessorOpenDeviceFunc)(const AnboxCameraProcessor* camera_processor,
                                                  AnboxCameraSpec spec,
                                                  AnboxCameraOrientation orientation);

/**
 * @brief Close a camera device.
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::CameraProcessor::close_device
 *
 **/
typedef int (*AnboxCameraProcessorCloseDeviceFunc)(const AnboxCameraProcessor* camera_processor);


/**
 * @brief Read next available video frame.
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::CameraProcessor::read_frame
 *
 **/
typedef int (*AnboxCameraProcessorReadFrameFunc)(const AnboxCameraProcessor* camera_processor,
                                                 AnboxVideoFrame* frame,
                                                 int timeout);

/**
 * @brief Inject a video frame into AnboxPlatform
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::CameraProcessor::inject_frame
 *
 **/
typedef int (*AnboxCameraProcessorInjectFrameFunc)(const AnboxCameraProcessor* camera_processor,
                                                   AnboxVideoFrame frame);

/*
 * @brief Release the video decoder instance
 **/
typedef int (*AnboxVideoDecoderReleaseFunc)(AnboxVideoDecoder* decoder);

/*
 * @brief Configure the video decoder with the given spec
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::VideoDecoder::configure
 *
 **/
typedef int (*AnboxVideoDecoderConfigureFunc)(const AnboxVideoDecoder* decoder, AnboxVideoDecoderConfig config);

/*
 * @brief Flush any pending work the video decoder may have
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::VideoDecoder::flush
 *
 **/
typedef int (*AnboxVideoDecoderFlushFunc)(const AnboxVideoDecoder* decoder);

/*
 * @brief Submit the given frame to the video decoder
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::VideoDecoder::decode_frame
 *
 **/
typedef uint64_t (*AnboxVideoDecoderDecodeFrameFunc)(const AnboxVideoDecoder* decoder, const AnboxVideoFrame* frame, uint64_t pts);

/*
 * @brief Retrieve a decoded image from the decoder
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::VideoDecoder::retrieve_image
 *
 **/
typedef int (*AnboxVideoDecoderRetrieveImageFunc)(const AnboxVideoDecoder* decoder, AnboxVideoImage *img);

/**
 * @brief Retrieve the platform vhal connector instance.
 *
 * Every platform implementation has to export a symbol named
 * 'anbox_platform_get_vhal_connector' implementing the
 * AnboxPlatformGetVhalConnector function prototype.
 */
typedef AnboxVhalConnector* (*AnboxPlatformGetVhalConnectorFunc)(const AnboxPlatform* platform);

/**
 * @brief Allows the platform to call Android VHAL functions through Anbox.
 *
 * The function prototype for C API function which stands for
 * the C++ method of anbox::AnboxVhalConnector::set_callbacks
 *
 **/
typedef int (*AnboxVhalConnectorSetCallbacksFunc)(const AnboxVhalConnector* connector,
                                                  const AnboxVhalConnectorCallbacks* callbacks,
                                                  void* user_data);

#endif
