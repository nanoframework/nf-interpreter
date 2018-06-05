//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//


#include "win_dev_spi_native.h"


HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiBusInfo::get_MaxClockFrequency___I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock* pArg = &(stack.Arg1());

        // spiBus is an ASCII string with the bus name in format 'SPIn'
        // need to grab 'n' from the string and convert to the integer value from the ASCII code
        uint8_t bus = (uint8_t)pArg[0].RecoverString()[3] - 48;

        stack.SetResult_I4 ( MAX_CLOCK_FREQUENCY );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiBusInfo::get_MinClockFrequency___I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock* pArg = &(stack.Arg1());

        // spiBus is an ASCII string with the bus name in format 'SPIn'
        // need to grab 'n' from the string and convert to the integer value from the ASCII code
        uint8_t bus = (uint8_t)pArg[0].RecoverString()[3] - 48;

        // Max prescaler value = 256
        // SPI2 or SPI3 are on APB1, so divide max frequency by four.
        stack.SetResult_I4 (  1000000 );
    }
    NANOCLR_NOCLEANUP();
}
