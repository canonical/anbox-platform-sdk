// Anbox - The Android in a Box runtime environment
// Copyright 2020 Canonical Ltd.  All rights reserved.

#ifndef ANBOX_SDK_CAMERA_PROCESSOR_H_
#define ANBOX_SDK_CAMERA_PROCESSOR_H_

#include <errno.h>
#include <stdint.h>
#include <stddef.h>

/**
* @brief AnboxVideoColorFormat describes the color space format of a video frame.
*/
typedef enum {
 /** Unknown color space format */
 VIDEO_FRAME_FORMAT_UNKNOWN = 0,
 /** YUV standard format 4:2:0 */
 VIDEO_FRAME_FORMAT_YUV420,
 /** RAW color format with alpha channel*/
 VIDEO_FRAME_FORMAT_RGBA,
} AnboxVideoColorSpaceFormat;

/**
 * @brief describes the orientations that Anbox supports in the camea processor
 */
typedef enum {
  /** Camera in portrait mode */
  CAMERA_ORIENTATION_PORTRAIT = 0,
  /** Camera in landscape mode */
  CAMERA_ORIENTATION_LANDSCAPE = 1,
} AnboxCameraOrientation;

/**
 * @brief describes the camera facing mode
 */
typedef enum {
  /** Camera in front facing mode */
  CAMERA_FACING_MODE_FRONT = 0,
  /** Camera in rear facing mode */
  CAMERA_FACING_MODE_REAR = 1,
} AnboxCameraFacingMode;

/**
 * @brief AnboxCameraSpec represents the camera specification
 */
struct AnboxCameraSpec {
 /** Color space format of camera frame data */
 AnboxVideoColorSpaceFormat format;
 /** Camera facing mode */
 AnboxCameraFacingMode facing_mode;
 /** Camera video frame frequency */
 uint32_t fps;
 /** The width of the camera resolution */
 uint32_t width;
 /** The height  of the camera resolution */
 uint32_t height;
};

/**
* @brief AnboxVideoFrame represents a single complete video frame
*/
struct AnboxVideoFrame {
 /** Pointer to video frame data */
 uint8_t* data;
 /** Pointer to video frame data */
 size_t size;
};

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
