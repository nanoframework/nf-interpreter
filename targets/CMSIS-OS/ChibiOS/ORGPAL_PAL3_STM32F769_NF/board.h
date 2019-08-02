#ifndef BOARD_H
#define BOARD_H

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/*
 * Setup for STMicroelectronics ORGPAL STM32F769ZI board.
 */

/*
 * Board identifier.
 */
#define BOARD_ORGPAL_PAL3_STM32F769_NF
#define BOARD_NAME                  "OrgPal Pal 3 STM32F769ZI"

/*
 * Ethernet PHY type.
 */
#define BOARD_PHY_ID                MII_LAN8742A_ID
#define BOARD_PHY_RMII

/*
 * The board has an ULPI USB PHY.
 */
#define BOARD_OTG2_USES_ULPI

/*
 * Board oscillators-related settings.
 */
#if !defined(STM32_LSECLK)
#define STM32_LSECLK                32768U
#endif

#define STM32_LSEDRV                (3U << 3U)

#if !defined(STM32_HSECLK)
#define STM32_HSECLK                12000000U
#endif

/*
 * Board voltages.
 * Required for performance limits calculation.
 */
#define STM32_VDD                   330U

/*
 * MCU type as defined in the ST header.
 */
#define STM32F769xx

/*
 * IO pins assignments.
 */
#define GPIOA_BUTTON                0U
#define GPIOA_RMII_REF_CLK          1U 
#define GPIOA_RMII_MDIO             2U 
#define GPIOA_IO1_P17_ADC1_IN3_IO	3U 
#define GPIOA_IO0_P6_ADC1_IN4_IO    4U	
#define GPIOA_FLASH_SPI1_SCK        5U	
#define GPIOA_FLASH_SPI1_MISO       6U	
#define GPIOA_RMII_CRS_DV           7U
#define GPIOA_I2C3_SCL              8U
#define GPIOA_OTG_FS_VBUS			9U
#define GPIOA_OTG_FS_ID			    10U
#define GPIOA_OTG_FS_DM             11U
#define GPIOA_OTG_FS_DP             12U
#define GPIOA_SWDIO                 13U
#define GPIOA_SWCLK                 14U
#define GPIOA_JTDI                  15U

#define GPIOB_ADC1_IN8_BATTERY      0U
#define GPIOB_IO1_P18_ADC1_IN9      1U
#define GPIOB_BOOT1                 2U
#define GPIOB_SWO                   3U
#define GPIOB_JTRST                 4U
#define GPIOB_FLASH_SPI1_MOSI       5U	
#define GPIOB_FLASH_QSPI_NCS        6U	
#define GPIOB_PIN7                  7U
#define GPIOB_IO1_P7_IO             8U
#define GPIOB_IO1_P8_IO             9U
#define GPIOB_IO1_USART3_TX         10U
#define GPIOB_IO1_USART3_RX         11U
#define GPIOB_OTG_HS_ID             12U
#define GPIOB_OTG_HS_VBUS           13U
#define GPIOB_OTG_HS_DM             14U
#define GPIOB_OTG_HS_DP             15U

#define GPIOC_FMC_SDNWE             0U
#define GPIOC_RMII_MDC			    1U
#define GPIOC_ADC1_IN12_420MA       2U
#define GPIOC_ADC1_IN13_TEMP        3U
#define GPIOC_RMII_RXD0             4U
#define GPIOC_RMII_RXD1             5U
#define GPIOC_IO0_USART6_TX         6U
#define GPIOC_IO0_USART6_RX         7U
#define GPIOC_SD_D0                 8U
#define GPIOC_SD_D1		            9U
#define GPIOC_SD_D2		            10U
#define GPIOC_SD_D3		            11U
#define GPIOC_SD_CLK                12U
#define GPIOC_RTC                   13U
#define GPIOC_OSC32_IN              14U
#define GPIOC_OSC32_OUT             15U

#define GPIOD_FMC_D2                0U
#define GPIOD_FMC_D3                1U
#define GPIOD_SD_CMD                2U
#define GPIOD_RJ45_USART2_CTS       3U
#define GPIOD_RJ45_USART2_RTS       4U
#define GPIOD_RJ45_USART2_TX        5U
#define GPIOD_RJ45_USART2_RX        6U
#define GPIOD_PIN7                  7U
#define GPIOD_FMC_D13               8U
#define GPIOD_FMC_D14               9U
#define GPIOD_FMC_D15               10U
#define GPIOD_FLASH_QSPI_D0         11U
#define GPIOD_FLASH_QSPI_D1         12U
#define GPIOD_FLASH_QSPI_D3         13U
#define GPIOD_FMC_D0                14U
#define GPIOD_FMC_D1                15U


#define GPIOE_FMC_NBL0              0U
#define GPIOE_FMC_NBL1              1U
#define GPIOE_FLASH_QSPI_D2         2U
#define GPIOE_PIN3                  3U
#define GPIOE_PIN4                  4U
#define GPIOE_SD_CARD_DETECT        5U
#define GPIOE_PIN6                  6U
#define GPIOE_FMC_D4                7U
#define GPIOE_FMC_D5                8U
#define GPIOE_FMC_D6                9U
#define GPIOE_FMC_D7                10U
#define GPIOE_FMC_D8                11U
#define GPIOE_FMC_D9                12U
#define GPIOE_FMC_D10               13U
#define GPIOE_FMC_D11               14U
#define GPIOE_FMC_D12               15U

#define GPIOF_FMC_A0                0U
#define GPIOF_FMC_A1                1U
#define GPIOF_FMC_A2                2U
#define GPIOF_FMC_A3                3U
#define GPIOF_FMC_A4                4U
#define GPIOF_FMC_A5                5U
#define GPIOF_RS485_UART7_RX        6U
#define GPIOF_RS485_UART7_TX        7U
#define GPIOF_IO0_ADC3_IN6			8U
#define GPIOF_IO0_ADC3_IN7	        9U
#define GPIOF_FLASH_QSPI_CLK        10U
#define GPIOF_FMC_SDNRAS            11U
#define GPIOF_FMC_A6                12U
#define GPIOF_FMC_A7                13U
#define GPIOF_FMC_A8                14U
#define GPIOF_FMC_A9                15U

#define GPIOG_FMC_A10               0U
#define GPIOG_FMC_A11               1U
#define GPIOG_IO1_P23_INT           2U
#define GPIOG_IO1_P24_WAKEUP        3U
#define GPIOG_FMC_BA0               4U
#define GPIOG_FMC_BA1               5U
#define GPIOG_LED1                  6U
#define GPIOG_LED2                  7U
#define GPIOG_FMC_SDCLK             8U
#define GPIOG_IO0_P13_IO            9U
#define GPIOG_IO0_P19_IO            10U
#define GPIOG_RMII_TX_EN            11U
#define GPIOG_RMII_RST              12U
#define GPIOG_RMII_TXD0             13U
#define GPIOG_RMII_TXD1             14U
#define GPIOG_FMC_SDNCAS            15U

#define GPIOH_OSC_IN                0U
#define GPIOH_OSC_OUT               1U
#define GPIOH_PIN2                  2U
#define GPIOH_PIN3                  3U
#define GPIOH_I2C2_SCL              4U
#define GPIOH_I2C2_SDA              5U
#define GPIOH_FMC_SDNE1             6U
#define GPIOH_FMC_SDCKE1            7U
#define GPIOH_I2C3_SDA              8U
#define GPIOH_IO1_P12_PWM           9U
#define GPIOH_IO1_P13_IO            10U
#define GPIOH_IO1_P19_IO            11U
#define GPIOH_PWM_SPEAKER			12U
#define GPIOH_IO0_P7_IO             13U
#define GPIOH_IO0_P8_IO             14U
#define GPIOH_IO0_P12_IO           15U

#define GPIOI_IO0_P20_ON_OFF        0U
#define GPIOH_IO0_P24_IO            1U
#define GPIOH_IO0_P5_IO             2U
#define GPIOH_IO0_P23_INT           3U
#define GPIOI_KEYPAD_2              4U
#define GPIOI_KEYPAD_1              5U
#define GPIOI_KEYPAD_4              6U
#define GPIOI_KEYPAD_3              7U
#define GPIOI_PIN8                  8U
#define GPIOI_PIN9                  9U
#define GPIOI_RMII_RXER             10U
#define GPIOI_PIN11                 11U
#define GPIOI_RS485_RE              12U
#define GPIOI_RS485_SHDN            13U
#define GPIOI_RS485_BUS_TERM        14U
#define GPIOI_FLASH_SPI1_CS         15U


#define GPIOJ_IO0_PWR_ON_OFF        0U
#define GPIOJ_IO1_PWR_ON_OFF        1U
#define GPIOJ_PIN2                  2U
#define GPIOJ_SPI_FLASH_WP          3U
#define GPIOJ_SPI_FLASH_HOLD        4U
#define GPIOJ_RELAY					5U
#define GPIOJ_PIN6                  6U
#define GPIOJ_PIN7                  7U
#define GPIOJ_PIN8                  8U
#define GPIOJ_PIN9                  9U
#define GPIOJ_PIN10                 10U
#define GPIOJ_PIN11                 11U
#define GPIOJ_PULSE_COUNT           12U
#define GPIOJ_PIN13                 13U
#define GPIOJ_PIN14                 14U
#define GPIOJ_PIN15                 15U


#define GPIOK_PIN0                  0U
#define GPIOK_PIN1                  1U
#define GPIOK_PIN2                  2U
#define GPIOK_LCD_ENABLE            3U
#define GPIOK_IO1_P6_IO             4U
#define GPIOK_IO1_P5_IO             5U
#define GPIOK_IO1_P20_IO            6U
#define GPIOK_BUTTON_BOOT           7U
#define GPIOK_PIN8                  8U
#define GPIOK_PIN9                  9U
#define GPIOK_PIN10                 10U
#define GPIOK_PIN11                 11U
#define GPIOK_PIN12                 12U
#define GPIOK_PIN13                 13U
#define GPIOK_PIN14                 14U
#define GPIOK_PIN15                 15U

/*
 * IO lines assignments
 */
