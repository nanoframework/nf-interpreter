//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef MCUCONF_H
#define MCUCONF_H

/*
 * CC3200 drivers configuration.
 * The following settings override the default settings present in
 * the various device driver implementation headers.
 * Note that the settings for each driver only have effect if the whole
 * driver is enabled in halconf.h.
 *
 * IRQ priorities:
 * 7...0       Lowest...Highest.
 */

#define TICC3200_MCUCONF


/*
 * HAL driver system settings.
 */
#define TICC3200_OSCSRC                         SYSCTL_RCC2_OSCSRC2_MO
#define TICC3200_MOSC_ENABLE                    TRUE
#define TICC3200_DIV400_VALUE                   1
#define TICC3200_SYSDIV_VALUE                   2
#define TICC3200_USESYSDIV_ENABLE               FALSE
#define TICC3200_SYSDIV2LSB_ENABLE              FALSE
#define TICC3200_BYPASS_VALUE                   0
#define TICC3200_PWM_FIELDS                     (SYSCTL_RCC_USEPWMDIV |            \
                                                SYSCTL_RCC_PWMDIV_8)

/*
 * GPIO driver system settings.
 */
#define TICC3200_GPIO_GPIOA_USE_AHB             TRUE
#define TICC3200_GPIO_GPIOB_USE_AHB             TRUE
#define TICC3200_GPIO_GPIOC_USE_AHB             TRUE
#define TICC3200_GPIO_GPIOD_USE_AHB             TRUE
#define TICC3200_GPIO_GPIOE_USE_AHB             TRUE
#define TICC3200_GPIO_GPIOF_USE_AHB             TRUE

/*
 * GPT driver system settings.
 */
#define TICC3200_GPT_USE_GPT0                   FALSE
#define TICC3200_GPT_USE_GPT1                   FALSE
#define TICC3200_GPT_USE_GPT2                   FALSE
#define TICC3200_GPT_USE_GPT3                   FALSE

#define TICC3200_GPT_GPT0A_IRQ_PRIORITY         7
#define TICC3200_GPT_GPT1A_IRQ_PRIORITY         7
#define TICC3200_GPT_GPT2A_IRQ_PRIORITY         7
#define TICC3200_GPT_GPT3A_IRQ_PRIORITY         7
/*
 * SERIAL driver system settings.
 */
#define TICC3200_SERIAL_USE_UART0               TRUE
#define TICC3200_SERIAL_USE_UART1               FALSE
#define TICC3200_SERIAL_UART0_PRIORITY          5
#define TICC3200_SERIAL_UART1_PRIORITY          5

/*
 * ST driver system settings.
 */
#define TICC3200_ST_IRQ_PRIORITY                2
#define TICC3200_ST_TIMER_NUMBER                0
#define TICC3200_ST_TIMER_LETTER                A

// header for nanoFramework overlay drivers
#include "mcuconf_nf.h"

#endif /* MCUCONF_H */
