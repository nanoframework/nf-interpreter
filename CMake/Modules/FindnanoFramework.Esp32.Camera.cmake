#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

set(BASE_PATH_FOR_THIS_MODULE ${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/nanoFramework.Esp32.Camera)

list(APPEND nanoFramework.Esp32.Camera_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/CLR/Core)
list(APPEND nanoFramework.Esp32.Camera_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/CLR/Include)
list(APPEND nanoFramework.Esp32.Camera_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/HAL/Include)
list(APPEND nanoFramework.Esp32.Camera_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/PAL/Include)
list(APPEND nanoFramework.Esp32.Camera_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/esp32-camera/driver/include)
list(APPEND nanoFramework.Esp32.Camera_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/esp32-camera/conversions/include)
list(APPEND nanoFramework.Esp32.Camera_INCLUDE_DIRS ${IDF_PATH_CMAKED}/components/esp_jpeg/include)
list(APPEND nanoFramework.Esp32.Camera_INCLUDE_DIRS ${BASE_PATH_FOR_THIS_MODULE})
list(APPEND nanoFramework.Esp32.Camera_INCLUDE_DIRS ${TARGET_BASE_LOCATION}/nanoFramework.Esp32.Camera)

set(nanoFramework.Esp32.Camera_SRCS
    nano_esp32_cam.cpp
    nano_esp32_cam_nanoFramework_Esp32_Camera_Esp32Camera.cpp
)

foreach(SRC_FILE ${nanoFramework.Esp32.Camera_SRCS})
    set(nanoFramework.Esp32.Camera_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(nanoFramework.Esp32.Camera_SRC_FILE ${SRC_FILE}
        PATHS
            ${BASE_PATH_FOR_THIS_MODULE}
            ${TARGET_BASE_LOCATION}
            ${TARGET_BASE_LOCATION}/nanoFramework.Esp32.Camera
        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if(BUILD_VERBOSE)
        message("${SRC_FILE} >> ${nanoFramework.Esp32.Camera_SRC_FILE}")
    endif()

    list(APPEND nanoFramework.Esp32.Camera_SOURCES ${nanoFramework.Esp32.Camera_SRC_FILE})
endforeach()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(
    nanoFramework.Esp32.Camera
    DEFAULT_MSG
    nanoFramework.Esp32.Camera_INCLUDE_DIRS
    nanoFramework.Esp32.Camera_SOURCES
)
