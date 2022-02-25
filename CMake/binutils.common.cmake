#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# set compile definitions that are common to all builds/targets
# BUILD_TARGET target name building (either nanoBooter or nanoCLR)
# optional TARGET target name to set compiler definitions 
macro(nf_common_compiler_definitions) 

    # parse arguments
    cmake_parse_arguments(NFCCF "" "BUILD_TARGET;TARGET" "" ${ARGN})

    if(NOT NFCCF_BUILD_TARGET OR "${NFCCF_BUILD_TARGET}" STREQUAL "")
        message(FATAL_ERROR "Need to set BUILD_TARGET argument when calling nf_common_compiler_definitions()")
    endif()
   
    if(NOT NFCCF_TARGET OR "${NFCCF_TARGET}" STREQUAL "")
        set(NFCCF_TARGET ${NFCCF_BUILD_TARGET})
    endif()

    # set define according to target
    string(FIND ${NFCCF_BUILD_TARGET} ${NANOBOOTER_PROJECT_NAME} BOOTER_INDEX)
    string(FIND ${NFCCF_BUILD_TARGET} ${NANOCLR_PROJECT_NAME} CLR_INDEX)
    
    if(${BOOTER_INDEX} EQUAL 0)
        target_compile_definitions(${NFCCF_TARGET} PUBLIC -DI_AM_NANOBOOTER)
    elseif(${CLR_INDEX} EQUAL 0)
        target_compile_definitions(${NFCCF_TARGET} PUBLIC -DI_AM_NANOCLR)
    else()
        message(FATAL_ERROR "\n\n Build target name '${NFCCF_BUILD_TARGET}' is not any of the expected ones: '${NANOBOOTER_PROJECT_NAME}' or '${NANOCLR_PROJECT_NAME}'")
    endif()

    # build types that have debugging capabilities AND are NOT RTM have to have the define 'NANOCLR_ENABLE_SOURCELEVELDEBUGGING'
    if((NOT NF_BUILD_RTM) OR NF_FEATURE_DEBUGGER)
        target_compile_definitions(${NFCCF_TARGET} PUBLIC -DNANOCLR_ENABLE_SOURCELEVELDEBUGGING)
    endif()

    # set compiler definition for RTM build option
    if(NF_BUILD_RTM)
        target_compile_definitions(${NFCCF_TARGET} PUBLIC -DBUILD_RTM)
    endif()

    # set compiler definition for using Application Domains feature
    if(NF_FEATURE_USE_APPDOMAINS)
        target_compile_definitions(${NFCCF_TARGET} PUBLIC -DNANOCLR_USE_APPDOMAINS)
    endif()

    # set compiler definition for implementing (or not) CRC32 in Wire Protocol
    if(NF_WP_IMPLEMENTS_CRC32)
        target_compile_definitions(${NFCCF_TARGET} PUBLIC -DWP_IMPLEMENTS_CRC32)
    endif()

    # set definition for Wire Protocol trace mask
    target_compile_definitions(${NFCCF_TARGET} PUBLIC -DTRACE_MASK=${WP_TRACE_MASK})

    # set compiler definition regarding inclusion of trace messages and checks on CLR
    if(NF_PLATFORM_NO_CLR_TRACE)
        target_compile_definitions(${NFCCF_TARGET} PUBLIC -DPLATFORM_NO_CLR_TRACE=1)
    endif()

    # set compiler definition regarding CLR IL inlining
    if(NF_CLR_NO_IL_INLINE)
        target_compile_definitions(${NFCCF_TARGET} PUBLIC -DNANOCLR_NO_IL_INLINE=1)
    endif()

endmacro()

# Add packages that are common to ALL builds
# To be called from target CMakeList.txt
macro(nf_add_common_packages)

    find_package(WireProtocol REQUIRED QUIET)
    find_package(NF_NativeAssemblies REQUIRED QUIET)
    find_package(NF_CoreCLR REQUIRED QUIET)
    find_package(NF_HALCore REQUIRED QUIET)

    if(NF_FEATURE_DEBUGGER)
        find_package(NF_Debugger REQUIRED QUIET)
        find_package(NF_Diagnostics REQUIRED QUIET)
    endif()

