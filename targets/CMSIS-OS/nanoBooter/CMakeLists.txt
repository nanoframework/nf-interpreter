#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# append target CMSIS-OS nanoBooter source files 
# list(APPEND TARGET_CMSIS_NANOBOOTER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_ReceiverThread.c")

# PAL source files 
list(APPEND TARGET_CMSIS_NANOBOOTER_SOURCES "${PROJECT_SOURCE_DIR}/src/PAL/BlockStorage/nanoPAL_BlockStorage.c")

# make var global
set(TARGET_CMSIS_NANOBOOTER_SOURCES ${TARGET_CMSIS_NANOBOOTER_SOURCES} CACHE INTERNAL "make global")

# appendtarget CMSIS-OS nanoBooter include directory
list(APPEND TARGET_CMSIS_NANOBOOTER_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}")
# make var global
set(TARGET_CMSIS_NANOBOOTER_INCLUDE_DIRS ${TARGET_CMSIS_NANOBOOTER_INCLUDE_DIRS} CACHE INTERNAL "make global")
