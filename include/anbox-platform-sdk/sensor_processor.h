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

#ifndef ANBOX_SDK_SENSOR_PROCESSOR_H_
#define ANBOX_SDK_SENSOR_PROCESSOR_H_

#include "anbox-platform-sdk/types.h"

#include <errno.h>

namespace anbox {
/**
 * @brief SensorProcessor allows processing sensor events from the Android container
 * and perform sensor processing for a variety of sensor devices.
 */
class SensorProcessor {
  public:
    SensorProcessor() = default;
    virtual ~SensorProcessor() = default;
    SensorProcessor(const SensorProcessor &) = delete;
    SensorProcessor& operator=(const SensorProcessor &) = delete;

    /**
     * @brief Sensors supported by this processor
     *
     * Internally Anbox will tell Android about the supported sensors and
     * ensure they are being exposed through the right Android APIs.
     * Multiple sensor types can be specified by using OR bitwise operator.
     *
     * @return sensors that are supported by this processor
     */
    virtual AnboxSensorType supported_sensors() const = 0;

    /**
     * @brief Read available sensor data.
     *
     * Anbox will call read_data() to query the plugin for the next available
     * sensor data which is then forwarded to the Android container. If no sensor
     * data is available and \a timeout is set to -1, the function must block until
     * sensor data is available.
     *
     * @param sensor data provided by the processor on a successful read
     * @param timeout maximum number of milliseconds to wait for the next available sensor data.
     * The following possible values for \a timeout must be handled:
     * | \a timeout  | Behavior |
     * | ------------- | ------------- |
     * | 0   | Non-blocking mode; return -EIO immediately if no sensor data to process. |
     * | < 0 | Block indefinitely until a sensor data is available. |
     * | > 0 | Wait up to a maximum of \a timeout milliseconds for a sensor data. |
     * @return 0 on success otherwise returns EINVAL on error occurs.
     */
    virtual int read_data(AnboxSensorData* data, int timeout) = 0;

    /**
     * @brief Inject sensor data into AnboxPlatform.
     *
     * This function allows injecting sensor data into the platform and letting
     * the SensorProcessor handle the sensor data.
     *
     * @param data a chunk of sensor data to be pushed into the internal queue.
     * @return 0 on success, otherwise returns EINVAL on error occurs.
     * @note This function is only used in our test suite to facilitate our automation
     *       tests and it is subject to change at any time.
     **/
    virtual int inject_data(AnboxSensorData data) = 0;

    /**
     * @brief Activate or deactivate a specific sensor
     *
     * Anbox will call this function whenever Android container requests to enable or
     * disable a specific sensor. The implementation should make sure the requested
     * sensor is properly initialized and starts producing data when activated, and
     * release or stop producing data when deactivated.
     *
     * @param type the type of the sensor to activate or deactivate
     * @param on true to activate (enable) the sensor, false to deactivate (disable) it
     * @return 0 on success otherwise returns EINVAL on error occurs.
     */
    virtual int activate_sensor(AnboxSensorType type, bool on) {
      (void) type;
      (void) on;
      return -EIO;
    }
};
} // namespace anbox

#endif
