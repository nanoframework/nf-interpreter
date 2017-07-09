//-----------------------------------------------------------------------------
//
//                   ** WARNING! ** 
//    This file was generated automatically by a tool.
//    Re-running the tool will overwrite this file.
//    You should copy this file to a custom location
//    before adding any customization in the copy to
//    prevent loss of your changes when the tool is
//    re-run.
//
//-----------------------------------------------------------------------------


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

