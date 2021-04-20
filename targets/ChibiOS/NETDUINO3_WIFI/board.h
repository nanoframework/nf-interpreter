//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef BOARD_H
#define BOARD_H

/*
 * Board identifier.
 */
#define BOARD_NETDUINO3WIFI
#define BOARD_NAME                  "Netduino 3 Wifi"

#define BOARD_OTG_NOVBUSSENS

/*
 * Board oscillators-related settings.
 * NOTE: LSE not fitted.
 */
#if !defined(STM32_LSECLK)
#define STM32_LSECLK                32768U
#endif

#if !defined(STM32_HSECLK)
#define STM32_HSECLK                25000000U
#endif

/*
 * Board voltages.
 * Required for performance limits calculation.
 */
#define STM32_VDD                   300U

/*
 * MCU type as defined in the ST header.
 */
#define STM32F427xx

/*
 * IO pins assignments.
 */
#define GPIOA_D8                	0U
#define GPIOA_D7             		1U
#define GPIOA_D3             		2U
#define GPIOA_D2                	3U
#define GPIOA_CC_IRQ           		4U
#define GPIOA_CC_SCK                5U
#define GPIOA_CC_MISO               6U
#define GPIOA_CC_MOSI               7U
#define GPIOA_STATE_LED				8U
#define GPIOA_VBUS               	9U
#define GPIOA_USR_LED              	10U
#define GPIOA_OTG_FS_DM             11U
#define GPIOA_OTG_FS_DP             12U
#define GPIOA_JTMS		            13U
#define GPIOA_JTCK                 	14U
#define GPIOA_JTDI                	15U

#define GPIOB_MICROSD_CS           	0U
#define GPIOB_MICROSD_CTRL         	1U
#define GPIOB_BOOT1                 2U
#define GPIOB_JTDO		            3U
#define GPIOB_PB4		            4U
#define GPIOB_SWITCH1            	5U
#define GPIOB_I2C1_SCL             	6U
#define GPIOB_I2C1_SDA              7U
#define GPIOB_D5                	8U
#define GPIOB_D6                	9U
#define GPIOB_D10                	10U
#define GPIOB_MICROSD_INS         	11U
#define GPIOB_D4	             	12U
#define GPIOB_D13           		13U
#define GPIOB_D12		            14U
#define GPIOB_D11		            15U

#define GPIOC_A0             		0U
#define GPIOC_A1          			1U
#define GPIOC_A2	           		2U
#define GPIOC_A3 	                3U
#define GPIOC_A4            		4U
#define GPIOC_A5	             	5U
#define GPIOC_D1             		6U
#define GPIOC_D0                	7U
#define GPIOC_CC_CS                 8U
#define GPIOC_LINK_LED              9U
#define GPIOC_SPI3_CLK              10U
#define GPIOC_SPI3_MISO             11U
#define GPIOC_SPI3_MOSI             12U
#define GPIOC_POWER_LED             13U
#define GPIOC_OSC32_IN              14U
#define GPIOC_OSC32_OUT             15U

#define GPIOD_SPI_CS_GOPORT1       	0U
#define GPIOD_SPI_CS_GOPORT2       	1U
#define GPIOD_SPI_CS_GOPORT3        2U
#define GPIOD_CC_RTS                3U
#define GPIOD_CC_CTS                4U
#define GPIOD_CC_RX                	5U
#define GPIOD_CC_TX                	6U
#define GPIOD_PWR_ON_GOPORT1        7U
#define GPIOD_USART3_TX_GOPORT1		8U
#define GPIOD_USART3_RX_GOPORT1		9U
#define GPIOD_PWR_ON_GOPORT2       	10U
#define GPIOD_SWITCH1		       	11U
#define GPIOD_PWR_ON_GOPORT3       	12U
#define GPIOD_INT_GOPORT1         	13U
#define GPIOD_INT_GOPORT2          	14U
#define GPIOD_INT_GOPORT3          	15U

#define GPIOE_USART8_RX_GOPORT3		0U
#define GPIOE_USART8_TX_GOPORT3     1U
#define GPIOE_SPI4_SCK              2U
#define GPIOE_EN_UART_TX_PU_GOPORT1	3U
#define GPIOE_CC_HIB                4U
#define GPIOE_D9	                5U
#define GPIOE_SPI4_MOSI             6U
#define GPIOE_USART7_RX_GOPORT2     7U
#define GPIOE_USART7_TX_GOPORT2     8U
#define GPIOE_LED_GOPORT1          	9U
#define GPIOE_EN_UART_TX_PU_GOPORT2	10U
#define GPIOE_LED_GOPORT2			11U
#define GPIOE_EN_UART_TX_PU_GOPORT3	12U
#define GPIOE_SPI4_MISO            	13U
#define GPIOE_LED_GOPORT3          	14U
#define GPIOE_CC_TCXO_EN_SOP2      	15U

#define GPIOH_OSC_IN                0U
#define GPIOH_OSC_OUT               1U
#define GPIOH_PIN2                  2U
#define GPIOH_PIN3                  3U
#define GPIOH_PIN4                  4U
#define GPIOH_PIN5                  5U
#define GPIOH_PIN6                  6U
#define GPIOH_PIN7                  7U
#define GPIOH_PIN8                  8U
#define GPIOH_PIN9                  9U
#define GPIOH_PIN10                 10U
#define GPIOH_PIN11                 11U
#define GPIOH_PIN12                 12U
#define GPIOH_PIN13                 13U
#define GPIOH_PIN14                 14U
#define GPIOH_PIN15                 15U

/*
 * IO lines assignments.
 */
#define LINE_LED1                   PAL_LINE(GPIOA, 10U)
#define LINE_SD_DETECT              PAL_LINE(GPIOB, 11U)

#define LINE_LED_GOPORT1            PAL_LINE(GPIOE, 9U)

#define LINE_LED_GOPORT2            PAL_LINE(GPIOE, 11U)

#define LINE_LED_GOPORT3            PAL_LINE(GPIOE, 14U)

/*
 * I/O ports initial setup, this configuration is established soon after reset
 * in the initialization code.
 * Please refer to the STM32 Reference Manual for details.
 */
#define PIN_MODE_INPUT(n)           (0U << ((n) * 2U))
#define PIN_MODE_OUTPUT(n)          (1U << ((n) * 2U))
#define PIN_MODE_ALTERNATE(n)       (2U << ((n) * 2U))
#define PIN_MODE_ANALOG(n)          (3U << ((n) * 2U))
#define PIN_ODR_LOW(n)              (0U << (n))
#define PIN_ODR_HIGH(n)             (1U << (n))
#define PIN_OTYPE_PUSHPULL(n)       (0U << (n))
#define PIN_OTYPE_OPENDRAIN(n)      (1U << (n))
#define PIN_OSPEED_VERYLOW(n)       (0U << ((n) * 2U))
#define PIN_OSPEED_LOW(n)           (1U << ((n) * 2U))
#define PIN_OSPEED_MEDIUM(n)        (2U << ((n) * 2U))
#define PIN_OSPEED_HIGH(n)          (3U << ((n) * 2U))
#define PIN_PUPDR_FLOATING(n)       (0U << ((n) * 2U))
#define PIN_PUPDR_PULLUP(n)         (1U << ((n) * 2U))
#define PIN_PUPDR_PULLDOWN(n)       (2U << ((n) * 2U))
#define PIN_AFIO_AF(n, v)           ((v) << (((n) % 8U) * 4U))

