
include(FetchContent)
include(binutils.common)
include(binutils.AzureRTOS)
include(AzureRTOS_${TARGET_SERIES}_GCC_options)

# Azure RTOS settings and inclusion of build system
set(THREADX_ARCH "cortex_m4" )
set(THREADX_TOOLCHAIN "gnu" )
set(TX_USER_FILE ${TARGET_BASE_LOCATION}/target_tx_user.h CACHE STRING "Enable TX user configuration")
# set(NX_USER_FILE ${TARGET_BASE_LOCATION}/target_nx_user.h CACHE STRING "Enable NX user configuration")
# set(NXD_ENABLE_FILE_SERVERS OFF CACHE BOOL "Disable fileX dependency by netxduo")

# set(STM32_DRIVER_TARGET_DEVICE STM32L475xx CACHE BOOL "Set STM32 target for HAL Driver")

set(SL_BOARD_NAME "BRD2204A" PARENT_SCOPE)

add_subdirectory(${azure_rtos_SOURCE_DIR} threadx)
# add_subdirectory(${azure_rtos_netxduo_SOURCE_DIR} netxduo)

nf_setup_target_build(
    HAS_NANOBOOTER

    BOOTER_LINKER_FILE 
        efm32gg11b_booter

    CLR_LINKER_FILE 
        efm32gg11b_CLR

    BOOTER_EXTRA_COMPILE_DEFINITIONS
        EFM32GG11B820F2048GL192=1
        SL_BOARD_NAME=\"BRD2204A\"
        SL_BOARD_REV=\"B07\"
        SL_COMPONENT_CATALOG_PRESENT=1
        _SILICON_LABS_32B_SERIES_2_CONFIG=0
        __START=main
        __STARTUP_CLEAR_BSS=1
        SL_STACK_SIZE=0x9000
        SL_HEAP_SIZE=0x9000

    CLR_EXTRA_COMPILE_DEFINITIONS
        EFM32GG11B820F2048GL192=1
        SL_BOARD_NAME=\"BRD2204A\"
        SL_BOARD_REV=\"B07\"
        SL_COMPONENT_CATALOG_PRESENT=1
        _SILICON_LABS_32B_SERIES_2_CONFIG=0
        SL_STACK_SIZE=0x4000
        SL_HEAP_SIZE=0x4000

    BOOTER_EXTRA_LINKMAP_PROPERTIES
        ",--library-path=${CMAKE_SOURCE_DIR}/targets/AzureRTOS/_common"

    CLR_EXTRA_LINKMAP_PROPERTIES
        ",--library-path=${CMAKE_SOURCE_DIR}/targets/AzureRTOS/_common"
)

# make vars global
set(TARGET_AZURERTOS_COMMON_SOURCES ${TARGET_AZURERTOS_COMMON_SOURCES} CACHE INTERNAL "make global")
