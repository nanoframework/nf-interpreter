INCLUDE(CMakeForceCompiler)
# if( DEFINED CMAKE_CROSSCOMPILING )
#     # subsequent toolchain loading is not really needed
#     return()
# endif()

# check for toolchain path
# if(NOT TOOLCHAIN_PREFIX)
# 	message( "tcp ${TOOLCHAIN_PREFIX}" )
#      message(STATUS "\n-- ########################################\nNo TOOLCHAIN_PREFIX specified, need one!\nCall CMake with -DTOOLCHAIN_PREFIX=\"<path_to_your_gcc_toolchain>\"\n specifing the path to your GCC toolchain")
#      message(STATUS "\nNOTE: mind the forward slash in the path, without trailing slash.)")
#      message(STATUS "########################################\n\n")
#      message(FATAL_ERROR "No TOOLCHAIN_PREFIX specified")
# endif()

# Stop cmake trying to check if compiler works
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY )

# set toolchain directories
set(TOOLCHAIN_BIN_DIR ${TOOLCHAIN_PREFIX}/xtensa-esp32-elf/bin)
set(TOOLCHAIN_INC_DIR ${ESP32_IDF_PATH}/components/newlib/include)
set(TOOLCHAIN_LIB_DIR ${TOOLCHAIN_PREFIX}/xtensa-esp32-elf/xtensa-esp32-elf/lib)

# the name of the operating system for which CMake is to build
set(CMAKE_SYSTEM_NAME Generic)

# name of the CPU CMake is building for
set(CMAKE_SYSTEM_PROCESSOR xtensa-esp32)

# macro to setup compilers
macro(SET_COMPILER_VAR var name)
   find_program(CMAKE_${var} xtensa-esp32-elf-${name} HINTS ${TOOLCHAIN_BIN_DIR} DOC "${name} tool")
endmacro()

# setup C compiler
if(NOT CMAKE_C_COMPILER)
    SET_COMPILER_VAR(C_COMPILER gcc)
endif()
SET_COMPILER_VAR(C_COMPILER gcc)

# setup C++ compiler
if(NOT CMAKE_CXX_COMPILER)
    SET_COMPILER_VAR(CXX_COMPILER g++)
endif()

# setup Assembler compiler
SET_COMPILER_VAR(ASM-ATT_COMPILER as)
 
# other toolchain configurations  
set(CMAKE_OBJCOPY ${TOOLCHAIN_BIN_DIR}/xtensa-esp32-elf-objcopy CACHE INTERNAL "objcopy tool")
set(CMAKE_OBJDUMP ${TOOLCHAIN_BIN_DIR}/xtensa-esp32-elf-objdump CACHE INTERNAL "objdump tool")
set(CMAKE_SIZE ${TOOLCHAIN_BIN_DIR}/xtensa-esp32-elf-size CACHE INTERNAL "size tool")
set(CMAKE_DEBUGER ${TOOLCHAIN_BIN_DIR}/xtensa-esp32-elf-gdb CACHE INTERNAL "debuger")

set(GCC_ESP32_LINKER_FLAGS, " -nostdlib -u call_user_start_cpu0  -Wl,--gc-sections -Wl,-static -Wl,--start-group -Wl,--print-memory-usage " CACHE INTERNAL "Gcc esp32 liker flags" )
set(GCC_ESP32_LINKER_LIBS, " ${ESP32_IDF_PATH}/components/newlib/lib/libc.a ${ESP32_IDF_PATH}/components/newlib/lib/libm.a ${ESP32_IDF_PATH}/components/esp32/libhal.a -L${ESP32_IDF_PATH}/components/esp32/lib -lcore -lrtc -lnet80211 -lmesh -lpp -lwpa -lsmartconfig -lcoexist -lwps -lwpa2 -lphy -lgcc -lstdc++ "  CACHE INTERNAL "Gcc esp32 liker libs")
set(GCC_ESP32_LINKER_LD, " -L ${ESP32_IDF_PATH}/components/esp32/ld  -T esp32_out.ld -u ld_include_panic_highint_hdl -T esp32.common.ld -T esp32.rom.ld -T esp32.peripherals.ld "  CACHE INTERNAL "Gcc esp32 liker LD")

