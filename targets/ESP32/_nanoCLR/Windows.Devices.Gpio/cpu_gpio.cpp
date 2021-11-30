//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

//  ESP32 GPIO ( 40 physical GPIO pads)
//
//  GPIO6-11 used for PSI flash
//
//  GPIO34-39 Only input mode
//

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Checks.h>
#include <targetHAL.h>
#include <Esp32_DeviceMapping.h>
#include <esp32_idf.h>

static const char *TAG = "cpu_Gpio";

#define ESP32_Gpio_MaxPins GPIO_PIN_COUNT // 0 -> 31,  32-39 (high)
#define TOTAL_GPIO_PORTS   ((ESP32_Gpio_MaxPins + 15) / 16)

TimerHandle_t oneshot;

// Double linkedlist to hold the state of each Input pin
struct gpio_input_state : public HAL_DblLinkedNode<gpio_input_state>
{
    GPIO_PIN pinNumber;                    // Pin number
    TimerHandle_t debounceTimer;           // debounce timer for this Pin
    GPIO_INTERRUPT_SERVICE_ROUTINE isrPtr; // Ptr to user ISR or null
    uint32_t debounceMs;                   // debounce Millsecs, no debonce=0
    uint8_t mode;                          // Interrupt mode
    void *param;                           // Param to user isr call
    bool expected;                         // Expected state for debounce handler
    bool waitingDebounce;                  // True if waiting for debounce timer to complete
};

static HAL_DblLinkedList<gpio_input_state> gpioInputList; // Doulble LInkedlist for GPIO input status
static uint16_t pinReserved[TOTAL_GPIO_PORTS];            //  reserved - 1 bit per pin

// memory for pin state
static uint16_t pinStateStore[TOTAL_GPIO_PORTS];

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
        memset(ptr, 0, sizeof(gpio_input_state));
        ptr->pinNumber = pinNumber;
        gpioInputList.LinkAtBack(ptr);
    }

    return ptr;
}

void UnlinkInputState(gpio_input_state *state)
{
    if (state->debounceTimer != NULL)
    {
        xTimerDelete(state->debounceTimer, 100);
    }

    // Remove interrupt associated with pin
    gpio_isr_handler_remove((gpio_num_t)state->pinNumber);

    state->Unlink();
    platform_free(state);
}

// Delete gpio_input_state from List and tidy up ( Timer & ISR handler )
void DeleteInputState(GPIO_PIN pinNumber)
{
    gpio_input_state *state = GetInputState(pinNumber);

    if (state)
    {
        UnlinkInputState(state);
    }
}

//
// Debounce Handler, called when timer is complete
//
static void Esp_Gpio_DebounceHandler(TimerHandle_t timer)
{
    gpio_input_state *state = (gpio_input_state *)pvTimerGetTimerID(timer);

    // sanity check
    if (state != NULL)
    {
        // get current pin state
        bool actual = CPU_GPIO_GetPinState(state->pinNumber);

        if (actual == state->expected)
        {
            state->isrPtr(state->pinNumber, actual, state->param);

            if (state->mode == GPIO_INT_EDGE_BOTH)
            {
                // both edges
                // update expected state
                state->expected ^= 1;
            }
        }

        // reset flag
        state->waitingDebounce = false;
    }
    else
    {
        // should never happen!!!
    }
}

bool CPU_GPIO_Initialize()
{
    // Initialise Double linked list for input pin states
    gpioInputList.Initialize();

    // Make sure all pins are not reserved
    memset(pinReserved, 0, sizeof(pinReserved));

    // check if PSRAM it's available (querying largets free block available with SPIRAM capabilities)
    if (heap_caps_get_largest_free_block(MALLOC_CAP_8BIT | MALLOC_CAP_32BIT | MALLOC_CAP_SPIRAM))
    {
        // Reserve Pins 6-11 as used by Spi flash
        for (int pinNumber = 6; pinNumber <= 11; pinNumber++)
        {
            CPU_GPIO_ReservePin(pinNumber, true);
        }
    }

    // Install ISR service for GPIO
    esp_err_t ret = gpio_install_isr_service(0);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Install isr service");
        return false;
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

    gpio_uninstall_isr_service();

    return true;
}

// Set/reset reserved state of pin
bool CPU_GPIO_ReservePin(GPIO_PIN pinNumber, bool fReserve)
{
    // Check if valid pin number
    if (!GPIO_IS_VALID_GPIO((gpio_num_t)pinNumber))
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
    return ESP32_Gpio_MaxPins;
}

// Get current state of pin
GpioPinValue CPU_GPIO_GetPinState(GPIO_PIN pin)
{
    return (GpioPinValue)gpio_get_level((gpio_num_t)pin);
}