/*
 * GPIOA setup:
 *
 * PA0  - D8
 * PA1  - D7
 * PA2  - D3
 * PA3  - D2
 * PA4  - CC_IRQ
 * PA5  - CC_SCK
 * PA6  - CC_MISO
 * PA7  - CC_MOSI
 * PA8  - STATE_LED
 * PA9  - VBUS
 * PA10 - USR_LED
 * PA11 - OTG_FS_DM
 * PA12 - OTG_FS_DP
 * PA13 - JTMS
 * PA14 - JTCK
 * PA15 - JTDI
 */
#define VAL_GPIOA_MODER             (PIN_MODE_INPUT(GPIOA_D8) |         	\
                                     PIN_MODE_INPUT(GPIOA_D7) |      		\
                                     PIN_MODE_INPUT(GPIOA_D3) |      		\
                                     PIN_MODE_INPUT(GPIOA_D2) |     		\
                                     PIN_MODE_INPUT(GPIOA_CC_IRQ) |  			\
                                     PIN_MODE_ALTERNATE(GPIOA_CC_SCK) |           \
                                     PIN_MODE_ALTERNATE(GPIOA_CC_MISO) |     		\
                                     PIN_MODE_ALTERNATE(GPIOA_CC_MOSI) |        	\
                                     PIN_MODE_OUTPUT(GPIOA_STATE_LED) |   	\
                                     PIN_MODE_INPUT(GPIOA_VBUS) |    	\
                                     PIN_MODE_OUTPUT(GPIOA_USR_LED) |    	\
                                     PIN_MODE_ALTERNATE(GPIOA_OTG_FS_DM) |  \
                                     PIN_MODE_ALTERNATE(GPIOA_OTG_FS_DP) |  \
                                     PIN_MODE_INPUT(GPIOA_JTMS) |      \
                                     PIN_MODE_INPUT(GPIOA_JTCK) |      		\
                                     PIN_MODE_INPUT(GPIOA_JTDI))
#define VAL_GPIOA_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOA_D8) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOA_D7) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOA_D3) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOA_D2) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOA_CC_IRQ) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOA_CC_SCK) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOA_CC_MISO) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOA_CC_MOSI) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOA_STATE_LED) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOA_VBUS) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOA_USR_LED) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOA_OTG_FS_DM) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOA_OTG_FS_DP) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOA_JTMS) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOA_JTCK) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOA_JTDI))
#define VAL_GPIOA_OSPEEDR           (PIN_OSPEED_HIGH(GPIOA_D8) |     \
                                     PIN_OSPEED_HIGH(GPIOA_D7) |  \
                                     PIN_OSPEED_HIGH(GPIOA_D3) |  \
                                     PIN_OSPEED_HIGH(GPIOA_D2) |        \
                                     PIN_OSPEED_HIGH(GPIOA_CC_IRQ) |     \
                                     PIN_OSPEED_HIGH(GPIOA_CC_SCK) |       \
                                     PIN_OSPEED_HIGH(GPIOA_CC_MISO) |        \
                                     PIN_OSPEED_HIGH(GPIOA_CC_MOSI) |    \
                                     PIN_OSPEED_HIGH(GPIOA_STATE_LED) |      \
                                     PIN_OSPEED_HIGH(GPIOA_VBUS) |    \
                                     PIN_OSPEED_HIGH(GPIOA_USR_LED) |    \
                                     PIN_OSPEED_HIGH(GPIOA_OTG_FS_DM) |        \
                                     PIN_OSPEED_HIGH(GPIOA_OTG_FS_DP) |        \
                                     PIN_OSPEED_HIGH(GPIOA_JTMS) |         \
                                     PIN_OSPEED_HIGH(GPIOA_JTCK) |         \
                                     PIN_OSPEED_HIGH(GPIOA_JTDI))
#define VAL_GPIOA_PUPDR             (PIN_PUPDR_FLOATING(GPIOA_D8) |     \
                                     PIN_PUPDR_FLOATING(GPIOA_D7) |  \
                                     PIN_PUPDR_FLOATING(GPIOA_D3) |  \
                                     PIN_PUPDR_FLOATING(GPIOA_D2) |     \
                                     PIN_PUPDR_FLOATING(GPIOA_CC_IRQ) |  \
                                     PIN_PUPDR_FLOATING(GPIOA_CC_SCK) |         \
                                     PIN_PUPDR_FLOATING(GPIOA_CC_MISO) |     \
                                     PIN_PUPDR_FLOATING(GPIOA_CC_MOSI) |      \
                                     PIN_PUPDR_FLOATING(GPIOA_STATE_LED) |   \
                                     PIN_PUPDR_FLOATING(GPIOA_VBUS) |      \
                                     PIN_PUPDR_FLOATING(GPIOA_USR_LED) |      \
                                     PIN_PUPDR_FLOATING(GPIOA_OTG_FS_DM) |     \
                                     PIN_PUPDR_FLOATING(GPIOA_OTG_FS_DP) |     \
                                     PIN_PUPDR_FLOATING(GPIOA_JTMS) |        \
                                     PIN_PUPDR_FLOATING(GPIOA_JTCK) |      \
                                     PIN_PUPDR_FLOATING(GPIOA_JTDI))
#define VAL_GPIOA_ODR               (PIN_ODR_LOW(GPIOA_D8) |           \
                                     PIN_ODR_LOW(GPIOA_D7) |        \
                                     PIN_ODR_LOW(GPIOA_D3) |        \
                                     PIN_ODR_LOW(GPIOA_D2) |           \
                                     PIN_ODR_LOW(GPIOA_CC_IRQ) |        \
                                     PIN_ODR_LOW(GPIOA_CC_SCK) |             \
                                     PIN_ODR_LOW(GPIOA_CC_MISO) |           \
                                     PIN_ODR_LOW(GPIOA_CC_MOSI) |          \
                                     PIN_ODR_LOW(GPIOA_STATE_LED) |         \
                                     PIN_ODR_LOW(GPIOA_VBUS) |          \
                                     PIN_ODR_LOW(GPIOA_USR_LED) |          \
                                     PIN_ODR_HIGH(GPIOA_OTG_FS_DM) |           \
                                     PIN_ODR_HIGH(GPIOA_OTG_FS_DP) |           \
                                     PIN_ODR_LOW(GPIOA_JTMS) |            \
                                     PIN_ODR_LOW(GPIOA_JTCK) |            \
                                     PIN_ODR_LOW(GPIOA_JTDI))
#define VAL_GPIOA_AFRL              (PIN_AFIO_AF(GPIOA_D8, 0U) |        \
                                     PIN_AFIO_AF(GPIOA_D7, 0U) |     \
                                     PIN_AFIO_AF(GPIOA_D3, 0U) |     \
                                     PIN_AFIO_AF(GPIOA_D2, 0U) |       \
                                     PIN_AFIO_AF(GPIOA_CC_IRQ, 0U) |    \
                                     PIN_AFIO_AF(GPIOA_CC_SCK, 5U) |          \
                                     PIN_AFIO_AF(GPIOA_CC_MISO, 5U) |       \
                                     PIN_AFIO_AF(GPIOA_CC_MOSI, 5U))