#define LINE_BUTTON_USER            PAL_LINE(GPIOA, 0U)
#define LINE_RMII_REF_CLK           PAL_LINE(GPIOA, 1U)
#define LINE_RMII_MDIO              PAL_LINE(GPIOA, 2U)
#define LINE_IO1_P17_ADC1_IN3		PAL_LINE(GPIOA, 3U)
#define LINE_IO0_P6_ADC1_IN4_IO     PAL_LINE(GPIOA, 4U)
#define LINE_FLASH_SPI1_SCK         PAL_LINE(GPIOA, 5U)
#define LINE_FLASH_SPI1_MISO        PAL_LINE(GPIOA, 6U)
#define LINE_RMII_CRS_DV            PAL_LINE(GPIOA, 7U)
#define LINE_I2C3_SCL               PAL_LINE(GPIOA, 8U)
#define LINE_OTG_FS_VBUS            PAL_LINE(GPIOA, 9U)
#define LINE_OTG_FS_ID				PAL_LINE(GPIOA, 10U)
#define LINE_OTG_FS_DM              PAL_LINE(GPIOA, 11U)
#define LINE_OTG_FS_DP              PAL_LINE(GPIOA, 12U)
#define LINE_SWDIO                  PAL_LINE(GPIOA, 13U)
#define LINE_SWCLK                  PAL_LINE(GPIOA, 14U)
#define LINE_JTDI					PAL_LINE(GPIOA, 15U)
#define LINE_ADC1_IN8_BATTERY       PAL_LINE(GPIOB, 0U)
#define LINE_BOOT1                  PAL_LINE(GPIOB, 2U)
#define LINE_SWO                    PAL_LINE(GPIOB, 3U)
#define LINE_JTRST                  PAL_LINE(GPIOB, 4U)
#define LINE_FLASH_SPI1_MOSI        PAL_LINE(GPIOB, 5U)
#define LINE_FLASH_QSPI_NCS         PAL_LINE(GPIOB, 6U)
#define LINE_IO1_USART3_TX          PAL_LINE(GPIOB, 10U)
#define LINE_IO1_USART3_RX          PAL_LINE(GPIOB, 11U)
#define LINE_OTG_HS_ID              PAL_LINE(GPIOB, 12U)
#define LINE_OTG_HS_VBUS            PAL_LINE(GPIOB, 13U)
#define LINE_OTG_HS_DM              PAL_LINE(GPIOB, 14U)
#define LINE_OTG_HS_DP              PAL_LINE(GPIOB, 15U)
#define LINE_FMC_SDNWE              PAL_LINE(GPIOC, 0U)
#define LINE_RMII_MDC               PAL_LINE(GPIOC, 1U)
#define LINE_ADC1_IN12_420MA        PAL_LINE(GPIOC, 2U)
#define LINE_ADC1_IN13_TEMP         PAL_LINE(GPIOC, 3U)
#define LINE_RMII_RXD0              PAL_LINE(GPIOC, 4U)
#define LINE_RMII_RXD1              PAL_LINE(GPIOC, 5U)
#define LINE_IO0_USART6_TX          PAL_LINE(GPIOC, 6U)
#define LINE_IO0_USART6_RX          PAL_LINE(GPIOC, 7U)
#define LINE_SD_D0					PAL_LINE(GPIOC, 8U)
#define LINE_SD_D1					PAL_LINE(GPIOC, 9U)
#define LINE_SD_D2                  PAL_LINE(GPIOC, 10U)
#define LINE_SD_D3                  PAL_LINE(GPIOC, 11U)
#define LINE_SD_CLK                 PAL_LINE(GPIOC, 12U)
#define LINE_LED2                   PAL_LINE(GPIOC, 13U)
#define LINE_OSC32_IN               PAL_LINE(GPIOC, 14U)
#define LINE_OSC32_OUT              PAL_LINE(GPIOC, 15U)
#define LINE_FMC_D2                 PAL_LINE(GPIOD, 0U)
#define LINE_FMC_D3                 PAL_LINE(GPIOD, 1U)
#define LINE_SD_CMD                 PAL_LINE(GPIOD, 2U)
#define LINE_RJ45_USART2_CTS        PAL_LINE(GPIOD, 3U)
#define LINE_RJ45_USART2_RTS        PAL_LINE(GPIOD, 4U)
#define LINE_RJ45_USART2_TX         PAL_LINE(GPIOD, 5U)
#define LINE_RJ45_USART2_RX         PAL_LINE(GPIOD, 6U)
#define LINE_FMC_D13                PAL_LINE(GPIOD, 8U)
#define LINE_FMC_D14                PAL_LINE(GPIOD, 9U)
#define LINE_FMC_D15                PAL_LINE(GPIOD, 10U)
#define LINE_FLASH_QSPI_D0          PAL_LINE(GPIOD, 11U)
#define LINE_FLASH_QSPI_D1          PAL_LINE(GPIOD, 12U)
#define LINE_FLASH_QSPI_D2          PAL_LINE(GPIOD, 2U) 
#define LINE_FLASH_QSPI_D3          PAL_LINE(GPIOD, 13U)
#define LINE_FMC_D0                 PAL_LINE(GPIOD, 14U)
#define LINE_FMC_D1                 PAL_LINE(GPIOD, 15U)
#define LINE_FMC_NBL0               PAL_LINE(GPIOE, 0U)
#define LINE_FMC_NBL1               PAL_LINE(GPIOE, 1U)
#define LINE_SD_CARD_DETECT         PAL_LINE(GPIOE, 5U) 
#define LINE_FMC_D4                 PAL_LINE(GPIOE, 7U)
#define LINE_FMC_D5                 PAL_LINE(GPIOE, 8U)
#define LINE_FMC_D6                 PAL_LINE(GPIOE, 9U)
#define LINE_FMC_D7                 PAL_LINE(GPIOE, 10U)
#define LINE_FMC_D8                 PAL_LINE(GPIOE, 11U)
#define LINE_FMC_D9                 PAL_LINE(GPIOE, 12U)
#define LINE_FMC_D10                PAL_LINE(GPIOE, 13U)
#define LINE_FMC_D11                PAL_LINE(GPIOE, 14U)
#define LINE_FMC_D12                PAL_LINE(GPIOE, 15U)
#define LINE_FMC_A0                 PAL_LINE(GPIOF, 0U)
#define LINE_FMC_A1                 PAL_LINE(GPIOF, 1U)
#define LINE_FMC_A2                 PAL_LINE(GPIOF, 2U)
#define LINE_FMC_A3                 PAL_LINE(GPIOF, 3U)
#define LINE_FMC_A4                 PAL_LINE(GPIOF, 4U)
#define LINE_FMC_A5                 PAL_LINE(GPIOF, 5U)
#define LINE_RS485_UART7_RX         PAL_LINE(GPIOF, 6U)
#define LINE_RS485_UART7_TX         PAL_LINE(GPIOF, 7U)
#define LINE_FLASH_QSPI_CLK         PAL_LINE(GPIOF, 10U)
#define LINE_FMC_SDNRAS             PAL_LINE(GPIOF, 11U)
#define LINE_FMC_A6                 PAL_LINE(GPIOF, 12U)
#define LINE_FMC_A7                 PAL_LINE(GPIOF, 13U)
#define LINE_FMC_A8                 PAL_LINE(GPIOF, 14U)
#define LINE_FMC_A9                 PAL_LINE(GPIOF, 15U)
#define LINE_FMC_A10                PAL_LINE(GPIOG, 0U)
#define LINE_FMC_A11                PAL_LINE(GPIOG, 1U)
#define LINE_IO1_P23_INT_IO         PAL_LINE(GPIOG, 2U)
#define LINE_IO1_P24_WAKEUP_IO      PAL_LINE(GPIOG, 3U)
#define LINE_FMC_BA0                PAL_LINE(GPIOG, 4U)
#define LINE_FMC_BA1                PAL_LINE(GPIOG, 5U)
#define LINE_LED1                   PAL_LINE(GPIOG, 6U)
#define LINE_FMC_SDCLK              PAL_LINE(GPIOG, 8U)
#define LINE_RMII_TX_EN             PAL_LINE(GPIOG, 11U)
#define LINE_RMII_RST               PAL_LINE(GPIOG, 12U)
#define LINE_RMII_TXD0              PAL_LINE(GPIOG, 13U)
#define LINE_RMII_TXD1              PAL_LINE(GPIOG, 14U)
#define LINE_FMC_SDNCAS             PAL_LINE(GPIOG, 15U)
#define LINE_OSC_IN                 PAL_LINE(GPIOH, 0U)
#define LINE_OSC_OUT                PAL_LINE(GPIOH, 1U)
#define LINE_I2C2_SCL               PAL_LINE(GPIOH, 4U)
#define LINE_I2C2_SDA               PAL_LINE(GPIOH, 5U)
#define LINE_FMC_SDNE1              PAL_LINE(GPIOH, 6U)
#define LINE_FMC_SDCKE1             PAL_LINE(GPIOH, 7U)
#define LINE_I2C3_SDA               PAL_LINE(GPIOH, 8U)
#define LINE_IO0_P24_WAKEUP_IO      PAL_LINE(GPIOI, 1U)
#define LINE_SPI2_MISO              PAL_LINE(GPIOI, 2U)
#define LINE_IO0_P23_INT_IO         PAL_LINE(GPIOI, 3U)
#define LINE_KEYPAD_2               PAL_LINE(GPIOI, 4U)
#define LINE_KEYPAD_1               PAL_LINE(GPIOI, 5U)
#define LINE_KEYPAD_4               PAL_LINE(GPIOI, 6U)
#define LINE_KEYPAD_3               PAL_LINE(GPIOI, 7U)
#define LINE_RMII_RXER              PAL_LINE(GPIOI, 10U)
#define LINE_FLASH_SPI1_CS          PAL_LINE(GPIOI, 15U)
#define LINE_SPI_FLASH_WP           PAL_LINE(GPIOJ, 3U)
#define LINE_SPI_FLASH_HOLD         PAL_LINE(GPIOJ, 4U)
#define LINE_RELAY			        PAL_LINE(GPIOJ, 5U)
#define LINE_PULSE_COUNT	        PAL_LINE(GPIOJ, 12U)
#define LINE_LCD_ENABLE		        PAL_LINE(GPIOK, 3U)
#define LINE_IO1_P6_IO		        PAL_LINE(GPIOK, 4U)
#define LINE_IO1_P5_IO		        PAL_LINE(GPIOK, 5U)
#define LINE_IO1_P20_ON_OFF         PAL_LINE(GPIOK, 6U)
#define LINE_BUTTON_BOOT	        PAL_LINE(GPIOK, 7U)


/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*
By default, STM32F4 pins are configured as inputs, except some JTAG pins which can impact the power 
consumption of the device in different power modes because pins are very sensitive to external noise 
in input mode I / O.

To avoid extra I/O current, all pins should be configured as analog input(AIN); in this mode the 
Schmitt trigger input is disabled, providing zero consumption for each I / O pin. 

We recommend that the I / O speed frequency(driving level) be configured at the lowest possible speed 
or as an output push - pull configuration, outputting 0 to the ODR.

The user should also disable the MCO pin of the clock output if not used.
*/


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
* PA0  - BUTTON                    (input floating).
* PA1  - RMII_REF_CLK              (alternate 11).
* PA2  - RMII_MDIO                 (alternate 11).
* PA3  - IO1_P17_ADC1_IN3          (input pullup).
* PA4  - IO0_P6_ADC1_IN4           (input pullup).
* PA5  - SPI1_SCK                  (alternate 5).
* PA6  - SPI1_MISO                 (alternate 5).
* PA7  - RMII_CRS_DV               (alternate 11).
* PA8  - I2C3_SCL                  (input pullup).
* PA9  - OTG_FS_VBUS               (analog).
* PA10 - OTG_FS_ID				   (alternate 10).
* PA11 - OTG_FS_DM                 (alternate 10).
* PA12 - OTG_FS_DP                 (alternate 10).
* PA13 - SWDIO                     (alternate 0).
* PA14 - SWCLK                     (alternate 0).
* PA15 - JTDI                      (alternate 0).
*/
#define VAL_GPIOA_MODER            (PIN_MODE_INPUT(GPIOA_BUTTON) |         \
                                     PIN_MODE_ALTERNATE(GPIOA_RMII_REF_CLK) |\
                                     PIN_MODE_ALTERNATE(GPIOA_RMII_MDIO) |  \
                                     PIN_MODE_INPUT(GPIOA_IO1_P17_ADC1_IN3_IO) |   \
                                     PIN_MODE_ALTERNATE(GPIOA_IO0_P6_ADC1_IN4_IO) |   \
                                     PIN_MODE_ALTERNATE(GPIOA_FLASH_SPI1_SCK) |   \
                                     PIN_MODE_ALTERNATE(GPIOA_FLASH_SPI1_MISO) |  \
                                     PIN_MODE_ALTERNATE(GPIOA_RMII_CRS_DV) | \
                                     PIN_MODE_INPUT(GPIOA_I2C3_SCL) |   \
                                     PIN_MODE_ANALOG(GPIOA_OTG_FS_VBUS) |    \
                                     PIN_MODE_ALTERNATE(GPIOA_OTG_FS_ID) |  \
                                     PIN_MODE_ALTERNATE(GPIOA_OTG_FS_DM) |     \
                                     PIN_MODE_ALTERNATE(GPIOA_OTG_FS_DP) |     \
                                     PIN_MODE_ALTERNATE(GPIOA_SWDIO) |      \
                                     PIN_MODE_ALTERNATE(GPIOA_SWCLK) |      \
                                     PIN_MODE_ALTERNATE(GPIOA_JTDI))
#define VAL_GPIOA_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOA_BUTTON) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOA_RMII_REF_CLK) |\
                                     PIN_OTYPE_PUSHPULL(GPIOA_RMII_MDIO) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOA_IO1_P17_ADC1_IN3_IO) | \
                                     PIN_OTYPE_PUSHPULL(GPIOA_IO0_P6_ADC1_IN4_IO) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOA_FLASH_SPI1_SCK) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOA_FLASH_SPI1_MISO) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOA_RMII_CRS_DV) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOA_I2C3_SCL) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOA_OTG_FS_VBUS) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOA_OTG_FS_ID) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOA_OTG_FS_DM) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOA_OTG_FS_DP) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOA_SWDIO) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOA_SWCLK) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOA_JTDI))
#define VAL_GPIOA_OSPEEDR           (PIN_OSPEED_VERYLOW(GPIOA_BUTTON) |     \
                                     PIN_OSPEED_HIGH(GPIOA_RMII_REF_CLK) |  \
                                     PIN_OSPEED_HIGH(GPIOA_RMII_MDIO) |     \
                                     PIN_OSPEED_HIGH(GPIOA_IO1_P17_ADC1_IN3_IO) |   \
                                     PIN_OSPEED_HIGH(GPIOA_IO0_P6_ADC1_IN4_IO) |     \
                                     PIN_OSPEED_HIGH(GPIOA_FLASH_SPI1_SCK) |      \
                                     PIN_OSPEED_HIGH(GPIOA_FLASH_SPI1_MISO) |     \
                                     PIN_OSPEED_VERYLOW(GPIOA_RMII_CRS_DV) |\
                                     PIN_OSPEED_HIGH(GPIOA_I2C3_SCL) |      \
                                     PIN_OSPEED_HIGH(GPIOA_OTG_FS_VBUS) |    \
                                     PIN_OSPEED_HIGH(GPIOA_OTG_FS_ID) |    \
                                     PIN_OSPEED_HIGH(GPIOA_OTG_FS_DM) |        \
                                     PIN_OSPEED_HIGH(GPIOA_OTG_FS_DP) |        \
                                     PIN_OSPEED_HIGH(GPIOA_SWDIO) |         \
                                     PIN_OSPEED_HIGH(GPIOA_SWCLK) |         \
                                     PIN_OSPEED_HIGH(GPIOA_JTDI))
#define VAL_GPIOA_PUPDR             (PIN_PUPDR_FLOATING(GPIOA_BUTTON) |     \
                                     PIN_PUPDR_FLOATING(GPIOA_RMII_REF_CLK) |\
                                     PIN_PUPDR_PULLUP(GPIOA_RMII_MDIO) |    \
                                     PIN_PUPDR_PULLUP(GPIOA_IO1_P17_ADC1_IN3_IO) | \
                                     PIN_PUPDR_FLOATING(GPIOA_IO0_P6_ADC1_IN4_IO) |  \
                                     PIN_PUPDR_FLOATING(GPIOA_FLASH_SPI1_SCK) |   \
                                     PIN_PUPDR_FLOATING(GPIOA_FLASH_SPI1_MISO) |  \
                                     PIN_PUPDR_FLOATING(GPIOA_RMII_CRS_DV) |\
                                     PIN_PUPDR_PULLUP(GPIOA_I2C3_SCL) |   \
                                     PIN_PUPDR_FLOATING(GPIOA_OTG_FS_VBUS) |      \
                                     PIN_PUPDR_FLOATING(GPIOA_OTG_FS_ID) |      \
                                     PIN_PUPDR_FLOATING(GPIOA_OTG_FS_DM) |     \
                                     PIN_PUPDR_FLOATING(GPIOA_OTG_FS_DP) |     \
                                     PIN_PUPDR_PULLUP(GPIOA_SWDIO) |        \
                                     PIN_PUPDR_PULLDOWN(GPIOA_SWCLK) |      \
                                     PIN_PUPDR_FLOATING(GPIOA_JTDI))
#define VAL_GPIOA_ODR               (PIN_ODR_HIGH(GPIOA_BUTTON) |           \
                                     PIN_ODR_HIGH(GPIOA_RMII_REF_CLK) |     \
                                     PIN_ODR_HIGH(GPIOA_RMII_MDIO) |        \
                                     PIN_ODR_HIGH(GPIOA_IO1_P17_ADC1_IN3_IO) |      \
                                     PIN_ODR_HIGH(GPIOA_IO0_P6_ADC1_IN4_IO) |        \
                                     PIN_ODR_HIGH(GPIOA_FLASH_SPI1_SCK) |        \
                                     PIN_ODR_HIGH(GPIOA_FLASH_SPI1_MISO) |       \
                                     PIN_ODR_HIGH(GPIOA_RMII_CRS_DV) |      \
                                     PIN_ODR_HIGH(GPIOA_I2C3_SCL) |         \
                                     PIN_ODR_HIGH(GPIOA_OTG_FS_VBUS) |          \
                                     PIN_ODR_HIGH(GPIOA_OTG_FS_ID) |          \
                                     PIN_ODR_HIGH(GPIOA_OTG_FS_DM) |           \
                                     PIN_ODR_HIGH(GPIOA_OTG_FS_DP) |           \
                                     PIN_ODR_HIGH(GPIOA_SWDIO) |            \
                                     PIN_ODR_HIGH(GPIOA_SWCLK) |            \
                                     PIN_ODR_HIGH(GPIOA_JTDI))
#define VAL_GPIOA_AFRL              (PIN_AFIO_AF(GPIOA_BUTTON, 0U) |        \
                                     PIN_AFIO_AF(GPIOA_RMII_REF_CLK, 11U) | \
                                     PIN_AFIO_AF(GPIOA_RMII_MDIO, 11U) |    \
                                     PIN_AFIO_AF(GPIOA_IO1_P17_ADC1_IN3_IO, 0U) |   \
                                     PIN_AFIO_AF(GPIOA_IO0_P6_ADC1_IN4_IO, 5U) |      \
                                     PIN_AFIO_AF(GPIOA_FLASH_SPI1_SCK, 5U) |      \
                                     PIN_AFIO_AF(GPIOA_FLASH_SPI1_MISO, 5U) |     \
                                     PIN_AFIO_AF(GPIOA_RMII_CRS_DV, 11U))
