#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
include(binutils.common)


# check if FATFS_SOURCE was specified or if it's empty (default is empty)
set(NO_FATFS_SOURCE TRUE)
if(FATFS_SOURCE)
    if(NOT "${FATFS_SOURCE}" STREQUAL "")   
        set(NO_FATFS_SOURCE FALSE)
    endif()
endif()

# FatFS version
set(FATFS_VERSION_EMPTY TRUE)

# check if build was requested with a specifc FatFS version
if(DEFINED FATFS_VERSION)
    if(NOT "${FATFS_VERSION}" STREQUAL "")
        set(FATFS_VERSION_EMPTY FALSE)
    endif()
endif()

# check if build was requested with a specifc FatFS version
if(FATFS_VERSION_EMPTY)
    # no FatFS version actualy specified, must be empty which is fine, we'll default to a known good version
    set(FATFS_VERSION_TAG "R0.14b")
else()
    # set version 
    set(FATFS_VERSION_TAG "${FATFS_VERSION}")
endif()

if(NO_FATFS_SOURCE)
    message(STATUS "FatFS ${FATFS_VERSION_TAG} from GitHub repo")
    
    FetchContent_Declare(
        fatfs
        GIT_REPOSITORY https://github.com/abbrev/fatfs.git
        GIT_TAG ${FATFS_VERSION_TAG}
    )

else()
    # FatFS source was specified

    message(STATUS "FatFS ${FATFS_VERSION_TAG} source from: ${FATFS_SOURCE})")

    FetchContent_Declare(
        fatfs
        SOURCE_DIR ${FATFS_SOURCE}
    )

endif()

FetchContent_GetProperties(fatfs)
if(NOT fatfs_POPULATED)
    # Fetch the content using previously declared details
    FetchContent_Populate(fatfs)
endif()

# have to delete the ffconf.h template from the repo
execute_process(
    COMMAND ${CMAKE_COMMAND} -E remove ${fatfs_SOURCE_DIR}/source/ffconf.h
)

# check if LWIP_SOURCE was specified or if it's empty (default is empty)
set(NO_LWIP_SOURCE TRUE)
if(LWIP_SOURCE)
    if(NOT "${LWIP_SOURCE}" STREQUAL "")
        set(NO_LWIP_SOURCE FALSE)
    endif()
endif()

# LWIP version
set(LWIP_VERSION_EMPTY TRUE)

# check if build was requested with a specifc LWIP version
if(DEFINED LWIP_VERSION)
    if(NOT "${LWIP_VERSION}" STREQUAL "")
        set(LWIP_VERSION_EMPTY FALSE)
    endif()
endif()

# check if build was requested with a specifc LWIP version
if(LWIP_VERSION_EMPTY)
    # no LWIP version actualy specified, must be empty which is fine, we'll default to a known good version
    set(LWIP_GIT_TAG "STABLE-2_0_3_RELEASE")
else()
    # set Git tag
    set(LWIP_GIT_TAG "${LWIP_VERSION}")
endif()            

if(NO_LWIP_SOURCE)

    message(STATUS "lwIP ${LWIP_GIT_TAG} from savannah.nongnu.org repo")

    FetchContent_Declare(
        lwip
        GIT_REPOSITORY https://git.savannah.nongnu.org/git/lwip.git
        GIT_TAG ${LWIP_GIT_TAG}
    )

else()
    # LWIP source was specified

    message(STATUS "lwIP ${LWIP_GIT_TAG} (source from: ${LWIP_SOURCE})")

    FetchContent_Declare(
        lwip
        SOURCE_DIR ${LWIP_SOURCE}
    )

endif()

FetchContent_GetProperties(lwip)
if(NOT lwip_POPULATED)
    # Fetch the content using previously declared details
    FetchContent_Populate(lwip)
endif()

# append FatFS files
if(USE_FILESYSTEM_OPTION)
    list(APPEND NANOCLR_PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/_fatfs/diskio.c")
    list(APPEND NANOCLR_PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/_fatfs/fsl_sd_disk.c")
    list(APPEND NANOCLR_PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/_fatfs/ffsystem.c")

    list(APPEND NANOCLR_PROJECT_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/_fatfs)

endif()

# append networking files, if enabled
if(USE_NETWORKING_OPTION)
    list(APPEND NANOCLR_PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/_LwIP/ethernetif.c")

    list(APPEND NANOCLR_PROJECT_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/_LwIP)

    # append mbed TLS entropy generator, if hardware has it
    # if(NF_SECURITY_MBEDTLS AND USE_RNG)
    #     list(APPEND NANOCLR_PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/mbedtls_entropy_hardware_pool.c")
    # endif()

endif()

# make vars global
set(NANOCLR_PROJECT_SOURCES ${NANOCLR_PROJECT_SOURCES} CACHE INTERNAL "make global")
set(NANOCLR_PROJECT_INCLUDE_DIRS ${NANOCLR_PROJECT_INCLUDE_DIRS} CACHE INTERNAL "make global")

list(APPEND TARGET_FREERTOS_COMMON_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)

# add platform dirs
add_subdirectory(_include)
add_subdirectory(_common)
add_subdirectory(_nanoBooter)
add_subdirectory(_nanoCLR)
