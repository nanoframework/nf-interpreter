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


#include "windows_devices_gpio_native.h"
#include "windows_devices_gpio_native_Windows_Devices_Gpio_GpioPin.h"

using namespace Windows::Devices::Gpio;

stm32_gpio_t* gpioPort[] = { GPIOA, GPIOB
#if STM32_HAS_GPIOC
, GPIOC
#endif
#if STM32_HAS_GPIOD
, GPIOD
#endif
#if STM32_HAS_GPIOE
, GPIOE
#endif
#if STM32_HAS_GPIOF
, GPIOF
#endif
#if STM32_HAS_GPIOG
, GPIOG
#endif
#if STM32_HAS_GPIOH
, GPIOH
#endif
#if STM32_HAS_GPIOI
, GPIOI
#endif
#if STM32_HAS_GPIOJ
, GPIOJ
#endif
#if STM32_HAS_GPIOK
, GPIOK
#endif
 };

#define GPIO_PORT(pin) (gpioPort[pin/16])

bool GpioPin::NativeIsDriveModeSupported( CLR_RT_HeapBlock* pMngObj, unsigned char param0, HRESULT &hr )
{
    return param0 < 5;
}

signed int GpioPin::NativeRead( CLR_RT_HeapBlock* pMngObj, signed int param0, HRESULT &hr )
{
    return palReadPad(GPIO_PORT(param0), param0 % 16);
}

void GpioPin::NativeWrite( CLR_RT_HeapBlock* pMngObj, signed int param0, unsigned char param1, HRESULT &hr )
{
    if (param1 == 0) palClearPad(GPIO_PORT(param0), param0 % 16);
    else palSetPad(GPIO_PORT(param0), param0 % 16);
}

void GpioPin::NativeSetDriveMode( CLR_RT_HeapBlock* pMngObj, signed int param0, unsigned char param1, HRESULT &hr )
{
	switch (param1)
    {
        case 0 :    palSetPadMode(GPIO_PORT(param0), param0 % 16, PAL_MODE_INPUT);
                    break;
        case 1 :    palSetPadMode(GPIO_PORT(param0), param0 % 16, PAL_MODE_INPUT_PULLDOWN);
                    break;
        case 2 :    palSetPadMode(GPIO_PORT(param0), param0 % 16, PAL_MODE_INPUT_PULLUP);
                    break;
        case 3 :    palSetPadMode(GPIO_PORT(param0), param0 % 16, PAL_MODE_OUTPUT_PUSHPULL);
                    break;
        case 4 :    palSetPadMode(GPIO_PORT(param0), param0 % 16, PAL_MODE_OUTPUT_OPENDRAIN);
                    break;
        default :   palSetPadMode(GPIO_PORT(param0), param0 % 16, PAL_MODE_INPUT_PULLDOWN);
                    break;
    }
}

signed int GpioPin::get_DebounceTimeout( CLR_RT_HeapBlock* pMngObj, HRESULT &hr )
{
    signed int retVal = 0; 
    return retVal;
}

void GpioPin::set_DebounceTimeout( CLR_RT_HeapBlock* pMngObj, signed int param0, HRESULT &hr )
{
}

void GpioPin::DisposeNative( CLR_RT_HeapBlock* pMngObj, HRESULT &hr )
{
}

