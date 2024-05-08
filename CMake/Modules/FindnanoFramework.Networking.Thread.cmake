#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
set(BASE_PATH_FOR_THIS_MODULE "${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/nanoFramework.Networking.Thread")

# set include directories
list(APPEND nanoFramework.Networking.Thread_INCLUDE_DIRS ${BASE_PATH_FOR_THIS_MODULE})
list(APPEND nanoFramework.Networking.Thread_INCLUDE_DIRS ${TARGET_BASE_LOCATION})
list(APPEND nanoFramework.Networking.Thread_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/nanoFramework.Networking.Thread)

# source files
set(nanoFramework.Networking.Thread_SRCS
    net_thread_native.cpp
    net_thread_native_nanoFramework_Networking_Thread_OpenThread.cpp
)

foreach(SRC_FILE ${nanoFramework.Networking.Thread_SRCS})

    set(nanoFramework.Networking.Thread_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(nanoFramework.Networking.Thread_SRC_FILE ${SRC_FILE}
        PATHS 
            ${BASE_PATH_FOR_THIS_MODULE}
            ${TARGET_BASE_LOCATION}
            ${CMAKE_SOURCE_DIR}/src/nanoFramework.Networking.Thread

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${nanoFramework.Networking.Thread_SRC_FILE}")
    endif()
    
    list(APPEND nanoFramework.Networking.Thread_SOURCES ${nanoFramework.Networking.Thread_SRC_FILE})

endforeach()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(nanoFramework.Networking.Thread DEFAULT_MSG nanoFramework.Networking.Thread_INCLUDE_DIRS nanoFramework.Networking.Thread_SOURCES)
