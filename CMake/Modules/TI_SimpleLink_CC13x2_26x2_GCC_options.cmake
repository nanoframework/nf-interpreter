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
set(CMAKE_EXE_LINKER_FLAGS " -mfpu=fpv4-sp-d16 -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -mcpu=cortex-m4 -nostartfiles -static -Wl,--gc-sections -L${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/source -L${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/kernel/tirtos/packages -L${PROJECT_BINARY_DIR}/SimpleLinkCC13x2_26x2SDK_Source/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/lib/thumb/v7e-m/hard -Wl,--gc-sections -Wl,--no-wchar-size-warning -Wl,--print-memory-usage -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Wall " CACHE INTERNAL "executable linker flags")


function(NF_SET_COMPILER_OPTIONS TARGET)

    # include any extra options coming from any extra args?
    target_compile_options(${TARGET} PUBLIC  ${ARGN} -march=armv7e-m -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mabi=aapcs -gstrict-dwarf -nostdlib -Wall -w -static -ffunction-sections -falign-functions=16 -fdata-sections -fno-builtin -fno-common -fsingle-precision-constant -fomit-frame-pointer -mlong-calls -fdollars-in-identifiers -fno-exceptions -fno-unroll-loops -ffast-math -ftree-vectorize -fcheck-new -fno-rtti -fno-use-cxa-atexit -fno-threadsafe-statics)
  
    # this series has FPU 
    target_compile_definitions(${TARGET} PUBLIC -DCORTEX_USE_FPU=TRUE) 

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
