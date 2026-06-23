# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
set(BASE_PATH_FOR_THIS_MODULE ${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/nanoFramework.EspNow)


# set include directories
list(APPEND nanoFramework.EspNow_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Core)
list(APPEND nanoFramework.EspNow_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Include)
list(APPEND nanoFramework.EspNow_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/HAL/Include)
list(APPEND nanoFramework.EspNow_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/PAL/Include)
list(APPEND nanoFramework.EspNow_INCLUDE_DIRS ${BASE_PATH_FOR_THIS_MODULE})
list(APPEND nanoFramework.EspNow_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/nanoFramework.EspNow)

# source files
set(nanoFramework.EspNow_SRCS
    nanoFramework_espnow_native.cpp
    nanoFramework_espnow_native_nanoFramework_Espnow_EspNowController.cpp
    nanoFramework_espnow_native_nanoFramework_Espnow_EspNowController__EspNowEventHandler.cpp
)

foreach(SRC_FILE ${nanoFramework.EspNow_SRCS})
    set(nanoFramework.EspNow_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(nanoFramework.EspNow_SRC_FILE ${SRC_FILE}
        PATHS
            ${BASE_PATH_FOR_THIS_MODULE}
            ${TARGET_BASE_LOCATION}
            ${PROJECT_SOURCE_DIR}/src/nanoFramework.EspNow
        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${nanoFramework.EspNow_SRC_FILE}")
    endif()

    list(APPEND nanoFramework.EspNow_SOURCES ${nanoFramework.EspNow_SRC_FILE})
endforeach()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(nanoFramework.EspNow DEFAULT_MSG nanoFramework.EspNow_INCLUDE_DIRS nanoFramework.EspNow_SOURCES)
