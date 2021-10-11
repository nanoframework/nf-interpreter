#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# need to specify linker flags here
set(CMAKE_EXE_LINKER_FLAGS " -Wl,--print-memory-usage " CACHE INTERNAL "executable linker flags")

# TARGET parameter to set the target that's setting them for
# optional EXTRA_COMPILE_OPTIONS with compile options to be added
macro(nf_set_compile_options)

    # parse arguments
    cmake_parse_arguments(NFSCO "" "TARGET" "EXTRA_COMPILE_OPTIONS" ${ARGN})
    
    if(NOT NFSCO_TARGET OR "${NFSCO_TARGET}" STREQUAL "")
        message(FATAL_ERROR "Need to set TARGET argument when calling nf_set_compile_options()")
    endif()

    # include any extra options coming from any extra args?
    target_compile_options(${NFSCO_TARGET} PUBLIC ${NFSCO__EXTRA_COMPILE_OPTIONS} -Wall -Wextra -Werror -Wno-unused-parameter -Wshadow -Wimplicit-fallthrough -fshort-wchar -fno-builtin -fno-common -fno-exceptions -fcheck-new )

    # this series has FPU 
    target_compile_definitions(${NFSCO_TARGET} PUBLIC -DTARGET=esp32s2 -DUSE_FPU=TRUE -DPLATFORM_ESP32) 

endmacro()

# TARGET parameter to set the target that's setting them for
# optional EXTRA_LINK_FLAGS with link flags to be added
macro(nf_set_link_options)

    # parse arguments
    cmake_parse_arguments(NFSLO "" "TARGET;EXTRA_LINK_FLAGS" "" ${ARGN})
    
    if(NOT NFSLO_TARGET OR "${NFSLO_TARGET}" STREQUAL "")
        message(FATAL_ERROR "Need to set TARGET argument when calling nf_set_link_options()")
    endif()

    # set optimization linker flags for RELEASE and MinSizeRel
    if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
        set_property(TARGET ${NFSLO_TARGET} APPEND_STRING PROPERTY LINK_FLAGS " -Os ")
    endif()

    # include libraries in build
    nf_include_libraries_in_build(${NFSLO_TARGET})

    # set extra linker flags
    set_property(TARGET ${NFSLO_TARGET} APPEND_STRING PROPERTY LINK_FLAGS " ${NFSLO_EXTRA_LINK_FLAGS} ")
      
    # set optimization flags
    nf_set_optimization_options(${NFSLO_TARGET})

endmacro()