#define VAL_GPIOA_AFRH              (PIN_AFIO_AF(GPIOA_I2C3_SCL, 0U) |      \
                                     PIN_AFIO_AF(GPIOA_OTG_FS_VBUS, 0U) |       \
                                     PIN_AFIO_AF(GPIOA_OTG_FS_ID, 10U) |  \
                                     PIN_AFIO_AF(GPIOA_OTG_FS_DM, 10U) |       \
                                     PIN_AFIO_AF(GPIOA_OTG_FS_DP, 10U) |       \
                                     PIN_AFIO_AF(GPIOA_SWDIO, 0U) |         \
                                     PIN_AFIO_AF(GPIOA_SWCLK, 0U) |         \
                                     PIN_AFIO_AF(GPIOA_JTDI, 0U))

/*
* GPIOB setup:
*
* PB0  - ADC1_IN8_BATTERY          (input pullup).
* PB1  - IO1_P18_ADC1_IN9          (input pullup).
* PB2  - BOOT1                     (input pullup).
* PB3  - SWO                       (alternate 0).
* PB4  - JTRST                     (alternate 0).
* PB5  - SPI1_MOSI                 (alternate 5).
* PB6  - QSPI_NCS                  (alternate 10).
* PB7  - PIN7                      (input pullup).
* PB8  - GPIOB_IO1_P7_IO           (input floating).
* PB9  - GPIOB_IO1_P8_IO           (input floating).
* PB10  - USART3_TX				   (alternate 7).
* PB11  - USART3_RX				   (alternate 7).
* PB12 - OTG_HS_ID                 (input pulldown).
* PB13 - OTG_HS_VBUS               (input pulldown).
* PB14 - OTG_HS_DM                 (alternate 12).
* PB15 - OTG_HS_DP                 (alternate 12).
*/
#define VAL_GPIOB_MODER             (PIN_MODE_INPUT(GPIOB_ADC1_IN8_BATTERY) |     \
                                     PIN_MODE_INPUT(GPIOB_IO1_P18_ADC1_IN9) |     \
                                     PIN_MODE_INPUT(GPIOB_BOOT1) |          \
                                     PIN_MODE_ALTERNATE(GPIOB_SWO) |        \
                                     PIN_MODE_ALTERNATE(GPIOB_JTRST) |       \
                                     PIN_MODE_ALTERNATE(GPIOB_FLASH_SPI1_MOSI) | \
									 PIN_MODE_ALTERNATE(GPIOB_FLASH_QSPI_NCS) |       \
                                     PIN_MODE_INPUT(GPIOB_PIN7) |           \
                                     PIN_MODE_INPUT(GPIOB_IO1_P7_IO) |     \
                                     PIN_MODE_INPUT(GPIOB_IO1_P8_IO) |     \
                                     PIN_MODE_ALTERNATE(GPIOB_IO1_USART3_TX) |     \
                                     PIN_MODE_ALTERNATE(GPIOB_IO1_USART3_RX) |     \
                                     PIN_MODE_ANALOG(GPIOB_OTG_HS_ID) |  \
                                     PIN_MODE_ANALOG(GPIOB_OTG_HS_VBUS) |    \
                                     PIN_MODE_ALTERNATE(GPIOB_OTG_HS_DM) |  \
                                     PIN_MODE_ALTERNATE(GPIOB_OTG_HS_DP))
#define VAL_GPIOB_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOB_ADC1_IN8_BATTERY) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOB_IO1_P18_ADC1_IN9) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOB_BOOT1) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOB_SWO) |        \
                                     PIN_OTYPE_PUSHPULL(GPIOB_JTRST) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOB_FLASH_SPI1_MOSI) | \
                                     PIN_OTYPE_PUSHPULL(GPIOB_FLASH_QSPI_NCS) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOB_PIN7) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOB_IO1_P7_IO) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOB_IO1_P8_IO) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOB_IO1_USART3_TX) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOB_IO1_USART3_RX) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOB_OTG_HS_ID) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOB_OTG_HS_VBUS) |\
                                     PIN_OTYPE_PUSHPULL(GPIOB_OTG_HS_DM) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOB_OTG_HS_DP))
#define VAL_GPIOB_OSPEEDR           (PIN_OSPEED_HIGH(GPIOB_ADC1_IN8_BATTERY) |        \
                                     PIN_OSPEED_HIGH(GPIOB_IO1_P18_ADC1_IN9) |        \
                                     PIN_OSPEED_HIGH(GPIOB_BOOT1) |         \
                                     PIN_OSPEED_HIGH(GPIOB_SWO) |           \
                                     PIN_OSPEED_HIGH(GPIOB_JTRST) |       \
                                     PIN_OSPEED_HIGH(GPIOB_FLASH_SPI1_MOSI) |    \
                                     PIN_OSPEED_HIGH(GPIOB_FLASH_QSPI_NCS) |     \
                                     PIN_OSPEED_VERYLOW(GPIOB_PIN7) |       \
                                     PIN_OSPEED_LOW(GPIOB_IO1_P7_IO) |        \
                                     PIN_OSPEED_LOW(GPIOB_IO1_P8_IO) |        \
                                     PIN_OSPEED_HIGH(GPIOB_IO1_USART3_TX) |        \
                                     PIN_OSPEED_HIGH(GPIOB_IO1_USART3_RX) |        \
                                     PIN_OSPEED_HIGH(GPIOB_OTG_HS_ID) |     \
                                     PIN_OSPEED_HIGH(GPIOB_OTG_HS_VBUS) |\
                                     PIN_OSPEED_HIGH(GPIOB_OTG_HS_DM) |     \
                                     PIN_OSPEED_HIGH(GPIOB_OTG_HS_DP))
#define VAL_GPIOB_PUPDR             (PIN_PUPDR_PULLUP(GPIOB_ADC1_IN8_BATTERY) |     \
                                     PIN_PUPDR_PULLUP(GPIOB_IO1_P18_ADC1_IN9) |     \
                                     PIN_PUPDR_PULLUP(GPIOB_BOOT1) |        \
                                     PIN_PUPDR_FLOATING(GPIOB_SWO) |        \
                                     PIN_PUPDR_FLOATING(GPIOB_JTRST) |         \
                                     PIN_PUPDR_FLOATING(GPIOB_FLASH_SPI1_MOSI) | \
                                     PIN_PUPDR_FLOATING(GPIOB_FLASH_QSPI_NCS) |     \
                                     PIN_PUPDR_PULLUP(GPIOB_PIN7) |         \
                                     PIN_PUPDR_FLOATING(GPIOB_IO1_P7_IO) |     \
                                     PIN_PUPDR_FLOATING(GPIOB_IO1_P8_IO) |     \
                                     PIN_PUPDR_FLOATING(GPIOB_IO1_USART3_TX) |     \
                                     PIN_PUPDR_FLOATING(GPIOB_IO1_USART3_RX) |     \
                                     PIN_PUPDR_FLOATING(GPIOB_OTG_HS_ID) |  \
                                     PIN_PUPDR_FLOATING(GPIOB_OTG_HS_VBUS) |\
                                     PIN_PUPDR_FLOATING(GPIOB_OTG_HS_DM) |  \
                                     PIN_PUPDR_FLOATING(GPIOB_OTG_HS_DP))
#define VAL_GPIOB_ODR               (PIN_ODR_HIGH(GPIOB_ADC1_IN8_BATTERY) |           \
                                     PIN_ODR_HIGH(GPIOB_IO1_P18_ADC1_IN9) |           \
                                     PIN_ODR_HIGH(GPIOB_BOOT1) |            \
                                     PIN_ODR_HIGH(GPIOB_SWO) |              \
                                     PIN_ODR_HIGH(GPIOB_JTRST) |             \
                                     PIN_ODR_HIGH(GPIOB_FLASH_SPI1_MOSI) |       \
                                     PIN_ODR_HIGH(GPIOB_FLASH_QSPI_NCS) |        \
                                     PIN_ODR_HIGH(GPIOB_PIN7) |             \
                                     PIN_ODR_HIGH(GPIOB_IO1_P7_IO) |           \
                                     PIN_ODR_HIGH(GPIOB_IO1_P8_IO) |           \
                                     PIN_ODR_HIGH(GPIOB_IO1_USART3_TX) |           \
                                     PIN_ODR_HIGH(GPIOB_IO1_USART3_RX) |           \
                                     PIN_ODR_HIGH(GPIOB_OTG_HS_ID) |        \
                                     PIN_ODR_HIGH(GPIOB_OTG_HS_VBUS) |      \
                                     PIN_ODR_HIGH(GPIOB_OTG_HS_DM) |        \
                                     PIN_ODR_HIGH(GPIOB_OTG_HS_DP))
#define VAL_GPIOB_AFRL              (PIN_AFIO_AF(GPIOB_ADC1_IN8_BATTERY, 0U) |       \
                                     PIN_AFIO_AF(GPIOB_IO1_P18_ADC1_IN9, 0U) |       \
                                     PIN_AFIO_AF(GPIOB_BOOT1, 0U) |         \
                                     PIN_AFIO_AF(GPIOB_SWO, 0U) |           \
                                     PIN_AFIO_AF(GPIOB_JTRST, 0U) |          \
                                     PIN_AFIO_AF(GPIOB_FLASH_SPI1_MOSI, 5U) |   \
                                     PIN_AFIO_AF(GPIOB_FLASH_QSPI_NCS, 10U) |      \
                                     PIN_AFIO_AF(GPIOB_PIN7, 0U))
#define VAL_GPIOB_AFRH              (PIN_AFIO_AF(GPIOB_IO1_P7_IO, 0U) |       \
                                     PIN_AFIO_AF(GPIOB_IO1_P8_IO, 0U) |       \
                                     PIN_AFIO_AF(GPIOB_IO1_USART3_TX, 7U) |       \
                                     PIN_AFIO_AF(GPIOB_IO1_USART3_RX, 7U) |       \
                                     PIN_AFIO_AF(GPIOB_OTG_HS_ID, 0U) |    \
                                     PIN_AFIO_AF(GPIOB_OTG_HS_VBUS, 0U) |   \
                                     PIN_AFIO_AF(GPIOB_OTG_HS_DM, 12U) |    \
                                     PIN_AFIO_AF(GPIOB_OTG_HS_DP, 12U))

/*
* GPIOC setup:
*
* PC0  - FMC_SDNWE                 (alternate 12).
* PC1  - RMII_MDC                  (alternate 11).
* PC2  - ADC1_IN12_420MA           (input pullup).
* PC3  - ADC1_IN13_TEMP            (input pullup).
* PC4  - RMII_RXD0                 (alternate 11).
* PC5  - RMII_RXD1                 (alternate 11).
* PC6  - USART6_TX                 (alternate 8).
* PC7  - USART6_RX                 (alternate 8).
* PC8  - SD_D0                     (alternate 12).
* PC9  - SD_D1                     (alternate 12).
* PC10 - SD_D2                     (alternate 12).
* PC11 - SD_D3                     (alternate 12).
* PC12 - SD_CK                     (alternate 12).
* PC13 - RTC                       (input floating);
* PC14 - OSC32_IN                  (input floating).
* PC15 - OSC32_OUT                 (input floating).
*/
#define VAL_GPIOC_MODER             (PIN_MODE_ALTERNATE(GPIOC_FMC_SDNWE) |  \
                                     PIN_MODE_ALTERNATE(GPIOC_RMII_MDC) |   \
                                     PIN_MODE_INPUT(GPIOC_ADC1_IN12_420MA) |   \
                                     PIN_MODE_INPUT(GPIOC_ADC1_IN13_TEMP) |           \
                                     PIN_MODE_ALTERNATE(GPIOC_RMII_RXD0) |  \
                                     PIN_MODE_ALTERNATE(GPIOC_RMII_RXD1) |  \
                                     PIN_MODE_ALTERNATE(GPIOC_IO0_USART6_TX) |  \
                                     PIN_MODE_ALTERNATE(GPIOC_IO0_USART6_RX) |     \
                                     PIN_MODE_ALTERNATE(GPIOC_SD_D0) |           \
                                     PIN_MODE_ALTERNATE(GPIOC_SD_D1) |   \
                                     PIN_MODE_ALTERNATE(GPIOC_SD_D2) |     \
                                     PIN_MODE_ALTERNATE(GPIOC_SD_D3) |          \
                                     PIN_MODE_ALTERNATE(GPIOC_SD_CLK) |          \
                                     PIN_MODE_INPUT(GPIOC_RTC) |	\
                                     PIN_MODE_INPUT(GPIOC_OSC32_IN) |       \
                                     PIN_MODE_INPUT(GPIOC_OSC32_OUT))
#define VAL_GPIOC_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOC_FMC_SDNWE) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOC_RMII_MDC) |\
                                     PIN_OTYPE_PUSHPULL(GPIOC_ADC1_IN12_420MA) |\
                                     PIN_OTYPE_PUSHPULL(GPIOC_ADC1_IN13_TEMP) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOC_RMII_RXD0) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOC_RMII_RXD1) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOC_IO0_USART6_TX) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOC_IO0_USART6_RX) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOC_SD_D0) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOC_SD_D1) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOC_SD_D2) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOC_SD_D3) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOC_SD_CLK) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOC_RTC) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOC_OSC32_IN) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOC_OSC32_OUT))
#define VAL_GPIOC_OSPEEDR           (PIN_OSPEED_HIGH(GPIOC_FMC_SDNWE) |     \
                                     PIN_OSPEED_HIGH(GPIOC_RMII_MDC) |  \
                                     PIN_OSPEED_HIGH(GPIOC_ADC1_IN12_420MA) |   \
                                     PIN_OSPEED_HIGH(GPIOC_ADC1_IN13_TEMP) |       \
                                     PIN_OSPEED_HIGH(GPIOC_RMII_RXD0) |     \
                                     PIN_OSPEED_HIGH(GPIOC_RMII_RXD1) |     \
                                     PIN_OSPEED_HIGH(GPIOC_IO0_USART6_TX) |     \
                                     PIN_OSPEED_HIGH(GPIOC_IO0_USART6_RX) |        \
                                     PIN_OSPEED_HIGH(GPIOC_SD_D0) |       \
                                     PIN_OSPEED_HIGH(GPIOC_SD_D1) |      \
                                     PIN_OSPEED_HIGH(GPIOC_SD_D2) |        \
                                     PIN_OSPEED_HIGH(GPIOC_SD_D3) |      \
                                     PIN_OSPEED_HIGH(GPIOC_SD_CLK) |      \
                                     PIN_OSPEED_HIGH(GPIOC_RTC) |      \
                                     PIN_OSPEED_HIGH(GPIOC_OSC32_IN) |      \
                                     PIN_OSPEED_HIGH(GPIOC_OSC32_OUT))
