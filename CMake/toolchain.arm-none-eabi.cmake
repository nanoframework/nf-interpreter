#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(CMakeForceCompiler)

# set toolchain directories
set(TOOLCHAIN_BIN_DIR ${TOOLCHAIN_PREFIX}/bin)
set(TOOLCHAIN_INC_DIR ${TOOLCHAIN_PREFIX}/arm-none-eabi/include)
set(TOOLCHAIN_LIB_DIR ${TOOLCHAIN_PREFIX}/arm-none-eabi/lib)

# the name of the operating system for which CMake is to build
set(CMAKE_SYSTEM_NAME Generic)

# name of the CPU CMake is building for
set(CMAKE_SYSTEM_PROCESSOR ARM)

# macro to setup compilers
macro(SET_COMPILER_VAR var name)
   find_program(CMAKE_${var} arm-none-eabi-${name} HINTS ${TOOLCHAIN_BIN_DIR} DOC "${name} tool")
endmacro()

# setup C compiler
if(NOT CMAKE_C_COMPILER)
    SET_COMPILER_VAR(C_COMPILER gcc)
endif()

# setup C++ compiler
if(NOT CMAKE_CXX_COMPILER)
    SET_COMPILER_VAR(CXX_COMPILER g++)
endif()

# setup Assembler compiler
if(NOT CMAKE_ASM_COMPILER)
    SET_COMPILER_VAR(ASM_COMPILER gcc)
    SET_COMPILER_VAR(ASM-ATT_COMPILER as)
endif()

# other toolchain configurations
set(CMAKE_OBJCOPY ${TOOLCHAIN_BIN_DIR}/arm-none-eabi-objcopy CACHE INTERNAL "objcopy tool")
set(CMAKE_OBJDUMP ${TOOLCHAIN_BIN_DIR}/arm-none-eabi-objdump CACHE INTERNAL "objdump tool")
set(CMAKE_SIZE ${TOOLCHAIN_BIN_DIR}/arm-none-eabi-size CACHE INTERNAL "size tool")
set(CMAKE_DEBUGER ${TOOLCHAIN_BIN_DIR}/arm-none-eabi-gdb CACHE INTERNAL "debuger")

# root paths to search on the filesystem for cross-compiling
set(CMAKE_FIND_ROOT_PATH ${TOOLCHAIN_PREFIX}/arm-none-eabi ${EXTRA_FIND_PATH})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# set required C and C++ standard for ALL targets
set(CMAKE_C_STANDARD 11 CACHE INTERNAL "C standard for all targets")
set(CMAKE_CXX_STANDARD 11 CACHE INTERNAL "C++ standard for all targets")

# set all C and C++ extensions to be OFF on ALL targets
# this forces the use of -std=c11 and -std=c++11 instead of -std=gnu11 and -std=gnu++11
set(CMAKE_C_EXTENSIONS OFF CACHE INTERNAL "C compiler extensions OFF")
set(CMAKE_CXX_EXTENSIONS OFF CACHE INTERNAL "C++ compiler extensions OFF")

# Perform compiler test with static library
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
