#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# setup Zephyr west tool
# this is required to be able to execute west commands from CMake
# requires Python and Zephyr west to be installed
macro(setup_west)

    # make sure Python is installed and accessible
    find_package(Python 3.10 COMPONENTS Interpreter REQUIRED)

    # install west, if not already installed
    execute_process(
        COMMAND pip3 install west
        COMMAND_ECHO NONE
        OUTPUT_QUIET
    )

    # set west "executable"
    set(WEST_EXE ${Python_EXECUTABLE} -m west CACHE INTERNAL "West executable in CMake callable format")

endmacro()

# Add platform packages specific to Zephyr
# To be called from target CMakeList.txt
# optional TARGET argument with target name
macro(nf_add_platform_packages)

    # parse arguments
    cmake_parse_arguments(NFAPP "" "TARGET" "" ${ARGN})

    #
    # packages common to all targets
    #
    # (binutils.common.cmake) Calls FindPackage on the 4 main libs:
    # NF_HALCore NF_CoreCLR NF_NativeAssemblies WireProtocol
    # and optionally NF_Diagnostics and NF_Debugger
    nf_add_common_packages()

    # packages specific for nanoBooter
    if("${NFAPP_TARGET}" STREQUAL "${NANOBOOTER_PROJECT_NAME}")
        # no packages for booter
    endif()

    # packages specific for nanoCLR
    if("${NFAPP_TARGET}" STREQUAL "${NANOCLR_PROJECT_NAME}")

        if(USE_NETWORKING_OPTION)
            find_package(NF_Network REQUIRED QUIET)

        endif()

    endif()

endmacro()

# Add Zephyr platform dependencies to a specific CMake target
# To be called from target CMakeList.txt
macro(nf_add_platform_dependencies target)

message(ERROR "nf_add_platform_dependencies not used on this platform.")

endmacro()

# Add Zephyr platform include directories to a specific CMake target
# To be called from target CMakeList.txt
macro(nf_add_platform_include_directories target)

    target_include_directories(${target}.elf PUBLIC

        ${TARGET_NORDIC_COMMON_INCLUDE_DIRS}
    )

    # includes specific to nanoBooter
    if(${target} STREQUAL ${NANOBOOTER_PROJECT_NAME})

        target_include_directories(${target}.elf PUBLIC

            ${TARGET_NORDIC_NANOBOOTER_INCLUDE_DIRS}
        )

    endif()

    # includes specific to nanoCLR
    if(${target} STREQUAL ${NANOCLR_PROJECT_NAME})

        target_include_directories(${target}.elf PUBLIC

            ${NANOCLR_PROJECT_INCLUDE_DIRS}
            ${TARGET_NORDIC_NANOCLR_INCLUDE_DIRS}
            ${LWIP_INCLUDE_DIRS}
            ${CMAKE_CURRENT_BINARY_DIR}

        )

    endif()
endmacro()

# Add Zephyr platform target sources to a specific CMake target
# To be called from target CMakeList.txt
macro(nf_add_platform_sources target)

    # add header files with common OS definitions and board definitions
    configure_file(${TARGET_BASE_LOCATION}/target_common.h.in
                   ${CMAKE_BINARY_DIR}/targets/${RTOS}/${TARGET_BOARD}/target_common.h @ONLY)

    # sources specific to nanoCLR
    if(${target} STREQUAL ${NANOCLR_PROJECT_NAME})

        # add header files with common OS definitions and board definitions
        configure_file(${TARGET_BASE_LOCATION}/nanoCLR/target_board.h.in
                       ${CMAKE_BINARY_DIR}/targets/${RTOS}/${TARGET_BOARD}/nanoCLR/target_board.h @ONLY)

    endif()

endmacro()

