#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

cmake_minimum_required(VERSION 3.21)
project(nanoCLR_Windows_TLS LANGUAGES C)

include(FetchContent)
include("${CMAKE_CURRENT_LIST_DIR}/../../CMake/binutils.common.cmake")

set(ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(ENABLE_PROGRAMS OFF CACHE BOOL "" FORCE)
set(USE_STATIC_MBEDTLS_LIBRARY ON CACHE BOOL "" FORCE)
set(USE_SHARED_MBEDTLS_LIBRARY OFF CACHE BOOL "" FORCE)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
nf_add_mbedtls_library()
FetchContent_GetProperties(mbedtls)

install(TARGETS mbedtls mbedx509 mbedcrypto ARCHIVE DESTINATION lib)
foreach(dependency everest p256m)
    if(TARGET ${dependency})
        install(TARGETS ${dependency} ARCHIVE DESTINATION lib)
    endif()
endforeach()
install(DIRECTORY "${mbedtls_SOURCE_DIR}/include/" DESTINATION include)
install(FILES "${mbedtls_SOURCE_DIR}/LICENSE" DESTINATION share/nanoFramework RENAME LICENSE-mbedTLS.txt)
