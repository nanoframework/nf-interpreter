#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

cmake_minimum_required(VERSION 3.0)
ENABLE_LANGUAGE(ASM)

# add header files with common OS definitions and board definitions specific for each image
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/nanoBooter/target_board.h.in"
                "${CMAKE_CURRENT_BINARY_DIR}/nanoBooter/target_board.h" @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/target_board.h.in"
                "${CMAKE_CURRENT_BINARY_DIR}/nanoCLR/target_board.h" @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/target_common.h.in"
                "${CMAKE_CURRENT_BINARY_DIR}/target_common.h" @ONLY)

set(NANOBOOTER_PROJECT_NAME "nanoBooter")
set(NANOCLR_PROJECT_NAME "nanoCLR")

find_package(CHIBIOS REQUIRED)
find_package(ChibiOSnfOverlay REQUIRED)
find_package(WireProtocol REQUIRED)

# packages for nanoFramework libraries
# mandatory 
find_package(NF_CoreCLR REQUIRED)

# nF feature: debugger
if(NF_FEATURE_DEBUGGER)
    find_package(NF_Debugger REQUIRED)
endif()

# optional



add_subdirectory("common")
add_subdirectory("nanoBooter")
add_subdirectory("nanoCLR")

#######################
# nanoBooter executable

add_executable(
    # executables for project, project sources
    ${NANOBOOTER_PROJECT_NAME}.elf

    "${CMAKE_CURRENT_SOURCE_DIR}/target_common.c"

    ${COMMON_PROJECT_SOURCES}
    ${NANOBOOTER_PROJECT_SOURCES}

    ${TARGET_CMSIS_COMMON_SOURCES}
    ${TARGET_CMSIS_NANOBOOTER_SOURCES}
    
    ${TARGET_CHIBIOS_COMMON_SOURCES}
    ${TARGET_CHIBIOS_NANOBOOTER_SOURCES}

    ${CHIBIOS_SOURCES}
    ${ChibiOSnfOverlay_SOURCES}

    ${WireProtocol_SOURCES}
)

#######################
# nanoCLR executable

add_executable(
    # executables for project, project sources
    ${NANOCLR_PROJECT_NAME}.elf

    "${CMAKE_CURRENT_SOURCE_DIR}/target_common.c"

    ${COMMON_PROJECT_SOURCES}
    ${NANOCLR_PROJECT_SOURCES}

    ${TARGET_CMSIS_COMMON_SOURCES}
    ${TARGET_CMSIS_NANOCLR_SOURCES}
    
    ${TARGET_CHIBIOS_COMMON_SOURCES}
    ${TARGET_CHIBIOS_NANOCLR_SOURCES}

    ${CHIBIOS_SOURCES}
    ${ChibiOSnfOverlay_SOURCES}

    # sources for nanoFramework libraries
    "${NF_CoreCLR_SOURCES}"
    "${NF_Debugger_SOURCES}"
)


# include common directories
include_directories(
    "${CMAKE_CURRENT_BINARY_DIR}"
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${PROJECT_SOURCE_DIR}/src/CLR/Include
    ${PROJECT_SOURCE_DIR}/src/HAL/Include
    ${PROJECT_SOURCE_DIR}/src/PAL/Include

    ${WireProtocol_INCLUDE_DIRS}
    ${CHIBIOS_INCLUDE_DIRS}
    ${ChibiOSnfOverlay_INCLUDE_DIRS}

    ${TARGET_CMSIS_COMMON_INCLUDE_DIRS}
    ${TARGET_CHIBIOS_COMMON_INCLUDE_DIRS}
)

# include directories for nanoBooter
target_include_directories(${NANOBOOTER_PROJECT_NAME}.elf PUBLIC
    "${CMAKE_CURRENT_BINARY_DIR}/nanoBooter"
     ${CMAKE_CURRENT_SOURCE_DIR}/nanoBooter

     ${TARGET_CMSIS_NANOBOOTER_INCLUDE_DIRS}
     ${TARGET_CHIBIOS_NANOBOOTER_INCLUDE_DIRS}
)