#define VAL_GPIOA_AFRH              (PIN_AFIO_AF(GPIOA_STATE_LED, 0U) |      \
                                     PIN_AFIO_AF(GPIOA_VBUS, 0U) |       \
                                     PIN_AFIO_AF(GPIOA_USR_LED, 0U) |       \
                                     PIN_AFIO_AF(GPIOA_OTG_FS_DM, 10U) |       \
                                     PIN_AFIO_AF(GPIOA_OTG_FS_DP, 10U) |       \
                                     PIN_AFIO_AF(GPIOA_JTMS, 0U) |         \
                                     PIN_AFIO_AF(GPIOA_JTCK, 0U) |         \
                                     PIN_AFIO_AF(GPIOA_JTDI, 0U))

/*
 * GPIOB setup:
 *
 * PB0  - MICROSD_CS
 * PB1  - MICROSD_CTRL
 * PB2  - BOOT1
 * PB3  - JTDO
 * PB4  - PB4
 * PB5  - SWITCH1
 * PB6  - I2C1_SCL
 * PB7  - I2C1_SDA
 * PB8  - D5
 * PB9  - D6
 * PB10 - D10 (=SPI2_SS)
 * PB11 - MICROSD_INS
 * PB12 - D4
 * PB13 - D13 (=SPI2_SCK)
 * PB14 - D12 (=SPI2_MISO)
 * PB15 - D11 (=SPI2_MOSI)
 */
#define VAL_GPIOB_MODER             (PIN_MODE_OUTPUT(GPIOB_MICROSD_CS) |     \
                                     PIN_MODE_OUTPUT(GPIOB_MICROSD_CTRL) |     \
                                     PIN_MODE_OUTPUT(GPIOB_BOOT1) |          \
                                     PIN_MODE_OUTPUT(GPIOB_JTDO) |        \
                                     PIN_MODE_INPUT(GPIOB_PB4) |           \
                                     PIN_MODE_INPUT(GPIOB_SWITCH1) | \
                                     PIN_MODE_ALTERNATE(GPIOB_I2C1_SCL) |  \
                                     PIN_MODE_ALTERNATE(GPIOB_I2C1_SDA) |           \
                                     PIN_MODE_INPUT(GPIOB_D5) |     \
                                     PIN_MODE_INPUT(GPIOB_D6) |     \
                                     PIN_MODE_OUTPUT(GPIOB_D10) |     \
                                     PIN_MODE_INPUT(GPIOB_MICROSD_INS) |     \
                                     PIN_MODE_INPUT(GPIOB_D4) |  \
                                     PIN_MODE_ALTERNATE(GPIOB_D13) |    \
                                     PIN_MODE_ALTERNATE(GPIOB_D12) |  \
                                     PIN_MODE_ALTERNATE(GPIOB_D11))
#define VAL_GPIOB_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOB_MICROSD_CS) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOB_MICROSD_CTRL) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOB_BOOT1) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOB_JTDO) |        \
                                     PIN_OTYPE_PUSHPULL(GPIOB_PB4) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOB_SWITCH1) | \
                                     PIN_OTYPE_OPENDRAIN(GPIOB_I2C1_SCL) |  \
                                     PIN_OTYPE_OPENDRAIN(GPIOB_I2C1_SDA) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOB_D5) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOB_D6) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOB_D10) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOB_MICROSD_INS) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOB_D4) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOB_D13) |\
                                     PIN_OTYPE_PUSHPULL(GPIOB_D12) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOB_D11))
#define VAL_GPIOB_OSPEEDR           (PIN_OSPEED_HIGH(GPIOB_MICROSD_CS) |        \
                                     PIN_OSPEED_HIGH(GPIOB_MICROSD_CTRL) |        \
                                     PIN_OSPEED_HIGH(GPIOB_BOOT1) |         \
                                     PIN_OSPEED_HIGH(GPIOB_JTDO) |           \
                                     PIN_OSPEED_HIGH(GPIOB_PB4) |       \
                                     PIN_OSPEED_HIGH(GPIOB_SWITCH1) |    \
                                     PIN_OSPEED_HIGH(GPIOB_I2C1_SCL) |     \
                                     PIN_OSPEED_HIGH(GPIOB_I2C1_SDA) |       \
                                     PIN_OSPEED_HIGH(GPIOB_D5) |        \
                                     PIN_OSPEED_HIGH(GPIOB_D6) |        \
                                     PIN_OSPEED_HIGH(GPIOB_D10) |        \
                                     PIN_OSPEED_HIGH(GPIOB_MICROSD_INS) |        \
                                     PIN_OSPEED_HIGH(GPIOB_D4) |     \
                                     PIN_OSPEED_HIGH(GPIOB_D13) |\
                                     PIN_OSPEED_HIGH(GPIOB_D12) |     \
                                     PIN_OSPEED_HIGH(GPIOB_D11))
#define VAL_GPIOB_PUPDR             (PIN_PUPDR_FLOATING(GPIOB_MICROSD_CS) |     \
                                     PIN_PUPDR_FLOATING(GPIOB_MICROSD_CTRL) |     \
                                     PIN_PUPDR_FLOATING(GPIOB_BOOT1) |        \
                                     PIN_PUPDR_FLOATING(GPIOB_JTDO) |        \
                                     PIN_PUPDR_FLOATING(GPIOB_PB4) |         \
                                     PIN_PUPDR_FLOATING(GPIOB_SWITCH1) | \
                                     PIN_PUPDR_FLOATING(GPIOB_I2C1_SCL) |  \
                                     PIN_PUPDR_FLOATING(GPIOB_I2C1_SDA) |         \
                                     PIN_PUPDR_FLOATING(GPIOB_D5) |     \
                                     PIN_PUPDR_FLOATING(GPIOB_D6) |     \
                                     PIN_PUPDR_FLOATING(GPIOB_D10) |     \
                                     PIN_PUPDR_FLOATING(GPIOB_MICROSD_INS) |     \
                                     PIN_PUPDR_FLOATING(GPIOB_D4) |  \
                                     PIN_PUPDR_FLOATING(GPIOB_D13) |\
                                     PIN_PUPDR_FLOATING(GPIOB_D12) |  \
                                     PIN_PUPDR_FLOATING(GPIOB_D11))
#define VAL_GPIOB_ODR               (PIN_ODR_HIGH(GPIOB_MICROSD_CS) |           \
                                     PIN_ODR_HIGH(GPIOB_MICROSD_CTRL) |           \
                                     PIN_ODR_HIGH(GPIOB_BOOT1) |            \
                                     PIN_ODR_LOW(GPIOB_JTDO) |              \
                                     PIN_ODR_LOW(GPIOB_PB4) |             \
                                     PIN_ODR_LOW(GPIOB_SWITCH1) |       \
                                     PIN_ODR_LOW(GPIOB_I2C1_SCL) |        \
                                     PIN_ODR_LOW(GPIOB_I2C1_SDA) |             \
                                     PIN_ODR_LOW(GPIOB_D5) |           \
                                     PIN_ODR_LOW(GPIOB_D6) |           \
                                     PIN_ODR_LOW(GPIOB_D10) |           \
                                     PIN_ODR_HIGH(GPIOB_MICROSD_INS) |           \
                                     PIN_ODR_LOW(GPIOB_D4) |        \
                                     PIN_ODR_LOW(GPIOB_D13) |      \
                                     PIN_ODR_LOW(GPIOB_D12) |        \
                                     PIN_ODR_LOW(GPIOB_D11))
