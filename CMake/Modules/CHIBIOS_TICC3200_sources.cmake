#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# List of the ChibiOS generic CC3200 startup and CMSIS files.

# set include directories for CC3200 SDK
# list(APPEND CHIBIOS_INCLUDE_DIRS ${TI_CC3200_SDK_PATH})
# list(APPEND CHIBIOS_INCLUDE_DIRS ${TI_CC3200_SDK_PATH}/example/common)
# list(APPEND CHIBIOS_INCLUDE_DIRS ${TI_CC3200_SDK_PATH}/inc)
# list(APPEND CHIBIOS_INCLUDE_DIRS ${TI_CC3200_SDK_PATH}/driverlib)
# list(APPEND CHIBIOS_INCLUDE_DIRS ${TI_CC3200_SDK_PATH}/middleware/driver)
# list(APPEND CHIBIOS_INCLUDE_DIRS ${TI_CC3200_SDK_PATH}/middleware/driver/hal)
# list(APPEND CHIBIOS_INCLUDE_DIRS ${TI_CC3200_SDK_PATH}/middleware/framework/pm)
# list(APPEND CHIBIOS_INCLUDE_DIRS ${TI_CC3200_SDK_PATH}/middleware/framework/timer)
# list(APPEND CHIBIOS_INCLUDE_DIRS ${TI_CC3200_SDK_PATH}/middleware/soc)
# list(APPEND CHIBIOS_INCLUDE_DIRS ${TI_CC3200_SDK_PATH}/oslib)
# list(APPEND CHIBIOS_INCLUDE_DIRS ${TI_CC3200_SDK_PATH}/simplelink)
# list(APPEND CHIBIOS_INCLUDE_DIRS ${TI_CC3200_SDK_PATH}/simplelink/include)
# list(APPEND CHIBIOS_INCLUDE_DIRS ${TI_CC3200_SDK_PATH}/simplelink_extlib/include)
# list(APPEND CHIBIOS_INCLUDE_DIRS ${TI_CC3200_SDK_PATH}/simplelink_extlib/flc)
# list(APPEND CHIBIOS_INCLUDE_DIRS ${TI_CC3200_SDK_PATH}/simplelink_extlib/ota)
# list(APPEND CHIBIOS_INCLUDE_DIRS ${TI_CC3200_SDK_PATH}/simplelink_extlib/provisioninglib)
# list(APPEND CHIBIOS_INCLUDE_DIRS ${TI_CC3200_SDK_PATH}/third_party/FreeRTOS/source/include)
# list(APPEND CHIBIOS_INCLUDE_DIRS ${TI_CC3200_SDK_PATH}/third_party/FreeRTOS/source/portable/GCC/ARM_CM4)
# list(APPEND CHIBIOS_INCLUDE_DIRS ${TI_CC3200_SDK_PATH}/third_party/FreeRTOS/source)
# list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/targets/TI-RTOS/common/include)

# XDC tools includes
# list(APPEND CHIBIOS_INCLUDE_DIRS ${TI_XDCTOOLS_INSTALLATION_DIR}/packages)
# list(APPEND CHIBIOS_INCLUDE_DIRS ${TI_XDCTOOLS_INSTALLATION_DIR}/packages/xdc)
# # list(APPEND CHIBIOS_INCLUDE_DIRS ${TI_CC3200_SDK_PATH}/ti_rtos/ti_rtos_config/ewarm/iar/tirtos/xdc)
# # list(APPEND CHIBIOS_INCLUDE_DIRS ${TI_CC3200_SDK_PATH}/ti_rtos/ti_rtos_config/ewarm/iar/tirtos/ti/targets)

# ChibiOS includes
# list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os) 
# list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/license)
# list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/common/ARMCMx)
# list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/include)
# list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/boards/${CHIBIOS_BOARD})
# list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/osal/rt)
# list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/common/oslib/include)
# list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/common/ports/ARMCMx)
# list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/common/ports/ARMCMx/compilers/GCC)
# list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/common/abstractions/cmsis_os)

list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/rt/include)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/common/ARMCMx)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/common/abstractions/cmsis_os)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/common/ports/ARMCMx/compilers/GCC)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/common/ext/CMSIS/TI/TICC3200)


list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/common/startup/ARMCMx/compilers/GCC)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/common/ext/CMSIS/include)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/common/startup/ARMCMx/devices/TICC3200)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/common/ext/CCWare)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/common/ext/CCWare/inc)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/ports/TI/TICC3200)

