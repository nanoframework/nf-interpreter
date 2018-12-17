//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <string.h>
#include <targetPAL.h>
#include "win_dev_i2c_native.h"
#include "Esp32_DeviceMapping.h"


HRESULT Library_win_dev_i2c_native_Windows_Devices_I2c_I2cController::NativeInit___VOID( CLR_RT_StackFrame& stack )
{
    (void)stack;

    NANOCLR_HEADER();
    {

    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_win_dev_i2c_native_Windows_Devices_I2c_I2cController::GetDeviceSelector___STATIC__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
   {
       // declare the device selector string whose max size is "I2C1,I2C2,I2C3," + terminator and init with the terminator
       char deviceSelectorString[ 15 + 1] = { 0 };

       strcat(deviceSelectorString, "I2C1,I2C2");
    
       // because the caller is expecting a result to be returned
       // we need set a return result in the stack argument using the appropriate SetResult according to the variable type (a string here)
       stack.SetResult_String(deviceSelectorString);
   }
   NANOCLR_NOCLEANUP_NOLABEL();
}
