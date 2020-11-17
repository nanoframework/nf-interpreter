//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef BOARD_H
#define BOARD_H

// clang-format off

/*
 * Board identifier.
 */
#define BOARD_NETDUINO3WIFI
#define BOARD_NAME                  "NESHTEC_C2STM32F4"

#define BOARD_OTG_NOVBUSSENS

/*
 * Ethernet PHY type.
 */
#define BOARD_PHY_ID                MII_LAN8720_ID
#define BOARD_PHY_RMII

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
#define STM32_VDD                   330U

/*
 * MCU type as defined in the ST header.
 */
#define STM32F407xx

/*
 * IO pins assignments.
 */
#define GPIOA_PA0_AN            0U
#define GPIOA_RMII_REF_CLK      1U
#define GPIOA_RMII_MDIO         2U
#define GPIOA_MB2_AN            3U
#define GPIOA_MB1_AN            4U
#define GPIOA_PA5_T1            5U
#define GPIOA_PA6_PWM           6U
#define GPIOA_RMII_CRS_DV       7U
#define GPIOA_MCO				      8U
#define GPIOA_USB_DET           9U
#define GPIOA_PA10_PWM          10U
#define GPIOA_OTG_FS_DN         11U
#define GPIOA_OTG_FS_DP         12U
#define GPIOA_SWDIO		          13U
#define GPIOA_SWCLK             14U
#define GPIOA_JTDI              15U

#define GPIOB_PB0_PWM           0U
#define GPIOB_VBAT_SENSE        1U
#define BOOT1                   2U
#define SWO		                  3U
#define GPIOB_JTRST		          4U
#define GPIOB_PB5_GPIO          5U
#define GPIOB_I2C1_SCL          6U
#define GPIOB_I2C1_SDA          7U
#define GPIOB_PB8_PWM           8U
#define GPIOB_PB9_PWM           9U
#define GPIOB_RMII_RXER         10U
#define GPIOB_RMII_TX_EN        11U
#define GPIOB_RMII_TXD0	        12U
#define GPIOB_RMII_TXD1         13U
#define GPIOB_RMII_N_INT		    14U
#define GPIOB_PB15_PWM		      15U

#define GPIOC_PC0_AN            0U
#define GPIOC_RMII_MDC          1U
#define GPIOC_PC2_AN	       		2U
#define GPIOC_PC3_AN 	          3U
#define GPIOC_RMII_RXD0         4U
#define GPIOC_RMII_RXD1	        5U
#define GPIOC_UART6_TX          6U
#define GPIOC_UART6_RX          7U
#define GPIOC_PC8_GPIO          8U
#define GPIOC_PC9_GPIO          9U
#define GPIOC_SPI3_CLK          10U
#define GPIOC_SPI3_MISO         11U
#define GPIOC_SPI3_MOSI         12U
#define GPIOC_PC13_RST          13U
#define GPIOC_OSC32_IN          14U
#define GPIOC_OSC32_OUT         15U

#define GPIOD_PD0_INT           0U
#define GPIOD_PD1_INT           1U
#define GPIOD_PD2_INT           2U
#define GPIOD_PD3_INT           3U
#define GPIOD_BATSTAT           4U
#define GPIOD_UART2_TX          5U
#define GPIOD_UART2_RX          6U
#define GPIOD_LD2               7U
#define GPIOD_USART3_TX	        8U
#define GPIOD_USART3_RX	        9U
#define GPIOD_PD10_GPIO         10U
#define GPIOD_SENSEL		       	11U
#define GPIOD_PD12_MB2_PWM      12U
#define GPIOD_PD13_GPIO         13U
#define GPIOD_PD14_GPIO         14U
#define GPIOD_PD15_GPIO         15U

#define GPIOE_T2		            0U
#define GPIOE_PE1_GPIO          1U
#define GPIOE_PE2_GPIO          2U
#define GPIOE_PE3_GPIO	        3U
#define GPIOE_PE4_GPIO          4U
#define GPIOE_PE5_PWM	          5U
#define GPIOE_PE6_GPIO          6U
#define GPIOE_MB1_RST           7U
#define GPIOE_MB1_CS            8U
#define GPIOE_MB1_PWM          	9U
#define GPIOE_MB1_INT	          10U
#define GPIOE_MB2_CS			      11U
#define GPIOE_LD1	              12U
#define GPIOE_MB2_RST           13U
#define GPIOE_MB2_INT          	14U
#define GPIOE_PE15_GPIO      	  15U

#define GPIOH_OSC_IN            0U
#define GPIOH_OSC_OUT           1U
#define GPIOH_PIN2              2U
#define GPIOH_PIN3              3U
#define GPIOH_PIN4              4U
#define GPIOH_PIN5              5U
#define GPIOH_PIN6              6U
#define GPIOH_PIN7              7U
#define GPIOH_PIN8              8U
#define GPIOH_PIN9              9U
#define GPIOH_PIN10             10U
#define GPIOH_PIN11             11U
#define GPIOH_PIN12             12U
#define GPIOH_PIN13             13U
#define GPIOH_PIN14             14U
#define GPIOH_PIN15             15U

/*
 * IO lines assignments.
 */

#define LINE_T1            PAL_LINE(GPIOA, 5U)
#define LINE_LD1           PAL_LINE(GPIOE, 12U)
#define LINE_LD2           PAL_LINE(GPIOD, 7U)

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
 * PA0  - GPIOA_PA0_AN
 * PA1  - GPIOA_RMII_REF_CLK
 * PA2  - GPIOA_RMII_MDIO
 * PA3  - GPIOA_MB2_AN
 * PA4  - GPIOA_MB1_AN
 * PA5  - GPIOA_PA5_T1
 * PA6  - GPIOA_PA6_PWM
 * PA7  - GPIOA_RMII_CRS_DV
 * PA8  - GPIOA_MCO
 * PA9  - GPIOA_USB_DET
 * PA10 - GPIOA_PA10_PWM
 * PA11 - OTG_FS_DN
 * PA12 - OTG_FS_DP
 * PA13 - SWDIO
 * PA14 - SWCLK
 * PA15 - JTDI
 */
