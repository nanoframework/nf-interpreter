#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# includes for SimpleLink CC13x2_26x2 SDK
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source")
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers")
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/posix/gcc")
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/boards/CC3220SF_LAUNCHXL")
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/devices/CC13x2_26x2")
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/devices/CC13x2_26x2/driverlib")
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/net")
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/bsd")

# includes for nanoFramework SSL driver
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/PAL/Com/sockets/ssl")


# source files for board drivers 
set(BoardDrivers_SRCS

    # board drivers
    NVS.c
    NVSRAM.c
    NVSSPI25X.c
    SD.c
    SDSPI.c
    SDFatFS.c
    List.c
    RingBuf.c
    Random.c
    ADC.c
    ADCCC26XX.c
    ADCBuf.c
    ADCBufCC26XX.c
    CryptoCC26XX.c
    AESCBC.c
    AESCBCCC26XX.c
    AESCTR.c
    AESCTRCC26XX.c
    AESCTRDRBG.c
    AESCTRDRBGXX.c
    AESCCM.c
    AESCCMCC26XX.c
    AESECB.c
    AESECBCC26XX.c
    CryptoKeyPlaintextCC26XX.c
    CryptoResourceCC26XX.c
    UDMACC26XX.c
    GPIOCC26XX.c
    I2C.c
    I2CCC26XX.c
    I2S.c
    I2SCC26XX.c
    NVSCC26XX.c
    Codec1.c
    PDMCC26XX.c
    PDMCC26XX_util.c
    PINCC26XX.c
    PowerCC26XX.c
    PowerCC26XX_calibrateRCOSC.c
    PWM.c
    PWMTimerCC26XX.c
    SPI.c
    SPICC26XXDMA.c
    TRNG.c
    TRNGCC26XX.c
    GPTimerCC26XX.c
    UART.c
    UARTCC26XX.c
    UARTCC26X0.c
    Watchdog.c
    WatchdogCC26XX.c
    ADCBufCC26X2.c
    AESGCM.c
    AESGCMCC26XX.c
    ECCParamsCC26X2.c
    PKAResourceCC26XX.c
    ECDH.c
    ECDHCC26X2.c
    ECDSA.c
    ECDSACC26X2.c
    ECJPAKE.c
    ECJPAKECC26X2.c
    PowerCC26X2.c
    PowerCC26X2_calibrateRCOSC.c
    SHA2.c
    SHA2CC26X2.c
    SPICC26X2DMA.c
    UARTCC26X2.c
)

