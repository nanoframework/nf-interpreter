#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(FetchContent)
FetchContent_GetProperties(simplelinkcc13xx_26xxsdk)

#################################################################
# WHEN ADDING A NEW SERIES add the appropriate GCC options below
#################################################################

# need to specify this for assembler
set(CMAKE_ASM_FLAGS " -mthumb -mcpu=cortex-m4 -x assembler-with-cpp" CACHE INTERNAL "asm compiler flags")

# need to specify linker flags here
set(CMAKE_EXE_LINKER_FLAGS " -mfpu=fpv4-sp-d16 -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -mcpu=cortex-m4 -nostartfiles -static -Wl,--gc-sections -L${simplelinkcc13xx_26xxsdk_SOURCE_DIR}/source -L${simplelinkcc13xx_26xxsdk_SOURCE_DIR}/kernel/tirtos/packages -L${simplelinkcc13xx_26xxsdk_SOURCE_DIR}/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/lib/thumb/v7e-m/hard -Wl,--gc-sections -Wl,--no-wchar-size-warning -Wl,--print-memory-usage -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Wall " CACHE INTERNAL "executable linker flags")


# TARGET parameter to set the target that's setting them for
# optional EXTRA_COMPILE_OPTIONS with compile options to be added
macro(nf_set_compile_options)

    # parse arguments
    cmake_parse_arguments(NFSCO "" "TARGET" "EXTRA_COMPILE_OPTIONS" ${ARGN})
    
    if(NOT NFSCO_TARGET OR "${NFSCO_TARGET}" STREQUAL "")
        message(FATAL_ERROR "Need to set TARGET argument when calling nf_set_compile_options()")
    endif()


    # include any extra options coming from any extra args?
    target_compile_options(${NFSCO_TARGET} PUBLIC  ${NFSCO__EXTRA_COMPILE_OPTIONS} -march=armv7e-m -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mabi=aapcs -gstrict-dwarf -nostdlib -Wall -Wextra -Werror -Wundef -Wshadow -Wimplicit-fallthrough -w -static -ffunction-sections -falign-functions=16 -fdata-sections -fno-builtin -fno-common -fsingle-precision-constant -fomit-frame-pointer -mlong-calls -fdollars-in-identifiers -fno-exceptions -fno-unroll-loops -ffast-math -ftree-vectorize -fcheck-new -fno-rtti -fno-use-cxa-atexit -fno-threadsafe-statics)
  
    # this series has FPU 
    target_compile_definitions(${NFSCO_TARGET} PUBLIC -DPLATFORM_ARM -DCORTEX_USE_FPU=TRUE -DUSE_FPU=TRUE) 

endmacro()


# TARGET parameter to set the target that's setting them for
# optional EXTRA_LINK_FLAGS with link flags to be added
macro(nf_set_link_options)

    # parse arguments
    cmake_parse_arguments(NFSLO "" "TARGET;EXTRA_LINK_FLAGS" "" ${ARGN})
    
    if(NOT NFSLO_TARGET OR "${NFSLO_TARGET}" STREQUAL "")
        message(FATAL_ERROR "Need to set TARGET argument when calling nf_set_link_options()")
    endif()

    # request specs from newlib nano
    set_property(TARGET ${NFSLO_TARGET} APPEND_STRING PROPERTY LINK_FLAGS " --specs=nano.specs ")

    # set optimization linker flags for RELEASE and MinSizeRel
    if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
        set_property(TARGET ${NFSLO_TARGET} APPEND_STRING PROPERTY LINK_FLAGS " -flto -fuse-linker-plugin -Os ")
    endif()

    # include libraries in build
    nf_include_libraries_in_build(${NFSLO_TARGET})

    # set extra linker flags
    set_property(TARGET ${NFSLO_TARGET} APPEND_STRING PROPERTY LINK_FLAGS " ${NFSLO_EXTRA_LINK_FLAGS} ")
      
    # set optimization flags
    nf_set_optimization_options(${NFSLO_TARGET})
 
endmacro()