#define VAL_GPIOA_MODER             (PIN_MODE_ANALOG(GPIOA_PA0_AN) |         	\
                                     PIN_MODE_ALTERNATE(GPIOA_RMII_REF_CLK) | \
                                     PIN_MODE_ALTERNATE(GPIOA_RMII_MDIO) |    \
                                     PIN_MODE_ANALOG(GPIOA_MB2_AN) |     		  \
                                     PIN_MODE_ANALOG(GPIOA_MB1_AN) |  			  \
                                     PIN_MODE_OUTPUT(GPIOA_PA5_T1) |          \
                                     PIN_MODE_ALTERNATE(GPIOA_PA6_PWM) |     	\
                                     PIN_MODE_ALTERNATE(GPIOA_RMII_CRS_DV) |  \
                                     PIN_MODE_ALTERNATE(GPIOA_MCO) |   	        \
                                     PIN_MODE_INPUT(GPIOA_USB_DET) |    	    \
                                     PIN_MODE_ALTERNATE(GPIOA_PA10_PWM) |    	\
                                     PIN_MODE_ALTERNATE(GPIOA_OTG_FS_DN) |    \
                                     PIN_MODE_ALTERNATE(GPIOA_OTG_FS_DP) |    \
                                     PIN_MODE_ALTERNATE(GPIOA_SWDIO) |        \
                                     PIN_MODE_ALTERNATE(GPIOA_SWCLK) |      	\
                                     PIN_MODE_ALTERNATE(GPIOA_JTDI))
#define VAL_GPIOA_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOA_PA0_AN) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOA_RMII_REF_CLK) | \
                                     PIN_OTYPE_PUSHPULL(GPIOA_RMII_MDIO) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOA_MB2_AN) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOA_MB1_AN) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOA_PA5_T1) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOA_PA6_PWM) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOA_RMII_CRS_DV) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOA_MCO) |         \
                                     PIN_OTYPE_PUSHPULL(GPIOA_USB_DET) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOA_PA10_PWM) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOA_OTG_FS_DN) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOA_OTG_FS_DP) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOA_SWDIO) |        \
                                     PIN_OTYPE_PUSHPULL(GPIOA_SWCLK) |        \
                                     PIN_OTYPE_PUSHPULL(GPIOA_JTDI))
#define VAL_GPIOA_OSPEEDR           (PIN_OSPEED_HIGH(GPIOA_PA0_AN) |          \
                                     PIN_OSPEED_HIGH(GPIOA_MCO) |            \
                                     PIN_OSPEED_HIGH(GPIOA_RMII_MDIO) |       \
                                     PIN_OSPEED_HIGH(GPIOA_MB2_AN) |          \
                                     PIN_OSPEED_HIGH(GPIOA_MB1_AN) |          \
                                     PIN_OSPEED_HIGH(GPIOA_PA5_T1) |          \
                                     PIN_OSPEED_HIGH(GPIOA_PA6_PWM) |         \
                                     PIN_OSPEED_HIGH(GPIOA_RMII_CRS_DV) |     \
                                     PIN_OSPEED_HIGH(GPIOA_MCO) |            \
                                     PIN_OSPEED_HIGH(GPIOA_USB_DET) |         \
                                     PIN_OSPEED_HIGH(GPIOA_PA10_PWM) |        \
                                     PIN_OSPEED_HIGH(GPIOA_OTG_FS_DN) |       \
                                     PIN_OSPEED_HIGH(GPIOA_OTG_FS_DP) |       \
                                     PIN_OSPEED_HIGH(GPIOA_SWDIO) |           \
                                     PIN_OSPEED_HIGH(GPIOA_SWCLK) |           \
                                     PIN_OSPEED_HIGH(GPIOA_JTDI))
#define VAL_GPIOA_PUPDR             (PIN_PUPDR_FLOATING(GPIOA_PA0_AN) |       \
                                     PIN_PUPDR_FLOATING(GPIOA_RMII_REF_CLK) | \
                                     PIN_PUPDR_FLOATING(GPIOA_RMII_MDIO) |      \
                                     PIN_PUPDR_FLOATING(GPIOA_MB2_AN) |       \
                                     PIN_PUPDR_FLOATING(GPIOA_MB1_AN) |       \
                                     PIN_PUPDR_FLOATING(GPIOA_PA5_T1) |       \
                                     PIN_PUPDR_FLOATING(GPIOA_PA6_PWM) |      \
                                     PIN_PUPDR_FLOATING(GPIOA_RMII_CRS_DV) |  \
                                     PIN_PUPDR_FLOATING(GPIOA_MCO) |         \
                                     PIN_PUPDR_PULLDOWN(GPIOA_USB_DET) |      \
                                     PIN_PUPDR_FLOATING(GPIOA_PA10_PWM) |     \
                                     PIN_PUPDR_FLOATING(GPIOA_OTG_FS_DN) |    \
                                     PIN_PUPDR_FLOATING(GPIOA_OTG_FS_DP) |    \
                                     PIN_PUPDR_PULLUP(GPIOA_SWDIO) |          \
                                     PIN_PUPDR_PULLDOWN(GPIOA_SWCLK) |        \
                                     PIN_PUPDR_FLOATING(GPIOA_JTDI))
#define VAL_GPIOA_ODR               (PIN_ODR_HIGH(GPIOA_PA0_AN) |             \
                                     PIN_ODR_HIGH(GPIOA_RMII_REF_CLK) |       \
                                     PIN_ODR_HIGH(GPIOA_RMII_MDIO) |          \
                                     PIN_ODR_HIGH(GPIOA_MB2_AN) |             \
                                     PIN_ODR_HIGH(GPIOA_MB1_AN) |             \
                                     PIN_ODR_HIGH(GPIOA_PA5_T1) |             \
                                     PIN_ODR_HIGH(GPIOA_PA6_PWM) |            \
                                     PIN_ODR_HIGH(GPIOA_RMII_CRS_DV) |        \
                                     PIN_ODR_HIGH(GPIOA_MCO) |               \
                                     PIN_ODR_HIGH(GPIOA_USB_DET) |            \
                                     PIN_ODR_HIGH(GPIOA_PA10_PWM) |           \
                                     PIN_ODR_HIGH(GPIOA_OTG_FS_DN) |          \
                                     PIN_ODR_HIGH(GPIOA_OTG_FS_DP) |          \
                                     PIN_ODR_HIGH(GPIOA_SWDIO) |              \
                                     PIN_ODR_HIGH(GPIOA_SWCLK) |              \
                                     PIN_ODR_HIGH(GPIOA_JTDI))
