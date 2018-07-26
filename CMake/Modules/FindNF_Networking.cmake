#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# set include directories for nanoFramework network
list(APPEND NF_Networking_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/PAL/Com/sockets)
list(APPEND NF_Networking_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/PAL/Com/sockets/ssl)
list(APPEND NF_Networking_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/PAL/Lwip)

if(USE_SECURITY_MBEDTLS_OPTION)
    #list(APPEND NF_Networking_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/PAL/Com/sockets/ssl/mbedTLS)
elseif(USE_SECURITY_OPENSSL_OPTION)
    list(APPEND NF_Networking_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/PAL/Com/sockets/ssl/openssl)
endif()

# source files for nanoFramework Networking
set(NF_Networking_SRCS

    #pal Socket
    sockets_lwip.cpp

    #Lwip 
    LwIP_Sockets.cpp
    LwIP_Sockets_functions.cpp 
)

# Select security module
if(USE_SECURITY_MBEDTLS_OPTION)
    list(APPEND NF_Networking_SRCS 
        ssl.cpp
        # FIXME - SSL mbedTLS specific interface (WIP)
        ssl_generic_init_internal.cpp
        ssl_initialize_internal.cpp
        ssl_parse_certificate_internal.cpp
        mbedtls.cpp
    )
elseif(USE_SECURITY_OPENSSL_OPTION)
    list(APPEND NF_Networking_SRCS
        ssl.cpp
        # SSL openssl (wrapper)
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
else()
    list(APPEND NF_Networking_SRCS
        # No security interface defined
        ssl_stubs.cpp
    )
endif()

if(NF_FEATURE_DEBUGGER)
    list(APPEND NF_Networking_SRCS sockets_debugger.cpp)
endif()

foreach(SRC_FILE ${NF_Networking_SRCS})
    set(NF_Networking_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(NF_Networking_SRC_FILE ${SRC_FILE}
        PATHS 
            ${PROJECT_SOURCE_DIR}/src/PAL/COM/sockets
            ${PROJECT_SOURCE_DIR}/src/PAL/COM/sockets/ssl

            if(USE_SECURITY_MBEDTLS_OPTION)
                ${PROJECT_SOURCE_DIR}/src/PAL/COM/sockets/ssl/mbedTLS
            elseif(USE_SECURITY_OPENSSL_OPTION)
                ${PROJECT_SOURCE_DIR}/src/PAL/COM/sockets/ssl/openssl
            endif()

            ${PROJECT_SOURCE_DIR}/src/PAL/Lwip
            ${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}
 
        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${NF_Networking_SRC_FILE}") # debug helper
    list(APPEND NF_Networking_SOURCES ${NF_Networking_SRC_FILE})
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(NF_Networking DEFAULT_MSG NF_Networking_INCLUDE_DIRS NF_Networking_SOURCES)
