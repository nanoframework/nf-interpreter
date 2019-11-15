#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# ST common includes
set(TARGET_FREERTOS_ST_COMMON_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE INTERNAL "make global")

# append nanoHAL
list(APPEND TARGET_FREERTOS_ST_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/targetHAL.c)

# make vars global
set(TARGET_FREERTOS_ST_COMMON_SOURCES ${TARGET_FREERTOS_ST_COMMON_SOURCES} CACHE INTERNAL "make global")
