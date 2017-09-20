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
