//
// Copyright (c) 2019 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "win_dev_gpio_native_target.h"
#include "nf_rt_events_native.h"

#include <ti/sysbios/knl/Clock.h>
#include <xdc/runtime/Error.h>
#include <ti/drivers/gpio/GPIOCC26XX.h>
#include <ti/drivers/PIN.h>

#define GPIO_MAX_PINS      16
#define TOTAL_GPIO_PORTS   ((GPIO_MAX_PINS + 15) / 16)

#define EMPTY_PIN  0xFFFF

// Array of Pin configurations
GPIO_PinConfig gpioPinConfigs[] = {
	GPIO_DO_NOT_CONFIG | EMPTY_PIN,
	GPIO_DO_NOT_CONFIG | EMPTY_PIN,
	GPIO_DO_NOT_CONFIG | EMPTY_PIN,
	GPIO_DO_NOT_CONFIG | EMPTY_PIN,
	GPIO_DO_NOT_CONFIG | EMPTY_PIN,
	GPIO_DO_NOT_CONFIG | EMPTY_PIN,
	GPIO_DO_NOT_CONFIG | EMPTY_PIN,
	GPIO_DO_NOT_CONFIG | EMPTY_PIN,
	GPIO_DO_NOT_CONFIG | EMPTY_PIN,
	GPIO_DO_NOT_CONFIG | EMPTY_PIN,
	GPIO_DO_NOT_CONFIG | EMPTY_PIN,
	GPIO_DO_NOT_CONFIG | EMPTY_PIN,
	GPIO_DO_NOT_CONFIG | EMPTY_PIN,
	GPIO_DO_NOT_CONFIG | EMPTY_PIN,
	GPIO_DO_NOT_CONFIG | EMPTY_PIN,
	GPIO_DO_NOT_CONFIG | EMPTY_PIN,
};

// Array of callback function pointers
GPIO_CallbackFxn gpioCallbackFunctions[GPIO_MAX_PINS];

// Gpio input state structure
struct gpio_input_state : public HAL_DblLinkedNode<gpio_input_state>
{
	// Pin number
	GPIO_PIN 		pinNumber;

	// pin config index
	uint8_t			pinConfigIndex;
	
	// debounce timer for this Pin
	Clock_Handle 	debounceTimer;			
	
	// poniter to user ISR or null
	GPIO_INTERRUPT_SERVICE_ROUTINE isrPtr;
		
	// debounce millsecs, no debonce=0
	uint32_t 		debounceMs;             
	
	// interrupt mode
	uint8_t  		mode;                   

	// param to user IRS call
	void *   		param; 

	// expected state for debounce handler
	bool     		expected;               
	
	// flag for waiting for debounce timer to complete
	bool     		waitingDebounce;	    
};

// Double Linked list for GPIO input status
static HAL_DblLinkedList<gpio_input_state> gpioInputList;

// Array of bits for saving reserved state
static uint16_t pinReserved[TOTAL_GPIO_PORTS];      

// Get pointer to gpio_input_state for GPIO pin
// return NULL if not found
gpio_input_state* GetInputState(GPIO_PIN pinNumber)
{
	gpio_input_state* pState = gpioInputList.FirstNode();
	
	while (pState->Next() != NULL)
	{
		if (pState->pinNumber == pinNumber) 
		{
			return pState;
		}

		pState = pState->Next();
	}

	return NULL;
}

// find a free slot in the pin config array
int8_t FindFreePinConfig()
{
	for(uint8_t index = 0; index < GPIO_MAX_PINS; index++)
	{
		if((uint16_t)gpioPinConfigs[index] == EMPTY_PIN)
		{
			// found a free slot!
			return index;
		}
	}

	return -1;
}

// find a pin number in the pin config array
int8_t FindPinConfig(GPIO_PIN pinNumber)
{
	for(uint8_t index = 0; index < GPIO_MAX_PINS; index++)
	{
		if((uint8_t)gpioPinConfigs[index] == pinNumber)
		{
			// found a free slot!
			return index;
		}
	}

	return -1;
}

