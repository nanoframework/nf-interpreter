//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef BOARD_H
#define BOARD_H

/*
 * Board identifier.
 */
#define BOARD_MBN_QUAIL
#define BOARD_NAME                  "Mikrobus.Net QUAIL"

#define BOARD_OTG_NOVBUSSENS

/*
 * Board oscillators-related settings.
 * NOTE: LSE not fitted.
 */
#if !defined(STM32_LSECLK)
#define STM32_LSECLK                32768U
#endif

#if !defined(STM32_HSECLK)
#define STM32_HSECLK                12000000U
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
#define GPIOA_PA0                	0U
#define GPIOA_INT1             		1U
#define GPIOA_RST1             		2U
#define GPIOA_CS1                	3U
#define GPIOA_AN2             		4U
#define GPIOA_AN4                  	5U
#define GPIOA_AN1                	6U
#define GPIOA_AN3               	7U
#define GPIOA_FLASH_HOLD            8U
#define GPIOA_TX4               	9U
#define GPIOA_RX4               	10U
#define GPIOA_OTG_FS_DM             11U
#define GPIOA_OTG_FS_DP             12U
#define GPIOA_FLASH_CS              13U
#define GPIOA_INT4                 	14U
#define GPIOA_GPIO1                	15U

#define GPIOB_GPIO2                	0U
#define GPIOB_PB1                	1U
#define GPIOB_BOOT1                 2U
#define GPIOB_SPI1_CLK              3U
#define GPIOB_SPI1_MISO             4U
#define GPIOB_SPI1_MOSI            	5U
#define GPIOB_I2C1_SCL             	6U
#define GPIOB_I2C1_SDA              7U
#define GPIOB_PB8                	8U
#define GPIOB_INT2                	9U
#define GPIOB_GPIO3                	10U
#define GPIOB_GPIO4                	11U
#define GPIOB_GPIO5             	12U
#define GPIOB_GPIO6           		13U
#define GPIOB_OTG_HS_DM             14U
#define GPIOB_OTG_HS_DP             15U

#define GPIOC_ID0             		0U
#define GPIOC_ID1          			1U
#define GPIOC_ID2           		2U
#define GPIOC_LED3                  3U
#define GPIOC_GPIO7            		4U
#define GPIOC_GPIO8             	5U
#define GPIOC_TX3             		6U
#define GPIOC_RX3                	7U
#define GPIOC_INT3                  8U
#define GPIOC_FLASH_WP              9U
#define GPIOC_SPI3_CLK              10U
#define GPIOC_SPI3_MISO             11U
#define GPIOC_SPI3_MOSI             12U
#define GPIOC_GPIO9                 13U
#define GPIOC_OSC32_IN              14U
#define GPIOC_OSC32_OUT             15U

#define GPIOD_RST4                	0U
#define GPIOD_CS4                	1U
#define GPIOD_GPIO10                2U
#define GPIOD_GPIO11                3U
#define GPIOD_GPIO12                4U
#define GPIOD_TX2                  	5U
#define GPIOD_RX2                	6U
#define GPIOD_GPIO13                7U
#define GPIOD_TX1               	8U
#define GPIOD_RX1               	9U
#define GPIOD_GPIO14               	10U
#define GPIOD_CS3                	11U
#define GPIOD_RST3               	12U
#define GPIOD_PWM3               	13U
#define GPIOD_PWM4                	14U
#define GPIOD_PWM2                	15U

#define GPIOE_CS2              		0U
#define GPIOE_RST2              	1U
#define GPIOE_GPIO15                2U
#define GPIOE_GPIO16                3U
#define GPIOE_GPIO17                4U
#define GPIOE_GPIO18                5U
#define GPIOE_GPIO19                6U
#define GPIOE_GPIO20                7U
#define GPIOE_GPIO21                8U
#define GPIOE_PWM1                	9U
#define GPIOE_LED2                	10U
#define GPIOE_GPIO22                11U
#define GPIOE_USB_DETECT            12U
#define GPIOE_GPIO23               	13U
#define GPIOE_GPIO24               	14U
#define GPIOE_LED1               	15U

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
 

 // Christophe : what are those "lines" used for ?
 
#define LINE_INT1              		PAL_LINE(GPIOA, 1U)
#define LINE_RST1              		PAL_LINE(GPIOA, 2U)
#define LINE_CS1                 	PAL_LINE(GPIOA, 3U)
#define LINE_AN2              		PAL_LINE(GPIOA, 4U)
#define LINE_AN4              		PAL_LINE(GPIOA, 5U)
#define LINE_AN1                 	PAL_LINE(GPIOA, 6U)
#define LINE_AN3                	PAL_LINE(GPIOA, 7U)

// The ANx lines are used by the microSD click board to detect the MicroSD
// _AN1 is when using socket 1
// _AN2 is when using socket 2
// _AN3 is when using socket 3
// _AN4 is when using socket 4
// Whatever socket is used the LINE_SD_DETECT has to match
// and the GPIO configs needs to be adjusted
//   - PIN_MODE has to be set to Input
//   - PIN_ODR has to be set to High
//   - PIN_PUPDR has to be set to Floating
//   - PIN_OTYPE has to be set to Pushpull
//   - PIN_OSPEED has to be set to High
//   - PIN_AFIO has to be set to 0
#define LINE_SD_DETECT             PAL_LINE(GPIOA, 6U)

#define LINE_FLASH_HOLD             PAL_LINE(GPIOA, 8U)
#define LINE_TX4                	PAL_LINE(GPIOA, 9U)
#define LINE_RX4                	PAL_LINE(GPIOA, 10U)
#define LINE_OTG_FS_DM              PAL_LINE(GPIOA, 11U)
#define LINE_OTG_FS_DP              PAL_LINE(GPIOA, 12U)
#define LINE_FLASH_CS               PAL_LINE(GPIOA, 13U)
#define LINE_INT4                  	PAL_LINE(GPIOA, 14U)
#define LINE_GPIO1                 	PAL_LINE(GPIOA, 15U)

#define LINE_GPIO2                 	PAL_LINE(GPIOB, 0U)
#define LINE_PB1                 	PAL_LINE(GPIOB, 1U)
#define LINE_BOOT1                  PAL_LINE(GPIOB, 2U)
#define LINE_SPI1_CLK               PAL_LINE(GPIOB, 3U)
#define LINE_SPI1_MISO              PAL_LINE(GPIOB, 4U)
#define LINE_SPI1_MOSI             	PAL_LINE(GPIOB, 5U)
#define LINE_I2C1_SCL              	PAL_LINE(GPIOB, 6U)
#define LINE_I2C1_SDA              	PAL_LINE(GPIOB, 7U)
#define LINE_PB8                 	PAL_LINE(GPIOB, 8U)
#define LINE_INT2                 	PAL_LINE(GPIOB, 9U)
#define LINE_GPIO3                 	PAL_LINE(GPIOB, 10U)
#define LINE_GPIO4                 	PAL_LINE(GPIOB, 11U)
#define LINE_GPIO5              	PAL_LINE(GPIOB, 12U)
#define LINE_GPIO6            		PAL_LINE(GPIOB, 13U)
#define LINE_OTG_HS_DM              PAL_LINE(GPIOB, 14U)
#define LINE_OTG_HS_DP              PAL_LINE(GPIOB, 15U)

