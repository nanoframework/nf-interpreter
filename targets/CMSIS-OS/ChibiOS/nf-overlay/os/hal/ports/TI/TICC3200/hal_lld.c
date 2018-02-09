//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

/**
 * @file    TI/TICC3200/hal_lld.c
 * @brief   TI CC3200 HAL Driver subsystem low level driver source.
 *
 * @addtogroup HAL
 * @{
 */

#include "hal.h"

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Low level HAL driver initialization.
 *
 * @notapi
 */
void hal_lld_init(void)
{
}

/**
 * @brief   CC3200 clocks and PLL initialization.
 * @note    All the involved constants come from the file @p board.h and 
 *          @p mcuconf.h.
 * @note    This function should be invoked just after the system reset.
 *
 * @special
 */
void ti3200_clock_init(void)
{
  uint32_t rcc, rcc2, i;

  /* 1. Bypass the PLL and system clock divider by setting the BYPASS bit and
   * clearing the USESYSDIV bit in the RCC register, thereby configuring the
   * microcontroller to run off a "raw" clock source and allowing for the new
   * PLL configuration to be validated before switching the system clock to the
   * PLL. */
  /* read */

  rcc = HWREG(SYSCTL_RCC);
  rcc2 = HWREG(SYSCTL_RCC2);
  
  /* modify */
  rcc |= SYSCTL_RCC_BYPASS;
  rcc &= ~SYSCTL_RCC_USESYSDIV;
  rcc2 |= SYSCTL_RCC2_BYPASS2 | SYSCTL_RCC2_USERCC2;
  
  /* write */
  HWREG(SYSCTL_RCC) = rcc;
  HWREG(SYSCTL_RCC2) = rcc2;

  /* 2 Select the crystal value (XTAL) and oscillator source (OSCSRC), and
   * clear the PWRDN bit in RCC and RCC2. Setting the XTAL field automatically
   * pulls valid PLL configuration data for the appropriate crystal, and
   * clearing the PWRDN bit powers and enables the PLL and its output. */
  /* modify */
  rcc &= ~(SYSCTL_RCC_OSCSRC_M | SYSCTL_RCC_XTAL_M | SYSCTL_RCC_PWRDN | SYSCTL_RCC_MOSCDIS);
  rcc |= ((TIVA_XTAL | TIVA_OSCSRC | TIVA_MOSCDIS) & (SYSCTL_RCC_XTAL_M | SYSCTL_RCC_OSCSRC_M | SYSCTL_RCC_MOSCDIS));
  rcc2 &= ~(SYSCTL_RCC2_OSCSRC2_M | SYSCTL_RCC2_PWRDN2);
  rcc2 |= ((TIVA_OSCSRC | TIVA_DIV400) & (SYSCTL_RCC2_OSCSRC2_M | SYSCTL_RCC2_DIV400));
  
  /* write */
  HWREG(SYSCTL_RCC) = rcc;
  HWREG(SYSCTL_RCC2) = rcc2;
  for(i = 100000; i; i--);

  /* 3. Select the desired system divider (SYSDIV) in RCC and RCC2 and set the
   * USESYSDIV bit in RCC. The SYSDIV field determines the system frequency for
   * the microcontroller. */
  /* modify */
  rcc &= ~SYSCTL_RCC_SYSDIV_M;
  rcc |= (TIVA_SYSDIV & SYSCTL_RCC_SYSDIV_M) | SYSCTL_RCC_USESYSDIV;
  rcc2 &= ~(SYSCTL_RCC2_SYSDIV2_M | SYSCTL_RCC2_SYSDIV2LSB);
  rcc2 |= ((TIVA_SYSDIV2 | TIVA_SYSDIV2LSB) & (SYSCTL_RCC2_SYSDIV2_M | SYSCTL_RCC2_SYSDIV2LSB));
  
  /* write */
  HWREG(SYSCTL_RCC) = rcc;
  HWREG(SYSCTL_RCC2) = rcc2;

  /* 4. Wait for the PLL to lock by polling the PLLLRIS bit in the Raw
   * Interrupt Status (RIS) register. */
  while ((HWREG(SYSCTL_RIS) & SYSCTL_RIS_PLLLRIS) == 0);

  /* 5. Enable use of the PLL by clearing the BYPASS bit in RCC and RCC2. */
  rcc &= ~SYSCTL_RCC_BYPASS;
  rcc2 &= ~SYSCTL_RCC2_BYPASS2;
  rcc |= (TIVA_BYPASS_VALUE << 11);
  rcc2 |= (TIVA_BYPASS_VALUE << 11);
  HWREG(SYSCTL_RCC) = rcc;
  HWREG(SYSCTL_RCC2) = rcc2;

#if HAL_USE_PWM
  HWREG(SYSCTL_RCC) |= TIVA_PWM_FIELDS;
#endif

#if defined(TICC3200_UDMA_REQUIRED)
  udmaInit();
#endif
}

/**
 * @}
 */
