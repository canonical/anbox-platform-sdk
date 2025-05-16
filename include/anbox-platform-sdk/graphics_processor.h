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

#ifndef ANBOX_SDK_GRAPHICS_PROCESSOR_H_
#define ANBOX_SDK_GRAPHICS_PROCESSOR_H_

#include "anbox-platform-sdk/types.h"

/**
 * @brief AnboxVsyncCallback is invoked to signal a new vsync is about to start.
 *
 * The provided timestamp is the time the callee will use to determine the start
 * time of the vsync.
 */
typedef void (*AnboxVsyncCallback)(uint64_t time_ns, void* user_data);

namespace anbox {
/**
 * @brief GraphicsProcessor allows integration with the graphics engine inside Anbox.
 */
class GraphicsProcessor {
  public:
    GraphicsProcessor() = default;
    virtual ~GraphicsProcessor() = default;
    GraphicsProcessor(const GraphicsProcessor &) = delete;
    GraphicsProcessor& operator=(const GraphicsProcessor &) = delete;

    /**
     * @brief Create an EGL display
     *
     * This function is called by Anbox when it initializes the rendering pipeline
     * to create an EGLDisplay to use for rendering. In case that a platform provides
     * no implementation or EGL_NO_DISPLAY is returned Anbox will use EGL_DEFAULT_DISPLAY
     * to create an EGLDisplay
     *
     * @return EGLDisplay A valid EGLDisplay or EGL_NO_DISPLAY
     */
    virtual EGLDisplay create_display() { return EGL_NO_DISPLAY; }

    /**
     * @brief Initialize the graphics processor
     *
     * This function is called by Anbox when the rendering pipeline is initialized. The
     * configuration structure is provided from Anbox and has to be filled out by the
     * platform. Anbox initializes the provided structure with its defaults.
     *
     * @param configuration Configuration initialized with default values from Anbox
     * @return 0 on success, a negative error code otherwise.
     **/
    virtual int initialize(AnboxGraphicsConfiguration* configuration) {
      (void) configuration;
      return 0;
    };

    /**
     * @brief Called from Anbox when a new frame is started
     *
     * This function is called by Anbox from its render thread after the call
     * to eglMakeCurrent to make the main rendering surface and context current.
     * Any operation performed should be as fast as possible to not delay the
     * frame for too long.
     **/
    virtual void begin_frame() { };

    /**
     * @brief Called from Anbox when a frame was fully rendered.
     *
     * This function is called by Anbox from its render thread before its calling
     * eglSwapBuffers on the main EGL surface. Any operation performed should be
     * as fast as possible to not delay the frame for too long.
     **/
    virtual void finish_frame() { };

    /**
     * @brief Create an offscreen EGL surface for the given display, configuration
     * and attributes.
     *
     * If not implemented Anbox will use eglCreatePbufferSurface as fallback.
     *
     * @param display EGL display to create the surface for
     * @param config EGL configuration to create the surface for
     * @param attribs Attributes to used for the surface creation
     * @return Created EGLSurface or EGL_NO_SURFACE on error
     */
    virtual EGLSurface create_offscreen_surface(EGLDisplay display, EGLConfig config, const EGLint* attribs) {
      (void) display;
      (void) config;
      (void) attribs;
      return EGL_NO_SURFACE;
    }

    /**
     * Destroy a surface created with create_offscreen_surface.
     *
     * If not implemented Anbox will use eglDestroySurface as fallback.
     *
     * @param display EGL display the surface was created for
     * @param surface EGL surface to destroy
     * @return true, if the surface is successfully destroyed and false otherwise.
     */
    virtual bool destroy_offscreen_surface(EGLDisplay display, EGLSurface surface) {
      (void) display;
      (void) surface;
      return false;
    }