#define VAL_GPIOA_AFRL              (PIN_AFIO_AF(GPIOA_PA0_AN, 0U) |          \
                                     PIN_AFIO_AF(GPIOA_RMII_REF_CLK, 11U) |   \
                                     PIN_AFIO_AF(GPIOA_RMII_MDIO, 11U) |      \
                                     PIN_AFIO_AF(GPIOA_MB2_AN, 0U) |          \
                                     PIN_AFIO_AF(GPIOA_MB1_AN, 0U) |          \
                                     PIN_AFIO_AF(GPIOA_PA5_T1, 0U) |          \
                                     PIN_AFIO_AF(GPIOA_PA6_PWM, 2U) |         \
                                     PIN_AFIO_AF(GPIOA_RMII_CRS_DV, 11U))
#define VAL_GPIOA_AFRH              (PIN_AFIO_AF(GPIOA_MCO, 1U) |            \
                                     PIN_AFIO_AF(GPIOA_USB_DET, 0U) |         \
                                     PIN_AFIO_AF(GPIOA_PA10_PWM, 2U) |        \
                                     PIN_AFIO_AF(GPIOA_OTG_FS_DN, 10U) |      \
                                     PIN_AFIO_AF(GPIOA_OTG_FS_DP, 10U) |      \
                                     PIN_AFIO_AF(GPIOA_SWDIO, 0U) |           \
                                     PIN_AFIO_AF(GPIOA_SWCLK, 0U) |           \
                                     PIN_AFIO_AF(GPIOA_JTDI, 0U))

/*
 * GPIOB setup:
 *
 * PB0  - GPIOB_PB0_PWM
 * PB1  - GPIOB_VBAT_SENSE
 * PB2  - BOOT1
 * PB3  - SWO
 * PB4  - GPIOB_JTRST
 * PB5  - GPIOB_PB5_GPIO
 * PB6  - GPIOB_I2C1_SCL
 * PB7  - GPIOB_I2C1_SDA
 * PB8  - GPIOB_PB8_PWM
 * PB9  - GPIOB_PB9_PWM
 * PB10 - GPIOB_RMII_RXER
 * PB11 - GPIOB_RMII_TX_EN
 * PB12 - GPIOB_RMII_TXD0
 * PB13 - GPIOB_RMII_TXD1
 * PB14 - GPIOB_RMII_N_INT
 * PB15 - GPIOB_PB15_PWM
 */
#define VAL_GPIOB_MODER             (PIN_MODE_ALTERNATE(GPIOB_PB0_PWM) |      \
                                     PIN_MODE_ANALOG(GPIOB_VBAT_SENSE) |      \
                                     PIN_MODE_INPUT(BOOT1) |                  \
                                     PIN_MODE_ALTERNATE(SWO) |                \
                                     PIN_MODE_ALTERNATE(GPIOB_JTRST) |        \
                                     PIN_MODE_INPUT(GPIOB_PB5_GPIO) |         \
                                     PIN_MODE_ALTERNATE(GPIOB_I2C1_SCL) |     \
                                     PIN_MODE_ALTERNATE(GPIOB_I2C1_SDA) |     \
                                     PIN_MODE_ALTERNATE(GPIOB_PB8_PWM) |      \
                                     PIN_MODE_ALTERNATE(GPIOB_PB9_PWM) |      \
                                     PIN_MODE_ALTERNATE(GPIOB_RMII_RXER) |    \
                                     PIN_MODE_ALTERNATE(GPIOB_RMII_TX_EN) |   \
                                     PIN_MODE_ALTERNATE(GPIOB_RMII_TXD0) |    \
                                     PIN_MODE_ALTERNATE(GPIOB_RMII_TXD1) |    \
                                     PIN_MODE_INPUT(GPIOB_RMII_N_INT) |       \
                                     PIN_MODE_ALTERNATE(GPIOB_PB15_PWM))
#define VAL_GPIOB_OTYPER            (PIN_PUPDR_FLOATING(GPIOB_PB0_PWM) |      \
                                     PIN_PUPDR_FLOATING(GPIOB_VBAT_SENSE) |   \
                                     PIN_PUPDR_FLOATING(BOOT1) |              \
                                     PIN_PUPDR_FLOATING(SWO) |                \
                                     PIN_PUPDR_FLOATING(GPIOB_JTRST) |        \
                                     PIN_PUPDR_FLOATING(GPIOB_PB5_GPIO) |     \
                                     PIN_OTYPE_OPENDRAIN(GPIOB_I2C1_SCL) |    \
                                     PIN_OTYPE_OPENDRAIN(GPIOB_I2C1_SDA) |    \
                                     PIN_PUPDR_FLOATING(GPIOB_PB8_PWM) |      \
                                     PIN_PUPDR_FLOATING(GPIOB_PB9_PWM) |      \
                                     PIN_PUPDR_FLOATING(GPIOB_RMII_RXER) |    \
                                     PIN_PUPDR_FLOATING(GPIOB_RMII_TX_EN) |   \
                                     PIN_PUPDR_FLOATING(GPIOB_RMII_TXD0) |    \
                                     PIN_PUPDR_FLOATING(GPIOB_RMII_TXD1) |    \
                                     PIN_PUPDR_FLOATING(GPIOB_RMII_N_INT) |   \
                                     PIN_PUPDR_FLOATING(GPIOB_PB15_PWM))
#define VAL_GPIOB_OSPEEDR           (PIN_OSPEED_HIGH(GPIOB_PB0_PWM) |         \
                                     PIN_OSPEED_HIGH(GPIOB_VBAT_SENSE) |      \
                                     PIN_OSPEED_HIGH(BOOT1) |                 \
                                     PIN_OSPEED_HIGH(SWO) |                   \
                                     PIN_OSPEED_HIGH(GPIOB_JTRST) |           \
                                     PIN_OSPEED_HIGH(GPIOB_PB5_GPIO) |        \
                                     PIN_OSPEED_HIGH(GPIOB_I2C1_SCL) |        \
                                     PIN_OSPEED_HIGH(GPIOB_I2C1_SDA) |        \
                                     PIN_OSPEED_HIGH(GPIOB_PB8_PWM) |         \
                                     PIN_OSPEED_HIGH(GPIOB_PB9_PWM) |         \
                                     PIN_OSPEED_HIGH(GPIOB_RMII_RXER) |       \
                                     PIN_OSPEED_HIGH(GPIOB_RMII_TX_EN) |      \
                                     PIN_OSPEED_HIGH(GPIOB_RMII_TXD0) |       \
                                     PIN_OSPEED_HIGH(GPIOB_RMII_TXD1) |       \
                                     PIN_OSPEED_HIGH(GPIOB_RMII_N_INT) |      \
                                     PIN_OSPEED_HIGH(GPIOB_PB15_PWM))
