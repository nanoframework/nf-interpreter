#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

#################################################################
# WHEN ADDING A NEW SERIES add the appropriate GCC options bellow
#################################################################

# need to specify this for assembler
set(CMAKE_ASM_FLAGS " -mthumb -mcpu=cortex-m7 -x assembler-with-cpp" CACHE INTERNAL "asm compiler flags")

# need to specify linker flags here
set(CMAKE_EXE_LINKER_FLAGS " -Wl,--gc-sections -Wl,--no-wchar-size-warning -mthumb -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mabi=aapcs -nostartfiles " CACHE INTERNAL "executable linker flags")


function(nf_set_compile_options target)

    # include any extra options comming from any extra args?
    # TODO: removed -Wundef until fix with FatFS is merged (https://github.com/abbrev/fatfs/pull/8)
    target_compile_options(${target} PUBLIC  ${ARGN} -mthumb -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mabi=aapcs -nostdlib -Wall -Wextra -Werror -ffunction-sections -fshort-wchar -falign-functions=16 -fdata-sections -fno-builtin -fno-common -fomit-frame-pointer -mlong-calls -fdollars-in-identifiers -fno-exceptions -fno-unroll-loops -frounding-math -fsignaling-nans -ffloat-store -fno-math-errno -ftree-vectorize -fcheck-new )

    # this series has FPU 
    target_compile_definitions(${target} PUBLIC -DPLATFORM_ARM -DCORTEX_USE_FPU=TRUE -DUSE_FPU=TRUE) 

endfunction()


# TARGET parameter to set the target that's setting them for
# optional EXTRA_LINK_FLAGS with link flags to be added
macro(nf_set_linker_options)

    # parse arguments
    cmake_parse_arguments(_ "" "TARGET;EXTRA_LINK_FLAGS" "" ${ARGN})
    
    if(NOT __TARGET OR "${__TARGET}" STREQUAL "")
        message(FATAL_ERROR "Need to set TARGET argument when calling nf_set_compile_definitions()")
    endif()

    # request specs from newlib nano
    set_property(TARGET ${__TARGET} APPEND_STRING PROPERTY LINK_FLAGS " --specs=nano.specs --specs=nosys.specs -Xlinker --gc-sections -Xlinker --sort-section=alignment -Xlinker -print-memory-usage")

    # set extra linker flags
    set_property(TARGET ${__TARGET} APPEND_STRING PROPERTY LINK_FLAGS " ${__EXTRA_LINK_FLAGS}")

    # set optimization linker flags for RELEASE and MinSizeRel
    if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
        set_property(TARGET ${__TARGET} APPEND_STRING PROPERTY LINK_FLAGS " -flto -fuse-linker-plugin -Os ")
    endif()
       
    # set optimization flags
    nf_set_optimization_options(${__TARGET})

endmacro()
