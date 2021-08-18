//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Texas Instruments Incorporated.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

// CC3220 Board Specific APIs

#ifndef __CC3220SF_LAUNCHXL_H
#define __CC3220SF_LAUNCHXL_H

#ifdef __cplusplus
extern "C" {
#endif

#define CC3220SF_LAUNCHXL_GPIO_LED_OFF (0)
#define CC3220SF_LAUNCHXL_GPIO_LED_ON  (1)

/*!
 *  @def    CC3220SF_LAUNCHXL_ADCName
 *  @brief  Enum of ADC names on the CC3220SF_LAUNCHXL dev board
 */
typedef enum CC3220SF_LAUNCHXL_ADCName {
    CC3220SF_LAUNCHXL_ADC0 = 0,
    CC3220SF_LAUNCHXL_ADC1,

    CC3220SF_LAUNCHXL_ADCCOUNT
} CC3220SF_LAUNCHXL_ADCName;

/*!
 *  @def    CC3220SF_LAUNCHXL_CaptureName
 *  @brief  Enum of Capture names on the CC3220SF_LAUNCHXL dev board
 */
typedef enum CC3220SF_LAUNCHXL_CaptureName {
    CC3220SF_LAUNCHXL_CAPTURE0 = 0,
    CC3220SF_LAUNCHXL_CAPTURE1,

    CC3220SF_LAUNCHXL_CAPTURECOUNT
} CC3220SF_LAUNCHXL_CaptureName;

/*!
 *  @def    CC3220SF_LAUNCHXL_CryptoName
 *  @brief  Enum of Crypto names on the CC3220SF_LAUNCHXL dev board
 */
typedef enum CC3220SF_LAUNCHXL_CryptoName {
    CC3220SF_LAUNCHXL_CRYPTO0 = 0,

    CC3220SF_LAUNCHXL_CRYPTOCOUNT
} CC3220SF_LAUNCHXL_CryptoName;

/*!
 *  @def    CC3220SF_LAUNCHXL_GPIOName
 *  @brief  Enum of GPIO names on the CC3220SF_LAUNCHXL dev board
 */
typedef enum CC3220SF_LAUNCHXL_GPIOName {
    CC3220SF_LAUNCHXL_GPIO_SW2 = 0,
    CC3220SF_LAUNCHXL_GPIO_SW3,
    CC3220SF_LAUNCHXL_SPI_MASTER_READY,
    CC3220SF_LAUNCHXL_SPI_SLAVE_READY,
    CC3220SF_LAUNCHXL_GPIO_LED_D10,

    /*
     *  CC3220SF_LAUNCHXL_GPIO_LED_D8 and CC3220SF_LAUNCHXL_GPIO_LED_D9 are shared with the
     *  I2C and PWM peripherals. In order for those examples to work, these
     *  LEDs are  taken out of gpioPinCOnfig[]
     */
    CC3220SF_LAUNCHXL_GPIO_LED_D9,
    CC3220SF_LAUNCHXL_GPIO_LED_D8,

    CC3220SF_LAUNCHXL_GPIO_TMP116_EN,

    /* Sharp LCD Pins */
    CC3220SF_LAUNCHXL_LCD_CS,
    CC3220SF_LAUNCHXL_LCD_POWER,
    CC3220SF_LAUNCHXL_LCD_ENABLE,

    CC3220SF_LAUNCHXL_GPIOCOUNT
} CC3220SF_LAUNCHXL_GPIOName;

/*!
 *  @def    CC3220SF_LAUNCHXL_I2CName
 *  @brief  Enum of I2C names on the CC3220SF_LAUNCHXL dev board
 */
typedef enum CC3220SF_LAUNCHXL_I2CName {
    CC3220SF_LAUNCHXL_I2C0 = 0,

    CC3220SF_LAUNCHXL_I2CCOUNT
} CC3220SF_LAUNCHXL_I2CName;

/*!
 *  @def    CC3220SF_LAUNCHXL_I2SName
 *  @brief  Enum of I2S names on the CC3220SF_LAUNCHXL dev board
 */
typedef enum CC3220SF_LAUNCHXL_I2SName {
    CC3220SF_LAUNCHXL_I2S0 = 0,

    CC3220SF_LAUNCHXL_I2SCOUNT
} CC3220SF_LAUNCHXL_I2SName;

/*!
 *  @def    CC3220SF_LAUNCHXL_PWMName
 *  @brief  Enum of PWM names on the CC3220SF_LAUNCHXL dev board
 */
typedef enum CC3220SF_LAUNCHXL_PWMName {
    CC3220SF_LAUNCHXL_PWM6 = 0,
    CC3220SF_LAUNCHXL_PWM7,

    CC3220SF_LAUNCHXL_PWMCOUNT
} CC3220SF_LAUNCHXL_PWMName;

/*!
 *  @def    CC3220SF_LAUNCHXL_SDFatFSName
 *  @brief  Enum of SDFatFS names on the CC3220SF_LAUNCHXL dev board
 */
typedef enum CC3220SF_LAUNCHXL_SDFatFSName {
    CC3220SF_LAUNCHXL_SDFatFS0 = 0,

    CC3220SF_LAUNCHXL_SDFatFSCOUNT
} CC3220SF_LAUNCHXL_SDFatFSName;

/*!
 *  @def    CC3220SF_LAUNCHXL_SDName
 *  @brief  Enum of SD names on the CC3220SF_LAUNCHXL dev board
 */
typedef enum CC3220SF_LAUNCHXL_SDName {
    CC3220SF_LAUNCHXL_SD0 = 0,

    CC3220SF_LAUNCHXL_SDCOUNT
} CC3220SF_LAUNCHXL_SDName;

/*!
 *  @def    CC3220SF_LAUNCHXL_SPIName
 *  @brief  Enum of SPI names on the CC3220SF_LAUNCHXL dev board
 */
typedef enum CC3220SF_LAUNCHXL_SPIName {
    CC3220SF_LAUNCHXL_SPI0 = 0,
    CC3220SF_LAUNCHXL_SPI1,

    CC3220SF_LAUNCHXL_SPICOUNT
} CC3220SF_LAUNCHXL_SPIName;

/*!
 *  @def    CC3220SF_LAUNCHXL_TimerName
 *  @brief  Enum of Timer names on the CC3220SF_LAUNCHXL dev board
 */
typedef enum CC3220SF_LAUNCHXL_TimerName {
    CC3220SF_LAUNCHXL_TIMER0 = 0,
    CC3220SF_LAUNCHXL_TIMER1,
    CC3220SF_LAUNCHXL_TIMER2,

    CC3220SF_LAUNCHXL_TIMERCOUNT
} CC3220SF_LAUNCHXL_TimerName;

/*!
 *  @def    CC3220SF_LAUNCHXL_UARTName
 *  @brief  Enum of UARTs on the CC3220SF_LAUNCHXL dev board
 */
typedef enum CC3220SF_LAUNCHXL_UARTName {
    CC3220SF_LAUNCHXL_UART0 = 0,
    CC3220SF_LAUNCHXL_UART1,

    CC3220SF_LAUNCHXL_UARTCOUNT
} CC3220SF_LAUNCHXL_UARTName;

/*!
 *  @def    CC3220SF_LAUNCHXL_WatchdogName
 *  @brief  Enum of Watchdogs on the CC3220SF_LAUNCHXL dev board
 */
typedef enum CC3220SF_LAUNCHXL_WatchdogName {
    CC3220SF_LAUNCHXL_WATCHDOG0 = 0,

    CC3220SF_LAUNCHXL_WATCHDOGCOUNT
} CC3220SF_LAUNCHXL_WatchdogName;

/*!
 *  @brief  Initialize the general board specific settings
 *
 *  This function initializes the general board specific settings.
 */
extern void CC3220SF_LAUNCHXL_initGeneral(void);

#ifdef __cplusplus
}
#endif

#endif /* __CC3220SF_LAUNCHXL_H */
