//
// Copyright (c) 2019 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include <targetHAL.h>
#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Checks.h>

#include <map>

#define ESP32_Gpio_MaxPins GPIO_PIN_COUNT  // 0 -> 31,  32-39 (high)

static const char* TAG = "cpu_Gpio";

struct gpio_state
{
    GPIO_PIN pinNumber;                      // Pin number
	TimerHandle_t debounceTimer;			 // debounce timer for this Pin
    GPIO_INTERRUPT_SERVICE_ROUTINE isrPtr;   // Ptr to user ISR or null
    uint32_t debounceMs;                     // debounce Millsecs, no debonce=0
    uint8_t  mode;                           // Interrupt mode
    void *   param;                          // Param to user isr call
    bool     reserved;                       // true if reserved
    bool     expected;                       // Expected state for debounce handler
	bool     waitingDebounce;	    		 // True if waiting for debounce timer to complete
};


gpio_state g_gpio_state[ESP32_Gpio_MaxPins];


//
// Debounce Handler, called when timer is complete
//
void Esp_Gpio_DebounceHandler(TimerHandle_t xTimer)
{
	gpio_state* pState = (gpio_state*)pvTimerGetTimerID(xTimer);
	if( pState->isrPtr )
    {
        bool actual = CPU_GPIO_GetPinState( pState->pinNumber );  // get current pin state
        if( actual == pState->expected )
        {
            pState->isrPtr( pState->pinNumber, actual, pState->param );
            if( pState->mode == GPIO_INT_EDGE_BOTH )
            { // both edges
                pState->expected ^= 1; // update expected state
            }
        }
    }

	pState->waitingDebounce = false;
}

bool   CPU_GPIO_Initialize()
{
    for( int i = 0; i < ESP32_Gpio_MaxPins; i++ )
    {
		g_gpio_state[i].isrPtr = NULL;
		g_gpio_state[i].debounceTimer = 0;
		g_gpio_state[i].reserved = false;
		g_gpio_state[i].waitingDebounce = false;
    }

    esp_err_t ret = gpio_install_isr_service(0);
    if ( ret != ESP_OK )
    {
        ESP_LOGE( TAG, "Install isr service");
    }
 
    return true;
}

bool   CPU_GPIO_Uninitialize()
{
    gpio_uninstall_isr_service();
    return true;
}

// Set/reset reserved state of pin
bool   CPU_GPIO_ReservePin( GPIO_PIN pinNumber, bool fReserve )
{
	// Check if valid pin number
	if (!GPIO_IS_VALID_GPIO((gpio_num_t)pinNumber)) return false;
	
	// Check not already reserved
	if (fReserve && g_gpio_state[pinNumber].reserved == true)
		return false;

	g_gpio_state[pinNumber].reserved = fReserve;
    return true;
}

// Return if Pin is reserved
bool CPU_GPIO_PinIsBusy( GPIO_PIN pin ) 
{
    return g_gpio_state[pin].reserved;
}

// Return maximum number of pins
int32_t CPU_GPIO_GetPinCount( )
{
    return ESP32_Gpio_MaxPins;
}

// Get current state of pin
bool CPU_GPIO_GetPinState( GPIO_PIN pin )
{
    if( pin >= ESP32_Gpio_MaxPins )
        return false;

	return gpio_get_level((gpio_num_t)pin);
 }

// Set Pin state
void CPU_GPIO_SetPinState( GPIO_PIN pin, bool PinState )
{
    gpio_set_level((gpio_num_t)pin, PinState?1:0);
}

