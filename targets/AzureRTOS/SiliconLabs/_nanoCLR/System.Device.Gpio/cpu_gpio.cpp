//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <tx_api.h>
#include "sys_dev_gpio_native_target.h"

#define GPIO_MAX_PIN     256
#define TOTAL_GPIO_PORTS ((GPIO_MAX_PIN + 15) / 16)

// Double linkedlist to hold the state of each Input pin
struct gpio_input_state : public HAL_DblLinkedNode<gpio_input_state>
{
    // Pin number
    GPIO_PIN pinNumber;
    // debounce timer for this Pin
    TX_TIMER debounceTimer;
    // Ptr to user ISR or null
    GPIO_INTERRUPT_SERVICE_ROUTINE isrPtr;
    // debounce Millsecs, no debonce=0
    uint32_t debounceMs;
    // Interrupt mode
    uint8_t mode;
    // Param to user isr call
    void *param;
    // Expected state for debounce handler
    bool expected;
    // True if waiting for debounce timer to complete
    bool waitingDebounce;
};

static HAL_DblLinkedList<gpio_input_state> gpioInputList; // Double Linked list for GPIO input status
static uint16_t pinReserved[TOTAL_GPIO_PORTS];            //  reserved - 1 bit per pin

// this is an utility function to get a Gecko API port and pin from our "encoded" pin number
void GetIoLine(int16_t pinNumber, GPIO_Port_TypeDef *port, uint32_t *portPin)
{
    *port = GPIO_PORT(pinNumber);
    *portPin = pinNumber % 16;
}

bool IsValidGpioPin(GPIO_PIN pinNumber)
{
    return (pinNumber <= GPIO_MAX_PIN);
}

// Get pointer to gpio_input_state for GPIO pin
// return NULL if not found
gpio_input_state *GetGpioWithInterrupt(uint16_t gpioPin)
{
    gpio_input_state *ptr = gpioInputList.FirstNode();
    while (ptr->Next() != NULL)
    {
        if (GPIO_PIN(ptr->pinNumber) == gpioPin)
        {
            return ptr;
        }

        ptr = ptr->Next();
    }

    return NULL;
}

static void DebounceTimerCallback(uint32_t id)
{
    gpio_input_state *pState = (gpio_input_state *)id;

    GPIO_Port_TypeDef port;
    uint32_t portPin;
    GetIoLine(pState->pinNumber, &port, &portPin);

    // get current pin state
    bool actual = GPIO_PinInGet(port, portPin);

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

    pState->waitingDebounce = false;
}

void GpioEventCallback(void *arg)
{
    NATIVE_INTERRUPT_START

    TX_DISABLE

    gpio_input_state *pGpio = (gpio_input_state *)arg;

    if (pGpio != NULL)
    {
        // Ignore any pin changes during debounce
        if (!pGpio->waitingDebounce)
        {
            // check if there is a debounce time set
            if (pGpio->debounceMs > 0)
            {
                // Set flag we are waiting for debounce on this pin
                pGpio->waitingDebounce = true;

                // setup timer
                tx_timer_deactivate(&pGpio->debounceTimer);
                tx_timer_change(&pGpio->debounceTimer, 0, pGpio->debounceMs / 10);
                tx_timer_activate(&pGpio->debounceTimer);
            }
            else
            {
                GPIO_Port_TypeDef port;
                uint32_t portPin;
                GetIoLine(pGpio->pinNumber, &port, &portPin);

                TX_RESTORE

                pGpio->isrPtr(pGpio->pinNumber, GPIO_PinInGet(port, portPin), pGpio->param);

                TX_DISABLE
            }
        }
    }

    TX_RESTORE

    NATIVE_INTERRUPT_END
}

// Get pointer to gpio_input_state for Gpio pin
// return NULL if not found
gpio_input_state *GetInputState(GPIO_PIN pinNumber)
{
    gpio_input_state *ptr = gpioInputList.FirstNode();

    while (ptr->Next() != NULL)
    {
        if (ptr->pinNumber == pinNumber)
        {
            return ptr;
        }

        ptr = ptr->Next();
    }

    return NULL;
}

