//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_ble_native.h"

extern EventGroupHandle_t ble_event_waitgroup;

HRESULT Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattReadRequest::
    NativeReadRespondWithValue___VOID__U2__SZARRAY_U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock_Array *buffer;
        uint8_t *bufPtr;
        int bufLen;
        int rc;

        // debug_printf("NativeReadRespondWithValue\n");

        // Response to correct event, or is it too late
        // Otherwise ignore
        if (ble_event_data.eventId == stack.Arg1().NumericByRef().u2)
        {
            // correct event
            buffer = stack.Arg2().DereferenceArray();
            FAULT_ON_NULL(buffer);

            bufPtr = buffer->GetFirstElement();
            bufLen = buffer->m_numOfElements;

            // debug_printf("NativeReadRespondWithValue data length %d\n",bufLen);

            rc = os_mbuf_append(ble_event_data.ctxt->om, bufPtr, bufLen);
            ble_event_data.result = rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;

            // Signal BLE callback, event complete
            xEventGroupSetBits(ble_event_waitgroup, 1);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattReadRequest::
    NativeReadRespondWithProtocolError___VOID__U2__U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        // Response to correct event, or is it too late
        // Otherwise ignore
        if (ble_event_data.eventId == stack.Arg1().NumericByRef().u2)
        {
            // Get protocol error code
            ble_event_data.result = stack.Arg2().NumericByRef().u2;

            // Signal BLE callback, event complete
            xEventGroupSetBits(ble_event_waitgroup, 1);
        }
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}
