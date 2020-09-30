#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#


# set include directories
list(APPEND nanoFramework.System.Collections_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/HAL/Include")
list(APPEND nanoFramework.System.Collections_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/nanoFramework.System.Collections")


# the following CLR support files are required and added to the build by FindNF_CoreCLR.cmake
# CLR_RT_HeapBlock_Queue.cpp
# CLR_RT_HeapBlock_Stack.cpp

# source files
set(nanoFramework.System.Collections_SRCS

    # source files of the assembly
    nf_system_collections_System_Collections_Queue.cpp
    nf_system_collections_System_Collections_Stack.cpp
    nf_system_collections.cpp
)

foreach(SRC_FILE ${nanoFramework.System.Collections_SRCS})
    set(nanoFramework.System.Collections_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(nanoFramework.System.Collections_SRC_FILE ${SRC_FILE}
        PATHS

            # path for source files of this module
            ${PROJECT_SOURCE_DIR}/src/nanoFramework.System.Collections

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${nanoFramework.System.Collections_SRC_FILE}") # debug helper
    list(APPEND nanoFramework.System.Collections_SOURCES ${nanoFramework.System.Collections_SRC_FILE})
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(nanoFramework.System.Collections DEFAULT_MSG nanoFramework.System.Collections_INCLUDE_DIRS nanoFramework.System.Collections_SOURCES)
