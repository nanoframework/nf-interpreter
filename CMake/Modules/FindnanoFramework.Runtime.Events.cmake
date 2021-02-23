#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
set(BASE_PATH_FOR_THIS_MODULE "${CMAKE_SOURCE_DIR}/src/nanoFramework.Runtime.Events")


# set include directories
list(APPEND nanoFramework.Runtime.Events_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/src/CLR/Core")
list(APPEND nanoFramework.Runtime.Events_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/src/CLR/Include")
list(APPEND nanoFramework.Runtime.Events_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/src/HAL/Include")
list(APPEND nanoFramework.Runtime.Events_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/src/PAL/Include")
list(APPEND nanoFramework.Runtime.Events_INCLUDE_DIRS "${BASE_PATH_FOR_THIS_MODULE}")


# source files
set(nanoFramework.Runtime.Events_SRCS

    # source files of the assembly
    nf_rt_events_native_nanoFramework_Runtime_Events_EventSink.cpp
    nf_rt_events_native_nanoFramework_Runtime_Events_NativeEventDispatcher.cpp
    nf_rt_events_native_nanoFramework_Runtime_Events_WeakDelegate.cpp    
    nf_rt_events_native.cpp

    # source files
    AsyncCompletions.cpp
    AsyncContinuations.cpp
    nanoPAL_Events_functions.cpp

)

foreach(SRC_FILE ${nanoFramework.Runtime.Events_SRCS})

    set(nanoFramework.Runtime.Events_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(nanoFramework.Runtime.Events_SRC_FILE ${SRC_FILE}
        PATHS

            # path for source files of this module
            ${BASE_PATH_FOR_THIS_MODULE}

            # path for AsyncProcCall and related code
            ${CMAKE_SOURCE_DIR}/src/PAL/AsyncProcCall
            ${CMAKE_SOURCE_DIR}/src/PAL/Events

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${nanoFramework.Runtime.Events_SRC_FILE}")
    endif()

    list(APPEND nanoFramework.Runtime.Events_SOURCES ${nanoFramework.Runtime.Events_SRC_FILE})
    
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(nanoFramework.Runtime.Events DEFAULT_MSG nanoFramework.Runtime.Events_INCLUDE_DIRS nanoFramework.Runtime.Events_SOURCES)
