//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _HALCONF_NF_H_
#define _HALCONF_NF_H_

// enables STM32 Flash driver
#if !defined(HAL_USE_STM32_FLASH) 
#define HAL_USE_STM32_FLASH         TRUE
#endif

// enables STM32 Can driver
#if !defined(HAL_USE_STM32_CAN) 
#define HAL_USE_STM32_CAN       TRUE
#endif

#endif // _HALCONF_NF_H_
