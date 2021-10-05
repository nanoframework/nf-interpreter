#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(CMakeForceCompiler)

# the name of the operating system for which CMake is to build
set(CMAKE_SYSTEM_NAME Generic)

# macro to setup compilers
macro(nf_set_compiler_var var name)
   find_program(
      CMAKE_${var}
      xtensa-esp32-elf-${name}
      CMAKE_FIND_ROOT_PATH_BOTH
      REQUIRED)
endmacro()

# safer to have these here as a check if the toolchain are accessible in the PATH

# setup C compiler
nf_set_compiler_var(C_COMPILER gcc)

# setup C++ compiler
nf_set_compiler_var(CXX_COMPILER g++)

# setup Assembler compiler
nf_set_compiler_var(ASM_COMPILER gcc)

# other toolchain configurations  
find_program(
   CMAKE_OBJCOPY
   xtensa-esp32-elf-objcopy
   CMAKE_FIND_ROOT_PATH_BOTH)

find_program(
   CMAKE_OBJDUMP
   xtensa-esp32-elf-objdump
   CMAKE_FIND_ROOT_PATH_BOTH)

find_program(
   CMAKE_SIZE
   xtensa-esp32-elf-size
   CMAKE_FIND_ROOT_PATH_BOTH)

# TODO check this (move to GCC options?)
set(CMAKE_C_FLAGS "-mlongcalls -Wno-frame-address" CACHE STRING "C Compiler Base Flags")
set(CMAKE_CXX_FLAGS "-mlongcalls -Wno-frame-address" CACHE STRING "C++ Compiler Base Flags")

# set(GCC_ESP32_LINKER_FLAGS, " -nostdlib -u call_user_start_cpu0  -Wl,--gc-sections -Wl,-static -Wl,--start-group -Wl,--print-memory-usage " CACHE INTERNAL "Gcc esp32 liker flags" )
# set(GCC_ESP32_LINKER_LIBS, " ${ESP32_IDF_PATH}/components/newlib/lib/libc.a ${ESP32_IDF_PATH}/components/newlib/lib/libm.a ${ESP32_IDF_PATH}/components/esp32/libhal.a -L${ESP32_IDF_PATH}/components/esp32/lib -lcore -lrtc -lnet80211 -lmesh -lpp -lwpa -lsmartconfig -lcoexist -lwps -lwpa2 -lphy -lgcc -lstdc++ "  CACHE INTERNAL "Gcc esp32 liker libs")
# set(GCC_ESP32_LINKER_LD, " -L ${ESP32_IDF_PATH}/components/esp32/ld  -T esp32_out.ld -u ld_include_panic_highint_hdl -T esp32.common.ld -T esp32.rom.ld -T esp32.peripherals.ld "  CACHE INTERNAL "Gcc esp32 liker LD")

# set debug flags
#set(CMAKE_C_FLAGS "-DTEST_WITH_C_FLAGS " CACHE INTERNAL "c compiler flags ")

# set(CMAKE_CXX_FLAGS "" CACHE INTERNAL "")

# set(CMAKE_C_FLAGS_DEBUG "-std=gnu99 -Og -ggdb -ffunction-sections -fdata-sections -fstrict-volatile-bitfields -mlongcalls -nostdlib -Wall -Wextra -Werror -DWITH_POSIX -DHAVE_CONFIG_H -DESP_PLATFORM -D IDF_VER=\"v3.3.5\" -MMD -MP  " CACHE INTERNAL "c compiler flags debug")
# set(CMAKE_CXX_FLAGS_DEBUG " -Og -ggdb -std=gnu++11 -fno-exceptions -fno-rtti  -ffunction-sections -fdata-sections -fstrict-volatile-bitfields -mlongcalls -nostdlib -Wall -Wextra -Werror -DESP_PLATFORM " CACHE INTERNAL "cxx compiler flags debug")

# set(CMAKE_ASM_FLAGS_DEBUG " -g3 -ggdb" CACHE INTERNAL "asm compiler flags debug")

#set(CMAKE_EXE_LINKER_FLAGS_DEBUG " ${GCC_ESP32_LINKER_FLAGS} ${GCC_ESP32_LINKER_LIBS} ${GCC_ESP32_LINKER_LD} " CACHE INTERNAL "linker flags debug")
# set(CMAKE_EXE_LINKER_FLAGS_DEBUG " -nostdlib -u call_user_start_cpu0  -Wl,--gc-sections -Wl,-static " CACHE INTERNAL "linker flags debug")

# # set release flags
# set(CMAKE_C_FLAGS_RELEASE " -std=gnu99 -Os -ffunction-sections -fdata-sections -fstrict-volatile-bitfields -mlongcalls -nostdlib -MMD -MP -Wall -Wextra -Werror -D IDF_VER=\"v3.3.5\"  " CACHE INTERNAL "c compiler flags release")
# set(CMAKE_C_FLAGS_MINSIZEREL " -std=gnu99 -Os -ffunction-sections -fdata-sections -fstrict-volatile-bitfields -mlongcalls -nostdlib -MMD -MP -Wall -Wextra -Werror -D IDF_VER=\"v3.3.5\"  " CACHE INTERNAL "c compiler flags minsizerel")
# set(CMAKE_CXX_FLAGS_RELEASE " -Os -std=gnu++11 -g3 -fno-exceptions -fno-rtti -ffunction-sections -fdata-sections -fstrict-volatile-bitfields -mlongcalls -nostdlib -Wall -Wextra -Werror -DESP_PLATFORM " CACHE INTERNAL "cxx compiler flags release")
# set(CMAKE_CXX_FLAGS_MINSIZEREL " -Os -std=gnu++11 -g3 -fno-exceptions -fno-rtti -ffunction-sections -fdata-sections -fstrict-volatile-bitfields -mlongcalls -nostdlib -Wall -Wextra -Werror -DESP_PLATFORM " CACHE INTERNAL "cxx compiler flags minsizerel")

# set(CMAKE_ASM_FLAGS_RELEASE "" CACHE INTERNAL "asm compiler flags release")
# set(CMAKE_EXE_LINKER_FLAGS_RELEASE " -nostdlib -u call_user_start_cpu0  -Wl,--gc-sections -Wl,-static " CACHE INTERNAL "linker flags release")
# set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL " -nostdlib -u call_user_start_cpu0  -Wl,--gc-sections -Wl,-static " CACHE INTERNAL "linker flags minsizerel")

# set( CMAKE_C_LINK_EXECUTABLE   "<CMAKE_C_COMPILER>   <CMAKE_C_LINK_FLAGS>   <OBJECTS> <LINK_FLAGS> <LINK_LIBRARIES> -o <TARGET>")
# set( CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_CXX_COMPILER> <CMAKE_C_LINK_FLAGS> <OBJECTS> <LINK_FLAGS> <LINK_LIBRARIES> -o <TARGET>")

# root paths to search on the filesystem for cross-compiling
get_filename_component(CMAKE_FIND_ROOT_PATH ${CMAKE_C_COMPILER} DIRECTORY CACHE)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# set required C and C++ standard for ALL targets
set(CMAKE_C_STANDARD 11 CACHE INTERNAL "C standard for all targets")
set(CMAKE_CXX_STANDARD 11 CACHE INTERNAL "C++ standard for all targets")

# Perform compiler test with static library
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
