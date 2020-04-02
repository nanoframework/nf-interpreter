//
// Copyright (c) 2019 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "win_dev_gpio_native_target.h"
#include "nf_rt_events_native.h"

#include <ti/sysbios/knl/Clock.h>
#include <xdc/runtime/Error.h>

#define GPIO_MAX_PINS      16
#define TOTAL_GPIO_PORTS   ((GPIO_MAX_PINS + 15) / 16)

// Gpio input state structure
struct gpio_input_state : public HAL_DblLinkedNode<gpio_input_state>
{
	GPIO_PIN 		pinNumber;              // Pin number
	Clock_Handle 	debounceTimer;			// debounce timer for this Pin
	GPIO_INTERRUPT_SERVICE_ROUTINE isrPtr;	// Ptr to user ISR or null
	uint32_t 		debounceMs;             // debounce Millsecs, no debonce=0
	uint8_t  		mode;                   // Interrupt mode
	void *   		param;                  // Param to user isr call
	bool     		expected;               // Expected state for debounce handler
	bool     		waitingDebounce;	    // True if waiting for debounce timer to complete
};

// Array holds pointers to gpio_input_state for active input pins
static gpio_input_state * gpioInputState[GPIO_MAX_PINS];

// Array of bits for saving reserved state
static uint16_t pinReserved[TOTAL_GPIO_PORTS];      

// Allocate a new gpio_input_state and update gpioInputState array
// if already exist then just return current ptr
gpio_input_state * AllocateGpioInputState(GPIO_PIN pinNumber)
{
	gpio_input_state * ptr = gpioInputState[pinNumber];
	if (ptr == NULL)
	{
		ptr = (gpio_input_state *)platform_malloc(sizeof(gpio_input_state));
		memset(ptr, 0, sizeof(gpio_input_state));

		ptr->pinNumber = pinNumber;
		gpioInputState[pinNumber] = ptr;
	}
	return ptr;
}

// Delete gpio_input_state from array and tidy up ( Timer & ISR handler )
void DeleteGpioInputState(GPIO_PIN pinNumber)
{
	if (pinNumber < GPIO_MAX_PINS) 
	{
		gpio_input_state * pState =  gpioInputState[pinNumber];
		if ( pState )
		{	
			if (pState->debounceTimer != 0)
			{
				Clock_delete(&pState->debounceTimer);
			}

			// Remove interrupt associated with pin
			// it's OK to do always this, no matter if interrupts are enabled or not
			GPIO_disableInt(pState->pinNumber);

			platform_free(pState);
			
			gpioInputState[pinNumber]  = NULL;
		}
	}
}

//
// Debounce Handler, called when timer is complete
//
static void debounceTimer_Callback(UArg arg)
{
    int16_t index = (int16_t)arg;

	gpio_input_state * pState = gpioInputState[index];

	if (pState->isrPtr)
	{
		// get current pin state
		bool actual = CPU_GPIO_GetPinState(pState->pinNumber);
		if (actual == pState->expected)
		{
			pState->isrPtr(pState->pinNumber, actual, pState->param);

			if (pState->mode == GPIO_INT_EDGE_BOTH)
			{
				// both edges
				// update expected state
				pState->expected ^= 1;
			}
		}
	}

	pState->waitingDebounce = false;
}

// Gpio event callback
static void GpioEventCallback(uint_least8_t index)
{
	NATIVE_INTERRUPT_START

	gpio_input_state * pState = gpioInputState[index];

	// Any pin set up here ?
	if (pState != NULL)
	{
		// Ignore any pin changes during debounce timeout
		if (!pState->waitingDebounce)
		{
			// If calling ISR available then call it
			if (pState->isrPtr)
			{
				// Debounce time set ?
				if (pState->debounceMs > 0)
				{
					// Yes, set up debounce timer
					pState->waitingDebounce = true;

					// Timer created yet ?
					if (pState->debounceTimer == 0)
					{
						// setup timer
						Clock_Params params;

						Clock_Params_init(&params);
						params.arg = (UArg)index;
						params.startFlag = false;
						params.period = 0;

						// Create and start timer
						pState->debounceTimer = Clock_create(debounceTimer_Callback, pState->debounceMs / Clock_tickPeriod, &params, Error_IGNORE);
					}
					else
					{
						// timer already exists
						// set timeout
						Clock_setTimeout(pState->debounceTimer, pState->debounceMs / Clock_tickPeriod);
					}

					// start timer
					Clock_start(pState->debounceTimer);
				}
				else
				{
					// No debounce so just call ISR with current pin state
					uint_fast8_t pinState = GPIO_read(pState->pinNumber);
					pState->isrPtr(pState->pinNumber, pinState, pState->param);
				}
			}
		}
	}

	NATIVE_INTERRUPT_END
}

