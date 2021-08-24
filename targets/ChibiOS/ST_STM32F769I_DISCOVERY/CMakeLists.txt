#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(binutils.common)
include(binutils.ChibiOS)

nf_setup_target_build(
    HAS_NANOBOOTER

    BOOTER_LINKER_FILE 
        STM32F76xx_booter

    CLR_LINKER_FILE 
        STM32F76xx_CLR

    CLR_EXTRA_SOURCE_FILES
        # the next one is required is the target implements and it's using external memory
        ${CMAKE_CURRENT_SOURCE_DIR}/target_external_memory.c
    
    BOOTER_EXTRA_LINKMAP_PROPERTIES
        ",--library-path=${CMAKE_SOURCE_DIR}/targets/ChibiOS/_common,--defsym=__main_stack_size__=0x400,--defsym=__process_stack_size__=0x800,--defsym=__crt_heap_size__=0x2000"

    CLR_EXTRA_LINKMAP_PROPERTIES
        ",--library-path=${CMAKE_SOURCE_DIR}/targets/ChibiOS/_common,--defsym=__main_stack_size__=0x400,--defsym=__process_stack_size__=0x800,--defsym=__crt_heap_size__=0x3B000"
)

# generate bin file for deployment
if(SRECORD_TOOL_AVAILABLE)

    ############################################################################################################
    ## when changing the linker file make sure to update the addresses below with the offset of the CLR image ##
    ## DO NOT use the leading 0x notation, just the address in plain hexadecimal formating                    ##
    ############################################################################################################

    if(CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
        nf_generate_bin_package(
            ${CMAKE_SOURCE_DIR}/build/${NANOBOOTER_PROJECT_NAME}.bin
            ${CMAKE_SOURCE_DIR}/build/${NANOCLR_PROJECT_NAME}.bin
            10000
            ${CMAKE_SOURCE_DIR}/build/nanobooter-nanoclr.bin)
    else()
        nf_generate_bin_package(
            ${CMAKE_SOURCE_DIR}/build/${NANOBOOTER_PROJECT_NAME}.bin
            ${CMAKE_SOURCE_DIR}/build/${NANOCLR_PROJECT_NAME}.bin
            10000
            ${CMAKE_SOURCE_DIR}/build/nanobooter-nanoclr.bin)
    endif()

endif()
