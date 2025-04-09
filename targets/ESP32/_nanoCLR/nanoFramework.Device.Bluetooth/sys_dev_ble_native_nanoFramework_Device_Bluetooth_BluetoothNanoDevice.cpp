// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_ble_native.h"

BluetoothNanoDevice_Mode ble_operatingMode = BluetoothNanoDevice_Mode_NotRunning;

char *bleDeviceName = NULL;

void UpdateNameInContext()
{
    if (bleContext.pDeviceName)
    {
        platform_free((void *)bleContext.pDeviceName);
    }

    size_t nlen = hal_strlen_s(bleDeviceName);

    bleContext.pDeviceName = (char *)platform_malloc(nlen + 1);
    memcpy(bleContext.pDeviceName, bleDeviceName, nlen);
    bleContext.pDeviceName[nlen] = 0;
}

void SetDeviceName(const char *deviceName)
{
    if (bleDeviceName)
    {
        platform_free(bleDeviceName);
        bleDeviceName = NULL;
    }

    size_t nlen = hal_strlen_s(deviceName);

    bleDeviceName = (char *)platform_malloc(nlen + 1);
    hal_strcpy_s(bleDeviceName, nlen + 1, deviceName);

    UpdateNameInContext();
}

HRESULT Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_BluetoothNanoDevice::NativeInitilise___STATIC__VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    const char *defaultName = "nanoFramework";

    // Set default device name
    SetDeviceName((char *)defaultName);

    if (ble_operatingMode != BluetoothNanoDevice_Mode_NotRunning)
    {
        StopBleStack();
        ble_operatingMode = BluetoothNanoDevice_Mode_NotRunning;
    }

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_BluetoothNanoDevice::
    NativeSetOperationMode___STATIC__VOID__nanoFrameworkDeviceBluetoothBluetoothNanoDeviceMode__STRING__U2(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        bool result = false;
#if defined(NANO_BLE_DEBUG)
        BluetoothNanoDevice_Mode last_operatingMode = ble_operatingMode;
#endif

        BluetoothNanoDevice_Mode newRunMode = (BluetoothNanoDevice_Mode)stack.Arg0().NumericByRef().u4;
        char *deviceName = (char *)stack.Arg1().RecoverString();
        uint16_t appearance = stack.Arg2().NumericByRef().u2;

        SetDeviceName(deviceName);

        if (ble_operatingMode != newRunMode)
        {
            ble_operatingMode = newRunMode;

            BLE_DEBUG_PRINTF("run mode %d/%d device name %s\n", ble_operatingMode, last_operatingMode, bleDeviceName);
            switch (ble_operatingMode)
            {
                case BluetoothNanoDevice_Mode_NotRunning:
                    result = StopBleStack();
                    break;

                case BluetoothNanoDevice_Mode_Server:
                case BluetoothNanoDevice_Mode_Client:
                    result = StartBleStack(deviceName, appearance);
                    break;
            }

            if (!result)
            {
                BLE_DEBUG_PRINTF(
                    "run mode %d/%d device name %s\n",
                    ble_operatingMode,
                    last_operatingMode,
                    bleDeviceName);
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
            }
        }
    }
    NANOCLR_NOCLEANUP();
}