//
// Copyright (c) 2019 The nanoFramework project contributors
// Portions Copyright 2018 NXP. All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _BOARD_H_
#define _BOARD_H_

#include "clock_config.h"
#include "fsl_common.h"
#include "fsl_gpio.h"
#include "fsl_clock.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief The board name */
#define BOARD_NAME "MIMXRT1060-EVK"

/* The UART to use for debug messages. */
#define BOARD_DEBUG_UART_TYPE kSerialPort_Uart
#define BOARD_DEBUG_UART_BASEADDR (uint32_t) LPUART1
#define BOARD_DEBUG_UART_INSTANCE 1U

#define BOARD_DEBUG_UART_CLK_FREQ BOARD_DebugConsoleSrcFreq()

#define BOARD_UART_IRQ LPUART1_IRQn
#define BOARD_UART_IRQ_HANDLER LPUART1_IRQHandler

#ifndef BOARD_DEBUG_UART_BAUDRATE
#define BOARD_DEBUG_UART_BAUDRATE (115200U)
#endif /* BOARD_DEBUG_UART_BAUDRATE */

/*! @brief The USER_LED used for board */
#define LOGIC_LED_ON (0U)
#define LOGIC_LED_OFF (1U)
#ifndef BOARD_USER_LED_GPIO
#define BOARD_USER_LED_GPIO GPIO1
#endif
#ifndef BOARD_USER_LED_GPIO_PIN
#define BOARD_USER_LED_GPIO_PIN (9U)
#endif

#define USER_LED_INIT(output)                                            \
    GPIO_PinWrite(BOARD_USER_LED_GPIO, BOARD_USER_LED_GPIO_PIN, output); \
    BOARD_USER_LED_GPIO->GDIR |= (1U << BOARD_USER_LED_GPIO_PIN) /*!< Enable target USER_LED */
#define USER_LED_ON() \
    GPIO_PortClear(BOARD_USER_LED_GPIO, 1U << BOARD_USER_LED_GPIO_PIN)                  /*!< Turn off target USER_LED */
#define USER_LED_OFF() GPIO_PortSet(BOARD_USER_LED_GPIO, 1U << BOARD_USER_LED_GPIO_PIN) /*!<Turn on target USER_LED*/
#define USER_LED_TOGGLE()                                       \
    GPIO_PinWrite(BOARD_USER_LED_GPIO, BOARD_USER_LED_GPIO_PIN, \
                  0x1 ^ GPIO_PinRead(BOARD_USER_LED_GPIO, BOARD_USER_LED_GPIO_PIN)) /*!< Toggle target USER_LED */

/*! @brief Define the port interrupt number for the board switches */
#ifndef BOARD_USER_BUTTON_GPIO
#define BOARD_USER_BUTTON_GPIO GPIO5
#endif
#ifndef BOARD_USER_BUTTON_GPIO_PIN
#define BOARD_USER_BUTTON_GPIO_PIN (0U)
#endif
#define BOARD_USER_BUTTON_IRQ GPIO5_Combined_0_15_IRQn
#define BOARD_USER_BUTTON_IRQ_HANDLER GPIO5_Combined_0_15_IRQHandler
#define BOARD_USER_BUTTON_NAME "SW8"

/*! @brief The board flash size */
#define BOARD_FLASH_SIZE (0x800000U)

/*! @brief The ENET PHY address. */
#define BOARD_ENET0_PHY_ADDRESS (0x02U) /* Phy address of enet port 0. */

/* USB PHY condfiguration */
#define BOARD_USB_PHY_D_CAL (0x0CU)
#define BOARD_USB_PHY_TXCAL45DP (0x06U)
#define BOARD_USB_PHY_TXCAL45DM (0x06U)

#define BOARD_ARDUINO_INT_IRQ (GPIO1_INT3_IRQn)
#define BOARD_ARDUINO_I2C_IRQ (LPI2C1_IRQn)
#define BOARD_ARDUINO_I2C_INDEX (1)
#define BOARD_USDHC1_BASEADDR USDHC1
#define BOARD_USDHC2_BASEADDR USDHC2
#define BOARD_USDHC_CD_GPIO_BASE GPIO2
#define BOARD_USDHC_CD_GPIO_PIN 28
#define BOARD_USDHC_CD_PORT_IRQ GPIO2_Combined_16_31_IRQn
#define BOARD_USDHC_CD_PORT_IRQ_HANDLER GPIO2_Combined_16_31_IRQHandler

#define BOARD_USDHC_CD_STATUS() (GPIO_PinRead(BOARD_USDHC_CD_GPIO_BASE, BOARD_USDHC_CD_GPIO_PIN))

#define BOARD_USDHC_CD_INTERRUPT_STATUS() (GPIO_PortGetInterruptFlags(BOARD_USDHC_CD_GPIO_BASE))
#define BOARD_USDHC_CD_CLEAR_INTERRUPT(flag) (GPIO_PortClearInterruptFlags(BOARD_USDHC_CD_GPIO_BASE, flag))

