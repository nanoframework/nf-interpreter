#
# Copyright (c) 2017 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
set(BASE_PATH_FOR_THIS_MODULE "targets/CMSIS-OS/ChibiOS/nanoCLR/nanoFramework.Runtime.Events")


# set include directories
list(APPEND nanoFramework.Runtime.Events_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/CLR/Core")
list(APPEND nanoFramework.Runtime.Events_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/CLR/Include")
list(APPEND nanoFramework.Runtime.Events_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/HAL/Include")
list(APPEND nanoFramework.Runtime.Events_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/PAL/Include")
list(APPEND nanoFramework.Runtime.Events_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/${BASE_PATH_FOR_THIS_MODULE}")


# source files
set(nanoFramework.Runtime.Events_SRCS

    nanoframework_runtime_events_native_nanoFramework_Runtime_Events_NativeEventDispatcher_mshl.cpp
    nanoframework_runtime_events_native.cpp
)

foreach(SRC_FILE ${nanoFramework.Runtime.Events_SRCS})
    set(nanoFramework.Runtime.Events_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(nanoFramework.Runtime.Events_SRC_FILE ${SRC_FILE}
        PATHS 
            ${PROJECT_SOURCE_DIR}/${BASE_PATH_FOR_THIS_MODULE}

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${nanoFramework.Runtime.Events_SRC_FILE}") # debug helper
    list(APPEND nanoFramework.Runtime.Events_SOURCES ${nanoFramework.Runtime.Events_SRC_FILE})
endforeach()


include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(NF_Debugger DEFAULT_MSG nanoFramework.Runtime.Events_INCLUDE_DIRS nanoFramework.Runtime.Events_SOURCES)