    /**
     * @brief Present the given buffer to a display or other output
     *
     * DEPRECATED: This variant of the #present method is deprecated and should no longer be used.
     * Instead use the variant which takes the #AnboxGraphicsBuffer2 structure.
     *
     * Used when graphics implementation is ANBOX_GRAPHICS_IMPLEMENTATION_TYPE_DIRECT_RENDERING
     * or ANBOX_GRAPHICS_IMPLEMENTATION_TYPE_HOST_RENDERING
     *
     * This method will be called by Anbox when Android has submitted a new buffer to be
     * presented on the output managed by the platform. The buffer contains information about
     * dimension, format and the handle to the buffer itself.
     *
     * The buffer handle is opaque to Anbox and it's up to the platform to map it to something
     * which it can handle. For all GPUs the handle is a file descriptor of a DMA buffer (see
     * https://www.kernel.org/doc/html/latest/driver-api/dma-buf.html) which can imported into
     * APIs like EGL or Vulkan for further processing. For software rendering the handle is
     * a memfd (https://man7.org/linux/man-pages/man2/memfd_create.2.html) pointing to a shared
     * memory region containing the buffer data.
     *
     * When the platform has finished presenting the buffer it has to call the provided callback
     * in order to return the buffer to Anbox for reuse. Not returning the buffer will cause the
     * rendering pipeline to become stuck.
     *
     * If #present returns false it is expected that #callback is not called by the
     * implementation.
     *
     * @param buffer Buffer to be presented on the output
     * @param callback A callback to be called by the platform when the buffer has been presented
     * @return true if the buffer was accepted for presentation, false otherwise.
     */
    [[deprecated("Use GraphicsProcessor::present(AnboxGraphicsBuffer2* buffer, AnboxCallback* callback) instead")]]
    virtual bool present(AnboxGraphicsBuffer* buffer, AnboxCallback* callback) {
      (void) buffer;
      (void) callback;
      return false;
    }

    /**
     * @brief Present the given buffer to a display or other output
     *
     * Used when graphics implementation is ANBOX_GRAPHICS_IMPLEMENTATION_TYPE_DIRECT_RENDERING
     * or ANBOX_GRAPHICS_IMPLEMENTATION_TYPE_HOST_RENDERING
     *
     * This method will be called by Anbox when Android has submitted a new buffer to be
     * presented on the output managed by the platform. The buffer contains information about
     * dimension, format and the handle to the buffer itself.
     *
     * The buffer handle is opaque to Anbox and it's up to the platform to map it to something
     * which it can handle. For all GPUs the handle is a file descriptor of a DMA buffer (see
     * https://www.kernel.org/doc/html/latest/driver-api/dma-buf.html) which can imported into
     * APIs like EGL or Vulkan for further processing. For software rendering the handle is
     * a memfd (https://man7.org/linux/man-pages/man2/memfd_create.2.html) pointing to a shared
     * memory region containing the buffer data.
     *
     * Passed buffer objects are reused by the caller so the given address for the
     * AnboxGraphicsBuffer2 can be used as identifier for cache implementations.
     *
     * When the platform has finished presenting the buffer it has to call the provided callback
     * in order to return the buffer to Anbox for reuse. Not returning the buffer will cause the
     * rendering pipeline to become stuck.
     *
     * If #present returns false it is expected that #callback is not called by the
     * implementation.
     *
     * @param buffer Buffer to be presented on the output
     * @param callback A callback to be called by the platform when the buffer has been presented
     * @return true if the buffer was accepted for presentation, false otherwise.
     */
    virtual bool present(AnboxGraphicsBuffer2* buffer, AnboxCallback* callback) {
      (void) buffer;
      (void) callback;
      return false;
    }

    /**
     * @brief Create a buffer with the given specifications
     *
     * @param width Width of the buffer in pixels
     * @param height Height of the buffer in pixels
     * @param format Color format of the buffer. See AnboxGraphicsBufferPixelFormat
     * @param usage Usage flags of the buffer. See AnboxGraphicsBufferUsage
     * @param buffer Allocated buffer object. See AnboxGraphicsBuffer2
     * @return true if the buffer was successfully created, false otherwise
     */
    virtual bool create_buffer(uint32_t width, uint32_t height, uint32_t format,
                               uint32_t usage, AnboxGraphicsBuffer2** buffer) {
      (void) width;
      (void) height;
      (void) format;
      (void) usage;
      (void) buffer;
      return false;
    }

  /**
   * @brief Sets a callback which will be invoked whenever a new vsync is about
   * to start.
   *
   * With the start of a vsync the previous frame will no longer be visible on
   * the output and a new one will be shown instead. The callback allows an
   * external client to synchronize to the internal vsync of the platform.
   *
   * @param callback Callback to set
   * @param user_data User data to be handed back with the callback
   */
  virtual void set_vsync_callback(const AnboxVsyncCallback& callback,
                                  void* user_data) {
    vsync_callback_ = callback;
    vsync_callback_user_data_ = user_data;
  }

 protected:
  void signal_vsync(uint64_t time_ns) {
    if (!vsync_callback_)
      return;

    vsync_callback_(time_ns, vsync_callback_user_data_);
  }

 private:
  AnboxVsyncCallback vsync_callback_ = nullptr;
  void* vsync_callback_user_data_ = nullptr;
};
} // namespace anbox

#endif
