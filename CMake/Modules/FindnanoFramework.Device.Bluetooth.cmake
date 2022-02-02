#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
set(BASE_PATH_FOR_THIS_MODULE ${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/nanoFramework.Device.Bluetooth)

# set include directories
list(APPEND nanoFramework.Device.Bluetooth_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Core)
list(APPEND nanoFramework.Device.Bluetooth_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Include)
list(APPEND nanoFramework.Device.Bluetooth_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/HAL/Include)
list(APPEND nanoFramework.Device.Bluetooth_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/PAL/Include)
list(APPEND nanoFramework.Device.Bluetooth_INCLUDE_DIRS ${BASE_PATH_FOR_THIS_MODULE})

# set include directories for Esp32 IDF
set(NIMBLE_COMPONENT_PATH "${ESP32_IDF_PATH}/components/bt/host/nimble")
list(APPEND nanoFramework.Device.Bluetooth_INCLUDE_DIRS "${NIMBLE_COMPONENT_PATH}/port/include")
list(APPEND nanoFramework.Device.Bluetooth_INCLUDE_DIRS "${NIMBLE_COMPONENT_PATH}/nimble/nimble/include")
list(APPEND nanoFramework.Device.Bluetooth_INCLUDE_DIRS "${NIMBLE_COMPONENT_PATH}/esp-hci/include")
list(APPEND nanoFramework.Device.Bluetooth_INCLUDE_DIRS "${NIMBLE_COMPONENT_PATH}/nimble/porting/nimble/include")
list(APPEND nanoFramework.Device.Bluetooth_INCLUDE_DIRS "${NIMBLE_COMPONENT_PATH}/nimble/porting/npl/freertos/include")
list(APPEND nanoFramework.Device.Bluetooth_INCLUDE_DIRS "${NIMBLE_COMPONENT_PATH}/nimble/nimble/host/include")
list(APPEND nanoFramework.Device.Bluetooth_INCLUDE_DIRS "${NIMBLE_COMPONENT_PATH}/nimble/nimble/host/util/include")
list(APPEND nanoFramework.Device.Bluetooth_INCLUDE_DIRS "${NIMBLE_COMPONENT_PATH}/nimble/nimble/host/services/gap/include")
list(APPEND nanoFramework.Device.Bluetooth_INCLUDE_DIRS "${NIMBLE_COMPONENT_PATH}/nimble/nimble/host/services/gatt/include")

# source files
set(nanoFramework.Device.Bluetooth_SRCS

    sys_ble_nimble_chars.c
    
    sys_dev_ble_native.cpp

    sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattServiceProvider.cpp
    sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattReadRequest.cpp
    sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattWriteRequest.cpp
    sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattLocalCharacteristic.cpp

    esp32_nimble.cpp
)

foreach(SRC_FILE ${nanoFramework.Device.Bluetooth_SRCS})
    set(nanoFramework.Device.Bluetooth_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(nanoFramework.Device.Bluetooth_SRC_FILE ${SRC_FILE}
        PATHS
	        ${BASE_PATH_FOR_THIS_MODULE}
	        ${TARGET_BASE_LOCATION}
            ${PROJECT_SOURCE_DIR}/src/nanoFramework.Device.Bluetooth

	    CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${nanoFramework.Device.Bluetooth_SRC_FILE}") # debug helper
    list(APPEND nanoFramework.Device.Bluetooth_SOURCES ${nanoFramework.Device.Bluetooth_SRC_FILE})
endforeach()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(nanoFramework.Device.Bluetooth DEFAULT_MSG nanoFramework.Device.Bluetooth_INCLUDE_DIRS nanoFramework.Device.Bluetooth_SOURCES)
