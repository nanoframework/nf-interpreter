#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# append target CMSIS-OS common source files 

# if SWO opting add source file
if(SWO_OUPUT_OPTION)
    list(APPEND TARGET_CMSIS_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/swo.cpp")
endif()

# make var global
set(TARGET_CMSIS_COMMON_SOURCES ${TARGET_CMSIS_COMMON_SOURCES} CACHE INTERNAL "make global")
