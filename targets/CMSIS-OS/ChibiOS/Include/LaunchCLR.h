//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _LAUNCHCLR_H_
#define _LAUNCHCLR_H_

void LaunchCLR(uint32_t address);
bool CheckValidCLRImage(uint32_t address);

#ifndef GPIO_PORT

static stm32_gpio_t* gpioPort[] = { GPIOA, GPIOB
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

#define GPIO_PORT(pin) (gpioPort[pin/16])

#endif  // GPIO_PORT


#endif //_LAUNCHCLR_H_

