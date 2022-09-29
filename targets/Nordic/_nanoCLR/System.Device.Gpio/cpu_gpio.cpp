//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

//
// Troublsome

#undef GPIO_INT_EDGE

#include <drivers/gpio.h>
#include <sys/types.h>
#include <sys\util.h>
#include <zephyr.h>
#include <zephyr/types.h>

// Troublesome naming conflict between zephyr and.net nf.
// GPIO_INT_EDGE is a bit define in zephyr and an enum
// in .net nf.
// These are used by the code.
// const uint32_t Z_GPIO_INT_EDGE = GPIO_INT_EDGE;
const uint32_t Z_GPIO_INT_LEVEL_HIGH = GPIO_INT_LEVEL_HIGH;
const uint32_t Z_GPIO_INT_LEVEL_LOW = GPIO_INT_LEVEL_LOW;
const uint32_t Z_GPIO_INT_RISING = GPIO_INT_EDGE_RISING;
const uint32_t Z_GPIO_INT_FALLING = GPIO_INT_EDGE_FALLING;

#undef GPIO_INT_EDGE
#undef GPIO_INT_EDGE_BOTH
#undef GPIO_INT_EDGE_HIGH
#undef GPIO_INT_LEVEL_HIGH
#undef GPIO_INT_LEVEL_LOW
#undef GPIO_INT_ENABLE

#include <nanoHAL_v2.h>
#include "CPU_GPIO_decl.h"
#include "sys_dev_gpio_native_target.h"
#include <targetPAL.h>

// TODO:: These two defines should be in a cpu-specific file.
#define GPIO_MAX_PIN     32
#define GPIO_PIN_PER_REG 32

// Generic macros
#define TOTAL_GPIO_PORTS      ((GPIO_MAX_PIN + (GPIO_PIN_PER_REG - 1)) / GPIO_PIN_PER_REG)
#define GET_GPIO_PORT(p)      (p / GPIO_PIN_PER_REG)
#define GPIO_OFFSET_IN_REG(p) (p % GPIO_PIN_PER_REG)
#define GPIO_BIT_MASK(p)      (1 << GPIO_OFFSET_IN_REG(p))

#define FLAG_EXPECTED_HIGH    0x0001
#define FLAG_WAITING_DEBOUNCE 0x0002
#define FLAG_INITIALIZED      0x0004

// Double linkedlist to hold the state of each Input pin
struct gpio_input_state : public HAL_DblLinkedNode<gpio_input_state>
{
    GPIO_PIN pinNumber;                    // Pin number
    struct k_work_delayable work_struct;   // Used for debouce and callback to nf
    struct gpio_callback gpio_cb;          // gpio irq callback struct
    GPIO_INTERRUPT_SERVICE_ROUTINE isrPtr; // Ptr to user ISR or null
    void *param;                           // Param to user isr call
    uint32_t debounceMs;                   // debounce Millsecs, no debonce=0
    uint8_t mode;                          // Interrupt mode
    uint8_t flags;                         // Flags; Init, debounce active, expected level
};

static HAL_DblLinkedList<gpio_input_state> gpioInputList; // Double Linked list for GPIO input status
static uint32_t pinReserved[TOTAL_GPIO_PORTS];            // reserved - 1 bit per pin
static const struct device *gpioDev[TOTAL_GPIO_PORTS];    // GPIO device handles

bool IsValidGpioPin(GPIO_PIN pinNumber)
{
    return (pinNumber <= GPIO_MAX_PIN);
}

const struct device *GetGpioDev(GPIO_PIN pinNumber)
{
    return gpioDev[GET_GPIO_PORT(pinNumber)];
}

int GetPinVal(uint32_t pinNumber)
{
    uint32_t bit = GPIO_BIT_MASK(pinNumber);
    uint32_t actual;
    int rc = gpio_port_get_raw(GetGpioDev(pinNumber), &actual);
    // Read the current value of the gpio register
    if (rc)
    {
        printk("**** Error calling gpio_port_get_raw rc %d\n", rc);
        return 0;
    }
    // Get the single bit state of the gpio in bit 0.
    return ((actual & bit) >> GPIO_OFFSET_IN_REG(pinNumber));
}
int SetPinVal(uint32_t pinNumber, bool set_val)
{
    uint32_t bit = GPIO_BIT_MASK(pinNumber);

    int rc = gpio_port_set_masked_raw(GetGpioDev(pinNumber), bit, set_val ? 0xffffffff : 0);
    if (rc)
    {
        printk("**** Error calling gpio_port_get_saw rc %d\n", rc);
    }
    return rc;
}

