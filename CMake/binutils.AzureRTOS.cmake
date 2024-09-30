#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(binutils.common)
# include(ChibiOS_HAL)

function(nf_set_optimization_options target) 

    target_compile_options(${target} PRIVATE
        $<$<CONFIG:Debug>:-Og -ggdb>
        $<$<CONFIG:Release>:-O3 -flto>
        $<$<CONFIG:MinSizeRel>:-Os -flto>
        $<$<CONFIG:RelWithDebInfo>:-Os -femit-class-debug-always -ggdb>
    )

endfunction()

function(nf_set_linker_file target linker_file_name)

    # set linker file name
    set_target_properties(${target} PROPERTIES LINK_FLAGS "-T${linker_file_name}")

endfunction()

# sets network connectivity options according to the NetX driver choosen in build options WIFI_DRIVER and ETHERNET_DRIVER
macro(nf_set_network_connectivity_options)

    ############################
    if(DEFINED WIFI_DRIVER)

        # list of supported Wi-Fi drivers
        set(WIFI_DRIVER_SUPPORTED_OPTIONS "ISM43362")
        # try to find the driver in the list
        list(FIND WIFI_DRIVER_SUPPORTED_OPTIONS ${WIFI_DRIVER} WIFI_DRIVER_NAME_INDEX)

        if(WIFI_DRIVER_NAME_INDEX EQUAL -1)
            # driver is NOT supported 
            message(FATAL_ERROR "\n\nSorry but '${WIFI_DRIVER}' provided in WIFI_DRIVER build option is not supported at this time...\nYou can wait for it to be added, or you might want to contribute by working on a PR for it.\n\n")
        elseif(WIFI_DRIVER_NAME_INDEX EQUAL 0)
            set(TARGET_HAS_WIFI_SUPPORT TRUE BOOL)
        else()
            message(FATAL_ERROR "invalid index for WIFI_DRIVER_NAME_INDEX")
        endif()

    endif()

    ################################
    if(DEFINED ETHERNET_DRIVER)

        # list of supported Ethernet drivers
        set(ETHERNET_DRIVER_SUPPORTED_OPTIONS "LAN8742")
        # try to find the driver in the list
        list(FIND ETHERNET_DRIVER_SUPPORTED_OPTIONS ${ETHERNET_DRIVER} ETHERNET_DRIVER_NAME_INDEX)

        if(ETHERNET_DRIVER_NAME_INDEX EQUAL -1)
            # driver is NOT supported 
            message(FATAL_ERROR "\n\nSorry but '${ETHERNET_DRIVER}' provided in ETHERNET_DRIVER build option is not supported at this time...\nYou can wait for it to be added, or you might want to contribute by working on a PR for it.\n\n")
        elseif(ETHERNET_DRIVER_NAME_INDEX EQUAL 0)
            set(TARGET_HAS_ETHERNET_SUPPORT TRUE BOOL)
        else()
            message(FATAL_ERROR "invalid index for ETHERNET_DRIVER_NAME_INDEX")
        endif()

    endif()

endmacro()

# setting compile definitions for a target based on general build options
# TARGET parameter to set the target that's setting them for
# optional EXTRA_COMPILE_DEFINITIONS with compiler definitions to be added to the library
# optional BUILD_TARGET when target it's a library pass here the name ot the target that's building for (either nanoBooter or nanoCLR)
macro(nf_set_compile_definitions)

    # parse arguments
    cmake_parse_arguments(NFSCD "" "TARGET" "EXTRA_COMPILE_DEFINITIONS;BUILD_TARGET" ${ARGN})

    if(NOT NFSCD_TARGET OR "${NFSCD_TARGET}" STREQUAL "")
        message(FATAL_ERROR "Need to set TARGET argument when calling nf_set_compile_definitions()")
    endif()

    # definition for platform 
    if(CHIBIOS_HAL_REQUIRED)
        # ChibiOS HAL community always include (nanoFramework overlay and official community contributions optionally)
        target_compile_definitions(${NFSCD_TARGET} PUBLIC -DHAL_USE_COMMUNITY)
    endif()

    nf_common_compiler_definitions(TARGET ${NFSCD_TARGET} BUILD_TARGET ${NFSCD_BUILD_TARGET})

    # include extra compiler definitions
    target_compile_definitions(${NFSCD_TARGET} PUBLIC ${NFSCD_EXTRA_COMPILE_DEFINITIONS})

