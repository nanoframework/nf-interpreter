#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(binutils.common)

function(nf_add_hex_bin_dump_targets target)
    if(EXECUTABLE_OUTPUT_PATH)
        set(FILENAME "${EXECUTABLE_OUTPUT_PATH}/${target}")
    else()
        set(FILENAME "${target}")
    endif()

	#get_filename_component(FNSHORT ${FILENAME} NAME_WE)
	string(REGEX REPLACE "\\.[^.]*$" "" FNSHORT ${FILENAME})

    # add targets for HEX, BIN and S19 formats with no output so they will always be built
    add_custom_target(${target}.hex DEPENDS ${target} COMMAND ${CMAKE_OBJCOPY} -Oihex ${FILENAME} ${FNSHORT}.hex)
    add_custom_target(${target}.s19 DEPENDS ${target} COMMAND ${CMAKE_OBJCOPY} -Osrec ${FILENAME} ${FNSHORT}.s19)
    add_custom_target(${target}.bin DEPENDS ${target} COMMAND ${CMAKE_OBJCOPY} -Obinary ${FILENAME} ${FNSHORT}.bin)
    add_custom_target(${target}.dump DEPENDS ${target} COMMAND ${CMAKE_OBJDUMP} -d -EL -S ${FILENAME} ${FNSHORT}.dump)
endfunction()


function(nf_print_target_size target)
    if(EXECUTABLE_OUTPUT_PATH)
      set(FILENAME "${EXECUTABLE_OUTPUT_PATH}/${target}")
    else()
      set(FILENAME "${target}")
    endif()
    add_custom_command(TARGET ${target} POST_BUILD COMMAND ${CMAKE_SIZE} -A -x ${FILENAME})
endfunction()


function(nf_set_linker_file target linker_file_name)

    # set linker file name
    set_target_properties(${target} PROPERTIES LINK_FLAGS "-T${linker_file_name}")

endfunction()