#define VAL_GPIOB_AFRL              (PIN_AFIO_AF(GPIOB_MICROSD_CS, 0U) |       \
                                     PIN_AFIO_AF(GPIOB_MICROSD_CTRL, 0U) |       \
                                     PIN_AFIO_AF(GPIOB_BOOT1, 0U) |         \
                                     PIN_AFIO_AF(GPIOB_JTDO, 0U) |           \
                                     PIN_AFIO_AF(GPIOB_PB4, 0U) |          \
                                     PIN_AFIO_AF(GPIOB_SWITCH1, 0U) |   \
                                     PIN_AFIO_AF(GPIOB_I2C1_SCL, 4U) |    \
                                     PIN_AFIO_AF(GPIOB_I2C1_SDA, 4U))
#define VAL_GPIOB_AFRH              (PIN_AFIO_AF(GPIOB_D5, 0U) |       \
                                     PIN_AFIO_AF(GPIOB_D6, 0U) |       \
                                     PIN_AFIO_AF(GPIOB_D10, 0U) |       \
                                     PIN_AFIO_AF(GPIOB_MICROSD_INS, 0U) |       \
                                     PIN_AFIO_AF(GPIOB_D4, 0U) |    \
                                     PIN_AFIO_AF(GPIOB_D13, 5U) |   \
                                     PIN_AFIO_AF(GPIOB_D12, 5U) |    \
                                     PIN_AFIO_AF(GPIOB_D11, 5U))

/*
 * GPIOC setup:
 *
 * PC0  - A0
 * PC1  - A1
 * PC2  - A2
 * PC3  - A3
 * PC4  - A4
 * PC5  - A5
 * PC6  - USART6_TX_D1
 * PC7  - USART6_RX_D0
 * PC8  - CC_CS
 * PC9  - LINK_LED
 * PC10 - SPI3_CLK
 * PC11 - SPI3_MISO
 * PC12 - SPI3_MOSI
 * PC13 - POWER_LED
 * PC14 - OSC32_IN
 * PC15 - OSC32_OUT
 */
#define VAL_GPIOC_MODER             (PIN_MODE_ANALOG(GPIOC_A0) |  \
                                     PIN_MODE_ANALOG(GPIOC_A1) |  \
                                     PIN_MODE_ANALOG(GPIOC_A2) |   \
                                     PIN_MODE_ANALOG(GPIOC_A3) |           \
                                     PIN_MODE_ANALOG(GPIOC_A4) |    \
                                     PIN_MODE_ANALOG(GPIOC_A5) |      \
                                     PIN_MODE_ALTERNATE(GPIOC_D1) |  \
                                     PIN_MODE_ALTERNATE(GPIOC_D0) |     \
                                     PIN_MODE_OUTPUT(GPIOC_CC_CS) |           \
                                     PIN_MODE_OUTPUT(GPIOC_LINK_LED) |   \
                                     PIN_MODE_ALTERNATE(GPIOC_SPI3_CLK) |     \
                                     PIN_MODE_ALTERNATE(GPIOC_SPI3_MISO) |          \
                                     PIN_MODE_ALTERNATE(GPIOC_SPI3_MOSI) |          \
                                     PIN_MODE_OUTPUT(GPIOC_POWER_LED) |          \
                                     PIN_MODE_ALTERNATE(GPIOC_OSC32_IN) |       \
                                     PIN_MODE_ALTERNATE(GPIOC_OSC32_OUT))
#define VAL_GPIOC_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOC_A0) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOC_A1) |\
                                     PIN_OTYPE_PUSHPULL(GPIOC_A2) |\
                                     PIN_OTYPE_PUSHPULL(GPIOC_A3) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOC_A4) | \
                                     PIN_OTYPE_PUSHPULL(GPIOC_A5) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOC_D1) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOC_D0) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOC_CC_CS) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOC_LINK_LED) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOC_SPI3_CLK) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOC_SPI3_MISO) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOC_SPI3_MOSI) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOC_POWER_LED) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOC_OSC32_IN) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOC_OSC32_OUT))
#define VAL_GPIOC_OSPEEDR           (PIN_OSPEED_HIGH(GPIOC_A0) |     \
                                     PIN_OSPEED_HIGH(GPIOC_A1) |  \
                                     PIN_OSPEED_HIGH(GPIOC_A2) |   \
                                     PIN_OSPEED_HIGH(GPIOC_A3) |       \
                                     PIN_OSPEED_HIGH(GPIOC_A4) |    \
                                     PIN_OSPEED_HIGH(GPIOC_A5) |     \
                                     PIN_OSPEED_HIGH(GPIOC_D1) |     \
                                     PIN_OSPEED_HIGH(GPIOC_D0) |        \
                                     PIN_OSPEED_HIGH(GPIOC_CC_CS) |       \
                                     PIN_OSPEED_HIGH(GPIOC_LINK_LED) |      \
                                     PIN_OSPEED_HIGH(GPIOC_SPI3_CLK) |        \
                                     PIN_OSPEED_HIGH(GPIOC_SPI3_MISO) |      \
                                     PIN_OSPEED_HIGH(GPIOC_SPI3_MOSI) |      \
                                     PIN_OSPEED_HIGH(GPIOC_POWER_LED) |      \
                                     PIN_OSPEED_HIGH(GPIOC_OSC32_IN) |      \
                                     PIN_OSPEED_HIGH(GPIOC_OSC32_OUT))
#define VAL_GPIOC_PUPDR             (PIN_PUPDR_FLOATING(GPIOC_A0) |  \
                                     PIN_PUPDR_FLOATING(GPIOC_A1) |\
                                     PIN_PUPDR_FLOATING(GPIOC_A2) |\
                                     PIN_PUPDR_FLOATING(GPIOC_A3) |         \
                                     PIN_PUPDR_FLOATING(GPIOC_A4) | \
                                     PIN_PUPDR_FLOATING(GPIOC_A5) |  \
                                     PIN_PUPDR_FLOATING(GPIOC_D1) |  \
                                     PIN_PUPDR_FLOATING(GPIOC_D0) |     \
                                     PIN_PUPDR_FLOATING(GPIOC_CC_CS) |         \
                                     PIN_PUPDR_FLOATING(GPIOC_LINK_LED) |   \
                                     PIN_PUPDR_FLOATING(GPIOC_SPI3_CLK) |     \
                                     PIN_PUPDR_FLOATING(GPIOC_SPI3_MISO) |        \
                                     PIN_PUPDR_FLOATING(GPIOC_SPI3_MOSI) |        \
                                     PIN_PUPDR_FLOATING(GPIOC_POWER_LED) |        \
                                     PIN_PUPDR_FLOATING(GPIOC_OSC32_IN) |   \
                                     PIN_PUPDR_FLOATING(GPIOC_OSC32_OUT))
