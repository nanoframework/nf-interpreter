//
// Copyright (c) 2019 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "win_dev_gpio_native.h"
#include "nf_rt_events_native.h"

#include <timers.h>

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
    GpioPinDriveMode_OutputOpenSourcePullDown
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

// this array points to the GPIO managed object that has interrupt active
CLR_RT_HeapBlock* indexPinMapping[] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

// this array points to the active debounce timers
TimerHandle_t debounceTimerMapping[ARRAYSIZE_CONST_EXPR(indexPinMapping)];

// this arrays points to the last read value to be used by the debounce timers
uint_fast8_t lastReadValueMapping[ARRAYSIZE_CONST_EXPR(indexPinMapping)];

static void debounceTimer_Callback(TimerHandle_t xTimer)
{
    (void)xTimer;

    int16_t index;

    // loop through timers to find the index for this
    for(index = 0; index < ARRAYSIZE_CONST_EXPR(indexPinMapping); index++)
    {
        if(debounceTimerMapping[index] == xTimer)
        {
            // found it!
                    
            // get current value
            uint_fast8_t currentValue = GPIO_read(index);

            // get last read value
            uint_fast8_t lastRead = (uint_fast8_t) pvTimerGetTimerID( xTimer );

            if(lastRead == currentValue)
            {
                // value hasn't change during debounce interval so this is a valid change

                // get managed GPIO object
                CLR_RT_HeapBlock*  pThis = (CLR_RT_HeapBlock*)indexPinMapping[index];

                // flag to determine if there are any callbacks registered in managed code
                bool callbacksRegistered = (pThis[ Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___callbacks ].Dereference() != NULL);

                // post a managed event with the current pin value, only if there is anyone listening otherwise don't bother
                if(callbacksRegistered)
                {
                    PostManagedEvent( EVENT_GPIO, 0, index, currentValue );
                }
            }

            xTimerDelete(xTimer, 0);
        }
    }
}