#define BOARD_USDHC_CD_GPIO_INIT()                                                          \
    {                                                                                       \
        gpio_pin_config_t sw_config = {                                                     \
            kGPIO_DigitalInput, 0, kGPIO_IntRisingOrFallingEdge,                            \
        };                                                                                  \
        GPIO_PinInit(BOARD_USDHC_CD_GPIO_BASE, BOARD_USDHC_CD_GPIO_PIN, &sw_config);        \
        GPIO_PortEnableInterrupts(BOARD_USDHC_CD_GPIO_BASE, 1U << BOARD_USDHC_CD_GPIO_PIN); \
        GPIO_PortClearInterruptFlags(BOARD_USDHC_CD_GPIO_BASE, ~0);                         \
    }
#define BOARD_HAS_SDCARD (1U)
#define BOARD_SD_POWER_RESET_GPIO (GPIO1)
#define BOARD_SD_POWER_RESET_GPIO_PIN (19U)

#define BOARD_USDHC_CARD_INSERT_CD_LEVEL (0U)

#define BOARD_USDHC_MMCCARD_POWER_CONTROL(state)

#define BOARD_USDHC_MMCCARD_POWER_CONTROL_INIT()                                            \
    {                                                                                       \
        gpio_pin_config_t sw_config = {                                                     \
            kGPIO_DigitalOutput, 0, kGPIO_NoIntmode,                                        \
        };                                                                                  \
        GPIO_PinInit(BOARD_SD_POWER_RESET_GPIO, BOARD_SD_POWER_RESET_GPIO_PIN, &sw_config); \
        GPIO_PinWrite(BOARD_SD_POWER_RESET_GPIO, BOARD_SD_POWER_RESET_GPIO_PIN, true);      \
    }

#define BOARD_USDHC_SDCARD_POWER_CONTROL_INIT()                                             \
    {                                                                                       \
        gpio_pin_config_t sw_config = {                                                     \
            kGPIO_DigitalOutput, 0, kGPIO_NoIntmode,                                        \
        };                                                                                  \
        GPIO_PinInit(BOARD_SD_POWER_RESET_GPIO, BOARD_SD_POWER_RESET_GPIO_PIN, &sw_config); \
    }

#define BOARD_USDHC_SDCARD_POWER_CONTROL(state) \
    (GPIO_PinWrite(BOARD_SD_POWER_RESET_GPIO, BOARD_SD_POWER_RESET_GPIO_PIN, state))

#define BOARD_USDHC1_CLK_FREQ (CLOCK_GetSysPfdFreq(kCLOCK_Pfd2) / (CLOCK_GetDiv(kCLOCK_Usdhc1Div) + 1U))
#define BOARD_USDHC2_CLK_FREQ (CLOCK_GetSysPfdFreq(kCLOCK_Pfd2) / (CLOCK_GetDiv(kCLOCK_Usdhc2Div) + 1U))

#define BOARD_SD_HOST_BASEADDR BOARD_USDHC1_BASEADDR
#define BOARD_SD_HOST_CLK_FREQ BOARD_USDHC1_CLK_FREQ
#define BOARD_SD_HOST_IRQ USDHC1_IRQn

#define BOARD_MMC_HOST_BASEADDR BOARD_USDHC2_BASEADDR
#define BOARD_MMC_HOST_CLK_FREQ BOARD_USDHC2_CLK_FREQ
#define BOARD_MMC_HOST_IRQ USDHC2_IRQn
#define BOARD_MMC_VCCQ_SUPPLY kMMC_VoltageWindow170to195
#define BOARD_MMC_VCC_SUPPLY kMMC_VoltageWindows270to360
/* we are using the BB SD socket to DEMO the MMC example,but the
* SD socket provide 4bit bus only, so we define this macro to avoid
* 8bit data bus test
*/
#define BOARD_MMC_SUPPORT_8BIT_BUS (1U)

#define BOARD_SD_HOST_SUPPORT_SDR104_FREQ (200000000U)
#define BOARD_SD_HOST_SUPPORT_HS200_FREQ (180000000U)

/*! @brief The WIFI-QCA shield pin. */
#define BOARD_INITGT202SHIELD_PWRON_GPIO GPIO1                    /*!< GPIO device name: GPIO */
#define BOARD_INITGT202SHIELD_PWRON_PORT 1U                       /*!< PORT device index: 1 */
#define BOARD_INITGT202SHIELD_PWRON_GPIO_PIN 3U                   /*!< PIO4 pin index: 3 */
#define BOARD_INITGT202SHIELD_PWRON_PIN_NAME GPIO1_3              /*!< Pin name */
#define BOARD_INITGT202SHIELD_PWRON_LABEL "PWRON"                 /*!< Label */
#define BOARD_INITGT202SHIELD_PWRON_NAME "PWRON"                  /*!< Identifier name */
#define BOARD_INITGT202SHIELD_PWRON_DIRECTION kGPIO_DigitalOutput /*!< Direction */