foreach(SRC_FILE ${BoardDrivers_SRCS})
    set(BoardDrivers_SCR_FILE SRC_FILE -NOTFOUND)
    find_file(BoardDrivers_SCR_FILE ${SRC_FILE}
        PATHS

        "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/nvs"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/sd"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/adc"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/adcbuf"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/aescbc"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/aesccm"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/aesctr"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/aesctrdrbg"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/aesecb"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/aesgcm"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/crypto"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/cryptoutils"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/cryptoutils/cryptokey"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/cryptoutils/ecc"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/cryptoutils/sharedresources"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/ecjpake"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/dpl"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/dma"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/ecdh"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/ecdsa"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/esjpake"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/gpio"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/i2c"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/i2s"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/pdm"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/pin"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/power"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/pwm"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/rf"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/sd"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/sha2"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/spi"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/timer"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/trng"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/uart"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/watchdog"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/utils"

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${BoardDrivers_SCR_FILE}") # debug helper
    list(APPEND TI_SimpleLink_SOURCES ${BoardDrivers_SCR_FILE})
endforeach()

# need to specify a language for this assembler source file because its not using a standard extension
set_source_files_properties("${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/power/PowerCC13x2_26x2_asm.sm4g" PROPERTIES LANGUAGE ASM)

# source files for drivers library 
set(DriversLibrary_SRCS

    #drivers library
    adi.c
    aes.c
    aon_batmon.c
    aon_event.c
    aon_ioc.c
    aon_pmctl.c
    aon_rtc.c
    aux_adc.c
    aux_smph.c
    aux_sysif.c
    aux_tdc.c
    ccfgread.c
    chipinfo.c
    cpu.c
    crypto.c
    ddi.c
    debug.c
    driverlib_release.c
    event.c
    flash.c
    gpio.c
    i2c.c
    i2s.c
    interrupt.c
    ioc.c
    osc.c
    pka.c
    prcm.c
    pwr_ctrl.c
    rfc.c
    rom_crypto.c
    setup.c
    setup_rom.c
    sha2.c
    smph.c
    ssi.c
    sw_chacha.c
    sw_poly1305-donna.c
    systick.c
    sys_ctrl.c
    timer.c
    trng.c
    uart.c
    udma.c
    vims.c
    watchdog.c

    # display specific
    DisplayUart.c
)

foreach(SRC_FILE ${DriversLibrary_SRCS})
    set(DriversLibrary_SCR_FILE SRC_FILE -NOTFOUND)
    find_file(DriversLibrary_SCR_FILE ${SRC_FILE}
        PATHS

        #drivers library
        "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2Sdk_Source/ti/devices/cc13x2_cc26x2/driverlib"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/display"

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${DriversLibrary_SCR_FILE}") # debug helper
    list(APPEND TI_SimpleLink_SOURCES ${DriversLibrary_SCR_FILE})
endforeach()

# # source files for SimpleLink 
# set(SimpleLink_SRCS

#     device.c
#     driver.c
#     flowcont.c
#     fs.c
#     netapp.c
#     netcfg.c
#     netutil.c
#     nonos.c
#     sl_socket.c
#     spawn.c
#     wlan.c
#     cc_pal.c
#     eventreg.c
#     wlanconfig.c

#     # eventreg.c

# )

# foreach(SRC_FILE ${SimpleLink_SRCS})
#     set(SimpleLink_SCR_FILE SRC_FILE -NOTFOUND)
#     find_file(SimpleLink_SCR_FILE ${SRC_FILE}
#         PATHS

#         #drivers library
#         "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/net/wifi"
#         "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/net/wifi/porting"
#         "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/net/wifi/source"

#         CMAKE_FIND_ROOT_PATH_BOTH
#     )
#     # message("${SRC_FILE} >> ${SimpleLink_SCR_FILE}") # debug helper
#     list(APPEND TI_SimpleLink_SOURCES ${SimpleLink_SCR_FILE})
# endforeach()


# # source files for SimpleLink net Wi-Fi 
# set(SLNetWiFi_SRCS

#     slnetsock.c
#     slnetutils.c
#     slnetif.c
    
#     # bsd
#     netdb.c
#     socket.c
#     errnoutil.c

#     # drivers
#     eventreg.c
#     slnetifwifi.c

#     # utils
#     clock_sync.c
# )

# foreach(SRC_FILE ${SLNetWiFi_SRCS})
#     set(SLNetWiFi_SCR_FILE SRC_FILE -NOTFOUND)
#     find_file(SLNetWiFi_SCR_FILE ${SRC_FILE}
#         PATHS

#         "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/net"
#         "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/net/bsd"

#         # drivers
#         "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/net/wifi"
#         "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/drivers/net/wifi/slnetif"

#         # utils
#         "${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/ti/net/utils"

#         CMAKE_FIND_ROOT_PATH_BOTH
#     )
#     # message("${SRC_FILE} >> ${SLNetWiFi_SCR_FILE}") # debug helper
#     list(APPEND TI_SimpleLink_SOURCES ${SLNetWiFi_SCR_FILE})
# endforeach()


# # source files for nanoFramework SSL driver
# set(nanoSsl_SRCS

#     ssl.cpp
# )

# foreach(SRC_FILE ${nanoSsl_SRCS})
#     set(nanoSsl_SCR_FILE SRC_FILE -NOTFOUND)
#     find_file(nanoSsl_SCR_FILE ${SRC_FILE}
#         PATHS

#         "${PROJECT_SOURCE_DIR}/src/PAL/COM/sockets/ssl"

#         CMAKE_FIND_ROOT_PATH_BOTH
#     )
#     # message("${SRC_FILE} >> ${nanoSsl_SCR_FILE}") # debug helper
#     list(APPEND TI_SimpleLink_SOURCES ${nanoSsl_SCR_FILE})
# endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(SimpleLinkCC13x2_26x2SDK DEFAULT_MSG TI_SimpleLink_INCLUDE_DIRS TI_SimpleLink_SOURCES)