#define VAL_GPIOC_ODR               (PIN_ODR_LOW(GPIOC_A0) |        \
                                     PIN_ODR_LOW(GPIOC_A1) |     \
                                     PIN_ODR_LOW(GPIOC_A2) |      \
                                     PIN_ODR_LOW(GPIOC_A3) |             \
                                     PIN_ODR_LOW(GPIOC_A4) |       \
                                     PIN_ODR_LOW(GPIOC_A5) |        \
                                     PIN_ODR_LOW(GPIOC_D1) |        \
                                     PIN_ODR_LOW(GPIOC_D0) |           \
                                     PIN_ODR_LOW(GPIOC_CC_CS) |             \
                                     PIN_ODR_LOW(GPIOC_LINK_LED) |         \
                                     PIN_ODR_LOW(GPIOC_SPI3_CLK) |           \
                                     PIN_ODR_LOW(GPIOC_SPI3_MISO) |            \
                                     PIN_ODR_LOW(GPIOC_SPI3_MOSI) |            \
                                     PIN_ODR_LOW(GPIOC_POWER_LED) |            \
                                     PIN_ODR_LOW(GPIOC_OSC32_IN) |         \
                                     PIN_ODR_LOW(GPIOC_OSC32_OUT))
#define VAL_GPIOC_AFRL              (PIN_AFIO_AF(GPIOC_A0, 0U) |    \
                                     PIN_AFIO_AF(GPIOC_A1, 0U) |  \
                                     PIN_AFIO_AF(GPIOC_A2, 0U) |   \
                                     PIN_AFIO_AF(GPIOC_A3, 0U) |          \
                                     PIN_AFIO_AF(GPIOC_A4, 0U) |    \
                                     PIN_AFIO_AF(GPIOC_A5, 0U) |     \
                                     PIN_AFIO_AF(GPIOC_D1, 8U) |    \
                                     PIN_AFIO_AF(GPIOC_D0, 8U))
#define VAL_GPIOC_AFRH              (PIN_AFIO_AF(GPIOC_CC_CS, 0U) |          \
                                     PIN_AFIO_AF(GPIOC_LINK_LED, 0U) |      \
                                     PIN_AFIO_AF(GPIOC_SPI3_CLK, 6U) |       \
                                     PIN_AFIO_AF(GPIOC_SPI3_MISO, 6U) |         \
                                     PIN_AFIO_AF(GPIOC_SPI3_MOSI, 6U) |         \
                                     PIN_AFIO_AF(GPIOC_POWER_LED, 0U) |         \
                                     PIN_AFIO_AF(GPIOC_OSC32_IN, 0U) |      \
                                     PIN_AFIO_AF(GPIOC_OSC32_OUT, 0U))

/*
 * GPIOD setup:
 *
 * PD0  - SPI_CS_GOPORT1
 * PD1  - SPI_CS_GOPORT2
 * PD2  - SPI_CS_GOPORT3
 * PD3  - CC_RTS
 * PD4  - CC_CTS
 * PD5  - CC_RX
 * PD6  - CC_TX
 * PD7  - PWR_ON_GOPORT1
 * PD8  - USART3_TX_GOPORT1
 * PD9  - USART3_RX_GOPORT1
 * PD10 - PWR_ON_GOPORT2
 * PD11 - SWITCH1
 * PD12 - PWR_ON_GOPORT3
 * PD13 - INT_GOPORT1
 * PD14 - INT_GOPORT2
 * PD15 - INT_GOPORT3
 */
#define VAL_GPIOD_MODER             (PIN_MODE_INPUT(GPIOD_SPI_CS_GOPORT1) |     \
                                     PIN_MODE_INPUT(GPIOD_SPI_CS_GOPORT2) |     \
                                     PIN_MODE_INPUT(GPIOD_SPI_CS_GOPORT3) |           \
                                     PIN_MODE_ALTERNATE(GPIOD_CC_RTS) |     \
                                     PIN_MODE_ALTERNATE(GPIOD_CC_CTS) |           \
                                     PIN_MODE_ALTERNATE(GPIOD_CC_RX) |           \
                                     PIN_MODE_ALTERNATE(GPIOD_CC_TX) |     \
                                     PIN_MODE_OUTPUT(GPIOD_PWR_ON_GOPORT1) |           \
                                     PIN_MODE_ALTERNATE(GPIOD_USART3_TX_GOPORT1) |    \
                                     PIN_MODE_ALTERNATE(GPIOD_USART3_RX_GOPORT1) |    \
                                     PIN_MODE_OUTPUT(GPIOD_PWR_ON_GOPORT2) |    \
                                     PIN_MODE_INPUT(GPIOD_SWITCH1) |         \
                                     PIN_MODE_OUTPUT(GPIOD_PWR_ON_GOPORT3) |       \
                                     PIN_MODE_OUTPUT(GPIOD_INT_GOPORT1) |       \
                                     PIN_MODE_OUTPUT(GPIOD_INT_GOPORT2) |     \
                                     PIN_MODE_OUTPUT(GPIOD_INT_GOPORT3))
#define VAL_GPIOD_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOD_SPI_CS_GOPORT1) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOD_SPI_CS_GOPORT2) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOD_SPI_CS_GOPORT3) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOD_CC_RTS) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOD_CC_CTS) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOD_CC_RX) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOD_CC_TX) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOD_PWR_ON_GOPORT1) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOD_USART3_TX_GOPORT1) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOD_USART3_RX_GOPORT1) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOD_PWR_ON_GOPORT2) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOD_SWITCH1) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOD_PWR_ON_GOPORT3) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOD_INT_GOPORT1) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOD_INT_GOPORT2) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOD_INT_GOPORT3))
#define VAL_GPIOD_OSPEEDR           (PIN_OSPEED_HIGH(GPIOD_SPI_CS_GOPORT1) |        \
                                     PIN_OSPEED_HIGH(GPIOD_SPI_CS_GOPORT2) |        \
                                     PIN_OSPEED_HIGH(GPIOD_SPI_CS_GOPORT3) |       \
                                     PIN_OSPEED_HIGH(GPIOD_CC_RTS) |        \
                                     PIN_OSPEED_HIGH(GPIOD_CC_CTS) |       \
                                     PIN_OSPEED_HIGH(GPIOD_CC_RX) |       \
                                     PIN_OSPEED_HIGH(GPIOD_CC_TX) |        \
                                     PIN_OSPEED_HIGH(GPIOD_PWR_ON_GOPORT1) |       \
                                     PIN_OSPEED_HIGH(GPIOD_USART3_TX_GOPORT1) |       \
                                     PIN_OSPEED_HIGH(GPIOD_USART3_RX_GOPORT1) |       \
                                     PIN_OSPEED_HIGH(GPIOD_PWR_ON_GOPORT2) |       \
                                     PIN_OSPEED_HIGH(GPIOD_SWITCH1) |        \
                                     PIN_OSPEED_HIGH(GPIOD_PWR_ON_GOPORT3) |       \
                                     PIN_OSPEED_HIGH(GPIOD_INT_GOPORT1) |       \
                                     PIN_OSPEED_HIGH(GPIOD_INT_GOPORT2) |        \
                                     PIN_OSPEED_HIGH(GPIOD_INT_GOPORT3))
