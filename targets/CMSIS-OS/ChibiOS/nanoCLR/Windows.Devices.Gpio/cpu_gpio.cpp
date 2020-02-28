//
// Copyright (c) 2019 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include <ch.h>
#include <hal.h>
#include <cmsis_os.h>

#include <targetPAL.h>
#include "win_dev_gpio_native_target.h"


//volatile uint16_t lastPadValue;


#define GPIO_MAX_PIN  		256 
#define TOTAL_GPIO_PORTS   ((GPIO_MAX_PIN + 15) / 16)

// Double linkedlist to hold the state of each Input pin
struct gpio_input_state : public HAL_DblLinkedNode<gpio_input_state>
{
	GPIO_PIN pinNumber;                      // Pin number
	virtual_timer_t debounceTimer;			 // debounce timer for this Pin
	GPIO_INTERRUPT_SERVICE_ROUTINE isrPtr;   // Ptr to user ISR or null
	uint32_t debounceMs;                     // debounce Millsecs, no debonce=0
	uint8_t  mode;                           // Interrupt mode
	void *   param;                          // Param to user isr call
	bool     expected;                       // Expected state for debounce handler
	bool     waitingDebounce;	    		 // True if waiting for debounce timer to complete
};

static HAL_DblLinkedList<gpio_input_state>  gpioInputList; // Doulble LInkedlist for GPIO input status
static uint16_t pinReserved[TOTAL_GPIO_PORTS];        //  reserved - 1 bit per pin


// this is an utility function to get a ChibiOS PAL IoLine from our "encoded" pin number
static ioline_t GetIoLine(int16_t pinNumber)
{
	stm32_gpio_t* port = GPIO_PORT(pinNumber);
	int16_t pad = pinNumber % 16;

	return PAL_LINE(port, pad);

}

bool  IsValidGpioPin(GPIO_PIN pinNumber)
{
	return (pinNumber <= GPIO_MAX_PIN);
}

static void debounceTimer_Callback(void* arg)
{
	gpio_input_state* pState = (gpio_input_state*)arg;
	if (pState->isrPtr)
	{
		// get current pin state
		bool actual = palReadLine(GetIoLine(pState->pinNumber));
		if (actual == pState->expected)
		{
			pState->isrPtr(pState->pinNumber, actual, pState->param);
			if (pState->mode == GPIO_INT_EDGE_BOTH)
			{
				// both edges
				pState->expected ^= 1; // update expected state
			}
		}
	}

	pState->waitingDebounce = false;
}


static void GpioEventCallback(void *arg)
{
	NATIVE_INTERRUPT_START

	chSysLockFromISR();

	gpio_input_state * pGpio = (gpio_input_state *)arg;

	// Ignore any pin changes during debounce
	if (pGpio->waitingDebounce)
	{
		chSysUnlockFromISR();
		return;
	}

	// check if there is a debounce time set
	if (pGpio->debounceMs > 0)
	{
		// Set flag we are waiting for debounce on this pin
		pGpio->waitingDebounce = true;

		// setup timer
		chVTSetI(&pGpio->debounceTimer, TIME_MS2I(pGpio->debounceMs), debounceTimer_Callback, pGpio);
	}
	else
	{
		// get IoLine from pin number
		ioline_t ioLine = GetIoLine(pGpio->pinNumber);

		pGpio->isrPtr(pGpio->pinNumber, palReadLine(ioLine), pGpio->param);
	}

	chSysUnlockFromISR();

	NATIVE_INTERRUPT_END
}



// Get pointer to gpio_input_state for Gpio pin
// return NULL if not found
gpio_input_state * GetInputState(GPIO_PIN pinNumber)
{
	gpio_input_state * ptr = gpioInputList.FirstNode();
	while (ptr->Next() != NULL)
	{
		if (ptr->pinNumber == pinNumber) return ptr;
		ptr = ptr->Next();
	}
	return NULL;
}