# include directories for nanoCLR
target_include_directories(${NANOCLR_PROJECT_NAME}.elf PUBLIC
    "${CMAKE_CURRENT_BINARY_DIR}/nanoCLR"
    ${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR

    ${TARGET_CMSIS_NANOCLR_INCLUDE_DIRS}
    ${TARGET_CHIBIOS_NANOCLR_INCLUDE_DIRS}

    # directories for nanoFramework libraries
    "${NF_CoreCLR_INCLUDE_DIRS}"
    "${NF_Debugger_INCLUDE_DIRS}"
)

###################
# set linker files
CHIBIOS_SET_LINKER_OPTIONS_AND_FILE(${NANOBOOTER_PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/nanoBooter/mbn_quail_booter.ld)
CHIBIOS_SET_LINKER_OPTIONS_AND_FILE(${NANOCLR_PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/mbn_quail_CLR.ld)
###################

CHIBIOS_ADD_HEX_BIN_DUMP_TARGETS(${NANOBOOTER_PROJECT_NAME}.elf)
CHIBIOS_ADD_HEX_BIN_DUMP_TARGETS(${NANOCLR_PROJECT_NAME}.elf)

CHIBIOS_PRINT_SIZE_OF_TARGETS(${NANOBOOTER_PROJECT_NAME}.elf)
CHIBIOS_PRINT_SIZE_OF_TARGETS(${NANOCLR_PROJECT_NAME}.elf)

# set extra compiler flags 
target_compile_definitions(${NANOBOOTER_PROJECT_NAME}.elf PUBLIC "-DPLATFORM_EMULATED_FLOATINGPOINT -DPLATFORM_ARM ")
target_compile_definitions(${NANOCLR_PROJECT_NAME}.elf PUBLIC "-DPLATFORM_EMULATED_FLOATINGPOINT -DPLATFORM_ARM ")

# set compiler definition for FPU
target_compile_definitions(${NANOBOOTER_PROJECT_NAME}.elf PUBLIC -DCORTEX_USE_FPU=$<$<BOOL:USE_FPU_IS_TRUE>:TRUE>$<$<NOT:$<BOOL:USE_FPU_IS_TRUE>>:FALSE>)
target_compile_definitions(${NANOCLR_PROJECT_NAME}.elf PUBLIC -DCORTEX_USE_FPU=$<$<BOOL:USE_FPU_IS_TRUE>:TRUE>$<$<NOT:$<BOOL:USE_FPU_IS_TRUE>>:FALSE>)

# set extra linker flags for DEBUG
set_property(TARGET ${NANOBOOTER_PROJECT_NAME}.elf APPEND_STRING PROPERTY LINK_FLAGS_DEBUG "")
set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf APPEND_STRING PROPERTY LINK_FLAGS_DEBUG "")

# set extra linker flags for RELEASE
set_property(TARGET ${NANOBOOTER_PROJECT_NAME}.elf APPEND_STRING PROPERTY LINK_FLAGS_RELEASE " -flto ")
set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf APPEND_STRING PROPERTY LINK_FLAGS_RELEASE " -flto ")

# add linker flags set with traditional CMAKE_EXE_LINKER_FLAGS
set_property(TARGET ${NANOBOOTER_PROJECT_NAME}.elf APPEND_STRING PROPERTY LINK_FLAGS ${CMAKE_EXE_LINKER_FLAGS})
set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf APPEND_STRING PROPERTY LINK_FLAGS ${CMAKE_EXE_LINKER_FLAGS})

# add linker flags to generate map file
set_property(TARGET ${NANOBOOTER_PROJECT_NAME}.elf APPEND_STRING PROPERTY LINK_FLAGS " -Wl,-Map=${PROJECT_SOURCE_DIR}/build/${NANOBOOTER_PROJECT_NAME}.map")
set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf APPEND_STRING PROPERTY LINK_FLAGS " -Wl,-Map=${PROJECT_SOURCE_DIR}/build/${NANOCLR_PROJECT_NAME}.map")

# add other linker flags
###################################################
# the size of the CLR managed heap is defined here
###################################################
set_property(TARGET ${NANOBOOTER_PROJECT_NAME}.elf APPEND_STRING PROPERTY LINK_FLAGS ",--library-path=${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/common,--defsym=__main_stack_size__=0x400,--defsym=__process_stack_size__=0x400,--defsym=__clr_managed_heap_size__=0x0")
set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf APPEND_STRING PROPERTY LINK_FLAGS ",--library-path=${PROJECT_SOURCE_DIR}/targets/CMSIS-OS/ChibiOS/common,--defsym=__main_stack_size__=0x400,--defsym=__process_stack_size__=0x400,--defsym=__clr_managed_heap_size__=0x9000")


set(NANOBOOTER_HEX_FILE ${PROJECT_SOURCE_DIR}/build/${NANOBOOTER_PROJECT_NAME}.hex)
set(NANOCLR_HEX_FILE ${PROJECT_SOURCE_DIR}/build/${NANOCLR_PROJECT_NAME}.hex)
set(NANOBOOTER_S19_FILE ${PROJECT_SOURCE_DIR}/build/${NANOBOOTER_PROJECT_NAME}.s19)
set(NANOCLR_S19_FILE ${PROJECT_SOURCE_DIR}/build/${NANOCLR_PROJECT_NAME}.s19)
set(NANOBOOTER_BIN_FILE ${PROJECT_SOURCE_DIR}/build/${NANOBOOTER_PROJECT_NAME}.bin)
set(NANOCLR_BIN_FILE ${PROJECT_SOURCE_DIR}/build/${NANOCLR_PROJECT_NAME}.bin)
set(NANOBOOTER_DUMP_FILE ${PROJECT_SOURCE_DIR}/build/${NANOBOOTER_PROJECT_NAME}.lst)
set(NANOCLR_DUMP_FILE ${PROJECT_SOURCE_DIR}/build/${NANOCLR_PROJECT_NAME}.lst)

