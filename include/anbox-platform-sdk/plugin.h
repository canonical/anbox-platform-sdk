// Anbox - The Android in a Box runtime environment
// Copyright 2018 Canonical Ltd.  All rights reserved.

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

struct AnboxProxy {
  anbox::AnboxProxy* instance{nullptr};
};

struct AnboxPlatform {
  std::unique_ptr<anbox::Platform> instance{nullptr};
  AnboxAudioProcessor audio_processor;
  AnboxInputProcessor input_processor;
  AnboxSensorProcessor sensor_processor;
  AnboxGpsProcessor gps_processor;
  AnboxGraphicsProcessor graphics_processor;
  AnboxProxy anbox_proxy;
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
