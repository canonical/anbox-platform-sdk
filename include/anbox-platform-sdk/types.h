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

#ifndef ANBOX_SDK_TYPES_H_
#define ANBOX_SDK_TYPES_H_

#include <stdint.h>
#include <stddef.h>
#include <unistd.h>

#include <linux/limits.h>
#include <linux/input.h>

#include <EGL/egl.h>

#define MAX_NAME_LENGTH 100
#define MAX_STRING_LENGTH 256
#define MAX_VHAL_AREA_NAME_LENGTH 32

/**
 * @brief A particular elf section that Anbox needs various information
 * from the platform plugin to appear in.
 */
#define ANBOX_PLATFORM_DESCRIPTOR_SECTION ".anbox_platform_descriptor_section"

/**
 * @brief AnboxDisplaySpec describes properties of the virtual
 * display Anbox creates for rendering.
 */
typedef struct {
  /**
   * Width of the display
   *
   * Needs to be greater than `0`.
   **/
  uint32_t width;

  /**
   * Height of the display
   *
   * Needs to be greater than `0`.
   **/
  uint32_t height;

  /**
   * Density of the display.
   *
   * Takes the same values as defined in https://developer.android.com/reference/android/util/DisplayMetrics.html
   *
   * If `0` is supplied a default density of `160` will be used.
   **/
  uint32_t density;
} AnboxDisplaySpec;

/**
 * @brief AnboxDisplaySpec2 describes properties of the virtual
 * display Anbox creates for rendering.
 */
typedef struct {
  /**
   * Width of the display
   *
   * Needs to be greater than `0`.
   **/
  uint32_t width;

  /**
   * Height of the display
   *
   * Needs to be greater than `0`.
   **/
  uint32_t height;

  /**
   * Density of the display.
   *
   * Takes the same values as defined in https://developer.android.com/reference/android/util/DisplayMetrics.html
   *
   * If `0` is supplied a default density of `160` will be used.
   **/
  uint32_t density;

  /**
   * Target frame rate of the display.
   *
   * Defaults to 60
   **/
  uint32_t fps;
} AnboxDisplaySpec2;

/**
 * @brief Audio pcm sub formats.
 *
 * All of these are in native byte order.
 */
typedef enum {
    /** DO NOT CHANGE - PCM signed 16 bits. */
    AUDIO_FORMAT_PCM_SUB_16_BIT          = 0x1,
    /** DO NOT CHANGE - PCM unsigned 8 bits. */
    AUDIO_FORMAT_PCM_SUB_8_BIT           = 0x2,
    /** PCM signed .31 fixed point. */
    AUDIO_FORMAT_PCM_SUB_32_BIT          = 0x3,
    /** PCM signed 8.23 fixed point. */
    AUDIO_FORMAT_PCM_SUB_8_24_BIT        = 0x4,
    /** PCM single-precision floating point. */
    AUDIO_FORMAT_PCM_SUB_FLOAT           = 0x5,
    /** PCM signed .23 fixed point packed in 3 bytes. */
    AUDIO_FORMAT_PCM_SUB_24_BIT_PACKED   = 0x6,
} AnboxAudioPcmSubFormat;

/* The audio_format_*_sub_fmt_t declarations are not currently used */

/**
 * @brief Audio format is a 32-bit word that consists of:
 *   main format field (upper 8 bits)
 *   sub format field (lower 24 bits).
 *
 * The main format indicates the main codec type. The sub format field
 * indicates options and parameters for each format. The sub format is mainly
 * used for instance to indicate the requested bitrate or profile.
 * It can also be used for certain formats to give informations not present in
 * the encoded audio stream (e.g. octet alignement for AMR).
 */
typedef enum {
    /** Invalid Audio format. */
    AUDIO_FORMAT_INVALID             = 0xFFFFFFFFUL,
    /** Default Audio format. */
    AUDIO_FORMAT_DEFAULT             = 0,
    /* DO NOT CHANGE */
    /** PCM format, reserved*/
    AUDIO_FORMAT_PCM                 = 0x00000000UL,

    /* Aliases */
    /* note != AudioFormat.ENCODING_PCM_16BIT */
    /** PCM signed 16 bits. */
    AUDIO_FORMAT_PCM_16_BIT          = (AUDIO_FORMAT_PCM |
                                        AUDIO_FORMAT_PCM_SUB_16_BIT),
    /* note != AudioFormat.ENCODING_PCM_8BIT */
    /** PCM unsigned 8 bits. */
    AUDIO_FORMAT_PCM_8_BIT           = (AUDIO_FORMAT_PCM |
                                        AUDIO_FORMAT_PCM_SUB_8_BIT),
    /** PCM signed .31 fixed point. */
    AUDIO_FORMAT_PCM_32_BIT          = (AUDIO_FORMAT_PCM |
                                        AUDIO_FORMAT_PCM_SUB_32_BIT),
    /** PCM signed 8.23 fixed point. */
    AUDIO_FORMAT_PCM_8_24_BIT        = (AUDIO_FORMAT_PCM |
                                        AUDIO_FORMAT_PCM_SUB_8_24_BIT),
    /** PCM single-precision floating point. */
    AUDIO_FORMAT_PCM_FLOAT           = (AUDIO_FORMAT_PCM |
                                        AUDIO_FORMAT_PCM_SUB_FLOAT),
    /** PCM signed .23 fixed point packed in 3 bytes. */
    AUDIO_FORMAT_PCM_24_BIT_PACKED   = (AUDIO_FORMAT_PCM |
                                        AUDIO_FORMAT_PCM_SUB_24_BIT_PACKED),
} AnboxAudioFormat;

/**
 * @brief AnboxAudioStreamType describes the audio stream type
 */
typedef enum {
  /** Audio output stream  */
  AUDIO_OUTPUT_STREAM = 0,
  /** Audio input stream */
  AUDIO_INPUT_STREAM = 1,
} AnboxAudioStreamType;

/**
 * @brief The audio input/output format from anbox.
 *
 *  E.g. AnboxAudioSpec spec{44100, AUDIO_FORMAT_PCM_16_BIT, 2, 4096};
 *       The above spec indicates the audio data is sampled 44100 times per second,
 *       16-bit pcm audio format, two channel with 4kb buffer size in samples.
 */
struct AnboxAudioSpec {
  /** The number of samples of audio per second. */
  uint32_t freq;
  /** The audio data format. */
  AnboxAudioFormat format;
  /** The number of audio signal channels. */
  uint8_t channels;
  /** The audio buffer size in samples. */
  uint16_t samples;
};

/**
 * @brief The struct of binder devices that being used in Android container.
 */
struct AnboxBinderDevices {
  /** framework binder which is used for IPC between framework/app processes with AIDL interfaces */
  char framework[PATH_MAX];
  /** hardware binder which is used for IPC between framework/vendor processes with HIDL interfaces */
  char hardware[PATH_MAX];
  /** vendor binder which is used for IPC between vendor/vendor processes with AIDL Interfaces */
  char vendor[PATH_MAX];
};

/**
 * @brief Opaque handle
 */
typedef uintptr_t AnboxNativeHandle;

/**
 * @brief Pixel format used for the pixels stored in a AnboxGraphicsBuffer or AnboxGraphicsBuffer2
 */
