# //
# // Copyright (c) 2017 The nanoFramework project contributors
# // See LICENSE file in the project root for full license information.
# //

# append target ChibiOS source files 
list(APPEND TARGET_CHIBIOS_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/LaunchCLR.c")
list(APPEND TARGET_CHIBIOS_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_ReceiverThread.c")
# make var global
set(TARGET_CHIBIOS_COMMON_SOURCES ${TARGET_CHIBIOS_COMMON_SOURCES} CACHE INTERNAL "make global")

# append target ChibiOS include directory
list(APPEND TARGET_CHIBIOS_COMMON_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}")
# make var global
set(TARGET_CHIBIOS_COMMON_INCLUDE_DIRS ${TARGET_CHIBIOS_COMMON_INCLUDE_DIRS} CACHE INTERNAL "make global")
