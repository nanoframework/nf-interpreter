//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

/**
 * @file    TI/TICC3200/hal_lld.h
 * @brief   TI CC3200 HAL subsystem low level driver header.
 * @pre     This module requires the following macros to be defined in the
 *          @p board.h file:
 *          - TODO: add required macros
 *
 * @addtogroup HAL
 * @{
 */

#ifndef HAL_LLD_H
#define HAL_LLD_H

#include "ticc3200_registry.h"

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/**
 * @name    Platform identification
 * @{
 */
 
#define PLATFORM_NAME           "TI CC3200"

/**
 * @}
 */

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @name    Configuration options
 * @{
 */

#if !defined(TICC3200_OSCSRC)
#define TICC3200_OSCSRC                     SYSCTL_RCC2_OSCSRC2_MO
#endif

#if !defined(TICC3200_MOSC_ENABLE)
#define TICC3200_MOSC_ENABLE                TRUE
#endif

#if !defined(TICC3200_DIV400_VALUE)
#define TICC3200_DIV400_VALUE               1
#endif

#if !defined(TICC3200_SYSDIV_VALUE)
#define TICC3200_SYSDIV_VALUE               2
#endif

#if !defined(TICC3200_USESYSDIV_ENABLE)
#define TICC3200_USESYSDIV_ENABLE           FALSE
#endif

#if !defined(TICC3200_SYSDIV2LSB_ENABLE)
#define TICC3200_SYSDIV2LSB_ENABLE          FALSE
#endif

#if !defined(TICC3200_BYPASS_VALUE)
#define TICC3200_BYPASS_VALUE               0
#endif

/**
 * @}
 */

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*
 * Configuration-related checks.
 */
#if !defined(TICC3200_MCUCONF)
#error "Using a wrong mcuconf.h file, TICC3200_MCUCONF not defined"
#endif

/*
 * Oscillator-related checks.
 */
#if !(TICC3200_OSCSRC == SYSCTL_RCC2_OSCSRC2_MO) &&                                 \
    !(TICC3200_OSCSRC == SYSCTL_RCC2_OSCSRC2_IO) &&                                \
    !(TICC3200_OSCSRC == SYSCTL_RCC2_OSCSRC2_IO4) &&                              \
    !(TICC3200_OSCSRC == SYSCTL_RCC2_OSCSRC2_30) &&                               \
    !(TICC3200_OSCSRC == SYSCTL_RCC2_OSCSRC2_32)
#error "Invalid value for TICC3200_OSCSRC defined"
#endif

// #if TICC3200_XTAL_VALUE == 4000000
// #define TICC3200_XTAL_              SYSCTL_RCC_XTAL_4MHZ
// #elif TICC3200_XTAL_VALUE == 4096000
// #define TICC3200_XTAL_              SYSCTL_RCC_XTAL_4_09MHZ
// #elif TICC3200_XTAL_VALUE == 4915200
// #define TICC3200_XTAL_              SYSCTL_RCC_XTAL_4_91MHZ
// #elif TICC3200_XTAL_VALUE == 5000000
// #define TICC3200_XTAL_              SYSCTL_RCC_XTAL_5MHZ
// #elif TICC3200_XTAL_VALUE == 5120000
// #define TICC3200_XTAL_              SYSCTL_RCC_XTAL_5_12MHZ
// #elif TICC3200_XTAL_VALUE == 6000000
// #define TICC3200_XTAL_              SYSCTL_RCC_XTAL_6MHZ
// #elif TICC3200_XTAL_VALUE == 6144000
// #define TICC3200_XTAL_              SYSCTL_RCC_XTAL_6_14MHZ
// #elif TICC3200_XTAL_VALUE == 7372800
// #define TICC3200_XTAL_              SYSCTL_RCC_XTAL_7_37MHZ
// #elif TICC3200_XTAL_VALUE == 8000000
// #define TICC3200_XTAL_              SYSCTL_RCC_XTAL_8MHZ
// #elif TICC3200_XTAL_VALUE == 8192000
// #define TICC3200_XTAL_              SYSCTL_RCC_XTAL_8_19MHZ
// #elif TICC3200_XTAL_VALUE == 10000000
// #define TICC3200_XTAL_              SYSCTL_RCC_XTAL_10MHZ
// #elif TICC3200_XTAL_VALUE == 12000000
// #define TICC3200_XTAL_              SYSCTL_RCC_XTAL_12MHZ
// #elif TICC3200_XTAL_VALUE == 12288000
// #define TICC3200_XTAL_              SYSCTL_RCC_XTAL_12_2MHZ
// #elif TICC3200_XTAL_VALUE == 13560000
// #define TICC3200_XTAL_              SYSCTL_RCC_XTAL_13_5MHZ
// #elif TICC3200_XTAL_VALUE == 14318180
// #define TICC3200_XTAL_              SYSCTL_RCC_XTAL_14_3MHZ
// #elif TICC3200_XTAL_VALUE == 16000000
// #define TICC3200_XTAL_              SYSCTL_RCC_XTAL_16MHZ
// #elif TICC3200_XTAL_VALUE == 16384000
// #define TICC3200_XTAL_              SYSCTL_RCC_XTAL_16_3MHZ
// #elif TICC3200_XTAL_VALUE == 18000000
// #define TICC3200_XTAL_              SYSCTL_RCC_XTAL_18MHZ
// #elif TICC3200_XTAL_VALUE == 20000000
// #define TICC3200_XTAL_              SYSCTL_RCC_XTAL_20MHZ
// #elif TICC3200_XTAL_VALUE == 24000000
// #define TICC3200_XTAL_              SYSCTL_RCC_XTAL_24MHZ
// #elif TICC3200_XTAL_VALUE == 25000000
// #define TICC3200_XTAL_              SYSCTL_RCC_XTAL_25MHZ
// #else
// #error "Invalid value for TICC3200_XTAL_VALUE defined"
// #endif

// #if TICC3200_MOSC_ENABLE == TRUE
// #define TICC3200_MOSCDIS            (0 << 0)
// #define TICC3200_XTAL               TICC3200_XTAL_
// #elif TICC3200_MOSC_ENABLE == FALSE
// #define TICC3200_MOSCDIS            (1 << 0)
// #define TICC3200_XTAL               0
// #else
// #error "Invalid value for TICC3200_MOSC_ENABLE defined"
// #endif

// #if TICC3200_DIV400_VALUE == 1
// #define TICC3200_DIV400             (1 << 30)
// #elif TICC3200_DIV400_VALUE == 0
// #define TICC3200_DIV400             (0 << 30)
// #else
// #error "Invalid value for TICC3200_DIV400_VALUE defined"
// #endif

// #if (TICC3200_SYSDIV_VALUE >= 0x02) && (TICC3200_SYSDIV_VALUE <= 0x3f)
// #define TICC3200_SYSDIV             (TICC3200_SYSDIV_VALUE << 23)
// #define TICC3200_SYSDIV2            (TICC3200_SYSDIV_VALUE << 23)
// #else
// #error "Invalid value for TICC3200_SYSDIV_VALUE defined"
// #endif

// #if TICC3200_USESYSDIV_ENABLE == TRUE
// #define TICC3200_USESYSDIV          (1 << 22)
// #elif TICC3200_USESYSDIV_ENABLE == FALSE
// #define TICC3200_USESYSDIV          (0 << 22)
// #else
// #error "Invalid value for TICC3200_USESYSDIV_ENABLE defined"
// #endif

// #if TICC3200_SYSDIV2LSB_ENABLE == TRUE
// #define TICC3200_SYSDIV2LSB         (1 << 22)
// #elif TICC3200_SYSDIV2LSB_ENABLE == FALSE
// #define TICC3200_SYSDIV2LSB         (0 << 22)
// #else
// #error "Invalid value for TICC3200_SYSDIV2LSB_ENABLE defined"
// #endif

// #if TICC3200_BYPASS_VALUE == 1
// #define TICC3200_SRC                16000000
// #elif TICC3200_BYPASS_VALUE == 0
// #define TICC3200_SRC                (200000000 + (TICC3200_DIV400_VALUE * 200000000))
// #else
// #error "Invalid value for TICC3200_BYPASS_VALUE defined"
// #endif

// #if (TICC3200_OSCSRC == TICC3200_RCC_OSCSRC_MOSC) && (TICC3200_MOSC_ENABLE == FALSE)
// #error "Main Oscillator selected but not enabled"
// #endif

/*
 * System Clock calculation 
 */
#define TICC3200_SYSCLK             (TICC3200_SRC / (((TICC3200_SYSDIV_VALUE << TICC3200_DIV400_VALUE /*& TICC3200_BYPASS_VALUE*/) | (TICC3200_SYSDIV2LSB >> 22)) + 1))

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

/* Various helpers.*/
#include "nvic.h"
#include "ticc3200_isr.h"
// #include "TICC3200_udma.h"

#ifdef __cplusplus
extern "C" {
#endif
  void hal_lld_init(void);
  void ti3200_clock_init(void);
#ifdef __cplusplus
}
#endif

#endif /* HAL_LLD_H */

/**
 * @}
 */
