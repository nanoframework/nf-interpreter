//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "stdafx.h"
#include "win_dev_spi_native.h"


HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiBusInfo::get_MaxClockFrequency___I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock* pArg = &(stack.Arg1());

        // spiBus is an ASCII string with the bus name in format 'SPIn'
        // need to grab 'n' from the string and convert to the integer value from the ASCII code
        uint8_t bus = (uint8_t)pArg[0].RecoverString()[3] - 48;

        // According to STM : "At a minimum, the clock frequency should be twice the required communication frequency."
        // So maximum useable frequency is CoreClock / 2.
        // SPI2 or SPI3 are on APB1, so divide max frequency by four.
        stack.SetResult_I4 (987654321);
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
        stack.SetResult_I4 (123456789);
    }
    NANOCLR_NOCLEANUP();
}
