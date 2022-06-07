#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

list(APPEND TARGET_AZURERTOS_NANOBOOTER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_MonitorCommands.c)

# need to include nano Ring Buffer
list(APPEND TARGET_AZURERTOS_NANOBOOTER_SOURCES ${CMAKE_SOURCE_DIR}/src/CLR/Helpers/NanoRingBuffer/NanoRingBuffer.c)

# make var global
set(TARGET_AZURERTOS_NANOBOOTER_SOURCES ${TARGET_AZURERTOS_NANOBOOTER_SOURCES} CACHE INTERNAL "make global")
