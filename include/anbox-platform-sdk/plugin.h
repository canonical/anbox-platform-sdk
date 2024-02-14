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

#ifndef ANBOX_PLATFORM_SDK_PLUGIN_H_
#define ANBOX_PLATFORM_SDK_PLUGIN_H_

#include "anbox-platform-sdk/platform.h"

#include <memory>

struct AnboxAudioProcessor {
  anbox::AudioProcessor* instance{nullptr};
};

struct AnboxInputProcessor {
  anbox::InputProcessor* instance{nullptr};
};

struct AnboxGraphicsProcessor {
  anbox::GraphicsProcessor* instance{nullptr};
};

struct AnboxSensorProcessor {
  anbox::SensorProcessor* instance{nullptr};
};

struct AnboxGpsProcessor {
  anbox::GpsProcessor* instance{nullptr};
};

struct AnboxCameraProcessor {
  anbox::CameraProcessor* instance{nullptr};
};

struct AnboxProxy {
  anbox::AnboxProxy* instance{nullptr};
};

struct AnboxVideoDecoder {
  std::unique_ptr<anbox::VideoDecoder> instance;
};

struct AnboxVhalConnector {
  anbox::VhalConnector* instance{nullptr};
};

struct AnboxPlatform {
  std::unique_ptr<anbox::Platform> instance{nullptr};
  AnboxAudioProcessor audio_processor;
  AnboxInputProcessor input_processor;
  AnboxSensorProcessor sensor_processor;
  AnboxGpsProcessor gps_processor;
  AnboxGraphicsProcessor graphics_processor;
  AnboxCameraProcessor camera_processor;
  AnboxProxy anbox_proxy;
  AnboxVhalConnector vhal_connector;
};

/**
 * @brief Register a platform plugin.
 *
 * every platform implementation has to register the implemented AnboxPlatform
 * instance, This function will helps to register all the required methods that
 * a platform plugin implemented.
 */
extern AnboxPlatform* anbox_platform_plugin_register(std::unique_ptr<anbox::Platform>&& platform);

/**
 * @brief Unregister a platform plugin.
 *
 * every platform implementation has to unregister the implemented AnboxPlatform
 * instance, This function will helps to register all the required methods that
 * a platform plugin implemented.

 */
extern void anbox_platform_plugin_unregister(AnboxPlatform*);

#endif
