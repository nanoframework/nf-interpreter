//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "win_dev_spi_native.h"

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiController::GetDeviceSelector___STATIC__STRING( CLR_RT_StackFrame& stack )
{
   NANOCLR_HEADER();
   {
       // declare the device selector string whose max size is "SPI1,SPI2" + terminator and init with the terminator
       char deviceSelectorString[ 15 ] = { "SPI1,SPI2" };   // HSPI & VSPI

       // because the caller is expecting a result to be returned
       // we need set a return result in the stack argument using the appropriate SetResult according to the variable type (a string here)
       stack.SetResult_String(deviceSelectorString);
   }
   NANOCLR_NOCLEANUP_NOLABEL();
}
