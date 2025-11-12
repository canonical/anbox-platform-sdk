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

#ifndef ANBOX_SDK_PLATFORM_VERSION_H_
#define ANBOX_SDK_PLATFORM_VERSION_H_

#include <stdint.h>

#define ANBOX_PLATFORM_MAJOR_VERSION 1
#define ANBOX_PLATFORM_MINOR_VERSION 27
#define ANBOX_PLATFORM_PATCH_VERSION 0

#define ANBOX_PLATFORM_VERSION 12700

/**
 * @brief      Get the individual version numbers from the combined version number.
 *
 * @param      version  The combined version, i.e. ANBOX_PLATFORM_VERSION or 10101 for version 1.1.1
 * @param      major    The major version
 * @param      minor    The minor version
 * @param      patch    The patch version
 *
 * @return     void
 */
inline void anbox_platform_get_versions(uint32_t version, uint32_t& major, uint32_t& minor, uint32_t& patch) {
  major = ((version / 10000) % 100);
  minor = ((version / 100) % 100);
  patch = (version % 100);
}

#endif