// Allocate a new gpio_input_state and add to end of list
// if already exist then just return current ptr
gpio_input_state * AllocateGpioInputState(GPIO_PIN pinNumber)
{
	gpio_input_state * ptr = GetInputState(pinNumber);
	if (ptr == NULL)
	{
		ptr = (gpio_input_state *)malloc(sizeof(gpio_input_state));
		memset(ptr, 0, sizeof(gpio_input_state));
		ptr->pinNumber = pinNumber;
		gpioInputList.LinkAtBack(ptr);
	}
	return ptr;
}

void UnlinkInputState(gpio_input_state * pState)
{
	chVTReset(&pState->debounceTimer);

	// disable the EXT interrupt channel
	// it's OK to do always this, no matter if it's enabled or not
	palDisableLineEvent(GetIoLine(pState->pinNumber));

	pState->Unlink();
	free(pState);
}

// Delete gpio_input_state from List and tidy up ( Timer & ISR handler )
void DeleteInputState(GPIO_PIN pinNumber)
{
	gpio_input_state * pState = GetInputState(pinNumber);
	if (pState)
		UnlinkInputState(pState);
}


bool   CPU_GPIO_Initialize()
{
	// Initialise Double linked list for input pin states
	gpioInputList.Initialize();

	// Make sure all pins are not reserved
	memset(pinReserved, 0, sizeof(pinReserved));

	return true;
}

bool   CPU_GPIO_Uninitialize()
{
	gpio_input_state * pGpio;

	pGpio = gpioInputList.FirstNode();

	// Clean up input state list
	while (pGpio->Next() != NULL)
	{
		UnlinkInputState(pGpio);
		pGpio = gpioInputList.FirstNode();
	}

	return true;
}

// Set/reset reserved state of pin
bool   CPU_GPIO_ReservePin(GPIO_PIN pinNumber, bool fReserve)
{
	// Check if valid pin number
	if (!IsValidGpioPin(pinNumber)) return false;

	int port = pinNumber >> 4, bit = 1 << (pinNumber & 0x0F);
	bool ret = true;
	GLOBAL_LOCK();

	if (fReserve)
	{
		if (pinReserved[port] & bit)
		{
			ret = false; // already reserved
		}
		else
		{

		pinReserved[port] |= bit;
		}
	}
	else
	{
		pinReserved[port] &= ~bit;
	}

	GLOBAL_UNLOCK();
	return ret;
}

// Return if Pin is reserved
bool CPU_GPIO_PinIsBusy(GPIO_PIN pinNumber)
{
	// Check if valid pin number
	if (!IsValidGpioPin(pinNumber)) return false;

	int port = pinNumber >> 4, sh = pinNumber & 0x0F;
	return (pinReserved[port] >> sh) & 1;
}

// Return maximum number of pins
int32_t CPU_GPIO_GetPinCount()
{
	return GPIO_MAX_PIN;
}

// Get current state of pin
GpioPinValue CPU_GPIO_GetPinState(GPIO_PIN pin)
{
	return (GpioPinValue)palReadLine(GetIoLine(pin));
}

// Set Pin state
void CPU_GPIO_SetPinState(GPIO_PIN pin, GpioPinValue PinState)
{
	palWriteLine(GetIoLine(pin), (int)PinState);
}

