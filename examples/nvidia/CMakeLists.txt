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

project(ANBOX_PLATFORM_PLUGIN_NVIDIA)
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
if("${CMAKE_PROJECT_NAME}" STREQUAL "ANBOX_PLATFORM_PLUGIN_NVIDIA")
  find_package(anbox-platform-sdk REQUIRED)
endif()

set(PLATFORM_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/anbox/platforms/nvidia)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPLATFORM_INSTALL_DIR=\\\"${CMAKE_INSTALL_PREFIX}/${PLATFORM_INSTALL_DIR}\\\"")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSYSTEM_LIBDIR=\\\"${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}\\\"")

# Add the plugin target
set(NVIDIA_PLATFORM_SOURCES nvidia_platform.cpp)
add_library(AnboxNvidiaPlatform
  SHARED ${NVIDIA_PLATFORM_SOURCES})

# Link against the anbox-platform-sdk-internal library
target_link_libraries(AnboxNvidiaPlatform PUBLIC
  anbox-platform-sdk-internal)

# Need to include the build directory for the configured file "arch.h"
target_include_directories(AnboxNvidiaPlatform
  PUBLIC ${CMAKE_BINARY_DIR})

# All platforms need to follow the naming scheme platform_<name>.so
set_target_properties(
    AnboxNvidiaPlatform PROPERTIES
  OUTPUT_NAME platform_nvidia
  PREFIX ""
  SUFFIX ".so"
  COMPILE_FLAGS "-fPIC -std=c++14"
  INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${PLATFORM_INSTALL_DIR}")

# Run the validation test suites against the platform implementation
add_test(NAME AudioStreamingPlatformValidation
    COMMAND ${ANBOX_PLATFORM_TESTER} --gtest_filter=-PlatformSensorProcessorTest.*:PlatformGpsProcessorTest.*:PlatformCameraProcessorTest.*:PlatformProxyTest.SendMessageWhenNotSet ${CMAKE_CURRENT_BINARY_DIR}/platform_nvidia.so)