endmacro()

function(nf_set_stm32_target_series)
    # process target series, which is in the format "STM32F4xx"
    string(REPLACE "STM32" "" TARGET_SERIES_SHORT_1 "${TARGET_SERIES}")
    string(REPLACE "xx" "" TARGET_SERIES_SHORT "${TARGET_SERIES_SHORT_1}")
    
    # store the series short name for later use
    set(TARGET_SERIES_SHORT ${TARGET_SERIES_SHORT} CACHE INTERNAL "STM32 target series short name")

endfunction()

# Add packages that are common to Azure RTOS platform builds
# To be called from target CMakeList.txt
# optional TARGET argument with target name
macro(nf_add_platform_packages)

    # parse arguments
    cmake_parse_arguments(NFAPP "" "TARGET" "" ${ARGN})

    # if(USE_FILESYSTEM_OPTION)
    #     find_package(CHIBIOS_FATFS REQUIRED QUIET)
    # endif()
   
    if(CHIBIOS_HAL_REQUIRED)
        find_package(ChibiOS_${TARGET_SERIES_SHORT}_HAL REQUIRED QUIET)
        find_package(ChibiOSnfOverlay REQUIRED QUIET)
    endif()

    if(MAXIM_MICROS_SDK_REQUIRED)
        find_package(MaximMicrosSDK REQUIRED QUIET)
    endif()    

    if(STM32_CUBE_PACKAGE_REQUIRED)
        find_package(${TARGET_STM32_CUBE_PACKAGE}_CubePackage REQUIRED QUIET)
    endif()

    if(SILABS_GECKO_SDK_REQUIRED)
        find_package(Gecko_SDK REQUIRED QUIET)
    endif()
 
    # packages specific for nanoBooter
    if(NFAPP_TARGET STREQUAL ${NANOBOOTER_PROJECT_NAME})
        # no packages for booter
    endif()

    # packages specific for nanoCRL
    if(NFAPP_TARGET STREQUAL ${NANOCLR_PROJECT_NAME})

        if(USE_NETWORKING_OPTION)
            find_package(NF_Network REQUIRED QUIET)
        endif()

        if(AZURERTOS_NETXDUO_REQUIRED)

            FetchContent_GetProperties(azure_rtos)

            # need to add ThreadX extension in order to use BSD
            if("${TARGET_SERIES}" STREQUAL "STM32F7xx")
                set(TX_PORT_FILE ${azure_rtos_SOURCE_DIR}/ports/cortex_m7/gnu/inc/tx_port.h)
            elseif("${TARGET_SERIES}" STREQUAL "STM32L4xx"
                    OR "${TARGET_SERIES}" STREQUAL "EFM32GG11")
                set(TX_PORT_FILE ${azure_rtos_SOURCE_DIR}/ports/cortex_m4/gnu/inc/tx_port.h)
            else()
                message(FATAL_ERROR "Support for NetX Duo is not implemented for ${TARGET_SERIES}.")
            endif()

            # need to read the supplied TX_Port file and add TX_THREAD_EXTENSION_3
            file(READ
                "${TX_PORT_FILE}"
                TX_PORT_FILE_CONTENTS)

            string(REPLACE
                "define TX_THREAD_EXTENSION_3"
                "define  TX_THREAD_EXTENSION_3 int bsd_errno;"
                TX_PORT_FILE_FINAL_CONTENTS
                "${TX_PORT_FILE_CONTENTS}")

            file(WRITE 
                ${TX_PORT_FILE} 
                "${TX_PORT_FILE_FINAL_CONTENTS}")

        endif()

    endif()

endmacro()

