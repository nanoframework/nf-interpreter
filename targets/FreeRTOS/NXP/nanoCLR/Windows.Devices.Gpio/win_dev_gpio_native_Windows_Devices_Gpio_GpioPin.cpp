//
// Copyright (c) 2019 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//


#include "MIMXRT1062.h"
#include "fsl_gpio.h"
#include "fsl_gpio_ext.h"
#undef MIN
#undef MAX

#include "win_dev_gpio_native_target.h"
#include "nf_rt_events_native.h"


///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::Read___WindowsDevicesGpioGpioPinValue( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

        // check if object has been disposed
        if(pThis[ Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___disposedValue ].NumericByRef().u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        int32_t pinNumber = pThis[ FIELD___pinNumber ].NumericByRefConst().s4;

        uint32_t val = GPIO_PinRead(GPIO_BASE(pinNumber), GPIO_PIN(pinNumber));

        stack.SetResult_I4(val);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::Toggle___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

        // check if object has been disposed
        if(pThis[ Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___disposedValue ].NumericByRef().u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        int32_t pinNumber = pThis[ FIELD___pinNumber ].NumericByRefConst().s4;
        
        GpioPinDriveMode driveMode = (GpioPinDriveMode)pThis[ FIELD___driveMode ].NumericByRefConst().s4;

        // sanity check for drive mode set to output so we don't mess up writing to an input pin
        if ((driveMode == GpioPinDriveMode_Output) ||
            (driveMode == GpioPinDriveMode_OutputOpenDrain) ||
            (driveMode == GpioPinDriveMode_OutputOpenDrainPullUp) ||
            (driveMode == GpioPinDriveMode_OutputOpenSource) ||
            (driveMode == GpioPinDriveMode_OutputOpenSourcePullDown))
        {
            GPIO_PortToggle(GPIO_BASE(pinNumber), 0x1u << GPIO_PIN(pinNumber));

            // // store new state
            pThis[ FIELD___lastOutputValue ].NumericByRef().s4 = (GpioPinValue)(GpioPinValue_High ^ (GpioPinValue)pThis[ FIELD___lastOutputValue ].NumericByRef().s4);
        }

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::DisposeNative___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::NativeIsDriveModeSupported___BOOLEAN__WindowsDevicesGpioGpioPinDriveMode( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        GpioPinDriveMode driveMode = (GpioPinDriveMode)stack.Arg1().NumericByRef().s4;

        bool driveModeSupported = false;

        // check if the requested drive mode is supported
        if ((driveMode == GpioPinDriveMode_Input) ||
            (driveMode == GpioPinDriveMode_InputPullDown) ||
            (driveMode == GpioPinDriveMode_InputPullUp) ||
            (driveMode == GpioPinDriveMode_Output) ||
            (driveMode == GpioPinDriveMode_OutputOpenDrain))
        {
            driveModeSupported = true;
        }

        // Return value to the managed application
        stack.SetResult_Boolean( driveModeSupported ) ;
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::NativeSetDriveMode___VOID__WindowsDevicesGpioGpioPinDriveMode( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

        if(pThis[ Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___disposedValue ].NumericByRef().u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        int32_t pinNumber = pThis[ FIELD___pinNumber ].NumericByRefConst().s4;
        
        // it's better cast this this to the appropriate enum
        GpioPinDriveMode driveMode = (GpioPinDriveMode)stack.Arg1().NumericByRef().s4;

        // check if drive mode is input
        bool driveModeIsInput = false;

        if( driveMode == GpioPinDriveMode_Input ||
            driveMode == GpioPinDriveMode_InputPullDown ||
            driveMode == GpioPinDriveMode_InputPullUp)
            {
                driveModeIsInput = true;
            }

        // flag to signal that interrupts need to be setup
        bool setupInterrupt = false;

        // flag to determine if there are any callbacks registered in managed code
        bool callbacksRegistered = (pThis[ FIELD___callbacks ].Dereference() != NULL);
        
        gpio_pin_direction_t direction;         
        uint32_t pinConfig;

        switch (driveMode)
        {
            case GpioPinDriveMode_Input:
                direction = kGPIO_DigitalInput;
                pinConfig = GPIO_IO;
                setupInterrupt = true;
                break;

            case GpioPinDriveMode_InputPullDown:
                direction = kGPIO_DigitalInput;
                pinConfig = GPIO_IN_PULLDOWN;
                setupInterrupt = true;
                break;

            case GpioPinDriveMode_InputPullUp:
                direction = kGPIO_DigitalInput;
                pinConfig = GPIO_IN_PULLUP;
                setupInterrupt = true;
                break;
    
            case GpioPinDriveMode_Output:
                direction = kGPIO_DigitalOutput;
                pinConfig = GPIO_IO;
                break;

            case GpioPinDriveMode_OutputOpenDrain:
                direction = kGPIO_DigitalOutput;
                pinConfig = GPIO_OUT_OPENDRAIN;
                break;

            default:
                // all other modes are NOT supported
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                break;
        }

        gpio_pin_config_t config = {direction, 0, kGPIO_NoIntmode};

        GPIO_PinMux(GPIO_PORT(pinNumber), GPIO_PIN(pinNumber), 0x5u);
        GPIO_PinConfig(GPIO_PORT(pinNumber), GPIO_PIN(pinNumber), pinConfig);
        GPIO_PinInit(GPIO_BASE(pinNumber), GPIO_PIN(pinNumber), &config);
        
        // if drive mode is output, read the pad to update the managed field _lastOutputValue
        if(!driveModeIsInput)
        {
            uint32_t val = GPIO_PinRead(GPIO_BASE(pinNumber), GPIO_PIN(pinNumber));
            pThis[ FIELD___lastOutputValue ].NumericByRef().s4 = val;
        }

        if(callbacksRegistered && setupInterrupt)
        {
            // there are callbacks registered and...
            // the drive mode is input so need to setup the interrupt

            // protect this from GC so that the callback is where it's supposed to
            //CLR_RT_ProtectFromGC         gc( *pThis );
            
            // read pad and store current value to check on debounce callback
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::NativeInit___BOOLEAN__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // TODO is probably a good idea keep track of the used pins, so we can check that here
        // TODO is probably a good idea to check if this pin exists

        // Return value to the managed application
        stack.SetResult_Boolean(true );
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::NativeSetDebounceTimeout___VOID( CLR_RT_StackFrame& stack )
{
    (void)stack;

    NANOCLR_HEADER();

    // nothing to do here as the debounce timeout is grabbed from the managed object when required

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::WriteNative___VOID__WindowsDevicesGpioGpioPinValue( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

        // check if object has been disposed
        if(pThis[ Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___disposedValue ].NumericByRef().u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        int32_t pinNumber = pThis[ FIELD___pinNumber ].NumericByRefConst().s4;

        GpioPinDriveMode driveMode = (GpioPinDriveMode)pThis[ FIELD___driveMode ].NumericByRefConst().s4;

        GpioPinValue state = (GpioPinValue)stack.Arg1().NumericByRef().s4;

        // sanity check for drive mode set to output so we don't mess up writing to an input pin
        if ((driveMode == GpioPinDriveMode_Output) ||
            (driveMode == GpioPinDriveMode_OutputOpenDrain) ||
            (driveMode == GpioPinDriveMode_OutputOpenDrainPullUp) ||
            (driveMode == GpioPinDriveMode_OutputOpenSource) ||
            (driveMode == GpioPinDriveMode_OutputOpenSourcePullDown))
        {
            GPIO_PinWrite(GPIO_BASE(pinNumber), GPIO_PIN(pinNumber), state);

            // update the managed field _lastOutputValue
            pThis[ FIELD___lastOutputValue ].NumericByRef().s4 = state;
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::NativeSetAlternateFunction___VOID__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

        // check if object has been disposed
        if(pThis[ Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___disposedValue ].NumericByRef().u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        int32_t pinNumber = pThis[ FIELD___pinNumber ].NumericByRefConst().s4;

        int32_t alternateFunction = stack.Arg1().NumericByRef().s4;

        GPIO_PinMux(GPIO_PORT(pinNumber), GPIO_PIN(pinNumber), alternateFunction);

    }
    NANOCLR_NOCLEANUP();
}