#define VAL_GPIOC_PUPDR             (PIN_PUPDR_FLOATING(GPIOC_FMC_SDNWE) |  \
                                     PIN_PUPDR_FLOATING(GPIOC_RMII_MDC) |\
                                     PIN_PUPDR_PULLUP(GPIOC_ADC1_IN12_420MA) |\
                                     PIN_PUPDR_PULLUP(GPIOC_ADC1_IN13_TEMP) |         \
                                     PIN_PUPDR_FLOATING(GPIOC_RMII_RXD0) |  \
                                     PIN_PUPDR_FLOATING(GPIOC_RMII_RXD1) |  \
                                     PIN_PUPDR_FLOATING(GPIOC_IO0_USART6_TX) |  \
                                     PIN_PUPDR_FLOATING(GPIOC_IO0_USART6_RX) |     \
                                     PIN_PUPDR_FLOATING(GPIOC_SD_D0) |         \
                                     PIN_PUPDR_FLOATING(GPIOC_SD_D1) |   \
                                     PIN_PUPDR_FLOATING(GPIOC_SD_D2) |     \
                                     PIN_PUPDR_FLOATING(GPIOC_SD_D3) |        \
                                     PIN_PUPDR_FLOATING(GPIOC_SD_CLK) |        \
                                     PIN_PUPDR_FLOATING(GPIOC_RTC) |        \
                                     PIN_PUPDR_FLOATING(GPIOC_OSC32_IN) |   \
                                     PIN_PUPDR_FLOATING(GPIOC_OSC32_OUT))
#define VAL_GPIOC_ODR               (PIN_ODR_HIGH(GPIOC_FMC_SDNWE) |        \
                                     PIN_ODR_HIGH(GPIOC_RMII_MDC) |     \
                                     PIN_ODR_HIGH(GPIOC_ADC1_IN12_420MA) |      \
                                     PIN_ODR_HIGH(GPIOC_ADC1_IN13_TEMP) |             \
                                     PIN_ODR_HIGH(GPIOC_RMII_RXD0) |        \
                                     PIN_ODR_HIGH(GPIOC_RMII_RXD1) |        \
                                     PIN_ODR_HIGH(GPIOC_IO0_USART6_TX) |        \
                                     PIN_ODR_HIGH(GPIOC_IO0_USART6_RX) |           \
                                     PIN_ODR_HIGH(GPIOC_SD_D0) |             \
                                     PIN_ODR_HIGH(GPIOC_SD_D1) |         \
                                     PIN_ODR_HIGH(GPIOC_SD_D2) |           \
                                     PIN_ODR_HIGH(GPIOC_SD_D3) |            \
                                     PIN_ODR_HIGH(GPIOC_SD_CLK) |            \
                                     PIN_ODR_HIGH(GPIOC_RTC) |            \
                                     PIN_ODR_HIGH(GPIOC_OSC32_IN) |         \
                                     PIN_ODR_HIGH(GPIOC_OSC32_OUT))
#define VAL_GPIOC_AFRL              (PIN_AFIO_AF(GPIOC_FMC_SDNWE, 12U) |    \
                                     PIN_AFIO_AF(GPIOC_RMII_MDC, 11U) |  \
                                     PIN_AFIO_AF(GPIOC_ADC1_IN12_420MA, 0U) |   \
                                     PIN_AFIO_AF(GPIOC_ADC1_IN13_TEMP, 0U) |          \
                                     PIN_AFIO_AF(GPIOC_RMII_RXD0, 11U) |    \
                                     PIN_AFIO_AF(GPIOC_RMII_RXD1, 11U) |    \
                                     PIN_AFIO_AF(GPIOC_IO0_USART6_TX, 8U) |    \
                                     PIN_AFIO_AF(GPIOC_IO0_USART6_RX, 8U))
#define VAL_GPIOC_AFRH              (PIN_AFIO_AF(GPIOC_SD_D0, 12U) |          \
                                     PIN_AFIO_AF(GPIOC_SD_D1, 12U) |      \
                                     PIN_AFIO_AF(GPIOC_SD_D2, 12U) |       \
                                     PIN_AFIO_AF(GPIOC_SD_D3, 12U) |         \
                                     PIN_AFIO_AF(GPIOC_SD_CLK, 12U) |         \
                                     PIN_AFIO_AF(GPIOC_RTC, 0U) |         \
                                     PIN_AFIO_AF(GPIOC_OSC32_IN, 0U) |      \
                                     PIN_AFIO_AF(GPIOC_OSC32_OUT, 0U))

/*
* GPIOD setup:
*
* PD0  - FMC_D2                    (alternate 12).
* PD1  - FMC_D3                    (alternate 12).
* PD2  - SD_CMD                    (alternate 12).
* PD3  - USART2_CTS                (alternate 7).
* PD4  - USART2_RTS                (alternate 7).
* PD5  - USART2_TX                 (alternate 7).
* PD6  - USART2_RX                 (alternate 7).
* PD7  - PIN7                      (input pullup).
* PD8  - FMC_D13                   (alternate 12).
* PD9  - FMC_D14                   (alternate 12).
* PD10 - FMC_D15                   (alternate 12).
* PD11 - QSPI_D0                   (alternate 9).
* PD12 - QSPI_D1                   (alternate 9).
* PD13 - QSPI_D3                   (alternate 9).
* PD14 - FMC_D0                    (alternate 12).
* PD15 - FMC_D1                    (alternate 12).
*/
#define VAL_GPIOD_MODER             (PIN_MODE_ALTERNATE(GPIOD_FMC_D2) |     \
                                     PIN_MODE_ALTERNATE(GPIOD_FMC_D3) |     \
                                     PIN_MODE_ALTERNATE(GPIOD_SD_CMD) |     \
                                     PIN_MODE_ALTERNATE(GPIOD_RJ45_USART2_CTS) |     \
                                     PIN_MODE_ALTERNATE(GPIOD_RJ45_USART2_RTS) |           \
                                     PIN_MODE_ALTERNATE(GPIOD_RJ45_USART2_TX) |           \
                                     PIN_MODE_ALTERNATE(GPIOD_RJ45_USART2_RX) |     \
                                     PIN_MODE_INPUT(GPIOD_PIN7) |           \
                                     PIN_MODE_ALTERNATE(GPIOD_FMC_D13) |    \
                                     PIN_MODE_ALTERNATE(GPIOD_FMC_D14) |    \
                                     PIN_MODE_ALTERNATE(GPIOD_FMC_D15) |    \
                                     PIN_MODE_ALTERNATE(GPIOD_FLASH_QSPI_D0) |         \
                                     PIN_MODE_ALTERNATE(GPIOD_FLASH_QSPI_D1) |       \
                                     PIN_MODE_ALTERNATE(GPIOD_FLASH_QSPI_D3) |       \
                                     PIN_MODE_ALTERNATE(GPIOD_FMC_D0) |     \
                                     PIN_MODE_ALTERNATE(GPIOD_FMC_D1))
#define VAL_GPIOD_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOD_FMC_D2) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOD_FMC_D3) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOD_SD_CMD) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOD_RJ45_USART2_CTS) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOD_RJ45_USART2_RTS) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOD_RJ45_USART2_TX) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOD_RJ45_USART2_RX) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOD_PIN7) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOD_FMC_D13) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOD_FMC_D14) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOD_FMC_D15) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOD_FLASH_QSPI_D0) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOD_FLASH_QSPI_D1) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOD_FLASH_QSPI_D3) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOD_FMC_D0) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOD_FMC_D1))
#define VAL_GPIOD_OSPEEDR           (PIN_OSPEED_HIGH(GPIOD_FMC_D2) |        \
                                     PIN_OSPEED_HIGH(GPIOD_FMC_D3) |        \
                                     PIN_OSPEED_HIGH(GPIOD_SD_CMD) |       \
                                     PIN_OSPEED_HIGH(GPIOD_RJ45_USART2_CTS) |        \
                                     PIN_OSPEED_HIGH(GPIOD_RJ45_USART2_RTS) |       \
                                     PIN_OSPEED_HIGH(GPIOD_RJ45_USART2_TX) |       \
                                     PIN_OSPEED_HIGH(GPIOD_RJ45_USART2_RX) |        \
                                     PIN_OSPEED_VERYLOW(GPIOD_PIN7) |       \
                                     PIN_OSPEED_HIGH(GPIOD_FMC_D13) |       \
                                     PIN_OSPEED_HIGH(GPIOD_FMC_D14) |       \
                                     PIN_OSPEED_HIGH(GPIOD_FMC_D15) |       \
                                     PIN_OSPEED_HIGH(GPIOD_FLASH_QSPI_D0) |        \
                                     PIN_OSPEED_HIGH(GPIOD_FLASH_QSPI_D1) |       \
                                     PIN_OSPEED_HIGH(GPIOD_FLASH_QSPI_D3) |       \
                                     PIN_OSPEED_HIGH(GPIOD_FMC_D0) |        \
                                     PIN_OSPEED_HIGH(GPIOD_FMC_D1))
#define VAL_GPIOD_PUPDR             (PIN_PUPDR_FLOATING(GPIOD_FMC_D2) |     \
                                     PIN_PUPDR_FLOATING(GPIOD_FMC_D3) |     \
                                     PIN_PUPDR_FLOATING(GPIOD_SD_CMD) |         \
                                     PIN_PUPDR_FLOATING(GPIOD_RJ45_USART2_CTS) |     \
                                     PIN_PUPDR_FLOATING(GPIOD_RJ45_USART2_RTS) |         \
                                     PIN_PUPDR_FLOATING(GPIOD_RJ45_USART2_TX) |         \
                                     PIN_PUPDR_FLOATING(GPIOD_RJ45_USART2_RX) |     \
                                     PIN_PUPDR_PULLUP(GPIOD_PIN7) |         \
                                     PIN_PUPDR_FLOATING(GPIOD_FMC_D13) |    \
                                     PIN_PUPDR_FLOATING(GPIOD_FMC_D14) |    \
                                     PIN_PUPDR_FLOATING(GPIOD_FMC_D15) |    \
                                     PIN_PUPDR_FLOATING(GPIOD_FLASH_QSPI_D0) |     \
                                     PIN_PUPDR_FLOATING(GPIOD_FLASH_QSPI_D1) |    \
                                     PIN_PUPDR_FLOATING(GPIOD_FLASH_QSPI_D3) |    \
                                     PIN_PUPDR_FLOATING(GPIOD_FMC_D0) |     \
                                     PIN_PUPDR_FLOATING(GPIOD_FMC_D1))
#define VAL_GPIOD_ODR               (PIN_ODR_HIGH(GPIOD_FMC_D2) |           \
                                     PIN_ODR_HIGH(GPIOD_FMC_D3) |           \
                                     PIN_ODR_HIGH(GPIOD_SD_CMD) |             \
                                     PIN_ODR_HIGH(GPIOD_RJ45_USART2_CTS) |           \
                                     PIN_ODR_HIGH(GPIOD_RJ45_USART2_RTS) |             \
                                     PIN_ODR_HIGH(GPIOD_RJ45_USART2_TX) |             \
                                     PIN_ODR_HIGH(GPIOD_RJ45_USART2_RX) |           \
                                     PIN_ODR_HIGH(GPIOD_PIN7) |             \
                                     PIN_ODR_HIGH(GPIOD_FMC_D13) |          \
                                     PIN_ODR_HIGH(GPIOD_FMC_D14) |          \
                                     PIN_ODR_HIGH(GPIOD_FMC_D15) |          \
                                     PIN_ODR_HIGH(GPIOD_FLASH_QSPI_D0) |           \
                                     PIN_ODR_HIGH(GPIOD_FLASH_QSPI_D1) |          \
                                     PIN_ODR_HIGH(GPIOD_FLASH_QSPI_D3) |          \
                                     PIN_ODR_HIGH(GPIOD_FMC_D0) |           \
                                     PIN_ODR_HIGH(GPIOD_FMC_D1))
#define VAL_GPIOD_AFRL              (PIN_AFIO_AF(GPIOD_FMC_D2, 12U) |       \
                                     PIN_AFIO_AF(GPIOD_FMC_D3, 12U) |       \
                                     PIN_AFIO_AF(GPIOD_SD_CMD, 12U) |      \
                                     PIN_AFIO_AF(GPIOD_RJ45_USART2_CTS, 7U) |       \
                                     PIN_AFIO_AF(GPIOD_RJ45_USART2_RTS, 7U) |          \
                                     PIN_AFIO_AF(GPIOD_RJ45_USART2_TX, 7U) |          \
                                     PIN_AFIO_AF(GPIOD_RJ45_USART2_RX, 7U) |       \
                                     PIN_AFIO_AF(GPIOD_PIN7, 0U))
#define VAL_GPIOD_AFRH              (PIN_AFIO_AF(GPIOD_FMC_D13, 12U) |      \
                                     PIN_AFIO_AF(GPIOD_FMC_D14, 12U) |      \
                                     PIN_AFIO_AF(GPIOD_FMC_D15, 12U) |      \
                                     PIN_AFIO_AF(GPIOD_FLASH_QSPI_D0, 9U) |        \
                                     PIN_AFIO_AF(GPIOD_FLASH_QSPI_D1, 9U) |       \
                                     PIN_AFIO_AF(GPIOD_FLASH_QSPI_D3, 9U) |       \
                                     PIN_AFIO_AF(GPIOD_FMC_D0, 12U) |       \
                                     PIN_AFIO_AF(GPIOD_FMC_D1, 12U))

/*
* GPIOE setup:
*
* PE0  - FMC_NBL0                  (alternate 12).
* PE1  - FMC_NBL1                  (alternate 12).
* PE2  - QSPI_D2                   (alternate 9).
* PE3  - PIN3                      (input pullup).
* PE4  - PIN4                      (input pullup).
* PE5  - SD_CARD_DETECT            (input floating).
* PE6  - PIN6                      (input pullup).
* PE7  - FMC_D4                    (alternate 12).
* PE8  - FMC_D5                    (alternate 12).
* PE9  - FMC_D6                    (alternate 12).
* PE10 - FMC_D7                    (alternate 12).
* PE11 - FMC_D8                    (alternate 12).
* PE12 - FMC_D9                    (alternate 12).
* PE13 - FMC_D10                   (alternate 12).
* PE14 - FMC_D11                   (alternate 12).
* PE15 - FMC_D12                   (alternate 12).
*/
#define VAL_GPIOE_MODER             (PIN_MODE_ALTERNATE(GPIOE_FMC_NBL0) |   \
                                     PIN_MODE_ALTERNATE(GPIOE_FMC_NBL1) |   \
                                     PIN_MODE_ALTERNATE(GPIOE_FLASH_QSPI_D2) |        \
                                     PIN_MODE_INPUT(GPIOE_PIN3) |           \
                                     PIN_MODE_INPUT(GPIOE_PIN4) |           \
                                     PIN_MODE_INPUT(GPIOE_SD_CARD_DETECT) |           \
                                     PIN_MODE_INPUT(GPIOE_PIN6) |           \
                                     PIN_MODE_ALTERNATE(GPIOE_FMC_D4) |     \
                                     PIN_MODE_ALTERNATE(GPIOE_FMC_D5) |     \
                                     PIN_MODE_ALTERNATE(GPIOE_FMC_D6) |     \
                                     PIN_MODE_ALTERNATE(GPIOE_FMC_D7) |     \
                                     PIN_MODE_ALTERNATE(GPIOE_FMC_D8) |     \
                                     PIN_MODE_ALTERNATE(GPIOE_FMC_D9) |     \
                                     PIN_MODE_ALTERNATE(GPIOE_FMC_D10) |    \
                                     PIN_MODE_ALTERNATE(GPIOE_FMC_D11) |    \
                                     PIN_MODE_ALTERNATE(GPIOE_FMC_D12))
