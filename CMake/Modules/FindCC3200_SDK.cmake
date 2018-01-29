#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# and here the GCC options tuned for the target series 
include(TIRTOS_CC3200_GCC_options)

# set include directories for CC3200 SDK
list(APPEND CC3200_SDK_INCLUDE_DIRS ${TI_CC3200_SDK_PATH})
list(APPEND CC3200_SDK_INCLUDE_DIRS ${TI_CC3200_SDK_PATH}/example/common)
list(APPEND CC3200_SDK_INCLUDE_DIRS ${TI_CC3200_SDK_PATH}/inc)
list(APPEND CC3200_SDK_INCLUDE_DIRS ${TI_CC3200_SDK_PATH}/driverlib)
list(APPEND CC3200_SDK_INCLUDE_DIRS ${TI_CC3200_SDK_PATH}/middleware/driver)
list(APPEND CC3200_SDK_INCLUDE_DIRS ${TI_CC3200_SDK_PATH}/middleware/driver/hal)
list(APPEND CC3200_SDK_INCLUDE_DIRS ${TI_CC3200_SDK_PATH}/middleware/framework/pm)
list(APPEND CC3200_SDK_INCLUDE_DIRS ${TI_CC3200_SDK_PATH}/middleware/framework/timer)
list(APPEND CC3200_SDK_INCLUDE_DIRS ${TI_CC3200_SDK_PATH}/middleware/soc)
list(APPEND CC3200_SDK_INCLUDE_DIRS ${TI_CC3200_SDK_PATH}/oslib)
list(APPEND CC3200_SDK_INCLUDE_DIRS ${TI_CC3200_SDK_PATH}/simplelink)
list(APPEND CC3200_SDK_INCLUDE_DIRS ${TI_CC3200_SDK_PATH}/simplelink/include)
list(APPEND CC3200_SDK_INCLUDE_DIRS ${TI_CC3200_SDK_PATH}/simplelink_extlib/include)
list(APPEND CC3200_SDK_INCLUDE_DIRS ${TI_CC3200_SDK_PATH}/simplelink_extlib/flc)
list(APPEND CC3200_SDK_INCLUDE_DIRS ${TI_CC3200_SDK_PATH}/simplelink_extlib/ota)
list(APPEND CC3200_SDK_INCLUDE_DIRS ${TI_CC3200_SDK_PATH}/simplelink_extlib/provisioninglib)
list(APPEND CC3200_SDK_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/targets/TI-RTOS/common/include)

# XDC tools includes
list(APPEND CC3200_SDK_INCLUDE_DIRS ${TI_XDCTOOLS_INSTALLATION_DIR}/packages)
list(APPEND CC3200_SDK_INCLUDE_DIRS ${TI_XDCTOOLS_INSTALLATION_DIR}/packages/xdc)
# list(APPEND CC3200_SDK_INCLUDE_DIRS ${TI_CC3200_SDK_PATH}/ti_rtos/ti_rtos_config/ewarm/iar/tirtos/xdc)
# list(APPEND CC3200_SDK_INCLUDE_DIRS ${TI_CC3200_SDK_PATH}/ti_rtos/ti_rtos_config/ewarm/iar/tirtos/ti/targets)

# source files for CC3200 SDK
set(CC3200_SDK_SRCS

    # examples common
    button_if.c
    gpio_if.c
    i2c_if.c
    i2s_if.c
    network_common.c
    network_if.c
    startup_gcc.c
    timer_if.c
    uart_if.c
    udma_if.c
    utils_if.c
    wdt_if.c

    # drivers
    # adc.c
    # aes.c
    # camera.c
    # cpu.c
    # crc.c
    # des.c
    # flash.c
    # gpio.c
    # i2c.c
    # i2s.c
    # interrupt.c
    # pin.c
    # prcm.c
    # sdhost.c
    # shamd5.c
    # spi.c
    # systick.c
    # timer.c
    # uart.c
    # udma.c
    # utils.c
    # wdt.c

    # middleware/driver/hal
    # dma_hal.c
    # gpio_hal.c
    # hw_timer32.c
    # hw_timer32.c
    # rtc_hal.c
    # spi_hal.c
    # uart_hal.c
    # util64.c

    # middleware/driver
    # spi_drv.c
    # uart_drv.c

    # middleware/framework/pm
    # cc_pm.c

    # middleware/framework/timer
    # cc_timer.c

    # middleware/soc
    # cc_io_park.c
    # cc_pm_ops.c

    # netapps

    # oslib
    # osi_tirtos.c

    # simplelink
    # cc_pal_pm.c
    # cc_pal.c
    # device.c
    # flowcont.c
    # fs.c
    # netapp.c
    # netcfg.c
    # nonos.c
    # socket.c
    # spawn.c
    # wlan.c

    # simplelink_extlib
    # flc.c
    # CdnClient.c
    # LogClient.c
    # OtaApp.c
    # OtaClient.c
    # OtaHttp.c
    # cfg_confirm.c
    # events_handler.c
    # networkListHndl.c
    # provisioning_logic.c

)

foreach(SRC_FILE ${CC3200_SDK_SRCS})
    set(CC3200_SDK_FILE SRC_FILE -NOTFOUND)
    find_file(CC3200_SDK_FILE ${SRC_FILE}
        PATHS

        # examples common
        ${TI_CC3200_SDK_PATH}/example/common

        # drivers
        ${TI_CC3200_SDK_PATH}/driverlib

        # middleware
        ${TI_CC3200_SDK_PATH}/middleware/driver
        ${TI_CC3200_SDK_PATH}/middleware/driver/hal
        ${TI_CC3200_SDK_PATH}/middleware/framework/pm
        ${TI_CC3200_SDK_PATH}/middleware/framework/timer
        ${TI_CC3200_SDK_PATH}/middleware/soc

        # netapps

        # oslib
        ${TI_CC3200_SDK_PATH}/oslib

        # simplelink
        ${TI_CC3200_SDK_PATH}/simplelink
        ${TI_CC3200_SDK_PATH}/simplelink/source

        # simplelink_extlib
        ${TI_CC3200_SDK_PATH}/simplelink_extlib/flc
        ${TI_CC3200_SDK_PATH}/simplelink_extlib/ota
        ${TI_CC3200_SDK_PATH}/simplelink_extlib/provisioninglib

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${CC3200_SDK_FILE}") # debug helper
    list(APPEND CC3200_SDK_SOURCES ${CC3200_SDK_FILE})
endforeach()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(CC3200_SDK DEFAULT_MSG CC3200_SDK_INCLUDE_DIRS CC3200_SDK_SOURCES)
