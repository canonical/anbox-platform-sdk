# Anbox - The Android in a Box runtime environment
# Copyright 2018 Canonical Ltd.  All rights reserved.

project(ANBOX_PLATFORM_SDK_EXAMPLES)
cmake_minimum_required(VERSION 3.1.3)

include(CTest)
enable_testing()

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to release")
    set(CMAKE_BUILD_TYPE "release")
endif()

# Load the anbox-sdk cmake package if we are running stand-alone
# Otherwise if we are running inside the SDK build itself, we can't include
# the SDK obviously because it hasn't been built yet
if("${CMAKE_PROJECT_NAME}" STREQUAL "ANBOX_PLATFORM_SDK_EXAMPLES")
  find_package(anbox-platform-sdk REQUIRED)
endif()

set(PLATFORMS
    minimal
    audio_streaming
    sensor
    gps
    camera)

foreach(platform ${PLATFORMS})
    string(TOLOWER ${platform} platform_name)
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/${platform_name}")
endforeach()
