#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

#################################################################
# WHEN ADDING A NEW SERIES add the appropriate GCC options bellow
#################################################################

# need to specify this for assembler
set(CMAKE_ASM_FLAGS " -mthumb -mcpu=cortex-m0 -mfloat-abi=soft -mabi=aapcs -x assembler-with-cpp" CACHE INTERNAL "asm compiler flags")

# need to specify linker flags here
set(CMAKE_EXE_LINKER_FLAGS " -Wl,--gc-sections -Wl,--no-wchar-size-warning -Wl,--print-memory-usage -mthumb -mcpu=cortex-m0 -mfloat-abi=soft -mabi=aapcs -nostartfiles " CACHE INTERNAL "executable linker flags")


function(NF_SET_COMPILER_OPTIONS TARGET)

    # include any extra options comming from any extra args?
    target_compile_options(${TARGET} PUBLIC  ${ARGN} -mthumb -mcpu=cortex-m0 -mabi=aapcs -nostdlib -Wall -Wextra -Werror -ffunction-sections -fshort-wchar -falign-functions=16 -fdata-sections -fno-builtin -fno-common -fomit-frame-pointer -mlong-calls -fdollars-in-identifiers -fno-exceptions -fno-unroll-loops -mstructure-size-boundary=8 -frounding-math -fsignaling-nans -ffloat-store -fno-math-errno -ftree-vectorize -fcheck-new )

    # this series doesn't have FPU 
    target_compile_definitions(${TARGET} PUBLIC -DCORTEX_USE_FPU=FALSE)

endfunction()


function(NF_SET_LINKER_OPTIONS TARGET)

    # request specs from newlib nano
    set_property(TARGET ${TARGET} APPEND_STRING PROPERTY LINK_FLAGS " --specs=nano.specs ")

    # set extra linker flags
    set_property(TARGET ${TARGET} APPEND_STRING PROPERTY LINK_FLAGS " ${ARGN}")

    # set optimization linker flags for RELEASE and MinSizeRel
    if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
        set_property(TARGET ${TARGET} APPEND_STRING PROPERTY LINK_FLAGS " -flto -fuse-linker-plugin -Os")
    endif()

    # set optimization flags
    nf_set_optimization_options(${TARGET})
    
    # set link map
    nf_set_link_map(${TARGET})

endfunction()
