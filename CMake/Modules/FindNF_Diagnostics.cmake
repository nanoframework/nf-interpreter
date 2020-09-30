#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# set include directories
list(APPEND NF_Diagnostics_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Diagnostics)
list(APPEND NF_Diagnostics_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Include)

# source files 
set(NF_Diagnostics_SRCS

    Info.cpp
    Profile.cpp
    Profiler.cpp
)

foreach(SRC_FILE ${NF_Diagnostics_SRCS})
    set(NF_Diagnostics_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(NF_Diagnostics_SRC_FILE ${SRC_FILE}
        PATHS 
            ${PROJECT_SOURCE_DIR}/src/CLR/Diagnostics

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${NF_Diagnostics_SRC_FILE}") # debug helper
    list(APPEND NF_Diagnostics_SOURCES ${NF_Diagnostics_SRC_FILE})
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(NF_Diagnostics DEFAULT_MSG NF_Diagnostics_INCLUDE_DIRS NF_Diagnostics_SOURCES)
