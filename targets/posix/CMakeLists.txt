#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

cmake_minimum_required(VERSION 3.24)

project(nanoFramework_nanoCLR_posix VERSION 0.1.0 LANGUAGES C CXX)

if(NOT UNIX)
    message(FATAL_ERROR "targets/posix can only be built on POSIX platforms (macOS or Linux).")
endif()

# Architecture selection is only meaningful on macOS (Apple Silicon vs Intel).
# On Linux the build uses the host toolchain architecture by default.
if(APPLE)
    set(NANO_POSIX_ARCH "arm64" CACHE STRING "Target architecture for macOS host build (arm64 or x86_64)")
    set_property(CACHE NANO_POSIX_ARCH PROPERTY STRINGS arm64 x86_64)

    if(NOT NANO_POSIX_ARCH STREQUAL "arm64" AND NOT NANO_POSIX_ARCH STREQUAL "x86_64")
        message(FATAL_ERROR "Invalid NANO_POSIX_ARCH='${NANO_POSIX_ARCH}'. Valid values: arm64, x86_64.")
    endif()

    set(CMAKE_OSX_ARCHITECTURES "${NANO_POSIX_ARCH}" CACHE STRING "macOS architecture" FORCE)
endif()

option(NANO_POSIX_ENABLE_SMOKE "Build smoke behavior (banner/version output)." ON)

# Note: the standard embedded-target layout (nanoBooter/ + nanoCLR/ + common/) applies to
# board targets (ChibiOS, ThreadX, ESP32, etc.).  Host/virtual targets (targets/win32,
# targets/posix) intentionally omit nanoBooter/ and common/ — there is no bootloader
# stage on a POSIX host, and platform glue lives directly in the nanoCLR/ tree.
add_subdirectory(nanoCLR)
