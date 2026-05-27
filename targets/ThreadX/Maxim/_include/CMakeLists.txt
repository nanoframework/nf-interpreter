#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# append include directory for target ThreadX
list(APPEND TARGET_THREADX_COMMON_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})

# need to include nano Ring Buffer
list(APPEND TARGET_THREADX_COMMON_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/CLR/Helpers/NanoRingBuffer)

# make var global
set(TARGET_THREADX_COMMON_INCLUDE_DIRS ${TARGET_THREADX_COMMON_INCLUDE_DIRS} CACHE INTERNAL "make global")