// Allocate a new gpio_input_state and add to end of list
// if already exist then just return current ptr
gpio_input_state *AllocateGpioInputState(GPIO_PIN pinNumber)
{
    gpio_input_state *ptr = GetInputState(pinNumber);

    if (ptr == NULL)
    {
        ptr = (gpio_input_state *)platform_malloc(sizeof(gpio_input_state));

        // sanity check
        if (ptr != NULL)
        {
            memset(ptr, 0, sizeof(gpio_input_state));
            ptr->pinNumber = pinNumber;

            tx_timer_create(
                &ptr->debounceTimer,
                (char *)"GPIO debounce timer",
                DebounceTimerCallback,
                0,
                0,
                1,
                TX_NO_ACTIVATE);

            gpioInputList.LinkAtBack(ptr);
        }
    }

    return ptr;
}

void UnlinkInputState(gpio_input_state *pState)
{
    tx_timer_delete(&pState->debounceTimer);

    GPIO_Port_TypeDef port;
    uint32_t portPin;
    GetIoLine(pState->pinNumber, &port, &portPin);

    // disable the EXT interrupt channel
    // it's OK to do always this, no matter if it's enabled or not
    // TODO
    // palDisableLineEvent(GetIoLine(pState->pinNumber));

    pState->Unlink();

    platform_free(pState);
}

// Delete gpio_input_state from List and tidy up ( Timer & ISR handler )
void DeleteInputState(GPIO_PIN pinNumber)
{
    gpio_input_state *pState = GetInputState(pinNumber);

    if (pState)
    {
        UnlinkInputState(pState);
    }
}

