# Anbox - The Android in a Box runtime environment
# Copyright 2018 Canonical Ltd.  All rights reserved.

# Add the interface library target
add_library(anbox-platform-sdk-internal INTERFACE)

# Add the include directories for the target using generator expressions
# this ensures that the proper include directory is used regardless of whether this
# target is being built stand-alone or whether it is being built as a dependent
target_include_directories(anbox-platform-sdk-internal INTERFACE
	$<BUILD_INTERFACE:${anbox-platform-sdk_SOURCE_DIR}/include> # for source headers when building
	$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include> # for generated headers when building
	$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>) # for client in install mode

# Same for the library source as the headers - because this is an interface only target, we have to set the cpp 
# target source, which then gets included in the the list of files to compile when another target links
# against the interface target, but because this happens both inside the build of the sdk itself as well
# as after having been installed and going to be used by the user, we have to use generator expressions to create the source files
set(SOURCE
	# For building the sdk itself internally - use the source directories
    $<BUILD_INTERFACE:${anbox-platform-sdk_SOURCE_DIR}/src/registry.cpp>
    $<BUILD_INTERFACE:${anbox-platform-sdk_SOURCE_DIR}/src/public_api.cpp>
     # For client in install mode - use the INSTALL_PREFIX
    $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/src/registry.cpp>
    $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/src/public_api.cpp>)
target_sources(anbox-platform-sdk-internal INTERFACE ${SOURCE})

# Install the library (note that this doesn't install any files, it only sets up the CMake targets to be imported)
install(TARGETS anbox-platform-sdk-internal EXPORT anbox-platform-sdk DESTINATION "${ANBOX_MAIN_LIB_DEST}" COMPONENT export)