typedef enum {
  /** Unknown pixel format */
  ANBOX_GRAPHICS_BUFFER_PIXEL_FORMAT_UNKNOWN = 0,
  /** 32 bit RGBA **/
  ANBOX_GRAPHICS_BUFFER_PIXEL_FORMAT_ARGB_8888 = 1,
  /** 32 bit RGBX **/
  ANBOX_GRAPHICS_BUFFER_PIXEL_FORMAT_XRGB_8888 = 2,
  /** 24 bit RGB (888) **/
  ANBOX_GRAPHICS_BUFFER_PIXEL_FORMAT_RGB_888 = 3,
  /** 24 bit RGB (565) **/
  ANBOX_GRAPHICS_BUFFER_PIXEL_FORMAT_RGB_565 = 4,
  /** 32 bit BGRA **/
  ANBOX_GRAPHICS_BUFFER_PIXEL_FORMAT_ABGR_8888 = 5,
  /** Floating point 64 bit ABGR **/
  ANBOX_GRAPHICS_BUFFER_PIXEL_FORMAT_ABGR_16161616F = 6,
  /** Floating point 64 bit ABGR **/
  ANBOX_GRAPHICS_BUFFER_PIXEL_FORMAT_RGBA_16F = ANBOX_GRAPHICS_BUFFER_PIXEL_FORMAT_ABGR_16161616F,
  /** 16 bit GR **/
  ANBOX_GRAPHICS_BUFFER_PIXEL_FORMAT_GR_88 = 7,
  /** 32 bit ABGR **/
  ANBOX_GRAPHICS_BUFFER_PIXEL_FORMAT_ABGR_2101010 = 8,
  /** 3 plane YCbCr **/
  ANBOX_GRAPHICS_BUFFER_PIXEL_FORMAT_YUV_420 = 9,
  /** 8 bit R **/
  ANBOX_GRAPHICS_BUFFER_PIXEL_FORMAT_R_8 = 10,
  /** 2 plane YCbCr **/
  ANBOX_GRAPHICS_BUFFER_PIXEL_FORMAT_NV_12 = 11,
  /** 32 bt XBGR **/
  ANBOX_GRAPHICS_BUFFER_PIXEL_FORMAT_XBGR_8888 = 12,
  /** 3 plane YCbCr */
  ANBOX_GRAPHICS_BUFFER_PIXEL_FORMAT_YVU_420 = 13,
} AnboxGraphicsBufferPixelFormat;

/**
 * Usage flags for graphics buffers
 */
typedef enum {
  ANBOX_GRAPHICS_BUFFER_USAGE_UNKNOWN = 0,
  /** Buffer is meant to be used for display scanout **/
  ANBOX_GRAPHICS_BUFFER_USAGE_SCANOUT = (1 << 0),
  /** Buffer is meant to be used for rendering **/
  ANBOX_GRAPHICS_BUFFER_USAGE_RENDERING = (1 << 1),
  /** Buffer can be mapped and written to from CPU **/
  ANBOX_GRAPHICS_BUFFER_USAGE_WRITE = (1 << 2),
  /** Buffer is linear, not tiled **/
  ANBOX_GRAPHICS_BUFFER_USAGE_LINEAR = (1 << 3),
} AnboxGraphicsBufferUsage;

/**
 * @brief Graphics buffer
 *
 * This version of the graphics buffer is deprecated and should no longer be used. Please
 * use AnboxGraphicsBuffer2 instead.
 */
typedef struct {
  /** Native handle pointing to the actual buffer. Passed through from Androids gralloc **/
  AnboxNativeHandle handle;
  /** Width of the buffer **/
  uint32_t width;
  /** Height of the buffer **/
  uint32_t height;
  /** Stride of the buffer **/
  uint32_t stride;
  /** Pixel format of the buffer. See AnboxGraphicsBufferPixelFormat **/
  uint32_t format;
} AnboxGraphicsBuffer;

/** Maximum number of planes a buffer can have **/
#define ANBOX_GRAPHICS_BUFFER_MAX_PLANES 4

/**
 * @brief Graphics buffer
 */
typedef struct {
  /** Width of the buffer **/
  uint32_t width;
  /** Height of the buffer **/
  uint32_t height;
  /** DRM color format of the buffer. See drm/drm_fourcc.h for a list of valid formats **/
  uint32_t format;
  /** GPU driver specific modifier describing the memory layout of the buffer **/
  uint64_t modifier;
  /** Number of planes the buffer has **/
  uint8_t num_planes;
  /** Native handle for a plane of the buffer **/
  AnboxNativeHandle handle[ANBOX_GRAPHICS_BUFFER_MAX_PLANES];
  /** Stride of a specific plane in the buffer in bytes **/
  uint32_t stride[ANBOX_GRAPHICS_BUFFER_MAX_PLANES];
  /** Offset inside the memory handle for the plane */
  uint32_t offset[ANBOX_GRAPHICS_BUFFER_MAX_PLANES];
} AnboxGraphicsBuffer2;

/**
 * @brief Generic callback wrapper
 */
typedef struct {
  /** Pointer to callback function to call **/
  void (*callback)(void* user_data);
  /** User data to pass to the callback **/
  void* user_data;
} AnboxCallback;

/**
 * @brief AnboxGraphicsImplementationType describes type of the graphics implementation the
 * platform provides
 */
typedef enum : uint8_t {
  /* Unknown graphics implementation */
  ANBOX_GRAPHICS_IMPLEMENTATION_TYPE_UNKNOWN = 0,
  /* Host rendering on the Anbox side */
  ANBOX_GRAPHICS_IMPLEMENTATION_TYPE_HOST_RENDERING = 1,
  /* Direct rendering inside the Android container */
  ANBOX_GRAPHICS_IMPLEMENTATION_TYPE_DIRECT_RENDERING = 2,
} AnboxGraphicsImplementationType;


/**
 * @brief AnboxGraphicsOpenGLESVersion describes a particular OpenGL ES API version
 */
typedef enum {
  /* OpenGL ES 2.0 */
  ANBOX_GRAPHICS_OPENGL_ES_VERSION_2_0 = 0,
  /* OpenGL ES 3.0 */
  ANBOX_GRAPHICS_OPENGL_ES_VERSION_3_0 = 1,
  /* OpenGL ES 3.1 */
  ANBOX_GRAPHICS_OPENGL_ES_VERSION_3_1 = 2,
  /* OpenGL ES 3.2 */
  ANBOX_GRAPHICS_OPENGL_ES_VERSION_3_2 = 3,
} AnboxGraphicsOpenGLESVersion;

/**
 * @brief AnboxGraphicsVulkanVersion describes a particular Vulkan API version
 */
typedef enum {
  /* Vulkan is not supported */
  ANBOX_GRAPHICS_VULKAN_VERSION_UNSUPPORTED = 0,
  /* Vulkan 1.0 */
  ANBOX_GRAPHICS_VULKAN_VERSION_1_0 = 1,
  /* Vulkan 1.1 */
  ANBOX_GRAPHICS_VULKAN_VERSION_1_1 = 2,
  /* Vulkan 1.2 */
  ANBOX_GRAPHICS_VULKAN_VERSION_1_2 = 3,
  /* Vulkan 1.3 */
  ANBOX_GRAPHICS_VULKAN_VERSION_1_3 = 4,
} AnboxGraphicsVulkanVersion;

/**
 * @brief AnboxDirectGraphicsConfiguration defines how Anbox should configure the
 * graphics implementation inside the Android container.
 *
 * This primarily contains names to be used to instruct Android which drivers
 * to load.
 *
 * @note Both a GL and a gralloc implementation MUST be provided. Providing
 * the name of a Vulkan implementation is optional.
 */
typedef struct {
  /*
   * Name of the vendor whichs OpenGL ES implementation Android should use. Value
   * will be set to the `ro.hardware.egl` Android system property.
   */
  char gl_vendor[MAX_NAME_LENGTH];

  /* Maximum OpenGL ES version supported, see AnboxGraphicsOpenGLESVersion */
  uint32_t gl_version;

  /*
   * Name of the vendor whichs Vulkan implementation Android should use. Value will
   * be set to the `ro.hardware.vulkan` Android system property.
   */
  char vulkan_vendor[MAX_NAME_LENGTH];

  /* Maximum Vulkan version supported, see AnboxGraphicsVulkanVersion */
  uint32_t vulkan_version;

  /*
   * Name of the gralloc implementation Android should use. Value will be set to the
   * `ro.hardware.gralloc` Android system property.
   */
  char gralloc_vendor[MAX_NAME_LENGTH];
} AnboxDirectGraphicsConfiguration;

