//
// Copyright (c) 2019 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

// i.MX RT1060 EVK board
//  User Button (GPIO5-00)    - Pin number 128
//  Ardunio interface (GPIO1) - Pin number 0 -> 31

#include "win_dev_gpio_native_target.h"
#include "nf_rt_events_native.h"

#include "timers.h"

#define GPIO_MAX_PINS  		160   // 5 Ports * 32 bits ?
#define GPIO_BITS_PORT		16    // 16 bits per gpio port
#define TOTAL_GPIO_PORTS   ((GPIO_MAX_PINS + (GPIO_BITS_PORT - 1)) / GPIO_BITS_PORT)

// Structure to hold information about input pin
struct gpio_input_state
{
	GPIO_PIN pinNumber;                      // Pin number
	TimerHandle_t debounceTimer;			 // debounce timer for this Pin
	GPIO_INTERRUPT_SERVICE_ROUTINE isrPtr;   // Ptr to user ISR or null
	uint32_t debounceMs;                     // debounce Millsecs, no debonce=0
	uint8_t  mode;                           // Interrupt mode
	void *   param;                          // Param to user isr call
	bool     expected;                       // Expected state for debounce handler
	bool     waitingDebounce;	    		 // True if waiting for debounce timer to complete
};

// Array of gpio_input_state ptrs for each gpio port
// each port (GPIO1 to GPIO5) has a low & high 16 bit port.
typedef gpio_input_state  * statePortArray[GPIO_BITS_PORT];

// For each 16 bit gpio port we have a ptr to an array ptrs to gpio_input_state
// These are initialised only when an input gpio bit is enabled keeping memory use to the minimum
static statePortArray * port_array[TOTAL_GPIO_PORTS];

// Array of bits for saving reserved state
static uint16_t pinReserved[TOTAL_GPIO_PORTS];      


// this is an utility define to get a port number from our "encoded" pin number
// pin 0 -> (GPIO_MAX_PINS - 1)
// i.e Port 0 = Pins 0 to 15, port 1 = pins 16 to 31 etc
#define  GetIoPort(pinNumber)   (pinNumber/GPIO_BITS_PORT)
#define  GetIoBit(pinNumber)   (pinNumber%GPIO_BITS_PORT)
#define  IsValidGpioPin(pinNumber)   (pinNumber < GPIO_MAX_PINS)


void Gpio_DebounceHandler(TimerHandle_t xTimer)
{
	gpio_input_state* pGpio = (gpio_input_state*)pvTimerGetTimerID(xTimer);
	if (pGpio->isrPtr)
	{
		bool actual =(GpioPinValue)GPIO_PinRead(GPIO_BASE(pGpio->pinNumber), GPIO_PIN(pGpio->pinNumber));
		if (actual == pGpio->expected)
		{
			pGpio->isrPtr(pGpio->pinNumber, actual, pGpio->param);
			if (pGpio->mode == GPIO_INT_EDGE_BOTH)
			{ // both edges
				pGpio->expected ^= 1; // update expected state
			}
		}
	}

	pGpio->waitingDebounce = false;
}

void GPIO_Main_IRQHandler( int portIndex, GPIO_Type * portBase )
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	// Get interrupting pins
	uint32_t intPins = GPIO_PortGetInterruptFlags(portBase);

	// clear the interrupt status
    GPIO_PortClearInterruptFlags(portBase, intPins);

	if (portIndex % 2) 
	{
		// use the upper 16 bits for odd ports
		intPins >>= 16;
	} 
	else 
	{
		// use the lower 16 bits for even ports
		intPins &= 0xFFFF;
	}

	// This port been initialised ?
	statePortArray * inputStates = port_array[portIndex];
	if ( inputStates )
	{
		uint32_t bitNumber = 0;

		// Handle all pins with pending interrupt
		while(intPins)
		{
			if ( intPins & 0x01 )
			{
				// Interupt on pin ?
				gpio_input_state * pGpio = (*inputStates)[bitNumber];
				// Do we have gpio_input_state setup for this pin ?
				if (pGpio)
				{
					// Ignore any pin changes during debounce
					if (!pGpio->waitingDebounce) 
					{
						// If user ISR available then call it
						if (pGpio->isrPtr)
						{
                            // If debounce timer defined then first wait for it to expire
							if (pGpio->debounceMs > 0)
							{
								pGpio->waitingDebounce = true;

								// Start Debounce timer
								xTimerChangePeriodFromISR(pGpio->debounceTimer, pdMS_TO_TICKS(pGpio->debounceMs), &xHigherPriorityTaskWoken);
							}
							else
							{
								GpioPinValue PinState = (GpioPinValue)GPIO_PinRead(GPIO_BASE(pGpio->pinNumber), GPIO_PIN(pGpio->pinNumber));
								pGpio->isrPtr(pGpio->pinNumber, PinState, pGpio->param);
							}
						}
					}
				} // if pin setup in nanoFramework
			} // if interrupt

			intPins>>=1;
			bitNumber++;
		} // while
	}

	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );

    // Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
    // exception return operation might vector to incorrect interrupt 
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

