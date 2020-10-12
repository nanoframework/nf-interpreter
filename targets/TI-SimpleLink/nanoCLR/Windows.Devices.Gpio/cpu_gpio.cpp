//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "win_dev_gpio_native_target.h"
#include "nf_rt_events_native.h"

#include <ti/sysbios/knl/Clock.h>
#include <xdc/runtime/Error.h>
#include <ti/drivers/gpio/GPIOCC26XX.h>
#include <ti/drivers/PIN.h>

#define GPIO_MAX_PINS 16

// SimpleLink doesn't follow the port&pin design pattern, there are no ports, just GPIO pins
#define TOTAL_GPIO_PORTS 1

#define EMPTY_PIN 0xFFFF

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
    GPIO_PIN pinNumber;

    // pin config index
    uint8_t pinConfigIndex;

    // debounce timer for this Pin
    Clock_Handle debounceTimer;

    // poniter to user ISR or null
    GPIO_INTERRUPT_SERVICE_ROUTINE isrPtr;

    // debounce millsecs, no debonce=0
    uint32_t debounceMs;

    // interrupt mode
    uint8_t mode;

    // param to user IRS call
    void *param;

    // expected state for debounce handler
    uint_fast8_t expected;

    // flag for waiting for debounce timer to complete
    bool waitingDebounce;
};

// Double Linked list for GPIO input status
static HAL_DblLinkedList<gpio_input_state> gpioInputList;

// array to store reserved state of GPIO pins
// not need to use this because we already keep track of the pins with the gpioPinConfigs
// static uint8_t pinReserved[TOTAL_GPIO_PORTS];

