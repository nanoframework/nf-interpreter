#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
include(binutils.common)
include(binutils.AzureRTOS)
include(AZURERTOS_${TARGET_SERIES}_GCC_options)

# Azure RTOS settings and inclusion of build system
set(THREADX_ARCH "cortex_m4")
set(THREADX_TOOLCHAIN "gnu")
set(MCPU_FLAGS "-mthumb -mcpu=cortex-m4")
set(VFP_FLAGS "")
set(SPEC_FLAGS "--specs=nosys.specs")

add_subdirectory(${azure_rtos_SOURCE_DIR} AzureSource)

nf_setup_target_build(
    HAS_NANOBOOTER

    BOOTER_LINKER_FILE 
        max78000_booter

    CLR_LINKER_FILE 
        max78000_CLR
    
    BOOTER_EXTRA_LINKMAP_PROPERTIES
        ",--library-path=${CMAKE_SOURCE_DIR}/targets/AzureRTOS/_common,--defsym=__main_stack_size__=0x400,--defsym=__crt_heap_size__=0x2000"

    CLR_EXTRA_LINKMAP_PROPERTIES
        ",--library-path=${CMAKE_SOURCE_DIR}/targets/AzureRTOS/_common,--defsym=__main_stack_size__=0x400,--defsym=__crt_heap_size__=0x2000"
)
