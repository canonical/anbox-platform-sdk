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

#ifndef ANBOX_SDK_GPS_PROCESSOR_H_
#define ANBOX_SDK_GPS_PROCESSOR_H_

#include "anbox-platform-sdk/types.h"

#include <stdint.h>
#include <stddef.h>
#include <cstdint>

namespace anbox {
/**
 * @brief GpsProcessor allows forwarding the gps data from platform plugin to
 * Android container and process gps data which is in one of the following forms
 *   . GGA NEMA sentence
 *   . RMC NEMA sentence
 *   . GNSS NEMA sentence
 * Please check http://www.gpsinformation.org/dale/nmea.htm out for more details
 * about the above NEMA sentence.
 */
class GpsProcessor {
  public:
    GpsProcessor() = default;
    virtual ~GpsProcessor() = default;
    GpsProcessor(const GpsProcessor &) = delete;
    GpsProcessor& operator=(const GpsProcessor &) = delete;

    /**
     * @brief Read available gps data.
     *
     * Anbox will call read_data() to query the plugin for the next available
     * gps data which is then forwarded to the Android container. If no gps
     * data is available and \a timeout is set to -1, the function must block
     * until gps data is available.
     *
     * @param gps data provided by the processor on a successful read
     * @param timeout maximum number of milliseconds to wait for the next available gps data.
     * The following possible values for \a timeout must be handled:
     * | \a timeout  | Behavior |
     * | ------------- | ------------- |
     * | 0   | Non-blocking mode; return -EIO immediately if no gps data to process. |
     * | < 0 | Block indefinitely until a gps data is available. |
     * | > 0 | Wait up to a maximum of \a timeout milliseconds for a gps data. |
     * @return 0 on success otherwise returns EINVAL on error occurs.
     */
    virtual int read_data(AnboxGpsData* data, int timeout) = 0;

    /**
     * @brief Inject gps data into AnboxPlatform.
     *
     * This function allows injecting gps data into the platform and letting
     * the GpsProcessor handle the gps data.
     *
     * @param data a chunk of gps data to be pushed into the internal queue.
     * @return 0 on success, otherwise returns EINVAL on error occurs.
     * @note This function is only used in our test suite to facilitate our automation
     *       tests and it is subject to change at any time.
     **/
    virtual int inject_data(AnboxGpsData data) = 0;
};
} // namespace anbox

#endif
