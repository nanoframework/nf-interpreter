//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_hardware_stm32_native.h"
#include <targetPAL.h>

#if defined(HAL_USE_ADC) && (HAL_USE_ADC == TRUE)
#include <sys_dev_adc_native_target.h>

NF_PAL_ADC_PORT_PIN_CHANNEL *RuntimeAdcPortPinConfig = nullptr;
uint8_t RuntimeAdcChannelCount = 0;

int8_t GetNextFreeAdcChannel(NF_PAL_ADC_PORT_PIN_CHANNEL *&newChannel);

#endif

typedef Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_GpioConfiguration GpioConfiguration;

HRESULT Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_Configuration::
    ConfigurePin___STATIC__VOID__I4__nanoFrameworkHardwareStm32GpioConfiguration(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    GpioConfiguration_IOMode mode;
    GpioConfiguration_PullUpDownActivation pullUpDown;
    GpioConfiguration_Speed speed;
    uint8_t alternateFunction;
    uint8_t pinNumber;
    stm32_gpio_t *port;
    int16_t pad;

    CLR_RT_HeapBlock *gpioConfiguration;

    pinNumber = (uint8_t)stack.Arg0().NumericByRef().s4;

    // decode pin number to get a ChibiOS PAL IoLine from our "encoded" pin number
    port = GPIO_PORT(pinNumber);
    pad = pinNumber % 16;

    // get a pointer to the managed GPIO configuration object instance
    gpioConfiguration = stack.Arg1().Dereference();

    // grab all the fields from the managed object
    mode = (GpioConfiguration_IOMode)gpioConfiguration[GpioConfiguration::FIELD___mode].NumericByRef().s4;
    pullUpDown = (GpioConfiguration_PullUpDownActivation)gpioConfiguration[GpioConfiguration::FIELD___pullUpDown]
                     .NumericByRef()
                     .s4;
    // ChibiOS encoding differs from STM32 HAL encoding
    pullUpDown = (GpioConfiguration_PullUpDownActivation)(pullUpDown << 5);

    speed = (GpioConfiguration_Speed)gpioConfiguration[GpioConfiguration::FIELD___speed].NumericByRef().s4;
    // ChibiOS encoding differs from STM32 HAL encoding
    speed = (GpioConfiguration_Speed)(speed << 3);

    alternateFunction = (uint8_t)gpioConfiguration[GpioConfiguration::FIELD___alternateFunction].NumericByRef().s4;

    // configure the pin
    palSetPadMode(port, pad, mode | pullUpDown | speed | (PAL_MODE_ALTERNATE(alternateFunction)));

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_Configuration::
    AddAdcChannel___STATIC__U4__U4__U4__U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(HAL_USE_ADC) && (HAL_USE_ADC == TRUE)

    uint8_t adc;
    uint8_t adcChannel;
    uint8_t pinNumber;
    int8_t newChannelIndex;
    NF_PAL_ADC_PORT_PIN_CHANNEL *newChannel;

    // grab arguments
    adc = (uint8_t)stack.Arg0().NumericByRef().u4;
    adcChannel = (uint8_t)stack.Arg1().NumericByRef().u4;
    pinNumber = (uint8_t)stack.Arg2().NumericByRef().u4;

    // validate arguments
    switch (adc)
    {
#if STM32_ADC_USE_ADC1
        case 1:
            // all good
            break;
#endif

#if STM32_ADC_USE_ADC2
        case 2:
            // all good
            break;
#endif

#if STM32_ADC_USE_ADC3
        case 3:
            // all good
            break;
#endif

        default:
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    if (adcChannel > 15)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // add channel to the list
    newChannelIndex = GetNextFreeAdcChannel(newChannel);

    // sanity check
    if (newChannelIndex < 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    // copy channel definition
    newChannel->adcIndex = adc;
    newChannel->adcChannel = adcChannel;
    newChannel->portId = GPIO_PORT(pinNumber);
    newChannel->pin = pinNumber % 16;

    // need to adjust channel index to account for the ones declared in the build
    newChannelIndex += AdcChannelCount;

    // return the index of the channel
    stack.SetResult_U4(newChannelIndex);

#else
    (void)stack;
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_Configuration::
    RemoveAdcChannel___STATIC__VOID__U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(HAL_USE_ADC) && (HAL_USE_ADC == TRUE)
    uint8_t channel;

    // grab arguments
    channel = (uint8_t)stack.Arg0().NumericByRef().u4;

    // need to adjust channel index to account for the ones declared in the build
    channel -= AdcChannelCount;

    // check if this channel definition is valid
    if (channel >= RuntimeAdcChannelCount)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // cler the channel definition
    memset(&RuntimeAdcPortPinConfig[channel], 0, sizeof(NF_PAL_ADC_PORT_PIN_CHANNEL));

#else
    (void)stack;
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif

    NANOCLR_NOCLEANUP();
}

#if defined(HAL_USE_ADC) && (HAL_USE_ADC == TRUE)

/// @brief Creates a new ADC channel definition and adds it to the list of channels
/// @param newChannel Pointer to the new channel definition
/// @return Index of the new channel definition. -1 if no more channels can be added.
int8_t GetNextFreeAdcChannel(NF_PAL_ADC_PORT_PIN_CHANNEL *&newChannel)
{
    int8_t channelIndex = -1;
    newChannel = nullptr;

    if (RuntimeAdcPortPinConfig == nullptr)
    {
        RuntimeAdcPortPinConfig = (NF_PAL_ADC_PORT_PIN_CHANNEL *)platform_malloc(sizeof(NF_PAL_ADC_PORT_PIN_CHANNEL));

        // sanity check
        if (RuntimeAdcPortPinConfig != nullptr)
        {
            // clear memory
            memset(RuntimeAdcPortPinConfig, 0, sizeof(NF_PAL_ADC_PORT_PIN_CHANNEL));

            // update the number of channels
            RuntimeAdcChannelCount = 1;
        }
    }

    // find the first free channel
    for (int8_t i = 0; i < RuntimeAdcChannelCount; i++)
    {
        if (RuntimeAdcPortPinConfig[i].adcIndex == 0)
        {
            // found a free channel
            channelIndex = i;
            newChannel = &RuntimeAdcPortPinConfig[i];
        }
    }

    if (channelIndex == -1)
    {
        // need to reallocate memory

        // allocate new memory block
        NF_PAL_ADC_PORT_PIN_CHANNEL *newRuntimeAdcPortPinConfig = (NF_PAL_ADC_PORT_PIN_CHANNEL *)platform_malloc(
            (RuntimeAdcChannelCount + 1) * sizeof(NF_PAL_ADC_PORT_PIN_CHANNEL));

        // sanity check
        if (RuntimeAdcPortPinConfig != nullptr)
        {
            // update the number of channels
            RuntimeAdcChannelCount++;

            // copy the old channels to the new memory block
            memcpy(
                newRuntimeAdcPortPinConfig,
                RuntimeAdcPortPinConfig,
                RuntimeAdcChannelCount * sizeof(NF_PAL_ADC_PORT_PIN_CHANNEL));

            // free the old memory block
            platform_free(RuntimeAdcPortPinConfig);

            // set the new memory block
            RuntimeAdcPortPinConfig = newRuntimeAdcPortPinConfig;

            // clear the new channel
            memset(&RuntimeAdcPortPinConfig[RuntimeAdcChannelCount - 1], 0, sizeof(NF_PAL_ADC_PORT_PIN_CHANNEL));

            // set pointer to the new channel
            newChannel = &RuntimeAdcPortPinConfig[RuntimeAdcChannelCount - 1];

            // set the index of the new channel
            channelIndex = RuntimeAdcChannelCount - 1;
        }
    }

    return channelIndex;
}

#endif
