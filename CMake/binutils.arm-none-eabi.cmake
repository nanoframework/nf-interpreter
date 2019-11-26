#
# Copyright (c) 2019 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

function(NF_ADD_HEX_BIN_DUMP_TARGETS TARGET)
    if(EXECUTABLE_OUTPUT_PATH)
        set(FILENAME "${EXECUTABLE_OUTPUT_PATH}/${TARGET}")
    else()
        set(FILENAME "${TARGET}")
    endif()

	#get_filename_component(FNSHORT ${FILENAME} NAME_WE)
	string(REGEX REPLACE "\\.[^.]*$" "" FNSHORT ${FILENAME})

    # add targets for HEX, BIN and S19 formats with no output so they will always be built
    add_custom_target(${TARGET}.hex DEPENDS ${TARGET} COMMAND ${CMAKE_OBJCOPY} -Oihex ${FILENAME} ${FNSHORT}.hex)
    add_custom_target(${TARGET}.s19 DEPENDS ${TARGET} COMMAND ${CMAKE_OBJCOPY} -Osrec ${FILENAME} ${FNSHORT}.s19)
    add_custom_target(${TARGET}.bin DEPENDS ${TARGET} COMMAND ${CMAKE_OBJCOPY} -Obinary ${FILENAME} ${FNSHORT}.bin)
    add_custom_target(${TARGET}.dump DEPENDS ${TARGET} COMMAND ${CMAKE_OBJDUMP} -d -EL -S ${FILENAME} ${FNSHORT}.dump)
endfunction()


function(NF_PRINT_SIZE_OF_TARGETS TARGET)
    if(EXECUTABLE_OUTPUT_PATH)
      set(FILENAME "${EXECUTABLE_OUTPUT_PATH}/${TARGET}")
    else()
      set(FILENAME "${TARGET}")
    endif()
    add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND ${CMAKE_SIZE} -A -x ${FILENAME})
endfunction()


function(NF_SET_LINKER_FILE TARGET LINKER_FILE_NAME)

    # set linker file name
    set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "-T${LINKER_FILE_NAME}")

endfunction()
