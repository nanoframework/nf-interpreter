#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# append FatFS files
if(USE_FILESYSTEM_OPTION)
    list(APPEND NANOCLR_PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/fatfs/diskio.c")
    list(APPEND NANOCLR_PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/fatfs/fsl_sd_disk.c")
    list(APPEND NANOCLR_PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/fatfs/ffsystem.c")

    list(APPEND NANOCLR_PROJECT_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/fatfs)

endif()

# append networking files, if enabled
if(USE_NETWORKING_OPTION)
    list(APPEND NANOCLR_PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/LwIP/ethernetif.c")

    list(APPEND NANOCLR_PROJECT_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/LwIP)

    # append mbed TLS entropy generator, if hardware has it
    # if(NF_SECURITY_MBEDTLS AND USE_RNG)
    #     list(APPEND NANOCLR_PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/mbedtls_entropy_hardware_pool.c")
    # endif()

endif()

# make vars global
set(NANOCLR_PROJECT_SOURCES ${NANOCLR_PROJECT_SOURCES} CACHE INTERNAL "make global")
set(NANOCLR_PROJECT_INCLUDE_DIRS ${NANOCLR_PROJECT_INCLUDE_DIRS} CACHE INTERNAL "make global")

# add platform dirs
add_subdirectory(include)
add_subdirectory(common)
add_subdirectory(nanoCLR)
