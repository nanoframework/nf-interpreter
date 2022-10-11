//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_usbstream_native.h"
#include "sys_dev_usbstream_native_target.h"

HRESULT Library_sys_dev_usbstream_native_System_Device_Usb_UsbClient::get_IsConnected___BOOLEAN( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    bool conn;
    
    if(sl_usbd_vendor_is_enabled(sl_usbd_vendor_winusb_number, &conn) == SL_STATUS_OK)
    {
        stack.SetResult_Boolean(conn);
    }
    else
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    NANOCLR_NOCLEANUP();
}
