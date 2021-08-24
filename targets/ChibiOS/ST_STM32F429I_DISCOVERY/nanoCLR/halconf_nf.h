//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef HALCONF_NF_H
#define HALCONF_NF_H

// enables STM32 Flash driver
#if !defined(HAL_NF_USE_STM32_FLASH) 
#define HAL_NF_USE_STM32_FLASH         TRUE
#endif

//  Enables the FSMC subsystem.
#if !defined(HAL_NF_USE_FSMC)
#define HAL_NF_USE_FSMC                TRUE
#endif

#endif // HALCONF_NF_H
