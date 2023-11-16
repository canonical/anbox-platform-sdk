/*
 * This file is part of Anbox Platform SDK
 *
 * Copyright 2022 Canonical Ltd.
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

#include <stdexcept>
#include <string.h>
#include <memory>

namespace anbox {

class DirectRenderingAudioProcessor : public AudioProcessor {
 public:
  DirectRenderingAudioProcessor() {}
  ~DirectRenderingAudioProcessor() override = default;

  size_t process_data(const uint8_t* data, size_t size) override;
};

size_t DirectRenderingAudioProcessor::process_data(const uint8_t* data, size_t size) {
  (void) data;
  (void) size;

  return 0;
}

class DirectRenderingInputProcessor : public InputProcessor {
  public:
    DirectRenderingInputProcessor() {}
    ~DirectRenderingInputProcessor() override = default;

    int read_event(AnboxInputEvent* event, int timeout) override;
    int inject_event(AnboxInputEvent event) override;
};

int DirectRenderingInputProcessor::read_event(AnboxInputEvent* event, int timeout = -1) {
  (void) event;
  (void) timeout;

  return 0;
}

int DirectRenderingInputProcessor::inject_event(AnboxInputEvent event) {
  (void) event;

  return 0;
}

class DirectRenderingGraphicsProcessor : public GraphicsProcessor {
 public:
  DirectRenderingGraphicsProcessor() {}
  ~DirectRenderingGraphicsProcessor() override = default;

  bool present(AnboxGraphicsBuffer* buffer, AnboxCallback* callback) override {
    // Each fully rendered and composited frame will be forwarded to the graphics processor
    // for further handling. When finished processing the supplied buffer the provided
    // callback needs to be called to return ownership of the buffer to Anbox

    // We don't do anything with the buffer here so return it back to Anbox
    if (callback && callback->callback)
      callback->callback(callback->user_data);

    return true;
  }
};

class DirectRendering : public anbox::Platform {
 public:
  DirectRendering(const AnboxPlatformConfiguration* configuration) :
    audio_processor_(std::make_unique<DirectRenderingAudioProcessor>()),
    input_processor_(std::make_unique<DirectRenderingInputProcessor>()),
    graphics_processor_(std::make_unique<DirectRenderingGraphicsProcessor>()) {
      (void) configuration;
    }
  ~DirectRendering() override = default;


  AudioProcessor* audio_processor() override;
  InputProcessor* input_processor() override;
  GraphicsProcessor* graphics_processor() override;
  bool ready() const override;
  int wait_until_ready() override;
  int get_config_item(AnboxPlatformConfigurationKey key, void* data, size_t data_size) override;

 private:
  AnboxDisplaySpec2 display_spec_ = {1280, 720, 160, 60};
  AnboxAudioSpec audio_spec_ = {48000, AUDIO_FORMAT_PCM_16_BIT, 2, 4096};
  const std::unique_ptr<DirectRenderingAudioProcessor> audio_processor_;
  const std::unique_ptr<DirectRenderingInputProcessor> input_processor_;
  const std::unique_ptr<DirectRenderingGraphicsProcessor> graphics_processor_;
};

AudioProcessor* DirectRendering::audio_processor() {
  return audio_processor_.get();
}

InputProcessor* DirectRendering::input_processor() {
  return input_processor_.get();
}

GraphicsProcessor* DirectRendering::graphics_processor() {
  return graphics_processor_.get();
}

bool DirectRendering::ready() const {
  return true;
}

int DirectRendering::wait_until_ready() {
  return 0;
}

int DirectRendering::get_config_item(AnboxPlatformConfigurationKey key, void* data, size_t data_size) {
  if (!data)
    return -EINVAL;

  switch (key) {
  case DISPLAY_SPEC2: {
    if (data_size != sizeof(AnboxDisplaySpec2))
      return -ENOMEM;

    auto spec = reinterpret_cast<AnboxDisplaySpec2*>(data);
    memcpy(spec, &display_spec_, sizeof(AnboxDisplaySpec2));
    break;
  }
  case AUDIO_SPEC: {
    if (data_size != sizeof(AnboxAudioSpec))
      return -ENOMEM;

    auto spec = reinterpret_cast<AnboxAudioSpec*>(data);
    memcpy(spec, &audio_spec_, sizeof(AnboxAudioSpec));
    break;
  }
  case GRAPHICS_IMPLEMENTATION_TYPE: {
    AnboxGraphicsImplementationType* type = reinterpret_cast<AnboxGraphicsImplementationType*>(data);
    *type = ANBOX_GRAPHICS_IMPLEMENTATION_TYPE_DIRECT_RENDERING;
    break;
  }
  case DIRECT_GRAPHICS_CONFIGURATION: {
    AnboxDirectGraphicsConfiguration* cfg = reinterpret_cast<AnboxDirectGraphicsConfiguration*>(data);

    // Value set here will be passed to Android as value for the ro.hardware.egl property
    // which will tell Android which EGL/GL driver to choose. In this case it will select
    // libEGL_mesa.so as available in /vendor/lib64/egl/libEGL_mesa.so for example.
    snprintf(cfg->gl_vendor, MAX_NAME_LENGTH, "mesa");

    // Value set here will be passed to Android as the value of the ro.hardware.gralloc
    // property which will tell Android which gralloc implementation to use. In this case
    // Android will load /vendor/lib64/hw/gralloc.anbox.so
    snprintf(cfg->gralloc_vendor, MAX_NAME_LENGTH, "anbox");

    // The following declare what GL/Vulkan version the selected driver supports
    cfg->gl_version = ANBOX_GRAPHICS_OPENGL_ES_VERSION_3_2;
    cfg->vulkan_version = ANBOX_GRAPHICS_VULKAN_VERSION_UNSUPPORTED;
    break;
  }
  default:
    return -EINVAL;
  }

  return 0;
}
} // namespace anbox

ANBOX_PLATFORM_PLUGIN_DESCRIBE(anbox::DirectRendering, "direct_rendering", "Canonical", "A direct rendering platform plugin")