#define VAL_GPIOD_PUPDR             (PIN_PUPDR_FLOATING(GPIOD_SPI_CS_GOPORT1) |     \
                                     PIN_PUPDR_FLOATING(GPIOD_SPI_CS_GOPORT2) |     \
                                     PIN_PUPDR_FLOATING(GPIOD_SPI_CS_GOPORT3) |         \
                                     PIN_PUPDR_FLOATING(GPIOD_CC_RTS) |     \
                                     PIN_PUPDR_FLOATING(GPIOD_CC_CTS) |         \
                                     PIN_PUPDR_FLOATING(GPIOD_CC_RX) |         \
                                     PIN_PUPDR_FLOATING(GPIOD_CC_TX) |     \
                                     PIN_PUPDR_FLOATING(GPIOD_PWR_ON_GOPORT1) |         \
                                     PIN_PUPDR_FLOATING(GPIOD_USART3_TX_GOPORT1) |    \
                                     PIN_PUPDR_FLOATING(GPIOD_USART3_RX_GOPORT1) |    \
                                     PIN_PUPDR_FLOATING(GPIOD_PWR_ON_GOPORT2) |    \
                                     PIN_PUPDR_PULLDOWN(GPIOD_SWITCH1) |     \
                                     PIN_PUPDR_FLOATING(GPIOD_PWR_ON_GOPORT3) |    \
                                     PIN_PUPDR_FLOATING(GPIOD_INT_GOPORT1) |    \
                                     PIN_PUPDR_FLOATING(GPIOD_INT_GOPORT2) |     \
                                     PIN_PUPDR_FLOATING(GPIOD_INT_GOPORT3))
#define VAL_GPIOD_ODR               (PIN_ODR_HIGH(GPIOD_SPI_CS_GOPORT1) |           \
                                     PIN_ODR_HIGH(GPIOD_SPI_CS_GOPORT2) |           \
                                     PIN_ODR_HIGH(GPIOD_SPI_CS_GOPORT3) |             \
                                     PIN_ODR_LOW(GPIOD_CC_RTS) |           \
                                     PIN_ODR_LOW(GPIOD_CC_CTS) |             \
                                     PIN_ODR_LOW(GPIOD_CC_RX) |             \
                                     PIN_ODR_LOW(GPIOD_CC_TX) |           \
                                     PIN_ODR_LOW(GPIOD_PWR_ON_GOPORT1) |             \
                                     PIN_ODR_LOW(GPIOD_USART3_TX_GOPORT1) |          \
                                     PIN_ODR_LOW(GPIOD_USART3_RX_GOPORT1) |          \
                                     PIN_ODR_LOW(GPIOD_PWR_ON_GOPORT2) |          \
                                     PIN_ODR_LOW(GPIOD_SWITCH1) |           \
                                     PIN_ODR_LOW(GPIOD_PWR_ON_GOPORT3) |          \
                                     PIN_ODR_HIGH(GPIOD_INT_GOPORT1) |          \
                                     PIN_ODR_HIGH(GPIOD_INT_GOPORT2) |           \
                                     PIN_ODR_HIGH(GPIOD_INT_GOPORT3))
#define VAL_GPIOD_AFRL              (PIN_AFIO_AF(GPIOD_SPI_CS_GOPORT1, 0U) |       \
                                     PIN_AFIO_AF(GPIOD_SPI_CS_GOPORT2, 0U) |       \
                                     PIN_AFIO_AF(GPIOD_SPI_CS_GOPORT3, 0U) |          \
                                     PIN_AFIO_AF(GPIOD_CC_RTS, 7U) |       \
                                     PIN_AFIO_AF(GPIOD_CC_CTS, 7U) |          \
                                     PIN_AFIO_AF(GPIOD_CC_RX, 7U) |          \
                                     PIN_AFIO_AF(GPIOD_CC_TX, 7U) |       \
                                     PIN_AFIO_AF(GPIOD_PWR_ON_GOPORT1, 0U))
#define VAL_GPIOD_AFRH              (PIN_AFIO_AF(GPIOD_USART3_TX_GOPORT1, 7U) |      \
                                     PIN_AFIO_AF(GPIOD_USART3_RX_GOPORT1, 7U) |      \
                                     PIN_AFIO_AF(GPIOD_PWR_ON_GOPORT2, 0U) |      \
                                     PIN_AFIO_AF(GPIOD_SWITCH1, 0U) |        \
                                     PIN_AFIO_AF(GPIOD_PWR_ON_GOPORT3, 0U) |       \
                                     PIN_AFIO_AF(GPIOD_INT_GOPORT1, 0U) |       \
                                     PIN_AFIO_AF(GPIOD_INT_GOPORT2, 0U) |       \
                                     PIN_AFIO_AF(GPIOD_INT_GOPORT3, 0U))

/*
 * GPIOE setup:
 *
 * PE0  - USART8_RX_GOPORT3
 * PE1  - USART8_TX_GOPORT3
 * PE2  - SPI4_SCK
 * PE3  - EN_UART_TX_PU_GOPORT1
 * PE4  - CC_HIB
 * PE5  - D9
 * PE6  - SPI4_MOSI
 * PE7  - USART7_RX_GOPORT2
 * PE8  - USART7_TX_GOPORT2
 * PE9  - LED_GOPORT1
 * PE10 - EN_UART_TX_PU_GOPORT2
 * PE11 - LED_GOPORT2
 * PE12 - EN_UART_TX_PU_GOPORT3
 * PE13 - SPI4_MISO
 * PE14 - LED_GOPORT3
 * PE15 - CC_TCXO_EN_SOP2
 */
#define VAL_GPIOE_MODER             (PIN_MODE_ALTERNATE(GPIOE_USART8_RX_GOPORT3) |   \
                                     PIN_MODE_ALTERNATE(GPIOE_USART8_TX_GOPORT3) |   \
                                     PIN_MODE_ALTERNATE(GPIOE_SPI4_SCK) |           \
                                     PIN_MODE_ALTERNATE(GPIOE_EN_UART_TX_PU_GOPORT1) |           \
                                     PIN_MODE_OUTPUT(GPIOE_CC_HIB) |           \
                                     PIN_MODE_INPUT(GPIOE_D9) |           \
                                     PIN_MODE_ALTERNATE(GPIOE_SPI4_MOSI) |           \
                                     PIN_MODE_ALTERNATE(GPIOE_USART7_RX_GOPORT2) |     \
                                     PIN_MODE_ALTERNATE(GPIOE_USART7_TX_GOPORT2) |     \
                                     PIN_MODE_OUTPUT(GPIOE_LED_GOPORT1) |     \
                                     PIN_MODE_OUTPUT(GPIOE_EN_UART_TX_PU_GOPORT2) |     \
                                     PIN_MODE_OUTPUT(GPIOE_LED_GOPORT2) |     \
                                     PIN_MODE_OUTPUT(GPIOE_EN_UART_TX_PU_GOPORT3) |     \
                                     PIN_MODE_ALTERNATE(GPIOE_SPI4_MISO) |    \
                                     PIN_MODE_OUTPUT(GPIOE_LED_GOPORT3) |    \
                                     PIN_MODE_INPUT(GPIOE_CC_TCXO_EN_SOP2))
