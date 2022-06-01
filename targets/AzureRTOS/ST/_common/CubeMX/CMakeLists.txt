#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# append include directory for target Azure RTOS
list(APPEND TARGET_AZURERTOS_COMMON_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})

# make var global
set(TARGET_AZURERTOS_COMMON_INCLUDE_DIRS ${TARGET_AZURERTOS_COMMON_INCLUDE_DIRS} CACHE INTERNAL "make global")

# make vars global
set(TARGET_AZURERTOS_COMMON_SOURCES ${TARGET_AZURERTOS_COMMON_SOURCES} CACHE INTERNAL "make global")
