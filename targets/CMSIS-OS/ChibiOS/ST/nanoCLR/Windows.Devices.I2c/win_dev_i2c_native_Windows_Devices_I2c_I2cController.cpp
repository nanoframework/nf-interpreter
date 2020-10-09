//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>
#include <string.h>
#include <targetPAL.h>
#include <nanoHAL.h>
#include "win_dev_i2c_native_target.h"


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
       // declare the device selector string whose max size is "I2C1,I2C2,I2C3,I2C4," + terminator and init with the terminator
       char deviceSelectorString[20 + 1] = { 0 };

    #if STM32_I2C_USE_I2C1
        strcat(deviceSelectorString, "I2C1,");
    #endif
    #if STM32_I2C_USE_I2C2
        strcat(deviceSelectorString, "I2C2,");
    #endif
    #if STM32_I2C_USE_I2C3
        strcat(deviceSelectorString, "I2C3,");
    #endif
    #if STM32_I2C_USE_I2C4
        strcat(deviceSelectorString, "I2C4,");
    #endif
        // replace the last comma with a terminator
       deviceSelectorString[hal_strlen_s(deviceSelectorString) - 1] = '\0';

       // because the caller is expecting a result to be returned
       // we need set a return result in the stack argument using the appropriate SetResult according to the variable type (a string here)
       stack.SetResult_String(deviceSelectorString);
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}