#define LINE_ID0              		PAL_LINE(GPIOC, 0U)
#define LINE_ID1           			PAL_LINE(GPIOC, 1U)
#define LINE_ID2            		PAL_LINE(GPIOC, 2U)
#define LINE_LED3		            PAL_LINE(GPIOC, 3U)
#define LINE_GPIO7             		PAL_LINE(GPIOC, 4U)
#define LINE_GPIO8              	PAL_LINE(GPIOC, 5U)
#define LINE_TX3              		PAL_LINE(GPIOC, 6U)
#define LINE_RX3                 	PAL_LINE(GPIOC, 7U)
#define LINE_INT3                 	PAL_LINE(GPIOC, 8U)
#define LINE_FLASH_WP               PAL_LINE(GPIOC, 9U)
#define LINE_SPI3_CLK               PAL_LINE(GPIOC, 10U)
#define LINE_SPI3_MISO              PAL_LINE(GPIOC, 11U)
#define LINE_SPI3_MOSI              PAL_LINE(GPIOC, 12U)
#define LINE_GPIO9        		    PAL_LINE(GPIOC, 13U)
#define LINE_OSC32_IN               PAL_LINE(GPIOC, 14U)
#define LINE_OSC32_OUT              PAL_LINE(GPIOC, 15U)

#define LINE_RST4                 	PAL_LINE(GPIOD, 0U)
#define LINE_CS4                 	PAL_LINE(GPIOD, 1U)
#define LINE_CS5                 	PAL_LINE(GPIOD, 2U)
#define LINE_GPIO11                 PAL_LINE(GPIOD, 3U)
#define LINE_GPIO12                 PAL_LINE(GPIOD, 4U)
#define LINE_CS6                 	PAL_LINE(GPIOD, 5U)
#define LINE_RX2                 	PAL_LINE(GPIOD, 6U)
#define LINE_CS7                 	PAL_LINE(GPIOD, 7U)
#define LINE_TX1                	PAL_LINE(GPIOD, 8U)
#define LINE_RX1                	PAL_LINE(GPIOD, 9U)
#define LINE_GPIO14                	PAL_LINE(GPIOD, 10U)
#define LINE_CS3                 	PAL_LINE(GPIOD, 11U)
#define LINE_RST3                	PAL_LINE(GPIOD, 12U)
#define LINE_PWM3                	PAL_LINE(GPIOD, 13U)
#define LINE_PWM4                 	PAL_LINE(GPIOD, 14U)
#define LINE_PWM2                 	PAL_LINE(GPIOD, 15U)

#define LINE_CS2               		PAL_LINE(GPIOE, 0U)
#define LINE_RST2               	PAL_LINE(GPIOE, 1U)
#define LINE_GPIO15            		PAL_LINE(GPIOE, 2U)
#define LINE_GPIO16            		PAL_LINE(GPIOE, 3U)
#define LINE_GPIO17            		PAL_LINE(GPIOE, 4U)
#define LINE_GPIO18            		PAL_LINE(GPIOE, 5U)
#define LINE_GPIO19            		PAL_LINE(GPIOE, 6U)
#define LINE_GPIO20                 PAL_LINE(GPIOE, 7U)
#define LINE_GPIO21                 PAL_LINE(GPIOE, 8U)
#define LINE_PWM1                 	PAL_LINE(GPIOE, 9U)
#define LINE_LED2	                PAL_LINE(GPIOE, 10U)
#define LINE_GPIO22                 PAL_LINE(GPIOE, 11U)
#define LINE_USB_DETECT             PAL_LINE(GPIOE, 12U)
#define LINE_GPIO23                	PAL_LINE(GPIOE, 13U)
#define LINE_GPIO24                	PAL_LINE(GPIOE, 14U)
#define LINE_LED1	                PAL_LINE(GPIOE, 15U)

#define LINE_OSC_IN                 PAL_LINE(GPIOH, 0U)
#define LINE_OSC_OUT                PAL_LINE(GPIOH, 1U)


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
 * PA0  - N/A
 * PA1  - INT1                 (input floating).
 * PA2  - RST1                 (input floating).
 * PA3  - CS1                    (alternate 14).
 * PA4  - AN2                 (alternate 14).
 * PA5  - AN5                      (input pullup).
 * PA6  - AN1                    (input floating).
 * PA7  - AN3                   (input pullup).
 * PA8  - FLASH_HOLD                  (alternate 4).
 * PA9  - TX4                   (alternate 7).
 * PA10 - RX4                   (alternate 7).
 * PA11 - OTG_FS_DM                    (alternate 14).
 * PA12 - OTG_FS_DP                    (alternate 14).
 * PA13 - FLASH_CS                     (alternate 0).
 * PA14 - INT4                     (alternate 0).
 * PA15 - GPIO1                    (input floating).
 */
#define VAL_GPIOA_MODER             (PIN_MODE_ALTERNATE(GPIOA_PA0) |         	\
                                     PIN_MODE_INPUT(GPIOA_INT1) |      		\
                                     PIN_MODE_INPUT(GPIOA_RST1) |      		\
                                     PIN_MODE_OUTPUT(GPIOA_CS1) |     		\
                                     PIN_MODE_ANALOG(GPIOA_AN2) |  			\
                                     PIN_MODE_ANALOG(GPIOA_AN4) |           \
                                     PIN_MODE_INPUT(GPIOA_AN1) |     		\
                                     PIN_MODE_ANALOG(GPIOA_AN3) |        	\
                                     PIN_MODE_OUTPUT(GPIOA_FLASH_HOLD) |   	\
                                     PIN_MODE_ALTERNATE(GPIOA_TX4) |    	\
                                     PIN_MODE_ALTERNATE(GPIOA_RX4) |    	\
                                     PIN_MODE_ALTERNATE(GPIOA_OTG_FS_DM) |  \
                                     PIN_MODE_ALTERNATE(GPIOA_OTG_FS_DP) |  \
                                     PIN_MODE_OUTPUT(GPIOA_FLASH_CS) |      \
                                     PIN_MODE_INPUT(GPIOA_INT4) |      		\
                                     PIN_MODE_INPUT(GPIOA_GPIO1))
