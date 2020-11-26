#
# Anbox - The Android in a Box runtime environment
# Copyright 2018 Canonical Ltd.  All rights reserved.
#

# Check to make sure the SDK is only used on Ubuntu 18.04 by default
option(ANBOX_UNSUPPORTED_PLATFORM_CHECK "Use unsupported platform" ON)
if(ANBOX_UNSUPPORTED_PLATFORM_CHECK)
    execute_process(COMMAND bash -c "cat /etc/lsb-release | grep -oP 'DISTRIB_RELEASE=\\K.*' | tr -d '\\n'"
        OUTPUT_VARIABLE ANBOX_SDK_UBUNTU_VERSION)
    if(NOT "${ANBOX_SDK_UBUNTU_VERSION}" STREQUAL "18.04")
        message(FATAL_ERROR "Invalid platform - only Ubuntu 18.04 is supported")
    endif()
else()
    message(WARNING "Unsupported platform - only Ubuntu 18.04 is supported")
endif()

get_filename_component(SELF_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
include(${SELF_DIR}/module/anbox-platform-sdk.cmake)

get_filename_component(ANBOX_SDK_LIB_PATH "${SELF_DIR}" DIRECTORY)
get_filename_component(ANBOX_SDK_PATH "${ANBOX_SDK_LIB_PATH}" DIRECTORY)

# Utilize cmake `externalproject_add` to build the Anbox platform tester on the fly
# when building custom platform plugin
include(ExternalProject)
ExternalProject_Add(ANBOX_PLATFORM_TESTER_PROJECT
  SOURCE_DIR ${ANBOX_SDK_PATH}/tool
  CMAKE_ARGS += -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
)

set(ANBOX_PLATFORM_TESTER "${CMAKE_CURRENT_BINARY_DIR}/bin/anbox-platform-tester"
    CACHE STRING "The path to anbox platform tester")