#define VAL_GPIOB_PUPDR             (PIN_PUPDR_FLOATING(GPIOB_PB0_PWM) |      \
                                     PIN_PUPDR_FLOATING(GPIOB_VBAT_SENSE) |   \
                                     PIN_PUPDR_FLOATING(BOOT1) |              \
                                     PIN_PUPDR_FLOATING(SWO) |                \
                                     PIN_PUPDR_FLOATING(GPIOB_JTRST) |        \
                                     PIN_PUPDR_FLOATING(GPIOB_PB5_GPIO) |     \
                                     PIN_PUPDR_FLOATING(GPIOB_I2C1_SCL) |     \
                                     PIN_PUPDR_FLOATING(GPIOB_I2C1_SDA) |     \
                                     PIN_PUPDR_FLOATING(GPIOB_PB8_PWM) |      \
                                     PIN_PUPDR_FLOATING(GPIOB_PB9_PWM) |      \
                                     PIN_PUPDR_FLOATING(GPIOB_RMII_RXER) |    \
                                     PIN_PUPDR_FLOATING(GPIOB_RMII_TX_EN) |   \
                                     PIN_PUPDR_FLOATING(GPIOB_RMII_TXD0) |    \
                                     PIN_PUPDR_FLOATING(GPIOB_RMII_TXD1) |    \
                                     PIN_PUPDR_FLOATING(GPIOB_RMII_N_INT) |   \
                                     PIN_PUPDR_FLOATING(GPIOB_PB15_PWM))
#define VAL_GPIOB_ODR               (PIN_ODR_LOW(GPIOB_PB0_PWM) |             \
                                     PIN_ODR_HIGH(GPIOB_VBAT_SENSE) |         \
                                     PIN_ODR_HIGH(BOOT1) |                    \
                                     PIN_ODR_LOW(SWO) |                       \
                                     PIN_ODR_LOW(GPIOB_JTRST) |               \
                                     PIN_ODR_LOW(GPIOB_PB5_GPIO) |            \
                                     PIN_ODR_LOW(GPIOB_I2C1_SCL) |            \
                                     PIN_ODR_LOW(GPIOB_I2C1_SDA) |            \
                                     PIN_ODR_LOW(GPIOB_PB8_PWM) |             \
                                     PIN_ODR_LOW(GPIOB_PB9_PWM) |             \
                                     PIN_ODR_HIGH(GPIOB_RMII_RXER) |           \
                                     PIN_ODR_HIGH(GPIOB_RMII_TX_EN) |          \
                                     PIN_ODR_HIGH(GPIOB_RMII_TXD0) |           \
                                     PIN_ODR_HIGH(GPIOB_RMII_TXD1) |           \
                                     PIN_ODR_HIGH(GPIOB_RMII_N_INT) |          \
                                     PIN_ODR_LOW(GPIOB_PB15_PWM))
#define VAL_GPIOB_AFRL              (PIN_AFIO_AF(GPIOB_PB0_PWM, 2U) |         \
                                     PIN_AFIO_AF(GPIOB_VBAT_SENSE, 0U) |      \
                                     PIN_AFIO_AF(BOOT1, 0U) |                 \
                                     PIN_AFIO_AF(SWO, 0U) |                   \
                                     PIN_AFIO_AF(GPIOB_JTRST, 0U) |           \
                                     PIN_AFIO_AF(GPIOB_PB5_GPIO, 0U) |        \
                                     PIN_AFIO_AF(GPIOB_I2C1_SCL, 4U) |        \
                                     PIN_AFIO_AF(GPIOB_I2C1_SDA, 4U))
#define VAL_GPIOB_AFRH              (PIN_AFIO_AF(GPIOB_PB8_PWM, 2U) |         \
                                     PIN_AFIO_AF(GPIOB_PB9_PWM, 2U) |         \
                                     PIN_AFIO_AF(GPIOB_RMII_RXER, 11U) |      \
                                     PIN_AFIO_AF(GPIOB_RMII_TX_EN, 11U) |     \
                                     PIN_AFIO_AF(GPIOB_RMII_TXD0, 11U) |      \
                                     PIN_AFIO_AF(GPIOB_RMII_TXD1, 11U) |      \
                                     PIN_AFIO_AF(GPIOB_RMII_N_INT, 0U) |      \
                                     PIN_AFIO_AF(GPIOB_PB15_PWM, 2U))

/*
 * GPIOC setup:
 *
 * PC0  - GPIOC_PC0_AN
 * PC1  - GPIOC_RMII_MDC
 * PC2  - GPIOC_PC2_AN
 * PC3  - GPIOC_PC3_AN
 * PC4  - GPIOC_RMII_RXD0
 * PC5  - GPIOC_RMII_RXD1
 * PC6  - USART6_TX_D1
 * PC7  - USART6_RX_D0
 * PC8  - GPIOC_PC8_GPIO
 * PC9  - GPIOC_PC9_GPIO
 * PC10 - SPI3_CLK
 * PC11 - SPI3_MISO
 * PC12 - SPI3_MOSI
 * PC13 - GPIOC_PC13_RST
 * PC14 - OSC32_IN
 * PC15 - OSC32_OUT
 */
