#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(binutils.common)

# process ESP32 Ethernet options
macro(nf_process_esp32_ethernet_options)

    # need to process this?
    if(ESP32_ETHERNET_SUPPORT)

        if(NOT ESP32_ETHERNET_INTERFACE)
            # default to LAN8720
            set(ESP32_ETHERNET_INTERFACE "LAN8720" CACHE INTERNAL "Defaulting LAN8720")

            message(STATUS "\n\n*** No Ethernet interface defined. Defaulting to LAN8720. ***\n\n")
        endif()

        # list of supported PHYs
        set(ESP32_SUPPORTED_PHY "LAN8720" "IP101" "RTL8201" "DP83848" "KSZ8041" CACHE INTERNAL "supported ESP32 PHYs")
        # list of supported ETH SPI PHYs
        # ENJ28J60 currently not supported, driver in IDF examples (TODO)
        set(ESP32_SUPPORTED_ETH_SPI "W5500" "DM9051" "ENJ28J60" CACHE INTERNAL "supported ESP32 ETH SPIs")

        list(FIND ESP32_SUPPORTED_PHY ${ESP32_ETHERNET_INTERFACE} ESP32_PHY_INDEX)

        if(ESP32_PHY_INDEX EQUAL -1)

            # can't find this under supported PHYs
        
            # try with SPIs
            list(FIND ESP32_SUPPORTED_ETH_SPI ${ESP32_ETHERNET_INTERFACE} ESP32_ETH_SPI_INDEX)
            
            if(ESP32_ETH_SPI_INDEX EQUAL -1)
                # can't find it under SPIs either
                message(FATAL_ERROR "\n\nSomething wrong happening: can't find support for Ethernet interface ${ESP32_ETHERNET_INTERFACE}!\n\n")
            else()
                # store SPI option
                set(ESP32_ETHERNET_SPI_OPTION TRUE CACHE INTERNAL "Set ESP32_ETHERNET_SPI option")
                set(ESP32_ETHERNET_INTERNAL_OPTION FALSE CACHE INTERNAL "Set ESP32_ETHERNET_INTERNAL option")
                # set define with SPI module
                set(ESP32_ETHERNET_DEFINES -DESP32_ETHERNET_SPI_MODULE_${ESP32_ETHERNET_INTERFACE} CACHE INTERNAL "define for Ethernet SPI module option")
            endif()

        else()
            # store PHY option
            set(ESP32_ETHERNET_INTERNAL_OPTION TRUE CACHE INTERNAL "Set ESP32_ETHERNET_INTERNAL option")
            set(ESP32_ETHERNET_SPI_OPTION FALSE CACHE INTERNAL "Set ESP32_ETHERNET_SPI option")
            # set define with PHY name
            set(ESP32_ETHERNET_DEFINES -DESP32_ETHERNET_PHY_${ESP32_ETHERNET_INTERFACE} CACHE INTERNAL "define for Ethernet PHY interface option")
        endif()

    endif()

endmacro()

# find a set of files on a list of possible locations
macro(nf_find_esp32_files_at_location files locations)

    foreach(SRC_FILE ${files})

        set(IDF_SRC_FILE SRC_FILE -NOTFOUND)

        find_file(IDF_SRC_FILE ${SRC_FILE}
            PATHS 

            ${locations}

            CMAKE_FIND_ROOT_PATH_BOTH
        )

        if (BUILD_VERBOSE)
            message("${SRC_FILE} >> ${IDF_SRC_FILE}")
        endif()

        list(APPEND ESP32_IDF_SOURCES ${IDF_SRC_FILE})

    endforeach()

endmacro()

function(nf_set_optimization_options target) 

    # debug compile options: -Og (optimize for debugging) and -g (produce generic debug symbols)
    target_compile_options(${target} PRIVATE
        $<$<CONFIG:Debug>:-Og -g>
        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:MinSizeRel>:-Os>
        $<$<CONFIG:RelWithDebInfo>:-Os -g>
    )

endfunction()


function(nf_set_linker_file target linker_file_name)


    # # set linker file name
    # set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "-T${LINKER_FILE_NAME}")

endfunction()

