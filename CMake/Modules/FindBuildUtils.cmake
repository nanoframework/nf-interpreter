#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

function(NF_GENERATE_DFU_PACKAGE FILE1 ADDRESS1 FILE2 ADDRESS2 OUTPUTFILENAME)

    add_custom_command(
        TARGET ${NANOCLR_PROJECT_NAME}.elf POST_BUILD
        COMMAND ${TOOL_HEX2DFU_PREFIX}/hex2dfu 
        
        -b="${FILE1}" -a="${ADDRESS1}"
        -b="${FILE2}" -a="${ADDRESS2}"
        -o="${OUTPUTFILENAME}"
        
        WORKING_DIRECTORY ${TOOL_HEX2DFU_PREFIX} 
        
        COMMENT "exporting bin files to DFU image" 
    )

endfunction()

function(NF_GENERATE_BUILD_OUTPUT_FILES TARGET)

#    if(${VISUAL_STUDIO})
#        # CMAKE command add_custom_Command 'POST_BUILD' currently fails,  "CMD.EXE" not recognised
#        # Use the CopyBuildOutput.cmd in the VisualStudioDevelopment folder to manually perform the same function
#        return()
#    endif()

    # need to remove the .elf suffix from target name
    string(FIND ${TARGET} "." TARGET_EXTENSION_DOT_INDEX)
    string(SUBSTRING ${TARGET} 0 ${TARGET_EXTENSION_DOT_INDEX} TARGET_SHORT)

    set(TARGET_HEX_FILE ${PROJECT_BINARY_DIR}/${TARGET_SHORT}.hex)
    set(TARGET_S19_FILE ${PROJECT_BINARY_DIR}/${TARGET_SHORT}.s19)
    set(TARGET_BIN_FILE ${PROJECT_BINARY_DIR}/${TARGET_SHORT}.bin)
    set(TARGET_DUMP_FILE ${PROJECT_BINARY_DIR}/${TARGET_SHORT}.lst)

    if(CMAKE_BUILD_TYPE EQUAL "Release" OR CMAKE_BUILD_TYPE EQUAL "MinSizeRel")

        add_custom_command(TARGET ${TARGET_SHORT}.elf POST_BUILD
                # copy target image to other formats
                COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${TARGET_SHORT}.elf> ${TARGET_HEX_FILE}
                COMMAND ${CMAKE_OBJCOPY} -Osrec $<TARGET_FILE:${TARGET_SHORT}.elf> ${TARGET_S19_FILE}
                COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${TARGET_SHORT}.elf> ${TARGET_BIN_FILE}

                # copy target file to build folder (this is only usefull for debugging in VS Code because of path in launch.json)
                #COMMAND ${CMAKE_OBJCOPY} $<TARGET_FILE:${TARGET_SHORT}.elf> ${PROJECT_SOURCE_DIR}/build/${TARGET_SHORT}.elf

                COMMENT "Generate nanoBooter HEX and BIN files for deployment")

    else()

        add_custom_command(TARGET ${TARGET_SHORT}.elf POST_BUILD
                # copy target image to other formats
                COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${TARGET_SHORT}.elf> ${TARGET_HEX_FILE}
                COMMAND ${CMAKE_OBJCOPY} -Osrec $<TARGET_FILE:${TARGET_SHORT}.elf> ${TARGET_S19_FILE}
                COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${TARGET_SHORT}.elf> ${TARGET_BIN_FILE}

                # copy target file to build folder (this is only usefull for debugging in VS Code because of path in launch.json)
                #COMMAND ${CMAKE_OBJCOPY} $<TARGET_FILE:${TARGET_SHORT}.elf> ${PROJECT_SOURCE_DIR}/build/${TARGET_SHORT}.elf

                # dump target image as source code listing 
                # ONLY when DEBUG info is available, this is on 'Debug' and 'RelWithDebInfo'
                COMMAND ${CMAKE_OBJDUMP} -d -EL -S $<TARGET_FILE:${TARGET_SHORT}.elf> > ${TARGET_DUMP_FILE}

                COMMENT "Generate nanoBooter HEX and BIN files for deployment, LST file for debug")

    endif()

    nf_add_hex_bin_dump_targets(${TARGET})
        
    # add this to print the size of the output targets
    nf_print_size_of_targets(${TARGET})

endfunction()


#######################################################################################################################################
# this function sets the linker options AND a specific linker file (full path and name, including extension)
function(NF_SET_LINKER_OPTIONS_AND_FILE TARGET LINKER_FILE_NAME)

    get_target_property(TARGET_LD_FLAGS ${TARGET} LINK_FLAGS)
    if(TARGET_LD_FLAGS)
        set(TARGET_LD_FLAGS "-T${LINKER_FILE_NAME} ${TARGET_LD_FLAGS}")
    else()
        set(TARGET_LD_FLAGS "-T${LINKER_FILE_NAME}")
    endif()
    set_target_properties(${TARGET} PROPERTIES LINK_FLAGS ${TARGET_LD_FLAGS})

endfunction()
