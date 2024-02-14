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

AnboxPlatform* anbox_platform_plugin_register(std::unique_ptr<anbox::Platform>&& platform) {
  auto anbox_platform = new AnboxPlatform;
  anbox_platform->audio_processor.instance = platform->audio_processor();
  anbox_platform->input_processor.instance = platform->input_processor();
  anbox_platform->graphics_processor.instance = platform->graphics_processor();
  anbox_platform->sensor_processor.instance = platform->sensor_processor();
  anbox_platform->anbox_proxy.instance = platform->anbox_proxy();
  anbox_platform->gps_processor.instance = platform->gps_processor();
  anbox_platform->camera_processor.instance = platform->camera_processor();
  anbox_platform->vhal_connector.instance = platform->vhal_connector();
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
