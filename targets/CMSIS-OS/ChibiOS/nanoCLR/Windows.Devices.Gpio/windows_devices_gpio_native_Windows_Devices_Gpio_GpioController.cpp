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


#include "windows_devices_gpio_native.h"
#include "windows_devices_gpio_native_Windows_Devices_Gpio_GpioController.h"

using namespace Windows::Devices::Gpio;

bool GpioController::NativeOpenpin( CLR_RT_HeapBlock* pMngObj, signed int param0, HRESULT &hr )
{
    bool retVal = 1; 
    return retVal;
}

signed int GpioController::get_PinCount( CLR_RT_HeapBlock* pMngObj, HRESULT &hr )
{
    signed int retVal = 0; 
    return 144;	// Fixme: temporary arbitrary value
}

