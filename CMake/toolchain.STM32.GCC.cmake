INCLUDE(CMakeForceCompiler)

# if( DEFINED CMAKE_CROSSCOMPILING )
#     # subsequent toolchain loading is not really needed
#     return()
# endif()

# set STM32 supported series
# full list would be F0, F1, F2, F3, F4, F7, L0, L1 and L4
# copy bellow as support is added 
set(STM32_SUPPORTED_SERIES F0 F4 F7 CACHE INTERNAL "STM32 supported series")

# check for toolchain path
if(NOT TOOLCHAIN_PREFIX)
     message(STATUS "\n-- ########################################\nNo TOOLCHAIN_PREFIX specified, need one!\nCall CMake with -DTOOLCHAIN_PREFIX=\"<path_to_your_gcc_toolchain>\"\n specifing the path to your GCC toolchain (ex: E:/GNU_Tools_ARM_Embedded/5_4_2016q3)")
     message(STATUS "\nNOTE: mind the forward slash in the path, without trailing slash.)")
     message(STATUS "########################################\n\n")
     message(FATAL_ERROR "No TOOLCHAIN_PREFIX specified")
endif()

# parse target chip
if(NOT TARGET_CHIP)
    message(FATAL_ERROR "\n\n-- ###################################\nNo TARGET_CHIP specified, need one!\nCall CMake with -DTARGET_CHIP=STM32F407VG (example).\n###################################\n\n")
else()
    string(REGEX REPLACE "^[S][T][M]32(([F][0-47])|([L][0-1])|([T])|([W])).+$" "\\1" STM32_SERIES ${TARGET_CHIP})
    set(STM32_SERIES "${STM32_SERIES}" CACHE INTERNAL "STM32 series name")
    # need the series name in lower case too for some paths
    string(TOLOWER ${STM32_SERIES} STM32_SERIES_LOWER)
    set(STM32_SERIES_LOWER "${STM32_SERIES_LOWER}" CACHE INTERNAL "STM32 series name lower case")

    message(STATUS "Selected STM32 series: ${STM32_SERIES}")
endif()

# check if this SMT32 series is supported
list(FIND STM32_SUPPORTED_SERIES ${STM32_SERIES} SERIES_INDEX)
if(SERIES_INDEX EQUAL -1)
    message(FATAL_ERROR "\n\nSorry but ${STM32_SERIES} is not supported at this time...\nYou can wait for that to be added or you might want to contribute and start working on a PR for that.\n\n")
endif()

# include CMake module with the specifics of the SMT32 series of the target chip
include(STM32${STM32_SERIES}_GCC_options)

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

function(STM32_ADD_HEX_BIN_DUMP_TARGETS TARGET)
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

function(STM32_PRINT_SIZE_OF_TARGETS TARGET)
    if(EXECUTABLE_OUTPUT_PATH)
      set(FILENAME "${EXECUTABLE_OUTPUT_PATH}/${TARGET}")
    else()
      set(FILENAME "${TARGET}")
    endif()
    add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND ${CMAKE_SIZE} ${FILENAME})
endfunction()

function(STM32_SET_FLASH_PARAMS TARGET STM32_FLASH_SIZE STM32_RAM_SIZE STM32_CCRAM_SIZE STM32_MIN_STACK_SIZE STM32_MIN_HEAP_SIZE STM32_FLASH_ORIGIN STM32_RAM_ORIGIN STM32_CCRAM_ORIGIN)
    
    include(STM32_linker)
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_flash.ld ${STM32_LINKER_SCRIPT_TEXT})

    get_target_property(TARGET_LD_FLAGS ${TARGET} LINK_FLAGS)
    if(TARGET_LD_FLAGS)
        set(TARGET_LD_FLAGS "-T${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_flash.ld ${TARGET_LD_FLAGS}")
    else()
        set(TARGET_LD_FLAGS "-T${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_flash.ld")
    endif()
    set_target_properties(${TARGET} PROPERTIES LINK_FLAGS ${TARGET_LD_FLAGS})

