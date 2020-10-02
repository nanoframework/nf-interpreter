#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#


# native code directory
set(BASE_PATH_FOR_THIS_MODULE "${PROJECT_SOURCE_DIR}/src/CLR/System.Math")


# set include directories
list(APPEND System.Math_INCLUDE_DIRS "${BASE_PATH_FOR_THIS_MODULE}")


# source files
set(System.Math_SRCS

    nf_native_system_math.cpp
    nf_native_system_math_System_Math.cpp

)

foreach(SRC_FILE ${System.Math_SRCS})
    set(System.Math_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(System.Math_SRC_FILE ${SRC_FILE}
        PATHS 
            "${BASE_PATH_FOR_THIS_MODULE}"

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${System.Math_SRC_FILE}") # debug helper
    list(APPEND System.Math_SOURCES ${System.Math_SRC_FILE})
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(System.Math DEFAULT_MSG System.Math_INCLUDE_DIRS System.Math_SOURCES)
