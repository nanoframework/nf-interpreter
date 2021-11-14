//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_ble_native.h"

extern EventGroupHandle_t ble_event_waitgroup;

//
//  Get data written to an attribute
//
HRESULT Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattWriteRequest::
    NativeWriteGetData___SZARRAY_U1__U2(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        uint8_t *pReturnBuffer;
        uint16_t om_len;
        uint16_t out_len;
        uint16_t eventid = stack.Arg1().NumericByRef().u2;

        // Make sure correct event, or is it too late
        // Otherwise ignore
        if (ble_event_data.eventId == eventid)
        {
            // Get length of available data
            om_len = OS_MBUF_PKTLEN(ble_event_data.ctxt->om);

            // Create managed byte array of correct size as per OM buffer
            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(
                stack.PushValueAndClear(),
                om_len,
                g_CLR_RT_WellKnownTypes.m_UInt8));

            // get a pointer to the first object in the array
            pReturnBuffer = stack.TopValue().DereferenceArray()->GetFirstElement();

            // Copy OM buffer to return byte buffer
            ble_hs_mbuf_to_flat(ble_event_data.ctxt->om, pReturnBuffer, om_len, &out_len);
        }
        else
        {
            // Error return empty array, event not found ?
            NANOCLR_CHECK_HRESULT(
                CLR_RT_HeapBlock_Array::CreateInstance(stack.PushValueAndClear(), 0, g_CLR_RT_WellKnownTypes.m_UInt8));
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattWriteRequest::
    NativeWriteRespond___VOID__U2(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        // debug_printf("NativeWriteRespond\n");
        if (ble_event_data.eventId == stack.Arg1().NumericByRef().u2)
        {
            ble_event_data.result = 0;

            // Signal BLE callback, event complete
            xEventGroupSetBits(ble_event_waitgroup, 1);
        }
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_dev_ble_native_nanoFramework_Device_Bluetooth_GenericAttributeProfile_GattWriteRequest::
    NativeWriteRespondWithProtocolError___VOID__U2__U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        // debug_printf("NativeWriteRespondWithProtocolError\n");

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
