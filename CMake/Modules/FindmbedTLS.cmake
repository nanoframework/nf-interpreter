#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
FetchContent_GetProperties(mbedtls)
FetchContent_GetProperties(esp32_idf)

# because of issues when passing the config file as a string when using ExternalProject_Add with mbedTLS
# we are replicating their CMakeList here. Actually this is more a simplified version...

# List of the required include paths
# the list of the required include paths needs to be platform specific because of ESP32 port
if(RTOS_ESP32_CHECK)
    list(APPEND mbedTLS_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/mbedtls/port/include)
    list(APPEND mbedTLS_INCLUDE_DIRS ${esp32_idf_SOURCE_DIR}/components/mbedtls/port/include/mbedtls)
endif()

list(APPEND mbedTLS_INCLUDE_DIRS ${mbedtls_SOURCE_DIR})
list(APPEND mbedTLS_INCLUDE_DIRS ${mbedtls_SOURCE_DIR}/include)


option(USE_PKCS11_HELPER_LIBRARY "Build mbed TLS with the pkcs11-helper library." OFF)
option(ENABLE_ZLIB_SUPPORT "Build mbed TLS with zlib library." OFF)

if(ENABLE_ZLIB_SUPPORT)
    find_package(ZLIB)

    if(ZLIB_FOUND)
        include_directories(${ZLIB_INCLUDE_DIR})
    endif(ZLIB_FOUND)
endif(ENABLE_ZLIB_SUPPORT)

set(src_crypto
    aes.c
    aesni.c
    arc4.c
    aria.c
    asn1parse.c
    asn1write.c
    base64.c
    blowfish.c
    camellia.c
    ccm.c
    cipher.c
    cipher_wrap.c
    cmac.c
    constant_time.c
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

if(NOT RTOS_ESP32_CHECK)
    # platform implementation of hardware random provider
    list(APPEND src_crypto mbedtls_entropy_hardware_pool.c)
    list(APPEND src_crypto bignum.c)
endif()

foreach(SRC_FILE ${src_crypto})

    set(MBEDTLS_SRC_FILE SRC_FILE -NOTFOUND)

    find_file(MBEDTLS_SRC_FILE ${SRC_FILE}
        PATHS
            ${mbedtls_SOURCE_DIR}/library

            ${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${MBEDTLS_SRC_FILE}")
    endif()

    list(APPEND mbedTLS_SOURCES ${MBEDTLS_SRC_FILE})

endforeach()

if(RTOS_ESP32_CHECK)
    list(APPEND mbedTLS_SOURCES ${esp32_idf_SOURCE_DIR}/components/mbedtls/mbedtls/library/bignum.c)
endif()

# unset this warning as error required for this source file
SET_SOURCE_FILES_PROPERTIES( ${mbedtls_SOURCE_DIR}/library/hmac_drbg.c PROPERTIES COMPILE_FLAGS -Wno-maybe-uninitialized)
SET_SOURCE_FILES_PROPERTIES( ${mbedtls_SOURCE_DIR}/library/x509_crt.c PROPERTIES COMPILE_FLAGS -Wno-maybe-uninitialized)

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

foreach(SRC_FILE ${src_x509})

    set(MBEDTLS_SRC_FILE SRC_FILE -NOTFOUND)

    find_file(MBEDTLS_SRC_FILE ${SRC_FILE}
        PATHS
            ${mbedtls_SOURCE_DIR}/library

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${MBEDTLS_SRC_FILE}")
    endif()

    list(APPEND mbedTLS_SOURCES ${MBEDTLS_SRC_FILE})

endforeach()

set(src_tls
    debug.c
    ssl_cache.c
    ssl_ciphersuites.c
    ssl_cli.c
    ssl_cookie.c
    ssl_srv.c
    ssl_ticket.c
    ssl_tls.c

    ssl_msg.c
)

foreach(SRC_FILE ${src_tls})

    set(MBEDTLS_SRC_FILE SRC_FILE -NOTFOUND)

    find_file(MBEDTLS_SRC_FILE ${SRC_FILE}
        PATHS
            ${mbedtls_SOURCE_DIR}/library

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${MBEDTLS_SRC_FILE}")
    endif()

    list(APPEND mbedTLS_SOURCES ${MBEDTLS_SRC_FILE})

endforeach()

# some sources need to be added from mbedTLS repo or ESP32 depending on build
# check port files specific to ESP32 IDF here 'components/mbedtls/CMakeLists.txt'
if(RTOS_ESP32_CHECK)

    set(src_platform_specific

        mbedtls_debug.c
        esp_hardware.c
        esp_mem.c
        esp_timing.c
        esp_sha.c
        esp_aes_xts.c
        esp_aes_common.c
        esp_aes.c
        sha.c
        esp_sha1.c
        esp_sha256.c
        esp_sha512.c
        esp_md.c
    )
        
    foreach(SRC_FILE ${src_platform_specific})

    set(MBEDTLS_SRC_FILE SRC_FILE -NOTFOUND)

    find_file(MBEDTLS_SRC_FILE ${SRC_FILE}
        PATHS

            ${esp32_idf_SOURCE_DIR}/components/mbedtls/port
            ${esp32_idf_SOURCE_DIR}/components/mbedtls/port/sha/parallel_engine
            ${esp32_idf_SOURCE_DIR}/components/mbedtls/port/sha/
            ${esp32_idf_SOURCE_DIR}/components/mbedtls/port/aes
            ${esp32_idf_SOURCE_DIR}/components/mbedtls/port/aes/block
            
            ${esp32_idf_SOURCE_DIR}/components/mbedtls/port/md

            ${CMAKE_SOURCE_DIR}/targets/ESP32/_IDF

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${MBEDTLS_SRC_FILE}")
    endif()

    list(APPEND mbedTLS_SOURCES ${MBEDTLS_SRC_FILE})

    endforeach()

    if(CONFIG_MBEDTLS_HARDWARE_MPI)
        list(APPEND mbedTLS_SOURCES ${esp32_idf_SOURCE_DIR}/components/mbedtls/port/esp_bignum.c)
        list(APPEND src_platform_specific ${esp32_idf_SOURCE_DIR}/components/mbedtls/port/${TARGET_SERIES_SHORT}/bignum.c)
    endif()

else()

    # other platforms use the official sources

    set(src_platform_specific

        net_sockets.c
    )

    foreach(SRC_FILE ${src_platform_specific})

    set(MBEDTLS_SRC_FILE SRC_FILE -NOTFOUND)

    find_file(MBEDTLS_SRC_FILE ${SRC_FILE}
        PATHS

            ${mbedtls_SOURCE_DIR}/library

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${MBEDTLS_SRC_FILE}")
    endif()

    list(APPEND mbedTLS_SOURCES ${MBEDTLS_SRC_FILE})

    endforeach()

endif()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(mbedTLS DEFAULT_MSG mbedTLS_INCLUDE_DIRS mbedTLS_SOURCES)
