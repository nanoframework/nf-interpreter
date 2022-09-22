#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
include(binutils.common)
include(binutils.AzureRTOS)
include(AzureRTOS_${TARGET_SERIES}_GCC_options)

# Azure RTOS settings and inclusion of build system
set(THREADX_ARCH "cortex_m4")
set(THREADX_TOOLCHAIN "gnu")
set(NX_USER_FILE ${TARGET_BASE_LOCATION}/target_nx_user.h CACHE STRING "Enable NX user configuration")
set(NXD_ENABLE_FILE_SERVERS OFF CACHE BOOL "Disable fileX dependency by netxduo")

set(STM32_DRIVER_TARGET_DEVICE STM32L475xx CACHE BOOL "Set STM32 target for HAL Driver")

add_subdirectory(${azure_rtos_SOURCE_DIR} threadx)
add_subdirectory(${azure_rtos_netxduo_SOURCE_DIR} netxduo)

nf_setup_target_build(
    HAS_NANOBOOTER

    BOOTER_LINKER_FILE 
        STM32L475xG_booter

    CLR_LINKER_FILE 
        STM32L475xG_CLR

    BOOTER_EXTRA_LINKMAP_PROPERTIES
        ",--library-path=${CMAKE_SOURCE_DIR}/targets/AzureRTOS/ST/_common,--defsym=__main_stack_size__=0x200,--defsym=__process_stack_size__=0x0,--defsym=__crt_heap_size__=0x4000"

    CLR_EXTRA_LINKMAP_PROPERTIES
        ",--library-path=${CMAKE_SOURCE_DIR}/targets/AzureRTOS/ST/_common,--defsym=__main_stack_size__=0x200,--defsym=__process_stack_size__=0x0,--defsym=__crt_heap_size__=0x2000"
)

# make vars global
set(TARGET_AZURERTOS_COMMON_SOURCES ${TARGET_AZURERTOS_COMMON_SOURCES} CACHE INTERNAL "make global")
