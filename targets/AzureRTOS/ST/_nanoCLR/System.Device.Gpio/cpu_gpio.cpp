//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <tx_api.h>
#include "sys_dev_gpio_native_target.h"

#define GPIO_MAX_PIN     256
#define TOTAL_GPIO_PORTS ((GPIO_MAX_PIN + 15) / 16)

// Double linked list to hold the state of each Input pin
struct gpio_input_state : public HAL_DblLinkedNode<gpio_input_state>
{
    // pin number
    GPIO_PIN pinNumber;
    // debounce timer for this pin
    TX_TIMER debounceTimer;
    // pointer to user ISR or null
    GPIO_INTERRUPT_SERVICE_ROUTINE isrPtr;
    // debounce milliseconds, NO debounce set to 0
    uint32_t debounceMs;
    // Interrupt mode
    uint8_t mode;
    // Param to user ISR call
    void *param;
    // Expected state for debounce handler
    bool expected;
    // True if waiting for debounce timer to complete
    bool waitingDebounce;
};

// Double Linked list for GPIO input status
static HAL_DblLinkedList<gpio_input_state> gpioInputList;
//  reserved - 1 bit per pin
static uint16_t pinReserved[TOTAL_GPIO_PORTS];

// this is an utility function to get a ChibiOS PAL IoLine from our "encoded" pin number
static ioline_t GetIoLine(int16_t pinNumber)
{
    stm32_gpio_t *port = GPIO_PORT(pinNumber);
    int16_t pad = pinNumber % 16;

    return PAL_LINE(port, pad);
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

static void DebounceTimerCallback(ULONG pinState)
{
    gpio_input_state *pState = (gpio_input_state *)pinState;

    // get current pin state
    bool actual = palReadLine(GetIoLine(pState->pinNumber));

    if (actual == pState->expected)
    {
        // call ISR
        pState->isrPtr(pState->pinNumber, actual, pState->param);
    }

    // reset flag
    pState->waitingDebounce = false;
}

void GpioEventCallback(void *arg)
{
    NATIVE_INTERRUPT_START

    TX_DISABLE

    gpio_input_state *pGpio = (gpio_input_state *)arg;

    if (pGpio != NULL)
    {
        if (pGpio->waitingDebounce)
        {
            // Ignore any pin changes during debounce
        }
        else
        {
            // check if there is a debounce time set
            if (pGpio->debounceMs > 0)
            {
                // stop timer, just in case
                tx_timer_deactivate(&pGpio->debounceTimer);
                // Set flag we are waiting for debounce on this pin
                pGpio->waitingDebounce = true;

                // expecting same state as current one
                pGpio->expected = CPU_GPIO_GetPinState(pGpio->pinNumber);

                // setup timer
                tx_timer_change(&pGpio->debounceTimer, TX_TICKS_PER_MILLISEC(pGpio->debounceMs), 0);
                tx_timer_activate(&pGpio->debounceTimer);
            }
            else
            {
                pGpio->isrPtr(pGpio->pinNumber, CPU_GPIO_GetPinState(pGpio->pinNumber), pGpio->param);
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
                (ULONG)ptr,
                1,
                0,
                TX_NO_ACTIVATE);

            gpioInputList.LinkAtBack(ptr);
        }
    }

    return ptr;
}

void UnlinkInputState(gpio_input_state *pState)
{
    tx_timer_delete(&pState->debounceTimer);

    // disable the EXT interrupt channel
    // it's OK to do always this, no matter if it's enabled or not
    palDisableLineEvent(GetIoLine(pState->pinNumber));

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
    return (GpioPinValue)palReadLine(GetIoLine(pin));
}

// Set Pin state
void CPU_GPIO_SetPinState(GPIO_PIN pin, GpioPinValue pinState)
{
    palWriteLine(GetIoLine(pin), (int)pinState);
}

void CPU_GPIO_TogglePinState(GPIO_PIN pinNumber)
{
    palToggleLine(GetIoLine(pinNumber));
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

    // Link ISR ptr supplied and not already set up
    // CPU_GPIO_EnableInputPin could be called a 2nd time with changed parameters
    if (pinISR != NULL && (pState->isrPtr == NULL))
    {
        // there are callbacks registered and...
        // the drive mode is input so need to setup the interrupt

        // get IoLine from pin number
        ioline_t ioLine = GetIoLine(pinNumber);

        palEnableLineEvent(ioLine, PAL_EVENT_MODE_BOTH_EDGES);
        palSetLineCallback(ioLine, GpioEventCallback, pState);

        // store parameters & configs
        pState->isrPtr = pinISR;
        pState->mode = intEdge;
        pState->param = (void *)isrParam;
        pState->debounceMs = (uint32_t)(debounceTimeMilliseconds);

        switch (intEdge)
        {
            case GPIO_INT_EDGE_LOW:
            case GPIO_INT_LEVEL_LOW:
                pState->expected = PAL_LOW;
                break;

            case GPIO_INT_EDGE_HIGH:
            case GPIO_INT_LEVEL_HIGH:
                pState->expected = PAL_HIGH;
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
        palDisableLineEvent(GetIoLine(pState->pinNumber));

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

    // get IoLine from pin number
    ioline_t ioLine = GetIoLine(pinNumber);
    palSetLineMode(ioLine, PAL_MODE_ALTERNATE(alternateFunction));

    GLOBAL_UNLOCK();

    CPU_GPIO_ReservePin(pinNumber, false);
}

// Validate pin and set drive mode
// return true if ok
bool CPU_GPIO_SetDriveMode(GPIO_PIN pinNumber, PinMode driveMode)
{
    // get IoLine from pin number
    ioline_t ioLine = GetIoLine(pinNumber);

    switch (driveMode)
    {
        case PinMode_Input:
            palSetLineMode(ioLine, PAL_MODE_INPUT);
            break;

        case PinMode_InputPullDown:
            palSetLineMode(ioLine, PAL_MODE_INPUT_PULLDOWN);
            break;

        case PinMode_InputPullUp:
            palSetLineMode(ioLine, PAL_MODE_INPUT_PULLUP);
            break;

        case PinMode_Output:
            palSetLineMode(ioLine, PAL_MODE_OUTPUT_PUSHPULL);
            break;

        case PinMode_OutputOpenDrain:
            palSetLineMode(ioLine, PAL_MODE_OUTPUT_OPENDRAIN);
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