/*
 * @brief Defines the type of the value stored for a configuration item described by
 * AnboxPlatformConfigurationItemInfo
 */
typedef enum {
  /* boolean value, represented as 8 bit unsigned integer type (uint8_t) where 0 = false and > 0 = true */
  BOOLEAN,
} AnboxPlatformConfigurationItemValueType;

/*
 * @brief AnboxPlatformConfigurationItemInfo describes a single platform configuration item
 *
 * It allows the platform to define a set of configuration items which will remain transparent
 * to Anbox but can be changed through it's /1.0/platform HTTP API endpoint at runtime.
 */
typedef struct {
  /* ID of the configuration item, must be >= PLATFORM_CONFIGURATION_ID_START and <= PLATFORM_CONFIGURATION_ID_END */
  int id;
  /* Name of the configuration item */
  char name[MAX_NAME_LENGTH];
  /* Type of the value the configuration item stores */
  AnboxPlatformConfigurationItemValueType type;
} AnboxPlatformConfigurationItemInfo;

/*
 * @brief AnboxPlatformConfigurationInfo describes a set of platform specific
 * configuration items.
 *
 * Anbox will query the platform for available configuration items and make
 * them accessible through its /1.0/platform HTTP API endpoint. This allows
 * runtime configuration of the platform.
 */
typedef struct {
  /* Number of available configuration items */
  uint16_t num_items;
  /* Available configuration items or NULL if none are available */
  AnboxPlatformConfigurationItemInfo **items;
} AnboxPlatformConfigurationInfo;

/**
 * @brief AnboxAndroidSystemProperties describes the Android system properties
 *        provided by a platform to be forwarded to the Android container.
 *
 * This enables the platform implmentation to provide required Android
 * system properties for itself to function within Anbox runtime.
 */
typedef struct {
  /* Number of Android system properties required by a platform*/
  uint16_t size;
  /* Array of Android system properties */
  const char** properties;
} AnboxAndroidSystemProperties;

/**
 * @brief AnboxPlatformConfigurationKey specifies configuration items which
 * allow to influence the behavior and configuration of Anbox.
 */
typedef enum {
  /**
   * Path to the platform implementation of the EGL shared library.
   *
   * Providing a valid path is mandatory.
   *
   * The value of this configuration item is of type `const char*`.
   */
  EGL_DRIVER_PATH = 0,

  /**
   * Path to the platform implementation of the OpenGL ES 1.x CM shared library.
   *
   * Providing a valid path is mandatory.
   *
   * The value of this configuration item is of type `const char*`.
   */
  OPENGL_ES1_CM_DRIVER_PATH = 1,

  /**
   * Path to the platform implementation of the OpenGL ES 2.x shared library.
   *
   * Providing a valid path is mandatory.
   *
   * The value of this configuration item is of type `const char*`.
   */
  OPENGL_ES2_DRIVER_PATH = 2,

  /**
   * Specification of parameters of the virtual display Anbox creates.
   *
   * Internally Anbox will setup a virtual display for Android to render onto.
   * The specification provided with this configuration item details the
   * dimension, density and other relevant parameters of the the created virtual
   * display.
   *
   * The value of this configuration item is of type `AnboxDisplaySpec`
   * @note: deprecated, it exists only for backward compatibility. Please use DISPLAY_SPEC2 instead.
   */
  DISPLAY_SPEC = 3,

  /**
   * Specification of parameters of the audio output Anbox creates.
   *
   * Internally Anbox will setup audio server for Android to connect to.
   * The specification provided with this configuration item details the
   * audio format, channel, sample rate and other relevant parameters of
   * the created audio server.
   *
   * The value of this configuration item is of type `AnboxAudioSpec`
   */
  AUDIO_SPEC = 4,

   /**
   * Specification of parameters of the virtual display Anbox creates.
   *
   * Internally Anbox will setup a virtual display for Android to render onto.
   * The specification provided with this configuration item details the
   * dimension, density and other relevant parameters of the the created virtual
   * display.
   *
   * The value of this configuration item is of type `AnboxDisplaySpec2`
   */
  DISPLAY_SPEC2 = 5,

  /**
   * Specification of parameters of the audio input Anbox creates.
   *
   * Internally Anbox will setup audio server for Android to connect to.
   * The specification provided with this configuration item details the
   * audio format, channel, sample rate and other relevant parameters of
   * the created audio server.
   *
   * The value of this configuration item is of type `AnboxAudioSpec`
   */
  AUDIO_INPUT_SPEC = 6,

  /**
   * Structure of path to binder devices used by the Android container.
   *
   * This is used by Anbox to notify the platform that all binder devices
   * are available by calling \sa anbox::Platform::set_config_item.
   *
   * The value of this configuration item is of type `AnboxBinderDevices`
   */
  BINDER_DEVICES = 7,

  /*
   * List of supported video codecs supported by the video decoder implemented
   * by the platform.
   *
   * Anbox will instruct it's HAL layer on the Android side to only exposed
   * support for the codecs listed.
   *
   * The value of this configuration item is of type AnboxVideoCodecType[]
   */
  SUPPORTED_VIDEO_DECODE_CODECS = 8,

  /*
   * The graphics implementation type defines how graphics acceleration will be
   * provided to the Android container.
   *
   * Anbox supports different ways of exposing graphic acceleration to the Android
   * container. Graphics acceleration can either be provided through a translation
   * layer where all access to the GPU happens on the Anbox side and Android has
   * no direct access to the GPU. Alternatively Android can get direct GPU access
   * and perform all graphics acceleration directly.
   *
   * If not provided by a platform implementation, Anbox will default to
   * ANBOX_GRAPHICS_IMPLEMENTATION_TYPE_HOST_RENDERING
   *
   * The value of this configuration item is of type `AnboxGraphicsImplementationType`
   */
  GRAPHICS_IMPLEMENTATION_TYPE = 9,

  /*
   * Configuration details for direct graphics acceleration
   *
   * The platform can further detail on how nativ graphics acceleration will be
   * made available inside the Android container. This allows defining GL/Vulkan
   * driver implementation names and other things.
   *
   * The value of this configuration item is of type `AnboxDirectGraphicsConfiguration`
   */
  DIRECT_GRAPHICS_CONFIGURATION = 10,

  /*
   * Information about platform specific configuration options
   *
   * The value of this configuration is of type AnboxPlatformConfigurationInfo
   */
  PLATFORM_CONFIGURATION_INFO = 11,

  /*
   * Base uid used in the Android contaienr
   *
   * The value of this configuration item is of type `uint32_t`.
   */
  CONTAINER_BASE_UID = 12,

  /*
   * Path to Vulkan ICD to be used for host side rendering
   *
   * The value of this configuration item is of type `const char*`
   */
  VULKAN_ICD_PATH = 13,

  /*
   * Path to the DRM render node Anbox should use for host side
   * rendering.
   *
   * The value of this configuration item is of type `const char*`
   */
  DRM_RENDER_NODE_PATH = 14,

  /*
   * Whether the platform requires support for sw sync primitives or not.
   *
   * The value of this configuration item is of type `uint8_t` which
   * represents a boolean value, encoded as 8 bit unsigned integer type
   * (uint8_t) where 0 = false and > 0 = true
   *
   * Not used anymore since 1.27
   */
  ENABLE_SW_SYNC_SUPPORT = 15,

  /*
   * Android system properties required to forward to Android container
   *
   * The value of this configuration item is of type `AnboxAndroidSystemProperties`
   */
  ANDROID_SYSTEM_PROPERTIES = 16,

  /*
   * The API defines a range of platform specific configuration items which can be
   * dynamically exposed by the platform. PLATFORM_CONFIGURATION_START specifies
   * the first configuration item id.
   */
  PLATFORM_CONFIGURATION_ID_START = 1000,

  /*
   * The API defines a range of platform specific configuration items which can be
   * dynamically exposed by the platform. PLATFORM_CONFIGURATION_END specifies
   * the last configuration item id.
   */
  PLATFORM_CONFIGURATION_ID_END = 1999,
} AnboxPlatformConfigurationKey;