endfunction()

function(STM32_SET_FLASH_PARAMS TARGET FLASH_SIZE RAM_SIZE)
    if(NOT STM32_FLASH_ORIGIN)
        set(STM32_FLASH_ORIGIN "0x08000000")
    endif()

    if(NOT STM32_RAM_ORIGIN)
        set(STM32_RAM_ORIGIN "0x20000000")
    endif()

    if(NOT STM32_MIN_STACK_SIZE)
        set(STM32_MIN_STACK_SIZE "0x400")
    endif()

    if(NOT STM32_MIN_HEAP_SIZE)
        set(STM32_MIN_HEAP_SIZE "0x200")
    endif()

    if(NOT STM32_CCRAM_ORIGIN)
        set(STM32_CCRAM_ORIGIN "0x10000000")
    endif()

    if(NOT STM32_CCRAM_SIZE)
        set(STM32_CCRAM_SIZE "64K")
    endif()

    include(STM32_linker)
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_flash.ld ${STM32_LINKER_SCRIPT_TEXT})

    get_target_property(TARGET_LD_FLAGS ${TARGET} LINK_FLAGS)
    if(TARGET_LD_FLAGS)
        set(TARGET_LD_FLAGS "-T${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_flash.ld ${TARGET_LD_FLAGS}")
    else()
        set(TARGET_LD_FLAGS "-T${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_flash.ld")
    endif()
    set_target_properties(${TARGET} PROPERTIES LINK_FLAGS ${TARGET_LD_FLAGS})
endfunction()

function(STM32_SET_TARGET_PROPERTIES TARGET)
    if(NOT TARGET_CHIP_TYPE)
        if(NOT TARGET_CHIP)
            message(WARNING "No valid TARGET_CHIP_TYPE or TARGET_CHIP selected")
        else()
            STM32_GET_CHIP_TYPE(${TARGET_CHIP} TARGET_CHIP_TYPE)
        endif()
    endif()

    STM32_SET_CHIP_DEFINITIONS(${TARGET} ${TARGET_CHIP_TYPE})
    
    if(((NOT STM32_FLASH_SIZE) OR (NOT STM32_RAM_SIZE)) AND (NOT TARGET_CHIP))
        message(FATAL_ERROR "Cannot get STM32_FLASH_SIZE and STM32_RAM_SIZE chip parameters ")
    endif()

    if((NOT STM32_FLASH_SIZE) OR (NOT STM32_RAM_SIZE))
        STM32_GET_CHIP_PARAMETERS(${TARGET_CHIP} STM32_FLASH_SIZE STM32_RAM_SIZE)
        
        if((NOT STM32_FLASH_SIZE) OR (NOT STM32_RAM_SIZE))
            message(FATAL_ERROR "Unknown chip: ${TARGET_CHIP}.")
        endif()
    endif()

    STM32_SET_FLASH_PARAMS(${TARGET} ${STM32_FLASH_SIZE} ${STM32_RAM_SIZE})
    message(STATUS "${TARGET_CHIP} has ${STM32_FLASH_SIZE}B of flash memory and ${STM32_RAM_SIZE}B of RAM")

endfunction()

macro(STM32_GENERATE_LIBRARIES NAME SOURCES LIBRARIES)
    string(TOLOWER ${STM32_SERIES} STM32_SERIES_LOWER)
    foreach(CHIP_TYPE ${TARGET_CHIP_TYPES})
        string(TOLOWER ${CHIP_TYPE} CHIP_TYPE_LOWER)
        LIST(APPEND ${LIBRARIES} ${NAME}_${STM32_SERIES_LOWER}_${CHIP_TYPE_LOWER})
        add_library(${NAME}_${STM32_SERIES_LOWER}_${CHIP_TYPE_LOWER} ${SOURCES})
        STM32_SET_CHIP_DEFINITIONS(${NAME}_${STM32_SERIES_LOWER}_${CHIP_TYPE_LOWER} ${CHIP_TYPE})
    endforeach()
endmacro()
