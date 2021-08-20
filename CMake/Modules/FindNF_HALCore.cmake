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
# TARGET parameter to set the target it's building
# optional EXTRA_INCLUDES with include paths to be added to the library
# optional EXTRA_COMPILE_DEFINITIONS with compiler definitions to be added to the library
macro(nf_add_lib_halcore)

    # parse arguments
    cmake_parse_arguments(_ "" "TARGET" "EXTRA_INCLUDES;EXTRA_COMPILE_DEFINITIONS" ${ARGN})

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

    nf_set_compile_options(${LIB_NAME})
    nf_set_compile_definitions(TARGET ${LIB_NAME} BUILD_TARGET ${__TARGET} EXTRA_COMPILE_DEFINITIONS ${__EXTRA_COMPILE_DEFINITIONS})
    
    # add alias
    add_library("nano::${LIB_NAME}" ALIAS ${LIB_NAME})
    
endmacro()