# set debug flags
#set(CMAKE_C_FLAGS "-DTEST_WITH_C_FLAGS " CACHE INTERNAL "c compiler flags ")


set(CMAKE_CXX_FLAGS "" CACHE INTERNAL "")

set(CMAKE_C_FLAGS_DEBUG "-std=gnu99 -Og -ggdb -ffunction-sections -fdata-sections -fstrict-volatile-bitfields -mlongcalls -nostdlib -Wall -Wextra -Werror -DWITH_POSIX -DHAVE_CONFIG_H -DESP_PLATFORM -D IDF_VER=\"v3.0\" -MMD -MP  " CACHE INTERNAL "c compiler flags debug")
set(CMAKE_CXX_FLAGS_DEBUG " -Og -ggdb -std=gnu++11 -fno-exceptions -fno-rtti  -ffunction-sections -fdata-sections -fstrict-volatile-bitfields -mlongcalls -nostdlib -Wall -Wextra -Werror -DESP_PLATFORM " CACHE INTERNAL "cxx compiler flags debug")

set(CMAKE_ASM_FLAGS_DEBUG " -g3 -ggdb" CACHE INTERNAL "asm compiler flags debug")

#set(CMAKE_EXE_LINKER_FLAGS_DEBUG " ${GCC_ESP32_LINKER_FLAGS} ${GCC_ESP32_LINKER_LIBS} ${GCC_ESP32_LINKER_LD} " CACHE INTERNAL "linker flags debug")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG " -nostdlib -u call_user_start_cpu0  -Wl,--gc-sections -Wl,-static " CACHE INTERNAL "linker flags debug")

# set release flags
set(CMAKE_C_FLAGS_RELEASE " -std=gnu99 -Os -ffunction-sections -fdata-sections -fstrict-volatile-bitfields -mlongcalls -nostdlib -MMD -MP -Wall -Wextra -Werror -D IDF_VER=\"v3.0\"  " CACHE INTERNAL "c compiler flags release")
set(CMAKE_C_FLAGS_MINSIZEREL " -std=gnu99 -Os -ffunction-sections -fdata-sections -fstrict-volatile-bitfields -mlongcalls -nostdlib -MMD -MP -Wall -Wextra -Werror -D IDF_VER=\"v3.0\"  " CACHE INTERNAL "c compiler flags minsizerel")
set(CMAKE_CXX_FLAGS_RELEASE " -Os -std=gnu++11 -g3 -fno-exceptions -fno-rtti -ffunction-sections -fdata-sections -fstrict-volatile-bitfields -mlongcalls -nostdlib -Wall -Wextra -Werror -DESP_PLATFORM " CACHE INTERNAL "cxx compiler flags release")
set(CMAKE_CXX_FLAGS_MINSIZEREL " -Os -std=gnu++11 -g3 -fno-exceptions -fno-rtti -ffunction-sections -fdata-sections -fstrict-volatile-bitfields -mlongcalls -nostdlib -Wall -Wextra -Werror -DESP_PLATFORM " CACHE INTERNAL "cxx compiler flags minsizerel")

set(CMAKE_ASM_FLAGS_RELEASE "" CACHE INTERNAL "asm compiler flags release")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE " -nostdlib -u call_user_start_cpu0  -Wl,--gc-sections -Wl,-static " CACHE INTERNAL "linker flags release")
set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL " -nostdlib -u call_user_start_cpu0  -Wl,--gc-sections -Wl,-static " CACHE INTERNAL "linker flags minsizerel")

set( CMAKE_C_LINK_EXECUTABLE   "<CMAKE_C_COMPILER>   <CMAKE_C_LINK_FLAGS>   <OBJECTS> <LINK_FLAGS> <LINK_LIBRARIES> -o <TARGET>")
set( CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_CXX_COMPILER> <CMAKE_C_LINK_FLAGS> <OBJECTS> <LINK_FLAGS> <LINK_LIBRARIES> -o <TARGET>")

# root paths to search on the filesystem for cross-compiling
set(CMAKE_FIND_ROOT_PATH ${TOOLCHAIN_PREFIX}/xtensa-esp32-elf ${EXTRA_FIND_PATH})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)


