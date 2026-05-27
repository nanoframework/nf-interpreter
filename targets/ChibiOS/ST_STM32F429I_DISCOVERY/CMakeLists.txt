#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(binutils.common)
include(binutils.ChibiOS)

nf_setup_target_build(
    HAS_NANOBOOTER

    BOOTER_LINKER_FILE 
        STM32F429xI_booter

    CLR_LINKER_FILE 
        STM32F429xI_CLR

    CLR_EXTRA_SOURCE_FILES
        # the next one is required is the target implements and it's using external memory
        ${CMAKE_CURRENT_SOURCE_DIR}/target_external_memory.c
    
    BOOTER_EXTRA_LINKMAP_PROPERTIES
        ",--library-path=${CMAKE_SOURCE_DIR}/targets/ChibiOS/_common,--defsym=__main_stack_size__=0x400,--defsym=__process_stack_size__=0x400,--defsym=__crt_heap_size__=0x10000"

    CLR_EXTRA_LINKMAP_PROPERTIES
        ",--library-path=${CMAKE_SOURCE_DIR}/targets/ChibiOS/_common,--defsym=__main_stack_size__=0x400,--defsym=__process_stack_size__=0x800,--defsym=__crt_heap_size__=0x10000"
)