/*
 * WorkCallback - Zephyr delayed work item. Not an interrupt.
 */
static void WorkCallback(struct k_work *work)
{
    // NATIVE_INTERRUPT_START
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
    // gpio_input_state *pState = CONTAINER_OF(timer, struct gpio_input_state,
    // debounceTimer);
    gpio_input_state *pState = CONTAINER_OF(work, struct gpio_input_state, work_struct);
#pragma GCC diagnostic pop

    bool Callback = true;
    uint32_t actual = GetPinVal(pState->pinNumber);
    // if (pState->waitingDebounce)
    if (pState->flags & FLAG_WAITING_DEBOUNCE)
    {
        // if (actual != pState->expected)
        if (actual != ((pState->flags & FLAG_EXPECTED_HIGH) == 0))
        {
            Callback = false;
        }
        // pState->waitingDebounce = false;
        pState->flags &= ~FLAG_WAITING_DEBOUNCE;
    }
    if (Callback)
    {
        // Call back into engine
        pState->isrPtr(pState->pinNumber, actual, pState->param);
        if (pState->mode == GPIO_INT_EDGE_BOTH)
        {
            // both edges so toggle from current.
            // if (pState->flags & FLAG_EXPECTED_HIGH)
            if (actual)
            {
                pState->flags &= ~FLAG_EXPECTED_HIGH;
            }
            else
            {
                pState->flags |= FLAG_EXPECTED_HIGH;
            }
        }
    }
    // Reenable the interrupt
    gpio_pin_interrupt_configure(
        GetGpioDev(pState->pinNumber),
        GPIO_OFFSET_IN_REG(pState->pinNumber),
        (pState->flags & FLAG_EXPECTED_HIGH) ? Z_GPIO_INT_RISING : Z_GPIO_INT_FALLING);
    // NATIVE_INTERRUPT_END
}

static void GpioIrqCallback(const struct device *dev, struct gpio_callback *cb, gpio_port_pins_t pins)
{
    NATIVE_INTERRUPT_START

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
    gpio_input_state *pGpio = CONTAINER_OF(cb, struct gpio_input_state, gpio_cb);
#pragma GCC diagnostic pop

    // Disable the interrupt
    gpio_pin_interrupt_configure(dev, GPIO_OFFSET_IN_REG(pGpio->pinNumber), GPIO_INT_DISABLE);

    // check if there is a debounce time set
    if (pGpio->debounceMs > 0)
    {
        // Set flag we are waiting for debounce on this pin
        // pGpio->waitingDebounce = true;
        pGpio->flags |= FLAG_WAITING_DEBOUNCE;

        k_work_schedule(&pGpio->work_struct, K_MSEC(pGpio->debounceMs));
    }
    else
    {
        // pGpio->waitingDebounce = false;
        pGpio->flags &= ~FLAG_WAITING_DEBOUNCE;

        k_work_schedule(&pGpio->work_struct, K_NO_WAIT);
    }
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
            // Init delayed work struct (used for debounce)
            k_work_init_delayable(&ptr->work_struct, WorkCallback);

            gpioInputList.LinkAtBack(ptr);
        }
    }
    return ptr;
}

