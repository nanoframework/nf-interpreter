#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# include littlefs sources, if feature is enabled
if(NF_FEATURE_USE_LITTLEFS)
    list(APPEND TARGET_CHIBIOS_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_littlefs.c")
    list(APPEND TARGET_CHIBIOS_COMMON_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}")
endif()

# make var global
set(TARGET_CHIBIOS_COMMON_SOURCES ${TARGET_CHIBIOS_COMMON_SOURCES} CACHE INTERNAL "make global")
set(TARGET_CHIBIOS_COMMON_INCLUDE_DIRS ${TARGET_CHIBIOS_COMMON_INCLUDE_DIRS} CACHE INTERNAL "make global")