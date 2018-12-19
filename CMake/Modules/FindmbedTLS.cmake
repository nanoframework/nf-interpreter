#
# Copyright (c) 2018 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# because of issues when passing the config file as a string when using ExternalProject_Add with mbedTLS
# we are replicating their CMakeList where. Actually this is more a simplified version...

# List of the required include paths
list(APPEND mbedTLS_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/mbedTLS_Source/include/)

option(USE_PKCS11_HELPER_LIBRARY "Build mbed TLS with the pkcs11-helper library." OFF)
option(ENABLE_ZLIB_SUPPORT "Build mbed TLS with zlib library." OFF)

if(ENABLE_ZLIB_SUPPORT)
    find_package(ZLIB)

    if(ZLIB_FOUND)
        include_directories(${ZLIB_INCLUDE_DIR})
    endif(ZLIB_FOUND)
endif(ENABLE_ZLIB_SUPPORT)

# add_subdirectory(library)
# add_subdirectory(include)

set(src_crypto
    aes.c
    aesni.c
    arc4.c
    aria.c
    asn1parse.c
    asn1write.c
    base64.c
    bignum.c
    blowfish.c
    camellia.c
    ccm.c
    cipher.c
    cipher_wrap.c
    cmac.c
    ctr_drbg.c
    des.c
    dhm.c
    ecdh.c
    ecdsa.c
    ecjpake.c
    ecp.c
    ecp_curves.c
    entropy.c
    entropy_poll.c
    error.c
    gcm.c
    havege.c
    hkdf.c
    hmac_drbg.c
    md.c
    md2.c
    md4.c
    md5.c
    md_wrap.c
    memory_buffer_alloc.c
    oid.c
    padlock.c
    pem.c
    pk.c
    pk_wrap.c
    pkcs12.c
    pkcs5.c
    pkparse.c
    pkwrite.c
    platform.c
    platform_util.c
    ripemd160.c
    rsa.c
    rsa_internal.c
    sha1.c
    sha256.c
    sha512.c
    threading.c
    timing.c
    version.c
    version_features.c
    xtea.c
)

set(src_x509
    certs.c
    pkcs11.c
    x509.c
    x509_create.c
    x509_crl.c
    x509_crt.c
    x509_csr.c
    x509write_crt.c
    x509write_csr.c
)

set(src_tls
    debug.c
    net_sockets.c
    ssl_cache.c
    ssl_ciphersuites.c
    ssl_cli.c
    ssl_cookie.c
    ssl_srv.c
    ssl_ticket.c
    ssl_tls.c
)

foreach(SRC_FILE ${src_crypto})
    set(MBEDTLS_SRC_FILE SRC_FILE -NOTFOUND)
    find_file(MBEDTLS_SRC_FILE ${SRC_FILE}
        PATHS 
            ${PROJECT_BINARY_DIR}/mbedTLS_Source/library

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${MBEDTLS_SRC_FILE}") # debug helper
    list(APPEND mbedTLS_SOURCES ${MBEDTLS_SRC_FILE})
endforeach()

# unset this warning as error required for this source file
SET_SOURCE_FILES_PROPERTIES( ${PROJECT_BINARY_DIR}/mbedTLS_Source/library/hmac_drbg.c PROPERTIES COMPILE_FLAGS -Wno-maybe-uninitialized)

foreach(SRC_FILE ${src_x509})
    set(MBEDTLS_SRC_FILE SRC_FILE -NOTFOUND)
    find_file(MBEDTLS_SRC_FILE ${SRC_FILE}
        PATHS 
            ${PROJECT_BINARY_DIR}/mbedTLS_Source/library

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${MBEDTLS_SRC_FILE}") # debug helper
    list(APPEND mbedTLS_SOURCES ${MBEDTLS_SRC_FILE})
endforeach()

foreach(SRC_FILE ${src_tls})
    set(MBEDTLS_SRC_FILE SRC_FILE -NOTFOUND)
    find_file(MBEDTLS_SRC_FILE ${SRC_FILE}
        PATHS 
            ${PROJECT_BINARY_DIR}/mbedTLS_Source/library

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${MBEDTLS_SRC_FILE}") # debug helper
    list(APPEND mbedTLS_SOURCES ${MBEDTLS_SRC_FILE})
endforeach()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(mbedTLS DEFAULT_MSG mbedTLS_INCLUDE_DIRS mbedTLS_SOURCES)
