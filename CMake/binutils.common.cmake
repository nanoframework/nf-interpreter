#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# set compiler definitions that are common to all builds/targets
macro(nf_common_compiler_definitions target) 

    # set define according to target
    string(FIND ${target} ${NANOBOOTER_PROJECT_NAME} BOOTER_INDEX)
    string(FIND ${target} ${NANOCLR_PROJECT_NAME} CLR_INDEX)
    
    if(${BOOTER_INDEX} EQUAL 0)
        target_compile_definitions(${target} PUBLIC -DI_AM_NANOBOOTER)
    elseif(${CLR_INDEX} EQUAL 0)
        target_compile_definitions(${target} PUBLIC -DI_AM_NANOCLR)
    else()
        message(FATAL_ERROR "\n\n Target name is not any of the expected ones: '${NANOBOOTER_PROJECT_NAME}' or '${NANOCLR_PROJECT_NAME}'")
    endif()

endmacro()

# Add packages that are common to ALL builds
# To be called from target CMakeList.txt
macro(nf_add_common_packages)

    find_package(WireProtocol REQUIRED)
    find_package(NF_NativeAssemblies)
    find_package(NF_CoreCLR REQUIRED)
    find_package(nanoHALCore REQUIRED)

    if(NF_FEATURE_DEBUGGER)
        find_package(NF_Debugger REQUIRED)
        find_package(NF_Diagnostics REQUIRED)
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

        ${NF_CoreCLR_INCLUDE_DIRS}
        ${NF_NativeAssemblies_INCLUDE_DIRS}

        ${WireProtocol_INCLUDE_DIRS}
        ${nanoHALCore_INCLUDE_DIRS}
    )

    # includes specific to nanoCRL
    if(${target} STREQUAL ${NANOCLR_PROJECT_NAME})

        target_include_directories(${target}.elf PUBLIC

            # directories for nanoFramework libraries
            ${NF_CoreCLR_INCLUDE_DIRS}
            ${NF_Debugger_INCLUDE_DIRS}
            ${NF_Diagnostics_INCLUDE_DIRS}
            ${Graphics_Includes}
        )

    endif()


endmacro()

# Add common target sources to a specific CMake target
# To be called from target CMakeList.txt
macro(nf_add_common_sources target)

    target_sources(${target}.elf PUBLIC
    
        ${CMAKE_CURRENT_SOURCE_DIR}/target_common.c

        ${COMMON_PROJECT_SOURCES}
        ${WireProtocol_SOURCES}
        ${nanoHALCore_SOURCES}    
    )

    # sources specific to nanoBooter
    if(${target} STREQUAL ${NANOBOOTER_PROJECT_NAME})

        target_sources(${target}.elf PUBLIC
            
            ${CMAKE_CURRENT_SOURCE_DIR}/target_BlockStorage.c

            ${NANOBOOTER_PROJECT_SOURCES}
        )

    endif()

    # sources specific to nanoCRL
    if(${target} STREQUAL ${NANOCLR_PROJECT_NAME})

        target_sources(${target}.elf PUBLIC

            ${NANOCLR_PROJECT_SOURCES}

            # sources for nanoFramework libraries
            ${NF_CoreCLR_SOURCES}
            ${NF_Debugger_SOURCES}
            ${NF_Diagnostics_SOURCES}

            # ${NF_Network_SOURCES}
            ${mbedTLS_SOURCES}

            # sources for nanoFramework APIs
            ${NF_NativeAssemblies_SOURCES}

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
# going throuhg the directory to find if it's not empty takes a lot of time because it sweeps all files
# simplifying this now to speed up local builds
macro(nf_directory_exists_not_empty path pathExists)

    if(EXISTS "${path}")
        set(${pathExists} TRUE)
    else()
        set(${pathExists} FALSE)
    endif()

endmacro()

# define the base path for the library modules
macro(nf_set_base_path_for_libraries_modules path)
    set(BASE_PATH_FOR_CLASS_LIBRARIES_MODULES ${path})
endmacro()