extern "C"
{
// Gpio ISR handler for GPIO port 1 bits 0-15
void GPIO1_Combined_0_15_IRQHandler(void)
{
	GPIO_Main_IRQHandler( 0, GPIO1 );
}
// Gpio ISR handler for GPIO port 1 bits 16-31
void GPIO1_Combined_16_31_IRQHandler(void)
{
	GPIO_Main_IRQHandler( 1, GPIO1 );
}
// Gpio ISR handler for GPIO port 2 bits 0-15
void GPIO2_Combined_0_15_IRQHandler(void)
{
	GPIO_Main_IRQHandler( 2, GPIO2 );
}
// Gpio ISR handler for GPIO port 2 bits 16-31

// TODO: this handler is used to sdcard detect
// void GPIO2_Combined_16_31_IRQHandler(void)
// {
// 	GPIO_Main_IRQHandler( 3, GPIO2 );
// }
// Gpio ISR handler for GPIO port 3 bits 0-15
void GPIO3_Combined_0_15_IRQHandler(void)
{
	GPIO_Main_IRQHandler( 4, GPIO3 );
}
// Gpio ISR handler for GPIO port 3 bits 16-31
void GPIO3_Combined_16_31_IRQHandler(void)
{
	GPIO_Main_IRQHandler( 5, GPIO3 );
}
// Gpio ISR handler for GPIO port 4 bits 0-15
void GPIO4_Combined_0_15_IRQHandler(void)
{
	GPIO_Main_IRQHandler( 6, GPIO4 );
}
// Gpio ISR handler for GPIO port 4 bits 16-31
void GPIO4_Combined_16_31_IRQHandler(void)
{
	GPIO_Main_IRQHandler( 7, GPIO4 );
}
// Gpio ISR handler for GPIO port 5 bits 0-15
void GPIO5_Combined_0_15_IRQHandler(void)
{
	GPIO_Main_IRQHandler( 8, GPIO5 );
}
// Gpio ISR handler for GPIO port 5 bits 16-31
void GPIO5_Combined_16_31_IRQHandler(void)
{
	GPIO_Main_IRQHandler( 9, GPIO5 );
}
}

// Get pointer to gpio_input_state for Gpio pin
// return NULL if not found
gpio_input_state * GetInputState(GPIO_PIN pinNumber )
{
	int port = GetIoPort(pinNumber);
	int bit  = GetIoBit(pinNumber);

	if ( port_array[port] == NULL) return NULL;

	statePortArray * inputStates = port_array[port];
	return *inputStates[bit];
}

// Allocate a new gpio_input_state and add to end of list
// if already exist then just return current ptr
gpio_input_state * AllocateGpioInputState(GPIO_PIN pinNumber)
{
	int port = GetIoPort(pinNumber);
	int bit  = GetIoBit(pinNumber);

	if ( port_array[port] == NULL)
	{
		port_array[port] = (statePortArray*)malloc(sizeof(statePortArray));
		if (port_array[port] == NULL ) return NULL;

		memset(port_array[port], 0, sizeof(statePortArray) );
	}

	statePortArray * inputStates = port_array[port];

	gpio_input_state * pGpio = (*inputStates)[bit];
	if (pGpio == NULL)
	{
		pGpio = (gpio_input_state *)malloc(sizeof(gpio_input_state));
		memset(pGpio, 0, sizeof(gpio_input_state));
		pGpio->pinNumber = pinNumber;
		(*inputStates)[bit] = pGpio;
	}
	return pGpio;
}

