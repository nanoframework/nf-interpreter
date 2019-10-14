#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

include(JSONParser)

# read mBed targets file
file(READ "${PROJECT_BINARY_DIR}/mBedOS_Source/targets/targets.json" TARGETS_JSON)

# try to find target
string(FIND ${TARGETS_JSON} ${MBED_TARGET} FOUND_TARGET)

# message(STATUS "${MBED_TARGET} @ ${FOUND_TARGET}")

if(${FOUND_TARGET} GREATER -1)
    # target found
    
    message(STATUS "mBed target '${MBED_TARGET}' found")
    # get substring with data
    math(EXPR FOUND_TARGET "${FOUND_TARGET} - 1")
    
    # because there is not an elegant way of finding the end of the json section of this target we are setting this to 1000 chars
    # the downside is that for most target this overuns to the next target
    string(SUBSTRING "${TARGETS_JSON}" ${FOUND_TARGET} 2000 TARGET_TENTATIVE_JSON)
else()
    message(FATAL_ERROR "\n\nSorry but ${MBED_TARGET} seems to be missing in the list of the mBed supported targets...\n\n")
endif()

# parse target data
sbeParseJson(MBED_TARGET_DATA TARGET_TENTATIVE_JSON)

# try to extract relevant data 
foreach(var ${MBED_TARGET_DATA})

    # sanity check when json parse has overun to the next target
    string(FIND ${var} "MBED_TARGET_DATA.${MBED_TARGET}" MBED_TARGET_NAME_FOUND)
    if(MBED_TARGET_NAME_FOUND GREATER -1)
        
        # seems to belong to target, extract data
        #message("${var} : ${${var}}") # debug helper

        # core 
        string(FIND ${var} "core" MBED_TARGET_CORE_INDEX)
        if(${MBED_TARGET_CORE_INDEX} GREATER -1)
            set(MBED_TARGET_CORE ${${var}})
        endif()

        # extra labels 0 is: target vendor
        string(FIND ${var} "MBED_TARGET_DATA.${MBED_TARGET}.extra_labels_0" MBED_TARGET_VENDOR_INDEX)
        if(${MBED_TARGET_VENDOR_INDEX} GREATER -1)
            set(MBED_TARGET_VENDOR ${${var}})
        endif()

        # extra labels 1 is: target series/variant
        string(FIND ${var} "MBED_TARGET_DATA.${MBED_TARGET}.extra_labels_1" MBED_TARGET_SERIES_INDEX)
        if(${MBED_TARGET_SERIES_INDEX} GREATER -1)
            set(MBED_TARGET_SERIES ${${var}})
        endif()
        
        # extra labels 2 is used for compiler defs
        string(FIND ${var} "MBED_TARGET_DATA.${MBED_TARGET}.extra_labels_2" MBED_TARGET_EXTRA_LABEL2_INDEX)
        if(${MBED_TARGET_EXTRA_LABEL2_INDEX} GREATER -1)
            set(MBED_TARGET_EXTRA_LABEL2 ${${var}})
        endif()

        # 'macros' lists target aditional macros
        string(FIND ${var} "MBED_TARGET_DATA.${MBED_TARGET}.macros_" MBED_TARGET_MACROS_INDEX)
        if(${MBED_TARGET_MACROS_INDEX} GREATER -1)
            list(APPEND MBED_TARGET_MACROS ${${var}})
        endif()

        # 'device has' lists target features
        string(FIND ${var} "MBED_TARGET_DATA.${MBED_TARGET}.device_has_" MBED_TARGET_DEVICE_HAS_INDEX)
        if(${MBED_TARGET_DEVICE_HAS_INDEX} GREATER -1)
            list(APPEND MBED_TARGET_FEATURES ${${var}})
        endif()

        # 'supported form factors' lists supported form factors for board
        string(FIND ${var} "MBED_TARGET_DATA.${MBED_TARGET}.supported_form_factors_" MBED_TARGET_DEVICE_FORM_FACTORS_INDEX)
        if(${MBED_TARGET_DEVICE_FORM_FACTORS_INDEX} GREATER -1)
            list(APPEND MBED_TARGET_DEVICE_FORM_FACTORS ${${var}})
        endif()

    endif()
    
endforeach()

message("mBed target has ${MBED_TARGET_CORE}")
message("mBed target vendor is ${MBED_TARGET_VENDOR}")
message("mBed target series is ${MBED_TARGET_SERIES}")
#message("mBed target features: ${MBED_TARGET_FEATURES}")

# get target core short name
string(REPLACE "Cortex-" "" MBED_TARGET_CORE_SHORT ${MBED_TARGET_CORE} )