#define VAL_GPIOA_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOA_PA0) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOA_INT1) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOA_RST1) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOA_CS1) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOA_AN2) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOA_AN4) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOA_AN1) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOA_AN3) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOA_FLASH_HOLD) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOA_TX4) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOA_RX4) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOA_OTG_FS_DM) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOA_OTG_FS_DP) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOA_FLASH_CS) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOA_INT4) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOA_GPIO1))
#define VAL_GPIOA_OSPEEDR           (PIN_OSPEED_HIGH(GPIOA_PA0) |     \
                                     PIN_OSPEED_HIGH(GPIOA_INT1) |  \
                                     PIN_OSPEED_HIGH(GPIOA_RST1) |  \
                                     PIN_OSPEED_HIGH(GPIOA_CS1) |        \
                                     PIN_OSPEED_HIGH(GPIOA_AN2) |     \
                                     PIN_OSPEED_HIGH(GPIOA_AN4) |       \
                                     PIN_OSPEED_HIGH(GPIOA_AN1) |        \
                                     PIN_OSPEED_HIGH(GPIOA_AN3) |    \
                                     PIN_OSPEED_HIGH(GPIOA_FLASH_HOLD) |      \
                                     PIN_OSPEED_HIGH(GPIOA_TX4) |    \
                                     PIN_OSPEED_HIGH(GPIOA_RX4) |    \
                                     PIN_OSPEED_HIGH(GPIOA_OTG_FS_DM) |        \
                                     PIN_OSPEED_HIGH(GPIOA_OTG_FS_DP) |        \
                                     PIN_OSPEED_HIGH(GPIOA_FLASH_CS) |         \
                                     PIN_OSPEED_HIGH(GPIOA_INT4) |         \
                                     PIN_OSPEED_HIGH(GPIOA_GPIO1))
#define VAL_GPIOA_PUPDR             (PIN_PUPDR_FLOATING(GPIOA_PA0) |     \
                                     PIN_PUPDR_FLOATING(GPIOA_INT1) |  \
                                     PIN_PUPDR_FLOATING(GPIOA_RST1) |  \
                                     PIN_PUPDR_FLOATING(GPIOA_CS1) |     \
                                     PIN_PUPDR_FLOATING(GPIOA_AN2) |  \
                                     PIN_PUPDR_FLOATING(GPIOA_AN4) |         \
                                     PIN_PUPDR_FLOATING(GPIOA_AN1) |     \
                                     PIN_PUPDR_FLOATING(GPIOA_AN3) |      \
                                     PIN_PUPDR_FLOATING(GPIOA_FLASH_HOLD) |   \
                                     PIN_PUPDR_FLOATING(GPIOA_TX4) |      \
                                     PIN_PUPDR_FLOATING(GPIOA_RX4) |      \
                                     PIN_PUPDR_FLOATING(GPIOA_OTG_FS_DM) |     \
                                     PIN_PUPDR_FLOATING(GPIOA_OTG_FS_DP) |     \
                                     PIN_PUPDR_FLOATING(GPIOA_FLASH_CS) |        \
                                     PIN_PUPDR_FLOATING(GPIOA_INT4) |      \
                                     PIN_PUPDR_FLOATING(GPIOA_GPIO1))
#define VAL_GPIOA_ODR               (PIN_ODR_LOW(GPIOA_PA0) |           \
                                     PIN_ODR_LOW(GPIOA_INT1) |        \
                                     PIN_ODR_LOW(GPIOA_RST1) |        \
                                     PIN_ODR_LOW(GPIOA_CS1) |           \
                                     PIN_ODR_LOW(GPIOA_AN2) |        \
                                     PIN_ODR_LOW(GPIOA_AN4) |             \
                                     PIN_ODR_HIGH(GPIOA_AN1) |           \
                                     PIN_ODR_LOW(GPIOA_AN3) |          \
                                     PIN_ODR_HIGH(GPIOA_FLASH_HOLD) |         \
                                     PIN_ODR_LOW(GPIOA_TX4) |          \
                                     PIN_ODR_LOW(GPIOA_RX4) |          \
                                     PIN_ODR_HIGH(GPIOA_OTG_FS_DM) |           \
                                     PIN_ODR_HIGH(GPIOA_OTG_FS_DP) |           \
                                     PIN_ODR_HIGH(GPIOA_FLASH_CS) |            \
                                     PIN_ODR_LOW(GPIOA_INT4) |            \
                                     PIN_ODR_LOW(GPIOA_GPIO1))
#define VAL_GPIOA_AFRL              (PIN_AFIO_AF(GPIOA_PA0, 0U) |        \
                                     PIN_AFIO_AF(GPIOA_INT1, 0U) |     \
                                     PIN_AFIO_AF(GPIOA_RST1, 0U) |     \
                                     PIN_AFIO_AF(GPIOA_CS1, 0U) |       \
                                     PIN_AFIO_AF(GPIOA_AN2, 0U) |    \
                                     PIN_AFIO_AF(GPIOA_AN4, 0U) |          \
                                     PIN_AFIO_AF(GPIOA_AN1, 0U) |       \
                                     PIN_AFIO_AF(GPIOA_AN3, 0U))
#define VAL_GPIOA_AFRH              (PIN_AFIO_AF(GPIOA_FLASH_HOLD, 0U) |      \
                                     PIN_AFIO_AF(GPIOA_TX4, 7U) |       \
                                     PIN_AFIO_AF(GPIOA_RX4, 7U) |       \
                                     PIN_AFIO_AF(GPIOA_OTG_FS_DM, 10U) |       \
                                     PIN_AFIO_AF(GPIOA_OTG_FS_DP, 10U) |       \
                                     PIN_AFIO_AF(GPIOA_FLASH_CS, 0U) |         \
                                     PIN_AFIO_AF(GPIOA_INT4, 0U) |         \
                                     PIN_AFIO_AF(GPIOA_GPIO1, 0U))

/*
 * GPIOB setup:
 *
 * PB0  - GPIO2                    (alternate 14).
 * PB1  - PB1                    (alternate 14).
 * PB2  - BOOT1                     (input pullup).
 * PB3  - SPI1_CLK                       (alternate 0).
 * PB4  - SPI1_MISO                      (input pullup).
 * PB5  - SPI1_MOSI                (alternate 12).
 * PB6  - I2C1_SCL                 (alternate 12).
 * PB7  - PIN7                      (input pullup).
 * PB8  - PB8                    (alternate 14).
 * PB9  - INT2                    (alternate 14).
 * PB10 - GPIO3                    (alternate 14).
 * PB11 - GPIO4                    (alternate 14).
 * PB12 - GPIO5                 (alternate 12).
 * PB13 - GPIO6               (input pulldown).
 * PB14 - OTG_HS_DM                 (alternate 12).
 * PB15 - OTG_HS_DP                 (alternate 12).
 */
