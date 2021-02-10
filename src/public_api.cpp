// Anbox - The Android in a Box runtime environment
// Copyright 2018 Canonical Ltd.  All rights reserved.

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
  if (!platform->instance)
    return false;
  return platform->instance->ready();
}

ANBOX_EXPORT int anbox_platform_wait_until_ready(const AnboxPlatform* platform) {
  if (!platform->instance)
    return -EINVAL;
  return platform->instance->wait_until_ready();
}

ANBOX_EXPORT int anbox_platform_get_config_item(const AnboxPlatform* platform,
                                                AnboxPlatformConfigurationKey key,
                                                void* data, size_t data_size) {
  if (!platform->instance)
    return -EINVAL;
  return platform->instance->get_config_item(key, data, data_size);
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

ANBOX_EXPORT size_t anbox_audio_processor_process_data(const AnboxAudioProcessor* audio_processor,
                                                       const uint8_t* data,
                                                       size_t size) {
  if (!audio_processor || !audio_processor->instance)
    return 0;
  return audio_processor->instance->process_data(data, size);
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
} // extern "C"
