// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_ble_native.h"

BluetoothNanoDevice_Mode ble_operatingMode = BluetoothNanoDevice_Mode_NotRunning;

char *bleDeviceName = NULL;

void SetDeviceName(char *deviceName)
{
    if (bleDeviceName)
    {
        platform_free(bleDeviceName);
        bleDeviceName = NULL;
    }

    size_t nlen = hal_strlen_s(deviceName);

    bleDeviceName = (char *)platform_malloc(nlen + 1);
    hal_strcpy_s(bleDeviceName, nlen + 1, deviceName);
}

HRESULT Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_BluetoothNanoDevice::NativeInitilise___STATIC__VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    const char *defaultName = "nanoFramework";

    // Set default device name
    SetDeviceName((char *)defaultName);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_BluetoothNanoDevice::
    NativeSetOperationMode___STATIC__VOID__nanoFrameworkDeviceBluetoothBluetoothNanoDeviceMode__STRING(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        BluetoothNanoDevice_Mode last_operatingMode = ble_operatingMode;

        BluetoothNanoDevice_Mode newRunMode = (BluetoothNanoDevice_Mode)stack.Arg0().NumericByRef().u4;

        char *deviceName = (char *)stack.Arg1().RecoverString();

        SetDeviceName(deviceName);

        ble_operatingMode = newRunMode;

        BLE_DEBUG_PRINTF("run mode %d/%d device name %s\n", ble_operatingMode, last_operatingMode, bleDeviceName);

        switch (ble_operatingMode)
        {
        case  BluetoothNanoDevice_Mode_NotRunning:
            if (last_operatingMode == BluetoothNanoDevice_Mode_Client)
            {
                // Only close stack when running in CLient mode
                Device_ble_dispose();
            }
            break;
        
        case  BluetoothNanoDevice_Mode_Server:
            // Stack started in Servic provider , start advertising
            break;

        case  BluetoothNanoDevice_Mode_Client:
            StartStack(deviceName);
            break;
        }
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}