#define VAL_GPIOC_MODER             (PIN_MODE_ANALOG(GPIOC_PC0_AN) |        \
                                     PIN_MODE_INPUT(GPIOC_RMII_MDC) |        \
                                     PIN_MODE_ANALOG(GPIOC_PC2_AN) |        \
                                     PIN_MODE_ANALOG(GPIOC_PC3_AN) |        \
                                     PIN_MODE_ALTERNATE(GPIOC_RMII_RXD0) |  \
                                     PIN_MODE_ALTERNATE(GPIOC_RMII_RXD1) |  \
                                     PIN_MODE_ALTERNATE(GPIOC_UART6_TX) |   \
                                     PIN_MODE_ALTERNATE(GPIOC_UART6_RX) |   \
                                     PIN_MODE_INPUT(GPIOC_PC8_GPIO) |       \
                                     PIN_MODE_INPUT(GPIOC_PC9_GPIO) |       \
                                     PIN_MODE_ALTERNATE(GPIOC_SPI3_CLK) |   \
                                     PIN_MODE_ALTERNATE(GPIOC_SPI3_MISO) |  \
                                     PIN_MODE_ALTERNATE(GPIOC_SPI3_MOSI) |  \
                                     PIN_MODE_OUTPUT(GPIOC_PC13_RST) |      \
                                     PIN_MODE_ALTERNATE(GPIOC_OSC32_IN) |   \
                                     PIN_MODE_ALTERNATE(GPIOC_OSC32_OUT))
#define VAL_GPIOC_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOC_PC0_AN) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOC_RMII_MDC) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOC_PC2_AN) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOC_PC3_AN) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOC_RMII_RXD0) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOC_RMII_RXD1) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOC_UART6_TX) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOC_UART6_RX) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOC_PC8_GPIO) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOC_PC9_GPIO) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOC_SPI3_CLK) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOC_SPI3_MISO) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOC_SPI3_MOSI) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOC_PC13_RST) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOC_OSC32_IN) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOC_OSC32_OUT))
#define VAL_GPIOC_OSPEEDR           (PIN_OSPEED_HIGH(GPIOC_PC0_AN) |        \
                                     PIN_OSPEED_HIGH(GPIOC_RMII_MDC) |       \
                                     PIN_OSPEED_HIGH(GPIOC_PC2_AN) |        \
                                     PIN_OSPEED_HIGH(GPIOC_PC3_AN) |        \
                                     PIN_OSPEED_HIGH(GPIOC_RMII_RXD0) |     \
                                     PIN_OSPEED_HIGH(GPIOC_RMII_RXD1) |     \
                                     PIN_OSPEED_HIGH(GPIOC_UART6_TX) |      \
                                     PIN_OSPEED_HIGH(GPIOC_UART6_RX) |      \
                                     PIN_OSPEED_HIGH(GPIOC_PC8_GPIO) |      \
                                     PIN_OSPEED_HIGH(GPIOC_PC9_GPIO) |      \
                                     PIN_OSPEED_HIGH(GPIOC_SPI3_CLK) |      \
                                     PIN_OSPEED_HIGH(GPIOC_SPI3_MISO) |     \
                                     PIN_OSPEED_HIGH(GPIOC_SPI3_MOSI) |     \
                                     PIN_OSPEED_HIGH(GPIOC_PC13_RST) |      \
                                     PIN_OSPEED_HIGH(GPIOC_OSC32_IN) |      \
                                     PIN_OSPEED_HIGH(GPIOC_OSC32_OUT))
#define VAL_GPIOC_PUPDR             (PIN_PUPDR_FLOATING(GPIOC_PC0_AN) |     \
                                     PIN_PUPDR_FLOATING(GPIOC_RMII_MDC) |    \
                                     PIN_PUPDR_FLOATING(GPIOC_PC2_AN) |     \
                                     PIN_PUPDR_FLOATING(GPIOC_PC3_AN) |     \
                                     PIN_PUPDR_FLOATING(GPIOC_RMII_RXD0) |  \
                                     PIN_PUPDR_FLOATING(GPIOC_RMII_RXD1) |  \
                                     PIN_PUPDR_FLOATING(GPIOC_UART6_TX) |   \
                                     PIN_PUPDR_FLOATING(GPIOC_UART6_RX) |   \
                                     PIN_PUPDR_FLOATING(GPIOC_PC8_GPIO) |   \
                                     PIN_PUPDR_FLOATING(GPIOC_PC9_GPIO) |   \
                                     PIN_PUPDR_FLOATING(GPIOC_SPI3_CLK) |   \
                                     PIN_PUPDR_FLOATING(GPIOC_SPI3_MISO) |  \
                                     PIN_PUPDR_FLOATING(GPIOC_SPI3_MOSI) |  \
                                     PIN_PUPDR_FLOATING(GPIOC_PC13_RST) |   \
                                     PIN_PUPDR_FLOATING(GPIOC_OSC32_IN) |   \
                                     PIN_PUPDR_FLOATING(GPIOC_OSC32_OUT))
#define VAL_GPIOC_ODR               (PIN_ODR_LOW(GPIOC_PC0_AN) |            \
                                     PIN_ODR_HIGH(GPIOC_RMII_MDC) |           \
                                     PIN_ODR_LOW(GPIOC_PC2_AN) |            \
                                     PIN_ODR_LOW(GPIOC_PC3_AN) |            \
                                     PIN_ODR_HIGH(GPIOC_RMII_RXD0) |         \
                                     PIN_ODR_HIGH(GPIOC_RMII_RXD1) |         \
                                     PIN_ODR_LOW(GPIOC_UART6_TX) |          \
                                     PIN_ODR_LOW(GPIOC_UART6_RX) |          \
                                     PIN_ODR_LOW(GPIOC_PC8_GPIO) |          \
                                     PIN_ODR_LOW(GPIOC_PC9_GPIO) |          \
                                     PIN_ODR_LOW(GPIOC_SPI3_CLK) |          \
                                     PIN_ODR_LOW(GPIOC_SPI3_MISO) |         \
                                     PIN_ODR_LOW(GPIOC_SPI3_MOSI) |         \
                                     PIN_ODR_LOW(GPIOC_PC13_RST) |          \
                                     PIN_ODR_LOW(GPIOC_OSC32_IN) |          \
                                     PIN_ODR_LOW(GPIOC_OSC32_OUT))
