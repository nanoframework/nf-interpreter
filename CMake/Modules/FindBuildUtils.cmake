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

function(NF_GENERATE_BUILD_OUTPUT_FILES)

    if(CMAKE_BUILD_TYPE EQUAL "Release")

        add_custom_command(TARGET ${NANOBOOTER_PROJECT_NAME}.elf POST_BUILD
                # copy target image to other formats
                COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${NANOBOOTER_PROJECT_NAME}.elf> ${NANOBOOTER_HEX_FILE}
                COMMAND ${CMAKE_OBJCOPY} -Osrec $<TARGET_FILE:${NANOBOOTER_PROJECT_NAME}.elf> ${NANOBOOTER_S19_FILE}
                COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${NANOBOOTER_PROJECT_NAME}.elf> ${NANOBOOTER_BIN_FILE}

                # copy target file to build folder (this is only usefull for debugging in VS Code because of path in launch.json)
                COMMAND ${CMAKE_OBJCOPY} $<TARGET_FILE:${NANOBOOTER_PROJECT_NAME}.elf> ${PROJECT_SOURCE_DIR}/build/${NANOBOOTER_PROJECT_NAME.elf}

                COMMENT "Generate nanoBooter HEX and BIN files for deployment")

        add_custom_command(TARGET ${NANOCLR_PROJECT_NAME}.elf POST_BUILD
                # copy target image to other formats
                COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${NANOCLR_PROJECT_NAME}.elf> ${NANOCLR_HEX_FILE}
                COMMAND ${CMAKE_OBJCOPY} -Osrec $<TARGET_FILE:${NANOCLR_PROJECT_NAME}.elf> ${NANOCLR_S19_FILE}
                COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${NANOCLR_PROJECT_NAME}.elf> ${NANOCLR_BIN_FILE}

                # copy target file to build folder (this is only usefull for debugging in VS Code because of path in launch.json)
                COMMAND ${CMAKE_OBJCOPY} $<TARGET_FILE:${NANOCLR_PROJECT_NAME}.elf> ${PROJECT_SOURCE_DIR}/build/${NANOCLR_PROJECT_NAME}.elf

                COMMENT "Generate nanoCLR HEX and BIN files for deployment")

    else()

        add_custom_command(TARGET ${NANOBOOTER_PROJECT_NAME}.elf POST_BUILD
                # copy target image to other formats
                COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${NANOBOOTER_PROJECT_NAME}.elf> ${NANOBOOTER_HEX_FILE}
                COMMAND ${CMAKE_OBJCOPY} -Osrec $<TARGET_FILE:${NANOBOOTER_PROJECT_NAME}.elf> ${NANOBOOTER_S19_FILE}
                COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${NANOBOOTER_PROJECT_NAME}.elf> ${NANOBOOTER_BIN_FILE}

                # copy target file to build folder (this is only usefull for debugging in VS Code because of path in launch.json)
                COMMAND ${CMAKE_OBJCOPY} $<TARGET_FILE:${NANOBOOTER_PROJECT_NAME}.elf> ${PROJECT_SOURCE_DIR}/build/${NANOBOOTER_PROJECT_NAME}.elf

                # dump target image as source code listing
                COMMAND ${CMAKE_OBJDUMP} -d -EL -S $<TARGET_FILE:${NANOBOOTER_PROJECT_NAME}.elf> > ${NANOBOOTER_DUMP_FILE}

                COMMENT "Generate nanoBooter HEX and BIN files for deployment, LST file for debug")

        add_custom_command(TARGET ${NANOCLR_PROJECT_NAME}.elf POST_BUILD
                # copy target image to other formats
                COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${NANOCLR_PROJECT_NAME}.elf> ${NANOCLR_HEX_FILE}
                COMMAND ${CMAKE_OBJCOPY} -Osrec $<TARGET_FILE:${NANOCLR_PROJECT_NAME}.elf> ${NANOCLR_S19_FILE}
                COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${NANOCLR_PROJECT_NAME}.elf> ${NANOCLR_BIN_FILE}

                # copy target file to build folder (this is only usefull for debugging in VS Code because of path in launch.json)
                COMMAND ${CMAKE_OBJCOPY} $<TARGET_FILE:${NANOCLR_PROJECT_NAME}.elf> ${PROJECT_SOURCE_DIR}/build/${NANOCLR_PROJECT_NAME}.elf

                # dump target image as source code listing
                COMMAND ${CMAKE_OBJDUMP} -d -EL -S $<TARGET_FILE:${NANOCLR_PROJECT_NAME}.elf> > ${NANOCLR_DUMP_FILE}
            
                COMMENT "Generate nanoCLR HEX and BIN files for deployment, LST file for debug")

    endif()

endfunction()
