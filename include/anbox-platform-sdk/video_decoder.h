// Anbox - The Android in a Box runtime environment
// Copyright 2021 Canonical Ltd.  All rights reserved.

#ifndef ANBOX_SDK_VIDEO_DECODER_H_
#define ANBOX_SDK_VIDEO_DECODER_H_

#include "anbox-platform-sdk/camera_processor.h"

#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include <errno.h>

#include <ostream>

/**
 * @brief AnboxVideoCodecType describes the type of a video codec
 */
typedef enum : uint8_t {
  ANBOX_VIDEO_CODEC_TYPE_UNKNOWN = 0,
  /* H.264 / AVC */
  ANBOX_VIDEO_CODEC_TYPE_H264 = 1,
} AnboxVideoCodecType;

/**
 * @brief AnboxVideoPixelFormat describes a pixel format
 */
typedef enum : uint8_t {
  ANBOX_VIDEO_PIXEL_FORMAT_UNKNOWN = 0,
  /* YUV420P */
  ANBOX_VIDEO_PIXEL_FORMAT_YUV420P = 1,
} AnboxVideoPixelFormat;

/**
 * @brief AnboxVideoDecoderConfig describes the configuration of a video decoder
 */
struct AnboxVideoDecoderConfig {
  /* Target output width */
  uint32_t width = 0;
  /* Target output height */
  uint32_t height = 0;
  /* Expected output pixel format */
  AnboxVideoPixelFormat output_format = ANBOX_VIDEO_PIXEL_FORMAT_UNKNOWN;
};

/**
 * @brief AnboxVideoImage describes a decoded image returned by the video decoder
 */
struct AnboxVideoImage {
  /* Pixel format of the image */
  AnboxVideoPixelFormat pixel_format = ANBOX_VIDEO_PIXEL_FORMAT_UNKNOWN;
  /* Width of the image */
  uint32_t width = 0;
  /* Height of the image */
  uint32_t height = 0;
  /* Presentation timestmap of the image in milliseconds */
  int64_t pts = 0;
  /* Color matrix coefficients of the image as defined in E.2.1 (VUI parameters semantics) of the H264 specification */
  uint8_t color_matrix = 0;
  /* Color primaries of the image as defined in E.2.1 (VUI parameters semantics) of the H264 specification */
  uint8_t color_primaries = 0;
  /* Color transfer of the image as defined in E.2.1 (VUI parameters semantics) of the H264 specification */
  uint8_t color_transfer = 0;
  /* Color range of the image as defined in E.2.1 (VUI parameters semantics) of the H264 specification */
  uint8_t color_range = 0;
  /* Size of the image data */
  uint64_t size = 0;
  /* Data of the image */
  uint8_t* data = nullptr;
};

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
