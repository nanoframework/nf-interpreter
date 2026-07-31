#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(binutils.common)
include(binutils.ChibiOS)

# this board has an onboard Inventek ISM43362 (ES-WIFI) module, driven over SPI - it doesn't
# expose a raw MAC/link-layer interface, so it uses a dedicated socket-proxy networking
# implementation (see targets/ChibiOS/_WiFi/inventek and CMake/Modules/FindNF_Network.cmake)
# instead of the generic lwIP one
if(TARGET_HAS_WIFI)
    set(TARGET_HAS_WIFI_ISM43362 TRUE)

    # needs to be visible to every compilation unit in this board (not just the NF_Network
    # library), since e.g. the System.Device.Wifi native implementation also branches on it
    add_compile_definitions(TARGET_HAS_WIFI_ISM43362)

    # ditto for the ES-WIFI driver headers (wifi.h), needed by the System.Device.Wifi native
    # implementation in addition to the NF_Network library
    include_directories(
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_WiFi
        ${CMAKE_SOURCE_DIR}/targets/ChibiOS/_WiFi/inventek)
endif()

nf_setup_target_build(
    HAS_NANOBOOTER

    BOOTER_LINKER_FILE
        STM32L475xG_booter

    CLR_LINKER_FILE
        STM32L475xG_CLR

    BOOTER_EXTRA_LINKMAP_PROPERTIES
        ",--library-path=${CMAKE_SOURCE_DIR}/targets/ChibiOS/_common,--defsym=__main_stack_size__=0x200,--defsym=__process_stack_size__=0x400,--defsym=__crt_heap_size__=0x4000"

    CLR_EXTRA_LINKMAP_PROPERTIES
        ",--library-path=${CMAKE_SOURCE_DIR}/targets/ChibiOS/_common,--defsym=__main_stack_size__=0x200,--defsym=__process_stack_size__=0x400,--defsym=__crt_heap_size__=0x6000"
)

# generate bin file for deployment
if(SRECORD_TOOL_AVAILABLE)

    ############################################################################################################
    ## when changing the linker file make sure to update the addresses below with the offset of the CLR image ##
    ## DO NOT use the leading 0x notation, just the address in plain hexadecimal formating                    ##
    ############################################################################################################

    if(CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
        nf_generate_bin_package(
            ${CMAKE_BINARY_DIR}/${NANOBOOTER_PROJECT_NAME}.bin
            ${CMAKE_BINARY_DIR}/${NANOCLR_PROJECT_NAME}.bin
            8000
            ${CMAKE_BINARY_DIR}/nanobooter-nanoclr.bin)
    else()
        nf_generate_bin_package(
            ${CMAKE_BINARY_DIR}/${NANOBOOTER_PROJECT_NAME}.bin
            ${CMAKE_BINARY_DIR}/${NANOCLR_PROJECT_NAME}.bin
            8000
            ${CMAKE_BINARY_DIR}/nanobooter-nanoclr.bin)
    endif()

endif()
