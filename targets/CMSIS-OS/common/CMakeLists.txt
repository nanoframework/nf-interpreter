#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# append target CMSIS-OS common source files 

# if SWO opting add source file
if(SWO_OUTPUT_OPTION)
    list(APPEND TARGET_CMSIS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/swo.cpp)
endif()

# include syscalls dummy calls, if needed
if(NOT RTOS_CHIBIOS_CHECK)
    list(APPEND TARGET_CMSIS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/syscalls.c)
endif()

# make var global
set(TARGET_CMSIS_COMMON_SOURCES ${TARGET_CMSIS_COMMON_SOURCES} CACHE INTERNAL "make global")