# Add Azure RTOS platform dependencies to a specific CMake target
# To be called from target CMakeList.txt
macro(nf_add_platform_dependencies target)

    nf_add_common_dependencies(${target})

    # specific to nanoCRL
    if(${target} STREQUAL ${NANOCLR_PROJECT_NAME})
       
        nf_add_lib_coreclr(
            EXTRA_INCLUDES
                ${TARGET_AZURERTOS_COMMON_INCLUDE_DIRS}
                ${NF_Network_INCLUDE_DIRS}
                ${CHIBIOS_CONTRIB_INCLUDE_DIRS}
                ${CHIBIOS_HAL_INCLUDE_DIRS}
                ${Gecko_SDK_INCLUDE_DIRS}
                ${azure_rtos_SOURCE_DIR}/common/inc
                ${AZRTOS_INCLUDES}
        )
                        
        # add_dependencies(NF_CoreCLR azrtos::threadx)
        add_dependencies(${target}.elf nano::NF_CoreCLR)


        if(AZURERTOS_NETXDUO_REQUIRED)
            FetchContent_GetProperties(azure_rtos_netxduo)
            get_target_property(NETXDUO_INCLUDES azrtos::netxduo INCLUDE_DIRECTORIES)
        endif()

        nf_add_lib_wireprotocol(
            EXTRA_INCLUDES
                ${TARGET_AZURERTOS_COMMON_INCLUDE_DIRS}
                ${CHIBIOS_CONTRIB_INCLUDE_DIRS}
                ${CHIBIOS_HAL_INCLUDE_DIRS}
                ${Gecko_SDK_INCLUDE_DIRS}
                ${azure_rtos_SOURCE_DIR}/common/inc
                ${AZRTOS_INCLUDES}
        )

        add_dependencies(${target}.elf nano::WireProtocol)

        if(NF_FEATURE_DEBUGGER)

            nf_add_lib_debugger(
                EXTRA_INCLUDES
                    ${TARGET_AZURERTOS_COMMON_INCLUDE_DIRS}
                    ${CHIBIOS_CONTRIB_INCLUDE_DIRS}
                    ${CHIBIOS_HAL_INCLUDE_DIRS}
                    ${Gecko_SDK_INCLUDE_DIRS}
                    ${azure_rtos_SOURCE_DIR}/common/inc
                    ${AZRTOS_INCLUDES}
            )

            add_dependencies(${target}.elf nano::NF_Debugger)

        endif()

        nf_add_lib_native_assemblies(
            EXTRA_INCLUDES
                ${TARGET_AZURERTOS_COMMON_INCLUDE_DIRS}
                ${ChibiOSnfOverlay_INCLUDE_DIRS}
                ${CHIBIOS_CONTRIB_INCLUDE_DIRS}
                ${CHIBIOS_HAL_INCLUDE_DIRS}
                ${Gecko_SDK_INCLUDE_DIRS}
                ${azure_rtos_SOURCE_DIR}/common/inc
                ${NETXDUO_INCLUDES}
                ${TARGET_BASE_LOCATION}
                ${${TARGET_STM32_CUBE_PACKAGE}_CubePackage_INCLUDE_DIRS}
                ${AZRTOS_INCLUDES}
                
            EXTRA_COMPILE_DEFINITIONS 
                -DNX_INCLUDE_USER_DEFINE_FILE            
        )
        
        add_dependencies(${target}.elf nano::NF_NativeAssemblies)

        if(USE_NETWORKING_OPTION)

            nf_add_lib_network(
                BUILD_TARGET
                    ${target}

                EXTRA_INCLUDES 
                    ${AZRTOS_INCLUDES}
                    ${NETXDUO_INCLUDES}
                    ${TARGET_AZURERTOS_COMMON_INCLUDE_DIRS}
                    ${CHIBIOS_HAL_INCLUDE_DIRS}
                    ${${TARGET_STM32_CUBE_PACKAGE}_CubePackage_INCLUDE_DIRS}

                EXTRA_COMPILE_DEFINITIONS 
                    -DNX_INCLUDE_USER_DEFINE_FILE
                    -DUSE_HAL_DRIVER
                    -D${STM32_DRIVER_TARGET_DEVICE}

            )
        
            add_dependencies(${target}.elf nano::NF_Network)

        endif()

    endif()

