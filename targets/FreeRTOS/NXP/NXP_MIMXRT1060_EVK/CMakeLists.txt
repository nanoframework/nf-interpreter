#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(binutils.common)
include(binutils.FreeRTOS)

nf_setup_target_build(
    HAS_NANOBOOTER

    BOOTER_LINKER_FILE 
        MIMXRT10xx

    CLR_LINKER_FILE 
        MIMXRT10xx

    BOOTER_EXTRA_COMPILE_DEFINITIONS
        -DCPU_MIMXRT1062CVL5A -DCPU_MIMXRT1062CVL5A_cm7
    
    CLR_EXTRA_COMPILE_DEFINITIONS
        -DCPU_MIMXRT1062CVL5A -DCPU_MIMXRT1062CVL5A_cm7

    BOOTER_EXTRA_LINKMAP_PROPERTIES
        ",--library-path=${CMAKE_SOURCE_DIR}/targets/TI_SimpleLink/common,--defsym=__crt_heap_size__=0x0"

    CLR_EXTRA_LINKMAP_PROPERTIES
        ",--library-path=${CMAKE_SOURCE_DIR}/targets/TI_SimpleLink/common,--defsym=__crt_heap_size__=0x0"
)

target_compile_definitions(NF_Network PUBLIC -DCPU_MIMXRT1062CVL5A -DCPU_MIMXRT1062CVL5A_cm7)
target_compile_definitions(NF_NativeAssemblies PUBLIC -DCPU_MIMXRT1062CVL5A -DCPU_MIMXRT1062CVL5A_cm7)

if(SRECORD_TOOL_AVAILABLE)

    nf_generate_hex_package(
        ${CMAKE_SOURCE_DIR}/build/${NANOBOOTER_PROJECT_NAME}.hex
        ${CMAKE_SOURCE_DIR}/build/${NANOCLR_PROJECT_NAME}.hex
        ${CMAKE_SOURCE_DIR}/build/nanobooter-nanoclr.hex)

endif()
