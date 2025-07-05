//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <sys_dev_usbstream_native.h>
#include "sys_dev_usbstream_native_target.h"

NF_PAL_USB UsbStream_PAL;

static void UsbAsyncWriteCompleted(
    uint8_t class_nbr,
    void *p_buf,
    uint32_t buf_len,
    uint32_t xfer_len,
    void *p_callback_arg,
    sl_status_t status)
{
    (void)class_nbr;
    (void)p_buf;
    (void)buf_len;
    (void)status;

    NATIVE_INTERRUPT_START

    NF_PAL_USB *usbPal = (NF_PAL_USB *)p_callback_arg;

    // process this only IF the operation wasn't aborted
    // AND we're expecting an event
    if (status != SL_STATUS_ABORT && usbPal->WaitingForTxEvent)
    {
        // store TX count
        usbPal->TxBytesSent = xfer_len;

        Events_Set(SYSTEM_EVENT_FLAG_USB_OUT);
    }

    // clear the flag
    usbPal->WaitingForTxEvent = false;

    NATIVE_INTERRUPT_END
}

static void UsbAsyncReadCompleted(
    uint8_t class_nbr,
    void *p_buf,
    uint32_t buf_len,
    uint32_t xfer_len,
    void *p_callback_arg,
    sl_status_t status)
{
    (void)class_nbr;
    (void)p_buf;
    (void)buf_len;

    NATIVE_INTERRUPT_START

    NF_PAL_USB *usbPal = (NF_PAL_USB *)p_callback_arg;

    // process this only IF the operation wasn't aborted
    // AND we're expecting an event
    if (status != SL_STATUS_ABORT && usbPal->WaitingForRxEvent)
    {
        // store RX count
        usbPal->RxBytesReceived = xfer_len;

        Events_Set(SYSTEM_EVENT_FLAG_USB_IN);
    }

    // clear the flag
    usbPal->WaitingForRxEvent = false;

    NATIVE_INTERRUPT_END
}

// -- //