#define VAL_GPIOB_MODER             (PIN_MODE_INPUT(GPIOB_GPIO2) |     \
                                     PIN_MODE_ALTERNATE(GPIOB_PB1) |     \
                                     PIN_MODE_ALTERNATE(GPIOB_BOOT1) |          \
                                     PIN_MODE_ALTERNATE(GPIOB_SPI1_CLK) |        \
                                     PIN_MODE_ALTERNATE(GPIOB_SPI1_MISO) |           \
                                     PIN_MODE_ALTERNATE(GPIOB_SPI1_MOSI) | \
                                     PIN_MODE_ALTERNATE(GPIOB_I2C1_SCL) |  \
                                     PIN_MODE_ALTERNATE(GPIOB_I2C1_SDA) |           \
                                     PIN_MODE_INPUT(GPIOB_PB8) |     \
                                     PIN_MODE_INPUT(GPIOB_INT2) |     \
                                     PIN_MODE_INPUT(GPIOB_GPIO3) |     \
                                     PIN_MODE_INPUT(GPIOB_GPIO4) |     \
                                     PIN_MODE_INPUT(GPIOB_GPIO5) |  \
                                     PIN_MODE_INPUT(GPIOB_GPIO6) |    \
                                     PIN_MODE_ALTERNATE(GPIOB_OTG_HS_DM) |  \
                                     PIN_MODE_ALTERNATE(GPIOB_OTG_HS_DP))
#define VAL_GPIOB_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOB_GPIO2) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOB_PB1) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOB_BOOT1) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOB_SPI1_CLK) |        \
                                     PIN_OTYPE_PUSHPULL(GPIOB_SPI1_MISO) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOB_SPI1_MOSI) | \
                                     PIN_OTYPE_OPENDRAIN(GPIOB_I2C1_SCL) |  \
                                     PIN_OTYPE_OPENDRAIN(GPIOB_I2C1_SDA) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOB_PB8) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOB_INT2) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOB_GPIO3) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOB_GPIO4) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOB_GPIO5) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOB_GPIO6) |\
                                     PIN_OTYPE_PUSHPULL(GPIOB_OTG_HS_DM) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOB_OTG_HS_DP))
#define VAL_GPIOB_OSPEEDR           (PIN_OSPEED_HIGH(GPIOB_GPIO2) |        \
                                     PIN_OSPEED_HIGH(GPIOB_PB1) |        \
                                     PIN_OSPEED_HIGH(GPIOB_BOOT1) |         \
                                     PIN_OSPEED_HIGH(GPIOB_SPI1_CLK) |           \
                                     PIN_OSPEED_HIGH(GPIOB_SPI1_MISO) |       \
                                     PIN_OSPEED_HIGH(GPIOB_SPI1_MOSI) |    \
                                     PIN_OSPEED_HIGH(GPIOB_I2C1_SCL) |     \
                                     PIN_OSPEED_HIGH(GPIOB_I2C1_SDA) |       \
                                     PIN_OSPEED_HIGH(GPIOB_PB8) |        \
                                     PIN_OSPEED_HIGH(GPIOB_INT2) |        \
                                     PIN_OSPEED_HIGH(GPIOB_GPIO3) |        \
                                     PIN_OSPEED_HIGH(GPIOB_GPIO4) |        \
                                     PIN_OSPEED_HIGH(GPIOB_GPIO5) |     \
                                     PIN_OSPEED_HIGH(GPIOB_GPIO6) |\
                                     PIN_OSPEED_HIGH(GPIOB_OTG_HS_DM) |     \
                                     PIN_OSPEED_HIGH(GPIOB_OTG_HS_DP))
#define VAL_GPIOB_PUPDR             (PIN_PUPDR_FLOATING(GPIOB_GPIO2) |     \
                                     PIN_PUPDR_FLOATING(GPIOB_PB1) |     \
                                     PIN_PUPDR_FLOATING(GPIOB_BOOT1) |        \
                                     PIN_PUPDR_FLOATING(GPIOB_SPI1_CLK) |        \
                                     PIN_PUPDR_FLOATING(GPIOB_SPI1_MISO) |         \
                                     PIN_PUPDR_FLOATING(GPIOB_SPI1_MOSI) | \
                                     PIN_PUPDR_FLOATING(GPIOB_I2C1_SCL) |  \
                                     PIN_PUPDR_FLOATING(GPIOB_I2C1_SDA) |         \
                                     PIN_PUPDR_FLOATING(GPIOB_PB8) |     \
                                     PIN_PUPDR_FLOATING(GPIOB_INT2) |     \
                                     PIN_PUPDR_FLOATING(GPIOB_GPIO3) |     \
                                     PIN_PUPDR_FLOATING(GPIOB_GPIO4) |     \
                                     PIN_PUPDR_FLOATING(GPIOB_GPIO5) |  \
                                     PIN_PUPDR_FLOATING(GPIOB_GPIO6) |\
                                     PIN_PUPDR_FLOATING(GPIOB_OTG_HS_DM) |  \
                                     PIN_PUPDR_FLOATING(GPIOB_OTG_HS_DP))
#define VAL_GPIOB_ODR               (PIN_ODR_LOW(GPIOB_GPIO2) |           \
                                     PIN_ODR_LOW(GPIOB_PB1) |           \
                                     PIN_ODR_HIGH(GPIOB_BOOT1) |            \
                                     PIN_ODR_LOW(GPIOB_SPI1_CLK) |              \
                                     PIN_ODR_LOW(GPIOB_SPI1_MISO) |             \
                                     PIN_ODR_LOW(GPIOB_SPI1_MOSI) |       \
                                     PIN_ODR_LOW(GPIOB_I2C1_SCL) |        \
                                     PIN_ODR_LOW(GPIOB_I2C1_SDA) |             \
                                     PIN_ODR_LOW(GPIOB_PB8) |           \
                                     PIN_ODR_LOW(GPIOB_INT2) |           \
                                     PIN_ODR_LOW(GPIOB_GPIO3) |           \
                                     PIN_ODR_LOW(GPIOB_GPIO4) |           \
                                     PIN_ODR_LOW(GPIOB_GPIO5) |        \
                                     PIN_ODR_LOW(GPIOB_GPIO6) |      \
                                     PIN_ODR_LOW(GPIOB_OTG_HS_DM) |        \
                                     PIN_ODR_LOW(GPIOB_OTG_HS_DP))
