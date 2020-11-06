#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# set include directories for nanoHAL Core

# include directories for nanoHAL Core
list(APPEND nanoHALCore_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/HAL/Include)
list(APPEND nanoHALCore_INCLUDE_DIRS ${BASE_PATH_FOR_PLATFORM}/Include)

# source files for nanoHAL Core
set(nanoHALCore_SRCS

    # Core
    nanoHAL_Capabilites.c

)
# message("BASE_PATH_FOR_PLATFORM >> ${BASE_PATH_FOR_PLATFORM}") # debug helper
foreach(SRC_FILE ${nanoHALCore_SRCS})
    set(nanoHALCore_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(nanoHALCore_SRC_FILE ${SRC_FILE}
        PATHS 
            
            # Core
            ${PROJECT_SOURCE_DIR}/src/HAL

            # target
            ${BASE_PATH_FOR_PLATFORM}/common

            # target
            "${TARGET_BASE_LOCATION}"

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${nanoHALCore_SRC_FILE}") # debug helper
    list(APPEND nanoHALCore_SOURCES ${nanoHALCore_SRC_FILE})
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(nanoHALCore DEFAULT_MSG nanoHALCore_INCLUDE_DIRS nanoHALCore_SOURCES)