# set compiler options (general)
string(APPEND MBEDOS_COMPILE_OPTIONS "-D__CORTEX_${MBED_TARGET_CORE_SHORT} -D__MBED__=1 -DTARGET_LIKE_MBED -D__MBED_CMSIS_RTOS_CM -DTOOLCHAIN_object -D__CMSIS_RTOS -DTOOLCHAIN_GCC -DTARGET_CORTEX_M -DARM_MATH_C${MBED_TARGET_CORE_SHORT} -DTARGET_${MBED_TARGET_CORE_SHORT} -DTOOLCHAIN_GCC_ARM -DTARGET_RELEASE -DTARGET_LIKE_CORTEX_${MBED_TARGET_CORE_SHORT} -D__CORTEX_${MBED_TARGET_CORE_SHORT} -DTARGET_${MBED_TARGET_VENDOR} -DTARGET_${MBED_TARGET_SERIES} -DTARGET_UVISOR_UNSUPPORTED -DTARGET_${MBED_TARGET}")

# extra compiler def from extra label 2
if(MBED_TARGET_EXTRA_LABEL2)
    string(APPEND MBEDOS_COMPILE_OPTIONS " -DTARGET_${MBED_TARGET_EXTRA_LABEL2}")
endif()

# set compiler options (target specific features)
foreach(feature ${MBED_TARGET_FEATURES})
    string(APPEND MBEDOS_COMPILE_OPTIONS " -DDEVICE_${feature}=1")
endforeach()

# set compiler options (form factors)
foreach(ff ${MBED_TARGET_DEVICE_FORM_FACTORS})
    string(APPEND MBEDOS_COMPILE_OPTIONS " -DTARGET_FF_${ff}")
endforeach()

# set compiler options (macros)
foreach(macro ${MBED_TARGET_MACROS})
    string(APPEND MBEDOS_COMPILE_OPTIONS " -D${macro}")
endforeach()

#message(">>MBEDOS_COMPILE_OPTIONS: ${MBEDOS_COMPILE_OPTIONS}<<") # debug helper

# set include directories for mBed RTOS
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source)
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/cmsis)
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/cmsis/TOOLCHAIN_GCC)
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/docs)
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/drivers)
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/events)
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/events/equeue)
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/features)
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/features/frameworks)
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/features/frameworks/greentea-client)
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/features/frameworks/greentea-client/greentea-client)
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/features/frameworks/greentea-client/source)
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/features/frameworks/unity)
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/features/frameworks/unity/source)
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/features/frameworks/unity/unity)
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/features/frameworks/utest)
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/features/frameworks/utest/source)
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/features/frameworks/utest/utest)
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/features/mbedtls)
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/features/mbedtls/importer)
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/features/mbedtls/inc)
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/features/mbedtls/inc/mbedtls)
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/features/mbedtls/platform)
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/features/mbedtls/platform/inc)
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/features/mbedtls/platform/src)
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/features/mbedtls/src)
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/features/nanostack)
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/features/netsocket)
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/features/storage)
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/hal)
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/hal/storage_abstraction)
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/platform)
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/rtos)
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/rtos/rtx)
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/rtos/rtx/TARGET_CORTEX_M)
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/rtos/rtx/TARGET_CORTEX_M/TARGET_${MBED_TARGET_CORE_SHORT})
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/rtos/rtx/TARGET_CORTEX_M/TARGET_${MBED_TARGET_CORE_SHORT}/TOOLCHAIN_GCC)
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/targets)
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/targets/TARGET_${MBED_TARGET_VENDOR})
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/targets/TARGET_${MBED_TARGET_VENDOR}/TARGET_${MBED_TARGET_SERIES})
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/targets/TARGET_${MBED_TARGET_VENDOR}/TARGET_${MBED_TARGET_SERIES}/device)
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/targets/TARGET_${MBED_TARGET_VENDOR}/TARGET_${MBED_TARGET_SERIES}/TARGET_${MBED_TARGET})
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/targets/TARGET_${MBED_TARGET_VENDOR}/TARGET_${MBED_TARGET_SERIES}/TARGET_${MBED_TARGET}/device)
list(APPEND MBEDOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/mBedOS_Source/targets/TARGET_${MBED_TARGET_VENDOR}/TARGET_${MBED_TARGET_SERIES}/TARGET_${MBED_TARGET}/device/TOOLCHAIN_GCC_ARM)

# source files and GCC options according to target vendor and series
if("${MBED_TARGET_VENDOR}" STREQUAL "STM")
    include(MBED_${MBED_TARGET_SERIES}_sources)
    include(MBED_${MBED_TARGET_SERIES}_GCC_options)
endif()

