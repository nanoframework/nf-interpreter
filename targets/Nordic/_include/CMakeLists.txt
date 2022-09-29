#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# add header with target platform definitions
configure_file(${CMAKE_SOURCE_DIR}/CMake/Nordic_target_os.h.in
               ${CMAKE_BINARY_DIR}/targets/${RTOS}/${TARGET_BOARD}/target_os.h @ONLY)

# append include directory for target Zephyr RTOS
list(APPEND TARGET_NORDIC_COMMON_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})

# make var global
set(TARGET_NORDIC_COMMON_INCLUDE_DIRS ${TARGET_NORDIC_COMMON_INCLUDE_DIRS} CACHE INTERNAL "make global")

