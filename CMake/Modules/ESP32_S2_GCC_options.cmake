#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# need to specify this for assembler
set(CMAKE_ASM_FLAGS " -mthumb -mcpu=cortex-m0 -mfloat-abi=soft -mabi=aapcs -x assembler-with-cpp" CACHE INTERNAL "asm compiler flags")

# need to specify linker flags here
set(CMAKE_EXE_LINKER_FLAGS " -Wl,--gc-sections -Wl,--no-wchar-size-warning -Wl,--print-memory-usage -mthumb -mcpu=cortex-m0 -mfloat-abi=soft -mabi=aapcs -nostartfiles " CACHE INTERNAL "executable linker flags")


function(NF_SET_COMPILER_OPTIONS TARGET)

    # # include any extra options coming from any extra args?
    # target_compile_options(${TARGET} PUBLIC  ${ARGN} -mthumb -mcpu=cortex-m0 -mabi=aapcs -nostdlib -Wall -Wextra -Werror -Wundef -Wshadow -Wimplicit-fallthrough -fshort-wchar -fno-builtin -fno-common -mno-long-calls -fno-exceptions -fcheck-new )

    # need to set IDF target here
    target_compile_definitions(${TARGET} PUBLIC -DUSE_FPU=TRUE -DPLATFORM_ESP32 -DTARGET=esp32s2 -DCONFIG_IDF_TARGET_ESP32S2 )

endfunction()


function(NF_SET_LINKER_OPTIONS TARGET)

    # # request specs from newlib nano
    # set_property(TARGET ${TARGET} APPEND_STRING PROPERTY LINK_FLAGS " --specs=nano.specs ")

    # # set extra linker flags
    # set_property(TARGET ${TARGET} APPEND_STRING PROPERTY LINK_FLAGS " ${ARGN}")

    # # set optimization linker flags for RELEASE and MinSizeRel
    # if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    #     set_property(TARGET ${TARGET} APPEND_STRING PROPERTY LINK_FLAGS " -flto -Os -fstrict-aliasing -fomit-frame-pointer -fno-unroll-loops -frounding-math -fsignaling-nans -ffloat-store -fno-math-errno -ftree-vectorize -fno-default-inline -finline-functions-called-once -fno-defer-pop ")
    # endif()

    # # set optimization flags
    # nf_set_optimization_options(${TARGET})
    
    # # set link map
    # nf_set_link_map(${TARGET})

endfunction()
