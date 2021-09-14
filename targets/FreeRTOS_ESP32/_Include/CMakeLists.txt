#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# append include directory for target 
list(APPEND TARGET_ESP32_IDF_COMMON_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})

# make var global
set(TARGET_ESP32_IDF_COMMON_INCLUDE_DIRS ${TARGET_ESP32_IDF_COMMON_INCLUDE_DIRS} CACHE INTERNAL "make global")
