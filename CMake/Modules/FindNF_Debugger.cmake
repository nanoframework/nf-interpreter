#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# set include directories for nanoFramework Debugger
list(APPEND NF_Debugger_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/CLR/Debugger)
list(APPEND NF_Debugger_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/CLR/Messaging)
list(APPEND NF_Debugger_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/CLR/WireProtocol)

# source files for nanoFramework Debugger
set(NF_Debugger_SRCS

    # debugger
    Debugger.cpp
    Messaging.cpp
)

# add the debugger source file according to the build flavor
if(NF_BUILD_RTM)
    set(NF_Debugger_SRCS ${NF_Debugger_SRCS} Debugger_minimal.cpp)
else()
    set(NF_Debugger_SRCS ${NF_Debugger_SRCS} Debugger_full.cpp)
endif()


foreach(SRC_FILE ${NF_Debugger_SRCS})

    set(NF_Debugger_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(NF_Debugger_SRC_FILE ${SRC_FILE}
        PATHS 
            ${CMAKE_SOURCE_DIR}/src/CLR/Debugger
            ${CMAKE_SOURCE_DIR}/src/CLR/Messaging
            ${CMAKE_SOURCE_DIR}/src/CLR/WireProtocol
            ${CMAKE_SOURCE_DIR}/src/CLR/Core

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${NF_Debugger_SRC_FILE}")
    endif()

    list(APPEND NF_Debugger_SOURCES ${NF_Debugger_SRC_FILE})
    
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(NF_Debugger DEFAULT_MSG NF_Debugger_INCLUDE_DIRS NF_Debugger_SOURCES)

# macro to be called from binutils to add Debugger library
# optional EXTRA_INCLUDES with include paths to be added to the library
# optional EXTRA_COMPILE_DEFINITIONS with compiler definitions to be added to the library
macro(nf_add_lib_debugger)

    # parse arguments
    cmake_parse_arguments(NFALD "" "" "EXTRA_INCLUDES;EXTRA_COMPILE_DEFINITIONS" ${ARGN})

    # add this has a library
    set(LIB_NAME NF_Debugger)

    add_library(
        ${LIB_NAME} STATIC 
            ${NF_Debugger_SOURCES})   

    target_include_directories(
        ${LIB_NAME} 
        PUBLIC 
            ${NF_Debugger_INCLUDE_DIRS}
            ${NF_CoreCLR_INCLUDE_DIRS}
            ${NFALD_EXTRA_INCLUDES})   

    # TODO can be removed later
    if(RTOS_ESP32_CHECK)

        nf_common_compiler_definitions(TARGET ${LIB_NAME} BUILD_TARGET ${NANOCLR_PROJECT_NAME})

        # this is the only one different
        target_compile_definitions(
            ${LIB_NAME} PUBLIC
            -DPLATFORM_ESP32
            ${NFALD_EXTRA_COMPILER_DEFINITIONS}
        )

    else() 
        nf_set_compile_options(TARGET ${LIB_NAME} BUILD_TARGET ${NANOCLR_PROJECT_NAME})
        nf_set_compile_definitions(TARGET ${LIB_NAME} EXTRA_COMPILE_DEFINITIONS ${NFALD_EXTRA_COMPILE_DEFINITIONS} BUILD_TARGET ${NANOCLR_PROJECT_NAME})
        nf_set_link_options(TARGET ${LIB_NAME})
    endif()

    # add alias
    add_library("nano::${LIB_NAME}" ALIAS ${LIB_NAME})

    # debugger needs Wire Protocol
    add_dependencies(${LIB_NAME} nano::WireProtocol)

endmacro()
