#
# Anbox - The Android in a Box runtime environment
# Copyright 2018 Canonical Ltd.  All rights reserved.
#
project(ANBOX_PLATFORM_TESTER)
cmake_minimum_required(VERSION 3.1.3)

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to release")
    set(CMAKE_BUILD_TYPE "release")
endif()

# As we add the platform tester as an external project in cmake config file to ease
# platform tester compilation together with custom platform plugin, we don't use
# `find_package` to prevent platform tester from getting building recursively
get_filename_component(ANBOX_SDK_PATH "../" ABSOLUTE)
include(${ANBOX_SDK_PATH}/lib/anbox-platform-sdk/module/anbox-platform-sdk.cmake)

find_package(GMock)
include(FindPkgConfig)
pkg_check_modules(ELF libelf REQUIRED)

set(SOURCES
    main.cpp)

add_executable(anbox-platform-tester ${SOURCES})

set_target_properties(anbox-platform-tester PROPERTIES COMPILE_FLAGS "-std=c++14")

# The only additional include directories we need to specify are the ELF
# include directories, as everything else is taken care of when we link against
# the libraries below.
target_include_directories(anbox-platform-tester PUBLIC
    ${ELF_INCLUDE_DIRS}
)

# Link against the 
target_link_libraries(anbox-platform-tester
    anbox-platform-sdk-internal
    ${GMOCK_LIBRARIES}
    ${GTEST_BOTH_LIBRARIES}
    ${ELF_LIBRARIES}
    dl)

install(
    TARGETS anbox-platform-tester
    RUNTIME DESTINATION bin)