# fixes the ESP32_C3 rom linker script for rom_temp_to_power symbol
# this is required if the CPU it's a revision <= 2 
macro(nf_fix_esp32c3_rom_file)

    if((${TARGET_SERIES_SHORT} STREQUAL "esp32c3"))
        # build is for esp32c3

        if(${ESP32_REVISION} LESS_EQUAL 2)
            # need to UNcomment the rom_temp_to_power symbol
            file(READ
                ${esp32_idf_SOURCE_DIR}/components/esp_rom/esp32c3/ld/esp32c3.rom.ld
                ESP32_C3_ROM_LD_CONTENT)
        
            string(REPLACE
                    "/* rom_temp_to_power = 0x40001ab4; */"
                    "rom_temp_to_power = 0x40001ab4;"
                    ESP32_C3_ROM_LD_NEW_CONTENT
                    "${ESP32_C3_ROM_LD_CONTENT}")
        
            file(WRITE 
                ${esp32_idf_SOURCE_DIR}/components/esp_rom/esp32c3/ld/esp32c3.rom.ld
                "${ESP32_C3_ROM_LD_NEW_CONTENT}")
        else()
            # need to COMMENT the rom_temp_to_power symbol
            file(READ
                ${esp32_idf_SOURCE_DIR}/components/esp_rom/esp32c3/ld/esp32c3.rom.ld
                ESP32_C3_ROM_LD_CONTENT)

            string(FIND "${ESP32_C3_ROM_LD_CONTENT}" "/* rom_temp_to_power = 0x40001ab4; */" ROM_TEMP_SYMBOL_INDEX)
        
            if(ROM_TEMP_SYMBOL_INDEX EQUAL -1)
             
                string(REPLACE
                        "rom_temp_to_power = 0x40001ab4;"
                        "/* rom_temp_to_power = 0x40001ab4; */"
                        ESP32_C3_ROM_LD_NEW_CONTENT
                        "${ESP32_C3_ROM_LD_CONTENT}")
            
                file(WRITE 
                    ${esp32_idf_SOURCE_DIR}/components/esp_rom/esp32c3/ld/esp32c3.rom.ld
                    "${ESP32_C3_ROM_LD_NEW_CONTENT}")
            endif()

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
    nf_common_compiler_definitions(TARGET ${NFSCD_TARGET} BUILD_TARGET ${NFSCD_BUILD_TARGET})

    # include extra compiler definitions
    target_compile_definitions(${NFSCD_TARGET} PUBLIC ${NFSCD_EXTRA_COMPILE_DEFINITIONS})

endmacro()

function(nf_set_esp32_target_series)

    # process target series name to set it to lower case
    string(TOLOWER "${TARGET_SERIES}" TARGET_SERIES_1)
   
    # need to remove the underscore from the series name
    string(REPLACE "_" "" TARGET_SERIES_2 "${TARGET_SERIES_1}")

    # store the series name for later use
    set(TARGET_SERIES_SHORT ${TARGET_SERIES_2} CACHE INTERNAL "ESP32 target series lower case, short version")

    # set the CPU type
    if(${TARGET_SERIES_SHORT} STREQUAL "esp32c3" OR ${TARGET_SERIES_SHORT} STREQUAL "esp32h2")
        set(ESP32_CPU_TYPE "riscv" CACHE INTERNAL "Setting CPU type")
    else()
        set(ESP32_CPU_TYPE "xtensa" CACHE INTERNAL "Setting CPU type")
    endif()

endfunction()

# Add packages that are common to ESP32 platform builds
# To be called from target CMakeList.txt
# optional TARGET argument with target name
macro(nf_add_platform_packages)

    # parse arguments
    cmake_parse_arguments(NFAPP "" "TARGET" "" ${ARGN})

    find_package(ESP32_IDF REQUIRED QUIET)

    # packages specific for nanoCLR
    if("${NFAPP_TARGET}" STREQUAL "${NANOCLR_PROJECT_NAME}")

        if(USE_NETWORKING_OPTION)

            find_package(NF_Network REQUIRED QUIET)

        endif()

    endif()
    
endmacro()

# Add ESP32  platform dependencies to a specific CMake target
# To be called from target CMakeList.txt
macro(nf_add_platform_dependencies target)

    nf_add_common_dependencies(${target})

    nf_add_lib_coreclr(
        EXTRA_INCLUDES
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_CURRENT_BINARY_DIR}/${target}
            ${CMAKE_CURRENT_SOURCE_DIR}/${target}
            ${CMAKE_CURRENT_SOURCE_DIR}/Include
            ${CMAKE_CURRENT_SOURCE_DIR}/Network
            ${CMAKE_BINARY_DIR}/targets/${RTOS}
            ${ESP32_IDF_INCLUDE_DIRS}
            ${TARGET_ESP32_IDF_INCLUDES})

    add_dependencies(${target}.elf nano::NF_CoreCLR)

    nf_add_lib_wireprotocol(
        EXTRA_INCLUDES
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_BINARY_DIR}/targets/${RTOS}
            ${ESP32_IDF_INCLUDE_DIRS}
            ${TARGET_ESP32_IDF_INCLUDES})

    add_dependencies(${target}.elf nano::WireProtocol)

    
    if(NF_FEATURE_DEBUGGER)

        nf_add_lib_debugger(
            EXTRA_INCLUDES
                ${CMAKE_CURRENT_SOURCE_DIR}
                ${CMAKE_BINARY_DIR}/targets/${RTOS}
                ${ESP32_IDF_INCLUDE_DIRS}
                ${TARGET_ESP32_IDF_INCLUDES})

        add_dependencies(${target}.elf nano::NF_Debugger)

    endif()

    nf_add_lib_native_assemblies(
        EXTRA_INCLUDES
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_BINARY_DIR}/targets/${RTOS}
            ${ESP32_IDF_INCLUDE_DIRS}
            ${TARGET_ESP32_IDF_INCLUDES})
    
    add_dependencies(${target}.elf nano::NF_NativeAssemblies)
  
    if(USE_NETWORKING_OPTION)

        nf_add_lib_network(
            BUILD_TARGET
                ${target}
            EXTRA_COMPILE_DEFINITIONS
                ${ESP32_ETHERNET_DEFINES}
            EXTRA_SOURCES 
                ${TARGET_ESP32_IDF_NETWORK_SOURCES}
            EXTRA_INCLUDES 
                ${ESP32_IDF_INCLUDE_DIRS}
                ${TARGET_ESP32_IDF_INCLUDES}
                ${CMAKE_BINARY_DIR}/targets/${RTOS}
                ${esp32_idf_SOURCE_DIR}/components/mbedtls/mbedtls/include
        )

        add_dependencies(${target}.elf nano::NF_Network)

        # security provider is MbedTLS
        if(USE_SECURITY_MBEDTLS_OPTION)
            add_dependencies(NF_Network mbedtls)
        endif()
    endif()

    # if(USE_FILESYSTEM_OPTION)
    #     find_package(CHIBIOS_FATFS REQUIRED)
    # endif()

    # # SPIFFS
    # if(NF_FEATURE_USE_SPIFFS)
    #     find_package(STM32F7_CubePackage REQUIRED)
    #     find_package(SPIFFS REQUIRED)
    # endif()

