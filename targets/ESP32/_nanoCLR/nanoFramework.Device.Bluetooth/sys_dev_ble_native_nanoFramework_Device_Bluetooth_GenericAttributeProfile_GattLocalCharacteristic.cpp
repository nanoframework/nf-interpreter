//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_ble_native.h"

extern uint16_t FindHandleIdFromId(ble_gatt_chr_def *characteristicsDefs, int countDefs, uint16_t characteristicId);

//
//  Notify a Client
//  Arg1 - Client Connection
//  Arg2 - Characteristic ID
//  Arg3 - Notify Byte buffer
//
HRESULT Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattLocalCharacteristic::
    NativeNotifyClient___I4__U2__U2__SZARRAY_U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        uint16_t characteristicId;
        uint16_t connectionHandle;
        uint16_t attHandle;
        CLR_RT_HeapBlock_Array *notifyBuffer;
        uint8_t *bufPtr;
        int bufLen;
        // error characteristicId not known
        int rc = 1;

        connectionHandle = stack.Arg1().NumericByRef().u2;
        characteristicId = stack.Arg2().NumericByRef().u2;

        notifyBuffer = stack.Arg3().DereferenceArray();
        FAULT_ON_NULL(notifyBuffer);

        bufPtr = notifyBuffer->GetFirstElement();
        bufLen = notifyBuffer->m_numOfElements;

        // Find attr handle from Characteristic ID
        attHandle =
            FindHandleIdFromId(blecontext.characteristicsDefs, blecontext.characteristicsCount, characteristicId);

        if (attHandle != 0xffff)
        {
            // Send Notify buffer
            struct os_mbuf *om = ble_hs_mbuf_from_flat(bufPtr, bufLen);
            rc = ble_gattc_notify_custom(connectionHandle, attHandle, om);
        }

        stack.SetResult_I4(rc);
    }
    NANOCLR_NOCLEANUP();
}