/**
 * @brief AnboxPlatformConfiguration holds platform configuration options which
 * Anbox supplies to the platform implementation at initialization time.
 */
struct AnboxPlatformConfiguration {
  /** This API level is used at runtime by Android system **/
  uint32_t android_api_level;

  /*
   * A pointer to a null-terminated ('\0') string containing additional configuration
   * data used by the platform during initialization or NULL if no additional
   * configuration data is available.
   */
  const char* extra_config_data;
};

/**
 * @brief AnboxPlatformDescriptor provides information about the implemented platform.
 */
struct AnboxPlatformDescriptor {
  /** The name of the platform plugin. */
  const char name[MAX_STRING_LENGTH];
  /** The name of the vendor implementing the plugin. */
  const char vendor[MAX_STRING_LENGTH];
  /** The description of the platform plugin. */
  const char description[MAX_STRING_LENGTH];
  /** The Anbox platform API version. */
  uint32_t platform_version;
};

/**
 * @brief AnboxEventType describes the type of event sent from Anbox
 */
typedef enum {
  /** Android is fully booted */
  ANBOX_EVENT_TYPE_ANDROID_BOOT_FINISHED = 0,
  /** Anbox is fully initialized */
  ANBOX_EVENT_TYPE_INITIALIZATION_FINISHED,
  /** Anbox is terminating */
  ANBOX_EVENT_TYPE_TERMINATING,
} AnboxEventType;

/**
 * @brief AnboxScreenOrientationType describes the type of display orientation
 */
typedef enum {
  /** Portrait orientation mode */
  PORTRAIT = 0,
  /** Landscape orientation mode */
  LANDSCAPE,
  /** Reversed portrait orientation mode */
  PORTRAIT_REVERSED,
  /** Reversed landscape orientation mode */
  LANDSCAPE_REVERSED,
} AnboxScreenOrientationType;

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

#define GNSS_MAX_MEASUREMENT 64

/** Milliseconds since January 1, 1970 */
typedef int64_t GpsUtcTime;

/**
 * Constellation type of GnssSvInfo
 */
typedef enum : uint8_t {
  UNKNOWN = 0,
  GPS     = 1,
  SBAS    = 2,
  GLONASS = 3,
  QZSS    = 4,
  BEIDOU  = 5,
  GALILEO = 6,
} GnssConstellationType;

/**
 * Flags indicating the GNSS measurement state.
 *
 * The expected behavior here is for GPS HAL to set all the flags that applies.
 * For example, if the state for a satellite is only C/A code locked and bit
 * synchronized, and there is still millisecond ambiguity, the state should be
 * set as:
 *
 * GNSS_MEASUREMENT_STATE_CODE_LOCK | GNSS_MEASUREMENT_STATE_BIT_SYNC |
 *         GNSS_MEASUREMENT_STATE_MSEC_AMBIGUOUS
 *
 * If GNSS is still searching for a satellite, the corresponding state should be
 * set to GNSS_MEASUREMENT_STATE_UNKNOWN(0).
 */
typedef enum : uint32_t {
  STATE_UNKNOWN                = 0,
  STATE_CODE_LOCK              = 1 << 0,
  STATE_BIT_SYNC               = 1 << 1,
  STATE_SUBFRAME_SYNC          = 1 << 2,
  STATE_TOW_DECODED            = 1 << 3,
  STATE_MSEC_AMBIGUOUS         = 1 << 4,
  STATE_SYMBOL_SYNC            = 1 << 5,
  STATE_GLO_STRING_SYNC        = 1 << 6,
  STATE_GLO_TOD_DECODED        = 1 << 7,
  STATE_BDS_D2_BIT_SYNC        = 1 << 8,
  STATE_BDS_D2_SUBFRAME_SYNC   = 1 << 9,
  STATE_GAL_E1BC_CODE_LOCK     = 1 << 10,
  STATE_GAL_E1C_2ND_CODE_LOCK  = 1 << 11,
  STATE_GAL_E1B_PAGE_SYNC      = 1 << 12,
  STATE_SBAS_SYNC              = 1 << 13,
  STATE_TOW_KNOWN              = 1 << 14,
  STATE_GLO_TOD_KNOWN          = 1 << 15,
} GnssMeasurementState;

/**
 * Flags to indicate what fields in GnssClock are valid.
 */
typedef enum : uint16_t {
  /** A valid 'leap second' is stored in the data structure. */
  HAS_LEAP_SECOND        = 1 << 0,
  /** A valid 'time uncertainty' is stored in the data structure. */
  HAS_TIME_UNCERTAINTY   = 1 << 1,
  /** A valid 'full bias' is stored in the data structure. */
  HAS_FULL_BIAS          = 1 << 2,
  /** A valid 'bias' is stored in the data structure. */
  HAS_BIAS               = 1 << 3,
  /** A valid 'bias uncertainty' is stored in the data structure. */
  HAS_BIAS_UNCERTAINTY   = 1 << 4,
  /** A valid 'drift' is stored in the data structure. */
  HAS_DRIFT              = 1 << 5,
  /** A valid 'drift uncertainty' is stored in the data structure. */
  HAS_DRIFT_UNCERTAINTY  = 1 << 6
} GnssClockFlags;

/**
 * Represents a GNSS Measurement, it contains raw and computed information.
 *
 * Independence - All signal measurement information (e.g. sv_time,
 * pseudorange_rate, multipath_indicator) reported in this struct should be
 * based on GNSS signal measurements only. You may not synthesize measurements
 * by calculating or reporting expected measurements based on known or estimated
 * position, velocity, or time.
 */
struct GnssMeasurement {
  /**
   * Satellite vehicle ID number, as defined in GnssSvInfo::svid
   * This is a mandatory value.
   */
  int16_t svid;

  /**
   * Defines the constellation of the given SV. Value should be one of those
   * GNSS_CONSTELLATION_* constants
   */
  GnssConstellationType constellation;

  /**
   * Per satellite sync state. It represents the current sync state for the associated satellite.
   * Based on the sync state, the 'received GPS tow' field should be interpreted accordingly.
   *
   * This is a mandatory value.
   */
  GnssMeasurementState state;