// Set Pin state
void CPU_GPIO_SetPinState(GPIO_PIN pinNumber, GpioPinValue pinState)
{
    // need to store pin state
    int port = pinNumber >> 4, bit = 1 << (pinNumber & 0x0F);

    if (pinState == GpioPinValue_High)
    {
        pinStateStore[port] |= bit;
    }
    else
    {
        pinStateStore[port] &= ~bit;
    }

    gpio_set_level((gpio_num_t)pinNumber, (uint32_t)pinState);
}

// Toggle pin state
void CPU_GPIO_TogglePinState(GPIO_PIN pinNumber)
{
    // platform DOES NOT support toggle
    // need to do it "the hard way"

    uint32_t newValue;

    // need to store pin state
    int port = pinNumber >> 4, bit = 1 << (pinNumber & 0x0F);

    if (pinStateStore[port] & bit)
    {
        pinStateStore[port] &= ~bit;
        newValue = 0;
    }
    else
    {
        pinStateStore[port] |= bit;
        newValue = 1;
    }

    gpio_set_level((gpio_num_t)pinNumber, newValue);
}

// ISR called by IDF
static void gpio_isr(void *arg)
{
    NATIVE_INTERRUPT_START

    gpio_input_state *state = (gpio_input_state *)arg;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // Ignore any pin changes if waiting for debounce timeout to occur
    if (state->waitingDebounce)
    {
        return;
    }

    // get current pin state
    bool actual = CPU_GPIO_GetPinState(state->pinNumber);

    if (state->debounceMs > 0)
    {
        // flag waiting for debounce timeout
        state->waitingDebounce = true;

        // store current state
        state->expected = actual;

        // start timer
        xTimerStartFromISR(state->debounceTimer, &xHigherPriorityTaskWoken);

        if (xHigherPriorityTaskWoken != pdFAIL)
        {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }
    else
    {
        // just fire event
        state->isrPtr(state->pinNumber, actual, state->param);
    }

    NATIVE_INTERRUPT_END
}

bool CPU_GPIO_EnableInputPin(
    GPIO_PIN pinNumber,
    uint32_t debounceTimeMilliseconds,
    GPIO_INTERRUPT_SERVICE_ROUTINE pinISR,
    void *isr_Param,
    GPIO_INT_EDGE intEdge,
    GpioPinDriveMode driveMode)
{
    esp_err_t ret;
    gpio_input_state *state;

    // Check Input drive mode
    if (driveMode >= (int)GpioPinDriveMode_Output)
        return false;

    // Set as Input GPIO_INT_EDGE intEdge, GPIO_RESISTOR ResistorState
    if (!CPU_GPIO_SetDriveMode(pinNumber, driveMode))
    {
        return false;
    }

    state = AllocateGpioInputState(pinNumber);

    // Link ISR ptr supplied and not already set up
    // CPU_GPIO_EnableInputPin could be called a 2nd time with changed parameters
    if (pinISR != NULL && (state->isrPtr == NULL))
    {
        ret = gpio_isr_handler_add((gpio_num_t)pinNumber, gpio_isr, (void *)state);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Add interrupt to gpio pin failed");
            return false;
        }

        // setup debounce for pin
        // don't care about the return value
        CPU_GPIO_SetPinDebounce(pinNumber, debounceTimeMilliseconds);

        // Map Interrupt edge to ESP32 edge
        // NONE=0, EDGE_LOW=1, EDGE_HIGH=2, EDGE_BOTH=3, LEVEL_HIGH=4, LEVEL_LOW
        uint8_t mapint[6] = {
            GPIO_INTR_DISABLE,
            GPIO_INTR_NEGEDGE,
            GPIO_INTR_POSEDGE,
            GPIO_INTR_ANYEDGE,
            GPIO_INTR_HIGH_LEVEL,
            GPIO_INTR_LOW_LEVEL};
        gpio_set_intr_type((gpio_num_t)pinNumber, (gpio_int_type_t)mapint[intEdge]);

        // store parameters & configs
        state->isrPtr = pinISR;
        state->mode = intEdge;
        state->param = (void *)isr_Param;
        state->debounceMs = debounceTimeMilliseconds;

        switch (intEdge)
        {
            case GPIO_INT_EDGE_LOW:
            case GPIO_INT_LEVEL_LOW:
                state->expected = false;
                break;

            case GPIO_INT_EDGE_HIGH:
            case GPIO_INT_LEVEL_HIGH:
                state->expected = true;
                break;

            case GPIO_INT_EDGE_BOTH:
                state->expected = !CPU_GPIO_GetPinState(pinNumber); // Use not current state
                break;

            default:
                break;
        }
    }
    else if (pinISR == NULL && (state->isrPtr != NULL))
    {
        // there is no managed handler setup anymore
        // remove INT handler

        // remove callback
        gpio_isr_handler_remove((gpio_num_t)state->pinNumber);

        // clear parameters & configs
        state->isrPtr = NULL;
        state->mode = GPIO_INT_NONE;
        state->param = NULL;
        state->debounceMs = 0;
    }

    return true;
}

