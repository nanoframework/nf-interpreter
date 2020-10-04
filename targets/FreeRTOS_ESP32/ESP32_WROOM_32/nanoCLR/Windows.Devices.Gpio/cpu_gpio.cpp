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

#include <targetHAL.h>
#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Checks.h>

#include "Esp32_DeviceMapping.h"

static const char *TAG = "cpu_Gpio";

#define ESP32_Gpio_MaxPins GPIO_PIN_COUNT // 0 -> 31,  32-39 (high)
#define TOTAL_GPIO_PORTS   ((ESP32_Gpio_MaxPins + 15) / 16)

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
        memset(ptr, 0, sizeof(gpio_input_state));
        ptr->pinNumber = pinNumber;
        gpioInputList.LinkAtBack(ptr);
    }
    return ptr;
}

void UnlinkInputState(gpio_input_state *pState)
{
    if (pState->debounceTimer != 0)
    {
        xTimerDelete(pState->debounceTimer, 100);
    }

    // Remove interrupt associatted with pin
    gpio_isr_handler_remove((gpio_num_t)pState->pinNumber);

    pState->Unlink();
    platform_free(pState);
}

// Delete gpio_input_state from List and tidy up ( Timer & ISR handler )
void DeleteInputState(GPIO_PIN pinNumber)
{
    gpio_input_state *pState = GetInputState(pinNumber);
    if (pState)
        UnlinkInputState(pState);
}

void Esp_Gpio_fire_event(gpio_input_state *pState)
{
    bool actual = CPU_GPIO_GetPinState(pState->pinNumber); // get current pin state
    if (actual == pState->expected)
    {
        pState->isrPtr(pState->pinNumber, actual);

        if (pState->mode == GPIO_INT_EDGE_BOTH)
        {
            // both edges
            pState->expected ^= 1; // update expected state
        }
    }
}

//
// Debounce Handler, called when timer is complete
//
void Esp_Gpio_DebounceHandler(TimerHandle_t xTimer)
{
    gpio_input_state *pState = (gpio_input_state *)pvTimerGetTimerID(xTimer);
    if (pState->isrPtr)
    {
        Esp_Gpio_fire_event(pState);
    }

    pState->waitingDebounce = false;
}

bool CPU_GPIO_Initialize()
{
    // Initialise Double linked list for input pin states
    gpioInputList.Initialize();

    // Make sure all pins are not reserved
    memset(pinReserved, 0, sizeof(pinReserved));

    // Reserve Pins 6-11 as used by Spi flash
    for (int pinNumber = 6; pinNumber <= 11; pinNumber++)
        CPU_GPIO_ReservePin(pinNumber, true);

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
    gpio_input_state *pGpio;

    pGpio = gpioInputList.FirstNode();

    // Clean up input state list
    while (pGpio->Next() != NULL)
    {
        UnlinkInputState(pGpio);
        pGpio = pGpio->Next();
    }

    gpio_uninstall_isr_service();
    return true;
}

// Set/reset reserved state of pin
bool CPU_GPIO_ReservePin(GPIO_PIN pinNumber, bool fReserve)
{
    // Check if valid pin number
    if (!GPIO_IS_VALID_GPIO((gpio_num_t)pinNumber))
        return false;

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
void CPU_GPIO_SetPinState(GPIO_PIN pin, GpioPinValue PinState)
{
    gpio_set_level((gpio_num_t)pin, (uint32_t)PinState);
}

// ISR called by IDF
static void gpio_isr(void *arg)
{
    NATIVE_INTERRUPT_START

    gpio_input_state *pState = (gpio_input_state *)arg;
    ;

    // Ignore any pin changes during debounce
    if (pState->waitingDebounce)
        return;

    if (pState->debounceMs > 0)
    {
        pState->waitingDebounce = true;

        if (pState->debounceTimer == 0)
        {
            // Create timer if it doesn't already exist for this pin
            pState->debounceTimer = xTimerCreate("debounce", 100, pdFALSE, (void *)pState, Esp_Gpio_DebounceHandler);
        }

        // Start Debounce timer (minimum 1 freeRtos tick(10ms) )
        int ticks = pdMS_TO_TICKS(pState->debounceMs);
        if (ticks == 0)
            ticks = 1;
        xTimerChangePeriodFromISR(pState->debounceTimer, ticks, pdFALSE);
    }
    else
    {
        Esp_Gpio_fire_event(pState);
    }

    NATIVE_INTERRUPT_END
}

bool CPU_GPIO_EnableInputPin(
    GPIO_PIN pinNumber,
    CLR_UINT64 debounceTimeMilliseconds,
    GPIO_INTERRUPT_SERVICE_ROUTINE pinISR,
    void *isr_Param,
    GPIO_INT_EDGE intEdge,
    GpioPinDriveMode driveMode)
{
    esp_err_t ret;
    gpio_input_state *pState;

    // Check Input drive mode
    if (driveMode >= (int)GpioPinDriveMode_Output)
        return false;

    // Set as Input GPIO_INT_EDGE intEdge, GPIO_RESISTOR ResistorState
    if (!CPU_GPIO_SetDriveMode(pinNumber, driveMode))
        return false;

    pState = AllocateGpioInputState(pinNumber);

    // Link ISR ptr supplied and not already set up
    // CPU_GPIO_EnableInputPin could be called a 2nd time with changed parameters
    if (pinISR != NULL && (pState->isrPtr == NULL))
    {
        ret = gpio_isr_handler_add((gpio_num_t)pinNumber, gpio_isr, (void *)pState);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Add interrupt to gpio pin failed");
            return false;
        }

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
        pState->isrPtr = pinISR;
        pState->mode = intEdge;
        pState->param = (void *)isr_Param;
        pState->debounceMs = (uint32_t)(debounceTimeMilliseconds);

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
                pState->expected = !CPU_GPIO_GetPinState(pinNumber); // Use not current state
                break;

            default:
                break;
        }
    }
    else if (pinISR == NULL && (pState->isrPtr != NULL))
    {
        // there is no managed handler setup anymore
        // remove INT handler

        // remove callback
        gpio_isr_handler_remove((gpio_num_t)pState->pinNumber);

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
        return false;

    // If this is currently an input pin then clean up
    DeleteInputState(pinNumber);

    if (CPU_GPIO_SetDriveMode(pinNumber, driveMode) == false)
        return false;

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
        return 0;

    gpio_input_state *ptr = GetInputState(pinNumber);
    if (ptr)
        return ptr->debounceMs;

    return 0;
}

bool CPU_GPIO_SetPinDebounce(GPIO_PIN pinNumber, CLR_UINT64 debounceTimeMilliseconds)
{
    if (!GPIO_IS_VALID_GPIO(pinNumber))
        return false;

    gpio_input_state *ptr = GetInputState(pinNumber);
    if (ptr)
    {
        ptr->debounceMs = (uint32_t)(debounceTimeMilliseconds);
        return true;
    }
    return false;
}