// Allocate a new gpio_input_state and add to end of list
// if already exist then just return current ptr
gpio_input_state* AllocateGpioInputState(GPIO_PIN pinNumber)
{
	gpio_input_state * pState = GetInputState(pinNumber);

	if (pState == NULL)
	{
		// no input state for this GPIO
		// check if there is room at the pinf config array
		uint8_t index = FindFreePinConfig();

		if(index >= 0)
		{
			// found a free slot!
			pState = (gpio_input_state *)platform_malloc(sizeof(gpio_input_state));
	
			// sanity check
			if(pState != NULL)
			{
				memset(pState, 0, sizeof(gpio_input_state));
				
				// store the pin number
				pState->pinNumber = pinNumber;

				// store the index of the pin config
				pState->pinConfigIndex = index;

				gpioInputList.LinkAtBack(pState);

				// set the pin number in the config array
				gpioPinConfigs[index] = pinNumber | GPIO_CFG_IN_NOPULL | GPIO_CFG_IN_INT_NONE;
			}
		}
	}

	return pState;
}

void UnlinkInputState(gpio_input_state* pState)
{
	if (pState->debounceTimer != 0)
	{
		Clock_delete(&pState->debounceTimer);
	}

	// Remove interrupt associated with pin
	// it's OK to do always this, no matter if interrupts are enabled or not
	GPIO_disableInt(pState->pinConfigIndex);

	// remove callback
	gpioCallbackFunctions[pState->pinConfigIndex] = NULL;

	// clear pin config
	gpioPinConfigs[pState->pinConfigIndex] = GPIO_DO_NOT_CONFIG | EMPTY_PIN;

	// unlink from list
	pState->Unlink();

	// free memory
	platform_free(pState);
}

// Delete gpio_input_state from array and tidy up ( Timer & ISR handler )
void DeleteGpioInputState(GPIO_PIN pinNumber)
{
	gpio_input_state* pState = GetInputState(pinNumber);

	if (pState)
	{
		UnlinkInputState(pState);
	}
}

//
// Debounce Handler, called when timer is complete
//
static void debounceTimer_Callback(UArg arg)
{
    int16_t index = (int16_t)arg;

	gpio_input_state * pState = GetInputState(index);

	if (pState)
	{
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
	}

	pState->waitingDebounce = false;
}

// Gpio event callback
static void GpioEventCallback(uint_least8_t index)
{
	NATIVE_INTERRUPT_START

	gpio_input_state* pState = GetInputState(index);

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
					uint_fast8_t pinState = GPIO_read(pState->pinConfigIndex);
					pState->isrPtr(pState->pinNumber, pinState, pState->param);
				}
			}
		}
	}

	NATIVE_INTERRUPT_END
}

bool CPU_GPIO_Initialize()
{
	// Initialise Double linked list for input pin states
	gpioInputList.Initialize();

	// clear GPIO pin configs
	for(uint8_t index = 0; index < GPIO_MAX_PINS; index++)
	{
		gpioPinConfigs[index] == GPIO_DO_NOT_CONFIG | EMPTY_PIN;
	}

	// clear callbacks
	memset(gpioCallbackFunctions, 0, sizeof(gpioCallbackFunctions));
	
	// Make sure all pins are not reserved
	memset(pinReserved, 0, sizeof(pinReserved));
	
	return true;
}

bool CPU_GPIO_Uninitialize()
{
	NANOCLR_FOREACH_NODE(gpio_input_state, pGpio, gpioInputList)
	{
		UnlinkInputState(pGpio);
	}
	NANOCLR_FOREACH_NODE_END();

	return true;
}

// Set/reset reserved state of pin
bool CPU_GPIO_ReservePin(GPIO_PIN pinNumber, bool fReserve)
{
	// Check if valid pin number 0 - 15
	// TODO get this define from the SDK as the number is wrong 
	// for CC13x2 there are 32 possible GPIO pins
	if ( pinNumber >= GPIO_MAX_PINS )
	{
		return false;
	}

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
	// get index of pin in config array
	uint8_t index = FindPinConfig(pinNumber);

	if(index >= 0)
	{
		return (GpioPinValue)GPIO_read(index);
	}
}

