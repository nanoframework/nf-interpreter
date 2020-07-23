#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# append source files required for ChibiOS interface  
list(APPEND COMMON_PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Device_BlockStorage$<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:-DEBUG>.c")
list(APPEND COMMON_PROJECT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/stm32f0xx_hal_msp.c)
list(APPEND COMMON_PROJECT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/system_stm32f0xx.c)
list(APPEND COMMON_PROJECT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/stm32f0xx_it.c)
list(APPEND COMMON_PROJECT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/common.c)

# append startup code from ST HAL package
list(APPEND COMMON_PROJECT_SOURCES ${PROJECT_BINARY_DIR}/STM32F0_CubePackage_Source/Drivers/CMSIS/Device/ST/STM32F0xx/Source/Templates/gcc/startup_stm32f091xc.s)

# make var global
set(COMMON_PROJECT_SOURCES ${COMMON_PROJECT_SOURCES} CACHE INTERNAL "make global")
