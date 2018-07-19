#
# Copyright (c) 2018 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# sanity check on FPU preference
if(USE_FPU)
    message(FATAL_ERROR "STM32L0 series doesn't have FPU. Need to set USE_FPU option to OFF.")
endif()

#################################################################
# WHEN ADDING A NEW SERIES add the appropriate GCC options bellow
#################################################################

# need to specify this for assembler
#####################################################################
# because nanoCLR is loaded the VTOR has to be init in ChibiOS CRT0
# this define has to be placed here because it's on an assembly file
#####################################################################
set(CMAKE_ASM_FLAGS " -mthumb -mcpu=cortex-m0plus -mfloat-abi=soft -mabi=aapcs -mtune=cortex-m0plus -x assembler-with-cpp -DCRT0_VTOR_INIT=TRUE" CACHE INTERNAL "asm compiler flags")

# need to specify linker flags here
set(CMAKE_EXE_LINKER_FLAGS " -Wl,--gc-sections -Wl,--no-wchar-size-warning -mthumb -mcpu=cortex-m0plus -mfloat-abi=soft -mabi=aapcs -mtune=cortex-m0plus -nostartfiles " CACHE INTERNAL "executable linker flags")


function(NF_SET_COMPILER_OPTIONS TARGET)

    # include any extra options comming from any extra args?
    target_compile_options(${TARGET} PUBLIC  ${ARGN} -mthumb -mcpu=cortex-m0plus -mfloat-abi=soft -mabi=aapcs -mtune=cortex-m0plus -nostdlib -Wall -Wextra -Werror -ffunction-sections -fshort-wchar -falign-functions=16 -fdata-sections -fno-builtin -fno-common -fomit-frame-pointer -mlong-calls -fdollars-in-identifiers -fno-exceptions -fno-unroll-loops -mstructure-size-boundary=8 -ffast-math -ftree-vectorize -fcheck-new )
            
endfunction()


function(NF_SET_LINKER_OPTIONS TARGET PRINTF_FLOAT_SUPPORT)

    # request to include support for float printf form newlib nano
    if(PRINTF_FLOAT_SUPPORT)
        set_property(TARGET ${TARGET} APPEND_STRING PROPERTY LINK_FLAGS " --specs=nano.specs -u _printf_float")
    else()
        set_property(TARGET ${TARGET} APPEND_STRING PROPERTY LINK_FLAGS " --specs=nano.specs ")
    endif()

    # set extra linker flags
    set_property(TARGET ${TARGET} APPEND_STRING PROPERTY LINK_FLAGS " ${ARGN}")

    # set optimization linker flags for RELEASE and MinSizeRel
    if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
        set_property(TARGET ${TARGET} APPEND_STRING PROPERTY LINK_FLAGS " -flto -fno-strict-aliasing ")
    endif()

    # set optimization flags
    nf_set_optimization_options(${TARGET})
    
    # set link map
    nf_set_link_map(${TARGET})

endfunction()
