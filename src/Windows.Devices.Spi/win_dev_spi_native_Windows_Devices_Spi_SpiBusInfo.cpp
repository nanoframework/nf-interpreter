//
// Copyright (c) 2020 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "win_dev_spi_native_target.h"

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiBusInfo::NativeChipSelectLineCount___I4__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        stack.SetResult_I4(CPU_SPI_ChipSelectLineCount(stack.Arg1().NumericByRef().s4));
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiBusInfo::NativeMaxClockFrequency___I4__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        stack.SetResult_I4(CPU_SPI_MaxClockFrequency(stack.Arg1().NumericByRef().s4));
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiBusInfo::NativeMinClockFrequency___I4__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        stack.SetResult_I4(CPU_SPI_MinClockFrequency(stack.Arg1().NumericByRef().s4));
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}
