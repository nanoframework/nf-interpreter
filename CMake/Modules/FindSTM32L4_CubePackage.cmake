#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
FetchContent_GetProperties(stm32l4_cubepackage)

# set include directories
list(APPEND STM32L4_CubePackage_INCLUDE_DIRS ${stm32l4_cubepackage_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32L4xx/Include)
list(APPEND STM32L4_CubePackage_INCLUDE_DIRS ${stm32l4_cubepackage_SOURCE_DIR}/Drivers/STM32L4xx_HAL_Driver/Inc)

# HAL source files
set(STM32L4_CubePackage_SRCS

    # add HAL files here as required
    stm32l4xx_hal_cortex.c
    stm32l4xx_hal_crc_ex.c
    stm32l4xx_hal_crc.c
    stm32l4xx_hal_dma_ex.c
    stm32l4xx_hal_dma.c
    stm32l4xx_hal_i2c_ex.c
    stm32l4xx_hal_i2c.c
    stm32l4xx_hal_flash_ex.c
    stm32l4xx_hal_flash.c
    stm32l4xx_hal_gpio.c
    stm32l4xx_hal_pwr_ex.c
    stm32l4xx_hal_pwr.c
    stm32l4xx_hal_qspi.c
    stm32l4xx_hal_rcc_ex.c
    stm32l4xx_hal_rcc.c
    stm32l4xx_hal_rtc_ex.c
    stm32l4xx_hal_rtc.c
    stm32l4xx_hal_qspi.c
    stm32l4xx_hal_uart_ex.c
    stm32l4xx_hal_uart.c
    stm32l4xx_hal.c
)

foreach(SRC_FILE ${STM32L4_CubePackage_SRCS})

    set(STM32L4_CubePackage_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(STM32L4_CubePackage_SRC_FILE ${SRC_FILE}
        PATHS 

        ${stm32l4_cubepackage_SOURCE_DIR}/Drivers/STM32L4xx_HAL_Driver/Src

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${STM32L4_CubePackage_SRC_FILE}")
    endif()

    list(APPEND STM32L4_CubePackage_SOURCES ${STM32L4_CubePackage_SRC_FILE})
    
endforeach()

# BSP
if(${TARGET_BOARD} STREQUAL "ST_B_L475E_IOT01A")

    # ST_B_L475E_IOT01A
    list(APPEND STM32L4_CubePackage_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/STM32L4_CubePackage_Source/Drivers/BSP/B-L475E-IOT01)
    list(APPEND STM32L4_CubePackage_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/STM32L4_CubePackage_Source/Drivers/BSP/Components/hts221)

    set(BSP_CubePackage_SRCS

        # BSP
        stm32l475e_iot01_accelero.c
        stm32l475e_iot01_gyro.c
        stm32l475e_iot01_hsensor.c
        stm32l475e_iot01_magneto.c
        stm32l475e_iot01_psensor.c
        stm32l475e_iot01_qspi.c
        stm32l475e_iot01_tsensor.c
        stm32l475e_iot01.c

        # Sensors
        hts221.c
        lis3mdl.c
        lsm6dsl.c
        lps22hb.c
    )

    foreach(SRC_FILE ${BSP_CubePackage_SRCS})
        set(BSP_CubePackage_SRC_FILE SRC_FILE-NOTFOUND)
        find_file(BSP_CubePackage_SRC_FILE ${SRC_FILE}
            PATHS 

            # BSP
            ${CMAKE_BINARY_DIR}/STM32L4_CubePackage_Source/Drivers/BSP/B-L475E-IOT01

            # Sensors
            ${CMAKE_BINARY_DIR}/STM32L4_CubePackage_Source/Drivers/BSP/Components/hts221
            ${CMAKE_BINARY_DIR}/STM32L4_CubePackage_Source/Drivers/BSP/Components/lis3mdl
            ${CMAKE_BINARY_DIR}/STM32L4_CubePackage_Source/Drivers/BSP/Components/lsm6dsl
            ${CMAKE_BINARY_DIR}/STM32L4_CubePackage_Source/Drivers/BSP/Components/lps22hb

            CMAKE_FIND_ROOT_PATH_BOTH
        )
        # message("${SRC_FILE} >> ${BSP_CubePackage_SRC_FILE}") # debug helper
        list(APPEND STM32L4_CubePackage_SOURCES ${BSP_CubePackage_SRC_FILE})
    endforeach()

    # unset these warnings as errors because these come from a 3rd party library 
    SET_SOURCE_FILES_PROPERTIES( ${CMAKE_BINARY_DIR}/STM32L4_CubePackage_Source/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01.c PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)
    SET_SOURCE_FILES_PROPERTIES( ${CMAKE_BINARY_DIR}/STM32L4_CubePackage_Source/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_qspi.c PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)
    SET_SOURCE_FILES_PROPERTIES( ${CMAKE_BINARY_DIR}/STM32L4_CubePackage_Source/Drivers/BSP/Components/hts221/hts221.c PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)
    SET_SOURCE_FILES_PROPERTIES( ${CMAKE_BINARY_DIR}/STM32L4_CubePackage_Source/Drivers/BSP/Components/lps22hb/lps22hb.c PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)
 
endif()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(STM32L4_CubePackage DEFAULT_MSG STM32L4_CubePackage_INCLUDE_DIRS STM32L4_CubePackage_SOURCES)
