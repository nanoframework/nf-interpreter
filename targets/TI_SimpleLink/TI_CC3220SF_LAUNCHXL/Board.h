//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Texas Instruments Incorporated.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef __BOARD_H
#define __BOARD_H

#define Board_CC3220SF_LAUNCHXL

#include <ti/drivers/Board.h>
#include "CC3220SF_LAUNCHXL.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "CC3220SF_LAUNCHXL.h"

#define Board_init                   CC3220SF_LAUNCHXL_initGeneral
#define Board_initGeneral            CC3220SF_LAUNCHXL_initGeneral

#define Board_ADC0                   CC3220SF_LAUNCHXL_ADC0
#define Board_ADC1                   CC3220SF_LAUNCHXL_ADC1

#define Board_CAPTURE0               CC3220SF_LAUNCHXL_CAPTURE0
#define Board_CAPTURE1               CC3220SF_LAUNCHXL_CAPTURE1

#define Board_CRYPTO0                CC3220SF_LAUNCHXL_CRYPTO0

#define Board_GPIO_LED_ON            CC3220SF_LAUNCHXL_GPIO_LED_ON
#define Board_GPIO_LED_OFF           CC3220SF_LAUNCHXL_GPIO_LED_OFF
#define Board_GPIO_LED0              CC3220SF_LAUNCHXL_GPIO_LED_D10
/*
 *  CC3220SF_LAUNCHXL_GPIO_LED_D8 and CC3220SF_LAUNCHXL_GPIO_LED_D9 are shared with the I2C
 *  and PWM peripherals. In order for those examples to work, these LEDs are
 *  taken out of gpioPinCOnfig[]
 */
#define Board_GPIO_LED1              CC3220SF_LAUNCHXL_GPIO_LED_D9
#define Board_GPIO_LED2              CC3220SF_LAUNCHXL_GPIO_LED_D8

#define Board_GPIO_BUTTON0           CC3220SF_LAUNCHXL_GPIO_SW2
#define Board_GPIO_BUTTON1           CC3220SF_LAUNCHXL_GPIO_SW3

#define Board_GPIO_TMP116_EN         CC3220SF_LAUNCHXL_GPIO_TMP116_EN

#define Board_I2C0                   CC3220SF_LAUNCHXL_I2C0
#define Board_I2C_TMP                CC3220SF_LAUNCHXL_I2C0

#define Board_I2S0                   CC3220SF_LAUNCHXL_I2S0

#define Board_PWM0                   CC3220SF_LAUNCHXL_PWM6
#define Board_PWM1                   CC3220SF_LAUNCHXL_PWM7

#define Board_SD0                    CC3220SF_LAUNCHXL_SD0

#define Board_SDFatFS0               CC3220SF_LAUNCHXL_SD0

/* CC3220SF_LAUNCHXL_SPI0 is reserved for the NWP */
#define Board_SPI0                   CC3220SF_LAUNCHXL_SPI1
#define Board_SPI_MASTER             CC3220SF_LAUNCHXL_SPI1
#define Board_SPI_SLAVE              CC3220SF_LAUNCHXL_SPI1
#define Board_SPI_MASTER_READY       CC3220SF_LAUNCHXL_SPI_MASTER_READY
#define Board_SPI_SLAVE_READY        CC3220SF_LAUNCHXL_SPI_SLAVE_READY

#define Board_TIMER0                 CC3220SF_LAUNCHXL_TIMER0
#define Board_TIMER1                 CC3220SF_LAUNCHXL_TIMER1
#define Board_TIMER2                 CC3220SF_LAUNCHXL_TIMER2

#define Board_UART0                  CC3220SF_LAUNCHXL_UART0
#define Board_UART1                  CC3220SF_LAUNCHXL_UART1

#define Board_WATCHDOG0              CC3220SF_LAUNCHXL_WATCHDOG0

/* Board specific I2C address */
#define Board_TMP006_ADDR            (0x41)
#define Board_I2C_TMP006_ADDR        Board_TMP006_ADDR

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H */
