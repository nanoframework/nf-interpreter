#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# need to specify this for assembler
set(CMAKE_ASM_FLAGS " -mthumb -mcpu=cortex-m4 -x assembler-with-cpp" CACHE INTERNAL "asm compiler flags")
set(CMAKE_C_FLAGS " -mthumb -mcpu=cortex-m4 -mfloat-abi=soft -mfpu=fpv4-sp-d16 -Wa,-mimplicit-it=thumb -Wall -Wextra -Werror -ffunction-sections -fshort-wchar -falign-functions=16 -fdata-sections -fno-builtin -fno-common -fomit-frame-pointer -mlong-calls -fdollars-in-identifiers -fno-exceptions -fno-unroll-loops -frounding-math -fsignaling-nans -ffloat-store -fno-math-errno -ftree-vectorize -fcheck-new " CACHE INTERNAL "asm compiler flags")
set(CMAKE_CXX_FLAGS " -mthumb -mcpu=cortex-m4 -mfloat-abi=soft -mfpu=fpv4-sp-d16 -Wa,-mimplicit-it=thumb -Wall -Wextra -Werror -ffunction-sections -fshort-wchar -falign-functions=16 -fdata-sections -fno-builtin -fno-common -fomit-frame-pointer -mlong-calls -fdollars-in-identifiers -fno-exceptions -fno-unroll-loops -frounding-math -fsignaling-nans -ffloat-store -fno-math-errno -ftree-vectorize -fcheck-new " CACHE INTERNAL "asm compiler flags")

# need to specify linker flags here
set(CMAKE_EXE_LINKER_FLAGS " -Wl,--gc-sections -Wl,--no-wchar-size-warning -Wl,--print-memory-usage -Wl,--no-warn-rwx-segments -mthumb -mcpu=cortex-m4 -mfloat-abi=soft -mfpu=fpv4-sp-d16 " CACHE INTERNAL "executable linker flags")


# TARGET parameter to set the target that's setting them for
# optional EXTRA_COMPILE_OPTIONS with compile options to be added
macro(nf_set_compile_options)

    # parse arguments
    cmake_parse_arguments(NFSCO "" "TARGET" "EXTRA_COMPILE_OPTIONS" ${ARGN})
    
    if(NOT NFSCO_TARGET OR "${NFSCO_TARGET}" STREQUAL "")
        message(FATAL_ERROR "Need to set TARGET argument when calling nf_set_compile_options()")
    endif()

    # include any extra options coming from any extra args?
    # can't include -Wundef because of MAXIM SDK
    target_compile_options(${NFSCO_TARGET} PUBLIC ${NFSCO_EXTRA_COMPILE_OPTIONS} -mthumb -mcpu=cortex-m4 -mfloat-abi=soft -mfpu=fpv4-sp-d16 -mfloat-abi=soft -Wa,-mimplicit-it=thumb -Wall -Wextra -Werror -Wshadow -Wimplicit-fallthrough -ffunction-sections -fshort-wchar -falign-functions=16 -fdata-sections -fno-builtin -fno-common -fomit-frame-pointer -mlong-calls -fdollars-in-identifiers -fno-exceptions -fno-unroll-loops -frounding-math -fsignaling-nans -ffloat-store -fno-math-errno -ftree-vectorize -fcheck-new )

    # enable:
    # - FPU 
    # - user TX file
    # - MAXIM target and revision
    target_compile_definitions(${NFSCO_TARGET} PUBLIC -DPLATFORM_ARM -DCORTEX_USE_FPU=TRUE -DTARGET=MAX78000 -DTARGET_REV=0x4131 -DFTHR_RevA -DTX_INCLUDE_USER_DEFINE_FILE) 

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
        set_property(TARGET ${NFSLO_TARGET} APPEND_STRING PROPERTY LINK_FLAGS " -Os -flto -fuse-linker-plugin ")
    endif()

    # request specs from newlib nano
    set_property(TARGET ${NFSLO_TARGET} APPEND_STRING PROPERTY LINK_FLAGS " --specs=nano.specs --specs=nosys.specs")
      
    # include libraries in build
    nf_include_libraries_in_build(${NFSLO_TARGET})

    # set extra linker flags
    set_property(TARGET ${NFSLO_TARGET} APPEND_STRING PROPERTY LINK_FLAGS " ${NFSLO_EXTRA_LINK_FLAGS} ")
 
    # set optimization flags
    nf_set_optimization_options(${NFSLO_TARGET})
    
endmacro()