// Delete gpio_input_state from List and tidy up ( Timer & ISR handler )
void DeleteInputState(GPIO_PIN pinNumber)
{
	int port = GetIoPort(pinNumber);
	int bit  = GetIoBit(pinNumber);

	statePortArray* inputStates = port_array[port];
	if ( inputStates == NULL ) return;

	gpio_input_state * pGpio = (*inputStates)[bit];
	if (pGpio)
	{
		if (pGpio->debounceTimer != 0)
		{
			xTimerDelete(pGpio->debounceTimer, 100);
		}

		// Remove interrupt associatted with pin
		gpio_pin_config_t config = {kGPIO_DigitalInput, 0, kGPIO_NoIntmode };
		GPIO_PinInit(GPIO_BASE(pinNumber), GPIO_PIN(pinNumber), &config);

		free(pGpio);
		(*inputStates)[bit] = NULL;
	}
}


bool   CPU_GPIO_Initialize()
{
	// All port ptrs are null
	memset(port_array, 0, sizeof(port_array));

	// Make sure all pins are not reserved
	memset(pinReserved, 0, sizeof(pinReserved));
	
	return true;
}

bool   CPU_GPIO_Uninitialize()
{
	// First remove any active pin states
	for(int pinNumber=0; pinNumber < GPIO_MAX_PINS; pinNumber++)
	{
		DeleteInputState(pinNumber);
	}

	// Remove statePortArray if any
	for(int port=0; port < TOTAL_GPIO_PORTS; port++)
	{
		statePortArray* inputStates = port_array[port];
		if ( inputStates != NULL )
		{
			free(port_array[port]);			// free up inputStates array
			port_array[port] = NULL;
		}
	}

	return true;
}

// Set/reset reserved state of pin
bool   CPU_GPIO_ReservePin(GPIO_PIN pinNumber, bool fReserve)
{
	// Check if valid pin number 
	if (!IsValidGpioPin(pinNumber)) return false;

	int port = pinNumber >> 4;
	int bit = 1 << (pinNumber & 0x0F);

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
	return GPIO_MAX_PINS;
}

// Get current state of pin
GpioPinValue CPU_GPIO_GetPinState(GPIO_PIN pinNumber)
{
    return (GpioPinValue)GPIO_PinRead(GPIO_BASE(pinNumber), GPIO_PIN(pinNumber));
}

// Set Pin state
void CPU_GPIO_SetPinState(GPIO_PIN pinNumber, GpioPinValue PinState)
{
    GPIO_PinWrite(GPIO_BASE(pinNumber), GPIO_PIN(pinNumber), PinState);
}

// Toggle pin state
void CPU_GPIO_TogglePinState(GPIO_PIN pinNumber)
{
	GPIO_PortToggle(GPIO_BASE(pinNumber), 0x1u << GPIO_PIN(pinNumber));
}

