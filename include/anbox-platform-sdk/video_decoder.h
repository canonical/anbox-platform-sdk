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

#ifndef ANBOX_SDK_VIDEO_DECODER_H_
#define ANBOX_SDK_VIDEO_DECODER_H_

#include "anbox-platform-sdk/camera_processor.h"

namespace anbox {
/**
 * @brief Provides access to a video decoder which will be used by both Anbox and the Android
 * instance for hardware accelerated video decoding.
 */
class VideoDecoder {
 public:
  VideoDecoder() = default;
  virtual ~VideoDecoder() = default;
  VideoDecoder(const VideoDecoder &) = delete;
  VideoDecoder& operator=(const VideoDecoder &) = delete;

  /**
   * @brief Configure the video decoder with the given spec
   *
   * @param config configuration for the video decoder
   * @return 0 on success, an error code otherwise
   */
  virtual int configure(const AnboxVideoDecoderConfig& config) = 0;

  /**
   * @brief Flush all pending frames to the decoder for decoding
   *
   * @return 0 on success, an error code otherwise
   */
  virtual int flush() = 0;

  /**
   * @brief Decode the given frame with its presentation timestamp
   *
   * @param frame the frame to decode
   * @param pts presentation timestamp of the frame in milliseconds
   * @return Number of bytes submitted to the decoder
   */
  virtual uint64_t decode_frame(const AnboxVideoFrame* frame, int64_t pts) = 0;

  /**
   * @brief Retrieve the latest decoded image from the video decoder
   *
   * @param img structure holding information about the returned image
   * @return 0 on success, an error code otherwise
   */
  virtual int retrieve_image(AnboxVideoImage* img) = 0;
};
} // namespace anbox

#endif
