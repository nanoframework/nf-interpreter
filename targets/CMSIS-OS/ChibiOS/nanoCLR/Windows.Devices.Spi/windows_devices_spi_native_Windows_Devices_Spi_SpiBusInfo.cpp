//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//


#include "windows_devices_spi_native.h"
#include "windows_devices_spi_native_Windows_Devices_Spi_SpiBusInfo.h"

using namespace Windows::Devices::Spi;

signed int SpiBusInfo::get_MaxClockFrequency( CLR_RT_HeapBlock* pMngObj, HRESULT &hr )
{
    signed int retVal = 48000000; 
    return retVal;
}

signed int SpiBusInfo::get_MinClockFrequency( CLR_RT_HeapBlock* pMngObj, HRESULT &hr )
{
    signed int retVal = 1000000; 
    return retVal;
}
