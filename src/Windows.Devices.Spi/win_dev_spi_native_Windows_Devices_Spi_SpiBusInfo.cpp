//
// Copyright (c) 2020 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "win_dev_spi_native_target.h"

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiBusInfo::NativeChipSelectLineCount___I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        int32_t controllerID =
            pThis[Library_win_dev_spi_native_Windows_Devices_Spi_SpiBusInfo::FIELD___controllerId].NumericByRef().s4;

        stack.SetResult_I4(CPU_SPI_ChipSelectLineCount(controllerID));
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiBusInfo::NativeMaxClockFrequency___I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        int32_t controllerID =
            pThis[Library_win_dev_spi_native_Windows_Devices_Spi_SpiBusInfo::FIELD___controllerId].NumericByRef().s4;

        stack.SetResult_I4(CPU_SPI_MaxClockFrequency(controllerID));
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_spi_native_Windows_Devices_Spi_SpiBusInfo::NativeMinClockFrequency___I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        int32_t controllerID =
            pThis[Library_win_dev_spi_native_Windows_Devices_Spi_SpiBusInfo::FIELD___controllerId].NumericByRef().s4;

        stack.SetResult_I4(CPU_SPI_MinClockFrequency(controllerID));
    }
    NANOCLR_NOCLEANUP();
}