endmacro()

# Add ESP32 platform include directories to a specific CMake target
# To be called from target CMakeList.txt
macro(nf_add_platform_include_directories target)

    FetchContent_GetProperties(esp32_idf)

    target_include_directories(${target}.elf PUBLIC

        ${TARGET_ESP32_IDF_COMMON_INCLUDE_DIRS}
        ${CMAKE_BINARY_DIR}/targets/${RTOS}
        ${ESP32_IDF_INCLUDE_DIRS}
        ${NF_NativeAssemblies_INCLUDE_DIRS}
        ${NF_CoreCLR_INCLUDE_DIRS}
    )

    # includes specific to nanoCLR
    if(${target} STREQUAL ${NANOCLR_PROJECT_NAME})

        target_include_directories(${target}.elf PUBLIC

            ${TARGET_ESP32_IDF_NANOCLR_INCLUDE_DIRS}
            ${TARGET_ESP32_IDF_NETWORK_INCLUDE_DIRS}
        )

    endif()

endmacro()

# Add ESP32 platform target sources to a specific CMake target
# To be called from target CMakeList.txt
macro(nf_add_platform_sources target)

    # add header files with common OS definitions and board definitions
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/target_common.h.in
                   ${CMAKE_CURRENT_BINARY_DIR}/target_common.h @ONLY)

    # sources common to both builds
    target_sources(${target}.elf PUBLIC
    
        ${TARGET_ESP32_IDF_COMMON_SOURCES}

    )

    # sources specific to nanoCLR
    if(${target} STREQUAL ${NANOCLR_PROJECT_NAME})
           
        # add header with target platform definitions
        configure_file(${CMAKE_SOURCE_DIR}/CMake/ESP32_target_os.h.in
                       ${CMAKE_BINARY_DIR}/targets/${RTOS}/target_os.h @ONLY)

        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/target_board.h.in
                       ${CMAKE_CURRENT_BINARY_DIR}/nanoCLR/target_board.h @ONLY)

        target_sources(${target}.elf PUBLIC

            ${TARGET_ESP32_IDF_NANOCLR_SOURCES}
            ${ESP32_IDF_SOURCES}
        )

    endif()

    if(USE_SECURITY_MBEDTLS_OPTION)
        target_link_libraries(${target}.elf
        mbedtls
        )

        add_dependencies(NF_Network mbedtls)
    endif()

endmacro()

