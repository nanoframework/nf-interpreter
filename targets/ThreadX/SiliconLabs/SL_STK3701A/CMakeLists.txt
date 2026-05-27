
include(FetchContent)
include(binutils.common)
include(binutils.ThreadX)
include(ThreadX_${TARGET_SERIES}_GCC_options)

# ThreadX settings and inclusion of build system
set(THREADX_ARCH "cortex_m4" )
set(THREADX_TOOLCHAIN "gnu" )
# set(NX_USER_FILE ${TARGET_BASE_LOCATION}/target_nx_user.h CACHE STRING "Enable NX user configuration")
# set(NXD_ENABLE_FILE_SERVERS OFF CACHE BOOL "Disable fileX dependency by netxduo")

set(SL_BOARD_NAME "BRD2204A" PARENT_SCOPE)
# include BSP from Gecko SDK
set(GECKO_SDK_BSP TRUE CACHE BOOL "Setting Gecko BSP flag ")

add_subdirectory(${threadx_SOURCE_DIR} threadx)
# add_subdirectory(${threadx_filex_SOURCE_DIR} filex)
# add_subdirectory(${threadx_netxduo_SOURCE_DIR} netxduo)
# add_subdirectory(${threadx_usbx_SOURCE_DIR} usbx)

nf_setup_target_build(
    HAS_NANOBOOTER

    BOOTER_LINKER_FILE 
        efm32gg11b_booter

    CLR_LINKER_FILE 
        efm32gg11b_CLR

    BOOTER_EXTRA_COMPILE_DEFINITIONS
        EFM32GG11B820F2048GL192=1
        SL_BOARD_NAME=\"BRD2204C\"
        SL_BOARD_REV=\"A02\"
        SL_COMPONENT_CATALOG_PRESENT=1
        _SILICON_LABS_32B_SERIES_2_CONFIG=0
        I2CSPM_TRANSFER_TIMEOUT=3000
        SL_STACK_SIZE=0x2000
        SL_HEAP_SIZE=0x2000

    CLR_EXTRA_COMPILE_DEFINITIONS
        EFM32GG11B820F2048GL192=1
        SL_BOARD_NAME=\"BRD2204C\"
        SL_BOARD_REV=\"A02\"
        SL_COMPONENT_CATALOG_PRESENT=1
        _SILICON_LABS_32B_SERIES_2_CONFIG=0
        I2CSPM_TRANSFER_TIMEOUT=3000
        SL_STACK_SIZE=0x7000
        SL_HEAP_SIZE=0x10000

    BOOTER_EXTRA_LINKMAP_PROPERTIES
        ",--library-path=${CMAKE_SOURCE_DIR}/targets/ThreadX/_common"

    CLR_EXTRA_LINKMAP_PROPERTIES
        ",--library-path=${CMAKE_SOURCE_DIR}/targets/ThreadX/_common"
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
            13000
            ${CMAKE_SOURCE_DIR}/build/nanobooter-nanoclr.bin)
    else()
        nf_generate_bin_package(
            ${CMAKE_SOURCE_DIR}/build/${NANOBOOTER_PROJECT_NAME}.bin
            ${CMAKE_SOURCE_DIR}/build/${NANOCLR_PROJECT_NAME}.bin
            0C000
            ${CMAKE_SOURCE_DIR}/build/nanobooter-nanoclr.bin)
    endif()

endif()
