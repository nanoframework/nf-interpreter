#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# append target CMSIS-OS nanoCLR source files 
# list(APPEND TARGET_CMSIS_NANOCLR_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR_dat.s)

# make var global
set(TARGET_CMSIS_NANOCLR_SOURCES ${TARGET_CMSIS_NANOCLR_SOURCES} CACHE INTERNAL "make global")


# append target CMSIS-OS nanoCLR include directory
list(APPEND TARGET_CMSIS_NANOCLR_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}")
# make var global
set(TARGET_CMSIS_NANOCLR_INCLUDE_DIRS ${TARGET_CMSIS_NANOCLR_INCLUDE_DIRS} CACHE INTERNAL "make global")
