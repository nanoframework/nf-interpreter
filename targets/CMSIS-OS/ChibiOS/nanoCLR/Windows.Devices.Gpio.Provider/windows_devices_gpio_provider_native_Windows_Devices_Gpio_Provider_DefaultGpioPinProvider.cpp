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
#include <ch.h>
#include <hal.h>
#include <cmsis_os.h>

#include "windows_devices_gpio_provider_native.h"
#include "windows_devices_gpio_provider_native_Windows_Devices_Gpio_Provider_DefaultGpioPinProvider.h"

using namespace Windows::Devices::Gpio::Provider;

stm32_gpio_t* gpioPort[] = { GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG, GPIOH, GPIOI, GPIOJ };

#define GPIO_PORT(pin) (gpioPort[(pin/16) - 1])

UNSUPPORTED_TYPE DefaultGpioPinProvider::get_DebounceTimeout( CLR_RT_HeapBlock* pMngObj, HRESULT &hr )
{
    UNSUPPORTED_TYPE retVal = 0; 
    return retVal;
}

void DefaultGpioPinProvider::set_DebounceTimeout( CLR_RT_HeapBlock* pMngObj, UNSUPPORTED_TYPE param0, HRESULT &hr )
{
}

void DefaultGpioPinProvider::NativeWrite( CLR_RT_HeapBlock* pMngObj, signed int param0, unsigned char param1, HRESULT &hr )
{
    if (param1 == 0) palClearPad(GPIO_PORT(param0), param0 % 16);
    else palSetPad(GPIO_PORT(param0), param0 % 16);
}

signed int DefaultGpioPinProvider::NativeRead( CLR_RT_HeapBlock* pMngObj, signed int param0, HRESULT &hr )
{
    return palReadPad(GPIO_PORT(param0), param0 % 16);
}

void DefaultGpioPinProvider::NativeSetDriveMode( CLR_RT_HeapBlock* pMngObj, signed int param0, signed int param1, HRESULT &hr )
{
    switch (param1)
    {
        case 0 :    palSetPadMode(GPIO_PORT(param0), param0 % 16, PAL_MODE_ALTERNATE(PAL_MODE_INPUT));
                    break;
        case 1 :    palSetPadMode(GPIO_PORT(param0), param0 % 16, PAL_MODE_ALTERNATE(PAL_MODE_INPUT_PULLDOWN));
                    break;
        case 2 :    palSetPadMode(GPIO_PORT(param0), param0 % 16, PAL_MODE_ALTERNATE(PAL_MODE_INPUT_PULLUP));
                    break;
        case 3 :    palSetPadMode(GPIO_PORT(param0), param0 % 16, PAL_MODE_ALTERNATE(PAL_MODE_OUTPUT_PUSHPULL));
                    break;
        case 4 :    palSetPadMode(GPIO_PORT(param0), param0 % 16, PAL_MODE_ALTERNATE(PAL_MODE_OUTPUT_OPENDRAIN ));
                    break;
        default :   palSetPadMode(GPIO_PORT(param0), param0 % 16, PAL_MODE_ALTERNATE(PAL_MODE_INPUT));
                    break;
    }
     
}

bool DefaultGpioPinProvider::NativeOpenpin( CLR_RT_HeapBlock* pMngObj, signed int param0, HRESULT &hr )
{
    bool retVal = 0; 
    return retVal;
}

bool DefaultGpioPinProvider::NativeIsDriveModeSupported( CLR_RT_HeapBlock* pMngObj, unsigned char param0, HRESULT &hr )
{
    return param0 < 5;
}

void DefaultGpioPinProvider::NativeDispose( CLR_RT_HeapBlock* pMngObj, HRESULT &hr )
{
}

