//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef HAL_NF_COMMUNITY_H
#define HAL_NF_COMMUNITY_H


// these are for error checks on the configuration header files
#if !defined(HAL_NF_USE_STM32_FLASH)
#define HAL_NF_USE_STM32_FLASH                  FALSE
#endif

#if !defined(HAL_NF_USE_STM32_CRC)
// the default for this driver is to be included
#define HAL_NF_USE_STM32_CRC                    TRUE
#endif

#if !defined(HAL_NF_USE_STM32_RNG)
// the default for this driver is to be included
#define HAL_NF_USE_STM32_RNG                    TRUE
#endif

#if !defined(HAL_NF_USE_FSMC)
#define HAL_NF_USE_FSMC                         FALSE
#endif

#if !defined(HAL_NF_USE_STM32_ONEWIRE)
// the default for this driver is NOT to be included
#define HAL_NF_USE_STM32_ONEWIRE                FALSE
#endif

#if !defined(HAL_NF_USE_STM32_QSPI)
// the default for this driver is NOT to be included
#define HAL_NF_USE_STM32_QSPI                   FALSE
#endif

// Abstract interfaces

// Shared headers
// #include "hal_nnnn.h"

// Normal drivers
#include "stm32_qspi/hal_stm32_qspi.h"
#include "stm32_flash/hal_stm32_flash.h"
#include "stm32_crc/hal_stm32_crc.h"
#include "stm32_rng/hal_stm32_rng.h"
#include "stm32_fsmc/hal_stm32_fsmc.h"
#include "stm32_onewire/hal_stm32_onewire.h"

// Complex drivers
// #include "hal_nnnn.h"

///////////////////////////////////////////////////////////////////////////////
// Driver constants.                                                         //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Driver pre-compile time settings.                                         //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Derived constants and error checks.                                       //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Driver data structures and types.                                         //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Driver macros.                                                            //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// External declarations.                                                    //
///////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

  void halCommunityInit(void);

#ifdef __cplusplus
}
#endif

#endif // HAL_NF_COMMUNITY_H

