#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

INCLUDE(CMakeForceCompiler)
# if( DEFINED CMAKE_CROSSCOMPILING )
#     # subsequent toolchain loading is not really needed
#     return()
# endif()

# check for toolchain path
if(NOT TOOLCHAIN_PREFIX)
     message(STATUS "\n-- ########################################\nNo TOOLCHAIN_PREFIX specified, need one!\nCall CMake with -DTOOLCHAIN_PREFIX=\"<path_to_your_gcc_toolchain>\"\n specifing the path to your GCC toolchain (ex: E:/GNU_Tools_ARM_Embedded/5_4_2016q3)")
     message(STATUS "\nNOTE: mind the forward slash in the path, without trailing slash.)")
     message(STATUS "########################################\n\n")
     message(FATAL_ERROR "No TOOLCHAIN_PREFIX specified")
endif()

# set toolchain directories
set(TOOLCHAIN_BIN_DIR ${TOOLCHAIN_PREFIX}/bin)
set(TOOLCHAIN_INC_DIR ${TOOLCHAIN_PREFIX}/arm-none-eabi/include)
set(TOOLCHAIN_LIB_DIR ${TOOLCHAIN_PREFIX}/arm-none-eabi/lib)

# the name of the operating system for which CMake is to build
set(CMAKE_SYSTEM_NAME Generic)

# name of the CPU CMake is building for
set(CMAKE_SYSTEM_PROCESSOR arm)

# macro to setup compilers
macro(SET_COMPILER_VAR var name)
   find_program(CMAKE_${var} arm-none-eabi-${name} HINTS ${TOOLCHAIN_BIN_DIR} DOC "${name} tool")
endmacro()

# setup C compiler
if(NOT CMAKE_C_COMPILER)
    SET_COMPILER_VAR(C_COMPILER gcc)
   if(CMAKE_C_COMPILER)
      CMAKE_FORCE_C_COMPILER(${CMAKE_C_COMPILER} GNU)
   endif()
endif()
SET_COMPILER_VAR(C_COMPILER gcc)

# setup C++ compiler
if(NOT CMAKE_CXX_COMPILER)
    SET_COMPILER_VAR(CXX_COMPILER g++)
   if(CMAKE_CXX_COMPILER)
      CMAKE_FORCE_CXX_COMPILER(${CMAKE_CXX_COMPILER} GNU)
   endif()
endif()

# setup Assembler compiler
SET_COMPILER_VAR(ASM-ATT_COMPILER as)
 
# other toolchain configurations  
set(CMAKE_OBJCOPY ${TOOLCHAIN_BIN_DIR}/arm-none-eabi-objcopy CACHE INTERNAL "objcopy tool")
set(CMAKE_OBJDUMP ${TOOLCHAIN_BIN_DIR}/arm-none-eabi-objdump CACHE INTERNAL "objdump tool")
set(CMAKE_SIZE ${TOOLCHAIN_BIN_DIR}/arm-none-eabi-size CACHE INTERNAL "size tool")
set(CMAKE_DEBUGER ${TOOLCHAIN_BIN_DIR}/arm-none-eabi-gdb CACHE INTERNAL "debuger")

# set debug flags
set(CMAKE_C_FLAGS_DEBUG "-Og -femit-class-debug-always -g3 -ggdb" CACHE INTERNAL "c compiler flags debug")
set(CMAKE_CXX_FLAGS_DEBUG "-Og -femit-class-debug-always -g3 -ggdb" CACHE INTERNAL "cxx compiler flags debug")
set(CMAKE_ASM_FLAGS_DEBUG "-g3 -ggdb" CACHE INTERNAL "asm compiler flags debug")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "" CACHE INTERNAL "linker flags debug")

# set release flags
set(CMAKE_C_FLAGS_RELEASE "-Os -flto" CACHE INTERNAL "c compiler flags release")
set(CMAKE_CXX_FLAGS_RELEASE "-Os -flto" CACHE INTERNAL "cxx compiler flags release")
set(CMAKE_ASM_FLAGS_RELEASE "" CACHE INTERNAL "asm compiler flags release")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-flto" CACHE INTERNAL "linker flags release")

# root paths to search on the filesystem for cross-compiling
set(CMAKE_FIND_ROOT_PATH ${TOOLCHAIN_PREFIX}/arm-none-eabi ${EXTRA_FIND_PATH})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

function(MBED_ADD_HEX_BIN_DUMP_TARGETS TARGET)
    if(EXECUTABLE_OUTPUT_PATH)
        set(FILENAME "${EXECUTABLE_OUTPUT_PATH}/${TARGET}")
    else()
        set(FILENAME "${TARGET}")
    endif()

    # add targets for HEX, BIN and S19 formats with no output so they will always be built
    add_custom_target(${TARGET}.hex DEPENDS ${TARGET} COMMAND ${CMAKE_OBJCOPY} -Oihex ${FILENAME} ${FILENAME}.hex)
    add_custom_target(${TARGET}.s19 DEPENDS ${TARGET} COMMAND ${CMAKE_OBJCOPY} -Osrec ${FILENAME} ${FILENAME}.s19)
    add_custom_target(${TARGET}.bin DEPENDS ${TARGET} COMMAND ${CMAKE_OBJCOPY} -Obinary ${FILENAME} ${FILENAME}.bin)
    add_custom_target(${TARGET}.dump DEPENDS ${TARGET} COMMAND ${CMAKE_OBJDUMP} -d -EL -S ${FILENAME} ${FILENAME}.dump)
endfunction()

function(MBED_PRINT_SIZE_OF_TARGETS TARGET)
    if(EXECUTABLE_OUTPUT_PATH)
      set(FILENAME "${EXECUTABLE_OUTPUT_PATH}/${TARGET}")
    else()
      set(FILENAME "${TARGET}")
    endif()
    add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND ${CMAKE_SIZE} ${FILENAME})
endfunction()