# source files for RTOS
set(MBEDRTOS_SRCS
    # drivers
    AnalogIn.cpp
    BusIn.cpp
    BusInOut.cpp
    BusOut.cpp
    CAN.cpp
    Ethernet.cpp
    FileBase.cpp
    FileLike.cpp
    FilePath.cpp
    FileSystemLike.cpp
    I2C.cpp
    I2CSlave.cpp
    InterruptIn.cpp
    InterruptManager.cpp
    LocalFileSystem.cpp
    RawSerial.cpp
    SPI.cpp
    SPISlave.cpp
    Serial.cpp
    SerialBase.cpp
    Stream.cpp
    Ticker.cpp
    Timeout.cpp
    Timer.cpp
    TimerEvent.cpp

    # events
    EventQueue.cpp
    equeue.c
    equeue_mbed.cpp
    equeue_posix.c

    # features
    greentea_metrics.cpp
    greentea_serial.cpp
    test_env.cpp
    unity.c
    mbed-utest-shim.cpp
    unity_handler.cpp
    utest_case.cpp
    utest_default_handlers.cpp
    utest_greentea_handlers.cpp
    utest_harness.cpp
    utest_shim.cpp
    utest_stack_trace.cpp
    utest_types.cpp
    
    # mbed tls
    mbed_trng.c
    aes.c
    aesni.c
    arc4.c
    asn1parse.c
    asn1write.c
    base64.c
    bignum.c
    blowfish.c
    camellia.c
    ccm.c
    certs.c
    cipher.c
    cipher_wrap.c
    cmac.c
    ctr_drbg.c
    debug.c
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
    hmac_drbg.c
    md.c
    md2.c
    md4.c
    md5.c
    md_wrap.c
    memory_buffer_alloc.c
    net_sockets.c
    oid.c
    padlock.c
    pem.c
    pk.c
    pk_wrap.c
    pkcs11.c
    pkcs12.c
    pkcs5.c
    pkparse.c
    pkwrite.c
    platform.c
    ripemd160.c
    rsa.c
    sha1.c
    sha256.c
    sha512.c
    ssl_cache.c
    ssl_ciphersuites.c
    ssl_cli.c
    ssl_cookie.c
    ssl_srv.c
    ssl_ticket.c
    ssl_tls.c
    threading.c
    timing.c
    version.c
    version_features.c
    x509.c
    x509_create.c
    x509_crl.c
    x509_crt.c
    x509_csr.c
    x509write_crt.c
    x509write_csr.c
    xtea.c

    # net socket
    NetworkInterface.cpp
    NetworkStack.cpp
    Socket.cpp
    SocketAddress.cpp
    TCPServer.cpp
    TCPSocket.cpp
    UDPSocket.cpp
    WiFiAccessPoint.cpp
    nsapi_dns.cpp

    # mBed HAL
    mbed_gpio.c
    mbed_lp_ticker_api.c
    mbed_pinmap_common.c
    mbed_ticker_api.c
    mbed_us_ticker_api.c

    # mBed platfom
    CallChain.cpp
    mbed_alloc_wrappers.cpp
    mbed_assert.c
    mbed_board.c
    mbed_critical.c
    mbed_error.c
    mbed_interface.c
    mbed_mem_trace.c
    mbed_rtc_time.cpp
    mbed_semihost_api.c
    mbed_stats.c
    mbed_wait_api_no_rtos.c
    mbed_wait_api_rtos.cpp
    retarget.cpp
    
    # RTOS
    Mutex.cpp
    RtosTimer.cpp
    Semaphore.cpp
    Thread.cpp
    rtos_idle.c
    HAL_CM.c
    RTX_Conf_CM.c
    rt_CMSIS.c
    rt_Event.c
    rt_List.c
    rt_Mailbox.c
    rt_MemBox.c
    rt_Memory.c
    rt_Mutex.c
    rt_OsEventObserver.c
    rt_Robin.c
    rt_Semaphore.c
    rt_System.c
    rt_Task.c
    rt_Time.c
    rt_Timer.c
)

foreach(SRC_FILE ${MBEDRTOS_SRCS})
    set(MBEDOS_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(MBEDOS_SRC_FILE ${SRC_FILE}
        PATHS 
            ${PROJECT_BINARY_DIR}/mBedOS_Source/drivers
            ${PROJECT_BINARY_DIR}/mBedOS_Source/events
            ${PROJECT_BINARY_DIR}/mBedOS_Source/events/equeue
            ${PROJECT_BINARY_DIR}/mBedOS_Source/features/frameworks/greentea-client/source
            ${PROJECT_BINARY_DIR}/mBedOS_Source/features/frameworks/unity/source
            ${PROJECT_BINARY_DIR}/mBedOS_Source/features/frameworks/utest
            ${PROJECT_BINARY_DIR}/mBedOS_Source/features/frameworks/utest/source
            
            ${PROJECT_BINARY_DIR}/mBedOS_Source/features/mbedtls/platform/src
            ${PROJECT_BINARY_DIR}/mBedOS_Source/features/mbedtls/src
            ${PROJECT_BINARY_DIR}/mBedOS_Source/features/netsocket

            ${PROJECT_BINARY_DIR}/mBedOS_Source/hal

            ${PROJECT_BINARY_DIR}/mBedOS_Source/platform

            ${PROJECT_BINARY_DIR}/mBedOS_Source/rtos 
            ${PROJECT_BINARY_DIR}/mBedOS_Source/rtos/rtx 
            ${PROJECT_BINARY_DIR}/mBedOS_Source/rtos/rtx/TARGET_CORTEX_M 

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${MBEDOS_SRC_FILE}") # debug helper
    list(APPEND MBEDOS_SOURCES ${MBEDOS_SRC_FILE})
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(MBEDOS DEFAULT_MSG MBEDOS_INCLUDE_DIRS MBEDOS_SOURCES)
