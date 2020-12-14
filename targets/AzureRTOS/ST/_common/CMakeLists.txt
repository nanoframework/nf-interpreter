#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# Azure RTOS ST specific file
list(APPEND TARGET_AZURERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/tx_initialize_low_level.S)
list(APPEND TARGET_AZURERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Target_BlockStorage_STM32FlashDriver.c)
list(APPEND TARGET_AZURERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_HAL_Interface.c)
list(APPEND TARGET_AZURERTOS_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/nanoSupport_CRC32.c)

# make vars global
set(TARGET_AZURERTOS_COMMON_SOURCES ${TARGET_AZURERTOS_COMMON_SOURCES} CACHE INTERNAL "make global")
