// Anbox - The Android in a Box runtime environment
// Copyright 2018 Canonical Ltd.  All rights reserved.

#ifndef ANBOX_SDK_GRAPHICS_PROCESSOR_H_
#define ANBOX_SDK_GRAPHICS_PROCESSOR_H_

#include <stdint.h>
#include <stddef.h>

#include <EGL/egl.h>

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
    virtual int initialize(AnboxGraphicsConfiguration* configuration) = 0;

    /**
     * @brief Called from Anbox when a new frame is started
     *
     * This function is called by Anbox from its render thread after the call
     * to eglMakeCurrent to make the main rendering surface and context current.
     * Any operation performed should be as fast as possible to not delay the
     * frame for too long.
     **/
    virtual void begin_frame() = 0;

    /**
     * @brief Called from Anbox when a frame was fully rendered.
     *
     * This function is called by Anbox from its render thread before its calling
     * eglSwapBuffers on the main EGL surface. Any operation performed should be
     * as fast as possible to not delay the frame for too long.
     **/
    virtual void finish_frame() = 0;

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
};
} // namespace anbox

#endif
