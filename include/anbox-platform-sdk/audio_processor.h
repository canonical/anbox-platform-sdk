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

#ifndef ANBOX_SDK_AUDIO_PROCESSOR_H_
#define ANBOX_SDK_AUDIO_PROCESSOR_H_

#include "anbox-platform-sdk/types.h"

#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include <errno.h>

namespace anbox {
/**
 * @brief AudioProcessor allows processing audio data from the Android container
 * and perform audio processing like audio playback and recording according to different platforms.
 */
class AudioProcessor {
  public:
    AudioProcessor() = default;
    virtual ~AudioProcessor() = default;
    AudioProcessor(const AudioProcessor &) = delete;
    AudioProcessor& operator=(const AudioProcessor &) = delete;

    /**
     * @brief Process a chunk of audio data
     *
     * This function allows processing a single chunk of audio data.
     * The audio data is passed by directly from the Android container to Anbox for audio playback
     *
     * @param data Pointer to the chunk of audio data that is passed from the anbox container.
     * @param size the number of bytes to be processed.
     * @return The number of bytes actually processed on success.
     */
    [[deprecated("Replaced by AudioProcessor::write_data")]]
    virtual size_t process_data(const uint8_t* data, size_t size) = 0;

    /**
     * @brief Write a chunk of audio data
     *
     * This function allows writing a single chunk of audio data.
     * The audio data is passed by directly from the Android container to Anbox for audio playback
     *
     * @param data Pointer to the chunk of audio data that is passed from the anbox container.
     * @param size the number of bytes to be written.
     * @return The number of bytes actually written on success, otherwise returns a negative value on error
     */
    virtual ssize_t write_data(const uint8_t* data, size_t size) {
      (void) data;
      (void) size;
      return -EIO;
    }

    /**
     * @brief Read a chunk of audio data
     *
     * This function allows reading a single chunk of audio data.
     * The audio data is passed by directly from Anbox to the Android container for audio recording
     *
     * @param data Pointer to the chunk of audio data that is passed from the Anbox.
     * @param size the number of bytes to be read.
     * @return The number of bytes actually read, otherwise returns a negative value on error
     */
    virtual ssize_t read_data(uint8_t* data, size_t size) {
      (void) data;
      (void) size;
      return -EIO;
    }

    /**
     * @brief Notify the platform when an audio stream is in standby mode
     *
     * This function will be called when an audio stream enters standby mode.
     * If a platform is notified by Anbox that one audio stream is going to standby mode,
     * it's always good to drain the audio buffer cached on the platform side since this
     * reduces the accumulated latency efficiently.
     *
     * @param type the specified audio stream type.
     * @return 0 on success or negative on failure.
     */
    virtual int standby(AnboxAudioStreamType type) {
      (void) type;
      return 0;
    }

    /**
     * @brief Notify the platform when an audio stream is in activation mode
     *
     * This function will be called when an audio stream is in activation mode.
     * If a platform is notified by Anbox that one audio stream is going to activation mode,
     *
     * @param type the specified audio stream type.
     * @return 0 on success or negative on failure.
     */
    virtual int activate(AnboxAudioStreamType type) {
      (void) type;
      return 0;
    }

    /**
     * @brief Produce a silent audio stream on need while the audio output stream goes into the standby state
     *
     * Anbox will produce a silent audio stream while the audio output stream goes into
     * the standby state if this function returns true, otherwise returns false.
     * Producing a silent audio stream helps with the audio latency reduction when doing audio
     * playback if no audio output device is installed on a platform.
     *
     * @return true when needs to produce a silent audio stream while audio output stream goes into the
     * standby state, otherwise return false.
     */
    virtual bool need_silence_on_standby() const {
      return false;
    }
};
} // namespace anbox

#endif