endmacro()

# Add common dependencies to a specific CMake target
# To be called from target CMakeList.txt
macro(nf_add_common_dependencies target)

    # dependencies specific to nanoBooter
    if("${target}" STREQUAL "${NANOBOOTER_PROJECT_NAME}")

    endif()
    
    # dependencies specific to nanoCLR
    if("${target}" STREQUAL "${NANOCLR_PROJECT_NAME}")
    
        configure_file(${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/target_platform.h.in
                       ${CMAKE_BINARY_DIR}/targets/${RTOS}/${TARGET_BOARD}/target_platform.h @ONLY)

    endif()

endmacro()

# Add common include directories to a specific CMake target
# To be called from target CMakeList.txt
macro(nf_add_common_include_directories target)

    target_include_directories(${target}.elf PUBLIC
        
        # target path (both source and binary)
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        # target common path
        ${CMAKE_CURRENT_SOURCE_DIR}/common

        # path for CMake target (both source and binary)
        ${CMAKE_CURRENT_BINARY_DIR}/${target}
        ${CMAKE_CURRENT_SOURCE_DIR}/${target}
        
        ${NF_HALCore_INCLUDE_DIRS}
    )

    # includes specific to nanoBooter
    if(${target} STREQUAL ${NANOBOOTER_PROJECT_NAME})

        target_include_directories(${target}.elf PUBLIC
            ${WireProtocol_INCLUDE_DIRS}
            ${CMAKE_BINARY_DIR}/targets/${RTOS}/${TARGET_BOARD}/nanoBooter
            ${CMAKE_SOURCE_DIR}/src/PAL/Include
            ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/Networking.Sntp
        )

    endif()

    # includes specific to nanoCLR
    if(${target} STREQUAL ${NANOCLR_PROJECT_NAME})

        target_include_directories(${target}.elf PUBLIC

            # directories for nanoFramework libraries
            ${NF_Diagnostics_INCLUDE_DIRS}
            ${Graphics_Includes}
        )

    endif()


endmacro()

# Add common target sources to a specific CMake target
# To be called from target CMakeList.txt
# mandatory TARGET parameter to set the target
# optional EXTRA_LIBRARIES with extra libraries to be included in the target build
macro(nf_add_common_sources)

    # parse arguments
    cmake_parse_arguments(
        NFACS 
        "" 
        "TARGET" 
        "EXTRA_LIBRARIES" 
        ${ARGN})

    target_sources(${NFACS_TARGET}.elf PUBLIC
    
        ${CMAKE_CURRENT_SOURCE_DIR}/target_common.c
        ${CMAKE_CURRENT_SOURCE_DIR}/target_BlockStorage.c
        ${CMAKE_SOURCE_DIR}/src/PAL/BlockStorage/nanoPAL_BlockStorage.c
        ${COMMON_PROJECT_SOURCES}
        ${NF_HALCore_SOURCES}
    )

    # sources specific to nanoBooter
    if(${NFACS_TARGET} STREQUAL ${NANOBOOTER_PROJECT_NAME})

        target_sources(${NFACS_TARGET}.elf PUBLIC
            ${NANOBOOTER_PROJECT_SOURCES}
            ${WireProtocol_SOURCES}
        )

        # include configuration manager file
        if(NF_FEATURE_HAS_CONFIG_BLOCK)
            # feature enabled, full support
            target_sources(${NFACS_TARGET}.elf PUBLIC
                ${CMAKE_SOURCE_DIR}/src/HAL/nanoHAL_ConfigurationManager.c)

        else()
            # feature disabled, stubs only
            target_sources(${NFACS_TARGET}.elf PUBLIC
                ${CMAKE_SOURCE_DIR}/src/HAL/nanoHAL_ConfigurationManager_stubs.c)

        endif()

    endif()

    # sources specific to nanoCLR
    if(${NFACS_TARGET} STREQUAL ${NANOCLR_PROJECT_NAME})

        target_link_libraries(${NFACS_TARGET}.elf
            nano::NF_CoreCLR
            nano::NF_NativeAssemblies
            nano::NF_Debugger
            nano::WireProtocol
            
            ${NFACS_EXTRA_LIBRARIES}
        )

        target_sources(${NFACS_TARGET}.elf PUBLIC

            ${NANOCLR_PROJECT_SOURCES}

            # sources for nanoFramework libraries
            ${Graphics_Sources}
        )

    endif()

endmacro()

function(nf_generate_dfu_package file1 address1 file2 address2 outputfilename)

    add_custom_command(
            
        TARGET ${NANOCLR_PROJECT_NAME}.elf POST_BUILD

        COMMAND ${TOOL_HEX2DFU_PREFIX}/hex2dfu 

        -b="${file1}" -a="${address1}"
        -b="${file2}" -a="${address2}"
        -o="${outputfilename}"

        WORKING_DIRECTORY ${TOOL_HEX2DFU_PREFIX} 

        COMMENT "exporting bin files to DFU image" 
    )

    # need to add a dependency of NANOCLR to NANOBOOTER because DFU util needs bin outputs of both targets
    add_dependencies(${NANOCLR_PROJECT_NAME}.elf ${NANOBOOTER_PROJECT_NAME}.elf)

endfunction()

function(nf_generate_hex_package file1 file2 outputfilename)

    add_custom_command(
            
        TARGET ${NANOCLR_PROJECT_NAME}.elf POST_BUILD

        COMMAND ${TOOL_SRECORD_PREFIX}/srec_cat

        "${file1}" -Intel
        "${file2}" -Intel
        -o "${outputfilename}" -Intel
        -line-length=44

        WORKING_DIRECTORY ${TOOL_SRECORD_PREFIX} 

        COMMENT "exporting hex files to one hex file" 
    )

    # need to add a dependency of NANOCLR to NANOBOOTER because SRECORD util needs hex outputs of both targets
    add_dependencies(${NANOCLR_PROJECT_NAME}.elf ${NANOBOOTER_PROJECT_NAME}.elf)

endfunction()

# generates a binary file with nanoBooter + nanoCLR at the proper addresses
# ready to be drag & drop on targets that feature DAPLink 
function(nf_generate_bin_package file1 file2 offset outputfilename)

    add_custom_command(

        TARGET ${NANOCLR_PROJECT_NAME}.elf POST_BUILD

        COMMAND ${TOOL_SRECORD_PREFIX}/srec_cat

        ${file1} -Binary
        ${file2} -Binary -offset 0x${offset}
        -o ${outputfilename} -Binary

        WORKING_DIRECTORY ${TOOL_SRECORD_PREFIX} 

        COMMENT "exporting hex files to one binary file" 
    )

    # need to add a dependency of NANOCLR to NANOBOOTER because SRECORD util needs hex outputs of both targets
    add_dependencies(${NANOCLR_PROJECT_NAME}.elf ${NANOBOOTER_PROJECT_NAME}.elf)

endfunction()

function(nf_generate_build_output_files target)

    # need to remove the .elf suffix from target name
    string(FIND ${target} "." TARGET_EXTENSION_DOT_INDEX)
    string(SUBSTRING ${target} 0 ${TARGET_EXTENSION_DOT_INDEX} TARGET_SHORT)

    set(TARGET_HEX_FILE ${CMAKE_BINARY_DIR}/${TARGET_SHORT}.hex)
    set(TARGET_S19_FILE ${CMAKE_BINARY_DIR}/${TARGET_SHORT}.s19)
    set(TARGET_BIN_FILE ${CMAKE_BINARY_DIR}/${TARGET_SHORT}.bin)
    set(TARGET_DUMP_FILE ${CMAKE_BINARY_DIR}/${TARGET_SHORT}.lst)

    if(CMAKE_BUILD_TYPE EQUAL "Release" OR CMAKE_BUILD_TYPE EQUAL "MinSizeRel")

        add_custom_command(TARGET ${TARGET_SHORT}.elf POST_BUILD
                # copy target image to other formats
                COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${TARGET_SHORT}.elf> ${TARGET_HEX_FILE}
                COMMAND ${CMAKE_OBJCOPY} -Osrec $<TARGET_FILE:${TARGET_SHORT}.elf> ${TARGET_S19_FILE}
                COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${TARGET_SHORT}.elf> ${TARGET_BIN_FILE}

                # copy target file to build folder (this is only usefull for debugging in VS Code because of path in launch.json)
                COMMAND ${CMAKE_OBJCOPY} $<TARGET_FILE:${TARGET_SHORT}.elf> ${CMAKE_SOURCE_DIR}/build/${TARGET_SHORT}.elf

                COMMENT "Generate nanoBooter HEX and BIN files for deployment")

    else()

        add_custom_command(TARGET ${TARGET_SHORT}.elf POST_BUILD
                # copy target image to other formats
                COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${TARGET_SHORT}.elf> ${TARGET_HEX_FILE}
                COMMAND ${CMAKE_OBJCOPY} -Osrec $<TARGET_FILE:${TARGET_SHORT}.elf> ${TARGET_S19_FILE}
                COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${TARGET_SHORT}.elf> ${TARGET_BIN_FILE}

                # copy target file to build folder (this is only usefull for debugging in VS Code because of path in launch.json)
                COMMAND ${CMAKE_OBJCOPY} $<TARGET_FILE:${TARGET_SHORT}.elf> ${CMAKE_SOURCE_DIR}/build/${TARGET_SHORT}.elf

                # dump target image as source code listing 
                # ONLY when DEBUG info is available, this is on 'Debug' and 'RelWithDebInfo'
                COMMAND ${CMAKE_OBJDUMP} -d -EL -S $<TARGET_FILE:${TARGET_SHORT}.elf> > ${TARGET_DUMP_FILE}

                COMMENT "Generate nanoBooter HEX and BIN files for deployment, LST file for debug")

    endif()

    nf_add_hex_bin_dump_targets(${target})
        
    # add this to print the size of the output targets
    nf_print_target_size(${target})

endfunction()


#######################################################################################################################################
# this function sets the linker options AND a specific linker file (full path and name, including extension)
function(nf_set_linker_options_and_file target linker_file_name)

    get_target_property(TARGET_LD_FLAGS ${target} LINK_FLAGS)
    if(TARGET_LD_FLAGS)
        set(TARGET_LD_FLAGS "-T${linker_file_name} ${TARGET_LD_FLAGS}")
    else()
        set(TARGET_LD_FLAGS "-T${linker_file_name}")
    endif()
    set_target_properties(${target} PROPERTIES LINK_FLAGS ${TARGET_LD_FLAGS})

endfunction()

# check if a directory exists
# going through the directory to find if it's not empty takes a lot of time because it sweeps all files
# simplifying this now to speed up local builds
macro(nf_directory_exists_not_empty path pathExists)

    if(IS_DIRECTORY "${path}")
        set(${pathExists} TRUE)
    else()
        set(${pathExists} FALSE)
    endif()

endmacro()

# define the base path for the library modules
macro(nf_set_base_path_for_libraries_modules path)
    set(BASE_PATH_FOR_CLASS_LIBRARIES_MODULES ${path})
endmacro()

# TARGET parameter to set the target that's setting them for
# optional EXTRA_LINKMAP_PROPERTIES with extra properties to add to the link map
macro(nf_set_link_map)

    # parse arguments
    cmake_parse_arguments(NFSLM "" "TARGET;EXTRA_LINKMAP_PROPERTIES" "" ${ARGN})
    
    if(NOT NFSLM_TARGET OR "${NFSLM_TARGET}" STREQUAL "")
        message(FATAL_ERROR "Need to set TARGET argument when calling nf_set_link_map()")
    endif()

    # need to remove the .elf suffix from target name
    string(FIND ${NFSLM_TARGET} "." TARGET_EXTENSION_DOT_INDEX)
    string(SUBSTRING ${NFSLM_TARGET} 0 ${TARGET_EXTENSION_DOT_INDEX} TARGET_SHORT)
    
    # add linker flags to generate map file
    set_property(TARGET ${TARGET_SHORT}.elf APPEND_STRING PROPERTY LINK_FLAGS " -Wl,-Map=${CMAKE_BINARY_DIR}/${TARGET_SHORT}.map${NFSLM_EXTRA_LINKMAP_PROPERTIES}")

endmacro()

# macro to include libraries in build
# to be called from the various gcc_options modules
# TARGET parameter with target building 
macro(nf_include_libraries_in_build target)
    
    # find out which build target
    string(FIND ${target} ${NANOBOOTER_PROJECT_NAME} BOOTER_INDEX)
    string(FIND ${target} ${NANOCLR_PROJECT_NAME} CLR_INDEX)
    
    if(${BOOTER_INDEX} EQUAL 0)
        # no libs in nanoBooter
    elseif(${CLR_INDEX} EQUAL 0)

        # these are always present
        set_property(TARGET ${target} APPEND_STRING PROPERTY LINK_FLAGS " -Wl,--whole-archive -L${CMAKE_CURRENT_BINARY_DIR} -lNF_CoreCLR -Wl,--no-whole-archive ")
        set_property(TARGET ${target} APPEND_STRING PROPERTY LINK_FLAGS " -Wl,--whole-archive -L${CMAKE_CURRENT_BINARY_DIR} -lNF_NativeAssemblies -Wl,--no-whole-archive ")
        set_property(TARGET ${target} APPEND_STRING PROPERTY LINK_FLAGS " -Wl,--whole-archive -L${CMAKE_CURRENT_BINARY_DIR} -lWireProtocol -Wl,--no-whole-archive ")
        
        # these are dependent on the feature being enabled
        if(USE_NETWORKING_OPTION)
            set_property(TARGET ${target} APPEND_STRING PROPERTY LINK_FLAGS " -Wl,--whole-archive -L${CMAKE_CURRENT_BINARY_DIR} -lNF_Network -Wl,--no-whole-archive ")
        endif()

        if(NF_FEATURE_DEBUGGER)
            set_property(TARGET ${target} APPEND_STRING PROPERTY LINK_FLAGS " -Wl,--whole-archive -L${CMAKE_CURRENT_BINARY_DIR} -lNF_Debugger -Wl,--no-whole-archive ")
        endif()

        # this one has to be at the very end of the list to keep the linker happy
        set_property(TARGET ${target} APPEND_STRING PROPERTY LINK_FLAGS " -Wl,--whole-archive -lgcc -Wl,--no-whole-archive ")

    endif()

endmacro()

# macro to setup the build for a target
# optional HAS_NANOBOOTER specifing if the target implements nanoBooter
# BOOTER_LINKER_FILE with the path to the linker file for nanoBooter (if the target has it)
# mandatory CLR_LINKER_FILE with the path to the linker file for nanoCLR
# optional BOOTER_EXTRA_SOURCE_FILES with paths to extra files to be added to the nanoBooter build target
# optional CLR_EXTRA_SOURCE_FILES with paths to extra files to be added to the nanoCLR build target
# optional BOOTER_EXTRA_COMPILE_DEFINITIONS extra nanoBooter compile definitions to pass to nf_set_compile_definitions() 
# optional CLR_EXTRA_COMPILE_DEFINITIONS extra nanoCLR compile definitions to pass to nf_set_compile_definitions() 
# optional BOOTER_EXTRA_COMPILE_OPTIONS extra nanoBooter compile options to pass to nf_set_compile_options() 
# optional CLR_EXTRA_COMPILE_OPTIONS extra nanoCLR compile options to pass to nf_set_compile_options() 
# optional BOOTER_EXTRA_LINKMAP_PROPERTIES extra nanoBooter link map properties to pass to nf_set_link_map() 
# optional CLR_EXTRA_LINKMAP_PROPERTIES extra nanoCLR link map properties to pass to nf_set_link_map()
# optional BOOTER_EXTRA_LINK_FLAGS extra nanoBooter link flags to pass to nf_set_link_options() 
# optional CLR_EXTRA_LINK_FLAGS extra nanoCLR link flags to pass to nf_set_link_options() 
macro(nf_setup_target_build_common)

    # parse arguments
    cmake_parse_arguments(
        NFSTBC 
        "HAS_NANOBOOTER" 
        "BOOTER_LINKER_FILE;CLR_LINKER_FILE;BOOTER_EXTRA_LINKMAP_PROPERTIES;CLR_EXTRA_LINKMAP_PROPERTIES;BOOTER_EXTRA_COMPILE_DEFINITIONS;CLR_EXTRA_COMPILE_DEFINITIONS;BOOTER_EXTRA_COMPILE_OPTIONS;CLR_EXTRA_COMPILE_OPTIONS;BOOTER_EXTRA_LINK_FLAGS;CLR_EXTRA_LINK_FLAGS" 
        "BOOTER_EXTRA_SOURCE_FILES;CLR_EXTRA_SOURCE_FILES;BOOTER_EXTRA_LIBRARIES;CLR_EXTRA_LIBRARIES" 
        ${ARGN})

    if(NOT NFSTBC_HAS_NANOBOOTER 
        AND (BOOTER_EXTRA_SOURCE_FILES OR BOOTER_EXTRA_COMPILE_DEFINITIONS OR BOOTER_EXTRA_LINKMAP_PROPERTIES))
        message(FATAL_ERROR "Add files/options for booter without setting HAS_NANOBOOTER argument when calling nf_setup_target_build()")
    endif()

    if(NFSTBC_HAS_NANOBOOTER AND (NOT NFSTBC_BOOTER_LINKER_FILE OR "${NFSTBC_BOOTER_LINKER_FILE}" STREQUAL ""))
        message(FATAL_ERROR "Need to provide BOOTER_LINKER_FILE argument when target has HAS_NANOBOOTER defined")
    endif()
    
    if(NOT NFSTBC_CLR_LINKER_FILE OR "${NFSTBC_CLR_LINKER_FILE}" STREQUAL "")
        message(FATAL_ERROR "Need to provide CLR_LINKER_FILE argument")
    endif()

    #######################################
    # now the actual calls for building a target

    # add packages
    nf_add_common_packages()
    nf_add_platform_packages()

    #######################################

    add_subdirectory("common")

    if(NFSTBC_HAS_NANOBOOTER)
        add_subdirectory("nanoBooter")
    endif()

    add_subdirectory("nanoCLR")

    #######################
    # nanoBooter executable
    if(NFSTBC_HAS_NANOBOOTER)

        add_executable(
            # executables for project, project sources
            ${NANOBOOTER_PROJECT_NAME}.elf

            # extra build files 
            ${NFSTBC_BOOTER_EXTRA_SOURCE_FILES}
        )

        nf_add_platform_packages(TARGET ${NANOBOOTER_PROJECT_NAME})
        nf_add_platform_dependencies(${NANOBOOTER_PROJECT_NAME})

        nf_add_common_sources(TARGET ${NANOBOOTER_PROJECT_NAME})
        nf_add_platform_sources(${NANOBOOTER_PROJECT_NAME})

        # include directories for nanoBooter
        nf_add_common_include_directories(${NANOBOOTER_PROJECT_NAME})
        nf_add_platform_include_directories(${NANOBOOTER_PROJECT_NAME})

        # set compile options
        nf_set_compile_options(TARGET ${NANOBOOTER_PROJECT_NAME}.elf EXTRA_COMPILE_OPTIONS ${NFSTBC_BOOTER_EXTRA_COMPILE_OPTIONS})
        
        # set compile definitions
        nf_set_compile_definitions(TARGET ${NANOBOOTER_PROJECT_NAME}.elf EXTRA_COMPILE_DEFINITIONS ${NFSTBC_BOOTER_EXTRA_COMPILE_DEFINITIONS} BUILD_TARGET ${NANOBOOTER_PROJECT_NAME})

        # set linker files
        if(CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
            nf_set_linker_file(${NANOBOOTER_PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/nanoBooter/${NFSTBC_BOOTER_LINKER_FILE}-DEBUG.ld)
        else()
            nf_set_linker_file(${NANOBOOTER_PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/nanoBooter/${NFSTBC_BOOTER_LINKER_FILE}.ld)
        endif()

        # set linker options
        nf_set_link_options(TARGET ${NANOBOOTER_PROJECT_NAME}.elf EXTRA_LINK_FLAGS ${NFSTBC_BOOTER_EXTRA_LINK_FLAGS})

        # set link maps
        ###########################################################
        # the sizes of CRT heap and ChibiOS stacks are defined here
        nf_set_link_map(
            TARGET 
                ${NANOBOOTER_PROJECT_NAME}.elf
            EXTRA_LINKMAP_PROPERTIES ${NFSTBC_BOOTER_EXTRA_LINKMAP_PROPERTIES})

        nf_clear_output_files_nanobooter()

    endif()

    #######################
    # nanoCLR executable
    add_executable(
        # executables for project, project sources
        ${NANOCLR_PROJECT_NAME}.elf

        # extra build files 
        ${NFSTBC_CLR_EXTRA_SOURCE_FILES}
    )

    nf_add_platform_packages(TARGET ${NANOCLR_PROJECT_NAME})
    nf_add_platform_dependencies(${NANOCLR_PROJECT_NAME})

    nf_add_common_sources(TARGET ${NANOCLR_PROJECT_NAME} EXTRA_LIBRARIES ${CLR_EXTRA_LIBRARIES})
    nf_add_platform_sources(${NANOCLR_PROJECT_NAME})

    # include directories for nanoCLR
    nf_add_common_include_directories(${NANOCLR_PROJECT_NAME})
    nf_add_platform_include_directories(${NANOCLR_PROJECT_NAME})

    # set compile options
    nf_set_compile_options(TARGET ${NANOCLR_PROJECT_NAME}.elf EXTRA_COMPILE_OPTIONS ${NFSTBC_CLR_EXTRA_COMPILE_OPTIONS})

    if(USE_SECURITY_MBEDTLS_OPTION AND NOT RTOS_ESP32_CHECK)

        # mbedTLS requires setting a compiler definition in order to pass a config file
        target_compile_definitions(mbedcrypto PUBLIC "-DMBEDTLS_CONFIG_FILE=\"${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/mbedTLS/nf_mbedtls_config.h\"")
        
        # need to add extra include directories for mbedTLS
        target_include_directories(
            mbedcrypto PUBLIC
            ${CMAKE_SOURCE_DIR}/src/CLR/Include
            ${CMAKE_SOURCE_DIR}/src/HAL/Include
            ${CMAKE_SOURCE_DIR}/src/PAL
            ${CMAKE_SOURCE_DIR}/src/PAL/Include
            ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets
            ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/mbedTLS
            ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/Networking.Sntp
            ${CMAKE_SOURCE_DIR}/targets/${RTOS}/_include
            ${TARGET_BASE_LOCATION}/nanoCLR
            ${TARGET_BASE_LOCATION}
        )

        # target_sources(mbedcrypto PUBLIC ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/mbedTLS/ssl_generic.cpp)
        # target_sources(mbedcrypto PRIVATE  ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/mbedTLS/mbed_network.c)
       
        if(NOT RTOS_ESP32_CHECK)
            # platform implementation of hardware random provider
            target_sources(mbedcrypto PRIVATE ${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/mbedtls_entropy_hardware_pool.c)
        endif()

        nf_set_compile_options(TARGET mbedcrypto BUILD_TARGET ${NANOCLR_PROJECT_NAME})
        nf_set_compile_options(TARGET mbedx509 BUILD_TARGET ${NANOCLR_PROJECT_NAME})
        nf_set_compile_options(TARGET mbedtls BUILD_TARGET ${NANOCLR_PROJECT_NAME})
        nf_set_compile_definitions(TARGET mbedcrypto BUILD_TARGET ${NANOCLR_PROJECT_NAME})
        nf_set_compile_definitions(TARGET mbedx509 BUILD_TARGET ${NANOCLR_PROJECT_NAME})
        nf_set_compile_definitions(TARGET mbedtls BUILD_TARGET ${NANOCLR_PROJECT_NAME})

    endif()

    # set compile definitions
    nf_set_compile_definitions(TARGET ${NANOCLR_PROJECT_NAME}.elf EXTRA_COMPILE_DEFINITIONS ${NFSTBC_CLR_EXTRA_COMPILE_DEFINITIONS} BUILD_TARGET ${NANOCLR_PROJECT_NAME} )

    # set linker files
    if(CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
        nf_set_linker_file(${NANOCLR_PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/${NFSTBC_CLR_LINKER_FILE}-DEBUG.ld)
    else()
        nf_set_linker_file(${NANOCLR_PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/${NFSTBC_CLR_LINKER_FILE}.ld)
    endif()

    # set linker options
    nf_set_link_options(TARGET ${NANOCLR_PROJECT_NAME}.elf EXTRA_LINK_FLAGS ${NFSTBC_CLR_EXTRA_LINK_FLAGS})

    # set link maps
    ###########################################################
    # the sizes of CRT heap and ChibiOS stacks are defined here
    nf_set_link_map(
        TARGET 
            ${NANOCLR_PROJECT_NAME}.elf
        EXTRA_LINKMAP_PROPERTIES ${NFSTBC_CLR_EXTRA_LINKMAP_PROPERTIES})

    # generate output files
    if(NFSTBC_HAS_NANOBOOTER)
        nf_generate_build_output_files(${NANOBOOTER_PROJECT_NAME}.elf)
    endif()

    nf_generate_build_output_files(${NANOCLR_PROJECT_NAME}.elf)
   
    nf_clear_output_files_nanoclr()

endmacro()

# macro to clear binary files related with nanoBooter from output
# to make sure that the build file it's up to date
macro(nf_clear_common_output_files_nanobooter)
    list(APPEND BOOTER_BUILD_FILES_TO_REMOVE ${CMAKE_BINARY_DIR}/${NANOBOOTER_PROJECT_NAME}.bin)
    list(APPEND BOOTER_BUILD_FILES_TO_REMOVE ${CMAKE_BINARY_DIR}/${NANOBOOTER_PROJECT_NAME}.elf)
    list(APPEND BOOTER_BUILD_FILES_TO_REMOVE ${CMAKE_BINARY_DIR}/${NANOBOOTER_PROJECT_NAME}.hex)

    add_custom_command(
        TARGET ${NANOBOOTER_PROJECT_NAME}.elf
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove -f ${BOOTER_BUILD_FILES_TO_REMOVE}
        COMMAND_EXPAND_LISTS
        COMMENT "Removing nanoBooter bin and elf files from build folder"
    )
    
endmacro()

# macro to clear binary files related with nanoCLR from output
# to make sure that the build file it's up to date
macro(nf_clear_common_output_files_nanoclr)
    list(APPEND CLR_BUILD_FILES_TO_REMOVE ${CMAKE_BINARY_DIR}/${NANOCLR_PROJECT_NAME}.bin)
    list(APPEND CLR_BUILD_FILES_TO_REMOVE ${CMAKE_BINARY_DIR}/${NANOCLR_PROJECT_NAME}.elf)
    list(APPEND CLR_BUILD_FILES_TO_REMOVE ${CMAKE_BINARY_DIR}/${NANOCLR_PROJECT_NAME}.hex)

    add_custom_command(
        TARGET ${NANOCLR_PROJECT_NAME}.elf
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove -f ${CLR_BUILD_FILES_TO_REMOVE}
        COMMAND_EXPAND_LISTS
        COMMENT "Removing nanoCLR bin and elf files from build folder"
    )

endmacro()
