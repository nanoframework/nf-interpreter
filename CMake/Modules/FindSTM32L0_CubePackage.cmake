#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# set include directories
list(APPEND STM32L0_CubePackage_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/STM32L0_CubePackage_Source/Drivers/STM32L0xx_HAL_Driver/Inc)
list(APPEND STM32L0_CubePackage_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/STM32L0_CubePackage_Source/Drivers/CMSIS/Device/ST/STM32L0xx/Include)
list(APPEND STM32L0_CubePackage_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/STM32L0_CubePackage_Source/Drivers/CMSIS/Include)
list(APPEND STM32L0_CubePackage_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/STM32L0_CubePackage_Source/Drivers/BSP/STM32L0xx_Nucleo)
list(APPEND STM32L0_CubePackage_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/STM32L0_CubePackage_Source/Middlewares/Third_Party/FreeRTOS/Source/Include)
list(APPEND STM32L0_CubePackage_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/STM32L0_CubePackage_Source/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS)
list(APPEND STM32L0_CubePackage_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/STM32L0_CubePackage_Source/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM0)

# and here the GCC options tuned for the target series 
include(FREERTOS_${TARGET_SERIES}_GCC_options)

# source files
set(STM32L0_CubePackage_SRCS

    # cmsis_os.c

    stm32l0xx_hal.c
    stm32l0xx_hal_cortex.c
    stm32l0xx_hal_dma.c
    stm32l0xx_hal_gpio.c
    stm32l0xx_hal_pwr.c
    stm32l0xx_hal_pwr_ex.c
    stm32l0xx_hal_rcc.c
    stm32l0xx_hal_rcc_ex.c
    stm32l0xx_hal_rtc.c
    stm32l0xx_hal_uart.c
    stm32l0xx_hal_uart_ex.c
)

foreach(SRC_FILE ${STM32L0_CubePackage_SRCS})
    set(STM32L0_CubePackage_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(STM32L0_CubePackage_SRC_FILE ${SRC_FILE}
        PATHS 

        ${PROJECT_BINARY_DIR}/STM32L0_CubePackage_Source/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS

        ${PROJECT_BINARY_DIR}/STM32L0_CubePackage_Source/Drivers/STM32L0xx_HAL_Driver/Src

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${STM32L0_CubePackage_SRC_FILE}") # debug helper
    list(APPEND STM32L0_CubePackage_SOURCES ${STM32L0_CubePackage_SRC_FILE})
endforeach()

# unset these warnings as errors because we don't want to have to patch these source files
set_source_files_properties( ${PROJECT_BINARY_DIR}/STM32L0_CubePackage_Source/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.c PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)
set_source_files_properties( ${PROJECT_BINARY_DIR}/STM32L0_CubePackage_Source/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_cortex.c PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(STM32L0_CubePackage DEFAULT_MSG STM32L0_CubePackage_INCLUDE_DIRS STM32L0_CubePackage_SOURCES)
