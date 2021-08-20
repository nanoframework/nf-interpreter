#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# set include directories for nanoFramework network
list(APPEND NF_Network_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets)
list(APPEND NF_Network_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl)
list(APPEND NF_Network_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/PAL/Lwip)
list(APPEND NF_Network_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/Networking.Sntp)

# source files for nanoFramework Networking
set(NF_Network_SRCS

    #pal Socket
    sockets_lwip.cpp

    #Lwip 
    lwIP_Sockets.cpp
    lwIP_Sockets_functions.cpp 

    # platform specific
    Target_Network.cpp
    targetHAL_Network.cpp
)

# need a conditional include because of ESP32 building network as a library 
if(NOT USE_SECURITY_MBEDTLS_OPTION)
    list(APPEND NF_Network_SRCS ssl_stubs.cpp)
endif()

# source files for security layer
set(NF_Network_Security_SRCS

    ssl.cpp
    ssl_accept_internal.cpp
    ssl_add_cert_auth_internal.cpp
    ssl_close_socket_internal.cpp
    ssl_connect_internal.cpp
    ssl_decode_private_key_internal.cpp
    ssl_exit_context_internal.cpp
    ssl_generic.cpp
    ssl_generic_init_internal.cpp
    ssl_initialize_internal.cpp
    ssl_parse_certificate_internal.cpp
    ssl_available_internal.cpp
    ssl_read_internal.cpp
    ssl_uninitialize_internal.cpp
    ssl_write_internal.cpp
)

if(NF_FEATURE_DEBUGGER)
    list(APPEND NF_Network_SRCS Sockets_debugger.cpp)
endif()

# add Enc28j60source files in two steps
if(USE_ENC28J60_DRIVER_OPTION)

    set(NF_Network_Driver_Srcs
        enc28j60_lwip_config_stubs.cpp
        enc28j60_lwip.cpp
        enc28j60_lwip_driver.cpp
    )

endif()

# add source files in two steps
# 1st pass: common source files
foreach(SRC_FILE ${NF_Network_SRCS})

    set(NF_Network_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(NF_Network_SRC_FILE ${SRC_FILE}
        PATHS

            ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets
            ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl
            ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/mbedTLS
            ${CMAKE_SOURCE_DIR}/src/PAL/Lwip
            ${CMAKE_SOURCE_DIR}/targets/${RTOS}/_common

            ${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}
 
        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${NF_Network_SRC_FILE}")
    endif()

    list(APPEND NF_Network_SOURCES ${NF_Network_SRC_FILE})

endforeach()

if(USE_SECURITY_MBEDTLS_OPTION)

    set(NF_Security_Search_Path "${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/mbedTLS")

    # 2nd pass: security files if option is selected 
    foreach(SRC_FILE ${NF_Network_Security_SRCS})

        set(NF_Network_SRC_FILE SRC_FILE-NOTFOUND)

        find_file(NF_Network_SRC_FILE ${SRC_FILE}
            PATHS 

                ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl
                ${NF_Security_Search_Path}
    
            CMAKE_FIND_ROOT_PATH_BOTH
        )

        if (BUILD_VERBOSE)
            message("${SRC_FILE} >> ${NF_Network_SRC_FILE}")
        endif()
    
        list(APPEND NF_Network_SOURCES ${NF_Network_SRC_FILE})

    endforeach()

endif()

if(Use_Networking_Extra_Driver)

    # 3rd pass: Any extra driver files 
    foreach(SRC_FILE ${NF_Network_Driver_Srcs})

        set(NF_Network_SRC_FILE SRC_FILE-NOTFOUND)

        find_file(NF_Network_SRC_FILE ${SRC_FILE}
            PATHS 
                ${NF_Network_Driver_Path}
    
            CMAKE_FIND_ROOT_PATH_BOTH
        )

        if (BUILD_VERBOSE)
            message("${SRC_FILE} >> ${NF_Network_Driver_Srcs}")
        endif()
    
        list(APPEND NF_Network_SOURCES ${NF_Network_SRC_FILE})
        
    endforeach()

endif()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(NF_Network DEFAULT_MSG NF_Network_INCLUDE_DIRS NF_Network_SOURCES)

# macro to be called from binutils to add network library
# TARGET parameter to set the target it's building
# optional EXTRA_SOURCES with source files to be added to the library
# optional EXTRA_INCLUDES with include paths to be added to the library
# optional EXTRA_COMPILE_DEFINITIONS with compiler definitions to be added to the library
# optional EXTRA_COMPILER_OPTIONS with compiler options to be added to the library
macro(nf_add_lib_network)

    # parse arguments
    cmake_parse_arguments(_ "" "TARGET;EXTRA_COMPILE_DEFINITIONS" "EXTRA_SOURCES;EXTRA_INCLUDES;EXTRA_COMPILER_OPTIONS" ${ARGN})

    if(NOT __TARGET OR "${__TARGET}" STREQUAL "")
        message(FATAL_ERROR "Need to set TARGET argument when calling nf_add_lib_network()")
    endif()

    # add this has a library
    set(LIB_NAME NF_Network)

    add_library(
        ${LIB_NAME} STATIC 
            ${NF_Network_SOURCES}
            ${mbedTLS_SOURCES}
            ${__EXTRA_SOURCES})

    target_include_directories(
        ${LIB_NAME} 
        PUBLIC 
            ${NF_Network_INCLUDE_DIRS}
            ${NF_CoreCLR_INCLUDE_DIRS}
            ${mbedTLS_INCLUDE_DIRS}
            ${__EXTRA_INCLUDES})

    nf_set_compiler_options(${LIB_NAME})
    nf_set_compile_definitions(TARGET ${LIB_NAME} BUILD_TARGET ${__TARGET} EXTRA_COMPILE_DEFINITIONS ${__EXTRA_COMPILE_DEFINITIONS})

    target_compile_options(${LIB_NAME} PUBLIC ${__EXTRA_COMPILER_OPTIONS})

    # add alias
    add_library("nano::${LIB_NAME}" ALIAS ${LIB_NAME})
    
endmacro()