  /**
   * The received GNSS Time-of-Week at the measurement time, in nanoseconds.
   * Ensure that this field is independent (see comment at top of
   * GnssMeasurement struct.)
   *
   * For GPS & QZSS, this is:
   *   Received GPS Time-of-Week at the measurement time, in nanoseconds.
   *   The value is relative to the beginning of the current GPS week.
   *
   *   Given the highest sync state that can be achieved, per each satellite, valid range
   *   for this field can be:
   *     Searching       : [ 0       ]   : GNSS_MEASUREMENT_STATE_UNKNOWN
   *     C/A code lock   : [ 0   1ms ]   : GNSS_MEASUREMENT_STATE_CODE_LOCK is set
   *     Bit sync        : [ 0  20ms ]   : GNSS_MEASUREMENT_STATE_BIT_SYNC is set
   *     Subframe sync   : [ 0    6s ]   : GNSS_MEASUREMENT_STATE_SUBFRAME_SYNC is set
   *     TOW decoded     : [ 0 1week ]   : GNSS_MEASUREMENT_STATE_TOW_DECODED is set
   *
   *   Note well: if there is any ambiguity in integer millisecond,
   *   GNSS_MEASUREMENT_STATE_MSEC_AMBIGUOUS should be set accordingly, in the 'state' field.
   *
   *   This value must be populated if 'state' != GNSS_MEASUREMENT_STATE_UNKNOWN.
   *
   * For Glonass, this is:
   *   Received Glonass time of day, at the measurement time in nanoseconds.
   *
   *   Given the highest sync state that can be achieved, per each satellite, valid range for
   *   this field can be:
   *     Searching       : [ 0       ]   : GNSS_MEASUREMENT_STATE_UNKNOWN
   *     C/A code lock   : [ 0   1ms ]   : GNSS_MEASUREMENT_STATE_CODE_LOCK is set
   *     Symbol sync     : [ 0  10ms ]   : GNSS_MEASUREMENT_STATE_SYMBOL_SYNC is set
   *     Bit sync        : [ 0  20ms ]   : GNSS_MEASUREMENT_STATE_BIT_SYNC is set
   *     String sync     : [ 0    2s ]   : GNSS_MEASUREMENT_STATE_GLO_STRING_SYNC is set
   *     Time of day     : [ 0  1day ]   : GNSS_MEASUREMENT_STATE_GLO_TOD_DECODED is set
   *
   * For Beidou, this is:
   *   Received Beidou time of week, at the measurement time in nanoseconds.
   *
   *   Given the highest sync state that can be achieved, per each satellite, valid range for
   *   this field can be:
   *     Searching    : [ 0       ] : GNSS_MEASUREMENT_STATE_UNKNOWN
   *     C/A code lock: [ 0   1ms ] : GNSS_MEASUREMENT_STATE_CODE_LOCK is set
   *     Bit sync (D2): [ 0   2ms ] : GNSS_MEASUREMENT_STATE_BDS_D2_BIT_SYNC is set
   *     Bit sync (D1): [ 0  20ms ] : GNSS_MEASUREMENT_STATE_BIT_SYNC is set
   *     Subframe (D2): [ 0  0.6s ] : GNSS_MEASUREMENT_STATE_BDS_D2_SUBFRAME_SYNC is set
   *     Subframe (D1): [ 0    6s ] : GNSS_MEASUREMENT_STATE_SUBFRAME_SYNC is set
   *     Time of week : [ 0 1week ] : GNSS_MEASUREMENT_STATE_TOW_DECODED is set
   *
   * For Galileo, this is:
   *   Received Galileo time of week, at the measurement time in nanoseconds.
   *
   *     E1BC code lock   : [ 0   4ms ]   : GNSS_MEASUREMENT_STATE_GAL_E1BC_CODE_LOCK is set
   *     E1C 2nd code lock: [ 0 100ms ]   :
   *     GNSS_MEASUREMENT_STATE_GAL_E1C_2ND_CODE_LOCK is set
   *
   *     E1B page    : [ 0    2s ] : GNSS_MEASUREMENT_STATE_GAL_E1B_PAGE_SYNC is set
   *     Time of week: [ 0 1week ] : GNSS_MEASUREMENT_STATE_TOW_DECODED is set
   *
   * For SBAS, this is:
   *   Received SBAS time, at the measurement time in nanoseconds.
   *
   *   Given the highest sync state that can be achieved, per each satellite,
   *   valid range for this field can be:
   *     Searching    : [ 0     ] : GNSS_MEASUREMENT_STATE_UNKNOWN
   *     C/A code lock: [ 0 1ms ] : GNSS_MEASUREMENT_STATE_CODE_LOCK is set
   *     Symbol sync  : [ 0 2ms ] : GNSS_MEASUREMENT_STATE_SYMBOL_SYNC is set
   *     Message      : [ 0  1s ] : GNSS_MEASUREMENT_STATE_SBAS_SYNC is set
   */
  int64_t received_sv_time_in_ns;

  /**
   * 1-Sigma uncertainty of the Received GPS Time-of-Week in nanoseconds.
   *
   * This value must be populated if 'state' != GPS_MEASUREMENT_STATE_UNKNOWN.
   */
  int64_t received_sv_time_uncertainty_in_ns;

  /**
   * Carrier-to-noise density in dB-Hz, typically in the range [0, 63].
   * It contains the measured C/N0 value for the signal at the antenna port.
   *
   * This is a mandatory value.
   */
  double c_n0_dbhz;

  /**
   * Pseudorange rate at the timestamp in m/s. The correction of a given
   * Pseudorange Rate value includes corrections for receiver and satellite
   * clock frequency errors. Ensure that this field is independent (see
   * comment at top of GnssMeasurement struct.)
   *
   * It is mandatory to provide the 'uncorrected' 'pseudorange rate', and provide GpsClock's
   * 'drift' field as well (When providing the uncorrected pseudorange rate, do not apply the
   * corrections described above.)
   *
   * The value includes the 'pseudorange rate uncertainty' in it.
   * A positive 'uncorrected' value indicates that the SV is moving away from the receiver.
   *
   * The sign of the 'uncorrected' 'pseudorange rate' and its relation to the sign of 'doppler
   * shift' is given by the equation:
   *      pseudorange rate = -k * doppler shift   (where k is a constant)
   *
   * This should be the most accurate pseudorange rate available, based on
   * fresh signal measurements from this channel.
   *
   * It is mandatory that this value be provided at typical carrier phase PRR
   * quality (few cm/sec per second of uncertainty, or better) - when signals
   * are sufficiently strong & stable, e.g. signals from a GPS simulator at >=
   * 35 dB-Hz.
   */
  double pseudorange_rate_mps;

  /**
   * 1-Sigma uncertainty of the pseudorange_rate_mps.
   * The uncertainty is represented as an absolute (single sided) value.
   *
   * This is a mandatory value.
   */
  double pseudorange_rate_uncertainty_mps;

  /**
   * Carrier frequency at which codes and messages are modulated, it can be L1 or L2.
   * If the field is not set, the carrier frequency is assumed to be L1.
   *
   * If the data is available, 'flags' must contain
   * GNSS_MEASUREMENT_HAS_CARRIER_FREQUENCY.
   */
  float carrier_frequency_hz;
};

/**
 * Represents an estimate of the GPS clock time.
 */
struct GnssClock {
  /**
   * The GNSS receiver internal clock value. This is the local hardware clock
   * value.
   *
   * For local hardware clock, this value is expected to be monotonically
   * increasing while the hardware clock remains power on. (For the case of a
   * HW clock that is not continuously on, see the
   * hw_clock_discontinuity_count field). The receiver's estimate of GPS time
   * can be derived by substracting the sum of full_bias_ns and bias_ns (when
   * available) from this value.
   *
   * This GPS time is expected to be the best estimate of current GPS time
   * that GNSS receiver can achieve.
   *
   * Sub-nanosecond accuracy can be provided by means of the 'bias_ns' field.
   * The value contains the 'time uncertainty' in it.
   *
   * This field is mandatory.
   */
  int64_t time_ns;

  /**
   * The difference between hardware clock ('time' field) inside GPS receiver
   * and the true GPS time since 0000Z, January 6, 1980, in nanoseconds.
   *
   * The sign of the value is defined by the following equation:
   *      local estimate of GPS time = time_ns - (full_bias_ns + bias_ns)
   *
   * This value is mandatory if the receiver has estimated GPS time. If the
   * computed time is for a non-GPS constellation, the time offset of that
   * constellation to GPS has to be applied to fill this value. The error
   * estimate for the sum of this and the bias_ns is the bias_uncertainty_ns,
   * and the caller is responsible for using this uncertainty (it can be very
   * large before the GPS time has been solved for.) If the data is available
   * 'flags' must contain GNSS_CLOCK_HAS_FULL_BIAS.
   */
  int64_t full_bias_ns;