endmacro()

# Add Azure RTOS platform include directories to a specific CMake target
# To be called from target CMakeList.txt
macro(nf_add_platform_include_directories target)

    target_include_directories(${target}.elf PUBLIC
        ${TARGET_AZURERTOS_COMMON_INCLUDE_DIRS}
    )

    if(STM32_CUBE_PACKAGE_REQUIRED)

        FetchContent_GetProperties(stm32${TARGET_SERIES_SHORT_LOWER}_hal_driver_${target})
        get_target_property(STM32_CUBE_PACKAGE_INCLUDE_DIRECTORIES stm32${TARGET_SERIES_SHORT_LOWER}_hal_driver_${target} INCLUDE_DIRECTORIES)

        target_include_directories(${target}.elf PUBLIC
            ${AZRTOS_INCLUDES}
            ${STM32_CUBE_PACKAGE_INCLUDE_DIRECTORIES}
        )

    endif()

    if(CHIBIOS_HAL_REQUIRED)
        target_include_directories(${target}.elf PUBLIC
            ${CHIBIOS_HAL_INCLUDE_DIRS}
            ${ChibiOSnfOverlay_INCLUDE_DIRS}
        )
    endif()

    if(CHIBIOS_CONTRIB_REQUIRED)
        target_include_directories(${target}.elf PUBLIC
            ${CHIBIOS_CONTRIB_INCLUDE_DIRS}
        )
    endif()
    
    if(MAXIM_MICROS_SDK_REQUIRED)
        target_include_directories(${target}.elf PUBLIC
            ${MaximMicrosSDK_INCLUDE_DIRS}
        )
    endif()
    
    if(SILABS_GECKO_SDK_REQUIRED)
        target_include_directories(${target}.elf PUBLIC
            ${Gecko_SDK_INCLUDE_DIRS}
        )
    endif()
  
    # includes specific to nanoBooter
    if(${target} STREQUAL ${NANOBOOTER_PROJECT_NAME})

        target_include_directories(${target}.elf PUBLIC
            ${TARGET_AZURERTOS_NANOBOOTER_INCLUDE_DIRS}
        )

    endif()

    # includes specific to nanoCRL
    if(${target} STREQUAL ${NANOCLR_PROJECT_NAME})

        target_include_directories(${target}.elf PUBLIC
            ${TARGET_AZURERTOS_NANOCLR_INCLUDE_DIRS}
        )

    endif()

endmacro()

