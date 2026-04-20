#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# CMake module to fetch and locate the georgerobotics/cyw43-driver sources.
# Used for Pico W (and other CYW43-based) WiFi targets on ChibiOS.

include(FetchContent)
FetchContent_GetProperties(cyw43_driver)

# CYW43 driver sources (high-level driver + lwIP glue + firmware blobs)
set(CYW43_DRIVER_SRCS
    cyw43_ll.c
    cyw43_ctrl.c
    cyw43_stats.c
)
# Note: cyw43_spi.c is intentionally excluded — our cyw43_bus_pio_spi.c
# provides all SPI protocol functions (half-duplex PIO, not full-duplex).
# Note: cyw43_lwip.c is intentionally excluded — nf_lwipthread_wifi.c provides
# the lwIP integration with nanoFramework-specific network interface management.

set(CYW43_DRIVER_INCLUDE_DIRS "")
set(CYW43_DRIVER_SOURCES "")

# Include directories
list(APPEND CYW43_DRIVER_INCLUDE_DIRS ${cyw43_driver_SOURCE_DIR}/src)
list(APPEND CYW43_DRIVER_INCLUDE_DIRS ${cyw43_driver_SOURCE_DIR}/firmware)
list(APPEND CYW43_DRIVER_INCLUDE_DIRS ${cyw43_driver_SOURCE_DIR})

# Platform-specific ChibiOS WiFi adapter includes
list(APPEND CYW43_DRIVER_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_WiFi/cyw43)
list(APPEND CYW43_DRIVER_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_WiFi)

# Find source files
foreach(SRC_FILE ${CYW43_DRIVER_SRCS})

    set(CYW43_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(CYW43_SRC_FILE ${SRC_FILE}
        PATHS
            ${cyw43_driver_SOURCE_DIR}/src
        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if(BUILD_VERBOSE)
        message("${SRC_FILE} >> ${CYW43_SRC_FILE}")
    endif()

    list(APPEND CYW43_DRIVER_SOURCES ${CYW43_SRC_FILE})

endforeach()

# Add the ChibiOS PIO SPI bus driver (our implementation)
list(APPEND CYW43_DRIVER_SOURCES ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_WiFi/cyw43/cyw43_bus_pio_spi.c)
list(APPEND CYW43_DRIVER_SOURCES ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_WiFi/cyw43/cyw43_arch_chibios.c)

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(CYW43_Driver DEFAULT_MSG CYW43_DRIVER_INCLUDE_DIRS CYW43_DRIVER_SOURCES)
