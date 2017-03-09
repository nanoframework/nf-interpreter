#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# try to find board in source 
if(EXISTS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/boards/${CHIBIOS_BOARD})
    # board found
    message(STATUS "ChibiOS board '${CHIBIOS_BOARD}' found")
else()

    # board NOT found in source
    # try to find it in nanoFramework community overlay 
    if(EXISTS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/boards/${CHIBIOS_BOARD})
        # board found
        message(STATUS "ChibiOS board '${CHIBIOS_BOARD}' found in nanoFramework overlays")
    else()

        # board NOT found in overlay
        # try to find it in the target boards
        if(EXISTS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/${CHIBIOS_BOARD})
            # board found!
            # in this case it's mandatory that the board definitions (board.c and board.h) are present in the target folder
            if( EXISTS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/${CHIBIOS_BOARD}/board.c AND
                EXISTS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/${CHIBIOS_BOARD}/board.h)
                # everything seems to be in order
                message(STATUS "ChibiOS board '${CHIBIOS_BOARD}' (including board definition files) found in nanoFramework targets")
            else()
                message(FATAL_ERROR "\n\nSorry but the board definition files (board.c and board.h) for ${CHIBIOS_BOARD} seem to be missing in the target directory...\n\n")
            endif()

        else()
            message(FATAL_ERROR "\n\nSorry but ${CHIBIOS_BOARD} seems to be missing in the available list of the ChibiOS supported boards...\n\n")
        endif()

    endif()

endif()

# try to find board in the targets folder
if(EXISTS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/${CHIBIOS_BOARD})
    # board found
    message(STATUS "support for target board '${CHIBIOS_BOARD}' found")
else()

    # board NOT found in targets folder
    message(FATAL_ERROR "\n\nSorry but support for ${CHIBIOS_BOARD} target is not available...\n\You can wait for that to be added or you might want to contribute and start working on a PR for that.\n\n")

endif()

###################################################################################################################################
# WHEN ADDING A NEW series add the respective name to the list bellow along with the CMake files with GCC options and source files
###################################################################################################################################

# check if the series name is supported 

set(CHIBIOS_SUPPORTED_SERIES "STM32F0xx" "STM32F4xx" "STM32F7xx" CACHE INTERNAL "supported series names for ChibiOS")
list(FIND CHIBIOS_SUPPORTED_SERIES ${TARGET_SERIES} TARGET_SERIES_NAME_INDEX)
if(TARGET_SERIES_NAME_INDEX EQUAL -1)
    message(FATAL_ERROR "\n\nSorry but ${TARGET_SERIES} is not supported at this time...\nYou can wait for that to be added or you might want to contribute and start working on a PR for that.\n\n")
endif()

# including here the CMake files for the source files specific to the target series
include(CHIBIOS_${TARGET_SERIES}_sources)
# and here the GCC options tuned for the target series 
include(CHIBIOS_${TARGET_SERIES}_GCC_options)

# message("ChibiOS board series is ${TARGET_SERIES}") # debug helper

# set include directories for ChibiOS
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/license)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/common/ARMCMx)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/include)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/boards/${CHIBIOS_BOARD})
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/osal/rt)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/rt/include)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/rt/ports/ARMCMx)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/rt/ports/ARMCMx/compilers/GCC)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/ext/CMSIS/include)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/ext/CMSIS/ST/${TARGET_SERIES})
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/common/ports/ARMCMx/compilers/GCC)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/ports/STM32/${TARGET_SERIES})
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/rt/ports/ARMCMx/cmsis_os)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/common/ports/ARMCMx)

# append include directory for boards in the nanoFramework community overlay
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/boards/${CHIBIOS_BOARD})

# source files and GCC options according to target vendor and series

# source files for ChibiOS
set(CHIBIOS_SRCS
    # HAL-OSAL files
    hal.c
    st.c
    
    hal_buffers.c
    hal_queues.c
    hal_mmcsd.c
    
    adc.c
    can.c
    dac.c
    ext.c
    gpt.c
    i2c.c
    i2s.c
    icu.c
    mac.c
    mmc_spi.c
    pal.c
    pwm.c
    rtc.c
    sdc.c
    serial.c
    serial_usb.c
    spi.c
    uart.c
    usb.c
    wdg.c

    # OSAL RT
    osal.c

    # RT
    chsys.c
    chdebug.c
    chvt.c
    chschd.c
    chthreads.c
    chtm.c
    chstats.c
    chdynamic.c
    chregistry.c
    chsem.c
    chmtx.c
    chcond.c
    chevents.c
    chmsg.c
    
    chheap.c
    chmboxes.c
    chmemcore.c
    chmempools.c

    chqueues.c

    # CMSIS
    cmsis_os.c

    # board file(s)
    board.c

)

foreach(SRC_FILE ${CHIBIOS_SRCS})
    set(CHIBIOS_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(CHIBIOS_SRC_FILE ${SRC_FILE}
        PATHS 
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/src
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/osal/rt
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/rt/src
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/common/oslib/src
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/rt/ports/ARMCMx/cmsis_os
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/common/abstractions/cmsis_os
        
            # this path hint is for the usual location of the board.c file
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/os/hal/boards/${CHIBIOS_BOARD}
         
            # this path hint is for the alternative boards folder in the nanoFramework community overlay
            ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/boards/${CHIBIOS_BOARD}

            # this path hint is for OEM boards for which the board file(s) are probably located directly in the "target" folder along with remaining files
            ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/${CHIBIOS_BOARD}

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${CHIBIOS_SRC_FILE}") # debug helper
    list(APPEND CHIBIOS_SOURCES ${CHIBIOS_SRC_FILE})
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(CHIBIOS DEFAULT_MSG CHIBIOS_INCLUDE_DIRS CHIBIOS_SOURCES)