#define VAL_GPIOC_AFRL              (PIN_AFIO_AF(GPIOC_PC0_AN, 0U) |        \
                                     PIN_AFIO_AF(GPIOC_RMII_MDC, 11U) |       \
                                     PIN_AFIO_AF(GPIOC_PC2_AN, 0U) |        \
                                     PIN_AFIO_AF(GPIOC_PC3_AN, 0U) |        \
                                     PIN_AFIO_AF(GPIOC_RMII_RXD0, 11U) |    \
                                     PIN_AFIO_AF(GPIOC_RMII_RXD1, 11U) |    \
                                     PIN_AFIO_AF(GPIOC_UART6_TX, 8U) |      \
                                     PIN_AFIO_AF(GPIOC_UART6_RX, 8U))
#define VAL_GPIOC_AFRH              (PIN_AFIO_AF(GPIOC_PC8_GPIO, 0U) |      \
                                     PIN_AFIO_AF(GPIOC_PC9_GPIO, 0U) |      \
                                     PIN_AFIO_AF(GPIOC_SPI3_CLK, 6U) |      \
                                     PIN_AFIO_AF(GPIOC_SPI3_MISO, 6U) |     \
                                     PIN_AFIO_AF(GPIOC_SPI3_MOSI, 6U) |     \
                                     PIN_AFIO_AF(GPIOC_PC13_RST, 0U) |      \
                                     PIN_AFIO_AF(GPIOC_OSC32_IN, 0U) |      \
                                     PIN_AFIO_AF(GPIOC_OSC32_OUT, 0U))

/*
 * GPIOD setup:
 *
 * PD0  - GPIOD_PD0_INT
 * PD1  - GPIOD_PD1_INT
 * PD2  - GPIOD_PD2_INT
 * PD3  - GPIOD_PD3_INT
 * PD4  - GPIOD_BATSTAT
 * PD5  - GPIOD_UART2_TX
 * PD6  - GPIOD_UART2_RX
 * PD7  - GPIOD_LD2
 * PD8  - GPIOD_USART3_TX
 * PD9  - GPIOD_USART3_RX
 * PD10 - GPIOD_PD10_GPIO
 * PD11 - GPIOD_SENSEL
 * PD12 - GPIOD_PD12_MB2_PWM
 * PD13 - GPIOD_PD13_GPIO
 * PD14 - GPIOD_PD14_GPIO
 * PD15 - GPIOD_PD15_GPIO
 */
#define VAL_GPIOD_MODER             (PIN_MODE_INPUT(GPIOD_PD0_INT) |          \
                                     PIN_MODE_INPUT(GPIOD_PD1_INT) |          \
                                     PIN_MODE_INPUT(GPIOD_PD2_INT) |          \
                                     PIN_MODE_INPUT(GPIOD_PD3_INT) |          \
                                     PIN_MODE_OUTPUT(GPIOD_BATSTAT) |         \
                                     PIN_MODE_ALTERNATE(GPIOD_UART2_TX) |     \
                                     PIN_MODE_ALTERNATE(GPIOD_UART2_RX) |     \
                                     PIN_MODE_OUTPUT(GPIOD_LD2) |             \
                                     PIN_MODE_ALTERNATE(GPIOD_USART3_TX) |    \
                                     PIN_MODE_ALTERNATE(GPIOD_USART3_RX) |    \
                                     PIN_MODE_INPUT(GPIOD_PD10_GPIO) |        \
                                     PIN_MODE_OUTPUT(GPIOD_SENSEL) |          \
                                     PIN_MODE_ALTERNATE(GPIOD_PD12_MB2_PWM) | \
                                     PIN_MODE_INPUT(GPIOD_PD13_GPIO) |        \
                                     PIN_MODE_INPUT(GPIOD_PD14_GPIO) |        \
                                     PIN_MODE_INPUT(GPIOD_PD15_GPIO))
#define VAL_GPIOD_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOD_PD0_INT) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOD_PD1_INT) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOD_PD2_INT) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOD_PD3_INT) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOD_BATSTAT) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOD_UART2_TX) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOD_UART2_RX) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOD_LD2) |          \
                                     PIN_OTYPE_PUSHPULL(GPIOD_USART3_TX) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOD_USART3_RX) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOD_PD10_GPIO) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOD_SENSEL) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOD_PD12_MB2_PWM) | \
                                     PIN_OTYPE_PUSHPULL(GPIOD_PD13_GPIO) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOD_PD14_GPIO) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOD_PD15_GPIO))
#define VAL_GPIOD_OSPEEDR           (PIN_OSPEED_HIGH(GPIOD_PD0_INT) |         \
                                     PIN_OSPEED_HIGH(GPIOD_PD1_INT) |         \
                                     PIN_OSPEED_HIGH(GPIOD_PD2_INT) |         \
                                     PIN_OSPEED_HIGH(GPIOD_PD3_INT) |         \
                                     PIN_OSPEED_HIGH(GPIOD_BATSTAT) |         \
                                     PIN_OSPEED_HIGH(GPIOD_UART2_TX) |        \
                                     PIN_OSPEED_HIGH(GPIOD_UART2_RX) |        \
                                     PIN_OSPEED_HIGH(GPIOD_LD2) |             \
                                     PIN_OSPEED_HIGH(GPIOD_USART3_TX) |       \
                                     PIN_OSPEED_HIGH(GPIOD_USART3_RX) |       \
                                     PIN_OSPEED_HIGH(GPIOD_PD10_GPIO) |       \
                                     PIN_OSPEED_HIGH(GPIOD_SENSEL) |          \
                                     PIN_OSPEED_HIGH(GPIOD_PD12_MB2_PWM) |    \
                                     PIN_OSPEED_HIGH(GPIOD_PD13_GPIO) |       \
                                     PIN_OSPEED_HIGH(GPIOD_PD14_GPIO) |       \
                                     PIN_OSPEED_HIGH(GPIOD_PD15_GPIO))
