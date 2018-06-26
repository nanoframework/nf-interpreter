#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# set include directories for nanoFramework network
list(APPEND NF_Networking_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/PAL/Com/sockets)
list(APPEND NF_Networking_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/PAL/Com/sockets/ssl)
list(APPEND NF_Networking_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/PAL/Lwip)

#list(APPEND NF_Networking_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/PAL/Com/sockets/ssl/MbedTls)
list(APPEND NF_Networking_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/PAL/Com/sockets/ssl/openssl)

# source files for nanoFramework Networking
set(NF_Networking_SRCS

    #pal Socket
    sockets_lwip.cpp

    #Lwip 
    LwIP_Sockets.cpp
    LwIP_Sockets_functions.cpp 

    #security SSL
    ssl.cpp
    #ssl_stubs.cpp

    # SSL MbedTLS specific 
    #MbedTls_parse_certificate.cpp
    #MbedTls.cpp
   
    # SSL openssl
    ssl_initialize_internal.cpp
    ssl_generic_init_internal.cpp
    ssl_uninitialize_internal.cpp
    ssl_parse_certificate_internal.cpp
    ssl_accept_internal.cpp
    ssl_connect_internal.cpp
    ssl_read_internal.cpp
    ssl_write_internal.cpp
    ssl_pending_internal.cpp
    ssl_closesocket_internal.cpp
    ssl_exit_context_internal.cpp
    ssl_add_cert_auth_internal.cpp
    ssl_clear_cert_auth_internal.cpp

)

if(NF_FEATURE_DEBUGGER)
    list(APPEND NF_Networking_SRCS sockets_debugger.cpp)
endif()

foreach(SRC_FILE ${NF_Networking_SRCS})
    set(NF_Networking_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(NF_Networking_SRC_FILE ${SRC_FILE}
        PATHS 
            ${PROJECT_SOURCE_DIR}/src/PAL/COM/sockets
            ${PROJECT_SOURCE_DIR}/src/PAL/COM/sockets/ssl
#            ${PROJECT_SOURCE_DIR}/src/PAL/COM/sockets/ssl/MbedTls
            ${PROJECT_SOURCE_DIR}/src/PAL/COM/sockets/ssl/openssl
            ${PROJECT_SOURCE_DIR}/src/PAL/Lwip
            ${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}
 
        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${NF_Networking_SRC_FILE}") # debug helper
    list(APPEND NF_Networking_SOURCES ${NF_Networking_SRC_FILE})
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(NF_Networking DEFAULT_MSG NF_Networking_INCLUDE_DIRS NF_Networking_SOURCES)