  /**
   * Sub-nanosecond bias.
   * The error estimate for the sum of this and the full_bias_ns is the
   * bias_uncertainty_ns
   *
   * If the data is available 'flags' must contain GNSS_CLOCK_HAS_BIAS. If GPS
   * has computed a position fix. This value is mandatory if the receiver has
   * estimated GPS time.
   */
  double bias_ns;

  /**
   * 1-Sigma uncertainty associated with the local estimate of GPS time (clock
   * bias) in nanoseconds. The uncertainty is represented as an absolute
   * (single sided) value.
   *
   * If the data is available 'flags' must contain
   * GNSS_CLOCK_HAS_BIAS_UNCERTAINTY. This value is mandatory if the receiver
   * has estimated GPS time.
   */
  double bias_uncertainty_ns;

  /**
   * The clock's drift in nanoseconds (per second).
   *
   * A positive value means that the frequency is higher than the nominal
   * frequency, and that the (full_bias_ns + bias_ns) is growing more positive
   * over time.
   *
   * The value contains the 'drift uncertainty' in it.
   * If the data is available 'flags' must contain GNSS_CLOCK_HAS_DRIFT.
   *
   * This value is mandatory if the receiver has estimated GNSS time
   */
  double drift_nsps;

  /**
   * 1-Sigma uncertainty associated with the clock's drift in nanoseconds (per second).
   * The uncertainty is represented as an absolute (single sided) value.
   *
   * If the data is available 'flags' must contain
   * GNSS_CLOCK_HAS_DRIFT_UNCERTAINTY. If GPS has computed a position fix this
   * field is mandatory and must be populated.
   */
  double drift_uncertainty_nsps;

  /**
   * When there are any discontinuities in the HW clock, this field is
   * mandatory.
   *
   * A "discontinuity" is meant to cover the case of a switch from one source
   * of clock to another.  A single free-running crystal oscillator (XO)
   * should generally not have any discontinuities, and this can be set and
   * left at 0.
   *
   * If, however, the time_ns value (HW clock) is derived from a composite of
   * sources, that is not as smooth as a typical XO, or is otherwise stopped &
   * restarted, then this value shall be incremented each time a discontinuity
   * occurs.  (E.g. this value may start at zero at device boot-up and
   * increment each time there is a change in clock continuity. In the
   * unlikely event that this value reaches full scale, rollover (not
   * clamping) is required, such that this value continues to change, during
   * subsequent discontinuity events.)
   *
   * While this number stays the same, between GnssClock reports, it can be
   * safely assumed that the time_ns value has been running continuously, e.g.
   * derived from a single, high quality clock (XO like, or better, that's
   * typically used during continuous GNSS signal sampling.)
   *
   * It is expected, esp. during periods where there are few GNSS signals
   * available, that the HW clock be discontinuity-free as long as possible,
   * as this avoids the need to use (waste) a GNSS measurement to fully
   * re-solve for the GPS clock bias and drift, when using the accompanying
   * measurements, from consecutive GnssData reports.
   */
  uint32_t hw_clock_discontinuity_count;

  /**
   * Flags to indicate what fields in GnssClock are valid.
   */
  GnssClockFlags flags;
};
/**
 * AnboxGGAData represents GGA essential fix data which provide 3D location and accuracy data
 */
struct AnboxGGAData {
  /** Represents time for the location fix. */
  GpsUtcTime      time;
  /** Represents latitude in degrees. */
  double          latitude;
  /** Represents latitude in hemisphere. */
  char            latitudeHemi;
  /** Represents longitude in degrees. */
  double          longitude;
  /** Represents longitude in degrees. */
  char            longitudeHemi;
  /** Represents altitude. */
  double          altitude;
  /** Represents altitude unit. */
  char            altitudeUnit;
  /** Represents horizontal accuracy. */
  float           horizontalAccuracy;
  /** Represents vertical accuracy. */
  float           verticalAccuracy;
};

/**
 * AnboxRMCData represents essential gps pvt (position, velocity, time) data.
 */
struct AnboxRMCData {
  /** Represents time for the location fix. */
  GpsUtcTime      time;
  /** Represents gps status */
  char            status;
  /** Represents latitude in degrees. */
  double          latitude;
  /** Represents latitude in hemisphere. */
  char            latitudeHemi;
  /** Represents longitude in degrees. */
  double          longitude;
  /** Represents longitude in degrees. */
  char            longitudeHemi;
  /** Represents speed in meters per second.*/
  float           speed;
  /** Represents heading in degrees. */
  float           bearing;
  /** Represents date for the location fix. */
  GpsUtcTime      date;
  /** Represents horizontal accuracy. */
  float           horizontalAccuracy;
  /** Represents vertical accuracy. */
  float           verticalAccuracy;
};

/**
 * AnboxGnssData represents a reading of GNSS measurements, which includes
 * the GPS constellation measurements received from the GNSS receiver.
 *
 * - Reporting of GPS constellation measurements is mandatory.
 * - Reporting of all tracked constellations are encouraged.
 */
struct AnboxGnssData {
  /** The GPS clock time reading. */
  GnssClock clock;

  /** Number of measurements. */
  size_t measurement_count;

  /** The array of measurements. */
  GnssMeasurement measurements[GNSS_MAX_MEASUREMENT];
};


/**
 * @brief AnboxGpsDataType describes all gps data types supported by Anbox
 */
typedef enum : uint32_t {
  /** Unknown GPS data type */
  Unknown = 1 << 0,
  /** GGA represents essential fix data which provide 3D location and accuracy data */
  GGA     = 1 << 1,
  /** RMC represents essential gps pvt (position, velocity, time) data */
  RMC     = 1 << 2,
  /** GNSSv1 represents the version one of a constellation of satellites providing signals
   * information from space that transmit positioning and timing data */
  GNSSv1  = 1 << 3,
} AnboxGpsDataType;

/**
 * AnboxGpsData represents a Gps data that is used to not only hold information taken from
 * GPS sensors and will represent longitude, latitude, altitude and so on, but also contains
 * GNSS measurements data emitted from GNSS satellites to obtains the position information.
 */
struct AnboxGpsData {
  /** Type of the gps data */
  AnboxGpsDataType data_type;
  union {
    AnboxGGAData gga_data;
    AnboxRMCData rmc_data;
    AnboxGnssData  gnss_data;
  };
};

/**
 * @brief AnboxGraphicsFlipMode describes if the final frame needs to be
 * flipped to have the right visual orientation.
 */
typedef enum {
  /** No flip required */
  FLIP_MODE_NONE = 0,
  /** Flip the frame vertically */
  FLIP_MODE_VERTICAL = 1,
  /** Flip the frame horizontally */
  FLIP_MODE_HORIZONTAL = 2,
} AnboxGraphicsFlipMode;

/**
 * @brief AnboxGraphicsTextureFormat describes a list of supported texture
 * formats Anbox can provide a frame in.
 */
typedef enum {
  /** RGBA texture format */
  TEXTURE_FORMAT_RGBA = 0,
  /** BGRA texture format */
  TEXTURE_FORMAT_BGRA = 1,
} AnboxGraphicsTextureFormat;

/**
 * @brief AnboxDisplaySpec describes properties of the Anbox rendering
 * pipeline the platform plugin can influence.
 */
