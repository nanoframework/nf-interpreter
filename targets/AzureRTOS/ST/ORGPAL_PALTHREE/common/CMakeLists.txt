#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# append common source files
list(APPEND COMMON_PROJECT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/usbcfg.c)
list(APPEND COMMON_PROJECT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Device_BlockStorage$<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:-DEBUG>.c)
list(APPEND COMMON_PROJECT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/targetHAL_ConfigurationManager.cpp)

list(APPEND NF_Network_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/CubeMX/stm32f7xx_hal_msp.c)

# make var global
set(COMMON_PROJECT_SOURCES ${COMMON_PROJECT_SOURCES} CACHE INTERNAL "make global")
