//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

//  ESP32 GPIO ( 40 physical GPIO pads)
//
//  GPIO6-11 used for PSI flash
//
//  GPIO34-39 Only input mode
//

#include <targetPAL.h>
#include "win_dev_gpio_native.h"
#include "nf_rt_events_native.h"

#include "Esp32_DeviceMapping.h"

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
///////////////////////////////////////////////////////////////////////////////////


static const char* TAG = "GpioPin";
static bool  Gpio_Initialised = false;

// this array keeps track of the Gpio pins that are assigned to each channel
CLR_RT_HeapBlock* channelPinMapping[] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                          NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };


void Initialize_gpio()
{
    esp_err_t ret = gpio_install_isr_service( 0);
    if ( ret != ESP_OK )
    {
        ESP_LOGE( TAG, "Install isr service");
    }
}

void Gpio_Interupt_ISR(void * args)
{
   uint32_t pinNumber = (uint32_t)args;

   CLR_RT_HeapBlock*  pThis = channelPinMapping[pinNumber];
   if ( pThis == NULL )
   {
       return;
   }
    
   // check if object has been disposed
   if( pThis[ Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___disposedValue ].NumericByRef().u1 != 0)
   {
       // object has been disposed, leave now
       return;
   }

   // flag to determine if there are any callbacks registered in managed code
   bool callbacksRegistered = (pThis[  Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___callbacks ].Dereference() != NULL);
   if ( callbacksRegistered )
   {
        // if handle registed then post a managed event with the current pin reading
        PostManagedEvent( EVENT_GPIO, 0, pinNumber, gpio_get_level((gpio_num_t)pinNumber)  );
   }
       
}

void Add_Gpio_Interrupt(gpio_num_t pinNumber)
{
    if ( !Gpio_Initialised )
    {
        Initialize_gpio();
        Gpio_Initialised = true;
    }

    esp_err_t ret = gpio_isr_handler_add( pinNumber, Gpio_Interupt_ISR, (void *)pinNumber);
    if ( ret != ESP_OK )
    {
        ESP_LOGE( TAG, "Add interrupt to pin");
    }
}


HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::Read___WindowsDevicesGpioGpioPinValue( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

        if(pThis[ Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___disposedValue ].NumericByRef().u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        gpio_num_t pinNumber = (gpio_num_t)pThis[ FIELD___pinNumber ].NumericByRefConst().s4;

        stack.SetResult_I4( gpio_get_level(pinNumber) );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::DisposeNative___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        // set pin to input to save power
        // clear interrupts
        // releases the pin
        int16_t pinNumber = pThis[ FIELD___pinNumber ].NumericByRefConst().s4;
        
        
        if ( channelPinMapping[pinNumber] != NULL )
        {
            // Remove from interrupts
            gpio_isr_handler_remove((gpio_num_t)pinNumber);

            // clear this channel in channel pin mapping
            channelPinMapping[pinNumber] = NULL;
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::NativeIsDriveModeSupported___BOOLEAN__WindowsDevicesGpioGpioPinDriveMode( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        GpioPinDriveMode driveMode = (GpioPinDriveMode)stack.Arg1().NumericByRef().s4;

        bool driveModeSupported = false;

        // check if the requested drive mode is support by ChibiOS config
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
    NANOCLR_NOCLEANUP();
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

        signed int pinNumber = pThis[ FIELD___pinNumber ].NumericByRefConst().s4;
        signed int driveMode = stack.Arg1().NumericByRef().s4;

        // Valid PinNumber
        if ( ! GPIO_IS_VALID_GPIO(pinNumber) ) 
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
 
        // Check Pin is output capable
        if ( driveMode >= 3 && !GPIO_IS_VALID_OUTPUT_GPIO(pinNumber) ) 
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
  
        switch (driveMode)
        {
            case GpioPinDriveMode_Input :    
                        gpio_set_direction( (gpio_num_t)pinNumber, GPIO_MODE_INPUT);     // 0 - GpioPinDriveMode_Input
                        gpio_set_pull_mode( (gpio_num_t)pinNumber, GPIO_FLOATING);
                        break;
            case GpioPinDriveMode_InputPullDown :    
                        gpio_set_direction( (gpio_num_t)pinNumber, GPIO_MODE_INPUT);     // 1 - GpioPinDriveMode_InputPullDown 
                        gpio_set_pull_mode( (gpio_num_t)pinNumber, GPIO_PULLDOWN_ONLY);
                        break;
            case GpioPinDriveMode_InputPullUp :    
                        gpio_set_direction( (gpio_num_t)pinNumber, GPIO_MODE_INPUT);     // 2 - GpioPinDriveMode_InputPullUp
                        gpio_set_pull_mode( (gpio_num_t)pinNumber, GPIO_PULLUP_ONLY);
                        break;
            case GpioPinDriveMode_Output :    
                        gpio_set_direction( (gpio_num_t)pinNumber, GPIO_MODE_OUTPUT);    // 3 - GpioPinDriveMode_Output
                        gpio_set_pull_mode( (gpio_num_t)pinNumber, GPIO_FLOATING);
                        break;
            case GpioPinDriveMode_OutputOpenDrain :    
                        gpio_set_direction( (gpio_num_t)pinNumber, GPIO_MODE_OUTPUT_OD); // 4 - GpioPinDriveMode_OutputOpenDrain
                        break;
            case GpioPinDriveMode_OutputOpenDrainPullUp :    
                        gpio_set_direction( (gpio_num_t)pinNumber, GPIO_MODE_OUTPUT_OD); // 5 - GpioPinDriveMode_OutputOpenDrainPullUp
                        gpio_set_pull_mode( (gpio_num_t)pinNumber, GPIO_PULLUP_ONLY);
                        break;
                                                                                          // 6 - GpioPinDriveMode_OutputOpenSource
                                                                                          // 7 - GpioPinDriveMode_OutputOpenSourcePullDown
            default :   gpio_set_direction( (gpio_num_t)pinNumber, GPIO_MODE_INPUT_OUTPUT); 
                        break;
        }

        gpio_set_intr_type((gpio_num_t)pinNumber, GPIO_INTR_ANYEDGE);
        
        // Enabler interrupts for all pins
        Add_Gpio_Interrupt( (gpio_num_t)pinNumber );

        // save pin reference
        channelPinMapping[pinNumber] = pThis;

        // protect this from GC so that the callback is where it's supposed to
        CLR_RT_ProtectFromGC         gc( *pThis );
                    
                   
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

        if ( !GPIO_IS_VALID_GPIO((gpio_num_t)pinNumber) )
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        // Return value to the managed application
        stack.SetResult_Boolean(true );
    }
    NANOCLR_NOCLEANUP();
}


HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::NativeSetDebounceTimeout___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    // nothing to do here as the debounce timeout is grabbed from the managed object when required

    NANOCLR_NOCLEANUP();
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

        int16_t pinNumber = pThis[ FIELD___pinNumber ].NumericByRefConst().s4;
        GpioPinDriveMode driveMode = (GpioPinDriveMode)pThis[ FIELD___driveMode ].NumericByRefConst().s4;

        GpioPinValue state = (GpioPinValue)stack.Arg1().NumericByRef().s4;
        
        // sanity check for drive mode set to output so we don't mess up writing to an input pin
        if ((driveMode == GpioPinDriveMode_Output) ||
            (driveMode == GpioPinDriveMode_OutputOpenDrain) ||
            (driveMode == GpioPinDriveMode_OutputOpenDrainPullUp) ||
            (driveMode == GpioPinDriveMode_OutputOpenSource) ||
            (driveMode == GpioPinDriveMode_OutputOpenSourcePullDown))
        {
            gpio_set_level( (gpio_num_t)pinNumber, state);
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

        // get pin number and take the port and pad references from that one
        int16_t pinNumber = pThis[ FIELD___pinNumber ].NumericByRefConst().s4;

        // get alternate function argument
		int32_t alternateFunction = stack.Arg1().NumericByRef().s4;

        Esp32_SetMappedDevicePins( (uint8_t)pinNumber, alternateFunction );
    }
    NANOCLR_NOCLEANUP();
}