typedef struct {
  /**
   * Native EGL display used to setup EGL
   */
  EGLNativeDisplayType native_display;

  /**
   * Native EGL window type used to create the main rendering surface. If
   * set to NULL Anbox will assume rendering in headless mode.
   */
  EGLNativeWindowType native_window;

  /**
   * If the GPU stores texture in a different orientation than the native
   * one the flip mode can be used to tell Anbox to flip frame over
   * during the final render pass.
   */
  AnboxGraphicsFlipMode output_flip_mode;

  /**
   * If the texture used for rendering should use a different format as
   * it should to a video encoder the texture format can be used to tell
   * Anbox to flip the color bits during the final render pass.
   *
   * The default is RGBA.
   */
  AnboxGraphicsTextureFormat texture_format;

  /**
   * If set to true Anbox will avoid querying EGL for configurations with
   * pbuffer support. This is usefull on driver implementations which don't
   * support EGL pbuffers.
   *
   * The default is false.
   */
  bool avoid_pbuffers;
} AnboxGraphicsConfiguration;

/**
 * @brief AnboxInputDeviceType describes the type of device an input event
 * belongs to. Possible device types are: pointer, keyboard, touchpanel.
 */
typedef enum {
  /** A pointer device */
  POINTER = 0,
  /** A keyboard device */
  KEYBOARD,
  /** A touchpanel device */
  TOUCHPANEL,
  /** A gamepad device */
  GAMEPAD,
} AnboxInputDeviceType;

/**
 * @brief AnboxInputEvent is similar to the input_event that is defined in the
 * Linux kernel API. The three member variables type, code and value have the
 * same meaning as defined in the Linux kernel API.
 * Please see https://www.kernel.org/doc/Documentation/input/input.txt for further information.
 */
struct AnboxInputEvent {
  /** Type of the device the input event belongs to. */
  AnboxInputDeviceType device_type;
  /** Id identifying a device of a given type */
  int32_t device_id;
  /** The type of the input event. E.g. EV_KEY stands for a keyboard event.
   * See complete list in /usr/include/linux/input-event-codes.h */
  uint16_t type;
  /** The code of the input event. E.g. KEY_ENTER when you press the ENTER key. */
  uint16_t code;
  /** The value of the input event. E.g. 0 when a keyboard key is released,
   * 1 stands for keyboard key is pressed. */
  int32_t value;
};

#define MAX_SENSOR_DATA_LENGTH 16
#define MAX_VECTOR_DATA_LENGTH 3

/**
 * @brief AnboxSensorType describes all sensor types supported by Anbox
 */
typedef enum : uint32_t {
  /** no sensor support */
  NONE = 0,
  /** 3-axis Accelerometer */
  ACCELERATION  = 1 << 0,
  /** 3-axis Gyroscope */
  GYROSCOPE     = 1 << 1,
  /** 3-axis Magnetic field sensor */
  MAGNETOMETER  = 1 << 2,
  /** Orientation sensor */
  ORIENTATION   = 1 << 3,
  /** Ambient Temperature sensor */
  TEMPERATURE   = 1 << 4,
  /** Proximity sensor */
  PROXIMITY     = 1 << 5,
  /** Light sensor */
  LIGHT         = 1 << 6,
  /** Pressure sensor */
  PRESSURE      = 1 << 7,
  /** Humidity sensor */
  HUMIDITY      = 1 << 8,
} AnboxSensorType;

/**
 * @brief AnboxSensorVector represents acceleration along each device axis
 * or current device rotation angles: azimuth, pitch, roll
 */
struct AnboxSensorVector {
  union {
    /** Underlying data value */
    float v[MAX_VECTOR_DATA_LENGTH];
    struct {
      /** X axis */
      float x;
      /** Y axis */
      float y;
      /** Z axis */
      float z;
    } axis;
    struct {
      /** Rotation angles: azimuth */
      float azimuth;
      /** Rotation angles: pitch */
      float pitch;
      /** Rotation angles: roll */
      float roll;
    } angle;
  };
};

/**
 * @brief AnboxSensorData represents the sensor data stucture,
 */
struct AnboxSensorData {
  /** Type of the sensor */
  AnboxSensorType sensor_type;
  union {
    float  values[MAX_SENSOR_DATA_LENGTH];
    /** Sensor data for 3-axis accelerometer */
    AnboxSensorVector acceleration;
    /** Sensor data for 3-axis gyroscope */
    AnboxSensorVector gyroscope;
    /** Sensor data for orientation */
    AnboxSensorVector orientation;
    /** Sensor data for magnetic field */
    AnboxSensorVector magnetic;
    /** Sensor data for ambient temperature */
    float           temperature;
    /** Sensor data for proximity */
    float           proximity;
    /** Sensor data for light */
    float           light;
    /** Sensor data for pressure */
    float           pressure;
    /** Sensor data for humidity */
    float           humidity;
  };
};

/**
 * @brief AnboxVhalPropertyStatus describes the status of a VHAL property.
 *
 * Matches the <a href="https://cs.android.com/android/platform/superproject/+/android10-release:hardware/interfaces/automotive/vehicle/2.0/types.hal;l=2700-2720">VehiclePropertyStatus</a>
 * enum of the Android VHAL interface.
 */
typedef enum {
  ANBOX_VHAL_PROPERTY_STATUS_AVAILABLE   = 0x0,
  ANBOX_VHAL_PROPERTY_STATUS_UNAVAILABLE = 0x1,
  ANBOX_VHAL_PROPERTY_STATUS_ERROR       = 0x2,
} AnboxVhalPropertyStatus;

/**
 * @brief AnboxVhalPropertyType describes the type of the value stored by a VHAL
 * property.
 *
 * Matches the <a href="https://cs.android.com/android/platform/superproject/+/android10-release:hardware/interfaces/automotive/vehicle/2.0/types.hal;l=19-42">VehiclePropertyType</a>
 * enum of the Android VHAL interface.
 */
typedef enum {
  ANBOX_VHAL_PROPERTY_TYPE_STRING    = 0x00100000,
  ANBOX_VHAL_PROPERTY_TYPE_BOOLEAN   = 0x00200000,
  ANBOX_VHAL_PROPERTY_TYPE_INT32     = 0x00400000,
  ANBOX_VHAL_PROPERTY_TYPE_INT32_VEC = 0x00410000,
  ANBOX_VHAL_PROPERTY_TYPE_INT64     = 0x00500000,
  ANBOX_VHAL_PROPERTY_TYPE_INT64_VEC = 0x00510000,
  ANBOX_VHAL_PROPERTY_TYPE_FLOAT     = 0x00600000,
  ANBOX_VHAL_PROPERTY_TYPE_FLOAT_VEC = 0x00610000,
  ANBOX_VHAL_PROPERTY_TYPE_BYTES     = 0x00700000,
  ANBOX_VHAL_PROPERTY_TYPE_MIXED     = 0x00e00000,
} AnboxVhalPropertyType;

/**
 * @brief AnboxVhalPropertyAccess describes if the property is read, write, or
 * both.
 *
 * Matches the <a href="https://cs.android.com/android/platform/superproject/+/android10-release:hardware/interfaces/automotive/vehicle/2.0/types.hal;l=2686-2698">VehiclePropertyAccess</a>
 * enum of the Android VHAL interface.
 */
typedef enum {
  ANBOX_VHAL_PROPERTY_ACCESS_NONE       = 0x00,
  ANBOX_VHAL_PROPERTY_ACCESS_READ       = 0x01,
  ANBOX_VHAL_PROPERTY_ACCESS_WRITE      = 0x02,
  ANBOX_VHAL_PROPERTY_ACCESS_READ_WRITE = 0x03,
} AnboxVhalPropertyAccess;

/**
 * @brief AnboxVhalPropertyChangeMode describes how the property changes.
 *
 * Matches the <a href="https://cs.android.com/android/platform/superproject/+/android10-release:hardware/interfaces/automotive/vehicle/2.0/types.hal;l=2657-2684">VehiclePropertyChangeMode</a> enum of the Android VHAL interface.
 */
