#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

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
    configure_file(${TARGET_RTOS_ROOT}/${TARGET_BOARD}/target_common.h.in
                   ${CMAKE_BINARY_DIR}/targets/${RTOS}/${TARGET_BOARD}/target_common.h @ONLY)

    # sources specific to nanoCLR
    if(${target} STREQUAL ${NANOCLR_PROJECT_NAME})

        # add header files with common OS definitions and board definitions 
        configure_file(${TARGET_RTOS_ROOT}/${TARGET_BOARD}/nanoCLR/target_board.h.in
                    ${CMAKE_BINARY_DIR}/targets/${RTOS}/${TARGET_BOARD}/nanoCLR/target_board.h @ONLY)

    endif()
endmacro()

# macro to setup the build for a target
# This macro is vastly different from other NF targets as the zephyr
# build system takes care of all the linking and other complexities
macro(nf_setup_target_build)

    # Get the zephyr base 
    string(REPLACE "\\" "/" ZBASE $ENV{ZEPHYR_BASE})
    # Add zephyr tree to CMAKE path.
    list(APPEND CMAKE_MODULE_PATH ${ZBASE}/cmake)

    # This tells the Zephyr RTOS where to look to add the NF lib as a module.
    list(APPEND ZEPHYR_EXTRA_MODULES
        ${TARGET_RTOS_ROOT}/module)

    # HACK::
    # We need to parse the build version in to seperate elements.
    # Normally, this is done by CMAKE in the project command in the 
    # main CMakelist.txt file. Since we skip that, but need the 
    # parsed values during the Zephyr build below, we do it manually.
    string(REPLACE "." ";" BV1 ${BUILD_VERSION})
    set(BVLIST ${BV1})
    list(GET BVLIST 0 nanoFramework_VERSION_MAJOR)
    list(GET BVLIST 1 nanoFramework_VERSION_MINOR)
    list(GET BVLIST 2 nanoFramework_VERSION_PATCH)
    list(GET BVLIST 3 nanoFramework_VERSION_TWEAK)

    #       
    # Continue build with standard NRF/Zephyr build 
    #
    # During this invocation, the NF will be built as a zephyr 'module'. 
    # This invokes .\tarrgets\Zephyr\module\cmakelists.txt which does the 
    # work to build the nanoframework.
    #
    message(STATUS "========= Building Zephyr board=${BOARD}   ZEPHYR_BASE >$ENV{ZEPHYR_BASE}<   build_version >${BUILD_VERSION}<")
    find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
    message(STATUS "========= Zephyr cmakelist.txt return.")

    # At this point, the nanoframework module (library) has been built. What remains is to build
    # a trivial app that invokes the NF runtime.

    zephyr_include_directories("../module/nf_lib/include")

    # The Zephyr build system requires project to be defined here
    project(nanoFramework VERSION ${BUILD_VERSION} LANGUAGES C CXX ASM)

    zephyr_compile_definitions(PROJECT_NAME=${PROJECT_NAME})

    # need this for NF build
    zephyr_compile_definitions(PLATFORM_ARM=1)

    if(APP_VERSION)
        zephyr_compile_definitions(APP_VERSION=${APP_VERSION})
    endif()

endmacro()

# macro to clear binary files related with nanoBooter from output
# to make sure that the build file it's up to date
macro(nf_clear_output_files_nanobooter)
    nf_clear_common_output_files_nanobooter()
    # other files specific to this platform should go here
endmacro()

# macro to clear binary files related with nanoCLR from output
# to make sure that the build file it's up to date
macro(nf_clear_output_files_nanoclr)
    nf_clear_common_output_files_nanoclr()
    # other files specific to this platform should go here
endmacro()