// Get pointer to gpio_input_state for GPIO pin
// return NULL if not found
gpio_input_state *GetInputState(GPIO_PIN pinNumber)
{
    gpio_input_state *pState = gpioInputList.FirstNode();

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

// Get pointer to gpio_input_state for GPIO pin
// return NULL if not found
gpio_input_state *GetInputStateByConfigIndex(uint8_t pinConfigIndex)
{
    gpio_input_state *pState = gpioInputList.FirstNode();

    while (pState->Next() != NULL)
    {
        if (pState->pinConfigIndex == pinConfigIndex)
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
    for (uint8_t index = 0; index < GPIO_MAX_PINS; index++)
    {
        if ((uint16_t)gpioPinConfigs[index] == EMPTY_PIN)
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
    for (uint8_t index = 0; index < GPIO_MAX_PINS; index++)
    {
        // need to mask the gpioPinConfigs item to get only the 8 LSbits where the pin number is stored
        if ((uint8_t)gpioPinConfigs[index] == pinNumber)
        {
            // found a free slot!
            return index;
        }
    }

    return -1;
}

// Allocate a new gpio_input_state and add to end of list
// if already exist then just return current ptr
gpio_input_state *AllocateGpioInputState(GPIO_PIN pinNumber)
{
    gpio_input_state *pState = GetInputState(pinNumber);

    if (pState == NULL)
    {
        // found a free slot!
        pState = (gpio_input_state *)platform_malloc(sizeof(gpio_input_state));

        // sanity check
        if (pState != NULL)
        {
            memset(pState, 0, sizeof(gpio_input_state));

            // store the pin number
            pState->pinNumber = pinNumber;

            gpioInputList.LinkAtBack(pState);
        }
    }

    return pState;
}

void UnlinkInputState(gpio_input_state *pState)
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

    // unlink from list
    pState->Unlink();

    // free memory
    platform_free(pState);
}

// Delete gpio_input_state from array and tidy up ( Timer & ISR handler )
void DeleteGpioInputState(GPIO_PIN pinNumber)
{
    gpio_input_state *pState = GetInputState(pinNumber);

    if (pState)
    {
        UnlinkInputState(pState);
    }
}

//
// Debounce Handler, called when timer is complete
//
static void DebounceTimerCallback(UArg arg)
{
    NATIVE_INTERRUPT_START

    gpio_input_state *pState = GetInputStateByConfigIndex((int16_t)arg);

    if (pState)
    {
        // get current pin state
        uint_fast8_t pinState = GPIO_read(pState->pinConfigIndex);

        if (pinState == pState->expected)
        {
            // post a managed event with the current pin reading
            pState->isrPtr(pState->pinNumber, pinState);
        }

        pState->waitingDebounce = false;
    }
}

// GPIO event callback
static void GpioEventCallback(uint_least8_t index)
{
    NATIVE_INTERRUPT_START

    gpio_input_state *pState = GetInputStateByConfigIndex(index);

    // Any pin set up here ?
    if (pState != NULL)
    {
        // Ignore any pin changes during debounce timeout
        if (!pState->waitingDebounce)
        {
            uint_fast8_t pinState = GPIO_read(pState->pinConfigIndex);

            // Debounce time set ?
            if (pState->debounceMs > 0)
            {
                // Yes, set up debounce timer
                pState->waitingDebounce = true;

                // store expected state
                pState->expected = pinState;

                // timer already exists
                // set timeout as we are using a one-shot timer
                Clock_setTimeout(pState->debounceTimer, pState->debounceMs * 1000 / Clock_tickPeriod);

                // start timer
                Clock_start(pState->debounceTimer);
            }
            else
            {
                // No debounce so just post a managed event with the current pin reading
                pState->isrPtr(pState->pinNumber, pinState);
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
    for (uint8_t index = 0; index < GPIO_MAX_PINS; index++)
    {
        gpioPinConfigs[index] == GPIO_DO_NOT_CONFIG | EMPTY_PIN;
    }

    // clear callbacks
    memset(gpioCallbackFunctions, 0, sizeof(gpioCallbackFunctions));

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
    // Check if this is a valid GPIO pin
    if (pinNumber >= GPIO_PINS_COUNT)
    {
        return false;
    }

    GLOBAL_LOCK();

    uint8_t index = FindPinConfig(pinNumber);

    if (fReserve)
    {
        if (index < 0)
        {
            // pin already in use

            GLOBAL_UNLOCK();

            return false;
        }
        else
        {
            // pin not being used, get the next free slot
            index = FindFreePinConfig();

            // reserve pin
            gpioPinConfigs[index] = pinNumber;
        }
    }
    else
    {
        gpioPinConfigs[index] == GPIO_DO_NOT_CONFIG | EMPTY_PIN;
    }

    GLOBAL_UNLOCK();

    return true;
}

// Return if Pin is reserved
bool CPU_GPIO_PinIsBusy(GPIO_PIN pinNumber)
{
    return (FindPinConfig(pinNumber) >= 0);
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
    uint8_t pinConfigIndex = FindPinConfig(pinNumber);

    if (pinConfigIndex >= 0)
    {
        return (GpioPinValue)GPIO_read(pinConfigIndex);
    }
}

// Set Pin state
void CPU_GPIO_SetPinState(GPIO_PIN pinNumber, GpioPinValue pinState)
{
    // get index of pin in config array
    uint8_t pinConfigIndex = FindPinConfig(pinNumber);

    if (pinConfigIndex >= 0)
    {
        GPIO_write(pinConfigIndex, pinState);
    }
}

// Toggle pin state
void CPU_GPIO_TogglePinState(GPIO_PIN pinNumber)
{
    uint8_t pinConfigIndex = FindPinConfig(pinNumber);

    if (pinConfigIndex >= 0)
    {
        GPIO_toggle(pinConfigIndex);
    }
}

// Enable gpio pin for input
bool CPU_GPIO_EnableInputPin(
    GPIO_PIN pinNumber,
    CLR_UINT64 debounceTimeMilliseconds,
    GPIO_INTERRUPT_SERVICE_ROUTINE pinISR,
    void *isrParam,
    GPIO_INT_EDGE intEdge,
    GpioPinDriveMode driveMode)
{
    gpio_input_state *pState;

    // Check Input drive mode
    if (driveMode >= (int)GpioPinDriveMode_Output)
    {
        return false;
    }

    pState = AllocateGpioInputState(pinNumber);

    // get the index of this GPIO in pin config array
    // and store it
    pState->pinConfigIndex = FindPinConfig(pinNumber);

    // set default input config for GPIO pin
    gpioPinConfigs[pState->pinConfigIndex] |= PIN_INPUT_EN | PIN_NOPULL | PIN_IRQ_DIS;

    if (!CPU_GPIO_SetDriveMode(pState->pinConfigIndex, driveMode))
    {
        return false;
    }

    // Link ISR ptr supplied and not already set up
    // CPU_GPIO_EnableInputPin could be called a 2nd time with changed parameters
    if (pinISR != NULL && (pState->isrPtr == NULL))
    {
        // get current config
        GPIO_PinConfig currentPinConfig;
        GPIO_getConfig(pState->pinConfigIndex, &currentPinConfig);

        // set interrupt on both edges
        GPIO_setConfig(pState->pinConfigIndex, currentPinConfig | GPIO_CFG_IN_INT_BOTH_EDGES);
        // set callback
        GPIO_setCallback(pState->pinConfigIndex, GpioEventCallback);
        // enable INT
        GPIO_enableInt(pState->pinConfigIndex);

        // store parameters & configs
        pState->isrPtr = pinISR;
        pState->mode = intEdge;
        pState->param = isrParam;
        pState->debounceMs = (uint32_t)(debounceTimeMilliseconds);

        // create timer if not there yet
        if (pState->debounceMs > 0 && pState->debounceTimer == NULL)
        {
            // setup timer
            Clock_Params params;

            Clock_Params_init(&params);
            params.arg = (UArg)pState->pinConfigIndex;
            params.startFlag = FALSE;
            // period it's 0 because we are using a one-shot timer
            params.period = 0;

            // create timer
            // set timeout as we are using a one-shot timer
            pState->debounceTimer = Clock_create(
                DebounceTimerCallback,
                pState->debounceMs * 1000 / Clock_tickPeriod,
                &params,
                Error_IGNORE);
        }

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
                pState->expected = !CPU_GPIO_GetPinState(pState->pinConfigIndex);
                break;

            default:
                break;
        }
    }
    else if (pinISR == NULL && (pState->isrPtr != NULL))
    {
        // there is no managed handler setup anymore
        // remove INT handler

        // get current config
        GPIO_PinConfig currentPinConfig;
        GPIO_getConfig(pState->pinConfigIndex, &currentPinConfig);

        // disable interrupt
        GPIO_disableInt(pState->pinConfigIndex);
        // remove callback
        GPIO_setCallback(pState->pinConfigIndex, NULL);
        // remove interrupt config
        GPIO_setConfig(pState->pinConfigIndex, currentPinConfig | GPIO_CFG_IN_INT_NONE);

        // clear parameters & configs
        pState->isrPtr = NULL;
        pState->mode = GPIO_INT_NONE;
        pState->param = NULL;
        pState->debounceMs = 0;
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
bool CPU_GPIO_EnableOutputPin(GPIO_PIN pinNumber, GpioPinValue InitialState, GpioPinDriveMode driveMode)
{
    // check is output drive mode
    if (driveMode < (int)GpioPinDriveMode_Output)
    {
        return false;
    }

    // If this is currently an input pin then clean up
    DeleteGpioInputState(pinNumber);

    // get free slot in pin config array
    uint8_t pinConfigIndex = FindPinConfig(pinNumber);

    // set the GPIO pin as output
    gpioPinConfigs[pinConfigIndex] |= GPIO_CFG_OUT_STD;

    if (CPU_GPIO_SetDriveMode(pinConfigIndex, driveMode) == false)
    {
        return false;
    }

    CPU_GPIO_SetPinState(pinNumber, InitialState);

    return true;
}

void CPU_GPIO_DisablePin(GPIO_PIN pinNumber, GpioPinDriveMode driveMode, uint32_t alternateFunction)
{
    GLOBAL_LOCK();

    uint8_t pinConfigIndex = FindPinConfig(pinNumber);

    if (pinConfigIndex >= 0)
    {
        DeleteGpioInputState(pinNumber);

        CPU_GPIO_SetDriveMode(pinConfigIndex, driveMode);

        if (alternateFunction)
        {
            GPIO_setConfig(pinConfigIndex, alternateFunction);
        }

        GLOBAL_UNLOCK();

        CPU_GPIO_ReservePin(pinNumber, false);
    }
}

// Set drive mode
// pinNumber is the index of the corresponding PIN config in array
// return true if ok
bool CPU_GPIO_SetDriveMode(GPIO_PIN pinConfigIndex, GpioPinDriveMode driveMode)
{
    // get current config
    GPIO_PinConfig currentPinConfig;
    GPIO_getConfig(pinConfigIndex, &currentPinConfig);

    switch (driveMode)
    {
        case GpioPinDriveMode_Input:
            GPIO_setConfig(pinConfigIndex, currentPinConfig | GPIO_CFG_IN_NOPULL);
            break;

        case GpioPinDriveMode_InputPullDown:
            GPIO_setConfig(pinConfigIndex, currentPinConfig | GPIO_CFG_IN_PD);
            break;

        case GpioPinDriveMode_InputPullUp:
            GPIO_setConfig(pinConfigIndex, currentPinConfig | GPIO_CFG_IN_PU);
            break;

        case GpioPinDriveMode_Output:
            GPIO_setConfig(pinConfigIndex, currentPinConfig | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_MED);
            break;

        case GpioPinDriveMode_OutputOpenDrain:
            GPIO_setConfig(pinConfigIndex, currentPinConfig | GPIO_CFG_OUT_OD_NOPULL);
            break;

        case GpioPinDriveMode_OutputOpenDrainPullUp:
            GPIO_setConfig(pinConfigIndex, currentPinConfig | GPIO_CFG_OUT_OD_PU);
            break;

        case GpioPinDriveMode_OutputOpenSourcePullDown:
            GPIO_setConfig(pinConfigIndex, currentPinConfig | GPIO_CFG_OUT_OD_PD);
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
    if ((driveMode == GpioPinDriveMode_Input) || (driveMode == GpioPinDriveMode_InputPullDown) ||
        (driveMode == GpioPinDriveMode_InputPullUp) || (driveMode == GpioPinDriveMode_Output) ||
        (driveMode == GpioPinDriveMode_OutputOpenDrain) || (driveMode == GpioPinDriveMode_OutputOpenDrainPullUp) ||
        (driveMode == GpioPinDriveMode_OutputOpenSourcePullDown))
    {
        driveModeSupported = true;
    }

    return driveModeSupported;
}

uint32_t CPU_GPIO_GetPinDebounce(GPIO_PIN pinNumber)
{
    gpio_input_state *pState = GetInputState(pinNumber);

    _ASSERTE(pState == NULL);

    return pState->debounceMs;
}

bool CPU_GPIO_SetPinDebounce(GPIO_PIN pinNumber, CLR_UINT64 debounceTimeMilliseconds)
{
    gpio_input_state *pState = GetInputState(pinNumber);

    // can only change the debounce in pin state if the pin has already been configured as input
    // if not, doesn't matter, because the new debounce will be used next time it's required
    if (pState != NULL)
    {
        pState->debounceMs = (uint32_t)(debounceTimeMilliseconds);
    }

    return true;
}
