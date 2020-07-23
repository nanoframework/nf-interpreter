#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

include(STM32_CubePackage)

# (default is OFF so STM Cube package is NOT included)
option(STM32_CUBE_PACKAGE_REQUIRED "option to include STM Cube package in the build")

if(STM32_CUBE_PACKAGE_REQUIRED)
    ProcessSTM32CubePackage()
endif()

# ST common directories
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/common)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/nanoBooter)