#define VAL_GPIOE_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOE_FMC_NBL0) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOE_FMC_NBL1) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOE_FLASH_QSPI_D2) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOE_PIN3) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOE_PIN4) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOE_SD_CARD_DETECT) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOE_PIN6) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOE_FMC_D4) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOE_FMC_D5) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOE_FMC_D6) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOE_FMC_D7) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOE_FMC_D8) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOE_FMC_D9) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOE_FMC_D10) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOE_FMC_D11) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOE_FMC_D12))
#define VAL_GPIOE_OSPEEDR           (PIN_OSPEED_HIGH(GPIOE_FMC_NBL0) |      \
                                     PIN_OSPEED_HIGH(GPIOE_FMC_NBL1) |      \
                                     PIN_OSPEED_HIGH(GPIOE_FLASH_QSPI_D2) |       \
                                     PIN_OSPEED_VERYLOW(GPIOE_PIN3) |       \
                                     PIN_OSPEED_VERYLOW(GPIOE_PIN4) |       \
                                     PIN_OSPEED_HIGH(GPIOE_SD_CARD_DETECT) |       \
                                     PIN_OSPEED_VERYLOW(GPIOE_PIN6) |       \
                                     PIN_OSPEED_HIGH(GPIOE_FMC_D4) |        \
                                     PIN_OSPEED_HIGH(GPIOE_FMC_D5) |        \
                                     PIN_OSPEED_HIGH(GPIOE_FMC_D6) |        \
                                     PIN_OSPEED_HIGH(GPIOE_FMC_D7) |        \
                                     PIN_OSPEED_HIGH(GPIOE_FMC_D8) |        \
                                     PIN_OSPEED_HIGH(GPIOE_FMC_D9) |        \
                                     PIN_OSPEED_HIGH(GPIOE_FMC_D10) |       \
                                     PIN_OSPEED_HIGH(GPIOE_FMC_D11) |       \
                                     PIN_OSPEED_HIGH(GPIOE_FMC_D12))
#define VAL_GPIOE_PUPDR             (PIN_PUPDR_FLOATING(GPIOE_FMC_NBL0) |   \
                                     PIN_PUPDR_FLOATING(GPIOE_FMC_NBL1) |   \
                                     PIN_PUPDR_FLOATING(GPIOE_FLASH_QSPI_D2) |         \
                                     PIN_PUPDR_PULLUP(GPIOE_PIN3) |         \
                                     PIN_PUPDR_PULLUP(GPIOE_PIN4) |         \
                                     PIN_PUPDR_FLOATING(GPIOE_SD_CARD_DETECT) |         \
                                     PIN_PUPDR_PULLUP(GPIOE_PIN6) |         \
                                     PIN_PUPDR_FLOATING(GPIOE_FMC_D4) |     \
                                     PIN_PUPDR_FLOATING(GPIOE_FMC_D5) |     \
                                     PIN_PUPDR_FLOATING(GPIOE_FMC_D6) |     \
                                     PIN_PUPDR_FLOATING(GPIOE_FMC_D7) |     \
                                     PIN_PUPDR_FLOATING(GPIOE_FMC_D8) |     \
                                     PIN_PUPDR_FLOATING(GPIOE_FMC_D9) |     \
                                     PIN_PUPDR_FLOATING(GPIOE_FMC_D10) |    \
                                     PIN_PUPDR_FLOATING(GPIOE_FMC_D11) |    \
                                     PIN_PUPDR_FLOATING(GPIOE_FMC_D12))
#define VAL_GPIOE_ODR               (PIN_ODR_HIGH(GPIOE_FMC_NBL0) |         \
                                     PIN_ODR_HIGH(GPIOE_FMC_NBL1) |         \
                                     PIN_ODR_HIGH(GPIOE_FLASH_QSPI_D2) |             \
                                     PIN_ODR_HIGH(GPIOE_PIN3) |             \
                                     PIN_ODR_HIGH(GPIOE_PIN4) |             \
                                     PIN_ODR_HIGH(GPIOE_SD_CARD_DETECT) |             \
                                     PIN_ODR_HIGH(GPIOE_PIN6) |             \
                                     PIN_ODR_HIGH(GPIOE_FMC_D4) |           \
                                     PIN_ODR_HIGH(GPIOE_FMC_D5) |           \
                                     PIN_ODR_HIGH(GPIOE_FMC_D6) |           \
                                     PIN_ODR_HIGH(GPIOE_FMC_D7) |           \
                                     PIN_ODR_HIGH(GPIOE_FMC_D8) |           \
                                     PIN_ODR_HIGH(GPIOE_FMC_D9) |           \
                                     PIN_ODR_HIGH(GPIOE_FMC_D10) |          \
                                     PIN_ODR_HIGH(GPIOE_FMC_D11) |          \
                                     PIN_ODR_HIGH(GPIOE_FMC_D12))
#define VAL_GPIOE_AFRL              (PIN_AFIO_AF(GPIOE_FMC_NBL0, 12U) |     \
                                     PIN_AFIO_AF(GPIOE_FMC_NBL1, 12U) |     \
                                     PIN_AFIO_AF(GPIOE_FLASH_QSPI_D2, 9U) |          \
                                     PIN_AFIO_AF(GPIOE_PIN3, 0U) |          \
                                     PIN_AFIO_AF(GPIOE_PIN4, 0U) |          \
                                     PIN_AFIO_AF(GPIOE_SD_CARD_DETECT, 0U) |          \
                                     PIN_AFIO_AF(GPIOE_PIN6, 0U) |          \
                                     PIN_AFIO_AF(GPIOE_FMC_D4, 12U))
#define VAL_GPIOE_AFRH              (PIN_AFIO_AF(GPIOE_FMC_D5, 12U) |       \
                                     PIN_AFIO_AF(GPIOE_FMC_D6, 12U) |       \
                                     PIN_AFIO_AF(GPIOE_FMC_D7, 12U) |       \
                                     PIN_AFIO_AF(GPIOE_FMC_D8, 12U) |       \
                                     PIN_AFIO_AF(GPIOE_FMC_D9, 12U) |       \
                                     PIN_AFIO_AF(GPIOE_FMC_D10, 12U) |      \
                                     PIN_AFIO_AF(GPIOE_FMC_D11, 12U) |      \
                                     PIN_AFIO_AF(GPIOE_FMC_D12, 12U))

/*
* GPIOF setup:
*
* PF0  - FMC_A0                    (alternate 12).
* PF1  - FMC_A1                    (alternate 12).
* PF2  - FMC_A2                    (alternate 12).
* PF3  - FMC_A3                    (alternate 12).
* PF4  - FMC_A4                    (alternate 12).
* PF5  - FMC_A5                    (alternate 12).
* PF6  - UART7_RX                  (alternate 8).
* PF7  - UART7_TX                  (alternate 8).
* PF8  - IO0_ADC3_IN6              (input floating).
* PF9  - IO0_ADC3_IN7              (input floating).
* PF10 - QSPI_CLK                  (alternate 9).
* PF11 - FMC_SDNRAS                (alternate 12).
* PF12 - FMC_A6                    (alternate 12).
* PF13 - FMC_A7                    (alternate 12).
* PF14 - FMC_A8                    (alternate 12).
* PF15 - FMC_A9                    (alternate 12).
*/
#define VAL_GPIOF_MODER             (PIN_MODE_ALTERNATE(GPIOF_FMC_A0) |     \
                                     PIN_MODE_ALTERNATE(GPIOF_FMC_A1) |     \
                                     PIN_MODE_ALTERNATE(GPIOF_FMC_A2) |     \
                                     PIN_MODE_ALTERNATE(GPIOF_FMC_A3) |     \
                                     PIN_MODE_ALTERNATE(GPIOF_FMC_A4) |     \
                                     PIN_MODE_ALTERNATE(GPIOF_FMC_A5) |     \
                                     PIN_MODE_ALTERNATE(GPIOF_RS485_UART7_RX) |    \
                                     PIN_MODE_ALTERNATE(GPIOF_RS485_UART7_TX) |    \
                                     PIN_MODE_INPUT(GPIOF_IO0_ADC3_IN6) |  \
                                     PIN_MODE_INPUT(GPIOF_IO0_ADC3_IN7) |  \
                                     PIN_MODE_ALTERNATE(GPIOF_FLASH_QSPI_CLK) |     \
                                     PIN_MODE_ALTERNATE(GPIOF_FMC_SDNRAS) | \
                                     PIN_MODE_ALTERNATE(GPIOF_FMC_A6) |     \
                                     PIN_MODE_ALTERNATE(GPIOF_FMC_A7) |     \
                                     PIN_MODE_ALTERNATE(GPIOF_FMC_A8) |     \
                                     PIN_MODE_ALTERNATE(GPIOF_FMC_A9))
#define VAL_GPIOF_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOF_FMC_A0) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOF_FMC_A1) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOF_FMC_A2) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOF_FMC_A3) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOF_FMC_A4) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOF_FMC_A5) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOF_RS485_UART7_RX) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOF_RS485_UART7_TX) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOF_IO0_ADC3_IN6) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOF_IO0_ADC3_IN7) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOF_FLASH_QSPI_CLK) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOF_FMC_SDNRAS) | \
                                     PIN_OTYPE_PUSHPULL(GPIOF_FMC_A6) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOF_FMC_A7) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOF_FMC_A8) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOF_FMC_A9))
#define VAL_GPIOF_OSPEEDR           (PIN_OSPEED_HIGH(GPIOF_FMC_A0) |        \
                                     PIN_OSPEED_HIGH(GPIOF_FMC_A1) |        \
                                     PIN_OSPEED_HIGH(GPIOF_FMC_A2) |        \
                                     PIN_OSPEED_HIGH(GPIOF_FMC_A3) |        \
                                     PIN_OSPEED_HIGH(GPIOF_FMC_A4) |        \
                                     PIN_OSPEED_HIGH(GPIOF_FMC_A5) |        \
                                     PIN_OSPEED_HIGH(GPIOF_RS485_UART7_RX) |       \
                                     PIN_OSPEED_HIGH(GPIOF_RS485_UART7_TX) |       \
                                     PIN_OSPEED_HIGH(GPIOF_IO0_ADC3_IN6) |     \
                                     PIN_OSPEED_HIGH(GPIOF_IO0_ADC3_IN7) |     \
                                     PIN_OSPEED_HIGH(GPIOF_FLASH_QSPI_CLK) |        \
                                     PIN_OSPEED_HIGH(GPIOF_FMC_SDNRAS) |    \
                                     PIN_OSPEED_HIGH(GPIOF_FMC_A6) |        \
                                     PIN_OSPEED_HIGH(GPIOF_FMC_A7) |        \
                                     PIN_OSPEED_HIGH(GPIOF_FMC_A8) |        \
                                     PIN_OSPEED_HIGH(GPIOF_FMC_A9))
#define VAL_GPIOF_PUPDR             (PIN_PUPDR_FLOATING(GPIOF_FMC_A0) |     \
                                     PIN_PUPDR_FLOATING(GPIOF_FMC_A1) |     \
                                     PIN_PUPDR_FLOATING(GPIOF_FMC_A2) |     \
                                     PIN_PUPDR_FLOATING(GPIOF_FMC_A3) |     \
                                     PIN_PUPDR_FLOATING(GPIOF_FMC_A4) |     \
                                     PIN_PUPDR_FLOATING(GPIOF_FMC_A5) |     \
                                     PIN_PUPDR_FLOATING(GPIOF_RS485_UART7_RX) |    \
                                     PIN_PUPDR_FLOATING(GPIOF_RS485_UART7_TX) |    \
                                     PIN_PUPDR_FLOATING(GPIOF_IO0_ADC3_IN6) |  \
                                     PIN_PUPDR_FLOATING(GPIOF_IO0_ADC3_IN7) |  \
                                     PIN_PUPDR_FLOATING(GPIOF_FLASH_QSPI_CLK) |     \
                                     PIN_PUPDR_FLOATING(GPIOF_FMC_SDNRAS) | \
                                     PIN_PUPDR_FLOATING(GPIOF_FMC_A6) |     \
                                     PIN_PUPDR_FLOATING(GPIOF_FMC_A7) |     \
                                     PIN_PUPDR_FLOATING(GPIOF_FMC_A8) |     \
                                     PIN_PUPDR_FLOATING(GPIOF_FMC_A9))
#define VAL_GPIOF_ODR               (PIN_ODR_HIGH(GPIOF_FMC_A0) |           \
                                     PIN_ODR_HIGH(GPIOF_FMC_A1) |           \
                                     PIN_ODR_HIGH(GPIOF_FMC_A2) |           \
                                     PIN_ODR_HIGH(GPIOF_FMC_A3) |           \
                                     PIN_ODR_HIGH(GPIOF_FMC_A4) |           \
                                     PIN_ODR_HIGH(GPIOF_FMC_A5) |           \
                                     PIN_ODR_HIGH(GPIOF_RS485_UART7_RX) |          \
                                     PIN_ODR_HIGH(GPIOF_RS485_UART7_TX) |          \
                                     PIN_ODR_HIGH(GPIOF_IO0_ADC3_IN6) |        \
                                     PIN_ODR_HIGH(GPIOF_IO0_ADC3_IN7) |        \
                                     PIN_ODR_HIGH(GPIOF_FLASH_QSPI_CLK) |           \
                                     PIN_ODR_HIGH(GPIOF_FMC_SDNRAS) |       \
                                     PIN_ODR_HIGH(GPIOF_FMC_A6) |           \
                                     PIN_ODR_HIGH(GPIOF_FMC_A7) |           \
                                     PIN_ODR_HIGH(GPIOF_FMC_A8) |           \
                                     PIN_ODR_HIGH(GPIOF_FMC_A9))
