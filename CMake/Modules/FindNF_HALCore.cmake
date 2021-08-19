#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# set include directories for nanoHAL Core

# include directories for nanoHAL Core
list(APPEND NF_HALCore_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/CLR/Include)
list(APPEND NF_HALCore_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/HAL/Include)
list(APPEND NF_HALCore_INCLUDE_DIRS ${BASE_PATH_FOR_PLATFORM}/Include)
list(APPEND NF_HALCore_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/targets/${RTOS}/${TARGET_BOARD})

if(RTOS_CHIBIOS_CHECK)
    
    # need this to include chversion.h
    include(FetchContent)
    FetchContent_GetProperties(chibios)

    list(APPEND NF_HALCore_INCLUDE_DIRS ${chibios_SOURCE_DIR}/os/license)

endif()

# source files for nanoHAL Core
set(NF_HALCore_SRCS

    # Core
    nanoHAL_Capabilites.c
    nanoHAL_Boot.c

)

# message("BASE_PATH_FOR_PLATFORM >> ${BASE_PATH_FOR_PLATFORM}") # debug helper
foreach(SRC_FILE ${NF_HALCore_SRCS})

    set(NF_HALCore_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(NF_HALCore_SRC_FILE ${SRC_FILE}
        PATHS 
            
            # Core
            ${CMAKE_SOURCE_DIR}/src/HAL

            # target
            ${BASE_PATH_FOR_PLATFORM}/common

            # target
            ${TARGET_BASE_LOCATION}

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${NF_HALCore_SRC_FILE}")
    endif()

    list(APPEND NF_HALCore_SOURCES ${NF_HALCore_SRC_FILE})
    
endforeach()

set_source_files_properties(${CMAKE_SOURCE_DIR}/src/HAL/nanoHAL_Capabilites.c PROPERTIES COMPILE_FLAGS "-fno-inline-functions -fno-inline-small-functions -fno-inline-functions-called-once")
set_source_files_properties(${CMAKE_SOURCE_DIR}/src/HAL/nanoHAL_Boot.c PROPERTIES COMPILE_FLAGS "-fno-inline-functions -fno-inline-small-functions -fno-inline-functions-called-once")

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(NF_HALCore DEFAULT_MSG NF_HALCore_INCLUDE_DIRS NF_HALCore_SOURCES)


# macro to be called from binutils to add network library
# optional EXTRA_INCLUDES with include paths to be added to the library
# optional EXTRA_COMPILER_DEFINITIONS with compiler definitions to be added to the library
# TARGET parameter to set the target it's building
macro(nf_add_lib_halcore)

    # parse arguments
    cmake_parse_arguments(_ "" "TARGET" "EXTRA_INCLUDES;EXTRA_COMPILER_DEFINITIONS" ${ARGN})

    if(NOT __TARGET OR "${__TARGET}" STREQUAL "")
        message(FATAL_ERROR "Need to set TARGET argument when calling nf_add_lib_halcore()")
    endif()

    # add this has a library
    set(LIB_NAME NF_HALCore_${__TARGET})

    add_library(
        ${LIB_NAME} STATIC 
            ${NF_HALCore_SOURCES})   

    target_include_directories(
        ${LIB_NAME} 
        PUBLIC 
            ${NF_HALCore_INCLUDE_DIRS}
            ${__EXTRA_INCLUDES})
    
    if("${__TARGET}" STREQUAL "${NANOBOOTER_PROJECT_NAME}")

        target_compile_definitions(
            ${LIB_NAME} PUBLIC
            -DI_AM_NANOBOOTER
        )

    endif()

    if("${__TARGET}" STREQUAL "${NANOCLR_PROJECT_NAME}")

        target_compile_definitions(
            ${LIB_NAME} PUBLIC
            -DI_AM_NANOCLR
        )

    endif()

    if(RTOS_FREERTOS_ESP32_CHECK)

        # this is the only one different
        target_compile_definitions(
                ${LIB_NAME} PUBLIC
                -DPLATFORM_ESP32
            )

    else()
        # use the general ones for the target
        # TODO OK to move outside of this IF() afterwards
        nf_set_compiler_options(${LIB_NAME})

        # all the others are ARM
        target_compile_definitions(
            ${LIB_NAME} PUBLIC
            -DPLATFORM_ARM
        )

    endif()

    # add alias
    add_library("nano::${LIB_NAME}" ALIAS ${LIB_NAME})
    
endmacro()
