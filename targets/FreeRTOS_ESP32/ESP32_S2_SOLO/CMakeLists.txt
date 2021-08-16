#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(binutils.common)
include(binutils.ESP32)

# add packages
NF_ADD_COMMON_PACKAGES()
NF_ADD_PLATFORM_PACKAGES()

#######################################

add_subdirectory("common")
add_subdirectory("nanoCLR")

#######################
# nanoCLR executable

add_executable(
    
    # executables for project, project sources
    ${NANOCLR_PROJECT_NAME}.elf
)

NF_ADD_PLATFORM_DEPENDENCIES(${NANOCLR_PROJECT_NAME})

NF_ADD_COMMON_SOURCES(${NANOCLR_PROJECT_NAME})
NF_ADD_PLATFORM_SOURCES(${NANOCLR_PROJECT_NAME})

# include directories for nanoCLR
NF_ADD_COMMON_INCLUDE_DIRECTORIES(${NANOCLR_PROJECT_NAME})
NF_ADD_PLATFORM_INCLUDE_DIRECTORIES(${NANOCLR_PROJECT_NAME})

# set compiler options
nf_set_compiler_options(${NANOCLR_PROJECT_NAME}.elf)

# mbed TLS requires a config file
if(USE_SECURITY_MBEDTLS_OPTION)
    # this seems to be only option to properly set a compiler define through the command line that needs to be a string literal
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMBEDTLS_CONFIG_FILE=\"<${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/mbedTLS/nf_mbedtls_config.h>\"")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMBEDTLS_CONFIG_FILE=\"<${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/mbedTLS/nf_mbedtls_config.h>\"")
endif()

# set compiler definitions
nf_set_compiler_definitions(${NANOCLR_PROJECT_NAME}.elf -DUSBH_DEBUG_MULTI_HOST=0)

# set linker files
if(CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
    nf_set_linker_file(${NANOCLR_PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/STM32F76xx_CLR-DEBUG.ld)
else()
    nf_set_linker_file(${NANOCLR_PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/STM32F76xx_CLR.ld)
endif()

# set linker options
nf_set_linker_options(${NANOCLR_PROJECT_NAME}.elf)

# generate output files
# nf_generate_build_output_files(${NANOCLR_PROJECT_NAME}.elf)