typedef enum {
  ANBOX_VHAL_PROPERTY_CHANGE_MODE_STATIC     = 0x00,
  ANBOX_VHAL_PROPERTY_CHANGE_MODE_ON_CHANGE  = 0x01,
  ANBOX_VHAL_PROPERTY_CHANGE_MODE_CONTINUOUS = 0x02,
} AnboxVhalPropertyChangeMode;

/**
 * @brief AnboxVhalPropertyValue describes the current value of a VHAL property,
 * as returned by a get call to the Android VHAL.
 *
 * Matches the <a
 * href="https://cs.android.com/android/platform/superproject/+/android10-release:hardware/interfaces/automotive/vehicle/2.0/types.hal;l=2862-2911">VehiclePropValue</a>
 * struct of the Android VHAL interface.
 */
struct AnboxVhalPropertyValue {
  int64_t timestamp;
  int32_t area_id;
  int32_t prop;
  AnboxVhalPropertyStatus status;
  uint32_t int32_values_size;
  int32_t* int32_values;
  uint32_t float_values_size;
  float* float_values;
  uint32_t int64_values_size;
  int64_t* int64_values;
  uint32_t bytes_size;
  uint8_t* bytes;
  uint32_t string_value_size;
  char* string_value;
};

/**
 * @brief AnboxVhalAreaConfig describes the configuration of a given area id
 * for a VHAL property.
 *
 * Matches the <a
 * href="https://cs.android.com/android/platform/superproject/+/android10-release:hardware/interfaces/automotive/vehicle/2.0/types.hal;l=2797-2819">VehicleAreaConfig</a>
 * struct of the Android VHAL interface.
 *
 * The following properties are not part of the Android VHAL interface but are
 * added for convenience:
 * - area_names: array of area names making up the area, as defined for each
 *   area type, e.g. [ROW_1_LEFT, ROW_1_RIGHT].
 */
struct AnboxVhalAreaConfig {
  int32_t area_id;
  int32_t min_int32_value;
  int32_t max_int32_value;
  int64_t min_int64_value;
  int64_t max_int64_value;
  float min_float_value;
  float max_float_value;
  uint32_t area_names_size;
  char (*area_names)[MAX_VHAL_AREA_NAME_LENGTH];
};

/**
 * @brief AnboxVhalPropertyConfig describes the configuration of a VHAL
 * property.
 *
 * Matches the <a
 * href="https://cs.android.com/android/platform/superproject/+/android10-release:hardware/interfaces/automotive/vehicle/2.0/types.hal;l=2821-2860">VehiclePropConfig</a>
 * struct of the Android VHAL interface.
 *
 * The following properties are not part of the Android VHAL interface but are
 * added for convenience:
 * - value_type: value type of the property
 * - prop_name: name of the property, as defined in the <a href="https://cs.android.com/android/platform/superproject/+/android10-release:hardware/interfaces/automotive/vehicle/2.0/types.hal;l=119-2324">
 * VehicleProperty</a> enum, e.g.: INFO_VIN, HVAC_TEMPERATURE_CURRENT
 */
struct AnboxVhalPropertyConfig {
  int32_t prop;
  AnboxVhalPropertyType value_type;
  AnboxVhalPropertyAccess access;
  AnboxVhalPropertyChangeMode change_mode;
  uint32_t area_configs_size;
  AnboxVhalAreaConfig* area_configs;
  uint32_t config_array_size;
  int32_t* config_array;
  uint32_t config_string_size;
  char* config_string;
  float min_sample_rate;
  float max_sample_rate;
  uint32_t prop_name_size;
  char* prop_name;
};

/**
 * @brief AnboxVhalAnswerStatus describes the return status of a request sent to
 * the Android VHAL.
 */
typedef enum {
  ANBOX_VHAL_ANSWER_STATUS_OK = 0,
  /** The request was invalid. */
  ANBOX_VHAL_ANSWER_STATUS_INVALID = 1,
  /** An unknown or internal error occurred. */
  ANBOX_VHAL_ANSWER_STATUS_UNKNOWN = 2,
} AnboxVhalAnswerStatus;

/**
 * @brief AnboxVhalAnswerGet contains the answer for a GetAllPropConfigs or
 * GetPropConfigs request sent to the Android VHAL.
 */
struct AnboxVhalAnswerGetConfigs {
  uint32_t configs_size;
  AnboxVhalPropertyConfig* configs;
};

/**
 * @brief AnboxVhalCommandGet describes a get request to send to the Android
 * VHAL.
 */
struct AnboxVhalCommandGet {
  int32_t prop_id;
  int32_t area_id;
  uint32_t int32_values_size;
  int32_t* int32_values;
  uint32_t float_values_size;
  float* float_values;
  uint32_t int64_values_size;
  int64_t* int64_values;
  uint32_t bytes_size;
  uint8_t* bytes;
  uint32_t string_value_size;
  char* string_value;
};

/**
 * @brief AnboxVhalCommandSet describes a set request to send to the Android
 * VHAL.
 */
struct AnboxVhalCommandSet {
  int32_t prop_id;
  int32_t area_id;
  AnboxVhalPropertyStatus status;
  uint32_t int32_values_size;
  int32_t* int32_values;
  uint32_t float_values_size;
  float* float_values;
  uint32_t int64_values_size;
  int64_t* int64_values;
  uint32_t bytes_size;
  uint8_t* bytes;
  uint32_t string_value_size;
  char* string_value;
};

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

/**
 * @brief Method prototype which will be used to determine if the given tracing category
 * is enabled for tracing inside the Anbox runtime.
 */
typedef const unsigned char* (*AnboxTracerGetCategoryEnabledFunc)(const char* name);

/**
 * @brief Type defining the phase of a trace event, e.g. begin/end pair
 */
typedef enum {
  ANBOX_TRACE_EVENT_PHASE_BEGIN = 'B',
  ANBOX_TRACE_EVENT_PHASE_END = 'E',
  ANBOX_TRACE_EVENT_PHASE_INSTANT = 'I',
  ANBOX_TRACE_EVENT_PHASE_COUNTER = 'C',
} AnboxTraceEventPhase;


/**
 * @brief Type of an argument passed with a trace event
 */
typedef enum {
  ANBOX_TRACE_EVENT_ARG_TYPE_BOOL = 1,
  ANBOX_TRACE_EVENT_ARG_TYPE_UINT = 2,
  ANBOX_TRACE_EVENT_ARG_TYPE_INT = 3,
  ANBOX_TRACE_EVENT_ARG_TYPE_DOUBLE = 4,
  ANBOX_TRACE_EVENT_ARG_TYPE_POINTER = 5,
  ANBOX_TRACE_EVENT_ARG_TYPE_STRING = 6,
  ANBOX_TRACE_EVENT_ARG_TYPE_COPY_STRING = 7,
} AnboxTraceEventArgType;

/**
 * @brief Method prototype which will be used by the platform to submit trace events to
 * the tracing implementation inside the Anbox runtime.
 *
 * @param phase Phase of the trace event (see AnboxTraceEventPhase)
 * @param category Pointer to a statically allocated string naming the category the event belongs to
 * @param name Name of the trace event
 * @param id Unique id of the trace event
 * @param num_args Number of arguments the trace event has
 * @param arg_names Array of length num_args containing the argument names
 * @param arg_types Array of length num_args containing the argument types
 * @param arg_values Array of length num_args containing the argument values
 * @param flags Flags passed with the trace event, currently unused
 */
typedef void (*AnboxTracerAddEventFunc)(
  char phase,
  const unsigned char* category,
  const char* name,
  unsigned long long id,
  int num_args,
  const char** arg_names,
  const unsigned char* arg_types,
  const unsigned long long* arg_values,
  unsigned char flags);

#endif
