# This file is part of Anbox Platform SDK
#
# Copyright 2021 Canonical Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

project(ANBOX_PLATFORM_SDK_EXAMPLES)
cmake_minimum_required(VERSION 3.10.2)

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
    camera
    direct_rendering
    nvidia)

foreach(platform ${PLATFORMS})
    string(TOLOWER ${platform} platform_name)
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/${platform_name}")
endforeach()
