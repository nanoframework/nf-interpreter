#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# include SPIFFS sources, if feature is enabled
if(NF_FEATURE_USE_SPIFFS)
    list(APPEND TARGET_CHIBIOS_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_spiffs.c")
    list(APPEND TARGET_CHIBIOS_COMMON_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}")
endif()

# make var global
set(TARGET_CHIBIOS_COMMON_SOURCES ${TARGET_CHIBIOS_COMMON_SOURCES} CACHE INTERNAL "make global")
set(TARGET_CHIBIOS_COMMON_INCLUDE_DIRS ${TARGET_CHIBIOS_COMMON_INCLUDE_DIRS} CACHE INTERNAL "make global")
