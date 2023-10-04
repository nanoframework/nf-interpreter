#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(binutils.common)

function(nf_print_target_size target)
    if(EXECUTABLE_OUTPUT_PATH)
      set(FILENAME "${EXECUTABLE_OUTPUT_PATH}/${target}")
    else()
      set(FILENAME "${target}")
    endif()
    add_custom_command(TARGET ${target} POST_BUILD COMMAND ${CMAKE_SIZE_UTIL} -A -x ${FILENAME})
endfunction()

function(nf_set_linker_file target linker_file_name)

    # set linker file name
    set_target_properties(${target} PROPERTIES LINK_FLAGS "-T${linker_file_name}")

endfunction()