# macro to setup the build for a target
# This macro is vastly different from other NF targets as the zephyr
# build system takes care of all the linking and other complexities
macro(nf_setup_target_build)

    if(CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
        set(OVERLAY_CONFIG ${CMAKE_SOURCE_DIR}/targets/Nordic/_common/overlay-debugging.conf CACHE STRING "Zephyr overlay config file for debug")
    endif()

    # points Zephyr to nanoCLR lib module
    list(APPEND ZEPHYR_EXTRA_MODULES ${CMAKE_SOURCE_DIR}/targets/Nordic/_nanoCLR_module)

    # add our location for board directories search
    list(APPEND BOARD_ROOT ${CMAKE_SOURCE_DIR}/targets/Nordic/_zephyr_boards)

    # take care of composing nanoCLR version
    nf_zephyr_compose_nano_version()

    # During this invocation nanoCLR will be built as a Zephyr 'module'.
    find_package(Zephyr REQUIRED QUIET HINTS ${NCS_BASE_LOCATION})

    if(NOT Zephyr_FOUND)
        # Zephyr SDK not found report error
        message(FATAL_ERROR "\n\nCouldn't find Zephyr SDK v${ZEPHYR_SDK_VERSION}!\n\nMake sure to install it using Toolchain Manager from Nordic Connect SDK or manually from the Zephyr SDK GitHub repository.\n\n")
    endif()

    # At this point nanoCLR module (library) has been built.
    # Add a trivial app that invokes nanoCLR.

    zephyr_include_directories(${CMAKE_SOURCE_DIR}/targets/Nordic/nf_lib_module/zephyr/include)

    # The Zephyr build system requires that CMake project has to be defined here
    project(nanoFramework VERSION ${BUILD_VERSION} LANGUAGES C CXX ASM)

    zephyr_compile_definitions(PROJECT_NAME=nanoCLR)

    #zephyr_compile_definitions(APP_VERSION=${BUILD_VERSION})

    #######################################################
    # the following are nanoCLR common compile definitions
    # for "regular" targets they are set in nf_common_compiler_definitions

    # build types that have debugging capabilities AND are NOT RTM have to have the define 'NANOCLR_ENABLE_SOURCELEVELDEBUGGING'
    if((NOT NF_BUILD_RTM) OR NF_FEATURE_DEBUGGER)
        zephyr_compile_definitions(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
    endif()

    # set compiler definition for RTM build option
    if(NF_BUILD_RTM)
        zephyr_compile_definitions(BUILD_RTM)
    endif()

    # set compiler definition for using Application Domains feature
    if(NF_FEATURE_USE_APPDOMAINS)
        zephyr_compile_definitions(NANOCLR_USE_APPDOMAINS)
    endif()

    # set compiler definition for implementing (or not) CRC32 in Wire Protocol
    if(NF_WP_IMPLEMENTS_CRC32)
        zephyr_compile_definitions(WP_IMPLEMENTS_CRC32)
    endif()

    # set definition for Wire Protocol trace mask
    zephyr_compile_definitions(TRACE_MASK=${WP_TRACE_MASK})

    # set compiler definition regarding inclusion of trace messages and checks on CLR
    if(NF_PLATFORM_NO_CLR_TRACE)
        zephyr_compile_definitions(PLATFORM_NO_CLR_TRACE=1)
    endif()

    # set compiler definition regarding CLR IL inlining
    if(NF_CLR_NO_IL_INLINE)
        zephyr_compile_definitions(NANOCLR_NO_IL_INLINE=1)
    endif()

    # set compiler definition for implementing (or not) TRACE to stdio
    if(NF_TRACE_TO_STDIO)
        zephyr_compile_definitions(NF_TRACE_TO_STDIO)
    endif()

    zephyr_compile_definitions(PLATFORM_ARM)

    #######################################################

    nf_clear_output_files_nanoclr()

endmacro()

# copy Zephyr build artifacts to build output folder
macro(nf_copy_zephyr_build_artifacts)

    # need to adjust these depending on the artifacts produced by Zephyr build
    # some targets produce merged.hex, others zephyr.hex
    if(${TARGET_BOARD} STREQUAL "NRF9160_DK"  
        OR ${TARGET_BOARD} STREQUAL "ARDESCO")
        # use merged.hex
        set(HEX_NAME "merged")
    elseif(${TARGET_BOARD} STREQUAL "NRF52840_DK")
        # there is no merged.hex being generated, stick with zephyr.hex
        set(HEX_NAME "zephyr")
    else()
        message(FATAL_ERROR "Unknown target board ${TARGET_BOARD}!")
    endif()

    add_custom_command(
        OUTPUT
            ${CMAKE_BINARY_DIR}/nanoCLR.hex

        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_BINARY_DIR}/targets/Nordic/${TARGET_BOARD}/zephyr/${HEX_NAME}.hex
            ${CMAKE_BINARY_DIR}/nanoCLR.hex

        DEPENDS
            ${CMAKE_BINARY_DIR}/targets/Nordic/${TARGET_BOARD}/zephyr/${HEX_NAME}.hex

        COMMENT "Copy build artifacts"
    )

    add_custom_target(
        copy_build_artifacts ALL

        DEPENDS
            ${CMAKE_BINARY_DIR}/nanoCLR.hex
    )

    add_dependencies(
        copy_build_artifacts
        app)

endmacro()

# macro to clear binary files related with nanoCLR from output
# to make sure that the build file it's up to date
macro(nf_clear_output_files_nanoclr)
    list(APPEND CLR_BUILD_FILES_TO_REMOVE ${CMAKE_BINARY_DIR}/nanoCLR.bin)
    list(APPEND CLR_BUILD_FILES_TO_REMOVE ${CMAKE_BINARY_DIR}/nanoCLR.hex)
    list(APPEND CLR_BUILD_FILES_TO_REMOVE ${CMAKE_BINARY_DIR}/targets/Nordic/${TARGET_BOARD}/zephyr/merged.hex)

    add_custom_command(
        TARGET app PRE_BUILD
        DEPENDS ${logical_target_for_zephyr_elf}
        COMMAND ${CMAKE_COMMAND} -E remove -f ${CLR_BUILD_FILES_TO_REMOVE}
        COMMAND_EXPAND_LISTS
        COMMENT "Removing nanoCLR artifacts from build folder"
    )

endmacro()

# this macro composes the nanoCLR version components
macro(nf_zephyr_compose_nano_version)

    # We need to parse the build version in to seperate elements.
    # Normally, this is done by CMAKE in the project command which is in the main CMakelist.txt file.
    # Since we need to set the project here for Zephyr builds, and still need the  parsed values during the Zephyr build, we do it manually.
    string(REPLACE "." ";" BV1 ${BUILD_VERSION})
    set(BVLIST ${BV1})
    list(GET BVLIST 0 nanoFramework_VERSION_MAJOR)
    list(GET BVLIST 1 nanoFramework_VERSION_MINOR)
    list(GET BVLIST 2 nanoFramework_VERSION_PATCH)
    list(GET BVLIST 3 nanoFramework_VERSION_TWEAK)

endmacro()
