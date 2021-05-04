#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

#################################################################
# WHEN ADDING A NEW SERIES add the appropriate GCC options below
#################################################################

# need to specify this for assembler
set(CMAKE_ASM_FLAGS " -mthumb -mcpu=cortex-m4 -x assembler-with-cpp" CACHE INTERNAL "asm compiler flags")
set(CMAKE_C_FLAGS " -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mabi=aapcs -Wall -Wextra -Werror -ffunction-sections -fshort-wchar -falign-functions=16 -fdata-sections -fno-builtin -fno-common -fomit-frame-pointer -mlong-calls -fdollars-in-identifiers -fno-exceptions -fno-unroll-loops -frounding-math -fsignaling-nans -ffloat-store -fno-math-errno -ftree-vectorize -fcheck-new " CACHE INTERNAL "asm compiler flags")
set(CMAKE_CXX_FLAGS " -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mabi=aapcs -Wall -Wextra -Werror -ffunction-sections -fshort-wchar -falign-functions=16 -fdata-sections -fno-builtin -fno-common -fomit-frame-pointer -mlong-calls -fdollars-in-identifiers -fno-exceptions -fno-unroll-loops -frounding-math -fsignaling-nans -ffloat-store -fno-math-errno -ftree-vectorize -fcheck-new " CACHE INTERNAL "asm compiler flags")

# need to specify linker flags here
set(CMAKE_EXE_LINKER_FLAGS " -Wl,--gc-sections -Wl,--no-wchar-size-warning -Wl,--print-memory-usage -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mabi=aapcs " CACHE INTERNAL "executable linker flags")


function(NF_SET_COMPILER_OPTIONS TARGET)

    # include any extra options coming from any extra args?
    target_compile_options(${TARGET} PUBLIC  ${ARGN} -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mabi=aapcs -Wall -Wextra -Werror -ffunction-sections -fshort-wchar -falign-functions=16 -fdata-sections -fno-builtin -fno-common -fomit-frame-pointer -mlong-calls -fdollars-in-identifiers -fno-exceptions -fno-unroll-loops -frounding-math -fsignaling-nans -ffloat-store -fno-math-errno -ftree-vectorize -fcheck-new )

    # enable:
    # - FPU 
    # - use LL drivers
    target_compile_definitions(${TARGET} PUBLIC -DCORTEX_USE_FPU=TRUE -DUSE_FULL_LL_DRIVER )

    # set Azure RTOS ticks
    target_compile_definitions(${TARGET} PUBLIC -DTX_TIMER_TICKS_PER_SECOND=10000) 

endfunction()


function(NF_SET_LINKER_OPTIONS TARGET)

    # request specs from newlib nano
    set_property(TARGET ${TARGET} APPEND_STRING PROPERTY LINK_FLAGS " --specs=nano.specs --specs=nosys.specs")

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