#define VAL_GPIOD_PUPDR             (PIN_PUPDR_FLOATING(GPIOD_PD0_INT) |      \
                                     PIN_PUPDR_FLOATING(GPIOD_PD1_INT) |      \
                                     PIN_PUPDR_FLOATING(GPIOD_PD2_INT) |      \
                                     PIN_PUPDR_FLOATING(GPIOD_PD3_INT) |      \
                                     PIN_PUPDR_FLOATING(GPIOD_BATSTAT) |      \
                                     PIN_PUPDR_FLOATING(GPIOD_UART2_TX) |     \
                                     PIN_PUPDR_FLOATING(GPIOD_UART2_RX) |     \
                                     PIN_PUPDR_FLOATING(GPIOD_LD2) |          \
                                     PIN_PUPDR_FLOATING(GPIOD_USART3_TX) |    \
                                     PIN_PUPDR_FLOATING(GPIOD_USART3_RX) |    \
                                     PIN_PUPDR_FLOATING(GPIOD_PD10_GPIO) |    \
                                     PIN_PUPDR_FLOATING(GPIOD_SENSEL) |       \
                                     PIN_PUPDR_FLOATING(GPIOD_PD12_MB2_PWM) | \
                                     PIN_PUPDR_FLOATING(GPIOD_PD13_GPIO) |    \
                                     PIN_PUPDR_FLOATING(GPIOD_PD14_GPIO) |    \
                                     PIN_PUPDR_FLOATING(GPIOD_PD15_GPIO))
#define VAL_GPIOD_ODR               (PIN_ODR_HIGH(GPIOD_PD0_INT) |            \
                                     PIN_ODR_HIGH(GPIOD_PD1_INT) |            \
                                     PIN_ODR_HIGH(GPIOD_PD2_INT) |            \
                                     PIN_ODR_HIGH(GPIOD_PD3_INT) |            \
                                     PIN_ODR_LOW(GPIOD_BATSTAT) |             \
                                     PIN_ODR_LOW(GPIOD_UART2_TX) |            \
                                     PIN_ODR_LOW(GPIOD_UART2_RX) |            \
                                     PIN_ODR_LOW(GPIOD_LD2) |                 \
                                     PIN_ODR_LOW(GPIOD_USART3_TX) |           \
                                     PIN_ODR_LOW(GPIOD_USART3_RX) |           \
                                     PIN_ODR_LOW(GPIOD_PD10_GPIO) |           \
                                     PIN_ODR_LOW(GPIOD_SENSEL) |              \
                                     PIN_ODR_LOW(GPIOD_PD12_MB2_PWM) |        \
                                     PIN_ODR_HIGH(GPIOD_PD13_GPIO) |          \
                                     PIN_ODR_HIGH(GPIOD_PD14_GPIO) |          \
                                     PIN_ODR_HIGH(GPIOD_PD15_GPIO))
#define VAL_GPIOD_AFRL              (PIN_AFIO_AF(GPIOD_PD0_INT, 0U) |         \
                                     PIN_AFIO_AF(GPIOD_PD1_INT, 0U) |         \
                                     PIN_AFIO_AF(GPIOD_PD2_INT, 0U) |         \
                                     PIN_AFIO_AF(GPIOD_PD3_INT, 0U) |         \
                                     PIN_AFIO_AF(GPIOD_BATSTAT, 0) |          \
                                     PIN_AFIO_AF(GPIOD_UART2_TX, 7U) |        \
                                     PIN_AFIO_AF(GPIOD_UART2_RX, 7U) |        \
                                     PIN_AFIO_AF(GPIOD_LD2, 0U))
#define VAL_GPIOD_AFRH              (PIN_AFIO_AF(GPIOD_USART3_TX, 7U) |       \
                                     PIN_AFIO_AF(GPIOD_USART3_RX, 7U) |       \
                                     PIN_AFIO_AF(GPIOD_PD10_GPIO, 0U) |       \
                                     PIN_AFIO_AF(GPIOD_SENSEL, 0U) |          \
                                     PIN_AFIO_AF(GPIOD_PD12_MB2_PWM, 2U) |    \
                                     PIN_AFIO_AF(GPIOD_PD13_GPIO, 0U) |       \
                                     PIN_AFIO_AF(GPIOD_PD14_GPIO, 0U) |       \
                                     PIN_AFIO_AF(GPIOD_PD15_GPIO, 0U))

/*
 * GPIOE setup:
 *
 * PE0  - GPIOE_T2
 * PE1  - GPIOE_PE1_GPIO
 * PE2  - GPIOE_PE2_GPIO
 * PE3  - EN_UART_TX_PU_GOPORT1
 * PE4  - GPIOE_PE4_GPIO
 * PE5  - GPIOE_PE5_PWM
 * PE6  - GPIOE_PE6_GPIO
 * PE7  - GPIOE_MB1_RST
 * PE8  - GPIOE_MB1_CS
 * PE9  - GPIOE_MB1_PWM
 * PE10 - GPIOE_MB1_INT
 * PE11 - GPIOE_MB2_CS
 * PE12 - GPIOE_LD1
 * PE13 - GPIOE_MB2_RST
 * PE14 - GPIOE_MB2_INT
 * PE15 - GPIOE_PE15_GPIO
 */
#define VAL_GPIOE_MODER             (PIN_MODE_INPUT(GPIOE_T2) |           \
                                     PIN_MODE_INPUT(GPIOE_PE1_GPIO) |     \
                                     PIN_MODE_INPUT(GPIOE_PE2_GPIO) |     \
                                     PIN_MODE_INPUT(GPIOE_PE3_GPIO) |     \
                                     PIN_MODE_INPUT(GPIOE_PE4_GPIO) |     \
                                     PIN_MODE_ALTERNATE(GPIOE_PE5_PWM) |  \
                                     PIN_MODE_INPUT(GPIOE_PE6_GPIO) |     \
                                     PIN_MODE_OUTPUT(GPIOE_MB1_RST) |     \
                                     PIN_MODE_OUTPUT(GPIOE_MB1_CS) |      \
                                     PIN_MODE_ALTERNATE(GPIOE_MB1_PWM) |  \
                                     PIN_MODE_INPUT(GPIOE_MB1_INT) |      \
                                     PIN_MODE_OUTPUT(GPIOE_MB2_CS) |      \
                                     PIN_MODE_OUTPUT(GPIOE_LD1) |         \
                                     PIN_MODE_OUTPUT(GPIOE_MB2_RST) |     \
                                     PIN_MODE_INPUT(GPIOE_MB2_INT) |      \
                                     PIN_MODE_INPUT(GPIOE_PE15_GPIO))
