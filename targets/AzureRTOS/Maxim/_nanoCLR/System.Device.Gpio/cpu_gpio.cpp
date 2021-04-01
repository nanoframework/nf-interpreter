//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <tx_api.h>

#include <targetPAL.h>
#include "sys_dev_gpio_native_target.h"

#define GPIO_MAX_PIN     256
#define TOTAL_GPIO_PORTS ((GPIO_MAX_PIN + 15) / 16)

// TODO
//  move this to sys_dev_gpio_native_System_Device_Gpio_GpioPin when Windows.Devices.Gpio is removed
void Gpio_Interupt_ISR(GPIO_PIN pinNumber, bool pinState, void *pArg)
{
    (void)pArg;

    // if handle registered then post a managed event with the current pin reading
    PostManagedEvent(EVENT_GPIO, 0, (uint16_t)pinNumber, (uint32_t)pinState);
}

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
    GPIO_PinState expected;
    // True if waiting for debounce timer to complete
    bool waitingDebounce;
};

static HAL_DblLinkedList<gpio_input_state> gpioInputList; // Double Linked list for GPIO input status
static uint16_t pinReserved[TOTAL_GPIO_PORTS];            //  reserved - 1 bit per pin

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
    GPIO_TypeDef *port = GPIO_PORT(pState->pinNumber);

    // get current pin state
    bool actual = HAL_GPIO_ReadPin(port, GPIO_PIN(pState->pinNumber));

    if (actual == pState->expected)
    {
        pState->isrPtr(pState->pinNumber, actual, pState->param);
        if (pState->mode == GPIO_INT_EDGE_BOTH)
        {
            // both edges
            // update expected state
            pState->expected = (GPIO_PinState)(pState->expected ^ GPIO_PIN_SET);
        }
    }

    pState->waitingDebounce = false;
}

void HAL_GPIO_EXTI_Callback(uint16_t gpioPin)
{
    NATIVE_INTERRUPT_START

    TX_INTERRUPT_SAVE_AREA

    TX_DISABLE

    // find pin number for this PIN
    // ST GPIO allows only one INT per GPIO pin (16 channels)
    // find which is our pin that corresponds to the GPIO that fired the interrupt
    gpio_input_state *pGpio = GetGpioWithInterrupt(gpioPin);

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
                TX_RESTORE

                pGpio->isrPtr(
                    pGpio->pinNumber,
                    HAL_GPIO_ReadPin(GPIO_PORT(pGpio->pinNumber), GPIO_PIN(pGpio->pinNumber)),
                    pGpio->param);

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
            return ptr;
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

    // disable the EXT interrupt channel
    // it's OK to do always this, no matter if it's enabled or not
    // TODO
    //    palDisableLineEvent(GetIoLine(pState->pinNumber));

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
        return false;

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
        return false;

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
    GPIO_TypeDef *port = GPIO_PORT(pin);

    return (GpioPinValue)HAL_GPIO_ReadPin(port, GPIO_PIN(pin));
}

// Set Pin state
void CPU_GPIO_SetPinState(GPIO_PIN pin, GpioPinValue pinState)
{
    GPIO_TypeDef *port = GPIO_PORT(pin);

    HAL_GPIO_WritePin(port, GPIO_PIN(pin), (GPIO_PinState)pinState);
}

void CPU_GPIO_TogglePinState(GPIO_PIN pin)
{
    GPIO_TypeDef *port = GPIO_PORT(pin);

    HAL_GPIO_TogglePin(port, GPIO_PIN(pin));
}

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

    // enable clock
    GPIO_TypeDef *port = GPIO_PORT(pinNumber);

#ifdef GPIOA
    if (port == GPIOA)
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();
    }
#endif

#ifdef GPIOB
    if (port == GPIOB)
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();
    }
#endif

#ifdef GPIOC
    if (port == GPIOC)
    {
        __HAL_RCC_GPIOC_CLK_ENABLE();
    }
#endif

#ifdef GPIOD
    if (port == GPIOD)
    {
        __HAL_RCC_GPIOD_CLK_ENABLE();
    }
#endif

#ifdef GPIOE
    if (port == GPIOE)
    {
        __HAL_RCC_GPIOE_CLK_ENABLE();
    }
#endif

#ifdef GPIOF
    if (port == GPIOF)
    {
        __HAL_RCC_GPIOF_CLK_ENABLE();
    }
#endif

#ifdef GPIOG
    if (port == GPIOG)
    {
        __HAL_RCC_GPIOG_CLK_ENABLE();
    }
#endif

#ifdef GPIOH
    if (port == GPIOH)
    {
        __HAL_RCC_GPIOH_CLK_ENABLE();
    }
