#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/swo.h.in"
"${CMAKE_CURRENT_BINARY_DIR}/swo.h" @ONLY)

# append include directory for target CMSIS, need this to be the binary dir because the header is a config file
list(APPEND TARGET_CMSIS_COMMON_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR})

# make var global
set(TARGET_CMSIS_COMMON_INCLUDE_DIRS ${TARGET_CMSIS_COMMON_INCLUDE_DIRS} CACHE INTERNAL "make global")