#define VAL_GPIOF_AFRL              (PIN_AFIO_AF(GPIOF_FMC_A0, 12U) |       \
                                     PIN_AFIO_AF(GPIOF_FMC_A1, 12U) |       \
                                     PIN_AFIO_AF(GPIOF_FMC_A2, 12U) |       \
                                     PIN_AFIO_AF(GPIOF_FMC_A3, 12U) |       \
                                     PIN_AFIO_AF(GPIOF_FMC_A4, 12U) |       \
                                     PIN_AFIO_AF(GPIOF_FMC_A5, 12U) |       \
                                     PIN_AFIO_AF(GPIOF_RS485_UART7_RX, 8U) |        \
                                     PIN_AFIO_AF(GPIOF_RS485_UART7_TX, 8U))
#define VAL_GPIOF_AFRH              (PIN_AFIO_AF(GPIOF_IO0_ADC3_IN6, 0U) |     \
                                     PIN_AFIO_AF(GPIOF_IO0_ADC3_IN7, 0U) |     \
                                     PIN_AFIO_AF(GPIOF_FLASH_QSPI_CLK, 9U) |       \
                                     PIN_AFIO_AF(GPIOF_FMC_SDNRAS, 12U) |   \
                                     PIN_AFIO_AF(GPIOF_FMC_A6, 12U) |       \
                                     PIN_AFIO_AF(GPIOF_FMC_A7, 12U) |       \
                                     PIN_AFIO_AF(GPIOF_FMC_A8, 12U) |       \
                                     PIN_AFIO_AF(GPIOF_FMC_A9, 12U))

/*
* GPIOG setup:
*
* PG0  - FMC_A10                   (alternate 12).
* PG1  - FMC_A11                   (alternate 12).
* PG2  - IO1_INT                   (input pullup).
* PG3  - IO1_WAKEUP                (input pullup).
* PG4  - FMC_BA0                   (alternate 12).
* PG5  - FMC_BA1                   (alternate 12).
* PG6  - LED1                      (output pushpull maximum).
* PG7  - LED2					   (output pushpull maximum).
* PG8  - FMC_SDCLK                 (alternate 12).
* PG9  - IO0_P13_IO                (input pullup).
* PG10 - IO0_P19_IO                (input pullup).
* PG11 - RMII_TX_EN                (alternate 11).
* PG12 - RMII_RST                  (output pushpull maximum)
* PG13 - RMII_TXD0                 (alternate 11).
* PG14 - RMII_TXD1                 (alternate 11).
* PG15 - FMC_SDNCAS                (alternate 12).
*/
#define VAL_GPIOG_MODER             (PIN_MODE_ALTERNATE(GPIOG_FMC_A10) |    \
                                     PIN_MODE_ALTERNATE(GPIOG_FMC_A11) |    \
                                     PIN_MODE_INPUT(GPIOG_IO1_P23_INT) |           \
                                     PIN_MODE_INPUT(GPIOG_IO1_P24_WAKEUP) |           \
                                     PIN_MODE_ALTERNATE(GPIOG_FMC_BA0) |    \
                                     PIN_MODE_ALTERNATE(GPIOG_FMC_BA1) |    \
                                     PIN_MODE_OUTPUT(GPIOG_LED1) |     \
                                     PIN_MODE_OUTPUT(GPIOG_LED2) |    \
                                     PIN_MODE_ALTERNATE(GPIOG_FMC_SDCLK) |  \
                                     PIN_MODE_INPUT(GPIOG_IO0_P13_IO) |           \
                                     PIN_MODE_INPUT(GPIOG_IO0_P19_IO) |     \
                                     PIN_MODE_ALTERNATE(GPIOG_RMII_TX_EN) |     \
                                     PIN_MODE_OUTPUT(GPIOG_RMII_RST) |     \
                                     PIN_MODE_ALTERNATE(GPIOG_RMII_TXD0) |  \
                                     PIN_MODE_ALTERNATE(GPIOG_RMII_TXD1) |  \
                                     PIN_MODE_ALTERNATE(GPIOG_FMC_SDNCAS))
#define VAL_GPIOG_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOG_FMC_A10) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOG_FMC_A11) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOG_IO1_P23_INT) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOG_IO1_P24_WAKEUP) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOG_FMC_BA0) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOG_FMC_BA1) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOG_LED1) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOG_LED2) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOG_FMC_SDCLK) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOG_IO0_P13_IO) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOG_IO0_P19_IO) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOG_RMII_TX_EN) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOG_RMII_RST) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOG_RMII_TXD0) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOG_RMII_TXD1) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOG_FMC_SDNCAS))
#define VAL_GPIOG_OSPEEDR           (PIN_OSPEED_HIGH(GPIOG_FMC_A10) |       \
                                     PIN_OSPEED_HIGH(GPIOG_FMC_A11) |       \
                                     PIN_OSPEED_HIGH(GPIOG_IO1_P23_INT) |       \
                                     PIN_OSPEED_HIGH(GPIOG_IO1_P24_WAKEUP) |       \
                                     PIN_OSPEED_HIGH(GPIOG_FMC_BA0) |       \
                                     PIN_OSPEED_HIGH(GPIOG_FMC_BA1) |       \
                                     PIN_OSPEED_HIGH(GPIOG_LED1) |        \
                                     PIN_OSPEED_HIGH(GPIOG_LED2) |        \
                                     PIN_OSPEED_HIGH(GPIOG_FMC_SDCLK) |     \
                                     PIN_OSPEED_HIGH(GPIOG_IO0_P13_IO) |       \
                                     PIN_OSPEED_HIGH(GPIOG_IO0_P19_IO) |        \
                                     PIN_OSPEED_HIGH(GPIOG_RMII_TX_EN) |        \
                                     PIN_OSPEED_HIGH(GPIOG_RMII_RST) |        \
                                     PIN_OSPEED_HIGH(GPIOG_RMII_TXD0) |     \
                                     PIN_OSPEED_HIGH(GPIOG_RMII_TXD1) |     \
                                     PIN_OSPEED_HIGH(GPIOG_FMC_SDNCAS))
#define VAL_GPIOG_PUPDR             (PIN_PUPDR_FLOATING(GPIOG_FMC_A10) |    \
                                     PIN_PUPDR_FLOATING(GPIOG_FMC_A11) |    \
                                     PIN_PUPDR_PULLUP(GPIOG_IO1_P23_INT) |         \
                                     PIN_PUPDR_PULLUP(GPIOG_IO1_P24_WAKEUP) |         \
                                     PIN_PUPDR_FLOATING(GPIOG_FMC_BA0) |    \
                                     PIN_PUPDR_FLOATING(GPIOG_FMC_BA1) |    \
                                     PIN_PUPDR_FLOATING(GPIOG_LED1) |     \
                                     PIN_PUPDR_FLOATING(GPIOG_LED2) |    \
                                     PIN_PUPDR_FLOATING(GPIOG_FMC_SDCLK) |  \
                                     PIN_PUPDR_PULLUP(GPIOG_IO0_P13_IO) |         \
                                     PIN_PUPDR_PULLUP(GPIOG_IO0_P19_IO) |     \
                                     PIN_PUPDR_FLOATING(GPIOG_RMII_TX_EN) |     \
                                     PIN_PUPDR_PULLDOWN(GPIOG_RMII_RST) |     \
                                     PIN_PUPDR_FLOATING(GPIOG_RMII_TXD0) |  \
                                     PIN_PUPDR_FLOATING(GPIOG_RMII_TXD1) |  \
                                     PIN_PUPDR_FLOATING(GPIOG_FMC_SDNCAS))
#define VAL_GPIOG_ODR               (PIN_ODR_HIGH(GPIOG_FMC_A10) |          \
                                     PIN_ODR_HIGH(GPIOG_FMC_A11) |          \
                                     PIN_ODR_HIGH(GPIOG_IO1_P23_INT) |             \
                                     PIN_ODR_HIGH(GPIOG_IO1_P24_WAKEUP) |             \
                                     PIN_ODR_HIGH(GPIOG_FMC_BA0) |          \
                                     PIN_ODR_HIGH(GPIOG_FMC_BA1) |          \
                                     PIN_ODR_LOW(GPIOG_LED1) |           \
                                     PIN_ODR_LOW(GPIOG_LED2) |          \
                                     PIN_ODR_HIGH(GPIOG_FMC_SDCLK) |        \
                                     PIN_ODR_HIGH(GPIOG_IO0_P13_IO) |             \
                                     PIN_ODR_HIGH(GPIOG_IO0_P19_IO) |           \
                                     PIN_ODR_HIGH(GPIOG_RMII_TX_EN) |           \
                                     PIN_ODR_HIGH(GPIOG_RMII_RST) |           \
                                     PIN_ODR_HIGH(GPIOG_RMII_TXD0) |        \
                                     PIN_ODR_HIGH(GPIOG_RMII_TXD1) |        \
                                     PIN_ODR_HIGH(GPIOG_FMC_SDNCAS))
#define VAL_GPIOG_AFRL              (PIN_AFIO_AF(GPIOG_FMC_A10, 12U) |      \
                                     PIN_AFIO_AF(GPIOG_FMC_A11, 12U) |      \
                                     PIN_AFIO_AF(GPIOG_IO1_P23_INT, 0U) |          \
                                     PIN_AFIO_AF(GPIOG_IO1_P24_WAKEUP, 0U) |     \
                                     PIN_AFIO_AF(GPIOG_FMC_BA0, 12U) |      \
                                     PIN_AFIO_AF(GPIOG_FMC_BA1, 12U) |      \
                                     PIN_AFIO_AF(GPIOG_LED1, 0U) |       \
                                     PIN_AFIO_AF(GPIOG_LED2, 0U))
#define VAL_GPIOG_AFRH              (PIN_AFIO_AF(GPIOG_FMC_SDCLK, 12U) |    \
                                     PIN_AFIO_AF(GPIOG_IO0_P13_IO, 0U) |          \
                                     PIN_AFIO_AF(GPIOG_IO0_P19_IO, 0U) |       \
                                     PIN_AFIO_AF(GPIOG_RMII_TX_EN, 11U) |       \
                                     PIN_AFIO_AF(GPIOG_RMII_RST, 0U) |       \
                                     PIN_AFIO_AF(GPIOG_RMII_TXD0, 11U) |    \
                                     PIN_AFIO_AF(GPIOG_RMII_TXD1, 11U) |    \
                                     PIN_AFIO_AF(GPIOG_FMC_SDNCAS, 12U))

/*
* GPIOH setup:
*
* PH0  - OSC_IN                    (input floating).
* PH1  - OSC_OUT                   (input floating).
* PH2  - PIN2                      (input pullup).
* PH3  - PIN3                      (input pullup).
* PH4  - I2C2_SCL                  (input pullup).
* PH5  - I2C2_SDA                  (input pullup).
* PH6  - FMC_SDNE1                 (alternate 12).
* PH7  - FMC_SDCKE1                (alternate 12).
* PH8  - I2C3_SDA                  (input pullup).
* PH9  - IO1_P12_PWM               (output pullup).
* PH10 - IO1_P13_IO                (input pullup).
* PH11 - IO1_P19_IO                (input pullup).
* PH12 - SPEAKER                   (output high floating).
* PH13 - IO0_P7_IO                 (input pullup).
* PH14 - IO0_P8_IO                 (input pullup).
* PH15 - IO0_P12_PWM               (output pullup).
*/
#define VAL_GPIOH_MODER             (PIN_MODE_INPUT(GPIOH_OSC_IN) |         \
                                     PIN_MODE_INPUT(GPIOH_OSC_OUT) |        \
                                     PIN_MODE_INPUT(GPIOH_PIN2) |           \
                                     PIN_MODE_INPUT(GPIOH_PIN3) |           \
                                     PIN_MODE_INPUT(GPIOH_I2C2_SCL) |           \
                                     PIN_MODE_INPUT(GPIOH_I2C2_SDA) |           \
                                     PIN_MODE_ALTERNATE(GPIOH_FMC_SDNE1) |  \
                                     PIN_MODE_ALTERNATE(GPIOH_FMC_SDCKE1) | \
                                     PIN_MODE_INPUT(GPIOH_I2C3_SDA) |           \
                                     PIN_MODE_OUTPUT(GPIOH_IO1_P12_PWM) |           \
                                     PIN_MODE_INPUT(GPIOH_IO1_P13_IO) |          \
                                     PIN_MODE_INPUT(GPIOH_IO1_P19_IO) |          \
                                     PIN_MODE_OUTPUT(GPIOH_PWM_SPEAKER) |          \
                                     PIN_MODE_INPUT(GPIOH_IO0_P7_IO) |          \
                                     PIN_MODE_INPUT(GPIOH_IO0_P8_IO) |          \
                                     PIN_MODE_OUTPUT(GPIOH_IO0_P12_IO))
#define VAL_GPIOH_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOH_OSC_IN) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOH_OSC_OUT) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN2) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN3) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_I2C2_SCL) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_I2C2_SDA) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_FMC_SDNE1) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOH_FMC_SDCKE1) | \
                                     PIN_OTYPE_PUSHPULL(GPIOH_I2C3_SDA) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_IO1_P12_PWM) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_IO1_P13_IO) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOH_IO1_P19_IO) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PWM_SPEAKER) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOH_IO0_P7_IO) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOH_IO0_P8_IO) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOH_IO0_P12_IO))
#define VAL_GPIOH_OSPEEDR           (PIN_OSPEED_HIGH(GPIOH_OSC_IN) |        \
                                     PIN_OSPEED_HIGH(GPIOH_OSC_OUT) |       \
                                     PIN_OSPEED_VERYLOW(GPIOH_PIN2) |       \
                                     PIN_OSPEED_VERYLOW(GPIOH_PIN3) |       \
                                     PIN_OSPEED_HIGH(GPIOH_I2C2_SCL) |       \
                                     PIN_OSPEED_HIGH(GPIOH_I2C2_SDA) |       \
                                     PIN_OSPEED_HIGH(GPIOH_FMC_SDNE1) |  \
                                     PIN_OSPEED_HIGH(GPIOH_FMC_SDCKE1) | \
                                     PIN_OSPEED_HIGH(GPIOH_I2C3_SDA) |       \
                                     PIN_OSPEED_HIGH(GPIOH_IO1_P12_PWM) |       \
                                     PIN_OSPEED_HIGH(GPIOH_IO1_P13_IO) |      \
                                     PIN_OSPEED_HIGH(GPIOH_IO1_P19_IO) |      \
                                     PIN_OSPEED_HIGH(GPIOH_PWM_SPEAKER) |      \
                                     PIN_OSPEED_HIGH(GPIOH_IO0_P7_IO) |      \
                                     PIN_OSPEED_HIGH(GPIOH_IO0_P8_IO) |      \
                                     PIN_OSPEED_HIGH(GPIOH_IO0_P12_IO))
