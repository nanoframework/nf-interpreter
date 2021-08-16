#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

list(APPEND COMMON_PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/platform_heap.c")

# make vars global
set(COMMON_PROJECT_SOURCES ${TARGET_ESP32_COMMON_SOURCES} CACHE INTERNAL "make global")