bool   CPU_GPIO_Initialize()
{
	// set gpioState ptrs to NULL
	memset(gpioInputState, 0, sizeof(gpioInputState));

	// Make sure all pins are not reserved
	memset(pinReserved, 0, sizeof(pinReserved));
	
	return true;
}

bool   CPU_GPIO_Uninitialize()
{
	// Clean up input list
	for(int index; index < GPIO_MAX_PINS; index++)
	{
		DeleteGpioInputState(index);
	}

	return true;
}

// Set/reset reserved state of pin
bool   CPU_GPIO_ReservePin(GPIO_PIN pinNumber, bool fReserve)
{
	// Check if valid pin number 0 - 15
	if ( pinNumber >= GPIO_MAX_PINS ) return false;

	int port = pinNumber >> 4, bit = 1 << (pinNumber & 0x0F);
	GLOBAL_LOCK();

	if (fReserve)
	{
		if (pinReserved[port] & bit)
		{
			GLOBAL_UNLOCK();
			return false; // already reserved
		}

		pinReserved[port] |= bit;
	}
	else
	{
		pinReserved[port] &= ~bit;
	}

	GLOBAL_UNLOCK();
	return true;
}

// Return if Pin is reserved
bool CPU_GPIO_PinIsBusy(GPIO_PIN pin)
{
	int port = pin >> 4, sh = pin & 0x0F;
	return (pinReserved[port] >> sh) & 1;
}

// Return maximum number of pins
int32_t CPU_GPIO_GetPinCount()
{
	return GPIO_MAX_PINS;
}

// Get current state of pin
GpioPinValue CPU_GPIO_GetPinState(GPIO_PIN pinNumber)
{
	return (GpioPinValue)GPIO_read(pinNumber);
}

// Set Pin state
void CPU_GPIO_SetPinState(GPIO_PIN pinNumber, GpioPinValue PinState)
{
	GPIO_write(pinNumber, PinState);
}

// Toggle pin state
void CPU_GPIO_TogglePinState(GPIO_PIN pinNumber)
{
	GPIO_toggle(pinNumber);
}

// Enable gpio pin for input
bool CPU_GPIO_EnableInputPin(GPIO_PIN pinNumber, CLR_UINT64 debounceTimeMilliseconds, GPIO_INTERRUPT_SERVICE_ROUTINE pin_ISR, void* isr_Param, GPIO_INT_EDGE intEdge, GpioPinDriveMode driveMode)
{
	gpio_input_state * pState;

	// Check if valid pin number 0 - 15
	if ( pinNumber >= GPIO_MAX_PINS ) return false;

	// Check Input drive mode
	if (driveMode >= (int)GpioPinDriveMode_Output)
		return false;

	if (!CPU_GPIO_SetDriveMode(pinNumber, driveMode))
		return false;

	pState = AllocateGpioInputState(pinNumber);

	// Link ISR ptr supplied and not already set up
	// CPU_GPIO_EnableInputPin could be called a 2nd time with changed parameters
	if ( (pin_ISR != NULL) && (pState->isrPtr == NULL))
	{
           // get current config
            GPIO_PinConfig currentPinConfig;
            GPIO_getConfig(pinNumber, &currentPinConfig);

            // Map Interrupt edge to Ti-SimpleLink edge definitions
            // NONE=0, EDGE_LOW=1, EDGE_HIGH=2, EDGE_BOTH=3, LEVEL_HIGH=4, LEVEL_LOW
            GPIO_PinConfig mapint[6] = 
				{ GPIO_CFG_IN_INT_NONE, GPIO_CFG_IN_INT_FALLING, GPIO_CFG_IN_INT_RISING, GPIO_CFG_IN_INT_BOTH_EDGES, GPIO_CFG_IN_INT_LOW, GPIO_CFG_IN_INT_HIGH  };

			currentPinConfig = mapint[6] | currentPinConfig;

            // set interrupt on both edges
            GPIO_setConfig(pinNumber, currentPinConfig | mapint[6] );
            GPIO_setCallback(pinNumber, GpioEventCallback);
			
	}

	pState->isrPtr = pin_ISR;
	pState->mode = intEdge;
	pState->param = (void *)isr_Param;
	pState->debounceMs = (uint32_t)(debounceTimeMilliseconds);

	switch (intEdge)
	{
		case GPIO_INT_EDGE_LOW:
		case GPIO_INT_LEVEL_LOW:
			pState->expected = false;
			break;

		case GPIO_INT_EDGE_HIGH:
		case GPIO_INT_LEVEL_HIGH:
			pState->expected = true;
			break;

		case GPIO_INT_EDGE_BOTH:
			pState->expected = !CPU_GPIO_GetPinState(pinNumber);  // Use inverse of current pin state  
			break;

		default:
			break;
	}

	return true;
}