HRESULT Library_sys_dev_usbstream_native_System_Device_Usb_UsbStream::Read___I4__SZARRAY_U1__I4__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Array *dataBuffer;
    CLR_RT_HeapBlock hbTimeout;
    int64_t *timeoutTicks;
    bool eventResult = true;

    uint8_t *data;
    uint32_t length = 0;
    uint32_t count = 0;
    uint32_t offset = 0;
    sl_status_t reqStatus;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    if ((bool)pThis[FIELD___disposed].NumericByRef().u1)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    // perform parameter validation and setup TX operation

    // dereference the data buffer from the argument
    dataBuffer = stack.Arg1().DereferenceArray();
    FAULT_ON_NULL_ARG(dataBuffer);
    dataBuffer->Pin();

    offset = stack.Arg2().NumericByRef().s4;
    count = stack.Arg3().NumericByRef().s4;

    // get the size of the buffer
    length = dataBuffer->m_numOfElements;

    // check parameters
    if ((offset > length) || (count > length))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }

    if (offset + count > length)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // get a the pointer to the array by using the offset
    data = dataBuffer->GetElement(offset);

    // setup timeout from managed property
    hbTimeout.SetInteger((CLR_INT64)pThis[FIELD___readTimeout].NumericByRef().s4 * TIME_CONVERSION__TO_MILLISECONDS);
    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeoutTicks));

    // this is a long running operation...
    if (stack.m_customState == 1)
    {
        // ... and hasn't started yet

        // bump custom state
        stack.m_customState = 2;

        // clear RX counter
        UsbStream_PAL.RxBytesReceived = 0;

        // set event flag
        UsbStream_PAL.WaitingForRxEvent = true;

        // start read operation with async API
        reqStatus = sl_usbd_vendor_read_bulk_async(
            sl_usbd_vendor_winusb_number,
            (void *)data,
            count,
            UsbAsyncReadCompleted,
            &UsbStream_PAL);

        if (reqStatus == SL_STATUS_INVALID_STATE)
        {
            // device is not connected, return exception
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
        }
        else if (reqStatus == SL_STATUS_NOT_READY)
        {
            // transfer already in progress, return exception
            NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
        }
    }

    while (eventResult)
    {
        // non-blocking wait allowing other threads to run while we wait for the USB operation to complete
        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeoutTicks, Event_UsbIn, eventResult));

        if (eventResult)
        {
            // if different than expected, abort anyway
            if (UsbStream_PAL.RxBytesReceived != count)
            {
                // cancel the async operation...
                sl_usbd_vendor_abort_read_bulk(sl_usbd_vendor_winusb_number);
            }

            // done here
            break;
        }
        else
        {
            // timeout has expired
            // cancel the async operation...
            sl_usbd_vendor_abort_read_bulk(sl_usbd_vendor_winusb_number);

            // ... return exception
            NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
        }
    }

    // pop timeout heap block from stack
    stack.PopValue();

    // set result with count of bytes received
    stack.SetResult_I4(UsbStream_PAL.RxBytesReceived);

    NANOCLR_CLEANUP();

    if (hr != CLR_E_THREAD_WAITING)
    {
        // need to clean up the buffer, if this was not rescheduled
        if (dataBuffer != NULL && dataBuffer->IsPinned())
        {
            dataBuffer->Unpin();
        }
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Library_sys_dev_usbstream_native_System_Device_Usb_UsbStream::Write___VOID__SZARRAY_U1__I4__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Array *dataBuffer;
    CLR_RT_HeapBlock hbTimeout;
    int64_t *timeoutTicks;
    int16_t timeoutMiliseconds = 0;
    bool eventResult = true;

    uint8_t *data;
    uint32_t length = 0;
    uint32_t count = 0;
    uint32_t offset = 0;
    sl_status_t reqStatus;
    uint32_t xfer_len = 0;
    bool isLongRunning = false;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    if ((bool)pThis[FIELD___disposed].NumericByRef().u1)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    // perform parameter validation and setup TX operation

    // dereference the data buffer from the argument
    dataBuffer = stack.Arg1().DereferenceArray();
    FAULT_ON_NULL_ARG(dataBuffer);
    dataBuffer->Pin();

    offset = stack.Arg2().NumericByRef().s4;
    count = stack.Arg3().NumericByRef().s4;

    // get the size of the buffer
    length = dataBuffer->m_numOfElements;

    // check parameters
    FAULT_ON_NULL_ARG(dataBuffer);

    if ((offset > length) || (count > length))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }

    if (offset + count > length)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // get a the pointer to the array by using the offset
    data = dataBuffer->GetElement(offset);

    // setup timeout from managed property
    hbTimeout.SetInteger((CLR_INT64)pThis[FIELD___writeTimeout].NumericByRef().s4 * TIME_CONVERSION__TO_MILLISECONDS);
    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeoutTicks));

    // check if it's worth to use async API
    if (count > 64)
    {
        isLongRunning = true;
    }

    // this is a long running operation...
    if (isLongRunning && stack.m_customState == 1)
    {
        // ... and hasn't started yet

        // bump custom state
        stack.m_customState = 2;

        // clear TX counter
        UsbStream_PAL.TxBytesSent = 0;

        // set event flag
        UsbStream_PAL.WaitingForTxEvent = true;

        // start write operation with async API
        // requesting handling of "End-of-transfer"
        reqStatus = sl_usbd_vendor_write_bulk_async(
            sl_usbd_vendor_winusb_number,
            (void *)data,
            count,
            UsbAsyncWriteCompleted,
            &UsbStream_PAL,
            false);

        if (reqStatus == SL_STATUS_INVALID_STATE)
        {
            // device is not connected, return exception
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
        }
        else if (reqStatus == SL_STATUS_NOT_READY)
        {
            // transfer already in progress, return exception
            NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
        }
    }
    else
    {
        if (*timeoutTicks != TIMEOUT_INFINITE)
        {
            // convert from ticks to milliseconds
            timeoutMiliseconds = (int16_t)pThis[FIELD___writeTimeout].NumericByRef().s4;
        }

        // start write operation with async API
        // requesting handling of "End-of-transfer"
        reqStatus = sl_usbd_vendor_write_bulk_sync(
            sl_usbd_vendor_winusb_number,
            (void *)data,
            count,
            timeoutMiliseconds,
            false,
            &xfer_len);
    }

    while (isLongRunning && eventResult)
    {
        // non-blocking wait allowing other threads to run while we wait for the USB operation to complete
        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeoutTicks, Event_UsbOut, eventResult));

        if (eventResult)
        {
            // done here
            break;
        }
        else
        {
            // timeout has expired
            // cancel the async operation...
            sl_usbd_vendor_abort_write_bulk(sl_usbd_vendor_winusb_number);

            // ... return exception
            NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
        }
    }

    // pop timeout heap block from stack
    stack.PopValue();

    NANOCLR_CLEANUP();

    if (isLongRunning && hr != CLR_E_THREAD_WAITING)
    {
        // need to clean up the buffer, if this was not rescheduled
        if (dataBuffer != NULL && dataBuffer->IsPinned())
        {
            dataBuffer->Unpin();
        }
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Library_sys_dev_usbstream_native_System_Device_Usb_UsbStream::get_IsConnected___BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    // default to false
    bool conn = false;

    // don't care about return value as we'll just return false if the connection state can't be determined
    sl_usbd_vendor_is_enabled(sl_usbd_vendor_winusb_number, &conn);
    stack.SetResult_Boolean(conn);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_dev_usbstream_native_System_Device_Usb_UsbStream::NativeClose___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    (void)stack;

    // abort any transfer in progress, just in case
    sl_usbd_vendor_abort_write_bulk(sl_usbd_vendor_winusb_number);
    sl_usbd_vendor_abort_read_bulk(sl_usbd_vendor_winusb_number);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_dev_usbstream_native_System_Device_Usb_UsbStream::NativeOpen___I4__STRING__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    const char *deviceDescription;
    const char *deviceClassGuid;

    // int32_t bufferSize;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // get device class GUID
    deviceClassGuid = stack.Arg1().RecoverString();

    // clear destination
    memset(UsbClassVendorDeviceInterfaceGuid, 0, sizeof(UsbClassVendorDeviceInterfaceGuid));

    for (uint16_t i = 0; i < sizeof(UsbClassVendorDeviceInterfaceGuid); i += 2)
    {
        UsbClassVendorDeviceInterfaceGuid[i] = *deviceClassGuid++;
    }

    // get description
    deviceDescription = stack.Arg2().RecoverString();
    FAULT_ON_NULL(deviceDescription);

    // validate device description length
    if (hal_strlen_s(deviceDescription) > sizeof(UsbClassVendorDescription) - 1)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // store device description
    hal_strcpy_s(UsbClassVendorDescription, sizeof(UsbClassVendorDescription), deviceDescription);

    // also update USB device product string
    sl_usbd_vendor_update_device_product_string((const char *)UsbClassVendorDescription);

    if (sli_usbd_vendor_winusb_init() != SL_STATUS_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    stack.SetResult_I4(sl_usbd_vendor_winusb_number);

    NANOCLR_NOCLEANUP();
}
