#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# set include directories for nanoFramework network
list(APPEND NF_Networking_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets)
list(APPEND NF_Networking_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl)
list(APPEND NF_Networking_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/PAL/Lwip)
list(APPEND NF_Networking_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/Networking.Sntp)

if(USE_SECURITY_MBEDTLS_OPTION)
    list(APPEND NF_Networking_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/mbedTLS)
endif()

if(USE_ENC28J60_DRIVER_OPTION)
    list(APPEND NF_Networking_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/Network/Enc28j60)
    list(APPEND NF_Network_Driver_Path ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/Network/Enc28j60)
    set(Use_Networking_Extra_Driver TRUE)
endif()

# source files for nanoFramework Networking
set(NF_Networking_SRCS

    #pal Socket
    sockets_lwip.cpp

    #Lwip 
    LwIP_Sockets.cpp
    LwIP_Sockets_functions.cpp 

)

# need a conditional include because of ESP32 building network as a library 
if(NOT USE_SECURITY_MBEDTLS_OPTION)
    list(APPEND NF_Networking_SRCS ssl_stubs.cpp)
endif()

# source files for security layer
set(NF_Networking_Security_SRCS

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
    ssl_pending_internal.cpp
    ssl_read_internal.cpp
    ssl_uninitialize_internal.cpp
    ssl_write_internal.cpp

)

if(NF_FEATURE_DEBUGGER)
    list(APPEND NF_Networking_SRCS sockets_debugger.cpp)
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
foreach(SRC_FILE ${NF_Networking_SRCS})
    set(NF_Networking_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(NF_Networking_SRC_FILE ${SRC_FILE}
        PATHS

            ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets
            ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl

            ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/mbedTLS

            ${CMAKE_SOURCE_DIR}/src/PAL/Lwip
            ${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}
 
        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${NF_Networking_SRC_FILE}") # debug helper
    list(APPEND NF_Networking_SOURCES ${NF_Networking_SRC_FILE})
endforeach()

if(USE_SECURITY_MBEDTLS_OPTION)

    set(NF_Security_Search_Path "${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/mbedTLS")

    # 2nd pass: security files if option is selected 
    foreach(SRC_FILE ${NF_Networking_Security_SRCS})
        set(NF_Networking_SRC_FILE SRC_FILE-NOTFOUND)
        find_file(NF_Networking_SRC_FILE ${SRC_FILE}
            PATHS 

                ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl
                ${NF_Security_Search_Path}
    
            CMAKE_FIND_ROOT_PATH_BOTH
        )
        # message("${SRC_FILE} >> ${NF_Networking_SRC_FILE}") # debug helper
        list(APPEND NF_Networking_SOURCES ${NF_Networking_SRC_FILE})
    endforeach()

endif()

if(Use_Networking_Extra_Driver)

    # 3rd pass: Any extra driver files 
    foreach(SRC_FILE ${NF_Network_Driver_Srcs})
        set(NF_Networking_SRC_FILE SRC_FILE-NOTFOUND)
        find_file(NF_Networking_SRC_FILE ${SRC_FILE}
            PATHS 
                ${NF_Network_Driver_Path}
    
            CMAKE_FIND_ROOT_PATH_BOTH
        )
        # message("${SRC_FILE} >> ${NF_Network_Driver_Srcs}") # debug helper
        list(APPEND NF_Networking_SOURCES ${NF_Networking_SRC_FILE})
    endforeach()

endif()


include(FindPackageHandleStandardArgs)


FIND_PACKAGE_HANDLE_STANDARD_ARGS(NF_Networking DEFAULT_MSG NF_Networking_INCLUDE_DIRS NF_Networking_SOURCES)
