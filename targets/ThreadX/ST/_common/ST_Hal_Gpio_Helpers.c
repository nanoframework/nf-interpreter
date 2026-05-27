//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <ST_Hal_Gpio_Helpers.h>

void HAL_GPIO_GetConfig(GPIO_TypeDef *gpiox, GPIO_InitTypeDef *gpioInit)
{
    uint32_t temp;
    uint32_t position = 0;

    if (gpioInit->Pin > 0)
    {
        position = (1uL << gpioInit->Pin);
    }

    // speed
    temp = gpiox->OSPEEDR;
    temp &= (GPIO_OSPEEDR_OSPEED0 << (position * 2u));
    gpioInit->Speed = (temp >> (position * 2u));

    // alternate
    temp = gpiox->AFR[position >> 3u];
    temp &= (0xFu << ((position & 0x07u) * 4u));
    gpioInit->Alternate = (temp >> ((position & 0x07u) * 4u));

    // pull-up/down
    temp = gpiox->PUPDR;
    temp &= (GPIO_PUPDR_PUPD0 << (position * 2u));
    gpioInit->Pull = (temp >> (position * 2u));

    // mode
    temp = gpiox->MODER;
    temp &= (GPIO_MODER_MODE0 << (position * 2u));
    gpioInit->Mode = ((temp & GPIO_MODE) >> (position * 2u));

#if defined(STM32L471xx) || defined(STM32L475xx) || defined(STM32L476xx) || defined(STM32L485xx) || defined(STM32L486xx)

    // check analog mode
    temp = gpiox->ASCR;
    temp &= (GPIO_ASCR_ASC0 << position);
    gpioInit->Mode |= (((temp & ANALOG_MODE) >> 3) >> position);

#endif // STM32L471xx || STM32L475xx || STM32L476xx || STM32L485xx || STM32L486xx

    // I/O direction
    temp = gpiox->MODER;
    temp &= (GPIO_MODER_MODE0 << (position * 2u));
    gpioInit->Mode |= ((temp & GPIO_MODE) >> (position * 2u));
}

void HAL_GPIO_SetConfig(GPIO_TypeDef *gpiox, GPIO_InitTypeDef *gpioInit)
{
    uint32_t temp;

    uint32_t position = 0;

    if (gpioInit->Pin > 0)
    {
        position = (1uL << gpioInit->Pin);
    }

    //--------------------- GPIO Mode Configuration ------------------------
    // In case of Output or Alternate function mode selection
    if ((gpioInit->Mode == GPIO_MODE_OUTPUT_PP) || (gpioInit->Mode == GPIO_MODE_AF_PP) ||
        (gpioInit->Mode == GPIO_MODE_OUTPUT_OD) || (gpioInit->Mode == GPIO_MODE_AF_OD))
    {
        // Check the Speed parameter
        assert_param(IS_GPIO_SPEED(gpioInit->Speed));
        // Configure the IO Speed
        temp = gpiox->OSPEEDR;
        temp &= ~(GPIO_OSPEEDR_OSPEED0 << (position * 2u));
        temp |= (gpioInit->Speed << (position * 2u));
        gpiox->OSPEEDR = temp;

        // Configure the IO Output Type
        temp = gpiox->OTYPER;
        temp &= ~(GPIO_OTYPER_OT0 << position);
        temp |= (((gpioInit->Mode & GPIO_OUTPUT_TYPE) >> 4u) << position);
        gpiox->OTYPER = temp;
    }

#if defined(STM32L471xx) || defined(STM32L475xx) || defined(STM32L476xx) || defined(STM32L485xx) || defined(STM32L486xx)

    // In case of Analog mode, check if ADC control mode is selected
    if ((gpioInit->Mode & GPIO_MODE_ANALOG) == GPIO_MODE_ANALOG)
    {
        // Configure the IO Output Type
        temp = gpiox->ASCR;
        temp &= ~(GPIO_ASCR_ASC0 << position);
        temp |= (((gpioInit->Mode & ANALOG_MODE) >> 3) << position);
        gpiox->ASCR = temp;
    }

#endif // STM32L471xx || STM32L475xx || STM32L476xx || STM32L485xx || STM32L486xx

    // Activate the Pull-up or Pull down resistor for the current IO
    temp = gpiox->PUPDR;
    temp &= ~(GPIO_PUPDR_PUPD0 << (position * 2u));
    temp |= ((gpioInit->Pull) << (position * 2u));
    gpiox->PUPDR = temp;

    // In case of Alternate function mode selection
    if ((gpioInit->Mode == GPIO_MODE_AF_PP) || (gpioInit->Mode == GPIO_MODE_AF_OD))
    {
        // Check the Alternate function parameters
        assert_param(IS_GPIO_AF_INSTANCE(gpiox));
        assert_param(IS_GPIO_AF(gpioInit->Alternate));

        // Configure Alternate function mapped with the current IO
        temp = gpiox->AFR[position >> 3u];
        temp &= ~(0xFu << ((position & 0x07u) * 4u));
        temp |= ((gpioInit->Alternate) << ((position & 0x07u) * 4u));
        gpiox->AFR[position >> 3u] = temp;
    }

    // Configure IO Direction mode (Input, Output, Alternate or Analog)
    temp = gpiox->MODER;
    temp &= ~(GPIO_MODER_MODE0 << (position * 2u));
    temp |= ((gpioInit->Mode & GPIO_MODE) << (position * 2u));
    gpiox->MODER = temp;

    //--------------------- EXTI Mode Configuration ------------------------
    // Configure the External Interrupt or event for the current IO
    if ((gpioInit->Mode & EXTI_MODE) == EXTI_MODE)
    {
        // Enable SYSCFG Clock
        __HAL_RCC_SYSCFG_CLK_ENABLE();

        temp = SYSCFG->EXTICR[position >> 2u];
        temp &= ~(0x0FuL << (4u * (position & 0x03u)));
        temp |= (GPIO_GET_INDEX(gpiox) << (4u * (position & 0x03u)));
        SYSCFG->EXTICR[position >> 2u] = temp;

        // Clear EXTI line configuration
        temp = EXTI->IMR1;
        temp &= ~(position);
        if ((gpioInit->Mode & GPIO_MODE_IT) == GPIO_MODE_IT)
        {
            temp |= position;
        }
        EXTI->IMR1 = temp;

        temp = EXTI->EMR1;
        temp &= ~(position);
        if ((gpioInit->Mode & GPIO_MODE_EVT) == GPIO_MODE_EVT)
        {
            temp |= position;
        }
        EXTI->EMR1 = temp;

        // Clear Rising Falling edge configuration
        temp = EXTI->RTSR1;
        temp &= ~(position);
        if ((gpioInit->Mode & RISING_EDGE) == RISING_EDGE)
        {
            temp |= position;
        }
        EXTI->RTSR1 = temp;

        temp = EXTI->FTSR1;
        temp &= ~(position);
        if ((gpioInit->Mode & FALLING_EDGE) == FALLING_EDGE)
        {
            temp |= position;
        }
        EXTI->FTSR1 = temp;
    }
}
