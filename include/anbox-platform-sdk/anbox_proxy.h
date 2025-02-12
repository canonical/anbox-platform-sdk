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

#ifndef ANBOX_SDK_ANBOX_PROXY_H_
#define ANBOX_SDK_ANBOX_PROXY_H_

#include "anbox-platform-sdk/types.h"

#include <errno.h>
#include <stdint.h>
#include <unistd.h>

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

/**
 * @brief AnboxTriggerActionCallback is invoked when an action is
 *        triggered within the Android system.
 * @return 0 on success otherwise returns EINVAL on error occurs.
 */
typedef int (*AnboxTriggerActionCallback)(const char* name, const char **args, size_t args_len, void* user_data);

/**
 * @brief AnboxCreateADBConnectionCallback is invoked when creating ADB connection.
 * @return 0 on success otherwise returns EINVAL on error occurs.
 */
typedef int (*AnboxCreateADBConnectionCallback)(const char* id, void* user_data);

/**
 * @brief AnboxDisconnectADBConnectionCallback is invoked when disconnecting ADB connection.
 * @return 0 on success otherwise returns EINVAL on error occurs.
 */
typedef int (*AnboxDisconnectADBConnectionCallback)(const char* id, void* user_data);

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

   /**
    * @brief Trigger an action which is executed within the Android container
    *
    * @param name the name of action
    * @param args the arguments to be launched the action
    * @param args_len the number of arguments to launch the action
    * @return 0 on success, otherwise returns EINVAL on error occurs.
    * @note the args only stands for the parameter to action and unlike standard C
    *       the action name must not be included in the parameters. So does `args_len`
    *       only reflects the number of parameters, the name of the action must not be counted.
    *       The returned value 0 only implies the request to launch action is handled by
    *       Anbox successfully, it doesn't tell if the action is invoked successfully
    *       within the Android container or not. To check the actual error message if
    *       the action script is not executed successfully within the Android container,
    *       please view the log of anbox service unit.
    **/
   int trigger_action(const char* name, const char** args, size_t args_len) {
     if (!trigger_action_callback_)
       return -EINVAL;
     return trigger_action_callback_(name, args, args_len, trigger_action_callback_user_data_);
   }

   /** @brief Set the trigger action callback. */
   void set_trigger_action_callback(const AnboxTriggerActionCallback& callback,
                                    void* user_data) {
     trigger_action_callback_ = callback;
     trigger_action_callback_user_data_ = user_data;
   }

   /**
    * @brief Send a message from Anbox to the platform
    *
    * @param type pointer to type data
    * @param type_size length of type data
    * @param data pointer to message data
    * @param data_size length of message data
    * @return 0 on success, otherwise returns a negative value on error.
    **/
   virtual int send_message(const char* type, size_t type_size,
                            const char* data, size_t data_size) {
     (void) type;
     (void) type_size;
     (void) data;
     (void) data_size;
     return -EIO;
   }

   /**
    * @brief Create ADB connection
    *
    * @param id the unique ID of the ADB connection
    * @return a positive value representing the exposed ADB connection port number on success,
    *         or EINVAL if an error occurs.
    **/
   int create_adb_connection(const char* id) {
     if (!create_adb_connection_callback_)
       return -EINVAL;
     return create_adb_connection_callback_(id, create_adb_connection_callback_user_data_);
   }

   /** @brief Set the ADB connection creation callback. */
   void set_create_adb_connection_callback(const AnboxCreateADBConnectionCallback& callback,
                                           void* user_data) {
     create_adb_connection_callback_ = callback;
     create_adb_connection_callback_user_data_ = user_data;
   }

   /**
    * @brief Disconnect ADB connection
    *
    * @param id the unique ID of the ADB connection
    * @return 0 on success, otherwise returns a negative value on error.
    **/
   int disconnect_adb_connection(const char* id) {
     if (!disconnect_adb_connection_callback_)
       return -EINVAL;
     return disconnect_adb_connection_callback_(id, disconnect_adb_connection_callback_user_data_);
   }

   /** @brief Set the ADB connection disconnect callback. */
   void set_disconnect_adb_connection_callback(const AnboxDisconnectADBConnectionCallback& callback,
                                               void* user_data) {
     disconnect_adb_connection_callback_ = callback;
     disconnect_adb_connection_callback_user_data_ = user_data;
   }

  private:
   AnboxChangeScreenOrientationCallback change_screen_orientation_callback_{nullptr};
   AnboxChangeDisplayDensityCallback change_display_density_callback_{nullptr};
   AnboxChangeDisplaySizeCallback change_display_size_callback_{nullptr};
   AnboxTriggerActionCallback trigger_action_callback_{nullptr};
   AnboxCreateADBConnectionCallback create_adb_connection_callback_{nullptr};
   AnboxDisconnectADBConnectionCallback disconnect_adb_connection_callback_{nullptr};

   void* screen_orientation_callback_user_data_{nullptr};
   void* display_density_callback_user_data_{nullptr};
   void* display_size_callback_user_data_{nullptr};
   void* trigger_action_callback_user_data_{nullptr};
   void* create_adb_connection_callback_user_data_{nullptr};
   void* disconnect_adb_connection_callback_user_data_{nullptr};
};
} // namespace anbox

#endif
