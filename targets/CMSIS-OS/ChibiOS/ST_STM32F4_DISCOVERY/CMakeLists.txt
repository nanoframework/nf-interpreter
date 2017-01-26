cmake_minimum_required(VERSION 3.0)
ENABLE_LANGUAGE(ASM)

set(NANOBOOTER_PROJECT_NAME "nanoBooter")
set(NANOCLR_PROJECT_NAME "nanoCLR")

find_package(CHIBIOS REQUIRED)
find_package(WireProtocol REQUIRED)

add_subdirectory("common")
add_subdirectory("nanoBooter")
add_subdirectory("nanoCLR")

#######################
# nanoBooter executable

add_executable(
    # executables for project, project sources
    ${NANOBOOTER_PROJECT_NAME}.elf

    ${COMMON_PROJECT_SOURCES}
    ${NANOBOOTER_PROJECT_SOURCES}

    ${TARGET_CMSIS_COMMON_SOURCES}
    ${TARGET_CMSIS_NANOBOOTER_SOURCES}
    
    ${TARGET_CHIBIOS_COMMON_SOURCES}
    ${TARGET_CHIBIOS_NANOBOOTER_SOURCES}

    # ChibiOS sources
    ${CHIBIOS_SOURCES}

    # Wire Protocol sources
    ${WireProtocol_SOURCES}
)

#######################
# nanoCLR executable

add_executable(
    # executables for project, project sources
    ${NANOCLR_PROJECT_NAME}.elf

    ${COMMON_PROJECT_SOURCES}
    ${NANOCLR_PROJECT_SOURCES}

    ${TARGET_CMSIS_COMMON_SOURCES}
    ${TARGET_CMSIS_NANOCLR_SOURCES}
    
    ${TARGET_CHIBIOS_COMMON_SOURCES}
    ${TARGET_CHIBIOS_NANOCLR_SOURCES}

    # ChibiOS sources
    ${CHIBIOS_SOURCES}

    # Wire Protocol sources
    ${WireProtocol_SOURCES}
)


# include common directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/common

    ${WireProtocol_INCLUDE_DIRS}
    ${CHIBIOS_INCLUDE_DIRS}

    ${TARGET_CMSIS_COMMON_INCLUDE_DIRS}
    ${TARGET_CHIBIOS_COMMON_INCLUDE_DIRS}
)

# include nanoBooter directories
target_include_directories(${NANOBOOTER_PROJECT_NAME}.elf PUBLIC
     ${CMAKE_CURRENT_SOURCE_DIR}/nanoBooter

     ${TARGET_CMSIS_NANOBOOTER_INCLUDE_DIRS}
     ${TARGET_CHIBIOS_NANOBOOTER_INCLUDE_DIRS}
)

# include nanoCLR directories
target_include_directories(${NANOCLR_PROJECT_NAME}.elf PUBLIC
     ${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR

     ${TARGET_CMSIS_NANOCLR_INCLUDE_DIRS}
     ${TARGET_CHIBIOS_NANOCLR_INCLUDE_DIRS}
)

############################################################
# use default linker options and linker file for nanoBooter
CHIBIOS_SET_LINKER_OPTIONS(${NANOBOOTER_PROJECT_NAME}.elf)
############################################################

#######################################################
# need to use a specific linker file for nanoCLR
CHIBIOS_SET_LINKER_OPTIONS_AND_FILE(${NANOCLR_PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/nanoCLR/STM32F407xG.ld)
#######################################################

CHIBIOS_ADD_HEX_BIN_DUMP_TARGETS(${NANOBOOTER_PROJECT_NAME}.elf)
CHIBIOS_ADD_HEX_BIN_DUMP_TARGETS(${NANOCLR_PROJECT_NAME}.elf)

CHIBIOS_PRINT_SIZE_OF_TARGETS(${NANOBOOTER_PROJECT_NAME}.elf)
CHIBIOS_PRINT_SIZE_OF_TARGETS(${NANOCLR_PROJECT_NAME}.elf)

# set linker flags for DEBUG
set_property(TARGET ${NANOBOOTER_PROJECT_NAME}.elf APPEND_STRING PROPERTY LINK_FLAGS_DEBUG "")
set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf APPEND_STRING PROPERTY LINK_FLAGS_DEBUG "")

# set linker flags for RELEASE
set_property(TARGET ${NANOBOOTER_PROJECT_NAME}.elf APPEND_STRING PROPERTY LINK_FLAGS_RELEASE " -flto ")
set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf APPEND_STRING PROPERTY LINK_FLAGS_RELEASE " -flto ")

# add linker flags set with traditional CMAKE_EXE_LINKER_FLAGS
set_property(TARGET ${NANOBOOTER_PROJECT_NAME}.elf APPEND_STRING PROPERTY LINK_FLAGS ${CMAKE_EXE_LINKER_FLAGS})
set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf APPEND_STRING PROPERTY LINK_FLAGS ${CMAKE_EXE_LINKER_FLAGS})

# add linker flags to generate map file
set_property(TARGET ${NANOBOOTER_PROJECT_NAME}.elf APPEND_STRING PROPERTY LINK_FLAGS " -Wl,-Map=${PROJECT_SOURCE_DIR}/build/${NANOBOOTER_PROJECT_NAME}.map")
set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf APPEND_STRING PROPERTY LINK_FLAGS " -Wl,-Map=${PROJECT_SOURCE_DIR}/build/${NANOCLR_PROJECT_NAME}.map")

# add other linker flags
set_property(TARGET ${NANOBOOTER_PROJECT_NAME}.elf APPEND_STRING PROPERTY LINK_FLAGS ",--library-path=${PROJECT_BINARY_DIR}/ChibiOS_Source/os/common/ports/ARMCMx/compilers/GCC,--defsym=__main_stack_size__=0x400,--defsym=__process_stack_size__=0x400")
set_property(TARGET ${NANOCLR_PROJECT_NAME}.elf APPEND_STRING PROPERTY LINK_FLAGS ",--library-path=${PROJECT_BINARY_DIR}/ChibiOS_Source/os/common/ports/ARMCMx/compilers/GCC,--defsym=__main_stack_size__=0x400,--defsym=__process_stack_size__=0x400")


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
