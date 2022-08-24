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

option(ANBOX_UNSUPPORTED_PLATFORM_CHECK "Use unsupported platform" ON)
if(ANBOX_UNSUPPORTED_PLATFORM_CHECK)
    execute_process(COMMAND bash -c "cat /etc/lsb-release | grep -oP 'DISTRIB_RELEASE=\\K.*' | tr -d '\\n'"
        OUTPUT_VARIABLE ANBOX_SDK_UBUNTU_VERSION)
    if(NOT "${ANBOX_SDK_UBUNTU_VERSION}" STREQUAL "18.04" AND NOT "${ANBOX_SDK_UBUNTU_VERSION}" STREQUAL "22.04" )
        message(FATAL_ERROR "Invalid platform - only Ubuntu 18.04 and 22.04 are supported")
    endif()
else()
    message(WARNING "Unsupported platform - only Ubuntu 18.04 and 22.04 are supported")
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
