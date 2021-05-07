#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# append include directory for target NXP
list(APPEND TARGET_NXP_COMMON_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}")

# make var global
set(TARGET_NXP_COMMON_INCLUDE_DIRS ${TARGET_NXP_COMMON_INCLUDE_DIRS} CACHE INTERNAL "make global")

# add header with target platform definitions
configure_file("${CMAKE_SOURCE_DIR}/CMake/FreeRTOS_target_os.h.in"
                "${CMAKE_BINARY_DIR}/targets/FreeRTOS/NXP/NXP_MIMXRT1060_EVK/target_os.h" @ONLY)