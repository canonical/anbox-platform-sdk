// Anbox - The Android in a Box runtime environment
// Copyright 2018 Canonical Ltd.  All rights reserved.
#ifndef ANBOX_SDK_PLATFORM_VERSION_H_
#define ANBOX_SDK_PLATFORM_VERSION_H_

#include <stdint.h>

#define ANBOX_PLATFORM_MAJOR_VERSION 1
#define ANBOX_PLATFORM_MINOR_VERSION 11
#define ANBOX_PLATFORM_PATCH_VERSION 0

#define ANBOX_PLATFORM_VERSION 11100

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
