#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

#################################################################
# WHEN ADDING A NEW TARGET add the appropriate GCC options below
#################################################################

set(CMAKE_C_FLAGS "-mthumb -mcpu=cortex-m0 -fno-builtin -std=c11 -g -Wall -ffunction-sections -fdata-sections -fomit-frame-pointer -mlong-calls -mabi=aapcs -fno-exceptions -fno-unroll-loops -ftree-vectorize -specs=nano.specs" CACHE INTERNAL "c compiler flags")
set(CMAKE_CXX_FLAGS "-mthumb -mcpu=cortex-m0 -std=c++11 -ffunction-sections -fdata-sections -fomit-frame-pointer -mlong-calls -mabi=aapcs -fno-exceptions -fno-unroll-loops -ftree-vectorize -specs=nano.specs" CACHE INTERNAL "cxx compiler flags")
set(CMAKE_ASM_FLAGS "-c -mthumb -mcpu=cortex-m0 -g -Wa,--no-warn -x assembler-with-cpp " CACHE INTERNAL "asm compiler flags")

set(CMAKE_EXE_LINKER_FLAGS "-mthumb -mcpu=cortex-m0 -static -mabi=aapcs -Wl,-gc-sections" CACHE INTERNAL "executable linker flags")
set(CMAKE_MODULE_LINKER_FLAGS "-mthumb -mcpu=cortex-m0 -specs=nano.specs " CACHE INTERNAL "module linker flags")
set(CMAKE_SHARED_LINKER_FLAGS "-mthumb -mcpu=cortex-m0 -specs=nano.specs " CACHE INTERNAL "shared linker flags")