#define BOARD_INITGT202SHIELD_IRQ_GPIO GPIO1                   /*!< GPIO device name: GPIO */
#define BOARD_INITGT202SHIELD_IRQ_PORT 1U                      /*!< PORT device index: 1 */
#define BOARD_INITGT202SHIELD_IRQ_GPIO_PIN 19U                 /*!< PIO1 pin index: 19 */
#define BOARD_INITGT202SHIELD_IRQ_PIN_NAME GPIO1_19            /*!< Pin name */
#define BOARD_INITGT202SHIELD_IRQ_LABEL "IRQ"                  /*!< Label */
#define BOARD_INITGT202SHIELD_IRQ_NAME "IRQ"                   /*!< Identifier name */
#define BOARD_INITGT202SHIELD_IRQ_DIRECTION kGPIO_DigitalInput /*!< Direction */

#define BOARD_INITSILEX2401SHIELD_PWRON_GPIO GPIO1                    /*!< GPIO device name: GPIO */
#define BOARD_INITSILEX2401SHIELD_PWRON_PORT 1U                       /*!< PORT device index: 1 */
#define BOARD_INITSILEX2401SHIELD_PWRON_GPIO_PIN 9U                   /*!< PIO4 pin index: 9 */
#define BOARD_INITSILEX2401SHIELD_PWRON_PIN_NAME GPIO1_9              /*!< Pin name */
#define BOARD_INITSILEX2401SHIELD_PWRON_LABEL "PWRON"                 /*!< Label */
#define BOARD_INITSILEX2401SHIELD_PWRON_NAME "PWRON"                  /*!< Identifier name */
#define BOARD_INITSILEX2401SHIELD_PWRON_DIRECTION kGPIO_DigitalOutput /*!< Direction */

#define BOARD_INITSILEX2401SHIELD_IRQ_GPIO GPIO1                   /*!< GPIO device name: GPIO */
#define BOARD_INITSILEX2401SHIELD_IRQ_PORT 1U                      /*!< PORT device index: 1 */
#define BOARD_INITSILEX2401SHIELD_IRQ_GPIO_PIN 11U                 /*!< PIO1 pin index: 11 */
#define BOARD_INITSILEX2401SHIELD_IRQ_PIN_NAME GPIO1_11            /*!< Pin name */
#define BOARD_INITSILEX2401SHIELD_IRQ_LABEL "IRQ"                  /*!< Label */
#define BOARD_INITSILEX2401SHIELD_IRQ_NAME "IRQ"                   /*!< Identifier name */
#define BOARD_INITSILEX2401SHIELD_IRQ_DIRECTION kGPIO_DigitalInput /*!< Direction */

/* @Brief Board accelerator sensor configuration */
#define BOARD_ACCEL_I2C_BASEADDR LPI2C1
/* Select USB1 PLL (480 MHz) as LPI2C's clock source */
#define BOARD_ACCEL_I2C_CLOCK_SOURCE_SELECT (0U)
/* Clock divider for LPI2C clock source */
#define BOARD_ACCEL_I2C_CLOCK_SOURCE_DIVIDER (5U)
#define BOARD_ACCEL_I2C_CLOCK_FREQ (CLOCK_GetFreq(kCLOCK_Usb1PllClk) / 8 / (BOARD_ACCEL_I2C_CLOCK_SOURCE_DIVIDER + 1U))

#define BOARD_CODEC_I2C_BASEADDR LPI2C1
#define BOARD_CODEC_I2C_CLOCK_SOURCE_SELECT (0U)
#define BOARD_CODEC_I2C_CLOCK_SOURCE_DIVIDER (5U)
#define BOARD_CODEC_I2C_CLOCK_FREQ \
    ((CLOCK_GetFreq(kCLOCK_Usb1PllClk) / 8) / (BOARD_CODEC_I2C_CLOCK_SOURCE_DIVIDER + 1U))

/* @Brief Board CAMERA configuration */
#define BOARD_CAMERA_I2C_BASEADDR LPI2C1
#define BOARD_CAMERA_I2C_CLOCK_SOURCE_DIVIDER (5U)
#define BOARD_CAMERA_I2C_CLOCK_SOURCE_SELECT (0U) /* Select USB1 PLL (480 MHz) as LPI2C's clock source */
#define BOARD_CAMERA_I2C_CLOCK_FREQ \
    (CLOCK_GetFreq(kCLOCK_Usb1PllClk) / 8 / (BOARD_CAMERA_I2C_CLOCK_SOURCE_DIVIDER + 1U))

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
 * API
 ******************************************************************************/
uint32_t BOARD_DebugConsoleSrcFreq(void);

void BOARD_InitDebugConsole(void);

void BOARD_ConfigMPU(void);

void BOARD_SD_Pin_Config(uint32_t speed, uint32_t strength);
void BOARD_MMC_Pin_Config(uint32_t speed, uint32_t strength);

status_t BOARD_InitSEMC(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _BOARD_H_ */
