#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

#################################################################
# WHEN ADDING A NEW SERIES add the appropriate GCC options bellow
#################################################################

set(CMAKE_C_FLAGS "-mthumb -fno-builtin -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=softfp -Wall -std=c11 -w -ffunction-sections -fshort-wchar -falign-functions=16 -fdata-sections -fsingle-precision-constant -fomit-frame-pointer -mlong-calls -fdollars-in-identifiers -mabi=aapcs -fno-exceptions -fno-unroll-loops -mstructure-size-boundary=8 -ffast-math -ftree-vectorize -specs=nano.specs" CACHE INTERNAL "c compiler flags")
set(CMAKE_CXX_FLAGS "-mthumb -fno-builtin -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=softfp -Wall -std=c++11 -w -fcheck-new -fno-rtti -falign-functions=16 -fsingle-precision-constant -fno-use-cxa-atexit -fno-threadsafe-statics -ffunction-sections -fshort-wchar -fdata-sections -fomit-frame-pointer -mlong-calls -fdollars-in-identifiers -mabi=aapcs -fno-exceptions -fno-unroll-loops -mstructure-size-boundary=8 -ffast-math -ftree-vectorize -specs=nano.specs" CACHE INTERNAL "cxx compiler flags")
set(CMAKE_ASM_FLAGS "-mthumb -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=softfp -x assembler-with-cpp" CACHE INTERNAL "asm compiler flags")

set(CMAKE_EXE_LINKER_FLAGS " -Wl,--gc-sections -mthumb -Wl,--no-wchar-size-warning -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=softfp -mabi=aapcs" CACHE INTERNAL "executable linker flags")
set(CMAKE_MODULE_LINKER_FLAGS " -mthumb -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=softfp -mabi=aapcs" CACHE INTERNAL "module linker flags")
set(CMAKE_SHARED_LINKER_FLAGS " -mthumb -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=softfp -mabi=aapcs" CACHE INTERNAL "shared linker flags")
