#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# this one has to follow the declaration on src\CLR\Include\WireProtocol_Message.h
# #define TRACE_ERRORS 1
# #define TRACE_HEADERS 2
# #define TRACE_STATE 4
# #define TRACE_NODATA 8

# default to 0 (no Wire Protocol trace)
set(WP_TRACE_MASK 0 CACHE INTERNAL "WP trace mask")

if(NF_WP_TRACE_ERRORS)
    math(EXPR WP_TRACE_MASK "${WP_TRACE_MASK} + 1")
endif()
if(NF_WP_TRACE_HEADERS)
    math(EXPR WP_TRACE_MASK "${WP_TRACE_MASK} + 2")
endif()
if(NF_WP_TRACE_STATE)
    math(EXPR WP_TRACE_MASK "${WP_TRACE_MASK} + 4")
endif()
if(NF_WP_TRACE_NODATA)
    math(EXPR WP_TRACE_MASK "${WP_TRACE_MASK} + 8")
endif()
if(NF_WP_TRACE_VERBOSE)
    math(EXPR WP_TRACE_MASK "${WP_TRACE_MASK} + 16")
endif()
if(NF_WP_TRACE_ALL)
    math(EXPR WP_TRACE_MASK "16 + 8 + 4 + 2 + 1")
endif()

if (BUILD_VERBOSE)
    message(STATUS "Wire Protocol TRACE_MASK is '${WP_TRACE_MASK}'")
endif()

# report Wire Protocol transport channel
if(NF_WP_TRANSPORT_USB_CDC)
    message(STATUS "Wire Protocol transport: USB CDC")
elseif(NF_WP_TRANSPORT_USB_CDC_OR_SERIAL)
    message(STATUS "Wire Protocol transport: USB CDC or serial port (UART)")
else()
    message(STATUS "Wire Protocol transport: serial port (UART)")
endif()

# sanity checks for the Wire Protocol channels (nanoCLR)
if(NF_FEATURE_DEBUGGER)

    if(NOT NF_WP_CHANNEL_SERIAL AND NOT NF_WP_CHANNEL_NETWORK)
        message(FATAL_ERROR "

ERROR: managed application debugging is enabled but there is no Wire Protocol channel. Enable NF_WP_CHANNEL_SERIAL and/or NF_WP_CHANNEL_NETWORK.")
    endif()

    if(NF_WP_CHANNEL_NETWORK)

        if(TARGET_HAS_WIFI_ISM43362)
            message(FATAL_ERROR "

ERROR: NF_WP_CHANNEL_NETWORK is not supported with the ISM43362 Wi-Fi module.")
        endif()

        if(NOT RTOS_CHIBIOS_CHECK AND NOT RTOS_FREERTOS_CHECK AND NOT RTOS_ESP32_CHECK)
            message(FATAL_ERROR "

ERROR: NF_WP_CHANNEL_NETWORK is only supported on ChibiOS, FreeRTOS and ESP32 targets.")
        endif()

    endif()

endif()

# report Wire Protocol channels
if(NF_FEATURE_DEBUGGER)
    if(NF_WP_CHANNEL_NETWORK)
        message(STATUS "Wire Protocol network channel: enabled (TCP port ${NF_WP_NETWORK_PORT})")
        if(NF_WP_NETWORK_DISCOVERY)
            message(STATUS "Wire Protocol network discovery: UDP port ${NF_WP_NETWORK_DISCOVERY_PORT}")
        endif()
    else()
        message(STATUS "Wire Protocol network channel: disabled")
    endif()
endif()

# set include directories for Wire Protocol
list(APPEND WireProtocol_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/CLR/Include)

# source files for Wire Protocol
# on ChibiOS, FreeRTOS and ESP32 the channel dispatcher replaces the default (weak) HAL interface
# the platform serial implementation is provided as WP_Serial_*
if(RTOS_CHIBIOS_CHECK OR RTOS_FREERTOS_CHECK OR RTOS_ESP32_CHECK)
    set(WireProtocol_HAL_SRC WireProtocol_Transport.c)
else()
    set(WireProtocol_HAL_SRC WireProtocol_HAL_Interface.c)
endif()

set(WireProtocol_SRCS

    WireProtocol_Message.c
    WireProtocol_MonitorCommands.c
    ${WireProtocol_HAL_SRC}
    WireProtocol_App_Interface.c
    WireProtocol_Auth.c

    nanoSupport_CRC32.c
)

foreach(SRC_FILE ${WireProtocol_SRCS})

    set(WireProtocol_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(WireProtocol_SRC_FILE ${SRC_FILE}
        PATHS 
            ${CMAKE_SOURCE_DIR}/src/CLR/WireProtocol
            ${CMAKE_SOURCE_DIR}/src/CLR/Core

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${WireProtocol_SRC_FILE}")
    endif()

    list(APPEND WireProtocol_SOURCES ${WireProtocol_SRC_FILE})
    
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(WireProtocol DEFAULT_MSG WireProtocol_INCLUDE_DIRS WireProtocol_SOURCES)


# macro to be called from binutils to add Wire Protocol library
# optional EXTRA_INCLUDES with include paths to be added to the library
# optional EXTRA_COMPILE_DEFINITIONS with compiler definitions to be added to the library
macro(nf_add_lib_wireprotocol)

    # parse arguments
    cmake_parse_arguments(NFAWP "" "" "EXTRA_INCLUDES;EXTRA_COMPILE_DEFINITIONS" ${ARGN})

    # add this has a library
    set(LIB_NAME WireProtocol)

    add_library(
        ${LIB_NAME} STATIC 
            ${WireProtocol_SOURCES})   

    target_include_directories(
        ${LIB_NAME} 
        PUBLIC 
            ${WireProtocol_INCLUDE_DIRS}
            ${NF_CoreCLR_INCLUDE_DIRS}
            ${NFAWP_EXTRA_INCLUDES})   

    # TODO can be removed later
    if(RTOS_ESP32_CHECK)

        nf_common_compiler_definitions(TARGET ${LIB_NAME} BUILD_TARGET ${NANOCLR_PROJECT_NAME})

        # this is the only one different
        target_compile_definitions(
            ${LIB_NAME} PUBLIC
            -DPLATFORM_ESP32
            ${NFAWP_EXTRA_COMPILER_DEFINITIONS}
        )

    else() 
        nf_set_compile_options(TARGET ${LIB_NAME})
        nf_set_compile_definitions(TARGET ${LIB_NAME} EXTRA_COMPILE_DEFINITIONS ${NFAWP_EXTRA_COMPILE_DEFINITIONS} BUILD_TARGET ${NANOCLR_PROJECT_NAME})
        nf_set_link_options(TARGET ${LIB_NAME})
    endif()

    # add alias
    add_library("nano::${LIB_NAME}" ALIAS ${LIB_NAME})
    
endmacro()
