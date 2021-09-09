#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(binutils.common)

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

function(nf_set_optimization_options TARGET) 

    # target_compile_options(${TARGET} PRIVATE
    #     $<$<CONFIG:Debug>:-Og -femit-class-debug-always -g3 -ggdb>
    #     $<$<CONFIG:Release>:-O3 -flto -fuse-linker-plugin -fno-fat-lto-objects>
    #     $<$<CONFIG:MinSizeRel>:-Os -flto>
    #     $<$<CONFIG:RelWithDebInfo>:-Os -femit-class-debug-always -g3 -ggdb>
    # )

endfunction()


function(nf_set_linker_file target linker_file_name)


    # # set linker file name
    # set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "-T${LINKER_FILE_NAME}")

endfunction()


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

endfunction()

# Add packages that are common to ESP32 platform builds
# To be called from target CMakeList.txt
# optional TARGET argument with target name
macro(nf_add_platform_packages)

    # parse arguments
    cmake_parse_arguments(NFAPP "" "TARGET" "" ${ARGN})

    # packages specific for nanoCRL
    if("${NFAPP_TARGET}" STREQUAL "${NANOCLR_PROJECT_NAME}")

        if(USE_NETWORKING_OPTION)

            find_package(NF_Network REQUIRED QUIET)

            # security provider is mbedTLS
            if(USE_SECURITY_MBEDTLS_OPTION)
                find_package(mbedTLS REQUIRED QUIET)
            endif()

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
            ${TARGET_ESP32_IDF_INCLUDES})

    find_package(ESP32_IDF REQUIRED)

    # if(USE_FILESYSTEM_OPTION)
    #     find_package(CHIBIOS_FATFS REQUIRED)
    # endif()

    # # SPIFFS
    # if(NF_FEATURE_USE_SPIFFS)
    #     find_package(STM32F7_CubePackage REQUIRED)
    #     find_package(SPIFFS REQUIRED)
    # endif()

endmacro()

# Add ESP32 platform dependencies to a specific CMake target
# To be called from target CMakeList.txt
macro(NF_ADD_PLATFORM_DEPENDENCIES TARGET)

    # if(USE_NETWORKING_OPTION)
    #     add_dependencies(${NANOCLR_PROJECT_NAME}.elf NetworkLib)
    # endif()

endmacro()

# Add ESP32 platform include directories to a specific CMake target
# To be called from target CMakeList.txt
macro(nf_add_platform_include_directories target)

    target_include_directories(${target}.elf PUBLIC
                
        # target path (both source and binary)
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        # target common path
        ${CMAKE_CURRENT_SOURCE_DIR}/common

        # path for CMake target (both source and binary)
        ${CMAKE_CURRENT_BINARY_DIR}/${target}
        ${CMAKE_CURRENT_SOURCE_DIR}/${target}

        ${NF_CoreCLR_INCLUDE_DIRS}
        ${NF_NativeAssemblies_INCLUDE_DIRS}

        ${WireProtocol_INCLUDE_DIRS}
    )

    target_include_directories(${target}.elf PUBLIC

        # directories for nanoFramework libraries
        ${NF_CoreCLR_INCLUDE_DIRS}
        ${NF_Debugger_INCLUDE_DIRS}
        ${NF_Diagnostics_INCLUDE_DIRS}
        
        ${NF_Network_INCLUDE_DIRS}

    # includes specific to nanoCRL
    if(${TARGET} STREQUAL ${NANOCLR_PROJECT_NAME})

        target_include_directories(${TARGET}.elf PUBLIC

            ${TARGET_ESP32_IDF_NANOCLR_INCLUDE_DIRS}
        )

    endif()

endmacro()

# Add ESP32 platform target sources to a specific CMake target
# To be called from target CMakeList.txt
macro(nf_add_platform_sources target)

    # add header files with common OS definitions and board definitions specific for each image
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/target_board.h.in
                   ${CMAKE_BINARY_DIR}/targets/${RTOS}/${TARGET_BOARD}/nanoCLR/target_board.h @ONLY)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/target_common.h.in
                   ${CMAKE_BINARY_DIR}/targets/${RTOS}/${TARGET_BOARD}/target_common.h @ONLY)

    configure_file(${CMAKE_SOURCE_DIR}/CMake/ESP32_target_os.h.in
                   ${CMAKE_BINARY_DIR}/targets/${RTOS}/${TARGET_BOARD}/target_os.h @ONLY)

    target_sources(${target}.elf PUBLIC

        ${CMAKE_CURRENT_SOURCE_DIR}/target_common.c
        ${CMAKE_CURRENT_SOURCE_DIR}/target_BlockStorage.c
        ${CMAKE_SOURCE_DIR}/src/PAL/BlockStorage/nanoPAL_BlockStorage.c

        ${COMMON_PROJECT_SOURCES}
        ${WireProtocol_SOURCES}

        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/target_board.h.in
                    ${CMAKE_CURRENT_BINARY_DIR}/nanoCLR/target_board.h @ONLY)

        ${NF_HALCore_SOURCES}
        ${NF_CoreCLR_SOURCES}
        ${NF_Debugger_SOURCES}
        ${NF_Diagnostics_SOURCES}

            ${TARGET_ESP32_IDF_NANOCLR_SOURCES}
            ${ESP32_IDF_SOURCES}
        )

    endif()

    # mbed TLS requires a config file
    if(USE_SECURITY_MBEDTLS_OPTION)
        # this seems to be only option to properly set a compiler define through the command line that needs to be a string literal
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMBEDTLS_CONFIG_FILE=\"<${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/mbedTLS/nf_mbedtls_config.h>\"")
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMBEDTLS_CONFIG_FILE=\"<${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/mbedTLS/nf_mbedtls_config.h>\"")
    endif()

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
# optional BOOTER_EXTRA_LINK_FLAGS extra nanoBooter link flags to pass to nf_set_link_options() 
# optional CLR_EXTRA_LINK_FLAGS extra nanoCLR link flags to pass to nf_set_link_options() 

# optional ESP32_EXTRA_COMPONENTS extra IDF components to add to the project
macro(nf_setup_target_build)


    # # Create idf::esp32 and idf::freertos static libraries
    # idf_build_process(esp32
    #     # try and trim the build; additional components
    #     # will be included as needed based on dependency tree
    #     #
    #     # although esptool_py does not generate static library,
    #     # processing the component is needed for flashing related
    #     # targets and file generation
    #     COMPONENTS 
    #         esp32s2
    #         freertos
    #         esptool_py
        
    #     SDKCONFIG ${CMAKE_CURRENT_DIR}/sdkconfig
    #     BUILD_DIR ${CMAKE_BINARY_DIR})

    #         set(elf_file ${CMAKE_PROJECT_NAME}.elf)
    #         add_executable(${elf_file} nanoCLR/app_main.c)


    # # OK to pass ARGN, to have it perform it's parsings and validation 
    # # nf_setup_target_build_common(${ARGN})

    # target_link_libraries(${elf_file} idf::esp32 idf::freertos idf::spi_flash)
    
    # # Attach additional targets to the executable file for flashing,
    # # linker script generation, partition_table generation, etc.
    # idf_build_executable(${elf_file})


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
