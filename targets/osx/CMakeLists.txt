#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

cmake_minimum_required(VERSION 3.24)

project(nanoFramework_nanoCLR_osx VERSION 0.1.0 LANGUAGES C CXX)

if(NOT APPLE)
    message(FATAL_ERROR "targets/osx can only be built on macOS.")
endif()

set(NANO_OSX_ARCH "arm64" CACHE STRING "Target architecture for macOS host build (arm64 or x86_64)")
set_property(CACHE NANO_OSX_ARCH PROPERTY STRINGS arm64 x86_64)

if(NOT NANO_OSX_ARCH STREQUAL "arm64" AND NOT NANO_OSX_ARCH STREQUAL "x86_64")
    message(FATAL_ERROR "Invalid NANO_OSX_ARCH='${NANO_OSX_ARCH}'. Valid values: arm64, x86_64.")
endif()

set(CMAKE_OSX_ARCHITECTURES "${NANO_OSX_ARCH}" CACHE STRING "macOS architecture" FORCE)

option(NANO_OSX_ENABLE_SMOKE "Build smoke behavior (--help/--version/banner)." ON)

add_subdirectory(nanoCLR)