// Enable an output pin
//
// pinNumber    -   Gpio pin number
// InitialState -   Initial state of pin
// driveMode    -   Drive mode and resistors
// return       -   True if succesful, false invalid pin, pin not putput, invalid drive mode for ouptput
//
bool  CPU_GPIO_EnableOutputPin(GPIO_PIN pinNumber, GpioPinValue InitialState, GpioPinDriveMode driveMode)
{
   	// Check if valid pin number 0 - 15
	if ( pinNumber >= GPIO_MAX_PINS ) return false;

	// check is output drive mode
	if (driveMode < (int)GpioPinDriveMode_Output) return false;

	// If this is currently an input pin then clean up
	DeleteGpioInputState(pinNumber);

	if (CPU_GPIO_SetDriveMode(pinNumber, driveMode) == false) return false;

	CPU_GPIO_SetPinState(pinNumber, InitialState);

	return true;
}

void CPU_GPIO_DisablePin(GPIO_PIN pinNumber, GpioPinDriveMode driveMode, uint32_t alternateFunction)
{
	GLOBAL_LOCK();
	
	DeleteGpioInputState(pinNumber);

	CPU_GPIO_SetDriveMode(pinNumber, driveMode);
	
	if (alternateFunction)
	{
		GPIO_setConfig(pinNumber, alternateFunction);
	}

	GLOBAL_UNLOCK();

	CPU_GPIO_ReservePin(pinNumber, false);
}


// Validate pin and set drive mode
// return true if ok
bool CPU_GPIO_SetDriveMode(GPIO_PIN pinNumber, GpioPinDriveMode driveMode)
{
	if (pinNumber >= GPIO_MAX_PINS) return false;

    // disable interrupt as default
    GPIO_disableInt(pinNumber);

    switch (driveMode)
    {
        case GpioPinDriveMode_Input:
            GPIO_setConfig(pinNumber, GPIO_CFG_IN_NOPULL);
            break;

        case GpioPinDriveMode_InputPullDown:
            GPIO_setConfig(pinNumber, GPIO_CFG_IN_PD);
            break;

        case GpioPinDriveMode_InputPullUp:
            GPIO_setConfig(pinNumber, GPIO_CFG_IN_PU);
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
            return false;
    }

	return true;
}

bool CPU_GPIO_DriveModeSupported(GPIO_PIN pinNumber, GpioPinDriveMode driveMode)
{
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

	return driveModeSupported;
}

uint32_t CPU_GPIO_GetPinDebounce(GPIO_PIN pinNumber)
{
	if (pinNumber >= GPIO_MAX_PINS) return false;

	gpio_input_state * ptr = gpioInputState[pinNumber];
	if (ptr)
		return ptr->debounceMs;

	return 0;
}

bool CPU_GPIO_SetPinDebounce(GPIO_PIN pinNumber, CLR_UINT64 debounceTimeMilliseconds)
{
	if (pinNumber >= GPIO_MAX_PINS) return false;

	gpio_input_state * ptr = gpioInputState[pinNumber];
	if (ptr)
	{
		ptr->debounceMs = (uint32_t)(debounceTimeMilliseconds);
		return true;
	}
	return false;
}