# macro to setup the build for a target
# mandatory CLR_LINKER_FILE with the path to the linker file for nanoCLR
# optional CLR_EXTRA_SOURCE_FILES with paths to extra files to be added to the nanoCLR build target
# optional CLR_EXTRA_COMPILE_DEFINITIONS extra nanoCLR compile definitions to pass to nf_set_compile_definitions() 
# optional CLR_EXTRA_LINKMAP_PROPERTIES extra nanoCLR link map properties to pass to nf_set_link_map() 
# optional CLR_EXTRA_LINK_FLAGS extra nanoCLR link flags to pass to nf_set_link_options() 
macro(nf_setup_target_build)

    # parse arguments
    cmake_parse_arguments(
        NFSTBC 
        "" 
        "CLR_LINKER_FILE;CLR_EXTRA_LINKMAP_PROPERTIES;CLR_EXTRA_COMPILE_DEFINITIONS;CLR_EXTRA_COMPILE_OPTIONS;CLR_EXTRA_LINK_FLAGS" 
        "CLR_EXTRA_SOURCE_FILES;CLR_EXTRA_LIBRARIES" 
        ${ARGN})

    #######################################
    # now the actual calls for building a target

    # add packages
    nf_add_common_packages()
    nf_add_platform_packages()

    #######################################

    add_subdirectory("common")

    #######################
    # nanoCLR executable has already been created on main CMakeLists
    # just add any extra source files here
    target_sources(
        ${NANOCLR_PROJECT_NAME}.elf PUBLIC

        # extra build files 
        ${NFSTBC_CLR_EXTRA_SOURCE_FILES}
    )

    nf_add_platform_packages(TARGET ${NANOCLR_PROJECT_NAME})
    nf_add_platform_dependencies(${NANOCLR_PROJECT_NAME})
    
    # can't call  nf_add_common_sources() just like with the other platforms
    # so need to replicate the code here  
    target_sources(${NANOCLR_PROJECT_NAME}.elf PUBLIC
    
        ${CMAKE_CURRENT_SOURCE_DIR}/target_common.c
        ${CMAKE_CURRENT_SOURCE_DIR}/target_BlockStorage.c
        ${CMAKE_SOURCE_DIR}/src/PAL/BlockStorage/nanoPAL_BlockStorage.c
        ${COMMON_PROJECT_SOURCES}
        ${NF_HALCore_SOURCES}
    )

    target_link_libraries(${NANOCLR_PROJECT_NAME}.elf
        NF_CoreCLR
        NF_NativeAssemblies
        NF_Debugger
        WireProtocol
        
        ${NFACS_EXTRA_LIBRARIES}
    )

    target_sources(${NANOCLR_PROJECT_NAME}.elf PUBLIC

        ${NANOCLR_PROJECT_SOURCES}

        # sources for nanoFramework libraries
        ${Graphics_Sources}
    )

    nf_add_platform_sources(${NANOCLR_PROJECT_NAME})

    # include directories for nanoCLR
    nf_add_common_include_directories(${NANOCLR_PROJECT_NAME})
    nf_add_platform_include_directories(${NANOCLR_PROJECT_NAME})

    # set compile options
    nf_set_compile_options(TARGET ${NANOCLR_PROJECT_NAME}.elf EXTRA_COMPILE_OPTIONS ${NFSTBC_BOOTER_EXTRA_COMPILE_OPTIONS})

    # set compile definitions
    nf_set_compile_definitions(TARGET ${NANOCLR_PROJECT_NAME}.elf EXTRA_COMPILE_DEFINITIONS ${NFSTBC_CLR_EXTRA_COMPILE_DEFINITIONS} BUILD_TARGET ${NANOCLR_PROJECT_NAME} )

    # set link options
    nf_set_link_options(TARGET ${NANOCLR_PROJECT_NAME}.elf EXTRA_LINK_FLAGS ${NFSTBC_CLR_EXTRA_LINK_FLAGS})

    # this where all the **magic** happens with the IDF build
    idf_build_executable(${NANOCLR_PROJECT_NAME}.elf)

endmacro()

# macro that setups the calls to the partition tool to generate the various partitions
macro(nf_setup_partition_tables_generator)

    # create partition tables for other memory sizes
    set(ESP32_PARTITION_TABLE_UTILITY ${IDF_PATH_CMAKED}/components/partition_table/gen_esp32part.py )

    # create command line for partition table generator
    set(gen_partition_table "python" "${ESP32_PARTITION_TABLE_UTILITY}")

    if(${TARGET_SERIES_SHORT} STREQUAL "esp32" OR ${TARGET_SERIES_SHORT} STREQUAL "esp32c3" OR ${TARGET_SERIES_SHORT} STREQUAL "esp32s2" OR ${TARGET_SERIES_SHORT} STREQUAL "esp32s3")

        add_custom_command( TARGET ${NANOCLR_PROJECT_NAME}.elf POST_BUILD
            COMMAND ${gen_partition_table} 
            --flash-size 4MB 
            ${CMAKE_SOURCE_DIR}/targets/ESP32/_IDF/${TARGET_SERIES_SHORT}/partitions_nanoclr_4mb.csv
            ${CMAKE_BINARY_DIR}/partitions_4mb.bin
            COMMENT "Generate partition table for 4MB flash" )

    endif()

    if(${TARGET_SERIES_SHORT} STREQUAL "esp32" OR ${TARGET_SERIES_SHORT} STREQUAL "esp32s2" OR ${TARGET_SERIES_SHORT} STREQUAL "esp32s3")

        add_custom_command( TARGET ${NANOCLR_PROJECT_NAME}.elf POST_BUILD
            COMMAND ${gen_partition_table} 
            --flash-size 8MB 
            ${CMAKE_SOURCE_DIR}/targets/ESP32/_IDF/${TARGET_SERIES_SHORT}/partitions_nanoclr_8mb.csv
            ${CMAKE_BINARY_DIR}/partitions_8mb.bin
            COMMENT "Generate partition table for 8MB flash" )

        add_custom_command( TARGET ${NANOCLR_PROJECT_NAME}.elf POST_BUILD
            COMMAND ${gen_partition_table} 
            --flash-size 16MB 
            ${CMAKE_SOURCE_DIR}/targets/ESP32/_IDF/${TARGET_SERIES_SHORT}/partitions_nanoclr_16mb.csv
            ${CMAKE_BINARY_DIR}/partitions_16mb.bin
            COMMENT "Generate partition table for 16MB flash" )

    endif()

    if(${TARGET_SERIES_SHORT} STREQUAL "esp32" OR ${TARGET_SERIES_SHORT} STREQUAL "esp32c3" )
        # 2MB partition table for ESP32
       
        add_custom_command( TARGET ${NANOCLR_PROJECT_NAME}.elf POST_BUILD
            COMMAND ${gen_partition_table}  
            --flash-size 2MB 
            ${CMAKE_SOURCE_DIR}/targets/ESP32/_IDF/esp32/partitions_nanoclr_2mb.csv
            ${CMAKE_BINARY_DIR}/partitions_2mb.bin
            COMMENT "Generate partition table for 2MB flash" )
    endif()

