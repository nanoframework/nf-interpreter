# try to find board
if(EXISTS ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/boards/${CHIBIOS_BOARD})
    # board found
    message(STATUS "ChibiOS board '${CHIBIOS_BOARD}' found")
else()
    message(FATAL_ERROR "\n\nSorry but ${CHIBIOS_BOARD} seems to be missing in the available list of the ChibiOS supported boards...\n\n")
endif()

# try to find out the MCU vendor from the board name
string(FIND ${CHIBIOS_BOARD} "ST_STM" ST_STM_BOARD_INDEX)  # boards name containing ST_STM
string(FIND ${CHIBIOS_BOARD} "ST_NUCLEO" ST_NUCLEO_BOARD_INDEX)  # boards name containing ST_NUCLEO
string(FIND ${CHIBIOS_BOARD} "ST_EVB" ST_EVB_BOARD_INDEX)  # boards name containing ST_EVB
string(FIND ${CHIBIOS_BOARD} "OLIMEX_STM32" OLIMEX_STM32_BOARD_INDEX)  # boards name containing OLIMEX_STM32

if( ST_STM_BOARD_INDEX GREATER -1 OR
    ST_NUCLEO_BOARD_INDEX GREATER -1 OR 
    ST_EVB_BOARD_INDEX GREATER -1 OR 
    OLIMEX_STM32_BOARD_INDEX GREATER -1
)
    set(CHIBIOS_TARGET_VENDOR "STM")
endif()

message("ChibiOS board MCU vendor is ${CHIBIOS_TARGET_VENDOR}") # debug helper

# try to find the MCU series from the board name
if("${CHIBIOS_TARGET_VENDOR}" STREQUAL "STM")
    # vendor is STM, so check the list for each series

    ####################################################
    # add here all boards with MCU from STM32F0xx series 
    ####################################################
    set(STM32_F0xx_BOARDS 
        ST_NUCLEO_F091RC 
        ST_NUCLEO_F072RB
        CACHE INTERNAL "F0xx series board")
    list(FIND STM32_F0xx_BOARDS ${CHIBIOS_BOARD} STM32_F0xx_BOARDS_INDEX)
    
    ####################################################
    # add here all boards with MCU from STM32F4xx series 
    ####################################################
    set(STM32_F4xx_BOARDS
        ST_STM32F4_DISCOVERY 
        CACHE INTERNAL "F4xx series board")
    list(FIND STM32_F4xx_BOARDS ${CHIBIOS_BOARD} STM32_F4xx_BOARDS_INDEX)


    ####################################################
    if(STM32_F0xx_BOARDS_INDEX GREATER -1)
        set(CHIBIOS_BOARD_SERIES "STM32F0xx")
    elseif(STM32_F4xx_BOARDS_INDEX GREATER -1)
        set(CHIBIOS_BOARD_SERIES "STM32F4xx")
    else()
        message(FATAL_ERROR "\n\n${CHIBIOS_BOARD} is not on any of the series lists.\nPlease add it to the correct series list and submit a PR.\n\n")
    endif()

    include(CHIBIOS_${CHIBIOS_BOARD_SERIES}_sources)
    include(CHIBIOS_${CHIBIOS_BOARD_SERIES}_GCC_options)

endif()

message("ChibiOS board MCU series is ${CHIBIOS_BOARD_SERIES}") # debug helper

# set include directories for ChibiOS
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/ports/common/ARMCMx)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/include)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/boards/${CHIBIOS_BOARD})
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/osal/rt)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/rt/include)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/rt/ports/ARMCMx)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/rt/ports/ARMCMx/compilers/GCC)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/CMSIS/include)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/ext/CMSIS/ST/${CHIBIOS_BOARD_SERIES})
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/common/ports/ARMCMx/compilers/GCC)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/common/ports/ARMCMx/compilers/GCC)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/ports/STM32/${CHIBIOS_BOARD_SERIES})
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/rt/ports/ARMCMx/cmsis_os)
list(APPEND CHIBIOS_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/ChibiOS_Source/various/cpp_wrappers)

# source files and GCC options according to target vendor and series


# source file(s) for board
list(APPEND CHIBIOS_SOURCES ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/boards/${CHIBIOS_BOARD}/board.c)

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
    chmboxes.c
    chqueues.c
    chmemcore.c
    chheap.c
    chmempools.c

    # CMSIS
    cmsis_os.c

    # CPP wrappers
    ch.cpp
    syscalls_cpp.cpp
)

foreach(SRC_FILE ${CHIBIOS_SRCS})
    set(CHIBIOS_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(CHIBIOS_SRC_FILE ${SRC_FILE}
        PATHS 
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/src
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/hal/osal/rt
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/rt/src
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/rt/ports/ARMCMx/cmsis_os
            ${PROJECT_BINARY_DIR}/ChibiOS_Source/various/cpp_wrappers

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${CHIBIOS_SRC_FILE}") # debug helper
    list(APPEND CHIBIOS_SOURCES ${CHIBIOS_SRC_FILE})
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(CHIBIOS DEFAULT_MSG CHIBIOS_INCLUDE_DIRS CHIBIOS_SOURCES)
