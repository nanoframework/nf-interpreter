//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <stm32l4xx_hal.h>

GPIO_TypeDef* GpioPorts[] = { 
#ifdef GPIOA
GPIOA,
#endif
#ifdef GPIOB
GPIOB,
#endif
#ifdef GPIOC
GPIOC,
#endif
#ifdef GPIOD
GPIOD,
#endif
#ifdef GPIOE
GPIOE,
#endif
#ifdef GPIOF
GPIOF,
#endif
#ifdef GPIOG
GPIOG,
#endif
#ifdef GPIOH
GPIOH,
#endif
};
