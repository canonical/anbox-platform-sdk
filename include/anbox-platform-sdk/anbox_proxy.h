// Anbox - The Android in a Box runtime environment
// Copyright 2019 Canonical Ltd.  All rights reserved.

#ifndef ANBOX_SDK_ANBOX_PROXY_H_
#define ANBOX_SDK_ANBOX_PROXY_H_

#include <errno.h>
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
 * @brief AnboxChangeScreenOrientationCallback is invoked when changing the screen orientation.
 * @return 0 on success otherwise returns EINVAL on error occurs.
 */
typedef int (*AnboxChangeScreenOrientationCallback)(AnboxScreenOrientationType orientation_type, void* user_data);

/**
 * @brief AnboxChangeDisplayDensityCallback is invoked when changing the display density.
 * @return 0 on success otherwise returns EINVAL on error occurs.
 */
typedef int (*AnboxChangeDisplayDensityCallback)(uint32_t density, void* user_data);

/**
 * @brief AnboxChangeDisplaySizeCallback is invoked when changing the display size.
 * @return 0 on success otherwise returns EINVAL on error occurs.
 */
typedef int (*AnboxChangeDisplaySizeCallback)(uint32_t width, uint32_t height, void* user_data);

namespace anbox {
/**
 * @brief AnboxProxy provides a proxy layer which encapsulates callbacks that can be used by a platform
 * to change Android system behaviors. E.g. change the screen orientation. The platform can invoke the
 * callbacks which are provided by Anbox on demand when triggering an action or changing a behavior.
 */
class AnboxProxy {
  public:
   AnboxProxy() = default;
   virtual ~AnboxProxy() = default;
   AnboxProxy(const AnboxProxy &) = delete;
   AnboxProxy& operator=(const AnboxProxy &) = delete;

    /**
     * @brief Change the screen orientation
     *
     * @param orientation_type the type of orientation to be applied to Android container
     * @return 0 on success, otherwise returns EINVAL on error occurs.
     **/
   int change_screen_orientation(AnboxScreenOrientationType orientation_type) {
     if (!change_screen_orientation_callback_)
       return -EINVAL;
     return change_screen_orientation_callback_(orientation_type, screen_orientation_callback_user_data_);
   }

   /** @brief Set the screen orientation change callback. */
   void set_change_screen_orientation_callback(const AnboxChangeScreenOrientationCallback& callback,
                                               void* user_data) {
     change_screen_orientation_callback_ = callback;
     screen_orientation_callback_user_data_ = user_data;
   }

    /**
     * @brief Change the display density
     *
     * @param density the density value of the display
     * @return 0 on success, otherwise returns EINVAL on error occurs.
     **/
   int change_display_density(uint32_t density) {
     if (!change_display_density_callback_)
       return -EINVAL;
     return change_display_density_callback_(density, display_density_callback_user_data_);
   }

   /** @brief Set the display density change callback. */
   void set_change_display_density_callback(const AnboxChangeDisplayDensityCallback& callback,
                                            void* user_data) {
     change_display_density_callback_ = callback;
     display_density_callback_user_data_ = user_data;
   }

    /**
     * @brief Change the display size
     *
     * @param width the width of the display size
     * @param height the height of the display size
     * @return 0 on success, otherwise returns EINVAL on error occurs.
     **/
   int change_display_size(uint32_t width, uint32_t height) {
     if (!change_display_size_callback_)
       return -EINVAL;
     return change_display_size_callback_(width, height, display_size_callback_user_data_);
   }

   /** @brief Set the display size change callback. */
   void set_change_display_size_callback(const AnboxChangeDisplaySizeCallback& callback,
                                         void* user_data) {
     change_display_size_callback_ = callback;
     display_size_callback_user_data_ = user_data;
   }

  private:
   AnboxChangeScreenOrientationCallback change_screen_orientation_callback_{nullptr};
   AnboxChangeDisplayDensityCallback change_display_density_callback_{nullptr};
   AnboxChangeDisplaySizeCallback change_display_size_callback_{nullptr};
   void* screen_orientation_callback_user_data_{nullptr};
   void* display_density_callback_user_data_{nullptr};
   void* display_size_callback_user_data_{nullptr};
};
} // namespace anbox

#endif