// ISR called by IDF
static void gpio_isr(void * arg)
{
    uint32_t pin = (uint32_t)arg;

    gpio_state * pGpio = &g_gpio_state[pin];

	// Ignore any pin changes during debounce
	if (pGpio->waitingDebounce) return;

    // If user ISR available then call it
    if (pGpio->isrPtr)
    {
       if (pGpio->debounceMs > 0 )
       {
		    pGpio->waitingDebounce = true;
			
			if (pGpio->debounceTimer == 0)
			{
				// Create timer if it doesn't already exist for this pin
				pGpio->debounceTimer = xTimerCreate("debounce", 100, pdFALSE, (void*)pGpio, Esp_Gpio_DebounceHandler);
			}

			// Start Debounce timer
			xTimerChangePeriodFromISR(pGpio->debounceTimer, pdMS_TO_TICKS(pGpio->debounceMs), pdFALSE);
       } 
       else
       {
            bool PinState = gpio_get_level((gpio_num_t)pin) == 1;
            pGpio->isrPtr(pin, PinState, pGpio->param );
       }
    }
}

bool CPU_GPIO_EnableInputPin( GPIO_PIN pinNumber, int64_t debounceTimeMilliseconds, GPIO_INTERRUPT_SERVICE_ROUTINE Pin_ISR, void* ISR_Param, GPIO_INT_EDGE IntEdge, GpioPinDriveMode driveMode )
{
    esp_err_t ret;
    gpio_state * pState;

    // Check Input drive mode
    if ( driveMode >= (int)GpioPinDriveMode_Output )
        return false;

    // Set as Input GPIO_INT_EDGE IntEdge, GPIO_RESISTOR ResistorState
    if ( !CPU_GPIO_SetDriveMode( pinNumber, driveMode ) )
        return false;

    // Link ISR if any
    if ( Pin_ISR != NULL )
    {
        ret = gpio_isr_handler_add((gpio_num_t)pinNumber, gpio_isr, (void *)pinNumber);
        if ( ret != ESP_OK )
        {
            ESP_LOGE( TAG, "Add interrupt to gpio pin failed");
            return false;
        }

        // Map Interrupt edge to ESP32 edge
        // NONE=0, EDGE_LOW=1, EDGE_HIGH=2, EDGE_BOTH=3, LEVEL_HIGH=4, LEVEL_LOW
        uint8_t mapint[6] = { GPIO_INTR_DISABLE, GPIO_INTR_NEGEDGE ,GPIO_INTR_POSEDGE, GPIO_INTR_ANYEDGE, GPIO_INTR_HIGH_LEVEL, GPIO_INTR_LOW_LEVEL };
        gpio_set_intr_type((gpio_num_t)pinNumber, (gpio_int_type_t)mapint[IntEdge]);
    }

    pState = &g_gpio_state[pinNumber];
    pState->pinNumber = pinNumber;
    pState->debounceMs = 0;
    pState->isrPtr = Pin_ISR;
    pState->mode   = IntEdge;
    pState->param  = (void *)ISR_Param;

    CPU_GPIO_SetPinDebounce( pinNumber, debounceTimeMilliseconds );

    switch( IntEdge )
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
            pState->expected = !CPU_GPIO_GetPinState( pinNumber );  // Use not current state  
            break;

        default:
            break;
    }

    CPU_GPIO_ReservePin( pinNumber, true );

    return true;
}

// Enable an output pin
//
// pinNumber    -   Gpio pin number
// InitialState -   Initial state of pin
// driveMode    -   Drive mode and resistors
// return       -   True if succesful, false invalid pin, pin not putput, invalid drive mode for ouptput
//
bool  CPU_GPIO_EnableOutputPin ( GPIO_PIN pinNumber, bool InitialState, GpioPinDriveMode driveMode )
{
    // check an output mode
    if ( driveMode < (int)GpioPinDriveMode_Output ) return false; 

    if ( CPU_GPIO_SetDriveMode( pinNumber, driveMode ) == false ) return false;

    CPU_GPIO_SetPinState( pinNumber, InitialState );
    return true;
}

void CPU_GPIO_DisablePin(GPIO_PIN pinNumber, GpioPinDriveMode driveMode, GPIO_ALT_MODE AltFunction)
{
    (void)AltFunction;

	gpio_state * pState;

	// Reset state
	pState = &g_gpio_state[pinNumber];
	pState->isrPtr = 0;
	if (pState->debounceTimer != 0)
	{
		xTimerDelete(pState->debounceTimer, 100);
		pState->debounceTimer = 0;
	}

	CPU_GPIO_SetDriveMode(pinNumber, driveMode);

	// Remove interrupt associatted with pin
	gpio_isr_handler_remove((gpio_num_t)pinNumber);

	CPU_GPIO_ReservePin(pinNumber, false);
}