// UnlinkInputState - Currently Zephyr implementation
// doesn't release block as could cause fragmentation.
// TODO: Solve this.
void UnlinkInputState(gpio_input_state *pState)
{
    uint32_t regoffset = GPIO_OFFSET_IN_REG(pState->pinNumber);

    // If IRQ enabled
    if (pState->isrPtr != NULL)
    {
        // disable irq
        gpio_pin_interrupt_configure(GetGpioDev(pState->pinNumber), regoffset, GPIO_INT_DISABLE);
        gpio_remove_callback(GetGpioDev(pState->pinNumber), &pState->gpio_cb);
    }
    // This doens't matter if there isn't anying scheduled.
    k_work_cancel_delayable(&pState->work_struct);

    // Don't release the struct because of fragmentation. If
    // they want it once, they may want it again.
    // pState->Unlink();
    // platform_free(pState);

    // We want to clear the other flags
    pState->flags &= FLAG_INITIALIZED;
    pState->debounceMs = 0;
    pState->mode = GPIO_INT_NONE;
    pState->isrPtr = NULL;
    pState->param = NULL;
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

    // Get the gpio drivers
    int i;
    char sz[32];
    for (i = 0; i < GPIO_MAX_PIN / GPIO_PIN_PER_REG; i++)
    {
        sprintf(sz, "GPIO_%d", i);
        gpioDev[i] = device_get_binding(sz);
        if (gpioDev[i] == 0)
        {
            printk("**** Error. failed to get gpio device.\n");
            return false;
        }
        // printk ("device_get_binding success for %s\n", sz);
    }

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

    int port = GET_GPIO_PORT(pinNumber);
    uint32_t bit = GPIO_BIT_MASK(pinNumber);
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

    int port = GET_GPIO_PORT(pinNumber);
    uint32_t bit = GPIO_BIT_MASK(pinNumber);
    return ((pinReserved[port] & bit) != 0);
}

// Return maximum number of pins
int32_t CPU_GPIO_GetPinCount()
{
    return GPIO_MAX_PIN;
}

// Get current state of pin
GpioPinValue CPU_GPIO_GetPinState(GPIO_PIN pinNumber)
{
    return (GpioPinValue)GetPinVal(pinNumber);
}

// Set Pin state
void CPU_GPIO_SetPinState(GPIO_PIN pinNumber, GpioPinValue PinState)
{
    SetPinVal(pinNumber, PinState);
}