#endif

    // Set as Input GPIO_INT_EDGE intEdge, GPIO_RESISTOR ResistorState
    if (!CPU_GPIO_SetDriveMode(pinNumber, driveMode))
    {
        return false;
    }

    pState = AllocateGpioInputState(pinNumber);

    GPIO_InitTypeDef gpio_init_structure;
    gpio_init_structure.Pin = GPIO_PIN(pinNumber);

    HAL_GPIO_GetConfig(port, &gpio_init_structure);

    // Link ISR ptr supplied and not already set up
    // CPU_GPIO_EnableInputPin could be called a 2nd time with changed parameters
    if (pinISR != NULL && (pState->isrPtr == NULL))
    {
        // there are callbacks registered and...
        // the drive mode is input so need to setup the interrupt

        // sanity check for a GPIO pin already associated to this channel
        gpio_input_state *pGpio = GetGpioWithInterrupt(gpio_init_structure.Pin);
        if (pGpio != NULL)
        {
            // there is already one!
            CPU_GPIO_DisablePin(pinNumber, driveMode, 0);

            return false;
        }

        gpio_init_structure.Mode = GPIO_MODE_IT_RISING_FALLING;
        HAL_GPIO_SetConfig(port, &gpio_init_structure);

        // store parameters & configs
        pState->isrPtr = pinISR;
        pState->mode = intEdge;
        pState->param = (void *)isrParam;
        pState->debounceMs = (uint32_t)(debounceTimeMilliseconds);

        switch (intEdge)
        {
            case GPIO_INT_EDGE_LOW:
            case GPIO_INT_LEVEL_LOW:
                pState->expected = GPIO_PIN_RESET;
                break;

            case GPIO_INT_EDGE_HIGH:
            case GPIO_INT_LEVEL_HIGH:
                pState->expected = GPIO_PIN_SET;
                break;

            case GPIO_INT_EDGE_BOTH:
                // expected NOT current state
                pState->expected = (GPIO_PinState)!CPU_GPIO_GetPinState(pinNumber);
                break;

            default:
                break;
        }
    }
    else if (pinISR == NULL && (pState->isrPtr != NULL))
    {
        // there is no managed handler setup anymore
        // need to clear pin, which includes interrupts
        HAL_GPIO_DeInit(port, gpio_init_structure.Pin);

        // call again
        CPU_GPIO_SetDriveMode(pinNumber, driveMode);

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
    // check not an output drive mode
    if (driveMode < (int)GpioPinDriveMode_Output)
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

void CPU_GPIO_DisablePin(GPIO_PIN pinNumber, GpioPinDriveMode driveMode, uint32_t alternateFunction)
{
    DeleteInputState(pinNumber);

    GLOBAL_LOCK();

    GPIO_TypeDef *port = GPIO_PORT(pinNumber);
    uint32_t pin = GPIO_PIN(pinNumber);
    
    GPIO_InitTypeDef gpio_init_structure;
    gpio_init_structure.Pin = pin;

    // deinit pin
    HAL_GPIO_DeInit(port, pin);

    // set drive mode
    CPU_GPIO_SetDriveMode(pinNumber, driveMode);

    // set AF, if different than 0
    if (alternateFunction)
    {
        HAL_GPIO_GetConfig(port, &gpio_init_structure);

        // change AF
        gpio_init_structure.Alternate = alternateFunction;

        HAL_GPIO_SetConfig(port, &gpio_init_structure);
    }

    GLOBAL_UNLOCK();

    CPU_GPIO_ReservePin(pinNumber, false);
}

// Validate pin and set drive mode
// return true if ok
bool CPU_GPIO_SetDriveMode(GPIO_PIN pinNumber, GpioPinDriveMode driveMode)
{
    GPIO_InitTypeDef gpio_init_structure;
    GPIO_TypeDef *port = GPIO_PORT(pinNumber);
    gpio_init_structure.Pin = GPIO_PIN(pinNumber);

    HAL_GPIO_GetConfig(port, &gpio_init_structure);

    // clear AF
    gpio_init_structure.Alternate = 0;
    // default speed
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

    switch (driveMode)
    {
        case GpioPinDriveMode_Input:
            gpio_init_structure.Mode = GPIO_MODE_INPUT;
            gpio_init_structure.Pull = GPIO_NOPULL;
            break;

        case GpioPinDriveMode_InputPullDown:
            gpio_init_structure.Mode = GPIO_MODE_INPUT;
            gpio_init_structure.Pull = GPIO_PULLDOWN;
            break;

        case GpioPinDriveMode_InputPullUp:
            gpio_init_structure.Mode = GPIO_MODE_INPUT;
            gpio_init_structure.Pull = GPIO_PULLUP;
            break;

        case GpioPinDriveMode_Output:
            gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
            gpio_init_structure.Pull = GPIO_NOPULL;
            break;

        case GpioPinDriveMode_OutputOpenDrain:
            gpio_init_structure.Mode = GPIO_MODE_OUTPUT_OD;
            gpio_init_structure.Pull = GPIO_NOPULL;
            break;

        default:
            // all other modes are NOT supported
            return false;
    }

    HAL_GPIO_SetConfig(port, &gpio_init_structure);

    return true;
}

bool CPU_GPIO_DriveModeSupported(GPIO_PIN pinNumber, GpioPinDriveMode driveMode)
{
    (void)pinNumber;

    bool driveModeSupported = false;

    // check if the requested drive mode is support by ChibiOS config
    if ((driveMode == GpioPinDriveMode_Input) || (driveMode == GpioPinDriveMode_InputPullDown) ||
        (driveMode == GpioPinDriveMode_InputPullUp) || (driveMode == GpioPinDriveMode_Output) ||
        (driveMode == GpioPinDriveMode_OutputOpenDrain))
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

bool CPU_GPIO_SetPinDebounce(GPIO_PIN pinNumber, CLR_UINT64 debounceTimeMilliseconds)
{
    gpio_input_state *ptr = GetInputState(pinNumber);
    if (ptr)
    {
        ptr->debounceMs = (uint32_t)(debounceTimeMilliseconds);
        return true;
    }
    return false;
}