#define VAL_GPIOB_AFRL              (PIN_AFIO_AF(GPIOB_GPIO2, 0U) |       \
                                     PIN_AFIO_AF(GPIOB_PB1, 0U) |       \
                                     PIN_AFIO_AF(GPIOB_BOOT1, 0U) |         \
                                     PIN_AFIO_AF(GPIOB_SPI1_CLK, 5U) |           \
                                     PIN_AFIO_AF(GPIOB_SPI1_MISO, 5U) |          \
                                     PIN_AFIO_AF(GPIOB_SPI1_MOSI, 5U) |   \
                                     PIN_AFIO_AF(GPIOB_I2C1_SCL, 4U) |    \
                                     PIN_AFIO_AF(GPIOB_I2C1_SDA, 4U))
#define VAL_GPIOB_AFRH              (PIN_AFIO_AF(GPIOB_PB8, 0U) |       \
                                     PIN_AFIO_AF(GPIOB_INT2, 0U) |       \
                                     PIN_AFIO_AF(GPIOB_GPIO3, 0U) |       \
                                     PIN_AFIO_AF(GPIOB_GPIO4, 0U) |       \
                                     PIN_AFIO_AF(GPIOB_GPIO5, 0U) |    \
                                     PIN_AFIO_AF(GPIOB_GPIO6, 0U) |   \
                                     PIN_AFIO_AF(GPIOB_OTG_HS_DM, 12U) |    \
                                     PIN_AFIO_AF(GPIOB_OTG_HS_DP, 12U))

/*
 * GPIOC setup:
 *
 * PC0  - ID1                 (alternate 12).
 * PC1  - ID1              (output pushpull maximum).
 * PC2  - ID2               (output pushpull maximum).
 * PC3  - PIN3                      (input pullup).
 * PC4  - GPIO7                (output pushpull maximum).
 * PC5  - GPIO8                 (input floating).
 * PC6  - TX3                 (alternate 14).
 * PC7  - RX3                    (alternate 14).
 * PC8  - PIN8                      (input pullup).
 * PC9  - FLASH_WP                  (alternate 4).
 * PC10 - SPI3_CLK                    (alternate 14).
 * PC11 - PIN11                     (input pullup).
 * PC12 - PIN12                     (input pullup).
 * PC13 - PIN13                     (input pullup).
 * PC14 - OSC32_IN                  (input floating).
 * PC15 - OSC32_OUT                 (input floating).
 */
#define VAL_GPIOC_MODER             (PIN_MODE_INPUT(GPIOC_ID0) |  \
                                     PIN_MODE_INPUT(GPIOC_ID1) |  \
                                     PIN_MODE_INPUT(GPIOC_ID2) |   \
                                     PIN_MODE_OUTPUT(GPIOC_LED3) |           \
                                     PIN_MODE_INPUT(GPIOC_GPIO7) |    \
                                     PIN_MODE_INPUT(GPIOC_GPIO8) |      \
                                     PIN_MODE_ALTERNATE(GPIOC_TX3) |  \
                                     PIN_MODE_ALTERNATE(GPIOC_RX3) |     \
                                     PIN_MODE_INPUT(GPIOC_INT3) |           \
                                     PIN_MODE_OUTPUT(GPIOC_FLASH_WP) |   \
                                     PIN_MODE_ALTERNATE(GPIOC_SPI3_CLK) |     \
                                     PIN_MODE_ALTERNATE(GPIOC_SPI3_MISO) |          \
                                     PIN_MODE_ALTERNATE(GPIOC_SPI3_MOSI) |          \
                                     PIN_MODE_INPUT(GPIOC_GPIO9) |          \
                                     PIN_MODE_ALTERNATE(GPIOC_OSC32_IN) |       \
                                     PIN_MODE_ALTERNATE(GPIOC_OSC32_OUT))
#define VAL_GPIOC_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOC_ID0) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOC_ID1) |\
                                     PIN_OTYPE_PUSHPULL(GPIOC_ID2) |\
                                     PIN_OTYPE_PUSHPULL(GPIOC_LED3) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOC_GPIO7) | \
                                     PIN_OTYPE_PUSHPULL(GPIOC_GPIO8) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOC_TX3) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOC_RX3) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOC_INT3) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOC_FLASH_WP) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOC_SPI3_CLK) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOC_SPI3_MISO) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOC_SPI3_MOSI) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOC_GPIO9) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOC_OSC32_IN) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOC_OSC32_OUT))
#define VAL_GPIOC_OSPEEDR           (PIN_OSPEED_HIGH(GPIOC_ID0) |     \
                                     PIN_OSPEED_HIGH(GPIOC_ID1) |  \
                                     PIN_OSPEED_HIGH(GPIOC_ID2) |   \
                                     PIN_OSPEED_HIGH(GPIOC_LED3) |       \
                                     PIN_OSPEED_HIGH(GPIOC_GPIO7) |    \
                                     PIN_OSPEED_HIGH(GPIOC_GPIO8) |     \
                                     PIN_OSPEED_HIGH(GPIOC_TX3) |     \
                                     PIN_OSPEED_HIGH(GPIOC_RX3) |        \
                                     PIN_OSPEED_HIGH(GPIOC_INT3) |       \
                                     PIN_OSPEED_HIGH(GPIOC_FLASH_WP) |      \
                                     PIN_OSPEED_HIGH(GPIOC_SPI3_CLK) |        \
                                     PIN_OSPEED_HIGH(GPIOC_SPI3_MISO) |      \
                                     PIN_OSPEED_HIGH(GPIOC_SPI3_MOSI) |      \
                                     PIN_OSPEED_HIGH(GPIOC_GPIO9) |      \
                                     PIN_OSPEED_HIGH(GPIOC_OSC32_IN) |      \
                                     PIN_OSPEED_HIGH(GPIOC_OSC32_OUT))
#define VAL_GPIOC_PUPDR             (PIN_PUPDR_FLOATING(GPIOC_ID0) |  \
                                     PIN_PUPDR_FLOATING(GPIOC_ID1) |\
                                     PIN_PUPDR_FLOATING(GPIOC_ID2) |\
                                     PIN_PUPDR_FLOATING(GPIOC_LED3) |         \
                                     PIN_PUPDR_FLOATING(GPIOC_GPIO7) | \
                                     PIN_PUPDR_FLOATING(GPIOC_GPIO8) |  \
                                     PIN_PUPDR_FLOATING(GPIOC_TX3) |  \
                                     PIN_PUPDR_FLOATING(GPIOC_RX3) |     \
                                     PIN_PUPDR_FLOATING(GPIOC_INT3) |         \
                                     PIN_PUPDR_FLOATING(GPIOC_FLASH_WP) |   \
                                     PIN_PUPDR_FLOATING(GPIOC_SPI3_CLK) |     \
                                     PIN_PUPDR_FLOATING(GPIOC_SPI3_MISO) |        \
                                     PIN_PUPDR_FLOATING(GPIOC_SPI3_MOSI) |        \
                                     PIN_PUPDR_FLOATING(GPIOC_GPIO9) |        \
                                     PIN_PUPDR_FLOATING(GPIOC_OSC32_IN) |   \
                                     PIN_PUPDR_FLOATING(GPIOC_OSC32_OUT))