void CPU_GPIO_TogglePinState(GPIO_PIN pinNumber)
{
    printk("CPU_GPIO_TogglePinState++ %d\n", pinNumber);
    SetPinVal(pinNumber, GetPinVal(pinNumber) ? 0 : 1);
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
    int rc;

    // Check Input drive mode.
    if (driveMode >= (int)PinMode_Output)
    {
        return false;
    }

    // Set as Input GPIO_INT_EDGE intEdge, GPIO_RESISTOR ResistorState
    // This sets the input/output state as well as pull up/down.
    if (!CPU_GPIO_SetDriveMode(pinNumber, driveMode))
    {
        return false;
    }

    uint32_t bit = GPIO_BIT_MASK(pinNumber);
    uint32_t regoffset = GPIO_OFFSET_IN_REG(pinNumber);

    // Get or allocate new state structure
    pState = AllocateGpioInputState(pinNumber);
    // See if used before.
    if ((pState->flags & FLAG_INITIALIZED) == 0)
    {
        k_work_init_delayable(&pState->work_struct, WorkCallback);

        // Initialize callback even if we're not using an irq
        gpio_init_callback(&pState->gpio_cb, GpioIrqCallback, bit);
        rc = gpio_add_callback(GetGpioDev(pinNumber), &pState->gpio_cb);

        // Mark that we've init'ed the structure.
        pState->flags = FLAG_INITIALIZED;
    }

    // Link ISR ptr supplied and not already set up
    if (pinISR != NULL && (pState->isrPtr == NULL))
    {
        // there are callbacks registered and...
        // the drive mode is input so need to setup the interrupt

        // Configure input gpio.

        // disable irq
        rc = gpio_pin_interrupt_configure(GetGpioDev(pinNumber), regoffset, GPIO_INT_DISABLE);
        if (rc == 0)
        {
            uint32_t irq_config = 0;
            switch (intEdge)
            {
                case GPIO_INT_EDGE_LOW:
                    pState->flags &= ~FLAG_EXPECTED_HIGH;
                    irq_config = Z_GPIO_INT_FALLING;
                    break;

                case GPIO_INT_LEVEL_LOW:
                    pState->flags &= ~FLAG_EXPECTED_HIGH;
                    irq_config = Z_GPIO_INT_LEVEL_LOW;
                    break;

                case GPIO_INT_EDGE_HIGH:
                    irq_config = Z_GPIO_INT_RISING;
                    pState->flags |= FLAG_EXPECTED_HIGH;
                    break;
                case GPIO_INT_LEVEL_HIGH:
                    irq_config = Z_GPIO_INT_LEVEL_HIGH;
                    // pState->expected = 1;
                    pState->flags |= FLAG_EXPECTED_HIGH;
                    break;

                case GPIO_INT_EDGE_BOTH:
                    if (CPU_GPIO_GetPinState(pinNumber))
                    {
                        pState->flags &= ~FLAG_EXPECTED_HIGH;
                        irq_config = Z_GPIO_INT_FALLING;
                    }
                    else
                    {
                        pState->flags |= FLAG_EXPECTED_HIGH;
                        irq_config = Z_GPIO_INT_RISING;
                    }
                    break;

                default:
                    break;
            }
            // store parameters & configs
            pState->isrPtr = pinISR;
            pState->mode = intEdge;
            pState->param = (void *)isrParam;
            pState->debounceMs = debounceTimeMilliseconds;

            // Enable the interrupt
            rc = gpio_pin_interrupt_configure(GetGpioDev(pinNumber), regoffset, irq_config);
            if (rc)
                printk("gpio_pin_interrupt_configure failed %d\n", rc);
        }
        else
        {
            printk("gpio_pin_interrupt_configure failed %d\n", rc);
        }
        if (rc != 0)
        {
            printk("*****   Unable to configure gpio!\n");
        }
    }
    else if (pinISR == NULL && (pState->isrPtr != NULL))
    {
        // disable the EXT interrupt channel
        // it's OK to do always this, no matter if it's enabled or not
        rc = gpio_pin_interrupt_configure(GetGpioDev(pinNumber), regoffset, GPIO_INT_DISABLE);

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
// return       -   True if successful, false invalid pin, pin not putput,
// invalid drive mode for ouptput
//
bool CPU_GPIO_EnableOutputPin(GPIO_PIN pinNumber, GpioPinValue InitialState, PinMode driveMode)
{
    // check not an output drive mode
    if (driveMode < (int)PinMode_Output)
        return false;

    // If this is currently an input pin then clean up
    DeleteInputState(pinNumber);

    if (CPU_GPIO_SetDriveMode(pinNumber, driveMode) == false)
        return false;

    CPU_GPIO_SetPinState(pinNumber, InitialState);

    return true;
}

void CPU_GPIO_DisablePin(GPIO_PIN pinNumber, PinMode driveMode, uint32_t alternateFunction)
{
    DeleteInputState(pinNumber);

    GLOBAL_LOCK();

    // (db) Very strange to 'disable' a pin but put it into some random drive
    // mode.
    CPU_GPIO_SetDriveMode(pinNumber, driveMode);

    GLOBAL_UNLOCK();

    CPU_GPIO_ReservePin(pinNumber, false);
}

// Validate pin and set drive mode
// return true if ok
bool CPU_GPIO_SetDriveMode(GPIO_PIN pinNumber, PinMode driveMode)
{
    // Standard vals
    uint32_t regoffset = GPIO_OFFSET_IN_REG(pinNumber);
    uint32_t config = 0;

    switch (driveMode)
    {
        case PinMode_Input:
            config = GPIO_INPUT;
            break;

        case PinMode_InputPullDown:
            config = GPIO_INPUT | GPIO_PULL_DOWN;
            break;

        case PinMode_InputPullUp:
            config = GPIO_INPUT | GPIO_PULL_UP;
            break;

        case PinMode_Output:
            config = GPIO_OUTPUT;
            break;

        case PinMode_OutputOpenDrain:
            config = GPIO_OUTPUT | GPIO_LINE_OPEN_DRAIN;
            break;

        default:
            // all other modes are NOT supported
            return false;
    }
    int rc = gpio_pin_configure(GetGpioDev(pinNumber), regoffset, config);
    if (rc != 0)
    {
        printk("**** Error gpio_pin_configure %d\n", rc);
        return false;
    }
    return true;
}

bool CPU_GPIO_DriveModeSupported(GPIO_PIN pinNumber, PinMode driveMode)
{
    (void)pinNumber;

    bool driveModeSupported = false;

    // check if the requested drive mode is support by config
    if ((driveMode == PinMode_Input) || (driveMode == PinMode_InputPullDown) ||
        (driveMode == PinMode_InputPullUp) || (driveMode == PinMode_Output) ||
        (driveMode == PinMode_OutputOpenDrain))
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