#define VAL_GPIOE_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOE_USART8_RX_GOPORT3) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOE_USART8_TX_GOPORT3) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOE_SPI4_SCK) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOE_EN_UART_TX_PU_GOPORT1) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOE_CC_HIB) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOE_D9) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOE_SPI4_MOSI) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOE_USART7_RX_GOPORT2) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOE_USART7_TX_GOPORT2) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOE_LED_GOPORT1) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOE_EN_UART_TX_PU_GOPORT2) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOE_LED_GOPORT2) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOE_EN_UART_TX_PU_GOPORT3) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOE_SPI4_MISO) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOE_LED_GOPORT3) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOE_CC_TCXO_EN_SOP2))
#define VAL_GPIOE_OSPEEDR           (PIN_OSPEED_HIGH(GPIOE_USART8_RX_GOPORT3) |      \
                                     PIN_OSPEED_HIGH(GPIOE_USART8_TX_GOPORT3) |      \
                                     PIN_OSPEED_HIGH(GPIOE_SPI4_SCK) |       \
                                     PIN_OSPEED_HIGH(GPIOE_EN_UART_TX_PU_GOPORT1) |       \
                                     PIN_OSPEED_HIGH(GPIOE_CC_HIB) |       \
                                     PIN_OSPEED_HIGH(GPIOE_D9) |       \
                                     PIN_OSPEED_HIGH(GPIOE_SPI4_MOSI) |       \
                                     PIN_OSPEED_HIGH(GPIOE_USART7_RX_GOPORT2) |        \
                                     PIN_OSPEED_HIGH(GPIOE_USART7_TX_GOPORT2) |        \
                                     PIN_OSPEED_HIGH(GPIOE_LED_GOPORT1) |        \
                                     PIN_OSPEED_HIGH(GPIOE_EN_UART_TX_PU_GOPORT2) |        \
                                     PIN_OSPEED_HIGH(GPIOE_LED_GOPORT2) |        \
                                     PIN_OSPEED_HIGH(GPIOE_EN_UART_TX_PU_GOPORT3) |        \
                                     PIN_OSPEED_HIGH(GPIOE_SPI4_MISO) |       \
                                     PIN_OSPEED_HIGH(GPIOE_LED_GOPORT3) |       \
                                     PIN_OSPEED_HIGH(GPIOE_CC_TCXO_EN_SOP2))
#define VAL_GPIOE_PUPDR             (PIN_PUPDR_FLOATING(GPIOE_USART8_RX_GOPORT3) |   \
                                     PIN_PUPDR_FLOATING(GPIOE_USART8_TX_GOPORT3) |   \
                                     PIN_PUPDR_FLOATING(GPIOE_SPI4_SCK) |         \
                                     PIN_PUPDR_PULLDOWN(GPIOE_EN_UART_TX_PU_GOPORT1) |         \
                                     PIN_PUPDR_PULLDOWN(GPIOE_CC_HIB) |         \
                                     PIN_PUPDR_FLOATING(GPIOE_D9) |         \
                                     PIN_PUPDR_FLOATING(GPIOE_SPI4_MOSI) |         \
                                     PIN_PUPDR_FLOATING(GPIOE_USART7_RX_GOPORT2) |     \
                                     PIN_PUPDR_FLOATING(GPIOE_USART7_TX_GOPORT2) |     \
                                     PIN_PUPDR_FLOATING(GPIOE_LED_GOPORT1) |     \
                                     PIN_PUPDR_PULLDOWN(GPIOE_EN_UART_TX_PU_GOPORT2) |     \
                                     PIN_PUPDR_FLOATING(GPIOE_LED_GOPORT2) |     \
                                     PIN_PUPDR_PULLDOWN(GPIOE_EN_UART_TX_PU_GOPORT3) |     \
                                     PIN_PUPDR_FLOATING(GPIOE_SPI4_MISO) |    \
                                     PIN_PUPDR_FLOATING(GPIOE_LED_GOPORT3) |    \
                                     PIN_PUPDR_FLOATING(GPIOE_CC_TCXO_EN_SOP2))
#define VAL_GPIOE_ODR               (PIN_ODR_LOW(GPIOE_USART8_RX_GOPORT3) |         \
                                     PIN_ODR_LOW(GPIOE_USART8_TX_GOPORT3) |         \
                                     PIN_ODR_LOW(GPIOE_SPI4_SCK) |             \
                                     PIN_ODR_LOW(GPIOE_EN_UART_TX_PU_GOPORT1) |             \
                                     PIN_ODR_LOW(GPIOE_CC_HIB) |             \
                                     PIN_ODR_LOW(GPIOE_D9) |             \
                                     PIN_ODR_LOW(GPIOE_SPI4_MOSI) |             \
                                     PIN_ODR_LOW(GPIOE_USART7_RX_GOPORT2) |           \
                                     PIN_ODR_LOW(GPIOE_USART7_TX_GOPORT2) |           \
                                     PIN_ODR_LOW(GPIOE_LED_GOPORT1) |           \
                                     PIN_ODR_LOW(GPIOE_EN_UART_TX_PU_GOPORT2) |           \
                                     PIN_ODR_LOW(GPIOE_LED_GOPORT2) |           \
                                     PIN_ODR_LOW(GPIOE_EN_UART_TX_PU_GOPORT3) |           \
                                     PIN_ODR_LOW(GPIOE_SPI4_MISO) |          \
                                     PIN_ODR_LOW(GPIOE_LED_GOPORT3) |          \
                                     PIN_ODR_LOW(GPIOE_CC_TCXO_EN_SOP2))
#define VAL_GPIOE_AFRL              (PIN_AFIO_AF(GPIOE_USART8_RX_GOPORT3, 8U) |     \
                                     PIN_AFIO_AF(GPIOE_USART8_TX_GOPORT3, 8U) |     \
                                     PIN_AFIO_AF(GPIOE_SPI4_SCK, 5U) |          \
                                     PIN_AFIO_AF(GPIOE_EN_UART_TX_PU_GOPORT1, 0U) |          \
                                     PIN_AFIO_AF(GPIOE_CC_HIB, 0U) |          \
                                     PIN_AFIO_AF(GPIOE_D9, 0U) |          \
                                     PIN_AFIO_AF(GPIOE_SPI4_MOSI, 5U) |          \
                                     PIN_AFIO_AF(GPIOE_USART7_RX_GOPORT2, 8U))
#define VAL_GPIOE_AFRH              (PIN_AFIO_AF(GPIOE_USART7_TX_GOPORT2, 8U) |       \
                                     PIN_AFIO_AF(GPIOE_LED_GOPORT1, 0U) |       \
                                     PIN_AFIO_AF(GPIOE_EN_UART_TX_PU_GOPORT2, 0U) |       \
                                     PIN_AFIO_AF(GPIOE_LED_GOPORT2, 0U) |       \
                                     PIN_AFIO_AF(GPIOE_EN_UART_TX_PU_GOPORT3, 0U) |       \
                                     PIN_AFIO_AF(GPIOE_SPI4_MISO, 0U) |      \
                                     PIN_AFIO_AF(GPIOE_LED_GOPORT3, 0U) |      \
                                     PIN_AFIO_AF(GPIOE_CC_TCXO_EN_SOP2, 0U))