#define VAL_GPIOC_ODR               (PIN_ODR_LOW(GPIOC_ID0) |        \
                                     PIN_ODR_LOW(GPIOC_ID1) |     \
                                     PIN_ODR_LOW(GPIOC_ID2) |      \
                                     PIN_ODR_LOW(GPIOC_LED3) |             \
                                     PIN_ODR_LOW(GPIOC_GPIO7) |       \
                                     PIN_ODR_LOW(GPIOC_GPIO8) |        \
                                     PIN_ODR_LOW(GPIOC_TX3) |        \
                                     PIN_ODR_LOW(GPIOC_RX3) |           \
                                     PIN_ODR_LOW(GPIOC_INT3) |             \
                                     PIN_ODR_HIGH(GPIOC_FLASH_WP) |         \
                                     PIN_ODR_LOW(GPIOC_SPI3_CLK) |           \
                                     PIN_ODR_LOW(GPIOC_SPI3_MISO) |            \
                                     PIN_ODR_LOW(GPIOC_SPI3_MOSI) |            \
                                     PIN_ODR_LOW(GPIOC_GPIO9) |            \
                                     PIN_ODR_LOW(GPIOC_OSC32_IN) |         \
                                     PIN_ODR_LOW(GPIOC_OSC32_OUT))
#define VAL_GPIOC_AFRL              (PIN_AFIO_AF(GPIOC_ID0, 0U) |    \
                                     PIN_AFIO_AF(GPIOC_ID1, 0U) |  \
                                     PIN_AFIO_AF(GPIOC_ID2, 0U) |   \
                                     PIN_AFIO_AF(GPIOC_LED3, 0U) |          \
                                     PIN_AFIO_AF(GPIOC_GPIO7, 0U) |    \
                                     PIN_AFIO_AF(GPIOC_GPIO8, 0U) |     \
                                     PIN_AFIO_AF(GPIOC_TX3, 8U) |    \
                                     PIN_AFIO_AF(GPIOC_RX3, 8U))
#define VAL_GPIOC_AFRH              (PIN_AFIO_AF(GPIOC_INT3, 0U) |          \
                                     PIN_AFIO_AF(GPIOC_FLASH_WP, 0U) |      \
                                     PIN_AFIO_AF(GPIOC_SPI3_CLK, 6U) |       \
                                     PIN_AFIO_AF(GPIOC_SPI3_MISO, 6U) |         \
                                     PIN_AFIO_AF(GPIOC_SPI3_MOSI, 6U) |         \
                                     PIN_AFIO_AF(GPIOC_GPIO9, 0U) |         \
                                     PIN_AFIO_AF(GPIOC_OSC32_IN, 0U) |      \
                                     PIN_AFIO_AF(GPIOC_OSC32_OUT, 0U))

/*
 * GPIOD setup:
 *
 * PD0  - RST4                    (alternate 12).
 * PD1  - CS4                    (alternate 12).
 * PD2  - PIN2                      (input pullup).
 * PD3  - GPIO11                    (alternate 14).
 * PD4  - GPIO12                      (input pullup).
 * PD5  - PIN5                      (input pullup).
 * PD6  - RX2                    (alternate 14).
 * PD7  - PIN7                      (input pullup).
 * PD8  - TX1                   (alternate 12).
 * PD9  - RX1                   (alternate 12).
 * PD10 - GPIO14                   (alternate 12).
 * PD11 - CS3                    (input floating).
 * PD12 - RST3                   (output pushpull maximum).
 * PD13 - PWM3                   (output pushpull maximum).
 * PD14 - PWM4                    (alternate 12).
 * PD15 - FMC_D1                    (alternate 12).
 */
#define VAL_GPIOD_MODER             (PIN_MODE_INPUT(GPIOD_RST4) |     \
                                     PIN_MODE_OUTPUT(GPIOD_CS4) |     \
                                     PIN_MODE_INPUT(GPIOD_GPIO10) |           \
                                     PIN_MODE_INPUT(GPIOD_GPIO11) |     \
                                     PIN_MODE_INPUT(GPIOD_GPIO12) |           \
                                     PIN_MODE_ALTERNATE(GPIOD_TX2) |           \
                                     PIN_MODE_ALTERNATE(GPIOD_RX2) |     \
                                     PIN_MODE_INPUT(GPIOD_GPIO13) |           \
                                     PIN_MODE_ALTERNATE(GPIOD_TX1) |    \
                                     PIN_MODE_ALTERNATE(GPIOD_RX1) |    \
                                     PIN_MODE_INPUT(GPIOD_GPIO14) |    \
                                     PIN_MODE_OUTPUT(GPIOD_CS3) |         \
                                     PIN_MODE_INPUT(GPIOD_RST3) |       \
                                     PIN_MODE_ALTERNATE(GPIOD_PWM3) |       \
                                     PIN_MODE_ALTERNATE(GPIOD_PWM4) |     \
                                     PIN_MODE_ALTERNATE(GPIOD_PWM2))
#define VAL_GPIOD_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOD_RST4) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOD_CS4) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOD_GPIO10) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOD_GPIO11) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOD_GPIO12) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOD_TX2) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOD_RX2) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOD_GPIO13) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOD_TX1) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOD_RX1) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOD_GPIO14) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOD_CS3) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOD_RST3) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOD_PWM3) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOD_PWM4) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOD_PWM2))
#define VAL_GPIOD_OSPEEDR           (PIN_OSPEED_HIGH(GPIOD_RST4) |        \
                                     PIN_OSPEED_HIGH(GPIOD_CS4) |        \
                                     PIN_OSPEED_HIGH(GPIOD_GPIO10) |       \
                                     PIN_OSPEED_HIGH(GPIOD_GPIO11) |        \
                                     PIN_OSPEED_HIGH(GPIOD_GPIO12) |       \
                                     PIN_OSPEED_HIGH(GPIOD_TX2) |       \
                                     PIN_OSPEED_HIGH(GPIOD_RX2) |        \
                                     PIN_OSPEED_HIGH(GPIOD_GPIO13) |       \
                                     PIN_OSPEED_HIGH(GPIOD_TX1) |       \
                                     PIN_OSPEED_HIGH(GPIOD_RX1) |       \
                                     PIN_OSPEED_HIGH(GPIOD_GPIO14) |       \
                                     PIN_OSPEED_HIGH(GPIOD_CS3) |        \
                                     PIN_OSPEED_HIGH(GPIOD_RST3) |       \
                                     PIN_OSPEED_HIGH(GPIOD_PWM3) |       \
                                     PIN_OSPEED_HIGH(GPIOD_PWM4) |        \
                                     PIN_OSPEED_HIGH(GPIOD_PWM2))
