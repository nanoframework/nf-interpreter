#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# set include directories for ChibiOS nanoFramework Overlay
list(APPEND ChibiOSnfOverlay_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/include)

####################################################################################
# WHEN ADDING A NEW CHIBIOS OVERLAY component add the include directory(ies) below 
####################################################################################
# component STM32_FLASH
list(APPEND ChibiOSnfOverlay_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/include/stm32_flash)
# component STM32_CRC
list(APPEND ChibiOSnfOverlay_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/include/stm32_crc)
# component STM32_RNG
list(APPEND ChibiOSnfOverlay_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/include/stm32_rng)
# component STM32_FSMC (Flexible Memory Controller)
list(APPEND ChibiOSnfOverlay_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/include/stm32_fsmc)
# component STM32_ONEWIRE (One Wire driver)
list(APPEND ChibiOSnfOverlay_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/include/stm32_onewire)
# component STM32_QSPI (QSPI driver)
list(APPEND ChibiOSnfOverlay_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/include/stm32_qspi)

##################################################################################################################################
# Add above the required include directory(ies) for a new nanoFramework overlay component that you are adding
# following the template below. 
#
# If the component includes a low level driver specific to a series, make sure you add the include dirs 
# in the CHIBIOS_NNNNNN_sources.cmake
#
# list(APPEND ChibiOSnfOverlay_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/include/<path-here>)
##################################################################################################################################


##################################################
# source files for ChibiOS nanoFramework Overlay
##################################################
list(APPEND ChibiOSnfOverlay_SOURCES ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/src/hal_community.c)

####################################################################################################
# WHEN ADDING A NEW CHIBIOS OVERLAY component add the source file(s) specific to this series below 
####################################################################################################
# component STM32_FLASH
list(APPEND ChibiOSnfOverlay_SOURCES ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/src/stm32_flash/hal_stm32_flash.c)
# component STM32_CRC
list(APPEND ChibiOSnfOverlay_SOURCES ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/src/stm32_crc/hal_stm32_crc.c)
# component STM32_RNG
list(APPEND ChibiOSnfOverlay_SOURCES ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/src/stm32_rng/hal_stm32_rng.c) 
# component STM32_FSMC (Flexible Memory Controller)
list(APPEND ChibiOSnfOverlay_SOURCES ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/src/stm32_fsmc/hal_stm32_fsmc.c)
# component STM32_ONEWIRE (One Wire driver)
list(APPEND ChibiOSnfOverlay_SOURCES ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/src/stm32_onewire/hal_stm32_onewire.c)
# component STM32_QSPI (QSPI driver)
list(APPEND ChibiOSnfOverlay_SOURCES ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/src/stm32_qspi/hal_stm32_qspi.c)

#########################################################################################################################
# Add above ALL the source code file(s) required for a new nanoFramework overlay component that you are adding
# following the template below.
#
# If the component includes a low level driver specific to a series, make sure you add the source files 
# in the CHIBIOS_NNNNNN_sources.cmake
#
# list(APPEND ChibiOSnfOverlay_SOURCES ${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/nf-overlay/os/hal/src/<path-here>)
#########################################################################################################################


include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ChibiOSnfOverlay DEFAULT_MSG ChibiOSnfOverlay_INCLUDE_DIRS ChibiOSnfOverlay_SOURCES)
