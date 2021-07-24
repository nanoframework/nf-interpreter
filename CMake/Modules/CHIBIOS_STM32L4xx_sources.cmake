#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
FetchContent_GetProperties(chibios)

# from startup_stm32L4xx.mk
# List of the ChibiOS generic STM32L4xx startup and CMSIS files.

################################################################################################
# WHEN ADDING A NEW BOARD add the source code file for the hal/ports/STM32/STM32L4xx/platform.mk
################################################################################################

set(CHIBIOS_PORT_SRCS
   
       # RT
       chcore.c
       chcoreasm.S
)

foreach(SRC_FILE ${CHIBIOS_PORT_SRCS})

    set(CHIBIOS_L4_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(CHIBIOS_L4_SRC_FILE ${SRC_FILE}
        PATHS 

            ${chibios_SOURCE_DIR}/os/common/ports/ARMv7-M/compilers/GCC
            ${chibios_SOURCE_DIR}/os/common/ports/ARMv7-M

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${CHIBIOS_L4_SRC_FILE}")
    endif()

    list(APPEND CHIBIOS_SOURCES ${CHIBIOS_L4_SRC_FILE})

endforeach()

list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/common/ports/ARMv7-M)

####################################################################################
# WHEN ADDING A NEW CHIBIOS OVERLAY component add the include directory(ies) below 
####################################################################################
# component STM32_FLASH
list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/STM32/LLD/FLASHv2)
# component STM32_CRC
list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/STM32/LLD/CRCv1)
# component STM32_RNG
list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/STM32/LLD/RNGv1) 
# component STM32_FSMC (Flexible Memory Controller)
list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/STM32/LLD/FSMCv1)
# component STM32_ONEWIRE (One Wire driver)
list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/STM32/LLD/ONEWIREv1)
# component STM32_QSPI (Quad-SPI)
list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/STM32/LLD/QSPIv1)

###############################################################################################################################
# Add above the required include directory(ies) for a new nanoFramework overlay component that you are adding
# following the template below. 
#
# list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/<path-here>)
###############################################################################################################################


####################################################################################################
# WHEN ADDING A NEW CHIBIOS OVERLAY component add the source file(s) specific to this series below 
####################################################################################################
# component STM32_FLASH
list(APPEND ChibiOSnfOverlay_SOURCES ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/STM32/LLD/FLASHv2/flash_lld.c)
# component STM32_CRC
list(APPEND ChibiOSnfOverlay_SOURCES ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/STM32/LLD/CRCv1/crc_lld.c)
# component STM32_RNG
list(APPEND ChibiOSnfOverlay_SOURCES ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/STM32/LLD/RNGv1/rng_lld.c)
# component STM32_FSMC (Flexible Memory Controller)
list(APPEND ChibiOSnfOverlay_SOURCES ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/STM32/LLD/FSMCv1/fsmc_nand_lld.c)
list(APPEND ChibiOSnfOverlay_SOURCES ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/STM32/LLD/FSMCv1/fsmc_sdram_lld.c)
list(APPEND ChibiOSnfOverlay_SOURCES ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/STM32/LLD/FSMCv1/fsmc_sram_lld.c)
# component STM32_ONEWIRE
list(APPEND ChibiOSnfOverlay_SOURCES ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/STM32/LLD/ONEWIREv1/onewire_lld.c)
# component STM32_QSPI (Quad-SPI)
list(APPEND ChibiOSnfOverlay_SOURCES ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/STM32/LLD/QSPIv1/qspi_lld.c)

##########################################################################################################################
# Add above ALL the source code file(s) low level driver specif for a series required for a new nanoFramework 
# overlay component that you are adding following the template below. 
#
# list(APPEND CHIBIOS_SOURCES ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/src/<path-here>)
##########################################################################################################################