#define VAL_GPIOE_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOE_T2) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOE_PE1_GPIO) | \
                                     PIN_OTYPE_PUSHPULL(GPIOE_PE2_GPIO) | \
                                     PIN_OTYPE_PUSHPULL(GPIOE_PE3_GPIO) | \
                                     PIN_OTYPE_PUSHPULL(GPIOE_PE4_GPIO) | \
                                     PIN_OTYPE_PUSHPULL(GPIOE_PE5_PWM) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOE_PE6_GPIO) | \
                                     PIN_OTYPE_PUSHPULL(GPIOE_MB1_RST) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOE_MB1_CS) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOE_MB1_PWM) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOE_MB1_INT) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOE_MB2_CS) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOE_LD1) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOE_MB2_RST) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOE_MB2_INT) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOE_PE15_GPIO))
#define VAL_GPIOE_OSPEEDR           (PIN_OSPEED_HIGH(GPIOE_T2) |          \
                                     PIN_OSPEED_HIGH(GPIOE_PE1_GPIO) |    \
                                     PIN_OSPEED_HIGH(GPIOE_PE2_GPIO) |    \
                                     PIN_OSPEED_HIGH(GPIOE_PE3_GPIO) |    \
                                     PIN_OSPEED_HIGH(GPIOE_PE4_GPIO) |    \
                                     PIN_OSPEED_HIGH(GPIOE_PE5_PWM) |     \
                                     PIN_OSPEED_HIGH(GPIOE_PE6_GPIO) |    \
                                     PIN_OSPEED_HIGH(GPIOE_MB1_RST) |     \
                                     PIN_OSPEED_HIGH(GPIOE_MB1_CS) |      \
                                     PIN_OSPEED_HIGH(GPIOE_MB1_PWM) |     \
                                     PIN_OSPEED_HIGH(GPIOE_MB1_INT) |     \
                                     PIN_OSPEED_HIGH(GPIOE_MB2_CS) |      \
                                     PIN_OSPEED_HIGH(GPIOE_LD1) |         \
                                     PIN_OSPEED_HIGH(GPIOE_MB2_RST) |     \
                                     PIN_OSPEED_HIGH(GPIOE_MB2_INT) |     \
                                     PIN_OSPEED_HIGH(GPIOE_PE15_GPIO))
#define VAL_GPIOE_PUPDR             (PIN_PUPDR_FLOATING(GPIOE_T2) |       \
                                     PIN_PUPDR_FLOATING(GPIOE_PE1_GPIO) | \
                                     PIN_PUPDR_FLOATING(GPIOE_PE2_GPIO) | \
                                     PIN_PUPDR_FLOATING(GPIOE_PE3_GPIO) | \
                                     PIN_PUPDR_FLOATING(GPIOE_PE4_GPIO) | \
                                     PIN_PUPDR_FLOATING(GPIOE_PE5_PWM) |  \
                                     PIN_PUPDR_FLOATING(GPIOE_PE6_GPIO) | \
                                     PIN_PUPDR_FLOATING(GPIOE_MB1_RST) |  \
                                     PIN_PUPDR_FLOATING(GPIOE_MB1_CS) |   \
                                     PIN_PUPDR_FLOATING(GPIOE_MB1_PWM) |  \
                                     PIN_PUPDR_FLOATING(GPIOE_MB1_INT) |  \
                                     PIN_PUPDR_FLOATING(GPIOE_MB2_CS) |   \
                                     PIN_PUPDR_FLOATING(GPIOE_LD1) |      \
                                     PIN_PUPDR_FLOATING(GPIOE_MB2_RST) |  \
                                     PIN_PUPDR_FLOATING(GPIOE_MB2_INT) |  \
                                     PIN_PUPDR_FLOATING(GPIOE_PE15_GPIO))
#define VAL_GPIOE_ODR               (PIN_ODR_LOW(GPIOE_T2) |              \
                                     PIN_ODR_LOW(GPIOE_PE1_GPIO) |        \
                                     PIN_ODR_LOW(GPIOE_PE2_GPIO) |        \
                                     PIN_ODR_LOW(GPIOE_PE3_GPIO) |        \
                                     PIN_ODR_LOW(GPIOE_PE4_GPIO) |        \
                                     PIN_ODR_LOW(GPIOE_PE5_PWM) |         \
                                     PIN_ODR_LOW(GPIOE_PE6_GPIO) |        \
                                     PIN_ODR_LOW(GPIOE_MB1_RST) |         \
                                     PIN_ODR_LOW(GPIOE_MB1_CS) |          \
                                     PIN_ODR_LOW(GPIOE_MB1_PWM) |         \
                                     PIN_ODR_LOW(GPIOE_MB1_INT) |         \
                                     PIN_ODR_LOW(GPIOE_MB2_CS) |          \
                                     PIN_ODR_LOW(GPIOE_LD1) |             \
                                     PIN_ODR_LOW(GPIOE_MB2_RST) |         \
                                     PIN_ODR_LOW(GPIOE_MB2_INT) |         \
                                     PIN_ODR_LOW(GPIOE_PE15_GPIO))
#define VAL_GPIOE_AFRL              (PIN_AFIO_AF(GPIOE_T2, 0U) |          \
                                     PIN_AFIO_AF(GPIOE_PE1_GPIO, 0U) |    \
                                     PIN_AFIO_AF(GPIOE_PE2_GPIO, 0U) |    \
                                     PIN_AFIO_AF(GPIOE_PE3_GPIO, 0U) |    \
                                     PIN_AFIO_AF(GPIOE_PE4_GPIO, 0U) |    \
                                     PIN_AFIO_AF(GPIOE_PE5_PWM, 2U) |     \
                                     PIN_AFIO_AF(GPIOE_PE6_GPIO, 0U) |    \
                                     PIN_AFIO_AF(GPIOE_MB1_RST, 0U))
#define VAL_GPIOE_AFRH              (PIN_AFIO_AF(GPIOE_MB1_CS, 0U) |      \
                                     PIN_AFIO_AF(GPIOE_MB1_PWM, 2U) |     \
                                     PIN_AFIO_AF(GPIOE_MB1_INT, 0U) |     \
                                     PIN_AFIO_AF(GPIOE_MB2_CS, 0U) |      \
                                     PIN_AFIO_AF(GPIOE_LD1, 0U) |         \
                                     PIN_AFIO_AF(GPIOE_MB2_RST, 0U) |     \
                                     PIN_AFIO_AF(GPIOE_MB2_INT, 0U) |     \
                                     PIN_AFIO_AF(GPIOE_PE15_GPIO, 0U))


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

// clang-format on

#endif /* BOARD_H */
