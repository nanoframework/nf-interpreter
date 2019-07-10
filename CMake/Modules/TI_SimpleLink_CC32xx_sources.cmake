#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# includes for SimpleLink CC32xx SDK
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source")
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/drivers")
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/posix/gcc")
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/boards/CC3220SF_LAUNCHXL")
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/devices/cc32xx")
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/devices/cc32xx/driverlib")
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/net")
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/bsd")

# includes for nanoFramework SSL driver
list(APPEND TI_SimpleLink_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/PAL/Com/sockets/ssl")


# source files for board drivers 
set(BoardDrivers_SRCS

    # board drivers
    # NVS.c
    # NVSRAM.c
    # NVSSPI25X.c
    # SD.c
    # SDSPI.c
    # SDFatFS.c
    List.c
    RingBuf.c
    ADC.c
    ADCCC32XX.c
    # Camera.c
    # CameraCC32XXDMA.c
    Capture.c
    CaptureCC32XX.c
    CryptoCC32XX.c
    UDMACC32XX.c
    GPIOCC32XX.c
    I2C.c
    I2CCC32XX.c
    I2S.c
    PowerCC32XX.c
    PowerCC32XX_asm.sm4g
    PWM.c
    PWMTimerCC32XX.c
    SDHostCC32XX.c
    SPI.c
    SPICC32XXDMA.c
    SPICC32XXDMA.c
    Timer.c
    TimerCC32XX.c
    UART.c
    UARTCC32XX.c
    UARTCC32XXDMA.c
    Watchdog.c
    WatchdogCC32XX.c
)

foreach(SRC_FILE ${BoardDrivers_SRCS})
    set(BoardDrivers_SCR_FILE SRC_FILE -NOTFOUND)
    find_file(BoardDrivers_SCR_FILE ${SRC_FILE}
        PATHS

        "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/drivers"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/drivers/nvs"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/drivers/sd"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/drivers/adc"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/drivers/camera"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/drivers/capture"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/drivers/crypto"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/drivers/dma"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/drivers/gpio"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/drivers/i2c"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/drivers/i2s"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/drivers/power"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/drivers/pwm"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/drivers/sd"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/drivers/spi"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/drivers/timer"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/drivers/uart"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/drivers/watchdog"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/drivers/utils"

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${BoardDrivers_SCR_FILE}") # debug helper
    list(APPEND TI_SimpleLink_SOURCES ${BoardDrivers_SCR_FILE})
endforeach()

# need to specify a language for this assembler source file because its not using a standard extension
set_source_files_properties("${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/drivers/power/PowerCC32XX_asm.sm4g" PROPERTIES LANGUAGE ASM)

# source files for drivers library 
set(DriversLibrary_SRCS

    #drivers library
    adc.c
    aes.c
    camera.c
    cpu.c
    crc.c
    des.c
    flash.c
    gpio.c
    hwspinlock.c
    i2c.c
    interrupt.c
    i2s.c
    pin.c
    prcm.c
    sdhost.c
    shamd5.c
    spi.c
    systick.c
    timer.c
    uart.c
    udma.c
    utils.c
    wdt.c

    # display specific
    DisplayUart.c
)

foreach(SRC_FILE ${DriversLibrary_SRCS})
    set(DriversLibrary_SCR_FILE SRC_FILE -NOTFOUND)
    find_file(DriversLibrary_SCR_FILE ${SRC_FILE}
        PATHS

        #drivers library
        "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/devices/cc32xx/driverlib"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/display"

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${DriversLibrary_SCR_FILE}") # debug helper
    list(APPEND TI_SimpleLink_SOURCES ${DriversLibrary_SCR_FILE})
endforeach()

# source files for SimpleLink 
set(SimpleLink_SRCS

    device.c
    driver.c
    flowcont.c
    fs.c
    netapp.c
    netcfg.c
    netutil.c
    nonos.c
    sl_socket.c
    spawn.c
    wlan.c
    cc_pal.c
    eventreg.c
    wlanconfig.c

    # eventreg.c

)

foreach(SRC_FILE ${SimpleLink_SRCS})
    set(SimpleLink_SCR_FILE SRC_FILE -NOTFOUND)
    find_file(SimpleLink_SCR_FILE ${SRC_FILE}
        PATHS

        #drivers library
        "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/drivers/net/wifi"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/drivers/net/wifi/porting"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/drivers/net/wifi/source"

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${SimpleLink_SCR_FILE}") # debug helper
    list(APPEND TI_SimpleLink_SOURCES ${SimpleLink_SCR_FILE})
endforeach()


# source files for SimpleLink net Wi-Fi 
set(SLNetWiFi_SRCS

    slnetsock.c
    slnetutils.c
    slnetif.c
    
    # bsd
    netdb.c
    socket.c
    errnoutil.c

    # drivers
    eventreg.c
    slnetifwifi.c

    # utils
    clock_sync.c
)

foreach(SRC_FILE ${SLNetWiFi_SRCS})
    set(SLNetWiFi_SCR_FILE SRC_FILE -NOTFOUND)
    find_file(SLNetWiFi_SCR_FILE ${SRC_FILE}
        PATHS

        "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/net"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/net/bsd"

        # drivers
        "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/drivers/net/wifi"
        "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/drivers/net/wifi/slnetif"

        # utils
        "${PROJECT_BINARY_DIR}/SimpleLinkCC32xxSDK_Source/ti/net/utils"

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${SLNetWiFi_SCR_FILE}") # debug helper
    list(APPEND TI_SimpleLink_SOURCES ${SLNetWiFi_SCR_FILE})
endforeach()


# source files for nanoFramework SSL driver
set(nanoSsl_SRCS

    ssl.cpp
)

foreach(SRC_FILE ${nanoSsl_SRCS})
    set(nanoSsl_SCR_FILE SRC_FILE -NOTFOUND)
    find_file(nanoSsl_SCR_FILE ${SRC_FILE}
        PATHS

        "${PROJECT_SOURCE_DIR}/src/PAL/COM/sockets/ssl"

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${nanoSsl_SCR_FILE}") # debug helper
    list(APPEND TI_SimpleLink_SOURCES ${nanoSsl_SCR_FILE})
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(SimpleLinkCC32xxSDK DEFAULT_MSG TI_SimpleLink_INCLUDE_DIRS TI_SimpleLink_SOURCES)
