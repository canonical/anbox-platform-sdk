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

#ifndef ANBOX_SDK_CAMERA_PROCESSOR_H_
#define ANBOX_SDK_CAMERA_PROCESSOR_H_

#include "anbox-platform-sdk/types.h"

#include <errno.h>
#include <stdint.h>
#include <stddef.h>

namespace anbox {
/**
 * @brief CameraProcessor allows a plugin to respond to the camera actions triggered
 * from Anobx and post video frames to Android container after a camera is open up
 * and display in camera preview for the picture taken ad video recording.
 */
class CameraProcessor {
  public:
    CameraProcessor() = default;
    virtual ~CameraProcessor() = default;
    CameraProcessor(const CameraProcessor &) = delete;
    CameraProcessor& operator=(const CameraProcessor &) = delete;

    /**
     * @brief Get camera device specifications of the platform.
     *
     * @param specs pointer to the list of camera specifications
     * @param specs_len the length of camera specifications
     * @return 0 on success, otherwise returns EINVAL on error occurs.
     **/
    virtual int get_device_specs(AnboxCameraSpec** specs, size_t *specs_len) {
      (void)specs;
      (void)specs_len;
      return -EINVAL;
    }

    /**
     * @brief Open the camera device and being ready for posting video frames to Android container
     *
     * @param video_resolution the resolution of camera video frame
     * @param orientation the currrent camera orientation in Android container
     * @return 0 on success, otherwise returns EINVAL on error occurs.
     **/
    virtual int open_device(AnboxCameraSpec spec, AnboxCameraOrientation orientation) {
      (void)spec;
      (void)orientation;
      return -EINVAL;
    }

    /**
     * @brief Close the camera device for not receiving video frames from the platform further
     * @return 0 on success, otherwise returns EINVAL on error occurs.
     **/
    virtual int close_device() {
      return -EINVAL;
    }

    /**
     * @brief Read next available video frame.
     *
     * Anbox will call read_frame() to query the plugin for the next available
     * video frame which is then forwarded to the Android container. If no video
     * frame  is available and \a timeout is set to -1, the function must block until
     * the next video frame is  available.
     *
     * @param frame Pointer to the available video frame  to be sent to the anbox container.
     * @param timeout maximum number of milliseconds to wait for the next available frame.
     * The following possible values for \a timeout must be handled:
     * | \a timeout  | Behavior |
     * | ------------- | ------------- |
     * | 0   | Non-blocking mode; return -EIO immediately if no video frame  to process. |
     * | < 0 | Block indefinitely until a video frame  is available. |
     * | > 0 | Wait up to a maximum of \a timeout milliseconds for a video frame. |
     */
    virtual int read_frame(AnboxVideoFrame* frame, int timeout) {
      (void) frame;
      (void) timeout;
      return -EIO;
    }

    /**
     * @brief Inject a video frame into AnboxPlatform.
     *
     * This function allows injecting a video frame into the platform and letting
     * the CameraProcessor handle the video frame.
     *
     * @param frame a video frame to be pushed into the internal queue.
     * @return 0 on success, otherwise returns EINVAL on error occurs.
     * @note This function is only used in our test suite to facilitate our automation
     *       tests and it is subject to change at any time.
     **/
    virtual int inject_frame(AnboxVideoFrame frame) {
      (void) frame;
      return -EIO;
    }
};
} // namespace anbox

#endif