#define VAL_GPIOH_PUPDR             (PIN_PUPDR_FLOATING(GPIOH_OSC_IN) |     \
                                     PIN_PUPDR_FLOATING(GPIOH_OSC_OUT) |    \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN2) |         \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN3) |         \
                                     PIN_PUPDR_PULLUP(GPIOH_I2C2_SCL) |         \
                                     PIN_PUPDR_PULLUP(GPIOH_I2C2_SDA) |         \
                                     PIN_PUPDR_FLOATING(GPIOH_FMC_SDNE1) |  \
                                     PIN_PUPDR_FLOATING(GPIOH_FMC_SDCKE1) | \
                                     PIN_PUPDR_PULLUP(GPIOH_I2C3_SDA) |         \
                                     PIN_PUPDR_FLOATING(GPIOH_IO1_P12_PWM) |         \
                                     PIN_PUPDR_FLOATING(GPIOH_IO1_P13_IO) |        \
                                     PIN_PUPDR_FLOATING(GPIOH_IO1_P19_IO) |        \
                                     PIN_PUPDR_PULLUP(GPIOH_PWM_SPEAKER) |        \
                                     PIN_PUPDR_FLOATING(GPIOH_IO0_P7_IO) |        \
                                     PIN_PUPDR_FLOATING(GPIOH_IO0_P8_IO) |        \
                                     PIN_PUPDR_FLOATING(GPIOH_IO0_P12_IO))
#define VAL_GPIOH_ODR               (PIN_ODR_HIGH(GPIOH_OSC_IN) |           \
                                     PIN_ODR_HIGH(GPIOH_OSC_OUT) |          \
                                     PIN_ODR_HIGH(GPIOH_PIN2) |             \
                                     PIN_ODR_HIGH(GPIOH_PIN3) |             \
                                     PIN_ODR_HIGH(GPIOH_I2C2_SCL) |             \
                                     PIN_ODR_HIGH(GPIOH_I2C2_SDA) |             \
                                     PIN_ODR_HIGH(GPIOH_FMC_SDNE1) |        \
                                     PIN_ODR_HIGH(GPIOH_FMC_SDCKE1) |       \
                                     PIN_ODR_HIGH(GPIOH_I2C3_SDA) |             \
                                     PIN_ODR_HIGH(GPIOH_IO1_P12_PWM) |             \
                                     PIN_ODR_HIGH(GPIOH_IO1_P13_IO) |            \
                                     PIN_ODR_HIGH(GPIOH_IO1_P19_IO) |            \
                                     PIN_ODR_HIGH(GPIOH_PWM_SPEAKER) |            \
                                     PIN_ODR_HIGH(GPIOH_IO0_P7_IO) |            \
                                     PIN_ODR_HIGH(GPIOH_IO0_P8_IO) |            \
                                     PIN_ODR_HIGH(GPIOH_IO0_P12_IO))
#define VAL_GPIOH_AFRL              (PIN_AFIO_AF(GPIOH_OSC_IN, 0U) |        \
                                     PIN_AFIO_AF(GPIOH_OSC_OUT, 0U) |       \
                                     PIN_AFIO_AF(GPIOH_PIN2, 0U) |          \
                                     PIN_AFIO_AF(GPIOH_PIN3, 0U) |          \
                                     PIN_AFIO_AF(GPIOH_I2C2_SCL, 0U) |          \
                                     PIN_AFIO_AF(GPIOH_I2C2_SDA, 0U) |          \
                                     PIN_AFIO_AF(GPIOH_FMC_SDNE1, 12U) |    \
                                     PIN_AFIO_AF(GPIOH_FMC_SDCKE1, 12U))   
#define VAL_GPIOH_AFRH              (PIN_AFIO_AF(GPIOH_I2C3_SDA, 0U) |          \
                                     PIN_AFIO_AF(GPIOH_IO1_P12_PWM, 0U) |          \
                                     PIN_AFIO_AF(GPIOH_IO1_P13_IO, 0U) |         \
                                     PIN_AFIO_AF(GPIOH_IO1_P19_IO, 0U) |         \
                                     PIN_AFIO_AF(GPIOH_PWM_SPEAKER, 0U) |         \
                                     PIN_AFIO_AF(GPIOH_IO0_P7_IO, 0U) |         \
                                     PIN_AFIO_AF(GPIOH_IO0_P8_IO, 0U) |         \
                                     PIN_AFIO_AF(GPIOH_IO0_P12_IO, 0U))

/*
* GPIOI setup:
*
* PI0  - IO0_P20_ON_OFF            (output floating).
* PI1  - SPI2_SCK                  (alternate 5).
* PI2  - SPI2_MISO                 (alternate 5).
* PI3  - SPI2_MOSI                 (alternate 5).
* PI4  - KEYPAD_2                  (input pullup).
* PI5  - KEYPAD_1                  (input pullup).
* PI6  - KEYPAD_4                  (input pullup).
* PI7  - KEYPAD_3                  (input pullup).
* PI8  - PIN8                      (input pullup).
* PI9  - PIN9                      (input pullup).
* PI10 - RMII_RXER                 (alternate 11).
* PI11 - PIN11                     (input pullup).
* PI12 - RS485_RE                  (output floating).
* PI13 - RS485_SHDN                (output floating).
* PI14 - RS485_BUS_TERM            (output floating).
* PI15 - PIN15                     (output pullup).
*/
#define VAL_GPIOI_MODER             (PIN_MODE_OUTPUT(GPIOI_IO0_P20_ON_OFF) |           \
                                     PIN_MODE_ALTERNATE(GPIOH_IO0_P24_IO) |           \
                                     PIN_MODE_ALTERNATE(GPIOH_IO0_P5_IO) |           \
                                     PIN_MODE_ALTERNATE(GPIOH_IO0_P23_INT) |           \
                                     PIN_MODE_INPUT(GPIOI_KEYPAD_2) |           \
                                     PIN_MODE_INPUT(GPIOI_KEYPAD_1) |           \
                                     PIN_MODE_INPUT(GPIOI_KEYPAD_4) |           \
                                     PIN_MODE_INPUT(GPIOI_KEYPAD_3) |           \
                                     PIN_MODE_INPUT(GPIOI_PIN8) |           \
                                     PIN_MODE_INPUT(GPIOI_PIN9) |           \
                                     PIN_MODE_INPUT(GPIOI_RMII_RXER) |          \
                                     PIN_MODE_INPUT(GPIOI_PIN11) |          \
                                     PIN_MODE_OUTPUT(GPIOI_RS485_RE) |          \
                                     PIN_MODE_OUTPUT(GPIOI_RS485_SHDN) |          \
                                     PIN_MODE_OUTPUT(GPIOI_RS485_BUS_TERM) |          \
                                     PIN_MODE_OUTPUT(GPIOI_FLASH_SPI1_CS))
#define VAL_GPIOI_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOI_IO0_P20_ON_OFF) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_IO0_P24_IO) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_IO0_P5_IO) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_IO0_P23_INT) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_KEYPAD_2) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_KEYPAD_1) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_KEYPAD_4) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_KEYPAD_3) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN8) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN9) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_RMII_RXER) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN11) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOI_RS485_RE) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOI_RS485_SHDN) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOI_RS485_BUS_TERM) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOI_FLASH_SPI1_CS))
#define VAL_GPIOI_OSPEEDR           (PIN_OSPEED_LOW(GPIOI_IO0_P20_ON_OFF) |       \
                                     PIN_OSPEED_HIGH(GPIOH_IO0_P24_IO) |       \
                                     PIN_OSPEED_HIGH(GPIOH_IO0_P5_IO) |       \
                                     PIN_OSPEED_HIGH(GPIOH_IO0_P23_INT) |       \
                                     PIN_OSPEED_MEDIUM(GPIOI_KEYPAD_2) |       \
                                     PIN_OSPEED_MEDIUM(GPIOI_KEYPAD_1) |       \
                                     PIN_OSPEED_MEDIUM(GPIOI_KEYPAD_4) |       \
                                     PIN_OSPEED_MEDIUM(GPIOI_KEYPAD_3) |       \
                                     PIN_OSPEED_HIGH(GPIOI_PIN8) |       \
                                     PIN_OSPEED_HIGH(GPIOI_PIN9) |       \
                                     PIN_OSPEED_HIGH(GPIOI_RMII_RXER) |      \
                                     PIN_OSPEED_VERYLOW(GPIOI_PIN11) |      \
                                     PIN_OSPEED_LOW(GPIOI_RS485_RE) |      \
                                     PIN_OSPEED_LOW(GPIOI_RS485_SHDN) |      \
                                     PIN_OSPEED_LOW(GPIOI_RS485_BUS_TERM) |      \
                                     PIN_OSPEED_HIGH(GPIOI_FLASH_SPI1_CS))
#define VAL_GPIOI_PUPDR             (PIN_PUPDR_FLOATING(GPIOI_IO0_P20_ON_OFF) |         \
                                     PIN_PUPDR_FLOATING(GPIOH_IO0_P24_IO) |         \
                                     PIN_PUPDR_FLOATING(GPIOH_IO0_P5_IO) |         \
                                     PIN_PUPDR_FLOATING(GPIOH_IO0_P23_INT) |         \
                                     PIN_PUPDR_PULLUP(GPIOI_KEYPAD_2) |         \
                                     PIN_PUPDR_PULLUP(GPIOI_KEYPAD_1) |         \
                                     PIN_PUPDR_PULLUP(GPIOI_KEYPAD_4) |         \
                                     PIN_PUPDR_PULLUP(GPIOI_KEYPAD_3) |         \
                                     PIN_PUPDR_PULLDOWN(GPIOI_PIN8) |         \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN9) |         \
                                     PIN_PUPDR_FLOATING(GPIOI_RMII_RXER) |        \
                                     PIN_PUPDR_FLOATING(GPIOI_PIN11) |        \
                                     PIN_PUPDR_FLOATING(GPIOI_RS485_RE) |        \
                                     PIN_PUPDR_FLOATING(GPIOI_RS485_SHDN) |        \
                                     PIN_PUPDR_FLOATING(GPIOI_RS485_BUS_TERM) |        \
                                     PIN_PUPDR_FLOATING(GPIOI_FLASH_SPI1_CS))
#define VAL_GPIOI_ODR               (PIN_ODR_HIGH(GPIOI_IO0_P20_ON_OFF) |             \
                                     PIN_ODR_HIGH(GPIOH_IO0_P24_IO) |             \
                                     PIN_ODR_HIGH(GPIOH_IO0_P5_IO) |             \
                                     PIN_ODR_HIGH(GPIOH_IO0_P23_INT) |             \
                                     PIN_ODR_HIGH(GPIOI_KEYPAD_2) |             \
                                     PIN_ODR_HIGH(GPIOI_KEYPAD_1) |             \
                                     PIN_ODR_HIGH(GPIOI_KEYPAD_4) |             \
                                     PIN_ODR_HIGH(GPIOI_KEYPAD_3) |             \
                                     PIN_ODR_HIGH(GPIOI_PIN8) |             \
                                     PIN_ODR_HIGH(GPIOI_PIN9) |             \
                                     PIN_ODR_HIGH(GPIOI_RMII_RXER) |            \
                                     PIN_ODR_HIGH(GPIOI_PIN11) |            \
                                     PIN_ODR_HIGH(GPIOI_RS485_RE) |            \
                                     PIN_ODR_HIGH(GPIOI_RS485_SHDN) |            \
                                     PIN_ODR_HIGH(GPIOI_RS485_BUS_TERM) |            \
                                     PIN_ODR_HIGH(GPIOI_FLASH_SPI1_CS))
#define VAL_GPIOI_AFRL              (PIN_AFIO_AF(GPIOI_IO0_P20_ON_OFF, 0U) |          \
                                     PIN_AFIO_AF(GPIOH_IO0_P24_IO, 5U) |          \
                                     PIN_AFIO_AF(GPIOH_IO0_P5_IO, 5U) |          \
                                     PIN_AFIO_AF(GPIOH_IO0_P23_INT, 5U) |          \
                                     PIN_AFIO_AF(GPIOI_KEYPAD_2, 0U) |          \
                                     PIN_AFIO_AF(GPIOI_KEYPAD_1, 0U) |          \
                                     PIN_AFIO_AF(GPIOI_KEYPAD_4, 0U) |          \
                                     PIN_AFIO_AF(GPIOI_KEYPAD_3, 0U))
#define VAL_GPIOI_AFRH              (PIN_AFIO_AF(GPIOI_PIN8, 0U) |          \
                                     PIN_AFIO_AF(GPIOI_PIN9, 0U) |          \
                                     PIN_AFIO_AF(GPIOI_RMII_RXER, 11U) |         \
                                     PIN_AFIO_AF(GPIOI_PIN11, 0U) |         \
                                     PIN_AFIO_AF(GPIOI_RS485_RE, 0U) |         \
                                     PIN_AFIO_AF(GPIOI_RS485_SHDN, 0U) |         \
                                     PIN_AFIO_AF(GPIOI_RS485_BUS_TERM, 0U) |         \
                                     PIN_AFIO_AF(GPIOI_FLASH_SPI1_CS, 0U))



/*
* GPIOJ setup:
*
* PJ0  - IO0_PWR_ON_OFF            (output pulldown).
* PJ1  - IO1_PWR_ON_OFF            (output pulldown).
* PJ2  - PIN2                      (input pullup).
* PJ3  - SPI_FLASH_WP              (output pullup).
* PJ4  - SPI_FLASH_HOLD            (output pullup).
* PJ5  - RELAY                     (output pulldown).
* PJ6  - PIN6                      (input pullup).
* PJ7  - PIN7                      (input pullup).
* PJ8  - PIN8                      (input pullup).
* PJ9  - PIN9                      (input pullup).
* PJ10 - PIN10                     (input pulup).
* PJ11 - PIN11                     (input pullup).
* PJ12 - PULSE_COUNT               (input ).
* PJ13 - PIN13                     (input pullup).
* PJ14 - PIN14                     (input pullup).
* PJ15 - PIN15                     (output).
*/
#define VAL_GPIOJ_MODER             (PIN_MODE_OUTPUT(GPIOJ_IO0_PWR_ON_OFF) |           \
                                     PIN_MODE_OUTPUT(GPIOJ_IO1_PWR_ON_OFF) |           \
                                     PIN_MODE_INPUT(GPIOJ_PIN2) |     \
                                     PIN_MODE_OUTPUT(GPIOJ_SPI_FLASH_WP) |           \
                                     PIN_MODE_OUTPUT(GPIOJ_SPI_FLASH_HOLD) |           \
                                     PIN_MODE_OUTPUT(GPIOJ_RELAY) |        \
                                     PIN_MODE_INPUT(GPIOJ_PIN6) |           \
                                     PIN_MODE_INPUT(GPIOJ_PIN7) |           \
                                     PIN_MODE_INPUT(GPIOJ_PIN8) |           \
                                     PIN_MODE_INPUT(GPIOJ_PIN9) |           \
                                     PIN_MODE_INPUT(GPIOJ_PIN10) |          \
                                     PIN_MODE_INPUT(GPIOJ_PIN11) |          \
                                     PIN_MODE_INPUT(GPIOJ_PULSE_COUNT) |          \
                                     PIN_MODE_INPUT(GPIOJ_PIN13) |          \
                                     PIN_MODE_INPUT(GPIOJ_PIN14) |          \
                                     PIN_MODE_INPUT(GPIOJ_PIN15))