#define VAL_GPIOD_PUPDR             (PIN_PUPDR_FLOATING(GPIOD_RST4) |     \
                                     PIN_PUPDR_FLOATING(GPIOD_CS4) |     \
                                     PIN_PUPDR_FLOATING(GPIOD_GPIO10) |         \
                                     PIN_PUPDR_FLOATING(GPIOD_GPIO11) |     \
                                     PIN_PUPDR_FLOATING(GPIOD_GPIO12) |         \
                                     PIN_PUPDR_FLOATING(GPIOD_TX2) |         \
                                     PIN_PUPDR_FLOATING(GPIOD_RX2) |     \
                                     PIN_PUPDR_FLOATING(GPIOD_GPIO13) |         \
                                     PIN_PUPDR_FLOATING(GPIOD_TX1) |    \
                                     PIN_PUPDR_FLOATING(GPIOD_RX1) |    \
                                     PIN_PUPDR_FLOATING(GPIOD_GPIO14) |    \
                                     PIN_PUPDR_FLOATING(GPIOD_CS3) |     \
                                     PIN_PUPDR_FLOATING(GPIOD_RST3) |    \
                                     PIN_PUPDR_FLOATING(GPIOD_PWM3) |    \
                                     PIN_PUPDR_FLOATING(GPIOD_PWM4) |     \
                                     PIN_PUPDR_FLOATING(GPIOD_PWM2))
#define VAL_GPIOD_ODR               (PIN_ODR_LOW(GPIOD_RST4) |           \
                                     PIN_ODR_LOW(GPIOD_CS4) |           \
                                     PIN_ODR_LOW(GPIOD_GPIO10) |             \
                                     PIN_ODR_LOW(GPIOD_GPIO11) |           \
                                     PIN_ODR_LOW(GPIOD_GPIO12) |             \
                                     PIN_ODR_LOW(GPIOD_TX2) |             \
                                     PIN_ODR_LOW(GPIOD_RX2) |           \
                                     PIN_ODR_LOW(GPIOD_GPIO13) |             \
                                     PIN_ODR_LOW(GPIOD_TX1) |          \
                                     PIN_ODR_LOW(GPIOD_RX1) |          \
                                     PIN_ODR_LOW(GPIOD_GPIO14) |          \
                                     PIN_ODR_LOW(GPIOD_CS3) |           \
                                     PIN_ODR_LOW(GPIOD_RST3) |          \
                                     PIN_ODR_LOW(GPIOD_PWM3) |          \
                                     PIN_ODR_LOW(GPIOD_PWM4) |           \
                                     PIN_ODR_LOW(GPIOD_PWM2))
#define VAL_GPIOD_AFRL              (PIN_AFIO_AF(GPIOD_RST4, 0U) |       \
                                     PIN_AFIO_AF(GPIOD_CS4, 0U) |       \
                                     PIN_AFIO_AF(GPIOD_GPIO10, 0U) |          \
                                     PIN_AFIO_AF(GPIOD_GPIO11, 0U) |       \
                                     PIN_AFIO_AF(GPIOD_GPIO12, 0U) |          \
                                     PIN_AFIO_AF(GPIOD_TX2, 7U) |          \
                                     PIN_AFIO_AF(GPIOD_RX2, 7U) |       \
                                     PIN_AFIO_AF(GPIOD_GPIO13, 0U))
#define VAL_GPIOD_AFRH              (PIN_AFIO_AF(GPIOD_TX1, 7U) |      \
                                     PIN_AFIO_AF(GPIOD_RX1, 7U) |      \
                                     PIN_AFIO_AF(GPIOD_GPIO14, 0U) |      \
                                     PIN_AFIO_AF(GPIOD_CS3, 0U) |        \
                                     PIN_AFIO_AF(GPIOD_RST3, 0U) |       \
                                     PIN_AFIO_AF(GPIOD_PWM3, 2U) |       \
                                     PIN_AFIO_AF(GPIOD_PWM4, 2U) |       \
                                     PIN_AFIO_AF(GPIOD_PWM2, 2U))

/*
 * GPIOE setup:
 *
 * PE0  - CS2                  (alternate 12).
 * PE1  - RST2                  (alternate 12).
 * PE2  - PIN2                      (input pullup).
 * PE3  - PIN3                      (input pullup).
 * PE4  - SPI1_MISO                      (input pullup).
 * PE5  - PIN5                      (input pullup).
 * PE6  - PIN6                      (input pullup).
 * PE7  - GPIO20                    (alternate 12).
 * PE8  - GPIO21                    (alternate 12).
 * PE9  - PWM1                    (alternate 12).
 * PE10 - FMC_D7                    (alternate 12).
 * PE11 - GPIO22                    (alternate 12).
 * PE12 - USB_DETECT                    (alternate 12).
 * PE13 - GPIO23                   (alternate 12).
 * PE14 - GPIO24                   (alternate 12).
 * PE15 - FMC_D12                   (alternate 12).
 */
#define VAL_GPIOE_MODER             (PIN_MODE_OUTPUT(GPIOE_CS2) |   \
                                     PIN_MODE_INPUT(GPIOE_RST2) |   \
                                     PIN_MODE_INPUT(GPIOE_GPIO15) |           \
                                     PIN_MODE_INPUT(GPIOE_GPIO16) |           \
                                     PIN_MODE_INPUT(GPIOE_GPIO17) |           \
                                     PIN_MODE_INPUT(GPIOE_GPIO18) |           \
                                     PIN_MODE_INPUT(GPIOE_GPIO19) |           \
                                     PIN_MODE_INPUT(GPIOE_GPIO20) |     \
                                     PIN_MODE_INPUT(GPIOE_GPIO21) |     \
                                     PIN_MODE_ALTERNATE(GPIOE_PWM1) |     \
                                     PIN_MODE_OUTPUT(GPIOE_LED2) |     \
                                     PIN_MODE_INPUT(GPIOE_GPIO22) |     \
                                     PIN_MODE_ALTERNATE(GPIOE_USB_DETECT) |     \
                                     PIN_MODE_INPUT(GPIOE_GPIO23) |    \
                                     PIN_MODE_INPUT(GPIOE_GPIO24) |    \
                                     PIN_MODE_OUTPUT(GPIOE_LED1))