//
// CPU_GPIO_EnableInputPin
// Enable input pin
//
bool CPU_GPIO_EnableInputPin(GPIO_PIN pinNumber, int64_t debounceTimeMilliseconds, GPIO_INTERRUPT_SERVICE_ROUTINE Pin_ISR, void* ISR_Param, GPIO_INT_EDGE IntEdge, GpioPinDriveMode driveMode)
{
	gpio_input_state * pGpio;

	// Check if valid pin number 
	if (!IsValidGpioPin(pinNumber)) return false;

	// Check Input drive mode
	if (driveMode >= (int)GpioPinDriveMode_Output)
		return false;

	if (!CPU_GPIO_SetDriveMode(pinNumber, driveMode))
		return false;

	pGpio = AllocateGpioInputState(pinNumber);

	// Map nanoFRamework Interrupt edge to NXP edge
	// NONE=0, EDGE_LOW=1, EDGE_HIGH=2, EDGE_BOTH=3, LEVEL_HIGH=4, LEVEL_LOW
	const gpio_interrupt_mode_t mapint[6] = { kGPIO_NoIntmode, kGPIO_IntFallingEdge, kGPIO_IntRisingEdge, kGPIO_IntRisingOrFallingEdge, kGPIO_IntHighLevel, kGPIO_IntLowLevel };

	// enable interupt mode with correct edge
	gpio_pin_config_t config = {kGPIO_DigitalInput, 0, mapint[IntEdge] };
	GPIO_PinInit(GPIO_BASE(pinNumber), GPIO_PIN(pinNumber), &config);
	
	// Enable GPIO pin interrupt
	IRQn_Type isrNo = (IRQn_Type)(GPIO1_Combined_0_15_IRQn + GetIoPort(pinNumber));
	NVIC_SetPriority(isrNo, 8U);
	EnableIRQ(isrNo);
	GPIO_PortEnableInterrupts(GPIO_BASE(pinNumber), 1U << GetIoBit(pinNumber));
	GPIO_PortClearInterruptFlags(GPIO_BASE(pinNumber), 1U << GetIoBit(pinNumber));

	// Initialise Gpio state structure
	pGpio->isrPtr = Pin_ISR;
	pGpio->mode = IntEdge;
	pGpio->param = (void *)ISR_Param;
	pGpio->debounceMs = (uint32_t)(debounceTimeMilliseconds);

	// Set up expected new value for debounce
	if ( pGpio->debounceMs > 0)
	{
		if (pGpio->debounceTimer == 0)
		{
			// Create timer if it doesn't already exist for this pin
			pGpio->debounceTimer = xTimerCreate("debounce", 100, pdFALSE, (void*)pGpio, Gpio_DebounceHandler);
		}
		switch (IntEdge)
		{
			case GPIO_INT_NONE:
			case GPIO_INT_EDGE_LOW:
			case GPIO_INT_LEVEL_LOW:
				pGpio->expected = false;
				break;

			case GPIO_INT_EDGE_HIGH:
			case GPIO_INT_LEVEL_HIGH:
				pGpio->expected = true;
				break;

			case GPIO_INT_EDGE_BOTH:
				pGpio->expected = !CPU_GPIO_GetPinState(pinNumber);  // Use NOT current state  
				break;
		}
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
	// Check if valid pin number 
	if (!IsValidGpioPin(pinNumber)) return false;

	// check is output drive mode
	if (driveMode < (int)GpioPinDriveMode_Output) return false;

	if (CPU_GPIO_SetDriveMode(pinNumber, driveMode) == false) return false;

	CPU_GPIO_SetPinState(pinNumber, InitialState);

	return true;
}

void CPU_GPIO_DisablePin(GPIO_PIN pinNumber, GpioPinDriveMode driveMode, uint32_t alternateFunction)
{
	GLOBAL_LOCK();

	CPU_GPIO_SetDriveMode(pinNumber, driveMode);

	DeleteInputState(pinNumber);
	
	if (alternateFunction)
	{
        GPIO_PinMux(GPIO_PORT(pinNumber), GPIO_PIN(pinNumber), alternateFunction);
	}

	GLOBAL_UNLOCK();

	CPU_GPIO_ReservePin(pinNumber, false);
}


// Validate pin and set drive mode
// return true if ok
bool CPU_GPIO_SetDriveMode(GPIO_PIN pinNumber, GpioPinDriveMode driveMode)
{
	// Check if valid pin number 
	if (!IsValidGpioPin(pinNumber)) return false;

	gpio_pin_direction_t direction;         
	uint32_t pinConfig;

	switch (driveMode)
	{
		case GpioPinDriveMode_Input:
			direction = kGPIO_DigitalInput;
			pinConfig = GPIO_IO;
			break;

		case GpioPinDriveMode_InputPullDown:
			direction = kGPIO_DigitalInput;
			pinConfig = GPIO_IN_PULLDOWN;
			break;

		case GpioPinDriveMode_InputPullUp:
			direction = kGPIO_DigitalInput;
			pinConfig = GPIO_IN_PULLUP;
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
			return false;
	}

	gpio_pin_config_t config = {direction, 0, kGPIO_NoIntmode};

	GPIO_PinMux(GPIO_PORT(pinNumber), GPIO_PIN(pinNumber), 0x5u);
	GPIO_PinConfig(GPIO_PORT(pinNumber), GPIO_PIN(pinNumber), pinConfig);
	GPIO_PinInit(GPIO_BASE(pinNumber), GPIO_PIN(pinNumber), &config);
        
	return true;
}

bool CPU_GPIO_DriveModeSupported(GPIO_PIN pinNumber, GpioPinDriveMode driveMode)
{
	// Check if valid pin number 
	if (!IsValidGpioPin(pinNumber)) return false;

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

	return driveModeSupported;
}

uint32_t CPU_GPIO_GetPinDebounce(GPIO_PIN pinNumber)
{
	// Check if valid pin number 
	if (IsValidGpioPin(pinNumber)) 
	{
		gpio_input_state * ptr = GetInputState(pinNumber);
		if (ptr)
			return ptr->debounceMs;
	}

	return 0;
}

bool   CPU_GPIO_SetPinDebounce(GPIO_PIN pinNumber, int64_t debounceTimeMilliseconds)
{
	// Check if valid pin number 
	if (IsValidGpioPin(pinNumber)) 
	{
		gpio_input_state * ptr = GetInputState(pinNumber);
		if (ptr)
		{
			ptr->debounceMs = (uint32_t)(debounceTimeMilliseconds);
			return true;
		}
	}
	return false;
}
