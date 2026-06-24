#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)

# check if MCUBOOT_SOURCE was specified or if it's empty (default is empty)
set(NO_MCUBOOT_SOURCE TRUE)

if(MCUBOOT_SOURCE)
    if(NOT ${MCUBOOT_SOURCE} STREQUAL "")
        set(NO_MCUBOOT_SOURCE FALSE)
    endif()
endif()

# set tag for currently supported version
# WHEN CHANGING THIS MAKE SURE TO UPDATE THE DEV CONTAINERS
set(MCUBOOT_GIT_TAG "v2.4.0")

if(NO_MCUBOOT_SOURCE)
    message(STATUS "MCUboot ${MCUBOOT_GIT_TAG} from GitHub repo")

    FetchContent_Declare(
        mcuboot
        GIT_REPOSITORY https://github.com/mcu-tools/mcuboot.git
        GIT_TAG        ${MCUBOOT_GIT_TAG}
        GIT_SHALLOW    ON
    )

else()
    message(STATUS "MCUboot ${MCUBOOT_GIT_TAG} (source from: ${MCUBOOT_SOURCE})")

    FetchContent_Declare(
        mcuboot
        SOURCE_DIR ${MCUBOOT_SOURCE}
    )

endif()

FetchContent_MakeAvailable(mcuboot)

# MCUboot upstream include paths required by the porting layer.
# These are the standard MCUboot header locations within the upstream tree.
list(APPEND MCUboot_INCLUDE_DIRS
    ${mcuboot_SOURCE_DIR}/boot/bootutil/include
    ${mcuboot_SOURCE_DIR}/ext/mbedtls-asn1/include
    ${CMAKE_SOURCE_DIR}/MCUboot/include
    ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_common/mcuboot
)

# Convention: each MCUboot-enabled board provides
#   <board>/MCUboot/mcuboot_flash_map_boot.c
# implementing the flash_area_* porting layer (flash_map_backend/flash_map_backend.h).
# The same file is compiled into the standalone bootloader and into the nanoCLR-side
# nf_mcuboot_port library; bootloader-only code is gated behind NF_MCUBOOT_BOOTLOADER
set(MCUboot_SOURCES ${TARGET_BASE_LOCATION}/MCUboot/mcuboot_flash_map_boot.c)

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(MCUboot DEFAULT_MSG MCUboot_INCLUDE_DIRS MCUboot_SOURCES)