// Enable an output pin
//
// pinNumber    -   Gpio pin number
// InitialState -   Initial state of pin
// driveMode    -   Drive mode and resistors
// return       -   True if successful, false invalid pin, pin not putput, invalid drive mode for ouptput
//
bool CPU_GPIO_EnableOutputPin(GPIO_PIN pinNumber, GpioPinValue InitialState, GpioPinDriveMode driveMode)
{
    // check not an output drive mode
    if (driveMode < (int)GpioPinDriveMode_Output)
        return false;

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
    GLOBAL_LOCK();

    DeleteInputState(pinNumber);

    CPU_GPIO_SetDriveMode(pinNumber, driveMode);

    if (alternateFunction != 0)
    {
        Esp32_SetMappedDevicePins((uint8_t)pinNumber, alternateFunction);
    }

    GLOBAL_UNLOCK();

    CPU_GPIO_ReservePin(pinNumber, false);
}

// Validate pin and set drive mode
// return true if ok
bool CPU_GPIO_SetDriveMode(GPIO_PIN pinNumber, GpioPinDriveMode driveMode)
{
    // Valid Pin
    if (!GPIO_IS_VALID_GPIO(pinNumber))
    {
        return false;
    }

    // Check Pin is output capable
    if (driveMode >= (int)GpioPinDriveMode_Output && !GPIO_IS_VALID_OUTPUT_GPIO(pinNumber))
    {
        return false;
    }

    gpio_mode_t mode = GPIO_MODE_DISABLE;
    gpio_pullup_t pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_pulldown_t pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_int_type_t intr_type = GPIO_INTR_ANYEDGE;

    switch (driveMode)
    {
        case GpioPinDriveMode_Input:
            mode = GPIO_MODE_INPUT;
            break;
        case GpioPinDriveMode_InputPullDown:
            mode = GPIO_MODE_INPUT;
            pull_down_en = GPIO_PULLDOWN_ENABLE;
            break;
        case GpioPinDriveMode_InputPullUp:
            mode = GPIO_MODE_INPUT;
            pull_up_en = GPIO_PULLUP_ENABLE;
            break;
        case GpioPinDriveMode_Output:
            mode = GPIO_MODE_OUTPUT;
            break;
        case GpioPinDriveMode_OutputOpenDrain:
            mode = GPIO_MODE_OUTPUT_OD;
            break;
        case GpioPinDriveMode_OutputOpenDrainPullUp:
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

    gpio_config(&GPIOConfig);

    return true;
}

bool CPU_GPIO_DriveModeSupported(GPIO_PIN pinNumber, GpioPinDriveMode driveMode)
{
    if (!GPIO_IS_VALID_GPIO(pinNumber))
        return false;

    // Input & Output pins use any valid drivemode.
    // Note: all output pins are also input pins
    if (GPIO_IS_VALID_OUTPUT_GPIO(pinNumber))
    {
        return (driveMode <= GpioPinDriveMode_OutputOpenSourcePullDown);
    }

    // Input only pins only input drive modes
    return (driveMode <= GpioPinDriveMode_InputPullUp);
}

uint32_t CPU_GPIO_GetPinDebounce(GPIO_PIN pinNumber)
{
    if (!GPIO_IS_VALID_GPIO(pinNumber))
    {
        return 0;
    }

    gpio_input_state *state = GetInputState(pinNumber);
    if (state)
    {
        return state->debounceMs;
    }

    return 0;
}

bool CPU_GPIO_SetPinDebounce(GPIO_PIN pinNumber, uint32_t debounceTimeMilliseconds)
{
    if (!GPIO_IS_VALID_GPIO(pinNumber))
    {
        return false;
    }

    gpio_input_state *state = GetInputState(pinNumber);

    if (state)
    {
        if (state->debounceMs > 0 && debounceTimeMilliseconds)
        {
            // removing debounce, nothing to do here
        }
        else if (debounceTimeMilliseconds > 0)
        {
            // compute ticks for debounce timer
            int ticks = debounceTimeMilliseconds / portTICK_PERIOD_MS;

            if (ticks == 0)
            {
                // (minimum 1 freeRTOS tick(10ms) )
                ticks = 1;
            }

            // check if there was a timer already setup
            if (state->debounceTimer == NULL)
            {
                // Create timer because it doesn't exist for this pin
                state->debounceTimer =
                    xTimerCreate("debounce", ticks, pdFALSE, (void *)state, Esp_Gpio_DebounceHandler);
            }
            else
            {
                // timer exists, update the timer period with the new value
                // no need to wait for the timer operation to execute
                xTimerChangePeriod(state->debounceTimer, ticks, 0);
            }
        }

        // store new value for debounce time
        state->debounceMs = debounceTimeMilliseconds;

        return true;
    }

    return false;
}