// Set Pin state
void CPU_GPIO_SetPinState(GPIO_PIN pinNumber, GpioPinValue pinState)
{
	// get index of pin in config array
	uint8_t index = FindPinConfig(pinNumber);

	if(index >= 0)
	{
		GPIO_write(index, pinState);
	}
}

// Toggle pin state
void CPU_GPIO_TogglePinState(GPIO_PIN pinNumber)
{
	uint8_t index = FindPinConfig(pinNumber);

	if(index >= 0)
	{
		GPIO_toggle(index);
	}
}

// Enable gpio pin for input
bool CPU_GPIO_EnableInputPin(GPIO_PIN pinNumber, CLR_UINT64 debounceTimeMilliseconds, GPIO_INTERRUPT_SERVICE_ROUTINE pin_ISR, void* isr_Param, GPIO_INT_EDGE intEdge, GpioPinDriveMode driveMode)
{
	gpio_input_state* pState;

	// Check Input drive mode
	if (driveMode >= (int)GpioPinDriveMode_Output)
	{
		return false;
	}

	pState = AllocateGpioInputState(pinNumber);

	if (!CPU_GPIO_SetDriveMode(pState->pinConfigIndex, driveMode))
	{
		return false;
	}

	// Link ISR ptr supplied and not already set up
	// CPU_GPIO_EnableInputPin could be called a 2nd time with changed parameters
	if ( (pin_ISR != NULL) && (pState->isrPtr == NULL))
	{
		// get current config
		GPIO_PinConfig currentPinConfig;
		GPIO_getConfig(pState->pinConfigIndex, &currentPinConfig);

		// set interrupt on both edges
		GPIO_setConfig(pState->pinConfigIndex, currentPinConfig | GPIO_CFG_IN_INT_BOTH_EDGES );
		GPIO_setCallback(pState->pinConfigIndex, GpioEventCallback);
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
			// Use inverse of current pin state  
			pState->expected = !CPU_GPIO_GetPinState(pinNumber);  
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
	// check is output drive mode
	if (driveMode < (int)GpioPinDriveMode_Output)
	{
		return false;
	}

	// If this is currently an input pin then clean up
	DeleteGpioInputState(pinNumber);

	// get free slot in pin config array
	uint8_t index = FindFreePinConfig();

	if(index >= 0)
	{
		// found a free slot!
		
		// set the pin number in the config array
		gpioPinConfigs[index] = pinNumber | PIN_GPIO_OUTPUT_EN;

		if (CPU_GPIO_SetDriveMode(index, driveMode) == false)
		{
			return false;
		}

		CPU_GPIO_SetPinState(pinNumber, InitialState);

		return true;
	}

	return false;
}

void CPU_GPIO_DisablePin(GPIO_PIN pinNumber, GpioPinDriveMode driveMode, uint32_t alternateFunction)
{
	GLOBAL_LOCK();

	uint8_t index = FindPinConfig(pinNumber);

	if(index >= 0)
	{
		DeleteGpioInputState(pinNumber);

		CPU_GPIO_SetDriveMode(index, driveMode);

		if (alternateFunction)
		{
			GPIO_setConfig(index, alternateFunction);
		}

		GLOBAL_UNLOCK();

		CPU_GPIO_ReservePin(pinNumber, false);
	}
}

// Set drive mode
// pinNumber is the index of the corresponding PIN config in array
// return true if ok
bool CPU_GPIO_SetDriveMode(GPIO_PIN pinNumber, GpioPinDriveMode driveMode)
{
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
            GPIO_setConfig(pinNumber, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_MED | GPIO_CFG_OUT_LOW);
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
	(void)pinNumber;

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
	gpio_input_state* pState = GetInputState(pinNumber);

	_ASSERTE(pState == NULL);

	return pState->debounceMs;
}

bool CPU_GPIO_SetPinDebounce(GPIO_PIN pinNumber, CLR_UINT64 debounceTimeMilliseconds)
{
	gpio_input_state* pState = GetInputState(pinNumber);

	_ASSERTE(pState == NULL);

	pState->debounceMs = (uint32_t)(debounceTimeMilliseconds);

	return true;
}
