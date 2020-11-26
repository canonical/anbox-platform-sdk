# Anbox - The Android in a Box runtime environment
# Copyright 2018 Canonical Ltd.  All rights reserved.

project(ANBOX_PLATFORM_PLUGIN_MINIMAL)
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
if("${CMAKE_PROJECT_NAME}" STREQUAL "ANBOX_PLATFORM_PLUGIN_MINIMAL")
  find_package(anbox-platform-sdk REQUIRED)
endif()

# Add the plugin target
set(MINIMAL_PLATFORM_SOURCES minimal_platform.cpp)
add_library(AnboxMinimalPlatform 
  SHARED ${MINIMAL_PLATFORM_SOURCES})

set(PLATFORM_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/anbox/platforms/custom)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPLATFORM_INSTALL_DIR=\\\"${CMAKE_INSTALL_PREFIX}/${PLATFORM_INSTALL_DIR}\\\"")

# Link against the anbox-platform-sdk-internal library
target_link_libraries(AnboxMinimalPlatform
  anbox-platform-sdk-internal)

# All platforms need to follow the naming scheme platform_<name>.so
set_target_properties(
  AnboxMinimalPlatform PROPERTIES
  OUTPUT_NAME platform_minimal
  PREFIX ""
  SUFFIX ".so"
  COMPILE_FLAGS "-fPIC -std=c++14"
  INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${PLATFORM_INSTALL_DIR}")

# Run the validation test suites against the platform implementation
# Explicitly skip PlatformInputProcessorTest and PlatformAudioProcessorTest here,
# as minimal platform doesn't have well-implemented audio or input processor and it
# only aims to provide a basic implementation of an Anbox platform.
# For all production ready platform plugin, all tests need to pass.
add_test(NAME MinimalPlatformValidation
	COMMAND ${ANBOX_PLATFORM_TESTER} --gtest_filter=-PlatformInputProcessorTest.*:PlatformAudioProcessorTest.*:PlatformGraphicsProcessorTest.*:PlatformSensorProcessorTest.*:PlatformGpsProcessorTest.* ${CMAKE_CURRENT_BINARY_DIR}/platform_minimal.so)