// Validate pin and set drive mode
// return true if ok
bool CPU_GPIO_SetDriveMode( GPIO_PIN pinNumber, GpioPinDriveMode driveMode )
{
        // Valid Pin
        if ( ! GPIO_IS_VALID_GPIO(pinNumber) ) 
        {
            return false;
        }
 
        // Check Pin is output capable
        if ( driveMode >= (int)GpioPinDriveMode_Output && !GPIO_IS_VALID_OUTPUT_GPIO(pinNumber) ) 
        {
            return false;
        }
  
        gpio_mode_t     mode         = GPIO_MODE_DISABLE;
        gpio_pullup_t   pull_up_en   = GPIO_PULLUP_DISABLE;
        gpio_pulldown_t pull_down_en = GPIO_PULLDOWN_DISABLE;
        gpio_int_type_t intr_type    = GPIO_INTR_ANYEDGE;

        switch (driveMode)
        {
            case GpioPinDriveMode_Input :   
                        mode = GPIO_MODE_INPUT; 
                        break;
            case GpioPinDriveMode_InputPullDown :    
                        mode = GPIO_MODE_INPUT; 
                        pull_down_en = GPIO_PULLDOWN_ENABLE;
                        break;
            case GpioPinDriveMode_InputPullUp :    
						mode = GPIO_MODE_INPUT;
						pull_up_en = GPIO_PULLUP_ENABLE;
                        break;
            case GpioPinDriveMode_Output :    
                        mode = GPIO_MODE_OUTPUT;
                        break;
            case GpioPinDriveMode_OutputOpenDrain :    
                        mode = GPIO_MODE_OUTPUT_OD;
                        break;
            case GpioPinDriveMode_OutputOpenDrainPullUp :    
                        mode = GPIO_MODE_OUTPUT_OD;
                        pull_up_en = GPIO_PULLUP_ENABLE;
                        break;
            case GpioPinDriveMode_OutputOpenSource:
                        mode = GPIO_MODE_OUTPUT_OD;
                        break;
            case GpioPinDriveMode_OutputOpenSourcePullDown:
                        mode = GPIO_MODE_OUTPUT_OD;
                        pull_down_en = GPIO_PULLDOWN_ENABLE;
                        break;
        }

        gpio_config_t GPIOConfig;
        
        GPIOConfig.pin_bit_mask = (1ULL << pinNumber);
        GPIOConfig.mode = mode;
        GPIOConfig.pull_up_en = pull_up_en;
        GPIOConfig.pull_down_en = pull_down_en;
        GPIOConfig.intr_type = intr_type;

        gpio_config( &GPIOConfig );

        return true;
    }


bool CPU_GPIO_DriveModeSupported(GPIO_PIN pinNumber, GpioPinDriveMode driveMode)
{
	if (!GPIO_IS_VALID_GPIO(pinNumber)) return false;

	// Outpt pin
	if (GPIO_IS_VALID_OUTPUT_GPIO(pinNumber))
	{
		return  (driveMode >= GpioPinDriveMode_Output && driveMode <= GpioPinDriveMode_OutputOpenSourcePullDown);
	}

	return  (driveMode >= GpioPinDriveMode_Input && driveMode <= GpioPinDriveMode_OutputOpenSourcePullDown);
}

uint32_t CPU_GPIO_GetPinDebounce( GPIO_PIN pinNumber )
{
    if (!GPIO_IS_VALID_GPIO(pinNumber)) return 0;

    return g_gpio_state[pinNumber].debounceMs;
}

bool   CPU_GPIO_SetPinDebounce( GPIO_PIN pinNumber, int64_t debounceTimeMilliseconds )
{
    if (!GPIO_IS_VALID_GPIO(pinNumber)) return false;

    g_gpio_state[pinNumber].debounceMs = (uint32_t)(debounceTimeMilliseconds);
    return true;
}