# generate HEX, BIN and LST files as needed
if(CMAKE_BUILD_TYPE EQUAL "Release")

    add_custom_command(TARGET ${NANOBOOTER_PROJECT_NAME}.elf POST_BUILD
            # copy target image to other formats
            COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${NANOBOOTER_PROJECT_NAME}.elf> ${NANOBOOTER_HEX_FILE}
            COMMAND ${CMAKE_OBJCOPY} -Osrec $<TARGET_FILE:${NANOBOOTER_PROJECT_NAME}.elf> ${NANOBOOTER_S19_FILE}
            COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${NANOBOOTER_PROJECT_NAME}.elf> ${NANOBOOTER_BIN_FILE}

            # copy target file to build folder (this is only usefull for debugging in VS Code because of path in launch.json)
            COMMAND ${CMAKE_OBJCOPY} $<TARGET_FILE:${NANOBOOTER_PROJECT_NAME}.elf> ${PROJECT_SOURCE_DIR}/build/${NANOBOOTER_PROJECT_NAME.elf}

            COMMENT "Generate nanoBooter HEX and BIN files for deployment")

    add_custom_command(TARGET ${NANOCLR_PROJECT_NAME}.elf POST_BUILD
            # copy target image to other formats
            COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${NANOCLR_PROJECT_NAME}.elf> ${NANOCLR_HEX_FILE}
            COMMAND ${CMAKE_OBJCOPY} -Osrec $<TARGET_FILE:${NANOCLR_PROJECT_NAME}.elf> ${NANOCLR_S19_FILE}
            COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${NANOCLR_PROJECT_NAME}.elf> ${NANOCLR_BIN_FILE}

            # copy target file to build folder (this is only usefull for debugging in VS Code because of path in launch.json)
            COMMAND ${CMAKE_OBJCOPY} $<TARGET_FILE:${NANOCLR_PROJECT_NAME}.elf> ${PROJECT_SOURCE_DIR}/build/${NANOCLR_PROJECT_NAME}.elf

            COMMENT "Generate nanoCLR HEX and BIN files for deployment")

else()

    add_custom_command(TARGET ${NANOBOOTER_PROJECT_NAME}.elf POST_BUILD
            # copy target image to other formats
            COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${NANOBOOTER_PROJECT_NAME}.elf> ${NANOBOOTER_HEX_FILE}
            COMMAND ${CMAKE_OBJCOPY} -Osrec $<TARGET_FILE:${NANOBOOTER_PROJECT_NAME}.elf> ${NANOBOOTER_S19_FILE}
            COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${NANOBOOTER_PROJECT_NAME}.elf> ${NANOBOOTER_BIN_FILE}

            # copy target file to build folder (this is only usefull for debugging in VS Code because of path in launch.json)
            COMMAND ${CMAKE_OBJCOPY} $<TARGET_FILE:${NANOBOOTER_PROJECT_NAME}.elf> ${PROJECT_SOURCE_DIR}/build/${NANOBOOTER_PROJECT_NAME}.elf

            # dump target image as source code listing
            COMMAND ${CMAKE_OBJDUMP} -d -EL -S $<TARGET_FILE:${NANOBOOTER_PROJECT_NAME}.elf> > ${NANOBOOTER_DUMP_FILE}

            COMMENT "Generate nanoBooter HEX and BIN files for deployment, LST file for debug")

    add_custom_command(TARGET ${NANOCLR_PROJECT_NAME}.elf POST_BUILD
            # copy target image to other formats
            COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${NANOCLR_PROJECT_NAME}.elf> ${NANOCLR_HEX_FILE}
            COMMAND ${CMAKE_OBJCOPY} -Osrec $<TARGET_FILE:${NANOCLR_PROJECT_NAME}.elf> ${NANOCLR_S19_FILE}
            COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${NANOCLR_PROJECT_NAME}.elf> ${NANOCLR_BIN_FILE}

            # copy target file to build folder (this is only usefull for debugging in VS Code because of path in launch.json)
            COMMAND ${CMAKE_OBJCOPY} $<TARGET_FILE:${NANOCLR_PROJECT_NAME}.elf> ${PROJECT_SOURCE_DIR}/build/${NANOCLR_PROJECT_NAME}.elf

            # dump target image as source code listing
            COMMAND ${CMAKE_OBJDUMP} -d -EL -S $<TARGET_FILE:${NANOCLR_PROJECT_NAME}.elf> > ${NANOCLR_DUMP_FILE}

            COMMENT "Generate nanoCLR HEX and BIN files for deployment, LST file for debug")

endif()