list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/ports/TI/LLD/GPTM)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/ports/TI/LLD/GPIO)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/ports/TI/LLD/UART)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/ports/TI/LLD/RTC)


# source files for CC3200 SDK
set(CHIBIOS_PORT_SRCS

    # startup code
    crt1.c
    vectors.c
    crt0_v7m.S

    nvic.c
    hal_lld.c

    # 
    hal_st_lld.c
    hal_pal_lld.c

    # examples common
    # button_if.c
    # gpio_if.c
    # i2c_if.c
    # i2s_if.c
    # network_common.c
    # network_if.c
    #######startup_gcc.c
    # timer_if.c
    # uart_if.c
    # udma_if.c
    # utils_if.c
    # wdt_if.c

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
    # osi_freertos.c
    # port.c
    # heap_2.c

    # FreeRTOS
    # croutine.c
    # event_groups.c
    # list.c
    # queue.c
    # tasks.c 
    # timers.c

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


    # # ChibiOS 
   
    # # OSAL RT
    # osal.c

    # # RT
    # chsys.c
    # chdebug.c
    # chvt.c
    # chschd.c
    # chthreads.c
    # chtm.c
    # chstats.c
    # chdynamic.c
    # chregistry.c
    # chsem.c
    # chmtx.c
    # chcond.c
    # chevents.c
    # chmsg.c
    
    # chheap.c
    # chmboxes.c
    # chmemcore.c
    # chmempools.c

    # # required to use malloc and other newlib stuff
    # syscalls.c

    # # CMSIS
    # cmsis_os.c

    # # board file(s)
    # # board.c

    # # RT
    # chcore.c
    # chcore_v7m.c
    # chcoreasm_v7m.S

    # startup code
    # crt1.c
    # vectors.c
    # crt0_v7m.S

    # nvic.c
    # hal_lld.c
    # hal_ext_lld_isr.c
    
)

foreach(SRC_FILE ${CHIBIOS_PORT_SRCS})
    set(CC3200_SCR_FILE SRC_FILE -NOTFOUND)
    find_file(CC3200_SCR_FILE ${SRC_FILE}
        PATHS

        # examples common
        # ${TI_CC3200_SDK_PATH}/example/common

        # drivers
        # ${TI_CC3200_SDK_PATH}/driverlib

        # middleware
        # ${TI_CC3200_SDK_PATH}/middleware/driver
        # ${TI_CC3200_SDK_PATH}/middleware/driver/hal
        # ${TI_CC3200_SDK_PATH}/middleware/framework/pm
        # ${TI_CC3200_SDK_PATH}/middleware/framework/timer
        # ${TI_CC3200_SDK_PATH}/middleware/soc

        # netapps

        # oslib
        # ${TI_CC3200_SDK_PATH}/oslib
        # ${TI_CC3200_SDK_PATH}/third_party/FreeRTOS/source
        # ${TI_CC3200_SDK_PATH}/third_party/FreeRTOS/source/portable/GCC/ARM_CM4
        # ${TI_CC3200_SDK_PATH}/third_party/FreeRTOS/source/portable/MemMang

        # simplelink
        # ${TI_CC3200_SDK_PATH}/simplelink
        # ${TI_CC3200_SDK_PATH}/simplelink/source

        # simplelink_extlib
        # ${TI_CC3200_SDK_PATH}/simplelink_extlib/flc
        # ${TI_CC3200_SDK_PATH}/simplelink_extlib/ota
        # ${TI_CC3200_SDK_PATH}/simplelink_extlib/provisioninglib

        # ChibiOS
        # ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/src
        # ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/osal/rt
        # ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/rt/src
        # # ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/common/ARMCMx
        # ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/common/oslib/src
        # ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/common/abstractions/cmsis_os
        # ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/common/ports/ARMCMx/compilers/GCC
        # ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/common/startup/ARMCMx/compilers/GCC
        # ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/common/ports/ARMCMx
        ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/various
        ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/common/ports/ARMCMx/compilers/GCC
        ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/common/startup/ARMCMx/compilers/GCC
        ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/common/ports/ARMCMx
        ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/common/ARMCMx

        ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/ports/TI/TICC3200
        ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/ports/TI/LLD/GPTM
        ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/ports/TI/LLD/GPIO
        ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/ports/TI/LLD/UART

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${CC3200_SCR_FILE}") # debug helper
    list(APPEND CHIBIOS_SOURCES ${CC3200_SCR_FILE})
endforeach()
