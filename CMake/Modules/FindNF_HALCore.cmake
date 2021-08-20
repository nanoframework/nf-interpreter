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

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(NF_HALCore DEFAULT_MSG NF_HALCore_INCLUDE_DIRS NF_HALCore_SOURCES)
