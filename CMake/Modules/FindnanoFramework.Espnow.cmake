#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
set(BASE_PATH_FOR_THIS_MODULE ${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/nanoFramework.Espnow)


# set include directories
list(APPEND nanoFramework.Espnow_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Core)
list(APPEND nanoFramework.Espnow_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Include)
list(APPEND nanoFramework.Espnow_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/HAL/Include)
list(APPEND nanoFramework.Espnow_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/PAL/Include)
list(APPEND nanoFramework.Espnow_INCLUDE_DIRS ${BASE_PATH_FOR_THIS_MODULE})
list(APPEND nanoFramework.Espnow_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/nanoFramework.Espnow)

# source files
set(nanoFramework.Espnow_SRCS

    nanoFramework_espnow_native.cpp


    nanoFramework_espnow_native_nanoFramework_Espnow_EspNowController.cpp
    nanoFramework_espnow_native_nanoFramework_Espnow_EspNowController__EspNowEventHandler.cpp

)

foreach(SRC_FILE ${nanoFramework.Espnow_SRCS})
    set(nanoFramework.Espnow_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(nanoFramework.Espnow_SRC_FILE ${SRC_FILE}
        PATHS
	        ${BASE_PATH_FOR_THIS_MODULE}
	        ${TARGET_BASE_LOCATION}
            ${PROJECT_SOURCE_DIR}/src/nanoFramework.Espnow

	    CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${nanoFramework.Espnow_SRC_FILE}") # debug helper
    list(APPEND nanoFramework.Espnow_SOURCES ${nanoFramework.Espnow_SRC_FILE})
endforeach()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(nanoFramework.Espnow DEFAULT_MSG nanoFramework.Espnow_INCLUDE_DIRS nanoFramework.Espnow_SOURCES)
