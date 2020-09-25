#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# append include directory for target ChibiOS
list(APPEND TARGET_CHIBIOS_COMMON_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}")

# make var global
set(TARGET_CHIBIOS_COMMON_INCLUDE_DIRS ${TARGET_CHIBIOS_COMMON_INCLUDE_DIRS} CACHE INTERNAL "make global")

# add header with target platform definitions
if(CHIBIOS_COMMUNITY_TARGET)
    configure_file("${CMAKE_SOURCE_DIR}/CMake/ChibiOS_target_os.h.in"
                    "${CMAKE_BINARY_DIR}/targets-community/CMSIS-OS/ChibiOS/${CHIBIOS_BOARD}/target_os.h" @ONLY)
else()
    configure_file("${CMAKE_SOURCE_DIR}/CMake/ChibiOS_target_os.h.in"
                    "${CMAKE_BINARY_DIR}/targets/CMSIS-OS/ChibiOS/${CHIBIOS_BOARD}/target_os.h" @ONLY)
endif()
