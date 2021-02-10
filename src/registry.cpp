// Anbox - The Android in a Box runtime environment
// Copyright 2018 Canonical Ltd.  All rights reserved.

#include "anbox-platform-sdk/plugin.h"

AnboxPlatform* anbox_platform_plugin_register(std::unique_ptr<anbox::Platform>&& platform) {
  auto anbox_platform = new AnboxPlatform;
  anbox_platform->audio_processor.instance = platform->audio_processor();
  anbox_platform->input_processor.instance = platform->input_processor();
  anbox_platform->graphics_processor.instance = platform->graphics_processor();
  anbox_platform->sensor_processor.instance = platform->sensor_processor();
  anbox_platform->anbox_proxy.instance = platform->anbox_proxy();
  anbox_platform->gps_processor.instance = platform->gps_processor();
  anbox_platform->camera_processor.instance = platform->camera_processor();
  anbox_platform->instance = std::move(platform);
  return anbox_platform;
}

void anbox_platform_plugin_unregister(AnboxPlatform* platform) {
  if (!platform)
    return;

  if (platform->instance)
    platform->instance.reset();

  delete platform;
}
