#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# append source files required for ChibiOS interface  
list(APPEND COMMON_PROJECT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Device_BlockStorage$<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:-DEBUG>.c)
list(APPEND COMMON_PROJECT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_HAL_Interface.c)
list(APPEND COMMON_PROJECT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/startup_stm32l072xx.s)
list(APPEND COMMON_PROJECT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/system_stm32l0xx.c)
list(APPEND COMMON_PROJECT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/common.c)

# make var global
set(COMMON_PROJECT_SOURCES ${COMMON_PROJECT_SOURCES} CACHE INTERNAL "make global")