endmacro()

# macro to add IDF as a library to the build and add the IDF components according to variant and options
macro(nf_add_idf_as_library)

    include(${IDF_PATH_CMAKED}/tools/cmake/idf.cmake)

    # "fix" the reported version so it doesn't show '-dirty' 
    # this is because we could be deleting some files and tweaking others in the IDF
    get_property(MY_IDF_VER TARGET __idf_build_target PROPERTY IDF_VER)
    string(REPLACE "-dirty" "" MY_IDF_VER_FIXED "${MY_IDF_VER}")
    set_property(TARGET __idf_build_target PROPERTY IDF_VER ${MY_IDF_VER_FIXED})
    set(IDF_VER_FIXED ${MY_IDF_VER_FIXED} CACHE INTERNAL "IDF version as CMake var")

    # for COMPILE DEFINITIONS it's a bit more work
    get_property(IDF_COMPILE_DEFINITIONS TARGET __idf_build_target PROPERTY COMPILE_DEFINITIONS )

    string(REPLACE "-dirty" "" IDF_COMPILE_DEFINITIONS_FIXED "${IDF_COMPILE_DEFINITIONS}")
    set_property(TARGET __idf_build_target PROPERTY COMPILE_DEFINITIONS ${IDF_COMPILE_DEFINITIONS_FIXED})
    
    message(STATUS "Fixed IDF version. Is now: ${MY_IDF_VER_FIXED}")

    # check for SDK config from build options
    if(SDK_CONFIG_FILE)
        # got an SDK config CONFIG on the build options

        # check if the file exists
        if(EXISTS ${SDK_CONFIG_FILE})
            set(SDKCONFIG_DEFAULTS_FILE ${SDK_CONFIG_FILE})
        else()
            # no, try the IDF folder
            if(EXISTS ${CMAKE_SOURCE_DIR}/targets/ESP32/_IDF/${SDK_CONFIG_FILE})
                # found it
                set(SDKCONFIG_DEFAULTS_FILE ${CMAKE_SOURCE_DIR}/targets/ESP32/_IDF/${SDK_CONFIG_FILE})
            else()
                message(FATAL_ERROR "Couldn't find IDF SDK CONFIG file '${SDK_CONFIG_FILE}'. Please check the SDK_CONFIG_FILE build option.")
            endif()
        endif()
    else()
        # get default SDK CONFIG file
        set(SDKCONFIG_DEFAULTS_FILE ${CMAKE_SOURCE_DIR}/targets/ESP32/_IDF/sdkconfig.default)
    endif()

    message(STATUS "\n-- SDK CONFIG is: '${SDKCONFIG_DEFAULTS_FILE}'.")

    # Save original contents to be restored later
    file(READ
    "${SDKCONFIG_DEFAULTS_FILE}"
    SDKCONFIG_ORIGINAL_CONTENTS)

    # set list with the IDF components to add
    # need to match the list below with the respective libraries
    set(IDF_COMPONENTS_TO_ADD
        ${TARGET_SERIES_SHORT}
        freertos
        esptool_py
        spiffs
        fatfs
    )

    # set list with the libraries for IDF components added
    # need to match the list above with the IDF components
    set(IDF_LIBRARIES_TO_ADD
        idf::${TARGET_SERIES_SHORT}
        idf::freertos
        idf::esptool_py
        idf::spiffs
        idf::fatfs
  )

    if(HAL_USE_BLE_OPTION)
        list(APPEND IDF_COMPONENTS_TO_ADD bt)
        list(APPEND IDF_LIBRARIES_TO_ADD idf::bt)
    endif()

    if(ESP32_ETHERNET_SUPPORT)
        list(APPEND IDF_COMPONENTS_TO_ADD esp_eth)
        list(APPEND IDF_LIBRARIES_TO_ADD idf::esp_eth)
    endif()

    # handle specifics for ESP32S2/S3 series
    if(${TARGET_SERIES_SHORT} STREQUAL "esp32s2" OR ${TARGET_SERIES_SHORT} STREQUAL "esp32s3")

        if(ESP32_USB_CDC)

            # add IDF components specific to ESP32S2/S3 series
            list(APPEND IDF_COMPONENTS_TO_ADD tinyusb)
            list(APPEND IDF_LIBRARIES_TO_ADD idf::tinyusb)

            message(STATUS "Support for embedded USB CDC enabled")

            # SDKCONFIG for ESP32S2 has embedded USB CDC enabled as default

        else()
            message(STATUS "Support for embedded USB CDC **IS NOT** enabled")

            # need to read the supplied SDK CONFIG file and replace the appropriate option
            file(READ
                "${SDKCONFIG_DEFAULTS_FILE}"
                SDKCONFIG_DEFAULT_CONTENTS)

            string(REPLACE
                "CONFIG_USB_ENABLED=y"
                "CONFIG_USB_ENABLED=n"
                SDKCONFIG_DEFAULT_FINAL_CONTENTS
                "${SDKCONFIG_DEFAULT_CONTENTS}")

            # need to temporarilly allow changes in source files
            set(CMAKE_DISABLE_SOURCE_CHANGES OFF)

            file(WRITE 
                ${SDKCONFIG_DEFAULTS_FILE} 
                ${SDKCONFIG_DEFAULT_FINAL_CONTENTS})

            set(CMAKE_DISABLE_SOURCE_CHANGES ON)

        endif()

    endif()

    # option for automatic XTAL frequency detection
    # (default is OFF which means that fixed default frequency will be used)
    option(ESP32_XTAL_FREQ_26 "option to set XTAL frequency to 26MHz")
   
    message(DEBUG "ESP32_XTAL_FREQ_26 option is ${ESP32_XTAL_FREQ_26}")

    if(ESP32_XTAL_FREQ_26)

        message(STATUS "Set XTAL frequency to 26MHz")

        # need to read the supplied SDK CONFIG file(s) and replace the appropriate options

        message(DEBUG "Reading SDK config from '${SDKCONFIG_DEFAULTS_FILE}'")
        file(READ
            "${SDKCONFIG_DEFAULTS_FILE}"
            SDKCONFIG_DEFAULT_CONTENTS)

        string(REPLACE
            "CONFIG_ESP32_XTAL_FREQ_40"
            "CONFIG_ESP32_XTAL_FREQ_26"
            SDKCONFIG_DEFAULT_FINAL_CONTENTS
            "${SDKCONFIG_DEFAULT_CONTENTS}")

        # now do the same for the series config file, if it exists
        file(READ
            "${SDKCONFIG_DEFAULTS_FILE}.${TARGET_SERIES_SHORT}"
            SDKCONFIG_DEFAULT_SERIES_CONTENTS)
        
        string(REPLACE
            "CONFIG_ESP32_XTAL_FREQ_40"
            "CONFIG_ESP32_XTAL_FREQ_26"
            SDKCONFIG_DEFAULT_SERIES_FINAL_CONTENTS
            "${SDKCONFIG_DEFAULT_SERIES_CONTENTS}")

        # need to temporarilly allow changes in source files
        set(CMAKE_DISABLE_SOURCE_CHANGES OFF)

        file(WRITE 
            ${SDKCONFIG_DEFAULTS_FILE} 
            ${SDKCONFIG_DEFAULT_FINAL_CONTENTS})

        message(DEBUG "Wrote updated SDK config to '${SDKCONFIG_DEFAULTS_FILE}'")

        file(WRITE 
            "${SDKCONFIG_DEFAULTS_FILE}.${TARGET_SERIES_SHORT}" 
            ${SDKCONFIG_DEFAULT_SERIES_FINAL_CONTENTS})

        message(DEBUG "Wrote updated SDK config to '${SDKCONFIG_DEFAULTS_FILE}.${TARGET_SERIES_SHORT}'")

        set(CMAKE_DISABLE_SOURCE_CHANGES ON)
    else()
        message(STATUS "Using default XTAL frequency")
    endif()

    # create IDF static libraries
    idf_build_process(${TARGET_SERIES_SHORT}
        COMPONENTS 
            ${IDF_COMPONENTS_TO_ADD}

        # SDKCONFIG ${CMAKE_SOURCE_DIR}/targets/ESP32/_IDF/sdkconfig
        SDKCONFIG_DEFAULTS
            ${SDKCONFIG_DEFAULTS_FILE}
        PROJECT_NAME "nanoCLR"
        PROJECT_VER ${BUILD_VERSION}
        PROJECT_DIR ${CMAKE_SOURCE_DIR}
    )

    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

    # add IDF app_main
    add_executable(
        ${NANOCLR_PROJECT_NAME}.elf
        ${CMAKE_SOURCE_DIR}/targets/ESP32/_IDF/${TARGET_SERIES_SHORT}/app_main.c
        ${CMAKE_SOURCE_DIR}/targets/ESP32/_IDF/project_elf_src_${TARGET_SERIES_SHORT}.c
    )

    #Restore original sdkconfig back to defaults
    set(CMAKE_DISABLE_SOURCE_CHANGES OFF)

    file(WRITE 
        ${SDKCONFIG_DEFAULTS_FILE} 
        ${SDKCONFIG_ORIGINAL_CONTENTS})

    set(CMAKE_DISABLE_SOURCE_CHANGES ON)

    if(USE_NETWORKING_OPTION)

        FetchContent_GetProperties(esp32_idf)

        # get list of source files for lwIP
        get_target_property(IDF_LWIP_SOURCES __idf_lwip SOURCES)

        # remove the ones we'll be replacing
        list(REMOVE_ITEM 
            IDF_LWIP_SOURCES
                ${esp32_idf_SOURCE_DIR}/components/lwip/lwip/src/api/api_msg.c
                ${esp32_idf_SOURCE_DIR}/components/lwip/lwip/src/api/sockets.c
                ${esp32_idf_SOURCE_DIR}/components/lwip/port/esp32/freertos/sys_arch.c
        )

        # add our modified sources
        list(APPEND 
            IDF_LWIP_SOURCES
                ${CMAKE_SOURCE_DIR}/targets/ESP32/_lwIP/nf_api_msg.c
                ${CMAKE_SOURCE_DIR}/targets/ESP32/_lwIP/nf_sockets.c
                ${CMAKE_SOURCE_DIR}/targets/ESP32/_lwIP/nf_sys_arch.c
        )

        # replace the source list
        set_property(
            TARGET __idf_lwip 
            PROPERTY SOURCES ${IDF_LWIP_SOURCES}
        )
        
        # get list of include directories for lwIP
        get_target_property(IDF_LWIP_INCLUDE_DIRECTORIES __idf_lwip INCLUDE_DIRECTORIES)

        # add nanoCLR include path to lwIP so our lwipots are taken instead of the IDF ones
        list(INSERT 
            IDF_LWIP_INCLUDE_DIRECTORIES 0
                ${CMAKE_SOURCE_DIR}/targets/ESP32/_include
                ${CMAKE_SOURCE_DIR}/targets/ESP32/${TARGET_BOARD}
                ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/Networking.Sntp
                ${CMAKE_SOURCE_DIR}/src/CLR/Include
                ${CMAKE_SOURCE_DIR}/src/HAL/Include
        )

        # replace the include directories
        set_property(
            TARGET __idf_lwip 
            PROPERTY INCLUDE_DIRECTORIES ${IDF_LWIP_INCLUDE_DIRECTORIES}
        )

        # add nanoCLR compile definitions to lwIP
        list(APPEND 
            IDF_LWIP_COMPILE_DEFINITIONS 
                PLATFORM_ESP32 )

        # add the compile definitions
        set_property(
            TARGET __idf_lwip 
            PROPERTY COMPILE_DEFINITIONS ${IDF_LWIP_COMPILE_DEFINITIONS}
        )

    endif()

    # need to add include path to find our ffconfig.h and target_platform.h
    
    # get list of include directories for FATFS
    get_target_property(IDF_FATFS_INCLUDE_DIRECTORIES __idf_fatfs INCLUDE_DIRECTORIES)

    # add nanoCLR include path to FATFS so our lwipots are taken instead of the IDF ones
    list(APPEND
        IDF_FATFS_INCLUDE_DIRECTORIES
        ${CMAKE_BINARY_DIR}/targets/${RTOS}/${TARGET_BOARD}/
    )

    # add nanoCLR include path to FATFS so our lwipots are taken instead of the IDF ones
    list(APPEND
        IDF_FATFS_INCLUDE_DIRECTORIES
            ${CMAKE_SOURCE_DIR}/targets/ESP32/${TARGET_BOARD}
    )

    # replace the include directories
    set_property(
        TARGET __idf_fatfs 
        PROPERTY INCLUDE_DIRECTORIES ${IDF_FATFS_INCLUDE_DIRECTORIES}
    )

    # Link the static libraries to the executable
    target_link_libraries(${NANOCLR_PROJECT_NAME}.elf 
        ${IDF_LIBRARIES_TO_ADD}
    )

    # add nano libraries to the link dependencies of IDF build
    idf_build_set_property(__LINK_DEPENDS "NF_CoreCLR;NF_NativeAssemblies;NF_Debugger;WireProtocol;NF_Network" APPEND)

    nf_setup_partition_tables_generator()

    # set variables to mirror SDK CONFIG options

    # need to read the supplied SDK CONFIG file        
    file(READ
        ${CMAKE_SOURCE_DIR}/sdkconfig
        SDKCONFIG_DEFAULT_CONTENTS)

    # find out if there is support for PSRAM
    set(SPIRAM_SUPPORT_PRESENT -1)
    if(TARGET_SERIES_SHORT STREQUAL "esp32")
        string(FIND ${SDKCONFIG_DEFAULT_CONTENTS} "CONFIG_ESP32_SPIRAM_SUPPORT=y" SPIRAM_SUPPORT_PRESENT)
    elseif(TARGET_SERIES_SHORT STREQUAL "esp32s2")
        string(FIND ${SDKCONFIG_DEFAULT_CONTENTS} "CONFIG_ESP32S2_SPIRAM_SUPPORT=y" SPIRAM_SUPPORT_PRESENT)
    elseif(TARGET_SERIES_SHORT STREQUAL "esp32s3")
        string(FIND ${SDKCONFIG_DEFAULT_CONTENTS} "CONFIG_ESP32S3_SPIRAM_SUPPORT=y" SPIRAM_SUPPORT_PRESENT)
    endif()

    # set variable
    if(${SPIRAM_SUPPORT_PRESENT} GREATER -1)
        set(PSRAM_INFO ", support for PSRAM")
        message(STATUS "Support for PSRAM included")
    else()
        set(PSRAM_INFO ", without support for PSRAM")
        message(STATUS "Support for PSRAM **IS NOT** included")
    endif()

    # find out revision info (ESP32)
    string(FIND ${SDKCONFIG_DEFAULT_CONTENTS} "CONFIG_ESP32_REV_MIN_0=y" CONFIG_ESP32_REV_MIN_0_POS)
    string(FIND ${SDKCONFIG_DEFAULT_CONTENTS} "CONFIG_ESP32_REV_MIN_3=y" CONFIG_ESP32_REV_MIN_3_POS)

    # set variable
    if(${CONFIG_ESP32_REV_MIN_0_POS} GREATER -1)
        set(REVISION_INFO ", chip rev. >= 0")
        message(STATUS "Building for chip revision >= 0")
    elseif(${CONFIG_ESP32_REV_MIN_3_POS} GREATER -1)
        set(REVISION_INFO ", chip rev. 3")
        message(STATUS "Building for chip revision 3")
    endif()

    # find out revision info (ESP32-C3)
    unset(ESP32_REVISION)
    string(FIND ${SDKCONFIG_DEFAULT_CONTENTS} "CONFIG_ESP32C3_REV_MIN_2=y" CONFIG_ESP32C3_REV_MIN_2_POS)
    string(FIND ${SDKCONFIG_DEFAULT_CONTENTS} "CONFIG_ESP32C3_REV_MIN_3=y" CONFIG_ESP32C3_REV_MIN_3_POS)
    string(FIND ${SDKCONFIG_DEFAULT_CONTENTS} "CONFIG_ESP32C3_REV_MIN_4=y" CONFIG_ESP32C3_REV_MIN_4_POS)

    # set variable
    if(${CONFIG_ESP32C3_REV_MIN_2_POS} GREATER -1)
        set(REVISION_INFO ", chip rev. >= 2")
        message(STATUS "Building for chip revision >= 2")
        set(ESP32_REVISION "2" CACHE STRING "ESP32 revision")
    elseif(${CONFIG_ESP32C3_REV_MIN_3_POS} GREATER -1)
        set(REVISION_INFO ", chip rev. >= 3")
        message(STATUS "Building for chip revision >= 3")
        set(ESP32_REVISION "3" CACHE STRING "ESP32 revision")
    elseif(${CONFIG_ESP32C3_REV_MIN_4_POS} GREATER -1)
        set(REVISION_INFO ", chip rev. 4")
        message(STATUS "Building for chip revision 4")
        set(ESP32_REVISION "4" CACHE STRING "ESP32 revision")
    endif()

    nf_fix_esp32c3_rom_file()

    # find out if there is support for BLE
    string(FIND ${SDKCONFIG_DEFAULT_CONTENTS} "CONFIG_BT_ENABLED=y" CONFIG_BT_ENABLED_POS)

    # set variable
    if(${CONFIG_BT_ENABLED_POS} GREATER -1)
        set(BLE_INFO ", support for BLE")
    endif()    

    ############################################################
    # output component size summary for the nanoCLR executable #
    # more on this here: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/performance/size.html#size-summary-idf-py-size
    
    # set the map file with the components
    set(nanoCLRMapfile "${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.map")
    target_link_libraries(${NANOCLR_PROJECT_NAME}.elf "-Wl,--cref" "-Wl,--Map=\"${nanoCLRMapfile}\"")

    # setup the call to the python script to generate the size summary
    set(ESP32_IDF_SIZE_UTILITY ${IDF_PATH_CMAKED}/tools/idf_size.py)
    set(output_idf_size "python" "${ESP32_IDF_SIZE_UTILITY}")

    add_custom_command(
        TARGET ${NANOCLR_PROJECT_NAME}.elf POST_BUILD
        COMMAND ${output_idf_size}
        --archives --target ${TARGET_SERIES_SHORT} ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.map
        COMMENT "Ouptut IDF size summary")

endmacro()

# macro to clear binary files related with nanoBooter from output
# to make sure that the build file it's up to date
macro(nf_clear_output_files_nanobooter)
    message("nanoCLR for ESP32 doesn't have a nanoBooter! Check the build options.")
endmacro()

# macro to clear binary files related with nanoCLR from output
# to make sure that the build file it's up to date
macro(nf_clear_output_files_nanoclr)
    list(APPEND CLR_BUILD_FILES_TO_REMOVE ${CMAKE_BINARY_DIR}/${NANOCLR_PROJECT_NAME}.bin)
    list(APPEND CLR_BUILD_FILES_TO_REMOVE ${CMAKE_BINARY_DIR}/${NANOCLR_PROJECT_NAME}.elf)

    add_custom_command(
        TARGET ${NANOCLR_PROJECT_NAME}.elf
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove -f ${CLR_BUILD_FILES_TO_REMOVE}
        COMMAND_EXPAND_LISTS
        COMMENT "Removing nanoCLR bin and elf files from build folder"
    )

endmacro()
