#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
set(BASE_PATH_FOR_THIS_MODULE "${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/nanoFramework.Hardware.Esp32.Ble")


# set include directories
list(APPEND nanoFramework.Hardware.Esp32.Ble_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/src/CLR/Core")
list(APPEND nanoFramework.Hardware.Esp32.Ble_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/src/CLR/Include")
list(APPEND nanoFramework.Hardware.Esp32.Ble_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/src/HAL/Include")
list(APPEND nanoFramework.Hardware.Esp32.Ble_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/src/PAL/Include")
list(APPEND nanoFramework.Hardware.Esp32.Ble_INCLUDE_DIRS "${BASE_PATH_FOR_THIS_MODULE}")

# set include directories for Esp32 IDF
set(NIMBLE_COMPONENT_PATH "${ESP32_IDF_PATH}/components/nimble")
list(APPEND nanoFramework.Hardware.Esp32.Ble_INCLUDE_DIRS "${NIMBLE_COMPONENT_PATH}/port/include")
list(APPEND nanoFramework.Hardware.Esp32.Ble_INCLUDE_DIRS "${NIMBLE_COMPONENT_PATH}/nimble/nimble/include")
list(APPEND nanoFramework.Hardware.Esp32.Ble_INCLUDE_DIRS "${NIMBLE_COMPONENT_PATH}/esp-hci/include")
list(APPEND nanoFramework.Hardware.Esp32.Ble_INCLUDE_DIRS "${NIMBLE_COMPONENT_PATH}/nimble/porting/nimble/include")
list(APPEND nanoFramework.Hardware.Esp32.Ble_INCLUDE_DIRS "${NIMBLE_COMPONENT_PATH}/nimble/porting/npl/freertos/include")
list(APPEND nanoFramework.Hardware.Esp32.Ble_INCLUDE_DIRS "${NIMBLE_COMPONENT_PATH}/nimble/nimble/host/include")
list(APPEND nanoFramework.Hardware.Esp32.Ble_INCLUDE_DIRS "${NIMBLE_COMPONENT_PATH}/nimble/nimble/host/util/include")
list(APPEND nanoFramework.Hardware.Esp32.Ble_INCLUDE_DIRS "${NIMBLE_COMPONENT_PATH}/nimble/nimble/host/services/gap/include")
list(APPEND nanoFramework.Hardware.Esp32.Ble_INCLUDE_DIRS "${NIMBLE_COMPONENT_PATH}/nimble/nimble/host/services/gatt/include")

# source files
set(nanoFramework.Hardware.Esp32.Ble_SRCS

    nanoFramework_hardware_esp32_ble.cpp
    nanoFramework_hardware_esp32_ble_native.cpp
    gatt_svr.c
)

foreach(SRC_FILE ${nanoFramework.Hardware.Esp32.Ble_SRCS})

    set(nanoFramework.Hardware.Esp32.Ble_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(nanoFramework.Hardware.Esp32.Ble_SRC_FILE ${SRC_FILE}
        PATHS 
            ${BASE_PATH_FOR_THIS_MODULE}

        CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${nanoFramework.Hardware.Esp32.Ble_SRC_FILE}")
    endif()

    list(APPEND nanoFramework.Hardware.Esp32.Ble_SOURCES ${nanoFramework.Hardware.Esp32.Ble_SRC_FILE})

endforeach()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(nanoFramework.Hardware.Esp32.Ble DEFAULT_MSG nanoFramework.Hardware.Esp32.Ble_INCLUDE_DIRS nanoFramework.Hardware.Esp32.Ble_SOURCES)
