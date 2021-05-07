//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _HALCONF_NF_H_
#define _HALCONF_NF_H_

// enables STM32 Flash driver
#if !defined(HAL_NF_USE_STM32_FLASH) 
#define HAL_NF_USE_STM32_FLASH         TRUE
#endif

#if !defined(HAL_NF_USE_STM32_RNG)
#define HAL_NF_USE_STM32_RNG           FALSE
#endif

#endif // _HALCONF_NF_H_

