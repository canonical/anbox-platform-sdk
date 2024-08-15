/*
 * This file is part of Anbox Platform SDK
 *
 * Copyright 2024 Canonical Ltd.
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

#ifndef ANBOX_SDK_VHAL_CONNECTOR_H_
#define ANBOX_SDK_VHAL_CONNECTOR_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief AnboxVhalGetAllPropConfigsCallback is invoked when requesting all prop
 * configs.
 * @return ANBOX_VHAL_ANSWER_STATUS_OK on success, error code otherwise.
 */
typedef AnboxVhalAnswerStatus (*AnboxVhalGetAllPropConfigsCallback)(
  AnboxVhalAnswerGetConfigs *result, void *user_data);

/**
 * @brief AnboxVhalGetPropConfigsCallback is invoked when requesting some prop
 * configs.
 * @return ANBOX_VHAL_ANSWER_STATUS_OK on success, error code otherwise.
 */
typedef AnboxVhalAnswerStatus (*AnboxVhalGetPropConfigsCallback)(
  int32_t *props, size_t props_size, AnboxVhalAnswerGetConfigs *result,
  void *user_data);

/**
 * @brief AnboxVhalIsAvailableCallback is invoked when checking if the Android
 * VHAL is supported and available.
 * @return 1 if available, 0 otherwise
 */
typedef bool (*AnboxVhalIsAvailableCallback)(void *user_data);

/**
 * @brief AnboxVhalGetCallback is invoked when requesting property values.
 * @return ANBOX_VHAL_ANSWER_STATUS_OK on success, error code otherwise.
 */
typedef AnboxVhalAnswerStatus (*AnboxVhalGetCallback)(
  AnboxVhalCommandGet *request, AnboxVhalPropertyValue *result,
  void *user_data);

/**
 * @brief AnboxVhalSetCallback is invoked when requesting to change property
 * values.
 * @return ANBOX_VHAL_ANSWER_STATUS_OK on success, error code otherwise.
 */
typedef AnboxVhalAnswerStatus (*AnboxVhalSetCallback)(
  AnboxVhalCommandSet *request, void *user_data);

/**
 * @brief AnboxVhalConnectorCallbacks is the structure holding all VHAL-related
 * callbacks.
 */
struct AnboxVhalConnectorCallbacks {
  AnboxVhalGetAllPropConfigsCallback get_all_prop_configs_callback = nullptr;
  AnboxVhalGetPropConfigsCallback get_prop_configs_callback = nullptr;
  AnboxVhalIsAvailableCallback is_available_callback = nullptr;
  AnboxVhalGetCallback get_callback = nullptr;
  AnboxVhalSetCallback set_callback = nullptr;
};

namespace anbox {
/**
 * @brief Connects a platform with the Android VHAL interface. The platform can
 * invoke the callbacks which are provided by Anbox on demand when answering a
 * request.
 */
class VhalConnector {
 public:
  VhalConnector() = default;
  virtual ~VhalConnector() = default;
  VhalConnector(const VhalConnector &) = delete;
  VhalConnector &operator=(const VhalConnector &) = delete;

  /**
   * @brief Check if the Android VHAL is supported and available.
   *
   * @return true if available, false otherwise
   */
  bool is_available() {
    if (!callbacks_.is_available_callback)
      return false;
    return callbacks_.is_available_callback(user_data_);
  }

  /**
   * @brief Get all property configs.
   *
   * @param result structure holding the returned configs.
   * The 'configs' member will be allocated on the heap, as well as all arrays
   * for each AnboxVhalPropertyConfig (area_configs, config_array,
   * config_string, prop_name). Each AnboxVhalAreaConfig has the area_names
   * array also allocated on the heap. The caller is responsible for freeing
   * their memory after the call.
   * @return ANBOX_VHAL_ANSWER_STATUS_OK on success, error code otherwise.
   */
  AnboxVhalAnswerStatus get_all_prop_configs(
    AnboxVhalAnswerGetConfigs *result) {
    if (!callbacks_.get_all_prop_configs_callback)
      return ANBOX_VHAL_ANSWER_STATUS_INVALID;
    return callbacks_.get_all_prop_configs_callback(result, user_data_);
  }

  /**
   * @brief Get requested property configs.
   *
   * @param props pointer to an array of size props_size. The array will not be
   * modified by the callee.
   * @param props_size Size of the array pointed to by props.
   * @param result structure holding the returned configs.
   * The 'configs' member will be allocated on the heap, as well as all arrays
   * for each AnboxVhalPropertyConfig (area_configs, config_array,
   * config_string, prop_name). The caller is responsible for freeing their
   * memory after the call.
   * @return ANBOX_VHAL_ANSWER_STATUS_OK on success, error code otherwise.
   */
  AnboxVhalAnswerStatus get_prop_configs(int32_t *props, size_t props_size,
                                         AnboxVhalAnswerGetConfigs *result) {
    if (!callbacks_.get_prop_configs_callback)
      return ANBOX_VHAL_ANSWER_STATUS_INVALID;
    return callbacks_.get_prop_configs_callback(props, props_size, result,
                                                user_data_);
  }

  /**
   * @brief Get the requested value
   *
   * @param request structure holding the Get request. It will not be modified
   * by the callee.
   * @param result structure holding the returned value.
   * All array members (int32_values, int64_values, float_values, bytes,
   * string_value) are allocated on the heap. The caller is responsible for
   * freeing their memory after the call.
   * @return ANBOX_VHAL_ANSWER_STATUS_OK on success, error code otherwise.
   */
  AnboxVhalAnswerStatus get(AnboxVhalCommandGet *request,
                            AnboxVhalPropertyValue *result) {
    if (!callbacks_.get_callback)
      return ANBOX_VHAL_ANSWER_STATUS_INVALID;
    return callbacks_.get_callback(request, result, user_data_);
  }

  /**
   * @brief Set the requested value
   *
   * @param request structure holding the Set request. It will not be modified
   * by the callee.
   * @return ANBOX_VHAL_ANSWER_STATUS_OK on success, error code otherwise.
   */
  AnboxVhalAnswerStatus set(AnboxVhalCommandSet *request) {
    if (!callbacks_.set_callback)
      return ANBOX_VHAL_ANSWER_STATUS_INVALID;
    return callbacks_.set_callback(request, user_data_);
  }

  /**
   * @brief Set all VHAL-related callbacks.
   *
   * @param callbacks Structure holding all the callbacks.
   * @param user_data Pointer that will be passed to callbacks.
   */
  void set_callbacks(const AnboxVhalConnectorCallbacks &callbacks,
                     void *user_data) {
    callbacks_ = callbacks;
    user_data_ = user_data;
  }

 private:
  AnboxVhalConnectorCallbacks callbacks_;
  void *user_data_{nullptr};
};
} // namespace anbox

#endif
