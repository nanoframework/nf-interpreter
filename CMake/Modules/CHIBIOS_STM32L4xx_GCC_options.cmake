#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

#################################################################
# WHEN ADDING A NEW SERIES add the appropriate GCC options below
#################################################################

# need to specify this for assembler
set(CMAKE_ASM_FLAGS " -mthumb -mcpu=cortex-m4 -x assembler-with-cpp" CACHE INTERNAL "asm compiler flags")

# need to specify linker flags here
set(CMAKE_EXE_LINKER_FLAGS " -Wl,--gc-sections -Wl,--no-wchar-size-warning -Wl,--print-memory-usage -mthumb -mcpu=cortex-m4 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mabi=aapcs -nostartfiles " CACHE INTERNAL "executable linker flags")


function(nf_set_compile_options TARGET)

    # include any extra options coming from any extra args?
    target_compile_options(${TARGET} PUBLIC  ${ARGN} -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mabi=aapcs -nostdlib -Wall -Wextra -Werror -Wundef -Wshadow -Wimplicit-fallthrough -ffunction-sections -fshort-wchar -falign-functions=16 -fdata-sections -fno-builtin -fno-common -fomit-frame-pointer -mlong-calls -fdollars-in-identifiers -fno-exceptions -fno-unroll-loops -frounding-math -fsignaling-nans -ffloat-store -fno-math-errno -ftree-vectorize -fcheck-new )

    # this series has FPU 
    target_compile_definitions(${TARGET} PUBLIC -DPLATFORM_ARM -DCORTEX_USE_FPU=TRUE -DUSE_FPU=TRUE) 

endfunction()


# TARGET parameter to set the target that's setting them for
# optional SET_LINKMAP to set link map on the target
# optional EXTRA_LINK_FLAGS with link flags to be added
macro(nf_set_linker_options)

    # parse arguments
    cmake_parse_arguments(_ "SET_LINKMAP" "TARGET;EXTRA_LINK_FLAGS" "" ${ARGN})
    
    if(NOT __TARGET OR "${__TARGET}" STREQUAL "")
        message(FATAL_ERROR "Need to set TARGET argument when calling nf_set_compile_definitions()")
    endif()

    # request specs from newlib nano
    set_property(TARGET ${__TARGET} APPEND_STRING PROPERTY LINK_FLAGS " --specs=nano.specs ")

    # set extra linker flags
    set_property(TARGET ${__TARGET} APPEND_STRING PROPERTY LINK_FLAGS " ${__EXTRA_LINK_FLAGS}")

    # set optimization linker flags for RELEASE and MinSizeRel
    if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
        set_property(TARGET ${__TARGET} APPEND_STRING PROPERTY LINK_FLAGS " -flto -fuse-linker-plugin -Os ")
    endif()
      
    # set optimization flags
    nf_set_optimization_options(${__TARGET})
    
    # set link map, if requested
    if(${__SET_LINKMAP})
        nf_set_link_map(${__TARGET})
    endif()
  
endmacro()
