#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
FetchContent_GetProperties(mbedtls)

# set include directories for nanoFramework network
list(APPEND NF_Network_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets)
list(APPEND NF_Network_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl)
list(APPEND NF_Network_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/PAL/Lwip)
list(APPEND NF_Network_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/PAL)
list(APPEND NF_Network_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/Networking.Sntp)

if(USE_SECURITY_MBEDTLS_OPTION)
    list(APPEND NF_Network_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/mbedTLS)
    list(APPEND NF_Network_INCLUDE_DIRS ${mbedtls_SOURCE_DIR}/include)
endif()

if(USE_ENC28J60_DRIVER_OPTION)
    list(APPEND NF_Network_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/Network/Enc28j60)
    list(APPEND NF_Network_Driver_Path ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/Network/Enc28j60)
    set(Use_Networking_Extra_Driver TRUE)
endif()

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
            ${CMAKE_SOURCE_DIR}/targets/${RTOS}/_Network

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
# BUILD_TARGET parameter to set the target it's building
# optional EXTRA_SOURCES with source files to be added to the library
# optional EXTRA_INCLUDES with include paths to be added to the library
# optional EXTRA_COMPILE_DEFINITIONS with compiler definitions to be added to the library
# optional EXTRA_COMPILE_OPTIONS with compile options to be added to the library
macro(nf_add_lib_network)

    # parse arguments
    cmake_parse_arguments(NFALN "" "BUILD_TARGET;EXTRA_COMPILE_OPTIONS" "EXTRA_SOURCES;EXTRA_INCLUDES;EXTRA_COMPILE_DEFINITIONS" ${ARGN})

    if(NOT NFALN_BUILD_TARGET OR "${NFALN_BUILD_TARGET}" STREQUAL "")
        message(FATAL_ERROR "Need to set BUILD_TARGET argument when calling nf_add_lib_network()")
    endif()

    # add this has a library
    set(LIB_NAME NF_Network)

    add_library(
        ${LIB_NAME} STATIC 
            ${NF_Network_SOURCES}
            ${NFALN_EXTRA_SOURCES})

    target_include_directories(
        ${LIB_NAME} 
        PUBLIC 
            ${NF_Network_INCLUDE_DIRS}
            ${NF_CoreCLR_INCLUDE_DIRS}
            ${NFALN_EXTRA_INCLUDES})

    # TODO can be removed later
    if(RTOS_ESP32_CHECK)
        # nf_common_compiler_definitions(TARGET ${LIB_NAME} BUILD_TARGET ${NFALN_TARGET})

        # this is the only one different
        target_compile_definitions(
            ${LIB_NAME} PUBLIC
            -DPLATFORM_ESP32
            ${NFALN_EXTRA_COMPILER_DEFINITIONS}
        )

    else()
        nf_set_compile_options(TARGET ${LIB_NAME} EXTRA_COMPILE_OPTIONS ${NFALN_EXTRA_COMPILE_OPTIONS} BUILD_TARGET ${NFALN_BUILD_TARGET})
        nf_set_compile_definitions(TARGET ${LIB_NAME} EXTRA_COMPILE_DEFINITIONS ${NFALN_EXTRA_COMPILE_DEFINITIONS} BUILD_TARGET ${NFALN_BUILD_TARGET})
        nf_set_link_options(TARGET ${LIB_NAME})
    endif()

    # add alias
    add_library("nano::${LIB_NAME}" ALIAS ${LIB_NAME})
    
endmacro()