#define VAL_GPIOJ_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOJ_IO0_PWR_ON_OFF) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOJ_IO1_PWR_ON_OFF) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOJ_PIN2) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOJ_SPI_FLASH_WP) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOJ_SPI_FLASH_HOLD) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOJ_RELAY) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOJ_PIN6) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOJ_PIN7) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOJ_PIN8) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOJ_PIN9) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOJ_PIN10) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOJ_PIN11) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOJ_PULSE_COUNT) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOJ_PIN13) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOJ_PIN14) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOJ_PIN15))
#define VAL_GPIOJ_OSPEEDR           (PIN_OSPEED_LOW(GPIOJ_IO0_PWR_ON_OFF) |       \
                                     PIN_OSPEED_LOW(GPIOJ_IO1_PWR_ON_OFF) |       \
                                     PIN_OSPEED_HIGH(GPIOJ_PIN2) |        \
                                     PIN_OSPEED_LOW(GPIOJ_SPI_FLASH_WP) |       \
                                     PIN_OSPEED_LOW(GPIOJ_SPI_FLASH_HOLD) |       \
                                     PIN_OSPEED_LOW(GPIOJ_RELAY) |    \
                                     PIN_OSPEED_VERYLOW(GPIOJ_PIN6) |       \
                                     PIN_OSPEED_VERYLOW(GPIOJ_PIN7) |       \
                                     PIN_OSPEED_VERYLOW(GPIOJ_PIN8) |       \
                                     PIN_OSPEED_VERYLOW(GPIOJ_PIN9) |       \
                                     PIN_OSPEED_VERYLOW(GPIOJ_PIN10) |      \
                                     PIN_OSPEED_VERYLOW(GPIOJ_PIN11) |      \
                                     PIN_OSPEED_HIGH(GPIOJ_PULSE_COUNT) |      \
                                     PIN_OSPEED_VERYLOW(GPIOJ_PIN13) |      \
                                     PIN_OSPEED_VERYLOW(GPIOJ_PIN14) |      \
                                     PIN_OSPEED_VERYLOW(GPIOJ_PIN15))
#define VAL_GPIOJ_PUPDR             (PIN_PUPDR_PULLDOWN(GPIOJ_IO0_PWR_ON_OFF) |         \
                                     PIN_PUPDR_PULLDOWN(GPIOJ_IO1_PWR_ON_OFF) |         \
                                     PIN_PUPDR_PULLUP(GPIOJ_PIN2) |     \
                                     PIN_PUPDR_PULLUP(GPIOJ_SPI_FLASH_WP) |         \
                                     PIN_PUPDR_PULLUP(GPIOJ_SPI_FLASH_HOLD) |         \
                                     PIN_PUPDR_PULLDOWN(GPIOJ_RELAY) |      \
                                     PIN_PUPDR_PULLUP(GPIOJ_PIN6) |         \
                                     PIN_PUPDR_PULLUP(GPIOJ_PIN7) |         \
                                     PIN_PUPDR_PULLUP(GPIOJ_PIN8) |         \
                                     PIN_PUPDR_PULLUP(GPIOJ_PIN9) |         \
                                     PIN_PUPDR_PULLUP(GPIOJ_PIN10) |        \
                                     PIN_PUPDR_PULLUP(GPIOJ_PIN11) |        \
                                     PIN_PUPDR_FLOATING(GPIOJ_PULSE_COUNT) |        \
                                     PIN_PUPDR_PULLUP(GPIOJ_PIN13) |        \
                                     PIN_PUPDR_PULLUP(GPIOJ_PIN14) |        \
                                     PIN_PUPDR_PULLUP(GPIOJ_PIN15))
#define VAL_GPIOJ_ODR               (PIN_ODR_HIGH(GPIOJ_IO0_PWR_ON_OFF) |             \
                                     PIN_ODR_HIGH(GPIOJ_IO1_PWR_ON_OFF) |             \
                                     PIN_ODR_HIGH(GPIOJ_PIN2) |           \
                                     PIN_ODR_HIGH(GPIOJ_SPI_FLASH_WP) |             \
                                     PIN_ODR_HIGH(GPIOJ_SPI_FLASH_HOLD) |             \
                                     PIN_ODR_HIGH(GPIOJ_RELAY) |          \
                                     PIN_ODR_HIGH(GPIOJ_PIN6) |             \
                                     PIN_ODR_HIGH(GPIOJ_PIN7) |             \
                                     PIN_ODR_HIGH(GPIOJ_PIN8) |             \
                                     PIN_ODR_HIGH(GPIOJ_PIN9) |             \
                                     PIN_ODR_HIGH(GPIOJ_PIN10) |            \
                                     PIN_ODR_HIGH(GPIOJ_PIN11) |            \
                                     PIN_ODR_HIGH(GPIOJ_PULSE_COUNT) |            \
                                     PIN_ODR_HIGH(GPIOJ_PIN13) |            \
                                     PIN_ODR_HIGH(GPIOJ_PIN14) |            \
                                     PIN_ODR_HIGH(GPIOJ_PIN15))
#define VAL_GPIOJ_AFRL              (PIN_AFIO_AF(GPIOJ_IO0_PWR_ON_OFF, 0U) |          \
                                     PIN_AFIO_AF(GPIOJ_IO1_PWR_ON_OFF, 0U) |          \
                                     PIN_AFIO_AF(GPIOJ_PIN2, 0U) |       \
                                     PIN_AFIO_AF(GPIOJ_SPI_FLASH_WP, 0U) |          \
                                     PIN_AFIO_AF(GPIOJ_SPI_FLASH_HOLD, 0U) |          \
                                     PIN_AFIO_AF(GPIOJ_RELAY, 0U) |       \
                                     PIN_AFIO_AF(GPIOJ_PIN6, 0U) |          \
                                     PIN_AFIO_AF(GPIOJ_PIN7, 0U))
#define VAL_GPIOJ_AFRH              (PIN_AFIO_AF(GPIOJ_PIN8, 0U) |          \
                                     PIN_AFIO_AF(GPIOJ_PIN9, 0U) |          \
                                     PIN_AFIO_AF(GPIOJ_PIN10, 0U) |         \
                                     PIN_AFIO_AF(GPIOJ_PIN11, 0U) |         \
                                     PIN_AFIO_AF(GPIOJ_PULSE_COUNT, 0U) |         \
                                     PIN_AFIO_AF(GPIOJ_PIN13, 0U) |         \
                                     PIN_AFIO_AF(GPIOJ_PIN14, 0U) |         \
                                     PIN_AFIO_AF(GPIOJ_PIN15, 0U))

/*
* GPIOK setup:
*
* PK0  - PIN0                      (input pullup).
* PK1  - PIN1                      (input pullup).
* PK2  - PIN2                      (input pullup).
* PK3  - LCD_ENABLE                (output pulldown).
* PK4  - IO1_P6_IO                 (input pullup).
* PK5  - IO1_P5_IO                 (input pullup).
* PK6  - IO1_P20_ON_OFF            (output pulldown).
* PK7  - BUTTON_BOOT               (input pullup).
* PK8  - PIN8                      (input pullup).
* PK9  - PIN9                      (input pullup).
* PK10 - PIN10                     (input pullup).
* PK11 - PIN11                     (input pullup).
* PK12 - PIN12                     (input pullup).
* PK13 - PIN13                     (input pullup).
* PK14 - PIN14                     (input pullup).
* PK15 - PIN15                     (input pullup).
*/
#define VAL_GPIOK_MODER             (PIN_MODE_INPUT(GPIOK_PIN0) |           \
                                     PIN_MODE_INPUT(GPIOK_PIN1) |           \
                                     PIN_MODE_INPUT(GPIOK_PIN2) |           \
                                     PIN_MODE_OUTPUT(GPIOK_LCD_ENABLE) |          \
                                     PIN_MODE_INPUT(GPIOK_IO1_P6_IO) |           \
                                     PIN_MODE_INPUT(GPIOK_IO1_P5_IO) |           \
                                     PIN_MODE_OUTPUT(GPIOK_IO1_P20_IO) |           \
                                     PIN_MODE_INPUT(GPIOK_BUTTON_BOOT) |           \
                                     PIN_MODE_INPUT(GPIOK_PIN8) |           \
                                     PIN_MODE_INPUT(GPIOK_PIN9) |           \
                                     PIN_MODE_INPUT(GPIOK_PIN10) |          \
                                     PIN_MODE_INPUT(GPIOK_PIN11) |          \
                                     PIN_MODE_INPUT(GPIOK_PIN12) |          \
                                     PIN_MODE_INPUT(GPIOK_PIN13) |          \
                                     PIN_MODE_INPUT(GPIOK_PIN14) |          \
                                     PIN_MODE_INPUT(GPIOK_PIN15))
#define VAL_GPIOK_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOK_PIN0) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOK_PIN1) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOK_PIN2) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOK_LCD_ENABLE) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOK_IO1_P6_IO) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOK_IO1_P5_IO) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOK_IO1_P20_IO) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOK_BUTTON_BOOT) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOK_PIN8) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOK_PIN9) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOK_PIN10) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOK_PIN11) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOK_PIN12) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOK_PIN13) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOK_PIN14) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOK_PIN15))
#define VAL_GPIOK_OSPEEDR           (PIN_OSPEED_VERYLOW(GPIOK_PIN0) |       \
                                     PIN_OSPEED_VERYLOW(GPIOK_PIN1) |       \
                                     PIN_OSPEED_VERYLOW(GPIOK_PIN2) |       \
                                     PIN_OSPEED_VERYLOW(GPIOK_LCD_ENABLE) |          \
                                     PIN_OSPEED_HIGH(GPIOK_IO1_P6_IO) |       \
                                     PIN_OSPEED_HIGH(GPIOK_IO1_P5_IO) |       \
                                     PIN_OSPEED_LOW(GPIOK_IO1_P20_IO) |       \
                                     PIN_OSPEED_HIGH(GPIOK_BUTTON_BOOT) |       \
                                     PIN_OSPEED_VERYLOW(GPIOK_PIN8) |       \
                                     PIN_OSPEED_VERYLOW(GPIOK_PIN9) |       \
                                     PIN_OSPEED_VERYLOW(GPIOK_PIN10) |      \
                                     PIN_OSPEED_VERYLOW(GPIOK_PIN11) |      \
                                     PIN_OSPEED_VERYLOW(GPIOK_PIN12) |      \
                                     PIN_OSPEED_VERYLOW(GPIOK_PIN13) |      \
                                     PIN_OSPEED_VERYLOW(GPIOK_PIN14) |      \
                                     PIN_OSPEED_VERYLOW(GPIOK_PIN15))
#define VAL_GPIOK_PUPDR             (PIN_PUPDR_PULLUP(GPIOK_PIN0) |         \
                                     PIN_PUPDR_PULLUP(GPIOK_PIN1) |         \
                                     PIN_PUPDR_PULLUP(GPIOK_PIN2) |         \
                                     PIN_PUPDR_FLOATING(GPIOK_LCD_ENABLE) |       \
                                     PIN_PUPDR_FLOATING(GPIOK_IO1_P6_IO) |         \
                                     PIN_PUPDR_FLOATING(GPIOK_IO1_P5_IO) |         \
                                     PIN_PUPDR_FLOATING(GPIOK_IO1_P20_IO) |         \
                                     PIN_PUPDR_FLOATING(GPIOK_BUTTON_BOOT) |         \
                                     PIN_PUPDR_PULLUP(GPIOK_PIN8) |         \
                                     PIN_PUPDR_PULLUP(GPIOK_PIN9) |         \
                                     PIN_PUPDR_PULLUP(GPIOK_PIN10) |        \
                                     PIN_PUPDR_PULLUP(GPIOK_PIN11) |        \
                                     PIN_PUPDR_PULLUP(GPIOK_PIN12) |        \
                                     PIN_PUPDR_PULLUP(GPIOK_PIN13) |        \
                                     PIN_PUPDR_PULLUP(GPIOK_PIN14) |        \
                                     PIN_PUPDR_PULLUP(GPIOK_PIN15))
#define VAL_GPIOK_ODR               (PIN_ODR_HIGH(GPIOK_PIN0) |             \
                                     PIN_ODR_HIGH(GPIOK_PIN1) |             \
                                     PIN_ODR_HIGH(GPIOK_PIN2) |             \
                                     PIN_ODR_HIGH(GPIOK_LCD_ENABLE) |             \
                                     PIN_ODR_HIGH(GPIOK_IO1_P6_IO) |             \
                                     PIN_ODR_HIGH(GPIOK_IO1_P5_IO) |             \
                                     PIN_ODR_HIGH(GPIOK_IO1_P20_IO) |             \
                                     PIN_ODR_HIGH(GPIOK_BUTTON_BOOT) |             \
                                     PIN_ODR_HIGH(GPIOK_PIN8) |             \
                                     PIN_ODR_HIGH(GPIOK_PIN9) |             \
                                     PIN_ODR_HIGH(GPIOK_PIN10) |            \
                                     PIN_ODR_HIGH(GPIOK_PIN11) |            \
                                     PIN_ODR_HIGH(GPIOK_PIN12) |            \
                                     PIN_ODR_HIGH(GPIOK_PIN13) |            \
                                     PIN_ODR_HIGH(GPIOK_PIN14) |            \
                                     PIN_ODR_HIGH(GPIOK_PIN15))
#define VAL_GPIOK_AFRL              (PIN_AFIO_AF(GPIOK_PIN0, 0U) |          \
                                     PIN_AFIO_AF(GPIOK_PIN1, 0U) |          \
                                     PIN_AFIO_AF(GPIOK_PIN2, 0U) |          \
                                     PIN_AFIO_AF(GPIOK_LCD_ENABLE, 0U) |      \
                                     PIN_AFIO_AF(GPIOK_IO1_P6_IO, 0U) |          \
                                     PIN_AFIO_AF(GPIOK_IO1_P5_IO, 0U) |          \
                                     PIN_AFIO_AF(GPIOK_IO1_P20_IO, 0U) |          \
                                     PIN_AFIO_AF(GPIOK_BUTTON_BOOT, 0U))
#define VAL_GPIOK_AFRH              (PIN_AFIO_AF(GPIOK_PIN8, 0U) |          \
                                     PIN_AFIO_AF(GPIOK_PIN9, 0U) |          \
                                     PIN_AFIO_AF(GPIOK_PIN10, 0U) |         \
                                     PIN_AFIO_AF(GPIOK_PIN11, 0U) |         \
                                     PIN_AFIO_AF(GPIOK_PIN12, 0U) |         \
                                     PIN_AFIO_AF(GPIOK_PIN13, 0U) |         \
                                     PIN_AFIO_AF(GPIOK_PIN14, 0U) |         \
                                     PIN_AFIO_AF(GPIOK_PIN15, 0U))




/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

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