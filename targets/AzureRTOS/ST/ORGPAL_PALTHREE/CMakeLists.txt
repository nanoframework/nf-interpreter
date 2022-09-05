#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
include(binutils.common)
include(binutils.AzureRTOS)
include(AzureRTOS_${TARGET_SERIES}_GCC_options)

# # sanity checks
# if(NOT USBX_FEATURE_CDC)
#     message(FATAL_ERROR "\n\nTarget requires USBX CDC. Make sure to enable USBX inclusion setting USBX_FEATURE_CDC build option to ON.\n\n")
# endif()

# Azure RTOS settings and inclusion of build system
set(THREADX_ARCH "cortex_m7")
set(THREADX_TOOLCHAIN "gnu")
# set(UX_USER_FILE ${TARGET_BASE_LOCATION}/target_ux_user.h CACHE STRING "Enable UX user configuration")
set(NX_USER_FILE ${TARGET_BASE_LOCATION}/target_nx_user.h CACHE STRING "Enable NX user configuration")
set(NXD_ENABLE_FILE_SERVERS OFF CACHE BOOL "Disable fileX dependency by netxduo")
# set(FX_USER_FILE ${TARGET_BASE_LOCATION}/target_fx_user.h CACHE STRING "Enable FX user configuration")


set(STM32_DRIVER_TARGET_DEVICE STM32F769xx CACHE BOOL "Set STM32 target for HAL Driver")

add_subdirectory(${azure_rtos_SOURCE_DIR} threadx)
# add_subdirectory(${azure_rtos_filex_SOURCE_DIR} FileXSource)
add_subdirectory(${azure_rtos_netxduo_SOURCE_DIR} netxduo)
# add_subdirectory(${azure_rtos_usbx_SOURCE_DIR} USBXSource)

nf_setup_target_build(
    HAS_NANOBOOTER

    BOOTER_LINKER_FILE 
        STM32F76xx_booter

    CLR_LINKER_FILE 
        STM32F76xx_CLR

    CLR_EXTRA_SOURCE_FILES
        # the next one is required is the target implements and it's using external memory
        ${CMAKE_CURRENT_SOURCE_DIR}/target_external_memory.c
    
    BOOTER_EXTRA_COMPILE_DEFINITIONS
        USBH_DEBUG_MULTI_HOST=0

    CLR_EXTRA_COMPILE_DEFINITIONS
        USBH_DEBUG_MULTI_HOST=0

    BOOTER_EXTRA_LINKMAP_PROPERTIES
        ",--library-path=${CMAKE_SOURCE_DIR}/targets/AzureRTOS/ST/_common,--defsym=__main_stack_size__=0x800,--defsym=__process_stack_size__=0x400,--defsym=__crt_heap_size__=0x2000"

    CLR_EXTRA_LINKMAP_PROPERTIES
        ",--library-path=${CMAKE_SOURCE_DIR}/targets/AzureRTOS/ST/_common,--defsym=__main_stack_size__=0x800,--defsym=__process_stack_size__=0x400,--defsym=__crt_heap_size__=0x3B000"
)

# if HEX2DFU tool is available pack the binaries into a DFU package
if(HEX2DFU_TOOL_AVAILABLE)

    ####################################################################################################
    ## when changing the linker file make sure to update the new addresses for the image files below  ##
    ## DO NOT use the leading 0x notation, just the address in plain hexadecimal formating            ##
    ####################################################################################################

    if(CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
        nf_generate_dfu_package(
            ${CMAKE_SOURCE_DIR}/build/${NANOBOOTER_PROJECT_NAME}.bin 08000000 
            ${CMAKE_SOURCE_DIR}/build/${NANOCLR_PROJECT_NAME}.bin 08010000 
            ${CMAKE_SOURCE_DIR}/build/nanobooter-nanoclr.dfu
        )
    else()
        nf_generate_dfu_package(
            ${CMAKE_SOURCE_DIR}/build/${NANOBOOTER_PROJECT_NAME}.bin 08000000
            ${CMAKE_SOURCE_DIR}/build/${NANOCLR_PROJECT_NAME}.bin 08010000 
            ${CMAKE_SOURCE_DIR}/build/nanobooter-nanoclr.dfu       
        )
    endif()

endif()

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
