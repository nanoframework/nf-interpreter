//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <tx_api.h>
#include "sys_dev_gpio_native_target.h"

#define _GPIOINT_IF_EVEN_MASK ((_GPIO_IF_MASK)&0x55555555UL)
#define _GPIOINT_IF_ODD_MASK  ((_GPIO_IF_MASK)&0xAAAAAAAAUL)
#define INTERRUPT_UNAVAILABLE (0xFF)

#define GPIO_MAX_PIN                                                                                                   \
    _GPIO_PORT_A_PIN_COUNT + _GPIO_PORT_B_PIN_COUNT + _GPIO_PORT_C_PIN_COUNT + _GPIO_PORT_D_PIN_COUNT +                \
        _GPIO_PORT_E_PIN_COUNT + _GPIO_PORT_F_PIN_COUNT + _GPIO_PORT_G_PIN_COUNT + _GPIO_PORT_H_PIN_COUNT +            \
        _GPIO_PORT_I_PIN_COUNT + _GPIO_PORT_J_PIN_COUNT + _GPIO_PORT_K_PIN_COUNT

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
static uint16_t pinReserved[GPIO_PORT_MAX];

// Array of user callbacks. One for each pin interrupt number.
static gpio_input_state *gpioCallbacks[32];

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

// Register (or unregister) ISR for a pin
// send NULL in pinState to unregister callback
unsigned int CallbackRegisterExt(uint8_t pin, gpio_input_state *pinState)
{
    uint16_t intNo = INTERRUPT_UNAVAILABLE;

    TX_DISABLE

#if defined(_GPIO_EXTIPINSELL_MASK)
    uint32_t intToCheck;
    uint32_t intGroupStart = (pin & 0xFFC);
    uint32_t intsEnabled = GPIO_EnabledIntGet();

    // loop through the interrupt group, starting
    // from the pin number, and take
    // the first available
    for (uint8_t i = 0; i < 4; i++)
    {
        intToCheck = intGroupStart + ((pin + i) & 0x3); // modulo 4
        if (((intsEnabled >> intToCheck) & 0x1) == 0)
        {
            intNo = (unsigned int)intToCheck;
            break;
        }
    }
#else
    if (gpioCallbacks[pin] == NULL)
    {
        intNo = (unsigned int)pin;
    }
#endif

    if (intNo != INTERRUPT_UNAVAILABLE)
    {
        gpioCallbacks[intNo] = pinState;
    }

    TX_RESTORE

    return intNo;
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

    GPIO_Port_TypeDef port;
    uint32_t portPin;
    GetIoLine(pState->pinNumber, &port, &portPin);

    // get current pin state
    bool actual = GPIO_PinInGet(port, portPin);

    if (actual == pState->expected)
    {
        // call ISR
        pState->isrPtr(pState->pinNumber, actual, pState->param);
    }

    // reset flag
    pState->waitingDebounce = false;
}

static void GpioEventCallback(uint32_t intFlags)
{
    uint32_t irqIdx;
    gpio_input_state *pGpio;

    NATIVE_INTERRUPT_START

    TX_DISABLE

    // check for all flags set in IF register
    while (intFlags != 0U)
    {
        irqIdx = SL_CTZ(intFlags);

        /* clear flag*/
        intFlags &= ~(1UL << irqIdx);

        pGpio = gpioCallbacks[irqIdx];

        if (pGpio != NULL)
        {
            if (pGpio->waitingDebounce)
            {
                // Ignore any pin changes during debounce
                continue;
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
    }

    TX_RESTORE

    NATIVE_INTERRUPT_END
}

// Get pointer to gpio_input_state for GPIO pin
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

    GPIO_Port_TypeDef port;
    uint32_t portPin;
    GetIoLine(pState->pinNumber, &port, &portPin);

    // disable the EXT interrupt channel
    // it's OK to do always this, no matter if it's enabled or not
    CallbackRegisterExt(portPin, NULL);

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

    // "free" all GPIO interrupts
    memset(gpioCallbacks, 0x0, sizeof(gpioCallbacks));

    // enable the interrupt handler for all GPIOs
    if (CORE_NvicIRQDisabled(GPIO_ODD_IRQn))
    {
        NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
        NVIC_EnableIRQ(GPIO_ODD_IRQn);
    }
    if (CORE_NvicIRQDisabled(GPIO_EVEN_IRQn))
    {
        NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
        NVIC_EnableIRQ(GPIO_EVEN_IRQn);
    }

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
        GPIO_PinOutSet(port, portPin);
    }
    else
    {
        GPIO_PinOutClear(port, portPin);
    }
}

void CPU_GPIO_TogglePinState(GPIO_PIN pin)
{
    GPIO_Port_TypeDef port;
    uint32_t portPin;
    GetIoLine(pin, &port, &portPin);

    GPIO_PinOutToggle(port, portPin);
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
    int interruptId;
    GetIoLine(pinNumber, &port, &portPin);

    // Link ISR ptr supplied and not already set up
    // CPU_GPIO_EnableInputPin could be called a 2nd time with changed parameters
    if (pinISR != NULL && (pState->isrPtr == NULL))
    {
        // register nanoFramework callback and store associated interrupt ID
        interruptId = CallbackRegisterExt(portPin, pState);

        // there are callbacks registered and...
        // the drive mode is input so need to setup the interrupt
        // need to use the interrupt ID to setup the interrupt
        GPIO_ExtIntConfig(port, portPin, interruptId, 1, 1, true);

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
        CallbackRegisterExt(portPin, NULL);

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

void GPIO_EVEN_IRQHandler(void)
{
    NATIVE_INTERRUPT_START

    uint32_t intFlags;

    // Get all even interrupts
    intFlags = GPIO_IntGetEnabled() & _GPIOINT_IF_EVEN_MASK;

    // Clean only even interrupts
    GPIO_IntClear(intFlags);

    GpioEventCallback(intFlags);

    NATIVE_INTERRUPT_END
}

void GPIO_ODD_IRQHandler(void)
{
    NATIVE_INTERRUPT_START

    uint32_t intFlags;

    // Get all odd interrupts
    intFlags = GPIO_IntGetEnabled() & _GPIOINT_IF_ODD_MASK;

    // Clean only odd interrupts
    GPIO_IntClear(intFlags);

    GpioEventCallback(intFlags);

    NATIVE_INTERRUPT_END
}
