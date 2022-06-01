#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
FetchContent_GetProperties(stm32l4_hal_driver)
FetchContent_GetProperties(cmsis_device_l4)
FetchContent_GetProperties(cmsis_core)

# set include directories
list(APPEND STM32L4_CubePackage_INCLUDE_DIRS ${cmsis_device_l4_SOURCE_DIR}/Include)
list(APPEND STM32L4_CubePackage_INCLUDE_DIRS ${stm32l4_hal_driver_SOURCE_DIR}/Inc)
list(APPEND STM32L4_CubePackage_INCLUDE_DIRS ${cmsis_core_SOURCE_DIR}/Include)
list(APPEND STM32L4_CubePackage_INCLUDE_DIRS ${TARGET_BASE_LOCATION})

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

    mx_common.c
)

foreach(SRC_FILE ${STM32L4_CubePackage_SRCS})

    set(STM32L4_CubePackage_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(STM32L4_CubePackage_SRC_FILE ${SRC_FILE}
        PATHS 

        ${stm32l4_hal_driver_SOURCE_DIR}/Src

        ${CMAKE_SOURCE_DIR}/targets/AzureRTOS/ST/_common/CubeMX

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${STM32L4_CubePackage_SRC_FILE}")
    endif()

    list(APPEND STM32L4_CubePackage_SOURCES ${STM32L4_CubePackage_SRC_FILE})
    
endforeach()

list(REMOVE_DUPLICATES STM32L4_CubePackage_INCLUDE_DIRS)

# remove duplicated defines that clash with ChibiOS defines (when used along with)
if(${CHIBIOS_HAL_REQUIRED})

    set(HAL_INCLUDE_FILE ${stm32l4_hal_driver_SOURCE_DIR}/Inc/Legacy/stm32_hal_legacy.h)

    # need to read the supplied files and rename the call
    file(READ
        "${HAL_INCLUDE_FILE}"
        HAL_INCLUDE_FILE_CONTENTS)

    string(REPLACE
        "KR_KEY_"
        "KR_KEY__DUMMY_FOR_NANO_BUILD"
        HAL_INCLUDE_FILE_FINAL_CONTENTS
        "${HAL_INCLUDE_FILE_CONTENTS}")
        
    file(WRITE 
        ${HAL_INCLUDE_FILE} 
        "${HAL_INCLUDE_FILE_FINAL_CONTENTS}")

endif()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(STM32L4_CubePackage DEFAULT_MSG STM32L4_CubePackage_INCLUDE_DIRS STM32L4_CubePackage_SOURCES)