/*
 * GPIOH setup:
 *
 * PH0  - OSC_IN
 * PH1  - OSC_OUT
 * PH2  - PIN2
 * PH3  - PIN3
 * PH4  - PIN4
 * PH5  - PIN5
 * PH6  - PIN6
 * PH7  - PIN7
 * PH8  - PIN8
 * PH9  - PIN9
 * PH10 - PIN10
 * PH11 - PIN11
 * PH12 - PIN12
 * PH13 - PIN13
 * PH14 - PIN14
 * PH15 - PIN15
 */
#define VAL_GPIOH_MODER             (PIN_MODE_INPUT(GPIOH_OSC_IN) |         \
                                     PIN_MODE_INPUT(GPIOH_OSC_OUT) |        \
                                     PIN_MODE_INPUT(GPIOH_PIN2) |           \
                                     PIN_MODE_INPUT(GPIOH_PIN3) |           \
                                     PIN_MODE_INPUT(GPIOH_PIN4) |           \
                                     PIN_MODE_INPUT(GPIOH_PIN5) |           \
                                     PIN_MODE_INPUT(GPIOH_PIN6) |           \
                                     PIN_MODE_INPUT(GPIOH_PIN7) |           \
                                     PIN_MODE_INPUT(GPIOH_PIN8) |           \
                                     PIN_MODE_INPUT(GPIOH_PIN9) |           \
                                     PIN_MODE_INPUT(GPIOH_PIN10) |          \
                                     PIN_MODE_INPUT(GPIOH_PIN11) |          \
                                     PIN_MODE_INPUT(GPIOH_PIN12) |          \
                                     PIN_MODE_INPUT(GPIOH_PIN13) |          \
                                     PIN_MODE_INPUT(GPIOH_PIN14) |          \
                                     PIN_MODE_INPUT(GPIOH_PIN15))
#define VAL_GPIOH_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOH_OSC_IN) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOH_OSC_OUT) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN2) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN3) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN4) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN5) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN6) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN7) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN8) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN9) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN10) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN11) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN12) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN13) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN14) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN15))
#define VAL_GPIOH_OSPEEDR           (PIN_OSPEED_HIGH(GPIOH_OSC_IN) |        \
                                     PIN_OSPEED_HIGH(GPIOH_OSC_OUT) |       \
                                     PIN_OSPEED_VERYLOW(GPIOH_PIN2) |       \
                                     PIN_OSPEED_VERYLOW(GPIOH_PIN3) |       \
                                     PIN_OSPEED_VERYLOW(GPIOH_PIN4) |       \
                                     PIN_OSPEED_VERYLOW(GPIOH_PIN5) |       \
                                     PIN_OSPEED_VERYLOW(GPIOH_PIN6) |       \
                                     PIN_OSPEED_VERYLOW(GPIOH_PIN7) |       \
                                     PIN_OSPEED_VERYLOW(GPIOH_PIN8) |       \
                                     PIN_OSPEED_VERYLOW(GPIOH_PIN9) |       \
                                     PIN_OSPEED_VERYLOW(GPIOH_PIN10) |      \
                                     PIN_OSPEED_VERYLOW(GPIOH_PIN11) |      \
                                     PIN_OSPEED_VERYLOW(GPIOH_PIN12) |      \
                                     PIN_OSPEED_VERYLOW(GPIOH_PIN13) |      \
                                     PIN_OSPEED_VERYLOW(GPIOH_PIN14) |      \
                                     PIN_OSPEED_VERYLOW(GPIOH_PIN15))
#define VAL_GPIOH_PUPDR             (PIN_PUPDR_FLOATING(GPIOH_OSC_IN) |     \
                                     PIN_PUPDR_FLOATING(GPIOH_OSC_OUT) |    \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN2) |         \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN3) |         \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN4) |         \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN5) |         \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN6) |         \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN7) |         \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN8) |         \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN9) |         \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN10) |        \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN11) |        \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN12) |        \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN13) |        \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN14) |        \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN15))
#define VAL_GPIOH_ODR               (PIN_ODR_HIGH(GPIOH_OSC_IN) |           \
                                     PIN_ODR_HIGH(GPIOH_OSC_OUT) |          \
                                     PIN_ODR_HIGH(GPIOH_PIN2) |             \
                                     PIN_ODR_HIGH(GPIOH_PIN3) |             \
                                     PIN_ODR_HIGH(GPIOH_PIN4) |             \
                                     PIN_ODR_HIGH(GPIOH_PIN5) |             \
                                     PIN_ODR_HIGH(GPIOH_PIN6) |             \
                                     PIN_ODR_HIGH(GPIOH_PIN7) |             \
                                     PIN_ODR_HIGH(GPIOH_PIN8) |             \
                                     PIN_ODR_HIGH(GPIOH_PIN9) |             \
                                     PIN_ODR_HIGH(GPIOH_PIN10) |            \
                                     PIN_ODR_HIGH(GPIOH_PIN11) |            \
                                     PIN_ODR_HIGH(GPIOH_PIN12) |            \
                                     PIN_ODR_HIGH(GPIOH_PIN13) |            \
                                     PIN_ODR_HIGH(GPIOH_PIN14) |            \
                                     PIN_ODR_HIGH(GPIOH_PIN15))
#define VAL_GPIOH_AFRL              (PIN_AFIO_AF(GPIOH_OSC_IN, 0U) |        \
                                     PIN_AFIO_AF(GPIOH_OSC_OUT, 0U) |       \
                                     PIN_AFIO_AF(GPIOH_PIN2, 0U) |          \
                                     PIN_AFIO_AF(GPIOH_PIN3, 0U) |          \
                                     PIN_AFIO_AF(GPIOH_PIN4, 0U) |          \
                                     PIN_AFIO_AF(GPIOH_PIN5, 0U) |          \
                                     PIN_AFIO_AF(GPIOH_PIN6, 0U) |          \
                                     PIN_AFIO_AF(GPIOH_PIN7, 0U))
#define VAL_GPIOH_AFRH              (PIN_AFIO_AF(GPIOH_PIN8, 0U) |          \
                                     PIN_AFIO_AF(GPIOH_PIN9, 0U) |          \
                                     PIN_AFIO_AF(GPIOH_PIN10, 0U) |         \
                                     PIN_AFIO_AF(GPIOH_PIN11, 0U) |         \
                                     PIN_AFIO_AF(GPIOH_PIN12, 0U) |         \
                                     PIN_AFIO_AF(GPIOH_PIN13, 0U) |         \
                                     PIN_AFIO_AF(GPIOH_PIN14, 0U) |         \
                                     PIN_AFIO_AF(GPIOH_PIN15, 0U))


#if !defined(_FROM_ASM_)
#ifdef __cplusplus
extern "C" {
#endif
  void boardInit(void);
#ifdef __cplusplus
}
#endif
#endif /* _FROM_ASM_ */

#endif /* BOARD_H */
