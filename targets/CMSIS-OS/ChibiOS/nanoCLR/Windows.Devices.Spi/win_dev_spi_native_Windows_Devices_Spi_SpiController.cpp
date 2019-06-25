//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//


#include "win_dev_spi_native_target.h"


HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiController::GetDeviceSelector___STATIC__STRING( CLR_RT_StackFrame& stack )
{
   NANOCLR_HEADER();
   {
       // declare the device selector string whose max size is "SPI1,SPI2,SPI3,SPI4,SPI5,SPI6," + terminator and init with the terminator
       char deviceSelectorString[ 30 + 1] = { 0 };

   #if STM32_SPI_USE_SPI1
       strcat(deviceSelectorString, "SPI1,");
   #endif
   #if STM32_SPI_USE_SPI2
       strcat(deviceSelectorString, "SPI2,");
   #endif
   #if STM32_SPI_USE_SPI3
       strcat(deviceSelectorString, "SPI3,");
   #endif
   #if STM32_SPI_USE_SPI4
       strcat(deviceSelectorString, "SPI4,");
   #endif
   #if STM32_SPI_USE_SPI5
       strcat(deviceSelectorString, "SPI5,");
   #endif
   #if STM32_SPI_USE_SPI6
       strcat(deviceSelectorString, "SPI6,");
   #endif

       // replace the last comma with a terminator
       deviceSelectorString[hal_strlen_s(deviceSelectorString) - 1] = '\0';

       // because the caller is expecting a result to be returned
       // we need set a return result in the stack argument using the appropriate SetResult according to the variable type (a string here)
       stack.SetResult_String(deviceSelectorString);
   }
   NANOCLR_NOCLEANUP_NOLABEL();
}