#define VAL_GPIOE_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOE_CS2) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOE_RST2) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOE_GPIO15) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOE_GPIO16) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOE_GPIO17) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOE_GPIO18) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOE_GPIO19) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOE_GPIO20) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOE_GPIO21) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOE_PWM1) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOE_LED2) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOE_GPIO22) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOE_USB_DETECT) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOE_GPIO23) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOE_GPIO24) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOE_LED1))
#define VAL_GPIOE_OSPEEDR           (PIN_OSPEED_HIGH(GPIOE_CS2) |      \
                                     PIN_OSPEED_HIGH(GPIOE_RST2) |      \
                                     PIN_OSPEED_HIGH(GPIOE_GPIO15) |       \
                                     PIN_OSPEED_HIGH(GPIOE_GPIO16) |       \
                                     PIN_OSPEED_HIGH(GPIOE_GPIO17) |       \
                                     PIN_OSPEED_HIGH(GPIOE_GPIO18) |       \
                                     PIN_OSPEED_HIGH(GPIOE_GPIO19) |       \
                                     PIN_OSPEED_HIGH(GPIOE_GPIO20) |        \
                                     PIN_OSPEED_HIGH(GPIOE_GPIO21) |        \
                                     PIN_OSPEED_HIGH(GPIOE_PWM1) |        \
                                     PIN_OSPEED_HIGH(GPIOE_LED2) |        \
                                     PIN_OSPEED_HIGH(GPIOE_GPIO22) |        \
                                     PIN_OSPEED_HIGH(GPIOE_USB_DETECT) |        \
                                     PIN_OSPEED_HIGH(GPIOE_GPIO23) |       \
                                     PIN_OSPEED_HIGH(GPIOE_GPIO24) |       \
                                     PIN_OSPEED_HIGH(GPIOE_LED1))
#define VAL_GPIOE_PUPDR             (PIN_PUPDR_FLOATING(GPIOE_CS2) |   \
                                     PIN_PUPDR_FLOATING(GPIOE_RST2) |   \
                                     PIN_PUPDR_FLOATING(GPIOE_GPIO15) |         \
                                     PIN_PUPDR_FLOATING(GPIOE_GPIO16) |         \
                                     PIN_PUPDR_FLOATING(GPIOE_GPIO17) |         \
                                     PIN_PUPDR_FLOATING(GPIOE_GPIO18) |         \
                                     PIN_PUPDR_FLOATING(GPIOE_GPIO19) |         \
                                     PIN_PUPDR_FLOATING(GPIOE_GPIO20) |     \
                                     PIN_PUPDR_FLOATING(GPIOE_GPIO21) |     \
                                     PIN_PUPDR_FLOATING(GPIOE_PWM1) |     \
                                     PIN_PUPDR_FLOATING(GPIOE_LED2) |     \
                                     PIN_PUPDR_FLOATING(GPIOE_GPIO22) |     \
                                     PIN_PUPDR_FLOATING(GPIOE_USB_DETECT) |     \
                                     PIN_PUPDR_FLOATING(GPIOE_GPIO23) |    \
                                     PIN_PUPDR_FLOATING(GPIOE_GPIO24) |    \
                                     PIN_PUPDR_FLOATING(GPIOE_LED1))
#define VAL_GPIOE_ODR               (PIN_ODR_LOW(GPIOE_CS2) |         \
                                     PIN_ODR_LOW(GPIOE_RST2) |         \
                                     PIN_ODR_LOW(GPIOE_GPIO15) |             \
                                     PIN_ODR_LOW(GPIOE_GPIO16) |             \
                                     PIN_ODR_LOW(GPIOE_GPIO17) |             \
                                     PIN_ODR_LOW(GPIOE_GPIO18) |             \
                                     PIN_ODR_LOW(GPIOE_GPIO19) |             \
                                     PIN_ODR_LOW(GPIOE_GPIO20) |           \
                                     PIN_ODR_LOW(GPIOE_GPIO21) |           \
                                     PIN_ODR_LOW(GPIOE_PWM1) |           \
                                     PIN_ODR_LOW(GPIOE_LED2) |           \
                                     PIN_ODR_LOW(GPIOE_GPIO22) |           \
                                     PIN_ODR_LOW(GPIOE_USB_DETECT) |           \
                                     PIN_ODR_LOW(GPIOE_GPIO23) |          \
                                     PIN_ODR_LOW(GPIOE_GPIO24) |          \
                                     PIN_ODR_LOW(GPIOE_LED1))
#define VAL_GPIOE_AFRL              (PIN_AFIO_AF(GPIOE_CS2, 0U) |     \
                                     PIN_AFIO_AF(GPIOE_RST2, 0U) |     \
                                     PIN_AFIO_AF(GPIOE_GPIO15, 0U) |          \
                                     PIN_AFIO_AF(GPIOE_GPIO16, 0U) |          \
                                     PIN_AFIO_AF(GPIOE_GPIO17, 0U) |          \
                                     PIN_AFIO_AF(GPIOE_GPIO18, 0U) |          \
                                     PIN_AFIO_AF(GPIOE_GPIO19, 0U) |          \
                                     PIN_AFIO_AF(GPIOE_GPIO20, 0U))
#define VAL_GPIOE_AFRH              (PIN_AFIO_AF(GPIOE_GPIO21, 0U) |       \
                                     PIN_AFIO_AF(GPIOE_PWM1, 1U) |       \
                                     PIN_AFIO_AF(GPIOE_LED2, 0U) |       \
                                     PIN_AFIO_AF(GPIOE_GPIO22, 0U) |       \
                                     PIN_AFIO_AF(GPIOE_USB_DETECT, 0U) |       \
                                     PIN_AFIO_AF(GPIOE_GPIO23, 0U) |      \
                                     PIN_AFIO_AF(GPIOE_GPIO24, 0U) |      \
                                     PIN_AFIO_AF(GPIOE_LED1, 0U))


/*
 * GPIOH setup:
 *
 * PH0  - OSC_IN                    (input floating).
 * PH1  - OSC_OUT                   (input floating).
 * PH2  - PIN2                      (input pullup).
 * PH3  - PIN3                      (input pullup).
 * PH4  - SPI1_MISO                      (input pullup).
 * PH5  - PIN5                      (input pullup).
 * PH6  - PIN6                      (input pullup).
 * PH7  - PIN7                      (input pullup).
 * PH8  - PIN8                      (input pullup).
 * PH9  - PIN9                      (input pullup).
 * PH10 - PIN10                     (input pullup).
 * PH11 - PIN11                     (input pullup).
 * PH12 - PIN12                     (input pullup).
 * PH13 - PIN13                     (input pullup).
 * PH14 - PIN14                     (input pullup).
 * PH15 - PIN15                     (input pullup).
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