# Add Azure RTOS platform target sources to a specific CMake target
# To be called from target CMakeList.txt
macro(nf_add_platform_sources target)

    # add header files with common OS definitions and board definitions
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/target_common.h.in
                   ${CMAKE_BINARY_DIR}/targets/${RTOS}/${TARGET_VENDOR}/${TARGET_BOARD}/target_common.h @ONLY)

    # sources common to both builds
    target_sources(${target}.elf PUBLIC
        ${TARGET_AZURERTOS_COMMON_SOURCES}
    )

    FetchContent_GetProperties(azure_rtos)
    get_target_property(AZRTOS_INCLUDES azrtos::threadx INCLUDE_DIRECTORIES)

    if(CHIBIOS_HAL_REQUIRED)
        target_sources(${target}.elf PUBLIC
            ${CHIBIOS_HAL_SOURCES}
            ${ChibiOSnfOverlay_SOURCES}
        )
    endif()

    if(MAXIM_MICROS_SDK_REQUIRED)
        target_sources(${target}.elf PUBLIC
            ${MaximMicrosSDK_SOURCES}
        )
    endif()

    # sources specific to nanoBooter
    if(${target} STREQUAL ${NANOBOOTER_PROJECT_NAME})

        # add header file for board definition
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/nanoBooter/target_board.h.in
                       ${CMAKE_BINARY_DIR}/targets/${RTOS}/${TARGET_VENDOR}/${TARGET_BOARD}/nanoBooter/target_board.h @ONLY)

        target_sources(${target}.elf PUBLIC
            
            ${TARGET_AZURERTOS_NANOBOOTER_SOURCES}
            ${CMAKE_SOURCE_DIR}/src/PAL/BlockStorage/nanoPAL_BlockStorage.c
           
            # need to add configuration manager to allow get/store configuration blocks
            ${CMAKE_SOURCE_DIR}/src/HAL/nanoHAL_ConfigurationManager_stubs.c
        )

    endif()

    # sources specific to nanoCRL
    if(${target} STREQUAL ${NANOCLR_PROJECT_NAME})

        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/target_board.h.in
                       ${CMAKE_BINARY_DIR}/targets/${RTOS}/${TARGET_VENDOR}/${TARGET_BOARD}/nanoCLR/target_board.h @ONLY)

        target_sources(${target}.elf PUBLIC
            ${TARGET_AZURERTOS_NANOCLR_SOURCES}
        )

        if(USE_NETWORKING_OPTION)         
            target_link_libraries(${target}.elf
                nano::NF_Network
                azrtos::netxduo
            )
        endif()

        if(USBX_FEATURE_HID_OPTION)
            target_link_libraries(${target}.elf
                azrtos::netxduo
                azrtos::filex
                azrtos::usbx
            )
        endif()

    endif()

    if(STM32_CUBE_PACKAGE_REQUIRED)

        nf_add_stm32_cube(
            BUILD_TARGET
                ${target}

            EXTRA_COMPILE_DEFINITIONS
                -DUSE_HAL_DRIVER
                -D${STM32_DRIVER_TARGET_DEVICE}
        )

        target_link_libraries(${target}.elf
            nano::stm32${TARGET_SERIES_SHORT_LOWER}_hal_driver_${target}
        )

    endif()

    if(SILABS_GECKO_SDK_REQUIRED)
        
        nf_add_gecko_sdk(
            BUILD_TARGET
                ${target}
            EXTRA_INCLUDES
                ${AZRTOS_INCLUDES}
                ${NF_CoreCLR_INCLUDE_DIRS}
        )
                        
        add_dependencies(${target}.elf nano::gecko_sdk_${target})

        target_link_libraries(${target}.elf
            -Wl,-whole-archive
            nano::gecko_sdk_${target}
            -Wl,-no-whole-archive
        )

    endif()

    target_link_libraries(${target}.elf
        azrtos::threadx
    )

endmacro()

# macro to setup the build for a target
# mandatory HAS_NANOBOOTER specifing if the target implements nanoBooter
# BOOTER_LINKER_FILE with the path to the linker file for nanoBooter (if the target has it)
# mandatory CLR_LINKER_FILE with the path to the linker file for nanoCLR
# optional BOOTER_EXTRA_SOURCE_FILES with paths to extra files to be added to the nanoBooter build target
# optional CLR_EXTRA_SOURCE_FILES with paths to extra files to be added to the nanoCLR build target
# optional BOOTER_EXTRA_COMPILE_DEFINITIONS extra nanoBooter compile definitions to pass to nf_set_compile_definitions() 
# optional CLR_EXTRA_COMPILE_DEFINITIONS extra nanoCLR compile definitions to pass to nf_set_compile_definitions() 
# optional BOOTER_EXTRA_LINKMAP_PROPERTIES extra nanoBooter link map properties to pass to nf_set_link_map() 
# optional CLR_EXTRA_LINKMAP_PROPERTIES extra nanoCLR link map properties to pass to nf_set_link_map() 
macro(nf_setup_target_build)

    # OK to pass ARGN, to have it perform it's parsings and validation 
    nf_setup_target_build_common(${ARGN})
endmacro()

# macro to clear binary files related with nanoBooter from output
# to make sure that the build file it's up to date
macro(nf_clear_output_files_nanobooter)
    nf_clear_common_output_files_nanobooter()
    # other files specific to this platform should go here
endmacro()

# macro to clear binary files related with nanoCLR from output
# to make sure that the build file it's up to date
macro(nf_clear_output_files_nanoclr)
    nf_clear_common_output_files_nanoclr()
    # other files specific to this platform should go here
endmacro()