bool CPU_GPIO_Initialize()
{
    // Initialise Double linked list for input pin states
    gpioInputList.Initialize();

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
    // Check if valid pin number
    if (!IsValidGpioPin(pinNumber))
    {
        return false;
    }

    int port = pinNumber >> 4, bit = 1 << (pinNumber & 0x0F);
    bool ret = true;
    GLOBAL_LOCK();

    if (fReserve)
    {
        if (pinReserved[port] & bit)
        {
            // already reserved
            ret = false;
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
    if (!IsValidGpioPin(pinNumber))
    {
        return false;
    }

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
    GPIO_Port_TypeDef port;
    uint32_t portPin;
    GetIoLine(pin, &port, &portPin);

    return (GpioPinValue)GPIO_PinInGet(port, portPin);
}

// Set Pin state
void CPU_GPIO_SetPinState(GPIO_PIN pin, GpioPinValue pinState)
{
    GPIO_Port_TypeDef port;
    uint32_t portPin;
    GetIoLine(pin, &port, &portPin);

    if (pinState == GpioPinValue_High)
    {
        GPIO_PortOutSet(port, portPin);
    }
    else
    {
        GPIO_PortOutClear(port, portPin);
    }
}

void CPU_GPIO_TogglePinState(GPIO_PIN pin)
{
    GPIO_Port_TypeDef port;
    uint32_t portPin;
    GetIoLine(pin, &port, &portPin);

    GPIO_PortOutToggle(port, portPin);
}

bool CPU_GPIO_EnableInputPin(
    GPIO_PIN pinNumber,
    uint32_t debounceTimeMilliseconds,
    GPIO_INTERRUPT_SERVICE_ROUTINE pinISR,
    void *isrParam,
    GPIO_INT_EDGE intEdge,
    PinMode driveMode)
{
    gpio_input_state *pState;

    // Check Input drive mode
    if (driveMode >= (int)PinMode_Output)
    {
        return false;
    }

    // Set as Input GPIO_INT_EDGE intEdge, GPIO_RESISTOR ResistorState
    if (!CPU_GPIO_SetDriveMode(pinNumber, driveMode))
    {
        return false;
    }

    pState = AllocateGpioInputState(pinNumber);

    GPIO_Port_TypeDef port;
    uint32_t portPin;
    GetIoLine(pinNumber, &port, &portPin);

    // Link ISR ptr supplied and not already set up
    // CPU_GPIO_EnableInputPin could be called a 2nd time with changed parameters
    if (pinISR != NULL && (pState->isrPtr == NULL))
    {
        // there are callbacks registered and...
        // the drive mode is input so need to setup the interrupt

        GPIO_ExtIntConfig(port, portPin, portPin, 1, 1, true);

        // TODO
        NVIC_EnableIRQ(GPIO_EVEN_IRQn);
        // palSetLineCallback(ioLine, GpioEventCallback, pState);

        // store parameters & configs
        pState->isrPtr = pinISR;
        pState->mode = intEdge;
        pState->param = (void *)isrParam;
        pState->debounceMs = (uint32_t)(debounceTimeMilliseconds);

        switch (intEdge)
        {
            case GPIO_INT_EDGE_LOW:
            case GPIO_INT_LEVEL_LOW:
                pState->expected = 1;
                break;

            case GPIO_INT_EDGE_HIGH:
            case GPIO_INT_LEVEL_HIGH:
                pState->expected = 0;
                break;

            case GPIO_INT_EDGE_BOTH:
                pState->expected = !CPU_GPIO_GetPinState(pinNumber); // expected NOT current state
                break;

            default:
                break;
        }
    }
    else if (pinISR == NULL && (pState->isrPtr != NULL))
    {
        // there is no managed handler setup anymore

        // disable the EXT interrupt channel
        // it's OK to do always this, no matter if it's enabled or not
        // TODO
        // palDisableLineEvent(GetIoLine(pState->pinNumber));

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
// return       -   True if successful, false invalid pin, pin not output, invalid drive mode for output
//
bool CPU_GPIO_EnableOutputPin(GPIO_PIN pinNumber, GpioPinValue InitialState, PinMode driveMode)
{
    // check not an output drive mode
    if (driveMode < (int)PinMode_Output)
    {
        return false;
    }

    // If this is currently an input pin then clean up
    DeleteInputState(pinNumber);

    if (CPU_GPIO_SetDriveMode(pinNumber, driveMode) == false)
    {
        return false;
    }

    CPU_GPIO_SetPinState(pinNumber, InitialState);

    return true;
}

void CPU_GPIO_DisablePin(GPIO_PIN pinNumber, PinMode driveMode, uint32_t alternateFunction)
{
    DeleteInputState(pinNumber);

    GLOBAL_LOCK();

    CPU_GPIO_SetDriveMode(pinNumber, driveMode);

    GPIO_Port_TypeDef port;
    uint32_t portPin;
    GetIoLine(pinNumber, &port, &portPin);

    GPIO_PinModeSet(port, portPin, (GPIO_Mode_TypeDef)alternateFunction, 0);

    GLOBAL_UNLOCK();

    CPU_GPIO_ReservePin(pinNumber, false);
}

// Validate pin and set drive mode
// return true if ok
bool CPU_GPIO_SetDriveMode(GPIO_PIN pinNumber, PinMode driveMode)
{
    GPIO_Port_TypeDef port;
    uint32_t portPin;
    GetIoLine(pinNumber, &port, &portPin);

    switch (driveMode)
    {
        case PinMode_Input:
            GPIO_PinModeSet(port, portPin, gpioModeInput, 0);
            break;

        case PinMode_InputPullDown:
            GPIO_PinModeSet(port, portPin, gpioModeInputPull, 0);
            break;

        case PinMode_InputPullUp:
            GPIO_PinModeSet(port, portPin, gpioModeInputPull, 1);
            break;

        case PinMode_Output:
            GPIO_PinModeSet(port, portPin, gpioModePushPull, 0);

            break;

        case PinMode_OutputOpenDrain:
            GPIO_PinModeSet(port, portPin, gpioModeWiredAnd, 1);
            break;

        default:
            // all other modes are NOT supported
            return false;
    }

    return true;
}

bool CPU_GPIO_DriveModeSupported(GPIO_PIN pinNumber, PinMode driveMode)
{
    (void)pinNumber;

    bool driveModeSupported = false;

    // check if the requested drive mode is support by ChibiOS config
    if ((driveMode == PinMode_Input) || (driveMode == PinMode_InputPullDown) || (driveMode == PinMode_InputPullUp) ||
        (driveMode == PinMode_Output) || (driveMode == PinMode_OutputOpenDrain))
    {
        driveModeSupported = true;
    }

    return driveModeSupported;
}

uint32_t CPU_GPIO_GetPinDebounce(GPIO_PIN pinNumber)
{
    gpio_input_state *ptr = GetInputState(pinNumber);

    if (ptr)
    {
        return ptr->debounceMs;
    }

    return 0;
}

bool CPU_GPIO_SetPinDebounce(GPIO_PIN pinNumber, uint32_t debounceTimeMilliseconds)
{
    gpio_input_state *ptr = GetInputState(pinNumber);

    if (ptr)
    {
        ptr->debounceMs = debounceTimeMilliseconds;
        return true;
    }

    return false;
}