static void GpioEventCallback(uint_least8_t index)
{
    NATIVE_INTERRUPT_START

    CLR_RT_HeapBlock*  pThis = indexPinMapping[index];

    if(pThis == NULL)
    {
        // no Gpio pin here, leave now
        NATIVE_INTERRUPT_END
        return;
    }

    // check if object has been disposed
    if( pThis[ Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___disposedValue ].NumericByRef().u1 != 0)
    {
        // object has been disposed, leave now
        NATIVE_INTERRUPT_END
        return;
    }

    // check if there is a debounce time set
    int64_t debounceTimeoutMilsec = (CLR_INT64_TEMP_CAST) pThis[ Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___debounceTimeout ].NumericByRefConst().s8 / TIME_CONVERSION__TO_MILLISECONDS;

    // store current value
    uint_fast8_t lastRead = GPIO_read(index);

    if(debounceTimeoutMilsec > 0)
    {
        // debounce set, need to handle it

        if(debounceTimerMapping[index] != NULL)
        {
            // there is a debounce timer already running so this change in pin value should be discarded 
            NATIVE_INTERRUPT_END
            return;
        }

        // setup timer
        debounceTimerMapping[index] = xTimerCreate( NULL, (debounceTimeoutMilsec / portTICK_PERIOD_MS), pdFALSE, (void*)lastRead, debounceTimer_Callback);
        xTimerStart(debounceTimerMapping[index], 0);
    }
    else
    {
        // read pin
        pThis[ Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___lastInputValue ].NumericByRef().s4 = lastRead;

        // flag to determine if there are any callbacks registered in managed code
        bool callbacksRegistered = (pThis[ Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___callbacks ].Dereference() != NULL);

        // post a managed event with the current pin reading, only if there is anyone listening otherwise don't bother
        if(callbacksRegistered)
        {
            PostManagedEvent( EVENT_GPIO, 0, index, lastRead );
        }
    }    

    NATIVE_INTERRUPT_END
}

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

        stack.SetResult_I4(GPIO_read(pinNumber));
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
            (driveMode == GpioPinDriveMode_OutputOpenSourcePullDown))
        {
            GPIO_toggle(pinNumber);

            // store new state
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

        int32_t pinNumber = pThis[ FIELD___pinNumber ].NumericByRefConst().s4;

        // it's OK to do always this, no matter if interrupts are enabled or not
        // disable interrupts
        GPIO_disableInt(pinNumber);

        // clear assignment
        indexPinMapping[pinNumber] = NULL;

        // set pin to input to save power
        GPIO_setConfig(pinNumber, GPIO_CFG_IN_NOPULL | GPIO_CFG_IN_INT_NONE);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::NativeIsDriveModeSupported___BOOLEAN__WindowsDevicesGpioGpioPinDriveMode( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        GpioPinDriveMode driveMode = (GpioPinDriveMode)stack.Arg1().NumericByRef().s4;

        bool driveModeSupported = false;

        // check if the requested drive mode is supported by SimpleLink
        if ((driveMode == GpioPinDriveMode_Input) ||
            (driveMode == GpioPinDriveMode_InputPullDown) ||
            (driveMode == GpioPinDriveMode_InputPullUp) ||
            (driveMode == GpioPinDriveMode_Output) ||
            (driveMode == GpioPinDriveMode_OutputOpenDrain) ||
             (driveMode == GpioPinDriveMode_OutputOpenDrainPullUp) ||
            (driveMode == GpioPinDriveMode_OutputOpenSourcePullDown))
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

        // get pin number
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

        // disable interrupt as default
        GPIO_disableInt(pinNumber);

        switch (driveMode)
        {
            case GpioPinDriveMode_Input:
                GPIO_setConfig(pinNumber, GPIO_CFG_IN_NOPULL);
                setupInterrupt = true;
                break;

            case GpioPinDriveMode_InputPullDown:
                GPIO_setConfig(pinNumber, GPIO_CFG_IN_PD);
                setupInterrupt = true;
                break;

            case GpioPinDriveMode_InputPullUp:
                GPIO_setConfig(pinNumber, GPIO_CFG_IN_PU);
                setupInterrupt = true;
                break;

            case GpioPinDriveMode_Output:
                GPIO_setConfig(pinNumber, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
                break;

            case GpioPinDriveMode_OutputOpenDrain:
                GPIO_setConfig(pinNumber, GPIO_CFG_OUT_OD_NOPULL);
                break;

            case GpioPinDriveMode_OutputOpenDrainPullUp:
                GPIO_setConfig(pinNumber, GPIO_CFG_OUT_OD_PU);
                break;

            case GpioPinDriveMode_OutputOpenSourcePullDown:
                GPIO_setConfig(pinNumber, GPIO_CFG_OUT_OD_PD);
                break;

            default:
                // all other modes are NOT supported
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                break;
        }

        // if drive mode is output, read the pad to update the managed field _lastOutputValue
        if(!driveModeIsInput)
        {
            pThis[ FIELD___lastOutputValue ].NumericByRef().s4 = GPIO_read(pinNumber);
        }

        if(callbacksRegistered && setupInterrupt)
        {
            // there are callbacks registered and...
            // the drive mode is input so need to setup the interrupt

            // save pin
            indexPinMapping[pinNumber] = pThis;

            // get current config
            GPIO_PinConfig currentPinConfig;
            GPIO_getConfig(pinNumber, &currentPinConfig);

            // set interrupt on both edges
            GPIO_setConfig(pinNumber, currentPinConfig | GPIO_CFG_IN_INT_BOTH_EDGES);
            GPIO_setCallback(pinNumber, &GpioEventCallback);

            GPIO_enableInt(pinNumber);

            // protect this from GC so that the callback is where it's supposed to
            CLR_RT_ProtectFromGC         gc( *pThis );
        }
        else
        {
            // clear assignment
            indexPinMapping[pinNumber] = NULL;
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::NativeInit___BOOLEAN__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        int16_t pinNumber = stack.Arg1().NumericByRef().s4;

        // TODO is probably a good idea keep track of the used pins, so we can check that here
        // TODO is probably a good idea to check if this pin exists

        // clear assignment, to make sure it's empty
        indexPinMapping[pinNumber] = NULL;

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
            (driveMode == GpioPinDriveMode_OutputOpenSourcePullDown))
        {
            GPIO_write(pinNumber, state);

            // store new state
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

        // get alternate function argument
		int32_t alternateFunction = stack.Arg1().NumericByRef().s4;

        GPIO_setConfig(pinNumber, alternateFunction);

    }
    NANOCLR_NOCLEANUP();
}
