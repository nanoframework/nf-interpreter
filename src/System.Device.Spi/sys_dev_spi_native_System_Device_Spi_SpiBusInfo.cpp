//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_spi_native.h"

HRESULT Library_sys_dev_spi_native_System_Device_Spi_SpiBusInfo::NativeMaxClockFrequency___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int32_t controllerID;
    int32_t maxClockFrequency;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    controllerID =
        pThis[Library_sys_dev_spi_native_System_Device_Spi_SpiBusInfo::FIELD___controllerId].NumericByRef().s4;

    NANOCLR_CHECK_HRESULT(CPU_SPI_MaxClockFrequency(controllerID, &maxClockFrequency));

    stack.SetResult_I4(maxClockFrequency);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_spi_native_System_Device_Spi_SpiBusInfo::NativeMinClockFrequency___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int32_t controllerID;
    int32_t minClockFrequency;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    controllerID =
        pThis[Library_sys_dev_spi_native_System_Device_Spi_SpiBusInfo::FIELD___controllerId].NumericByRef().s4;

    NANOCLR_CHECK_HRESULT(CPU_SPI_MinClockFrequency(controllerID, &minClockFrequency));

    stack.SetResult_I4(minClockFrequency);

    NANOCLR_NOCLEANUP();
}
