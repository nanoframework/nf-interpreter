//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2006..2015 Giovanni Di Sirio. All rights reserved.
// See LICENSE file in the project root for full license information.
//

// clang-format off

/**
 * @file    halconf.h
 * @brief   HAL configuration for the MCUboot bootloader on ST_STM32F769I_DISCOVERY.
 *
 * Enables only the subsystems needed by the MCUboot bootloader:
 *   PAL    — required by halInit() for board GPIO setup and palReadPad().
 *   SERIAL — USART1 (PA9/PA10 AF7) for MCUboot serial recovery output.
 */

#ifndef HALCONF_H
#define HALCONF_H

#define _CHIBIOS_HAL_CONF_
#define _CHIBIOS_HAL_CONF_VER_9_1_

#include "mcuconf.h"

#if !defined(HAL_USE_SAFETY) || defined(__DOXYGEN__)
#define HAL_USE_SAFETY                      FALSE
#endif

#if !defined(HAL_USE_PAL) || defined(__DOXYGEN__)
#define HAL_USE_PAL                         TRUE
#endif

#if !defined(HAL_USE_ADC) || defined(__DOXYGEN__)
#define HAL_USE_ADC                         FALSE
#endif

#if !defined(HAL_USE_CAN) || defined(__DOXYGEN__)
#define HAL_USE_CAN                         FALSE
#endif

#if !defined(HAL_USE_CRY) || defined(__DOXYGEN__)
#define HAL_USE_CRY                         FALSE
#endif

#if !defined(HAL_USE_DAC) || defined(__DOXYGEN__)
#define HAL_USE_DAC                         FALSE
#endif

#if !defined(HAL_USE_EFL) || defined(__DOXYGEN__)
#define HAL_USE_EFL                         FALSE
#endif

#if !defined(HAL_USE_GPT) || defined(__DOXYGEN__)
#define HAL_USE_GPT                         FALSE
#endif

#if !defined(HAL_USE_I2C) || defined(__DOXYGEN__)
#define HAL_USE_I2C                         FALSE
#endif

#if !defined(HAL_USE_I2S) || defined(__DOXYGEN__)
#define HAL_USE_I2S                         FALSE
#endif

#if !defined(HAL_USE_ICU) || defined(__DOXYGEN__)
#define HAL_USE_ICU                         FALSE
#endif

#if !defined(HAL_USE_MAC) || defined(__DOXYGEN__)
#define HAL_USE_MAC                         FALSE
#endif

#if !defined(HAL_USE_MMC_SPI) || defined(__DOXYGEN__)
#define HAL_USE_MMC_SPI                     FALSE
#endif

#if !defined(HAL_USE_PWM) || defined(__DOXYGEN__)
#define HAL_USE_PWM                         FALSE
#endif

#if !defined(HAL_USE_RTC) || defined(__DOXYGEN__)
#define HAL_USE_RTC                         FALSE
#endif

#if !defined(HAL_USE_SERIAL) || defined(__DOXYGEN__)
#define HAL_USE_SERIAL                      TRUE
#endif

#if !defined(HAL_USE_SERIAL_USB) || defined(__DOXYGEN__)
#define HAL_USE_SERIAL_USB                  FALSE
#endif

#if !defined(HAL_USE_SIO) || defined(__DOXYGEN__)
#define HAL_USE_SIO                         FALSE
#endif

#if !defined(HAL_USE_SPI) || defined(__DOXYGEN__)
#define HAL_USE_SPI                         FALSE
#endif

#if !defined(HAL_USE_TRNG) || defined(__DOXYGEN__)
#define HAL_USE_TRNG                        FALSE
#endif

#if !defined(HAL_USE_UART) || defined(__DOXYGEN__)
#define HAL_USE_UART                        FALSE
#endif

#if !defined(HAL_USE_USB) || defined(__DOXYGEN__)
#define HAL_USE_USB                         FALSE
#endif

#if !defined(HAL_USE_WDG) || defined(__DOXYGEN__)
#define HAL_USE_WDG                         FALSE
#endif

#if !defined(HAL_USE_WSPI) || defined(__DOXYGEN__)
#define HAL_USE_WSPI                        FALSE
#endif

/*===========================================================================*/
/* SERIAL driver settings.                                                   */
/*===========================================================================*/

#if !defined(SERIAL_DEFAULT_BITRATE) || defined(__DOXYGEN__)
#define SERIAL_DEFAULT_BITRATE              38400
#endif

#if !defined(SERIAL_BUFFERS_SIZE) || defined(__DOXYGEN__)
#define SERIAL_BUFFERS_SIZE                 16
#endif

/* nf-overlay community HAL drivers (STM32 internal flash FLASHv2 LLD). */
#include "halconf_nf.h"

#endif /* HALCONF_H */
