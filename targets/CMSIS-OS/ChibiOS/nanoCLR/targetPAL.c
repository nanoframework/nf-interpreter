//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <hal.h>

#if defined(STM32L0XX) || defined(STM32F0XX) || defined(STM32F4XX) || defined(STM32F7XX) || defined(STM32H7XX)

stm32_gpio_t* gpioPort[] = { GPIOA, GPIOB
#if STM32_HAS_GPIOC
, GPIOC
#endif
#if STM32_HAS_GPIOD
, GPIOD
#endif
#if STM32_HAS_GPIOE
, GPIOE
#endif
#if STM32_HAS_GPIOF
, GPIOF
#endif
#if STM32_HAS_GPIOG
, GPIOG
#endif
#if STM32_HAS_GPIOH
, GPIOH
#endif
#if STM32_HAS_GPIOI
, GPIOI
#endif
#if STM32_HAS_GPIOJ
, GPIOJ
#endif
#if STM32_HAS_GPIOK
, GPIOK
#endif
};

#endif
