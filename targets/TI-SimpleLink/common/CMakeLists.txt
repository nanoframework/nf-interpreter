#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# append target TI SimpleLink common source files 
list(APPEND TARGET_TI_SimpleLink_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/platform_heap.c")

list(APPEND TARGET_TI_SimpleLink_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_ReceiverThread.c")
list(APPEND TARGET_TI_SimpleLink_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_HAL_Interface.c")

list(APPEND TARGET_TI_SimpleLink_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/GenericPort.cpp")
list(APPEND TARGET_TI_SimpleLink_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/syscalls.c")
# list(APPEND TARGET_TI_SimpleLink_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/nanoSupport_CRC32.c")

# append nanoHAL
list(APPEND TARGET_TI_SimpleLink_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/platformHAL.c")
list(APPEND TARGET_TI_SimpleLink_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/platformHAL_Time.cpp")

# append configuration manager according to target
if("${TARGET_SERIES}" STREQUAL "CC32xx")
    
    list(APPEND TARGET_TI_SimpleLink_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/CC32xxFlash_BlockStorageInterface.c")

    # block storage and configration block
    list(APPEND TARGET_TI_SimpleLink_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Target_BlockStorage_CC32xxFlashDriver.c")
    list(APPEND TARGET_TI_SimpleLink_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/targetHAL_ConfigurationManager_CC32xx.cpp")

    # append sockets and SSL driver
    list(APPEND TARGET_TI_SimpleLink_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/simplelink_sockets_functions.cpp")
    list(APPEND TARGET_TI_SimpleLink_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/simplelink_sockets.cpp")
    list(APPEND TARGET_TI_SimpleLink_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/sockets_simplelink.cpp")
    list(APPEND TARGET_TI_SimpleLink_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/ssl_simplelink.cpp")

elseif("${TARGET_SERIES}" STREQUAL "CC13x2_26x2")
    
    list(APPEND TARGET_TI_SimpleLink_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/CC13x2_26x2Flash_BlockStorageInterface.c")

    list(APPEND TARGET_TI_SimpleLink_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Target_BlockStorage_CC13x2_26x2FlashDriver.c")
    # list(APPEND TARGET_TI_SimpleLink_COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/targetHAL_ConfigurationManager_CC13x2_26x2.cpp")
endif()

# make var global
set(TARGET_TI_SimpleLink_COMMON_SOURCES ${TARGET_TI_SimpleLink_COMMON_SOURCES} CACHE INTERNAL "make global")
