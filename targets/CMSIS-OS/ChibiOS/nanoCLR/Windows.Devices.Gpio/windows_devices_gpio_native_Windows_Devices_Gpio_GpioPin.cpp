//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//


#include <ch.h>
#include <hal.h>
#include <cmsis_os.h>

#include "windows_devices_gpio_native.h"
#include "windows_devices_gpio_native_Windows_Devices_Gpio_GpioPin.h"

///////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH Windows.Devices.Gpio.GpioPinDriveMode (in managed code) !!! //
///////////////////////////////////////////////////////////////////////////////////////

enum GpioPinDriveMode
{
    GpioPinDriveMode_Input = 0,
    GpioPinDriveMode_InputPullDown,
    GpioPinDriveMode_InputPullUp,
    GpioPinDriveMode_Output,
    GpioPinDriveMode_OutputOpenDrain,
    GpioPinDriveMode_OutputOpenDrainPullUp,
    GpioPinDriveMode_OutputOpenSource,
    GpioPinDriveMode_OutputOpenSourcePullDown,
};

///////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH Windows.Devices.Gpio.GpioPinValue (in managed code) !!! //
///////////////////////////////////////////////////////////////////////////////////

enum GpioPinValue
{
    GpioPinValue_Low = 0,
    GpioPinValue_High,
};

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////


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


void IsrProcedure( GPIO_PIN pin, bool pinState, void* context )
{
    // TODO check if we need this here 
    // ASSERT_IRQ_MUST_BE_OFF();
    PostManagedEvent( EVENT_GPIO, 0, pin, pinState );
}

signed int GpioPin::get_DebounceTimeout( CLR_RT_HeapBlock* pMngObj, HRESULT &hr )
{
    signed int retVal = 0; 
    return retVal;
}

void GpioPin::set_DebounceTimeout( CLR_RT_HeapBlock* pMngObj, signed int param0, HRESULT &hr )
{
}

signed int GpioPin::Read( CLR_RT_HeapBlock* pMngObj, HRESULT &hr )
{
    // check if this pin has been disposed
    bool disposed = Interop_Marshal_GetField_bool( pMngObj, (unsigned int)Library_windows_devices_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___disposedValue );
    
    if (disposed != 0)
    {
        // managed object has been disposed
        hr = CLR_E_OBJECT_DISPOSED;
        return -1;
    }

    signed int pinNumber = Interop_Marshal_GetField_INT32( pMngObj, (unsigned int)Library_windows_devices_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___pinNumber );

    return palReadPad(GPIO_PORT(pinNumber), pinNumber % 16);
}

void GpioPin::Write( CLR_RT_HeapBlock* pMngObj, signed int param0, HRESULT &hr )
{
    // check if this pin has been disposed
    bool disposed = Interop_Marshal_GetField_bool( pMngObj, Library_windows_devices_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___disposedValue );
    
    if (disposed != 0)
    {
        // managed object has been disposed
        hr = CLR_E_OBJECT_DISPOSED;
        return;
    }

    signed int pinNumber = Interop_Marshal_GetField_INT32( pMngObj, Library_windows_devices_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___pinNumber );

    signed int driveMode = Interop_Marshal_GetField_INT32( pMngObj, Library_windows_devices_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___driveMode );

    // sanity check for drive mode to output so we don't mess up with writing to an input pin
    // TODO should consider the other output variantions too???
    if (driveMode == GpioPinDriveMode_Output)
    {
        if (param0 == 0)
        {
            palClearPad(GPIO_PORT(pinNumber), pinNumber % 16);
        }
        else 
        {
            palSetPad(GPIO_PORT(pinNumber), pinNumber % 16);
        }
    }
}

void GpioPin::DisposeNative( CLR_RT_HeapBlock* pMngObj, HRESULT &hr )
{
}

bool GpioPin::NativeIsDriveModeSupported( CLR_RT_HeapBlock* pMngObj, signed int param0, HRESULT &hr )
{
    return param0 < 5;
}

void GpioPin::NativeSetDriveMode( CLR_RT_HeapBlock* pMngObj, signed int param0, HRESULT &hr )
{
    // check if this pin has been disposed
    bool disposed = Interop_Marshal_GetField_bool( pMngObj, Library_windows_devices_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___disposedValue );
    
    if (disposed != 0)
    {
        // managed object has been disposed
        hr = CLR_E_OBJECT_DISPOSED;
        return;
    }

    signed int pinNumber = Interop_Marshal_GetField_INT32( pMngObj, Library_windows_devices_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___pinNumber );
    
	switch (param0)
    {
        case 0 :    palSetPadMode(GPIO_PORT(pinNumber), pinNumber % 16, PAL_MODE_INPUT);
                    break;
        case 1 :    palSetPadMode(GPIO_PORT(pinNumber), pinNumber % 16, PAL_MODE_INPUT_PULLDOWN);
                    break;
        case 2 :    palSetPadMode(GPIO_PORT(pinNumber), pinNumber % 16, PAL_MODE_INPUT_PULLUP);
                    break;
        case 3 :    palSetPadMode(GPIO_PORT(pinNumber), pinNumber % 16, PAL_MODE_OUTPUT_PUSHPULL);
                    break;
        case 4 :    palSetPadMode(GPIO_PORT(pinNumber), pinNumber % 16, PAL_MODE_OUTPUT_OPENDRAIN);
                    break;
        default :   palSetPadMode(GPIO_PORT(pinNumber), pinNumber % 16, PAL_MODE_INPUT_PULLDOWN);
                    break;
    }
}

bool GpioPin::NativeInit( CLR_RT_HeapBlock* pMngObj, signed int param0, HRESULT &hr )
{
    signed int pinNumber = Interop_Marshal_GetField_INT32( pMngObj, Library_windows_devices_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___pinNumber );

    // TODO is probably a good idea keep track of the used pins, so we can check that here
    // TODO is probably a good idea to check if this pin exists

    // TODO initialize the pin INT

    // all good
    return true;
}
