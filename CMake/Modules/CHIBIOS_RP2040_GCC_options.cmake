#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

#################################################################
# WHEN ADDING A NEW SERIES add the appropriate GCC options below
#################################################################

# RP2040: ARM Cortex-M0+ (ARMv6-M), no FPU, soft float

# need to specify this for assembler
set(CMAKE_ASM_FLAGS " -mthumb -mcpu=cortex-m0plus -mfloat-abi=soft -mabi=aapcs -x assembler-with-cpp -DCRT0_VTOR_INIT=TRUE" CACHE INTERNAL "asm compiler flags")

# need to specify linker flags here
set(CMAKE_EXE_LINKER_FLAGS " -Wl,--gc-sections -Wl,--no-wchar-size-warning -Wl,--print-memory-usage -Wl,--no-warn-rwx-segments -mthumb -mcpu=cortex-m0plus -mfloat-abi=soft -mabi=aapcs -nostartfiles " CACHE INTERNAL "executable linker flags")


# TARGET parameter to set the target that's setting them for
# optional EXTRA_COMPILE_OPTIONS with compile options to be added
macro(nf_set_compile_options)

    # parse arguments
    cmake_parse_arguments(NFSCO "" "TARGET" "EXTRA_COMPILE_OPTIONS" ${ARGN})
    
    if(NOT NFSCO_TARGET OR "${NFSCO_TARGET}" STREQUAL "")
        message(FATAL_ERROR "Need to set TARGET argument when calling nf_set_compile_options()")
    endif()

    # include any extra options coming from any extra args?
    target_compile_options(${NFSCO_TARGET} PUBLIC  ${NFSCO_EXTRA_COMPILE_OPTIONS} -mthumb -mcpu=cortex-m0plus -mabi=aapcs -nostdlib -Wall -Wextra -Werror -Wundef -Wshadow -Wimplicit-fallthrough $<$<COMPILE_LANGUAGE:CXX>:-Wno-cast-user-defined> -fshort-wchar -fno-builtin -fno-common -mno-long-calls -fno-exceptions -fcheck-new )

    # RP2040 has no FPU (Cortex-M0+)
    # Note: RP2040 is defined by ChibiOS board.h, no need to add it here
    target_compile_definitions(${NFSCO_TARGET} PUBLIC -DPLATFORM_ARM -DCORTEX_USE_FPU=FALSE -DUSE_FPU=FALSE)

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
    # NOTE: LTO is intentionally disabled for RP2040 (Cortex-M0+ / Thumb-1)
    # Also using -O2 instead of -Os: GCC 15 with -Os generates code with immediate offsets
    # exceeding Thumb-1 encoding limits ("invalid offset, value too big" assembler errors)
    if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
        set_property(TARGET ${NFSLO_TARGET} APPEND_STRING PROPERTY LINK_FLAGS " -O2 -fstrict-aliasing -fomit-frame-pointer -fno-unroll-loops -frounding-math -fsignaling-nans -ffloat-store -fno-math-errno -ftree-vectorize -fno-default-inline -finline-functions-called-once -fno-defer-pop ")
    endif()

    # request specs from newlib nano
    set_property(TARGET ${NFSLO_TARGET} APPEND_STRING PROPERTY LINK_FLAGS " --specs=nano.specs ")

    # include libraries in build
    nf_include_libraries_in_build(${NFSLO_TARGET})

    # set extra linker flags
    set_property(TARGET ${NFSLO_TARGET} APPEND_STRING PROPERTY LINK_FLAGS " ${NFSLO_EXTRA_LINK_FLAGS} ")

    # set optimization flags
    nf_set_optimization_options(${NFSLO_TARGET})

endmacro()

# Override nf_set_optimization_options for RP2040 to disable LTO and use -O2
# Cortex-M0+ Thumb-1 instructions have limited immediate offset range.
# GCC 15 with -Os or -flto generates code with offsets exceeding Thumb-1 limits.
# -O2 uses different register allocation that avoids oversized stack frames.
function(nf_set_optimization_options target)

    target_compile_options(${target} PRIVATE
        $<$<CONFIG:Debug>:-Og -ggdb>
        $<$<CONFIG:Release>:-O2>
        $<$<CONFIG:MinSizeRel>:-O2>
        $<$<CONFIG:RelWithDebInfo>:-O2 -ggdb>
    )

endfunction()
