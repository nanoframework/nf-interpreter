//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//


#include "win_dev_spi_native.h"


HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiBusInfo::get_MaxClockFrequency___I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        stack.SetResult_I4 ( MAX_CLOCK_FREQUENCY );
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiBusInfo::get_MinClockFrequency___I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        stack.SetResult_I4 (  1000000 );
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}
