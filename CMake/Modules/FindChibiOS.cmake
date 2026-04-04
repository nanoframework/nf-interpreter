#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
FetchContent_GetProperties(chibios)

###################################################################################################################################
# WHEN ADDING A NEW SERIES, add the respective name to the list below along with the CMake files with GCC options and source files
###################################################################################################################################

# check if the series name is supported

# Supported series by vendor
set(CHIBIOS_STM_SUPPORTED_SERIES "STM32F0xx" "STM32F4xx" "STM32F7xx" "STM32H7xx" "STM32L4xx" CACHE INTERNAL "supported STM series names for ChibiOS")
set(CHIBIOS_RP_SUPPORTED_SERIES "RP2040" "RP2350" CACHE INTERNAL "supported RP series names for ChibiOS")

# Check STM32 series
list(FIND CHIBIOS_STM_SUPPORTED_SERIES ${TARGET_SERIES} TARGET_STM_INDEX)
# Check RP series
list(FIND CHIBIOS_RP_SUPPORTED_SERIES ${TARGET_SERIES} TARGET_RP_INDEX)

if(NOT TARGET_STM_INDEX EQUAL -1)
    # series is supported by ST
    set(TARGET_VENDOR "ST" CACHE INTERNAL "target vendor is ST")
    set(TARGET_VENDOR_FAMILY "STM32" CACHE INTERNAL "target vendor family")
elseif(NOT TARGET_RP_INDEX EQUAL -1)
    # series is supported by Raspberry Pi
    set(TARGET_VENDOR "RP" CACHE INTERNAL "target vendor is Raspberry Pi")
    set(TARGET_VENDOR_FAMILY "RP" CACHE INTERNAL "target vendor family")
else()
    message(FATAL_ERROR "\n\nSorry but the ${TARGET_SERIES} is not supported at this time...\nYou can wait for it to be added, or you might want to contribute by working on a PR for it.\n\n")
endif()

# store the package name for STM32 targets (only relevant for ST vendor)
if(TARGET_VENDOR STREQUAL "ST")
    set(TARGET_STM32_SERIES STM32${TARGET_SERIES_SHORT} CACHE INTERNAL "name for STM32 Cube package")
endif()

# including here the CMake files for the source files specific to the target series
include(CHIBIOS_${TARGET_SERIES}_sources)
# and here the GCC options tuned for the target series 
include(CHIBIOS_${TARGET_SERIES}_GCC_options)

if (BUILD_VERBOSE)
    message("ChibiOS board series is ${TARGET_SERIES}")
endif()

# set include directories for ChibiOS

# common include directories (vendor-agnostic)
list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os) 
list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/license)
list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/common/ARMCMx)
list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/include)
list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/osal/rt-nil)
list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/oslib/include)
list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/rt/include)
list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/common/abstractions/cmsis_os)
list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/common/startup/ARMCMx/compilers/GCC)

# vendor-specific include directories
if(TARGET_VENDOR STREQUAL "ST")
    list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/STM32/${TARGET_SERIES})
    list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/common/ports/ARMv7-M)
    list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/common/ports/ARMv7-M/compilers/GCC)
    list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/common/ext/CMSIS/include)
    list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/common/ext/CMSIS/ST/${TARGET_SERIES})
    list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/common/startup/ARMCMx/devices/${TARGET_SERIES})
    list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/targets/ChibiOS/ST/${TARGET_BOARD})
elseif(TARGET_VENDOR STREQUAL "RP")
    # >>> To use nanoFramework overlay EFL driver, uncomment the next line and comment the one after <<<
    # list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_nf-overlay/os/hal/ports/RP/LLD/EFLv1)
    list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/RP/LLD/EFLv1)
    list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/hal/ports/RP/${TARGET_SERIES})
    list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/common/portability/GCC)
    list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/common/ports/ARM-common)
    list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/common/ports/ARM-common/include)
    list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/common/ports/ARMv6-M)
    list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/common/ports/ARMv6-M/compilers/GCC)
    list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/common/ports/ARMv6-M/smp/rp2)
    list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/common/startup/ARMCMx/devices/${TARGET_SERIES})
    list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/common/ext/ARM/CMSIS/Core/Include)
    list(APPEND CHIBIOS_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/common/ext/RP/${TARGET_SERIES})
endif()

# common build output include directories
list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/targets/ChibiOS/${TARGET_BOARD}/nanoBooter)
list(APPEND CHIBIOS_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/targets/ChibiOS/${TARGET_BOARD}/nanoCLR)

# source files and GCC options according to target vendor and series

# source files for ChibiOS
set(CHIBIOS_SRCS

    # RT
    chsys.c
    chrfcu.c
    chdebug.c
    chtrace.c
    chvt.c
    chschd.c
    chinstances.c
    chthreads.c
    chtm.c
    chstats.c
    chregistry.c
    chsem.c
    chmtx.c
    chcond.c
    chevents.c
    chmsg.c
    chdynamic.c

    chmboxes.c
    chmemcore.c
    chmemchecks.c
    chmemheaps.c
    chmempools.c
    chpipes.c
    chobjcaches.c
    chdelegates.c
    chfactory.c

    # required to use malloc and other newlib stuff
    syscalls.c

    # CMSIS
    cmsis_os.c

)

foreach(SRC_FILE ${CHIBIOS_SRCS})

    set(CHIBIOS_SRC_FILE SRC_FILE -NOTFOUND)

    find_file(CHIBIOS_SRC_FILE ${SRC_FILE}
        PATHS 
            ${chibios_SOURCE_DIR}/os/hal/src
            ${chibios_SOURCE_DIR}/os/hal/osal/rt-nil
            ${chibios_SOURCE_DIR}/os/rt/src
            ${chibios_SOURCE_DIR}/os/oslib/src
            ${chibios_SOURCE_DIR}/os/common/abstractions/cmsis_os
            ${chibios_SOURCE_DIR}/os/various
            ${chibios_SOURCE_DIR}/os/various/newlib_bindings

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${CHIBIOS_SRC_FILE}")
    endif()

    list(APPEND CHIBIOS_SOURCES ${CHIBIOS_SRC_FILE})

endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(ChibiOS DEFAULT_MSG CHIBIOS_INCLUDE_DIRS CHIBIOS_SOURCES)
