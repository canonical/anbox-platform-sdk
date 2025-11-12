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

extern "C" {
ANBOX_EXPORT const AnboxAudioProcessor* anbox_platform_get_audio_processor(const AnboxPlatform* platform) {
  if (!platform || !platform->audio_processor.instance)
    return nullptr;
  return &platform->audio_processor;
}

ANBOX_EXPORT const AnboxInputProcessor* anbox_platform_get_input_processor(const AnboxPlatform* platform) {
  if (!platform || !platform->input_processor.instance)
    return nullptr;
  return &platform->input_processor;
}

ANBOX_EXPORT const AnboxGraphicsProcessor* anbox_platform_get_graphics_processor(const AnboxPlatform* platform) {
  if (!platform || !platform->graphics_processor.instance)
    return nullptr;
  return &platform->graphics_processor;
}

ANBOX_EXPORT bool anbox_platform_ready(const AnboxPlatform* platform) {
  if (!platform || !platform->instance)
    return false;
  return platform->instance->ready();
}

ANBOX_EXPORT int anbox_platform_wait_until_ready(const AnboxPlatform* platform) {
  if (!platform || !platform->instance)
    return -EINVAL;
  return platform->instance->wait_until_ready();
}

ANBOX_EXPORT int anbox_platform_get_config_item(const AnboxPlatform* platform,
                                                AnboxPlatformConfigurationKey key,
                                                void* data, size_t data_size) {
  if (!platform || !platform->instance)
    return -EINVAL;
  return platform->instance->get_config_item(key, data, data_size);
}

ANBOX_EXPORT int anbox_platform_set_config_item(const AnboxPlatform* platform,
                                                AnboxPlatformConfigurationKey key,
                                                void* data, size_t data_size) {
  if (!platform || !platform->instance)
    return -EINVAL;
  return platform->instance->set_config_item(key, data, data_size);
}

ANBOX_EXPORT void anbox_platform_setup_event_tracer(const AnboxPlatform* platform,
                                                    AnboxTracerGetCategoryEnabledFunc get_category_enabled_callback,
                                                    AnboxTracerAddEventFunc add_event_callback) {
  if (!platform || !platform->instance)
    return;

  platform->instance->setup_event_tracer(get_category_enabled_callback, add_event_callback);
}

ANBOX_EXPORT int anbox_platform_stop(const AnboxPlatform* platform) {
  if (!platform->instance)
    return -EINVAL;

  return platform->instance->stop();
}

ANBOX_EXPORT void anbox_platform_handle_event(const AnboxPlatform* platform,
                                              AnboxEventType type) {
  if (!platform->instance)
    return;

  platform->instance->handle_event(type);
}

ANBOX_EXPORT AnboxVideoDecoder* anbox_platform_create_video_decoder(const AnboxPlatform* platform,
                                                                    AnboxVideoCodecType codec_type) {
  if (!platform || !platform->instance)
    return nullptr;

  std::unique_ptr<anbox::VideoDecoder> decoder(platform->instance->create_video_decoder(codec_type));
  if (!decoder)
    return nullptr;

  return new AnboxVideoDecoder{std::move(decoder)};
}

ANBOX_EXPORT size_t anbox_audio_processor_process_data(const AnboxAudioProcessor* audio_processor,
                                                       const uint8_t* data,
                                                       size_t size) {
  if (!audio_processor || !audio_processor->instance)
    return 0;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
  return audio_processor->instance->process_data(data, size);
#pragma GCC diagnostic pop
}

ANBOX_EXPORT ssize_t anbox_audio_processor_write_data(const AnboxAudioProcessor* audio_processor,
                                                      const uint8_t* data,
                                                      size_t size) {
  if (!audio_processor || !audio_processor->instance)
    return 0;
  return audio_processor->instance->write_data(data, size);
}

ANBOX_EXPORT ssize_t anbox_audio_processor_read_data(const AnboxAudioProcessor* audio_processor,
                                                     uint8_t* data,
                                                     size_t size) {
  if (!audio_processor || !audio_processor->instance)
    return 0;
  return audio_processor->instance->read_data(data, size);
}

ANBOX_EXPORT int anbox_audio_processor_activate(const AnboxAudioProcessor* audio_processor,
                                                AnboxAudioStreamType type) {
  if (!audio_processor || !audio_processor->instance)
    return -EINVAL;
  return audio_processor->instance->activate(type);
}

ANBOX_EXPORT int anbox_audio_processor_standby(const AnboxAudioProcessor* audio_processor,
                                               AnboxAudioStreamType type) {
  if (!audio_processor || !audio_processor->instance)
    return -EINVAL;
  return audio_processor->instance->standby(type);
}

ANBOX_EXPORT bool anbox_audio_processor_need_silence_on_standby(const AnboxAudioProcessor* audio_processor) {
  if (!audio_processor || !audio_processor->instance)
    return false;
  return audio_processor->instance->need_silence_on_standby();
}

ANBOX_EXPORT int anbox_input_processor_read_event(const AnboxInputProcessor* input_processor,
                                                  AnboxInputEvent* event,
                                                  int timeout) {
  if (!input_processor || !input_processor->instance)
    return -EINVAL;
  return input_processor->instance->read_event(event, timeout);
}

ANBOX_EXPORT int anbox_input_processor_inject_event(const AnboxInputProcessor* input_processor,
                                                    AnboxInputEvent event) {
  if (!input_processor || !input_processor->instance)
    return -EINVAL;
  return input_processor->instance->inject_event(event);
}

ANBOX_EXPORT int anbox_graphics_processor_initialize(const AnboxGraphicsProcessor* graphics_processor,
                                                     AnboxGraphicsConfiguration* configuration) {
  if (!graphics_processor || !graphics_processor->instance)
    return -EINVAL;
  return graphics_processor->instance->initialize(configuration);
}

ANBOX_EXPORT EGLDisplay anbox_graphics_processor_create_display(const AnboxGraphicsProcessor* graphics_processor) {
  if (!graphics_processor || !graphics_processor->instance)
    return EGL_NO_DISPLAY;
  return graphics_processor->instance->create_display();
}

ANBOX_EXPORT void anbox_graphics_processor_begin_frame(const AnboxGraphicsProcessor* graphics_processor) {
  if (!graphics_processor || !graphics_processor->instance)
    return;

  graphics_processor->instance->begin_frame();
}

ANBOX_EXPORT void anbox_graphics_processor_finish_frame(const AnboxGraphicsProcessor* graphics_processor) {
  if (!graphics_processor || !graphics_processor->instance)
    return;

  graphics_processor->instance->finish_frame();
}

ANBOX_EXPORT EGLSurface anbox_graphics_processor_create_offscreen_surface(const AnboxGraphicsProcessor* graphics_processor,
                                                                    EGLDisplay display,
                                                                    EGLConfig config,
                                                                    const EGLint* attribs) {
  if (!graphics_processor || !graphics_processor->instance)
    return EGL_NO_SURFACE;
  return graphics_processor->instance->create_offscreen_surface(display, config, attribs);
}

ANBOX_EXPORT bool anbox_graphics_processor_destroy_offscreen_surface(const AnboxGraphicsProcessor* graphics_processor,
                                                                     EGLDisplay display,
                                                                     EGLSurface surface) {
  if (!graphics_processor || !graphics_processor->instance)
    return false;
  return graphics_processor->instance->destroy_offscreen_surface(display, surface);
}

ANBOX_EXPORT bool anbox_graphics_processor_present(const AnboxGraphicsProcessor* graphics_processor,
                                                   AnboxGraphicsBuffer* buffer,
                                                   AnboxCallback* callback) {
  if (!graphics_processor || !graphics_processor->instance)
    return false;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
  return graphics_processor->instance->present(buffer, callback);
#pragma GCC diagnostic pop
}

ANBOX_EXPORT bool anbox_graphics_processor_present2(const AnboxGraphicsProcessor* graphics_processor,
                                                    AnboxGraphicsBuffer2* buffer,
                                                    AnboxCallback* callback) {
  if (!graphics_processor || !graphics_processor->instance)
    return false;

  return graphics_processor->instance->present(buffer, callback);
}

ANBOX_EXPORT bool anbox_graphics_processor_create_buffer(const AnboxGraphicsProcessor* graphics_processor,
                                                         uint32_t width, uint32_t height, uint32_t format,
                                                         uint32_t usage, AnboxGraphicsBuffer2** buffer) {
  if (!graphics_processor || !graphics_processor->instance)
    return false;

  return graphics_processor->instance->create_buffer(width, height, format, usage, buffer);
}

ANBOX_EXPORT void anbox_graphics_processor_set_vsync_callback(
  const AnboxGraphicsProcessor* graphics_processor,
  const AnboxVsyncCallback& callback, void* user_data) {
  if (!graphics_processor || !graphics_processor->instance)
    return;
  return graphics_processor->instance->set_vsync_callback(callback, user_data);
}

ANBOX_EXPORT const AnboxSensorProcessor* anbox_platform_get_sensor_processor(const AnboxPlatform* platform) {
  if (!platform || !platform->sensor_processor.instance)
    return nullptr;
  return &platform->sensor_processor;
}

ANBOX_EXPORT AnboxSensorType anbox_sensor_processor_supported_sensors(const AnboxSensorProcessor* sensor_processor) {
  if (!sensor_processor || !sensor_processor->instance)
    return AnboxSensorType::NONE;
  return sensor_processor->instance->supported_sensors();
}

ANBOX_EXPORT int anbox_sensor_processor_activate_sensor(const AnboxSensorProcessor* sensor_processor,
                                                        const AnboxSensorType type, bool on) {
  if (!sensor_processor || !sensor_processor->instance)
    return AnboxSensorType::NONE;
  return sensor_processor->instance->activate_sensor(type, on);
}

ANBOX_EXPORT int anbox_sensor_processor_read_data(const AnboxSensorProcessor* sensor_processor,
                                                  AnboxSensorData* data,
                                                  int timeout) {
  if (!sensor_processor || !sensor_processor->instance)
    return -EINVAL;
  return sensor_processor->instance->read_data(data, timeout);
}

ANBOX_EXPORT int anbox_sensor_processor_inject_data(const AnboxSensorProcessor* sensor_processor,
                                                    AnboxSensorData data) {
  if (!sensor_processor || !sensor_processor->instance)
    return -EINVAL;
  return sensor_processor->instance->inject_data(data);
}

ANBOX_EXPORT const AnboxGpsProcessor* anbox_platform_get_gps_processor(const AnboxPlatform* platform) {
  if (!platform || !platform->gps_processor.instance)
    return nullptr;
  return &platform->gps_processor;
}

ANBOX_EXPORT int anbox_gps_processor_read_data(const AnboxGpsProcessor* gps_processor,
                                               AnboxGpsData* data,
                                               int timeout) {
  if (!gps_processor || !gps_processor->instance) {
    return -EINVAL;
  }
  return gps_processor->instance->read_data(data, timeout);
}

ANBOX_EXPORT int anbox_gps_processor_inject_data(const AnboxGpsProcessor* gps_processor,
                                                 AnboxGpsData data) {
  if (!gps_processor || !gps_processor->instance)
    return -EINVAL;
  return gps_processor->instance->inject_data(data);
}

ANBOX_EXPORT const AnboxCameraProcessor* anbox_platform_get_camera_processor(const AnboxPlatform* platform) {
  if (!platform || !platform->camera_processor.instance)
    return nullptr;
  return &platform->camera_processor;
}

ANBOX_EXPORT int anbox_camera_processor_get_device_specs(const AnboxCameraProcessor* camera_processor,
                                                         AnboxCameraSpec** specs,
                                                         size_t *length) {
  if (!camera_processor || !camera_processor->instance)
    return -EINVAL;
  return camera_processor->instance->get_device_specs(specs, length);
}

ANBOX_EXPORT int anbox_camera_processor_open_device(const AnboxCameraProcessor* camera_processor,
                                                    AnboxCameraSpec spec,
                                                    AnboxCameraOrientation orientation) {
  if (!camera_processor || !camera_processor->instance)
    return -EINVAL;
  return camera_processor->instance->open_device(spec, orientation);
}

ANBOX_EXPORT int anbox_camera_processor_close_device(const AnboxCameraProcessor* camera_processor) {
  if (!camera_processor || !camera_processor->instance)
    return -EINVAL;
  return camera_processor->instance->close_device();
}

ANBOX_EXPORT int anbox_camera_processor_read_frame(const AnboxCameraProcessor* camera_processor,
                                                   AnboxVideoFrame* frame,
                                                   int timeout) {
  if (!camera_processor || !camera_processor->instance)
    return -EINVAL;
  return camera_processor->instance->read_frame(frame, timeout);
}

ANBOX_EXPORT int anbox_camera_processor_inject_frame(const AnboxCameraProcessor* camera_processor,
                                                     AnboxVideoFrame frame) {
  if (!camera_processor || !camera_processor->instance)
    return -EINVAL;
  return camera_processor->instance->inject_frame(frame);
}

ANBOX_EXPORT const AnboxProxy* anbox_platform_get_anbox_proxy(const AnboxPlatform* platform) {
  if (!platform || !platform->anbox_proxy.instance)
    return nullptr;
  return &platform->anbox_proxy;
}

ANBOX_EXPORT int anbox_proxy_set_change_screen_orientation_callback(const AnboxProxy* anbox_proxy,
                                                                    const AnboxChangeScreenOrientationCallback& callback,
                                                                    void* user_data) {
  if (!anbox_proxy || !anbox_proxy->instance)
    return -EINVAL;
  anbox_proxy->instance->set_change_screen_orientation_callback(callback, user_data);
  return 0;
}

ANBOX_EXPORT int anbox_proxy_set_change_display_density_callback(const AnboxProxy* anbox_proxy,
                                                                 const AnboxChangeDisplayDensityCallback& callback,
                                                                 void* user_data) {
  if (!anbox_proxy || !anbox_proxy->instance)
    return -EINVAL;
  anbox_proxy->instance->set_change_display_density_callback(callback, user_data);
  return 0;
}

ANBOX_EXPORT int anbox_proxy_set_change_display_size_callback(const AnboxProxy* anbox_proxy,
                                                              const AnboxChangeDisplaySizeCallback& callback,
                                                              void* user_data) {
  if (!anbox_proxy || !anbox_proxy->instance)
    return -EINVAL;
  anbox_proxy->instance->set_change_display_size_callback(callback, user_data);
  return  0;
}

ANBOX_EXPORT int anbox_proxy_send_message(const AnboxProxy* anbox_proxy,
                                          const char* type,
                                          size_t type_size,
                                          const char* data,
                                          size_t data_size) {
  if (!anbox_proxy || !anbox_proxy->instance)
    return -EINVAL;
  return anbox_proxy->instance->send_message(type, type_size, data, data_size);
}

ANBOX_EXPORT int anbox_proxy_set_trigger_action_callback(const AnboxProxy* anbox_proxy,
                                                         const AnboxTriggerActionCallback& callback,
                                                         void* user_data) {
  if (!anbox_proxy || !anbox_proxy->instance)
    return -EINVAL;
  anbox_proxy->instance->set_trigger_action_callback(callback, user_data);
  return  0;
}

ANBOX_EXPORT int anbox_proxy_set_create_adb_connection_callback(const AnboxProxy* anbox_proxy,
                                                                const AnboxCreateADBConnectionCallback& callback,
                                                                void* user_data) {
  if (!anbox_proxy || !anbox_proxy->instance)
    return -EINVAL;
  anbox_proxy->instance->set_create_adb_connection_callback(callback, user_data);
  return  0;
}

ANBOX_EXPORT int anbox_proxy_set_disconnect_adb_connection_callback(const AnboxProxy* anbox_proxy,
                                                                    const AnboxDisconnectADBConnectionCallback& callback,
                                                                    void* user_data) {
  if (!anbox_proxy || !anbox_proxy->instance)
    return -EINVAL;
  anbox_proxy->instance->set_disconnect_adb_connection_callback(callback, user_data);
  return  0;
}

ANBOX_EXPORT int anbox_video_decoder_release(AnboxVideoDecoder* decoder) {
  if (!decoder || !decoder->instance)
    return -EINVAL;

  decoder->instance.reset();
  delete decoder;
  return 0;
}

ANBOX_EXPORT int anbox_video_decoder_configure(const AnboxVideoDecoder* decoder, AnboxVideoDecoderConfig config) {
  if (!decoder || !decoder->instance)
    return -EINVAL;

  return decoder->instance->configure(config);
}

ANBOX_EXPORT int anbox_video_decoder_flush(const AnboxVideoDecoder* decoder) {
  if (!decoder || !decoder->instance)
    return -EINVAL;

  return decoder->instance->flush();
}

ANBOX_EXPORT uint64_t anbox_video_decoder_decode_frame(const AnboxVideoDecoder* decoder, const AnboxVideoFrame* frame, uint64_t pts) {
  if (!decoder || !decoder->instance)
    return 0;

  return decoder->instance->decode_frame(frame, pts);
}

ANBOX_EXPORT int anbox_video_decoder_retrieve_image(const AnboxVideoDecoder* decoder, AnboxVideoImage* img) {
  if (!decoder || !decoder->instance)
    return -EINVAL;

  return decoder->instance->retrieve_image(img);
}

ANBOX_EXPORT const AnboxVhalConnector* anbox_platform_get_vhal_connector(const AnboxPlatform* platform) {
  if (!platform || !platform->vhal_connector.instance)
    return nullptr;
  return &platform->vhal_connector;
}

ANBOX_EXPORT int anbox_vhal_connector_set_callbacks(const AnboxVhalConnector* connector,
                                                    const AnboxVhalConnectorCallbacks& callbacks,
                                                    void* user_data) {
  if (!connector || !connector->instance)
    return -EINVAL;
  connector->instance->set_callbacks(callbacks, user_data);
  return 0;
}

} // extern "C"
