#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# append target CMSIS-OS common source files 
# list(APPEND TARGET_CMSIS_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_ReceiverThread.c")
# make var global
set(TARGET_CMSIS_COMMON_SOURCES ${TARGET_CMSIS_COMMON_SOURCES} CACHE INTERNAL "make global")

# append CMSIS-OS common include directory
list(APPEND TARGET_CMSIS_COMMON_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}")
# make var global
set(TARGET_CMSIS_COMMON_INCLUDE_DIRS ${TARGET_CMSIS_COMMON_INCLUDE_DIRS} CACHE INTERNAL "make global")