bool CPU_GPIO_EnableInputPin(GPIO_PIN pinNumber, int64_t debounceTimeMilliseconds, GPIO_INTERRUPT_SERVICE_ROUTINE Pin_ISR, void* ISR_Param, GPIO_INT_EDGE IntEdge, GpioPinDriveMode driveMode)
{
	gpio_input_state * pState;

	// Check Input drive mode
	if (driveMode >= (int)GpioPinDriveMode_Output)
		return false;

	// Set as Input GPIO_INT_EDGE IntEdge, GPIO_RESISTOR ResistorState
	if (!CPU_GPIO_SetDriveMode(pinNumber, driveMode))
		return false;

	pState = AllocateGpioInputState(pinNumber);

	// Link ISR ptr supplied and not already set up
	// CPU_GPIO_EnableInputPin could be called a 2nd time with changed parameters
	if ((Pin_ISR != NULL) && (pState->isrPtr == NULL))
	{
		// there are callbacks registered and...
		// the drive mode is input so need to setup the interrupt

		// get IoLine from pin number
		ioline_t ioLine = GetIoLine(pinNumber);

		palEnableLineEvent(ioLine, PAL_EVENT_MODE_BOTH_EDGES);
		palSetLineCallback(ioLine, GpioEventCallback, pState);
	}

	pState->isrPtr = Pin_ISR;
	pState->mode = IntEdge;
	pState->param = (void *)ISR_Param;
	pState->debounceMs = (uint32_t)(debounceTimeMilliseconds);

	switch (IntEdge)
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
			pState->expected = !CPU_GPIO_GetPinState(pinNumber);  // expected NOT current state  
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
	// check not an output drive mode
	if (driveMode < (int)GpioPinDriveMode_Output) return false;

	// If this is currently an input pin then clean up
	DeleteInputState(pinNumber);

	if (CPU_GPIO_SetDriveMode(pinNumber, driveMode) == false) return false;

	CPU_GPIO_SetPinState(pinNumber, InitialState);

	return true;
}


void CPU_GPIO_DisablePin(GPIO_PIN pinNumber, GpioPinDriveMode driveMode, uint32_t alternateFunction)
{
	DeleteInputState(pinNumber);

	GLOBAL_LOCK();

	CPU_GPIO_SetDriveMode(pinNumber, driveMode);

	// get IoLine from pin number
	ioline_t ioLine = GetIoLine(pinNumber);
	palSetLineMode(ioLine, PAL_MODE_ALTERNATE(alternateFunction));

	GLOBAL_UNLOCK();

	CPU_GPIO_ReservePin(pinNumber, false);
}

// Validate pin and set drive mode
// return true if ok
bool CPU_GPIO_SetDriveMode(GPIO_PIN pinNumber, GpioPinDriveMode driveMode)
{
	// get IoLine from pin number
	ioline_t ioLine = GetIoLine(pinNumber);

	switch (driveMode)
	{
		case GpioPinDriveMode_Input:
			palSetLineMode(ioLine, PAL_MODE_INPUT);
			break;

		case GpioPinDriveMode_InputPullDown:
			palSetLineMode(ioLine, PAL_MODE_INPUT_PULLDOWN);
			break;

		case GpioPinDriveMode_InputPullUp:
			palSetLineMode(ioLine, PAL_MODE_INPUT_PULLUP);
			break;

		case GpioPinDriveMode_Output:
			palSetLineMode(ioLine, PAL_MODE_OUTPUT_PUSHPULL);
			break;

		case GpioPinDriveMode_OutputOpenDrain:
			palSetLineMode(ioLine, PAL_MODE_OUTPUT_OPENDRAIN);
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

	// check if the requested drive mode is support by ChibiOS config
	if ((driveMode == GpioPinDriveMode_Input) ||
		(driveMode == GpioPinDriveMode_InputPullDown) ||
		(driveMode == GpioPinDriveMode_InputPullUp) ||
		(driveMode == GpioPinDriveMode_Output) ||
		(driveMode == GpioPinDriveMode_OutputOpenDrain))
	{
		driveModeSupported = true;
	}

	return driveModeSupported;
}

uint32_t CPU_GPIO_GetPinDebounce(GPIO_PIN pinNumber)
{
	gpio_input_state * ptr = GetInputState(pinNumber);
	if (ptr)
		return ptr->debounceMs;

	return 0;
}

bool   CPU_GPIO_SetPinDebounce(GPIO_PIN pinNumber, int64_t debounceTimeMilliseconds)
{
	gpio_input_state * ptr = GetInputState(pinNumber);
	if (ptr)
	{
		ptr->debounceMs = (uint32_t)(debounceTimeMilliseconds);
		return true;
	}
	return false;
}
