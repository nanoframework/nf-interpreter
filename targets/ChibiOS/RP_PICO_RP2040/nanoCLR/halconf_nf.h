//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef HALCONF_NF_H
#define HALCONF_NF_H

// nanoFramework HAL overlay for nanoCLR

// RP2040 does not have STM32-specific peripherals
#define HAL_NF_USE_STM32_RNG    FALSE
#define HAL_NF_USE_STM32_FLASH  FALSE

#endif // HALCONF_NF_H
