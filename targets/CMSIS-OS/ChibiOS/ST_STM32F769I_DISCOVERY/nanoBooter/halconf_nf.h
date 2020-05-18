//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _HALCONF_NF_H_
#define _HALCONF_NF_H_

// enables STM32 Flash driver
#if !defined(HAL_NF_USE_STM32_FLASH) 
#define HAL_NF_USE_STM32_FLASH         TRUE
#endif

// Enable graphics low level hardware on STM32F769

#if !defined(HAL_DSI_MODULE_ENABLED) 
#define HAL_DSI_MODULE_ENABLED         TRUE
#endif

#endif // _HALCONF_NF_H_

