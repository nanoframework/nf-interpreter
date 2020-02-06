//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _STM32F7LCDSUPPORT
#define _STM32F7LCDSUPPORT

#include <stdint.h>
#include <stddef.h>

#define  STM32F769xx
#include "stm32f7xx.h"

#define NVIC_SetPriority __NVIC_SetPriority  // Fix for undefined in core_cm7.h
                                             // Chibios must "see" this in another way?
#include "core_cm7.h"


uint32_t HAL_GetTick(void);
void wait_ms(int);
#define __STM32F7LCDSUPPORT

#define UNUSED(X) (void)X      /* To avoid gcc/g++ warnings */
#define  USE_HAL_DSI_REGISTER_CALLBACKS         0U /* DSI register callback disabled       */

#define __IO       volatile             //  Defines 'read / write' permissions 
#define assert_param(expr) ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))

void assert_failed(uint8_t* file, uint32_t line)
{
    UNUSED(file);
    UNUSED(line);
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
       /* Infinite loop */
    while (1)
    {
    }
}
typedef enum
{
    GPIO_PIN_RESET = 0,
    GPIO_PIN_SET
} GPIO_PinState;

#define GPIO_PIN_0                 ((uint16_t)0x0001U)  // Pin 0 selected
#define GPIO_PIN_1                 ((uint16_t)0x0002U)  // Pin 1 selected
#define GPIO_PIN_2                 ((uint16_t)0x0004U)  // Pin 2 selected
#define GPIO_PIN_3                 ((uint16_t)0x0008U)  // Pin 3 selected
#define GPIO_PIN_4                 ((uint16_t)0x0010U)  // Pin 4 selected
#define GPIO_PIN_5                 ((uint16_t)0x0020U)  // Pin 5 selected
#define GPIO_PIN_6                 ((uint16_t)0x0040U)  // Pin 6 selected
#define GPIO_PIN_7                 ((uint16_t)0x0080U)  // Pin 7 selected
#define GPIO_PIN_8                 ((uint16_t)0x0100U)  // Pin 8 selected
#define GPIO_PIN_9                 ((uint16_t)0x0200U)  // Pin 9 selected
#define GPIO_PIN_10                ((uint16_t)0x0400U)  // Pin 10 selected
#define GPIO_PIN_11                ((uint16_t)0x0800U)  // Pin 11 selected
#define GPIO_PIN_12                ((uint16_t)0x1000U)  // Pin 12 selected
#define GPIO_PIN_13                ((uint16_t)0x2000U)  // Pin 13 selected
#define GPIO_PIN_14                ((uint16_t)0x4000U)  // Pin 14 selected
#define GPIO_PIN_15                ((uint16_t)0x8000U)  // Pin 15 selected
#define GPIO_PIN_All               ((uint16_t)0xFFFFU)  // All pins selected


#define GPIO_GET_INDEX(__GPIOx__)   (uint8_t)(((__GPIOx__) == (GPIOA))? 0U :\
                                              ((__GPIOx__) == (GPIOB))? 1U :\
                                              ((__GPIOx__) == (GPIOC))? 2U :\
                                              ((__GPIOx__) == (GPIOD))? 3U :\
                                              ((__GPIOx__) == (GPIOE))? 4U :\
                                              ((__GPIOx__) == (GPIOF))? 5U :\
                                              ((__GPIOx__) == (GPIOG))? 6U :\
                                              ((__GPIOx__) == (GPIOH))? 7U :\
                                              ((__GPIOx__) == (GPIOI))? 8U :\
                                              ((__GPIOx__) == (GPIOJ))? 9U : 10U)
#define GPIO_AF0_RTC_50Hz      ((uint8_t)0x00U)  /* RTC_50Hz Alternate Function mapping                       */
#define GPIO_AF0_MCO           ((uint8_t)0x00U)  /* MCO (MCO1 and MCO2) Alternate Function mapping            */
#define GPIO_AF0_SWJ           ((uint8_t)0x00U)  /* SWJ (SWD and JTAG) Alternate Function mapping             */
#define GPIO_AF0_TRACE         ((uint8_t)0x00U)  /* TRACE Alternate Function mapping                          */

#define GPIO_AF1_TIM1          ((uint8_t)0x01U)  /* TIM1 Alternate Function mapping */
#define GPIO_AF1_TIM2          ((uint8_t)0x01U)  /* TIM2 Alternate Function mapping */
#define GPIO_AF1_UART5         ((uint8_t)0x01U)  /* UART5 Alternate Function mapping */
#define GPIO_AF1_I2C4          ((uint8_t)0x01U)  /* I2C4 Alternate Function mapping  */   

#define GPIO_AF2_TIM3          ((uint8_t)0x02U)  /* TIM3 Alternate Function mapping */
#define GPIO_AF2_TIM4          ((uint8_t)0x02U)  /* TIM4 Alternate Function mapping */
#define GPIO_AF2_TIM5          ((uint8_t)0x02U)  /* TIM5 Alternate Function mapping */

#define GPIO_AF3_TIM8          ((uint8_t)0x03U)  /* TIM8 Alternate Function mapping  */
#define GPIO_AF3_TIM9          ((uint8_t)0x03U)  /* TIM9 Alternate Function mapping  */
#define GPIO_AF3_TIM10         ((uint8_t)0x03U)  /* TIM10 Alternate Function mapping */
#define GPIO_AF3_TIM11         ((uint8_t)0x03U)  /* TIM11 Alternate Function mapping */
#define GPIO_AF3_LPTIM1        ((uint8_t)0x03U)  /* LPTIM1 Alternate Function mapping */
#define GPIO_AF3_CEC           ((uint8_t)0x03U)  /* CEC Alternate Function mapping */
#define GPIO_AF3_DFSDM1         ((uint8_t)0x03U)  /* DFSDM1 Alternate Function mapping */

#define GPIO_AF4_I2C1          ((uint8_t)0x04U)  /* I2C1 Alternate Function mapping */
#define GPIO_AF4_I2C2          ((uint8_t)0x04U)  /* I2C2 Alternate Function mapping */
#define GPIO_AF4_I2C3          ((uint8_t)0x04U)  /* I2C3 Alternate Function mapping */
#define GPIO_AF4_I2C4          ((uint8_t)0x04U)  /* I2C4 Alternate Function mapping */
#define GPIO_AF4_CEC           ((uint8_t)0x04U)  /* CEC Alternate Function mapping */
#define GPIO_AF4_USART1        ((uint8_t)0x04)  /* USART1 Alternate Function mapping */

#define GPIO_AF5_SPI1          ((uint8_t)0x05U)  /* SPI1 Alternate Function mapping        */
#define GPIO_AF5_SPI2          ((uint8_t)0x05U)  /* SPI2/I2S2 Alternate Function mapping   */
#define GPIO_AF5_SPI3          ((uint8_t)0x05U)  /* SPI3/I2S3 Alternate Function mapping   */
#define GPIO_AF5_SPI4          ((uint8_t)0x05U)  /* SPI4 Alternate Function mapping        */
#define GPIO_AF5_SPI5          ((uint8_t)0x05U)  /* SPI5 Alternate Function mapping        */
#define GPIO_AF5_SPI6          ((uint8_t)0x05U)  /* SPI6 Alternate Function mapping        */

#define GPIO_AF6_SPI3          ((uint8_t)0x06U)  /* SPI3/I2S3 Alternate Function mapping  */
#define GPIO_AF6_SAI1          ((uint8_t)0x06U)  /* SAI1 Alternate Function mapping       */
#define GPIO_AF6_UART4         ((uint8_t)0x06U)   /* UART4 Alternate Function mapping     */   
#define GPIO_AF6_DFSDM1        ((uint8_t)0x06U)  /* DFSDM1 Alternate Function mapping     */

#define GPIO_AF7_USART1        ((uint8_t)0x07U)  /* USART1 Alternate Function mapping     */
#define GPIO_AF7_USART2        ((uint8_t)0x07U)  /* USART2 Alternate Function mapping     */
#define GPIO_AF7_USART3        ((uint8_t)0x07U)  /* USART3 Alternate Function mapping     */
#define GPIO_AF7_UART5         ((uint8_t)0x07U)  /* UART5 Alternate Function mapping      */
#define GPIO_AF7_SPDIFRX       ((uint8_t)0x07U)  /* SPDIF-RX Alternate Function mapping   */
#define GPIO_AF7_SPI2          ((uint8_t)0x07U)  /* SPI2 Alternate Function mapping       */
#define GPIO_AF7_SPI3          ((uint8_t)0x07U)  /* SPI3 Alternate Function mapping       */
#define GPIO_AF7_SPI6          ((uint8_t)0x07U)  /* SPI6 Alternate Function mapping       */
#define GPIO_AF7_DFSDM1         ((uint8_t)0x07U) /* DFSDM1 Alternate Function mapping      */

#define GPIO_AF8_UART4         ((uint8_t)0x08U)  /* UART4 Alternate Function mapping  */
#define GPIO_AF8_UART5         ((uint8_t)0x08U)  /* UART5 Alternate Function mapping  */
#define GPIO_AF8_USART6        ((uint8_t)0x08U)  /* USART6 Alternate Function mapping */
#define GPIO_AF8_UART7         ((uint8_t)0x08U)  /* UART7 Alternate Function mapping  */
#define GPIO_AF8_UART8         ((uint8_t)0x08U)  /* UART8 Alternate Function mapping  */
#define GPIO_AF8_SPDIFRX       ((uint8_t)0x08U)  /* SPIDIF-RX Alternate Function mapping */
#define GPIO_AF8_SAI2          ((uint8_t)0x08U)  /* SAI2 Alternate Function mapping   */
#define GPIO_AF8_SPI6          ((uint8_t)0x08U)  /* SPI6 Alternate Function mapping   */  

#define GPIO_AF9_CAN1          ((uint8_t)0x09U)  /* CAN1 Alternate Function mapping    */
#define GPIO_AF9_CAN2          ((uint8_t)0x09U)  /* CAN2 Alternate Function mapping    */
#define GPIO_AF9_TIM12         ((uint8_t)0x09U)  /* TIM12 Alternate Function mapping   */
#define GPIO_AF9_TIM13         ((uint8_t)0x09U)  /* TIM13 Alternate Function mapping   */
#define GPIO_AF9_TIM14         ((uint8_t)0x09U)  /* TIM14 Alternate Function mapping   */
#define GPIO_AF9_QUADSPI       ((uint8_t)0x09U)  /* QUADSPI Alternate Function mapping */
#define GPIO_AF9_LTDC          ((uint8_t)0x09U)  /* LCD-TFT Alternate Function mapping */
#define GPIO_AF9_FMC           ((uint8_t)0x09U)   /* FMC Alternate Function mapping     */
#define GPIO_AF9_LTDC          ((uint8_t)0x09U)  /* LCD-TFT Alternate Function mapping */

#define GPIO_AF10_OTG_FS        ((uint8_t)0xAU)  /* OTG_FS Alternate Function mapping */
#define GPIO_AF10_OTG_HS        ((uint8_t)0xAU)  /* OTG_HS Alternate Function mapping */
#define GPIO_AF10_QUADSPI       ((uint8_t)0xAU)  /* QUADSPI Alternate Function mapping */
#define GPIO_AF10_SAI2          ((uint8_t)0xAU)  /* SAI2 Alternate Function mapping */
#define GPIO_AF10_DFSDM1        ((uint8_t)0x0AU)  /* DFSDM1 Alternate Function mapping  */
#define GPIO_AF10_SDMMC2        ((uint8_t)0x0AU)  /* SDMMC2 Alternate Function mapping */   
#define GPIO_AF10_LTDC          ((uint8_t)0x0AU)  /* LCD-TFT Alternate Function mapping */

#define GPIO_AF11_ETH           ((uint8_t)0x0BU)  /* ETHERNET Alternate Function mapping */
#define GPIO_AF11_CAN3          ((uint8_t)0x0BU)  /* CAN3 Alternate Function mapping     */
#define GPIO_AF11_SDMMC2        ((uint8_t)0x0BU)  /* SDMMC2 Alternate Function mapping   */
#define GPIO_AF11_I2C4          ((uint8_t)0x0BU)  /* I2C4 Alternate Function mapping     */   

#define GPIO_AF12_FMC           ((uint8_t)0xCU)  /* FMC Alternate Function mapping                      */
#define GPIO_AF12_OTG_HS_FS     ((uint8_t)0xCU)  /* OTG HS configured in FS, Alternate Function mapping */
#define GPIO_AF12_SDMMC1        ((uint8_t)0xCU)  /* SDMMC1 Alternate Function mapping                   */
#define GPIO_AF12_MDIOS         ((uint8_t)0xCU)  /* SDMMC1 Alternate Function mapping                    */
#define GPIO_AF12_UART7         ((uint8_t)0xCU)  /* UART7 Alternate Function mapping                     */   

#define GPIO_AF13_DCMI          ((uint8_t)0x0DU)  /* DCMI Alternate Function mapping */
#define GPIO_AF13_DSI           ((uint8_t)0x0DU)  /* DSI Alternate Function mapping  */
#define GPIO_AF14_LTDC          ((uint8_t)0x0EU)  /* LCD-TFT Alternate Function mapping */
#define GPIO_AF15_EVENTOUT      ((uint8_t)0x0FU)  /* EVENTOUT Alternate Function mapping */

#define  GPIO_SPEED_FREQ_LOW         ((uint32_t)0x00000000U)  /*!< Low speed     */
#define  GPIO_SPEED_FREQ_MEDIUM      ((uint32_t)0x00000001U)  /*!< Medium speed  */
#define  GPIO_SPEED_FREQ_HIGH        ((uint32_t)0x00000002U)  /*!< Fast speed    */
#define  GPIO_SPEED_FREQ_VERY_HIGH   ((uint32_t)0x00000003U)  /*!< High speed    */
#define  GPIO_SPEED_FAST             GPIO_SPEED_FREQ_HIGH
#define  GPIO_SPEED_HIGH             GPIO_SPEED_FREQ_VERY_HIGH
#define  GPIO_SPEED_LOW              GPIO_SPEED_FREQ_LOW
#define  GPIO_SPEED_MEDIUM           GPIO_SPEED_FREQ_MEDIUM

#define  GPIO_MODE_INPUT               ((uint32_t)0x00000000U)   /*!< Input Floating Mode                   */
#define  GPIO_MODE_OUTPUT_PP           ((uint32_t)0x00000001U)   /*!< Output Push Pull Mode                 */
#define  GPIO_MODE_OUTPUT_OD           ((uint32_t)0x00000011U)   /*!< Output Open Drain Mode                */
#define  GPIO_MODE_AF_PP               ((uint32_t)0x00000002U)   /*!< Alternate Function Push Pull Mode     */
#define  GPIO_MODE_AF_OD               ((uint32_t)0x00000012U)   /*!< Alternate Function Open Drain Mode    */
#define  GPIO_MODE_ANALOG              ((uint32_t)0x00000003U)   /*!< Analog Mode  */

#define  GPIO_MODE_IT_RISING           ((uint32_t)0x10110000U)   /*!< External Interrupt Mode with Rising edge trigger detection          */
#define  GPIO_MODE_IT_FALLING          ((uint32_t)0x10210000U)   /*!< External Interrupt Mode with Falling edge trigger detection         */
#define  GPIO_MODE_IT_RISING_FALLING   ((uint32_t)0x10310000U)   /*!< External Interrupt Mode with Rising/Falling edge trigger detection  */

#define  GPIO_MODE_EVT_RISING          ((uint32_t)0x10120000U)   /*!< External Event Mode with Rising edge trigger detection               */
#define  GPIO_MODE_EVT_FALLING         ((uint32_t)0x10220000U)   /*!< External Event Mode with Falling edge trigger detection              */
#define  GPIO_MODE_EVT_RISING_FALLING  ((uint32_t)0x10320000U)   /*!< External Event Mode with Rising/Falling edge trigger detection       */

#define GPIO_NOPULL                    ((uint32_t)0x00000000U)   // No Pull-up or Pull-down activation
#define GPIO_PULLUP                    ((uint32_t)0x00000001U)   // Pull-up activation
#define GPIO_PULLDOWN                  ((uint32_t)0x00000002U)   /*!< Pull-down activation                */

#define GPIO_MODE                      ((uint32_t)0x00000003U)
#define GPIO_MODE_IT                   ((uint32_t)0x00010000U)
#define GPIO_MODE_EVT                  ((uint32_t)0x00020000U)
#define GPIO_OUTPUT_TYPE               ((uint32_t)0x00000010U)

#define GPIO_NUMBER                    ((uint32_t)16U)

#define GPIO_PIN_MASK                 ((uint32_t)0x0000FFFFU) /* PIN mask for assert test */
#define GET_GPIO_INDEX                GPIO_GET_INDEX

#define IS_GPIO_SPEED(SPEED) (((SPEED) == GPIO_SPEED_LOW)  || ((SPEED) == GPIO_SPEED_MEDIUM) || \
                              ((SPEED) == GPIO_SPEED_FAST) || ((SPEED) == GPIO_SPEED_HIGH))
#define IS_GPIO_PIN(__PIN__)        ((((__PIN__) & GPIO_PIN_MASK) != (uint32_t)0x00))
#define IS_GPIO_PIN_ACTION(ACTION) (((ACTION) == GPIO_PIN_RESET) || ((ACTION) == GPIO_PIN_SET))
#define IS_GPIO_PULL(PULL) (((PULL) == GPIO_NOPULL) || ((PULL) == GPIO_PULLUP) || \
                            ((PULL) == GPIO_PULLDOWN))
#define IS_GPIO_MODE(MODE) (((MODE) == GPIO_MODE_INPUT)              ||\
                            ((MODE) == GPIO_MODE_OUTPUT_PP)          ||\
                            ((MODE) == GPIO_MODE_OUTPUT_OD)          ||\
                            ((MODE) == GPIO_MODE_AF_PP)              ||\
                            ((MODE) == GPIO_MODE_AF_OD)              ||\
                            ((MODE) == GPIO_MODE_IT_RISING)          ||\
                            ((MODE) == GPIO_MODE_IT_FALLING)         ||\
                            ((MODE) == GPIO_MODE_IT_RISING_FALLING)  ||\
                            ((MODE) == GPIO_MODE_EVT_RISING)         ||\
                            ((MODE) == GPIO_MODE_EVT_FALLING)        ||\
                            ((MODE) == GPIO_MODE_EVT_RISING_FALLING) ||\
                            ((MODE) == GPIO_MODE_ANALOG))
#define IS_GPIO_AF_INSTANCE(__INSTANCE__)   (((__INSTANCE__) == GPIOA) || \
                                             ((__INSTANCE__) == GPIOB) || \
                                             ((__INSTANCE__) == GPIOC) || \
                                             ((__INSTANCE__) == GPIOD) || \
                                             ((__INSTANCE__) == GPIOE) || \
                                             ((__INSTANCE__) == GPIOF) || \
                                             ((__INSTANCE__) == GPIOG) || \
                                             ((__INSTANCE__) == GPIOH) || \
                                             ((__INSTANCE__) == GPIOI) || \
                                             ((__INSTANCE__) == GPIOJ) || \
                                             ((__INSTANCE__) == GPIOK))
#define IS_GPIO_AF(AF)   (((AF) == GPIO_AF0_RTC_50Hz)   || ((AF) == GPIO_AF1_TIM1)        || \
                          ((AF) == GPIO_AF0_SWJ)        || ((AF) == GPIO_AF0_TRACE)      || \
                          ((AF) == GPIO_AF0_MCO)        || ((AF) == GPIO_AF1_TIM2)       || \
                          ((AF) == GPIO_AF2_TIM3)       || ((AF) == GPIO_AF2_TIM4)       || \
                          ((AF) == GPIO_AF2_TIM5)       || ((AF) == GPIO_AF3_TIM8)       || \
                          ((AF) == GPIO_AF3_TIM9)       || ((AF) == GPIO_AF3_TIM10)      || \
                          ((AF) == GPIO_AF3_TIM11)      || ((AF) == GPIO_AF3_LPTIM1)     || \
                          ((AF) == GPIO_AF3_CEC)        || ((AF) == GPIO_AF4_CEC)        || \
                          ((AF) == GPIO_AF4_I2C1)       || ((AF) == GPIO_AF4_I2C2)       || \
                          ((AF) == GPIO_AF4_I2C3)       || ((AF) == GPIO_AF4_I2C4)       || \
                          ((AF) == GPIO_AF5_SPI1)       || ((AF) == GPIO_AF5_SPI2)       || \
                          ((AF) == GPIO_AF5_SPI3)       || ((AF) == GPIO_AF5_SPI4)       || \
                          ((AF) == GPIO_AF5_SPI5)       || ((AF) == GPIO_AF5_SPI6)       || \
                          ((AF) == GPIO_AF6_SPI3)       || ((AF) == GPIO_AF6_SAI1)       || \
                          ((AF) == GPIO_AF7_SPI3)       || ((AF) == GPIO_AF7_SPI2)       || \
                          ((AF) == GPIO_AF7_USART1)     || ((AF) == GPIO_AF7_USART2)     || \
                          ((AF) == GPIO_AF7_USART3)     || ((AF) == GPIO_AF7_UART5)      || \
                          ((AF) == GPIO_AF7_SPDIFRX)    || ((AF) == GPIO_AF8_SPDIFRX)    || \
                          ((AF) == GPIO_AF8_SAI2)       || ((AF) == GPIO_AF8_USART6)     || \
                          ((AF) == GPIO_AF8_UART4)      || ((AF) == GPIO_AF8_UART5)      || \
                          ((AF) == GPIO_AF8_UART7)      || ((AF) == GPIO_AF8_UART8)      || \
                          ((AF) == GPIO_AF9_CAN1)       || ((AF) == GPIO_AF9_CAN2)       || \
                          ((AF) == GPIO_AF9_TIM12)      || ((AF) == GPIO_AF9_TIM12)      || \
                          ((AF) == GPIO_AF9_TIM14)      || ((AF) == GPIO_AF9_QUADSPI)    || \
                          ((AF) == GPIO_AF9_LTDC)       || ((AF) == GPIO_AF10_OTG_FS)    || \
                          ((AF) == GPIO_AF10_OTG_HS)    || ((AF) == GPIO_AF10_SAI2)      || \
                          ((AF) == GPIO_AF10_QUADSPI)   || ((AF) == GPIO_AF11_ETH)       || \
                          ((AF) == GPIO_AF10_SDMMC2)    || ((AF) == GPIO_AF11_SDMMC2)    || \
                          ((AF) == GPIO_AF11_CAN3)      || ((AF) == GPIO_AF12_OTG_HS_FS) || \
                          ((AF) == GPIO_AF12_SDMMC1)    || ((AF) == GPIO_AF12_FMC)       || \
                          ((AF) == GPIO_AF15_EVENTOUT)  || ((AF) == GPIO_AF13_DCMI)      || \
                          ((AF) == GPIO_AF14_LTDC)      || ((AF) == GPIO_AF13_DSI))

#define EXTI_MODE                      ((uint32_t)0x10000000U)

typedef struct
{
    uint32_t Pin;       /*!< Specifies the GPIO pins to be configured.
                             This parameter can be any value of @ref GPIO_pins_define */
    uint32_t Mode;      /*!< Specifies the operating mode for the selected pins.
                             This parameter can be a value of @ref GPIO_mode_define */
    uint32_t Pull;      /*!< Specifies the Pull-up or Pull-Down activation for the selected pins.
                             This parameter can be a value of @ref GPIO_pull_define */
    uint32_t Speed;     /*!< Specifies the speed for the selected pins.
                             This parameter can be a value of @ref GPIO_speed_define */
    uint32_t Alternate;  /*!< Peripheral to be connected to the selected pins.
                              This parameter can be a value of @ref GPIO_Alternate_function_selection */
} GPIO_InitTypeDef;






#define HAL_DSI_ERROR_NONE          0U
#define DSI_PLL_IN_DIV1             0x00000001U
#define DSI_PLL_IN_DIV2             0x00000002U
#define DSI_PLL_IN_DIV3             0x00000003U
#define DSI_PLL_IN_DIV4             0x00000004U
#define DSI_PLL_IN_DIV5             0x00000005U
#define DSI_PLL_IN_DIV6             0x00000006U
#define DSI_PLL_IN_DIV7             0x00000007U

#define DSI_PLL_OUT_DIV1            0x00000000U
#define DSI_PLL_OUT_DIV2            0x00000001U
#define DSI_PLL_OUT_DIV4            0x00000002U
#define DSI_PLL_OUT_DIV8            0x00000003U

#define __HAL_DSI_REG_ENABLE(__HANDLE__) do { \
                                                  __IO uint32_t tmpreg = 0x00U; \
                                                  SET_BIT((__HANDLE__)->Instance->WRPCR, DSI_WRPCR_REGEN);\
                                                  /* Delay after an DSI regulator enabling */ \
                                                  tmpreg = READ_BIT((__HANDLE__)->Instance->WRPCR, DSI_WRPCR_REGEN);\
                                                  UNUSED(tmpreg); \
                                                } while(0U)

#define MAX_LAYER                      2
#define DSI_FLAG_RRS                   DSI_WISR_RRS
#define DSI_TIMEOUT_VALUE              ((uint32_t)1000U)  /* 1s */
#define DSI_ONE_DATA_LANE              0U
#define DSI_TWO_DATA_LANES             1U
#define DSI_VMCR_LPCE_Pos              (15U)                                    
#define DSI_VMCR_LPCE_Msk              (0x1U << DSI_VMCR_LPCE_Pos)              /*!< 0x00008000 */
#define DSI_VMCR_LPCE                  DSI_VMCR_LPCE_Msk                        /*!< Low-Power Command Enable */
#define DSI_VMCR_LPHFPE_Pos            (13U)                                    
#define DSI_VMCR_LPHFPE_Msk            (0x1U << DSI_VMCR_LPHFPE_Pos)            /*!< 0x00002000 */
#define DSI_VMCR_LPHFPE                DSI_VMCR_LPHFPE_Msk                      /*!< Low-Power Horizontal Front-Porch Enable */
#define DSI_LP_HFP_ENABLE              DSI_VMCR_LPHFPE
#define DSI_VMCR_LPHBPE_Pos            (12U)                                    
#define DSI_VMCR_LPHBPE_Msk            (0x1U << DSI_VMCR_LPHBPE_Pos)            /*!< 0x00001000 */
#define DSI_VMCR_LPHBPE                DSI_VMCR_LPHBPE_Msk                      /*!< Low-Power Horizontal Back-Porch Enable */
#define DSI_LP_HBP_ENABLE              DSI_VMCR_LPHBPE
#define DSI_VMCR_LPVAE_Pos             (11U)                                    
#define DSI_VMCR_LPVAE_Msk             (0x1U << DSI_VMCR_LPVAE_Pos)             /*!< 0x00000800 */
#define DSI_VMCR_LPVAE                 DSI_VMCR_LPVAE_Msk                       /*!< Low-Power Vertical Active Enable */
#define DSI_LP_VACT_ENABLE             DSI_VMCR_LPVAE
#define DSI_VMCR_LPVFPE_Pos            (10U)                                    
#define DSI_VMCR_LPVFPE_Msk            (0x1U << DSI_VMCR_LPVFPE_Pos)            /*!< 0x00000400 */
#define DSI_VMCR_LPVFPE                DSI_VMCR_LPVFPE_Msk                      /*!< Low-power Vertical Front-porch Enable */
#define DSI_LP_VFP_ENABLE              DSI_VMCR_LPVFPE
#define DSI_VMCR_LPVBPE_Pos            (9U)                                     
#define DSI_VMCR_LPVBPE_Msk            (0x1U << DSI_VMCR_LPVBPE_Pos)            /*!< 0x00000200 */
#define DSI_VMCR_LPVBPE                DSI_VMCR_LPVBPE_Msk                      /*!< Low-power Vertical Back-Porch Enable */
#define DSI_LP_VBP_ENABLE              DSI_VMCR_LPVBPE
#define DSI_VMCR_LPVSAE_Pos            (8U)                                     
#define DSI_VMCR_LPVSAE_Msk            (0x1U << DSI_VMCR_LPVSAE_Pos)            /*!< 0x00000100 */
#define DSI_VMCR_LPVSAE                DSI_VMCR_LPVSAE_Msk                      /*!< Low-Power Vertical Sync Active Enable */
#define DSI_LP_VSYNC_ENABLE            DSI_VMCR_LPVSAE
#define DSI_LOOSELY_PACKED_ENABLE      DSI_LCOLCR_LPE
#define DSI_LOOSELY_PACKED_DISABLE     0x00000000U
#define DSI_AUTO_CLK_LANE_CTRL_DISABLE 0x00000000U
#define DSI_AUTO_CLK_LANE_CTRL_ENABLE  DSI_CLCR_ACR
#define DSI_RGB565                     0x00000000U /*!< The values 0x00000001 and 0x00000002 can also be used for the RGB565 color mode configuration */
#define DSI_RGB666                     0x00000003U /*!< The value 0x00000004 can also be used for the RGB666 color mode configuration                 */
#define DSI_RGB888                     0x00000005U
#define DSI_FLAG_PLLLS                 DSI_WISR_PLLLS

#define IS_DSI_COLOR_CODING(ColorCoding)          ((ColorCoding) <= 5U)
#define IS_DSI_LOOSELY_PACKED(LooselyPacked)      (((LooselyPacked) == DSI_LOOSELY_PACKED_ENABLE) || ((LooselyPacked) == DSI_LOOSELY_PACKED_DISABLE))
#define IS_DSI_PLL_NDIV(NDIV)                     ((10U <= (NDIV)) && ((NDIV) <= 125U))
#define IS_DSI_PLL_IDF(IDF)                       (((IDF) == DSI_PLL_IN_DIV1)  || \
                                                  ((IDF) == DSI_PLL_IN_DIV2)   || \
                                                  ((IDF) == DSI_PLL_IN_DIV3)   || \
                                                  ((IDF) == DSI_PLL_IN_DIV4)   || \
                                                  ((IDF) == DSI_PLL_IN_DIV5)   || \
                                                  ((IDF) == DSI_PLL_IN_DIV6)   || \
                                                  ((IDF) == DSI_PLL_IN_DIV7))
#define IS_DSI_PLL_ODF(ODF)                       (((ODF) == DSI_PLL_OUT_DIV1) || \
                                                  ((ODF) == DSI_PLL_OUT_DIV2)  || \
                                                  ((ODF) == DSI_PLL_OUT_DIV4)  || \
                                                  ((ODF) == DSI_PLL_OUT_DIV8))
#define IS_DSI_AUTO_CLKLANE_CONTROL(AutoClkLane)  (((AutoClkLane) == DSI_AUTO_CLK_LANE_CTRL_DISABLE) || ((AutoClkLane) == DSI_AUTO_CLK_LANE_CTRL_ENABLE))
#define IS_DSI_NUMBER_OF_LANES(NumberOfLanes)     (((NumberOfLanes) == DSI_ONE_DATA_LANE) || ((NumberOfLanes) == DSI_TWO_DATA_LANES))
#define __HAL_DSI_PLL_DISABLE(__HANDLE__) do { \
                                              __IO uint32_t tmpreg = 0x00U; \
                                              CLEAR_BIT((__HANDLE__)->Instance->WRPCR, DSI_WRPCR_PLLEN);\
                                              /* Delay after an DSI PLL disabling */ \
                                              tmpreg = READ_BIT((__HANDLE__)->Instance->WRPCR, DSI_WRPCR_PLLEN);\
                                              UNUSED(tmpreg); \
                                             } while(0U)
#define __HAL_DSI_DISABLE(__HANDLE__) do { \
                                          __IO uint32_t tmpreg = 0x00U; \
                                          CLEAR_BIT((__HANDLE__)->Instance->CR, DSI_CR_EN);\
                                          /* Delay after an DSI Host disabling */ \
                                          tmpreg = READ_BIT((__HANDLE__)->Instance->CR, DSI_CR_EN);\
                                          UNUSED(tmpreg); \
                                         } while(0U)
#define __HAL_DSI_WRAPPER_DISABLE(__HANDLE__) do { \
                                                  __IO uint32_t tmpreg = 0x00U; \
                                                  CLEAR_BIT((__HANDLE__)->Instance->WCR, DSI_WCR_DSIEN);\
                                                  /* Delay after an DSI warpper disabling*/ \
                                                  tmpreg = READ_BIT((__HANDLE__)->Instance->WCR, DSI_WCR_DSIEN);\
                                                  UNUSED(tmpreg); \
                                                 } while(0U)
#define __HAL_DSI_REG_DISABLE(__HANDLE__) do { \
                                              __IO uint32_t tmpreg = 0x00U; \
                                              CLEAR_BIT((__HANDLE__)->Instance->WRPCR, DSI_WRPCR_REGEN);\
                                              /* Delay after an DSI regulator disabling */ \
                                              tmpreg = READ_BIT((__HANDLE__)->Instance->WRPCR, DSI_WRPCR_REGEN);\
                                              UNUSED(tmpreg); \
                                             } while(0U)
#define __HAL_DSI_GET_FLAG(__HANDLE__, __FLAG__) ((__HANDLE__)->Instance->WISR & (__FLAG__))
#define __HAL_DSI_PLL_ENABLE(__HANDLE__) do { \
                                                 __IO uint32_t tmpreg = 0x00U; \
                                                 SET_BIT((__HANDLE__)->Instance->WRPCR, DSI_WRPCR_PLLEN);\
                                                 /* Delay after an DSI PLL enabling */ \
                                                 tmpreg = READ_BIT((__HANDLE__)->Instance->WRPCR, DSI_WRPCR_PLLEN);\
                                                 UNUSED(tmpreg); \
                                                } while(0U)
#define __HAL_RCC_DSI_RELEASE_RESET()    (RCC->APB2RSTR &= ~(RCC_APB2RSTR_DSIRST))
#define __HAL_RCC_DSI_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_DSIEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_DSIEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)
#define DSI_DCS_SHORT_PKT_WRITE_P1          ((uint32_t)0x00000015U) /*!< DCS short write, one parameter      */
#define DSI_DCS_LONG_PKT_WRITE              0x00000039U /*!< DCS long write     */
#define DSI_GEN_LONG_PKT_WRITE              0x00000029U /*!< Generic long write */
#define IS_DSI_LONG_WRITE_PACKET_TYPE(MODE) (((MODE) == DSI_DCS_LONG_PKT_WRITE) || ((MODE) == DSI_GEN_LONG_PKT_WRITE))

#define RCC_PERIPHCLK_LTDC  ((uint32_t)0x00000008U)

typedef enum
{
    HAL_DSI_STATE_RESET = 0x00U,
    HAL_DSI_STATE_READY = 0x01U,
    HAL_DSI_STATE_ERROR = 0x02U,
    HAL_DSI_STATE_BUSY = 0x03U,
    HAL_DSI_STATE_TIMEOUT = 0x04U
} HAL_DSI_StateTypeDef;
typedef struct
{
    uint32_t PLLNDIV; // PLL Loop Division Factor
                      // This parameter must be a value between 10 and 125   
    uint32_t PLLIDF;  // PLL Input Division Factor
                      // This parameter can be any value of @ref DSI_PLL_IDF 
    uint32_t PLLODF;  // PLL Output Division Factor
                      // This parameter can be any value of @ref DSI_PLL_ODF 
} DSI_PLLInitTypeDef;

#define __HAL_UNLOCK(__HANDLE__)  do{ (__HANDLE__)->Lock = HAL_UNLOCKED;  } while (0U)
#define __HAL_RCC_LTDC_FORCE_RESET()         (RCC->APB2RSTR |= (RCC_APB2RSTR_LTDCRST))
#define __HAL_RCC_DMA2D_FORCE_RESET()        (RCC->AHB1RSTR |= (RCC_AHB1RSTR_DMA2DRST))
#define __HAL_RCC_DSI_FORCE_RESET()          (RCC->APB2RSTR |= (RCC_APB2RSTR_DSIRST))
#define __HAL_RCC_LTDC_CLK_DISABLE()         (RCC->APB2ENR &= ~(RCC_APB2ENR_LTDCEN))
#define __HAL_RCC_DMA2D_CLK_DISABLE()        (RCC->AHB1ENR &= ~(RCC_AHB1ENR_DMA2DEN))
#define __HAL_RCC_DSI_CLK_DISABLE()          (RCC->APB2ENR &= ~(RCC_APB2ENR_DSIEN))

#define HSE_VALUE    ((uint32_t)25000000) /*!< Default value of the External oscillator in Hz */	



#define HAL_LTDC_StructInitFromVideoConfig  HAL_LTDCEx_StructInitFromVideoConfig

#define __HAL_LOCK(__HANDLE__)                                           \
                                do{                                        \
                                    if((__HANDLE__)->Lock == HAL_LOCKED)   \
                                    {                                      \
                                       return HAL_BUSY;                    \
                                    }                                      \
                                    else                                   \
                                    {                                      \
                                       (__HANDLE__)->Lock = HAL_LOCKED;    \
                                    }                                      \
                                  } while (0U)





#define __HAL_RCC_DMA2D_RELEASE_RESET()  (RCC->AHB1RSTR &= ~(RCC_AHB1RSTR_DMA2DRST))
#define __HAL_RCC_DMA2D_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DMA2DEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DMA2DEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_RCC_LTDC_RELEASE_RESET()   (RCC->APB2RSTR &= ~(RCC_APB2RSTR_LTDCRST))
#define __HAL_RCC_LTDC_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_LTDCEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_LTDCEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define PLLSAI_TIMEOUT_VALUE       100U             /* Timeout value fixed to 100 ms */
#define __HAL_RCC_PLLSAI_GET_FLAG() ((RCC->CR & (RCC_CR_PLLSAIRDY)) == (RCC_CR_PLLSAIRDY))
#define __HAL_RCC_PLLSAI_ENABLE() (RCC->CR |= (RCC_CR_PLLSAION))
#define __HAL_RCC_PLLSAI_PLLSAICLKDIVR_CONFIG(__PLLSAIDivR__)\
                            MODIFY_REG(RCC->DCKCFGR1, RCC_DCKCFGR1_PLLSAIDIVR, (uint32_t)(__PLLSAIDivR__))
#define __HAL_RCC_PLLSAI_CONFIG(__PLLSAIN__, __PLLSAIP__, __PLLSAIQ__, __PLLSAIR__)              \
                               (RCC->PLLSAICFGR = ((__PLLSAIN__) << RCC_PLLSAICFGR_PLLSAIN_Pos) |\
                               ((__PLLSAIP__) << RCC_PLLSAICFGR_PLLSAIP_Pos)                    |\
                               ((__PLLSAIQ__) << RCC_PLLSAICFGR_PLLSAIQ_Pos)                    |\
                               ((__PLLSAIR__) << RCC_PLLSAICFGR_PLLSAIR_Pos))

#define RCC_PERIPHCLK_CLK48           ((uint32_t)0x00200000U)
#define RCC_CLK48SOURCE_PLLSAIP     RCC_DCKCFGR2_CK48MSEL
#define __HAL_RCC_PLLSAI_PLLSAICLKDIVQ_CONFIG(__PLLSAIDivQ__) (MODIFY_REG(RCC->DCKCFGR1, RCC_DCKCFGR1_PLLSAIDIVQ, ((__PLLSAIDivQ__)-1)<<8))
#define RCC_PERIPHCLK_SAI1            ((uint32_t)0x00080000U)
#define RCC_PERIPHCLK_SAI2            ((uint32_t)0x00100000U)

#define RCC_SAI1CLKSOURCE_PLLSAI             ((uint32_t)0x00000000U)
#define RCC_SAI2CLKSOURCE_PLLSAI             ((uint32_t)0x00000000U)

#define IS_RCC_PLLSAIN_VALUE(VALUE) ((50 <= (VALUE)) && ((VALUE) <= 432))

#define __HAL_RCC_PLLSAI_DISABLE() (RCC->CR &= ~(RCC_CR_PLLSAION))
#define PLLI2S_TIMEOUT_VALUE       100U             /* Timeout value fixed to 100 ms */

#define RCC_FLAG_MASK  ((uint8_t)0x1F)
#define RCC_FLAG_PLLI2SRDY               ((uint8_t)0x3BU)

#define __HAL_RCC_PLLI2S_ENABLE() (RCC->CR |= (RCC_CR_PLLI2SON))
#define __HAL_RCC_PLLI2S_CONFIG(__PLLI2SN__, __PLLI2SP__, __PLLI2SQ__, __PLLI2SR__)              \
                               (RCC->PLLI2SCFGR = ((__PLLI2SN__) << RCC_PLLI2SCFGR_PLLI2SN_Pos) |\
                               ((__PLLI2SP__) << RCC_PLLI2SCFGR_PLLI2SP_Pos)                    |\
                               ((__PLLI2SQ__) << RCC_PLLI2SCFGR_PLLI2SQ_Pos)                    |\
                               ((__PLLI2SR__) << RCC_PLLI2SCFGR_PLLI2SR_Pos))
#define RCC_PERIPHCLK_PLLI2S          ((uint32_t)0x02000000U)
#define RCC_PERIPHCLK_SPDIFRX         ((uint32_t)0x01000000U)
#define __HAL_RCC_PLLI2S_PLLSAICLKDIVQ_CONFIG(__PLLI2SDivQ__) (MODIFY_REG(RCC->DCKCFGR1, RCC_DCKCFGR1_PLLI2SDIVQ, (__PLLI2SDivQ__)-1))

#define RCC_SAI1CLKSOURCE_PLLI2S             RCC_DCKCFGR1_SAI1SEL_0
#define RCC_SAI2CLKSOURCE_PLLI2S             RCC_DCKCFGR1_SAI2SEL_0
#define RCC_PERIPHCLK_I2S             ((uint32_t)0x00000001U)
#define IS_RCC_PLLI2SN_VALUE(VALUE) ((50 <= (VALUE)) && ((VALUE) <= 432))
#define __HAL_RCC_I2C4_CONFIG(__I2C4_CLKSOURCE__)  MODIFY_REG(RCC->DCKCFGR2, RCC_DCKCFGR2_I2C4SEL, (uint32_t)(__I2C4_CLKSOURCE__))
#define RCC_PERIPHCLK_I2C4            ((uint32_t)0x00020000U)
#define __HAL_RCC_I2C3_CONFIG(__I2C3_CLKSOURCE__)  MODIFY_REG(RCC->DCKCFGR2, RCC_DCKCFGR2_I2C3SEL, (uint32_t)(__I2C3_CLKSOURCE__))
#define RCC_PERIPHCLK_I2C3            ((uint32_t)0x00010000U)
#define __HAL_RCC_I2C2_CONFIG(__I2C2_CLKSOURCE__)  MODIFY_REG(RCC->DCKCFGR2, RCC_DCKCFGR2_I2C2SEL, (uint32_t)(__I2C2_CLKSOURCE__))
#define RCC_PERIPHCLK_I2C2            ((uint32_t)0x00008000U)
#define __HAL_RCC_I2C1_CONFIG(__I2C1_CLKSOURCE__)  MODIFY_REG(RCC->DCKCFGR2, RCC_DCKCFGR2_I2C1SEL, (uint32_t)(__I2C1_CLKSOURCE__))
#define RCC_PERIPHCLK_I2C1            ((uint32_t)0x00004000U)

typedef enum
{
    HAL_LTDC_STATE_RESET = 0x00U,    // LTDC not yet initialized or disabled 
    HAL_LTDC_STATE_READY = 0x01U,    // LTDC initialized and ready for use   
    HAL_LTDC_STATE_BUSY = 0x02U,    // LTDC internal process is ongoing     
    HAL_LTDC_STATE_TIMEOUT = 0x03U,    // LTDC Timeout state                   
    HAL_LTDC_STATE_ERROR = 0x04U     // LTDC state error                     
} HAL_LTDC_StateTypeDef;
typedef struct
{
    uint8_t Blue;                    // Configures the blue value.
                                     // This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF. */
    uint8_t Green;                   // Configures the green value.
                                     // This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF. */
    uint8_t Red;                     // Configures the red value.
                                     // This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF. */
    uint8_t Reserved;                // Reserved 0xFF */
} LTDC_ColorTypeDef;
typedef struct
{
    uint32_t WindowX0;                   // Configures the Window Horizontal Start Position.
                                         // This parameter must be a number between Min_Data = 0x000 and Max_Data = 0xFFF. 
    uint32_t WindowX1;                   // Configures the Window Horizontal Stop Position.
                                         // This parameter must be a number between Min_Data = 0x000 and Max_Data = 0xFFF. 
    uint32_t WindowY0;                   // Configures the Window vertical Start Position.
                                         // This parameter must be a number between Min_Data = 0x000 and Max_Data = 0x7FF. 
    uint32_t WindowY1;                   // Configures the Window vertical Stop Position.
                                         // This parameter must be a number between Min_Data = 0x0000 and Max_Data = 0x7FF. 
    uint32_t PixelFormat;                // Specifies the pixel format.
                                         // This parameter can be one of value of @ref LTDC_Pixelformat 
    uint32_t Alpha;                      // Specifies the constant alpha used for blending.
                                         // This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF. 
    uint32_t Alpha0;                     // Configures the default alpha value.
                                         // This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF. 
    uint32_t BlendingFactor1;            // Select the blending factor 1.
                                         // This parameter can be one of value of @ref LTDC_BlendingFactor1 
    uint32_t BlendingFactor2;            // Select the blending factor 2.
                                         // This parameter can be one of value of @ref LTDC_BlendingFactor2 
    uint32_t FBStartAdress;              // Configures the color frame buffer address 
    uint32_t ImageWidth;                 // Configures the color frame buffer line length.
                                         // This parameter must be a number between Min_Data = 0x0000 and Max_Data = 0x1FFF. 
    uint32_t ImageHeight;                // Specifies the number of line in frame buffer.
                                         // This parameter must be a number between Min_Data = 0x000 and Max_Data = 0x7FF. 
    LTDC_ColorTypeDef   Backcolor;       // Configures the layer background color. 
} LTDC_LayerCfgTypeDef;
typedef struct
{
    uint32_t            HSPolarity;                // configures the horizontal synchronization polarity.
                                                   // This parameter can be one value of @ref LTDC_HS_POLARITY 
    uint32_t            VSPolarity;                // configures the vertical synchronization polarity.
                                                   // This parameter can be one value of @ref LTDC_VS_POLARITY 
    uint32_t            DEPolarity;                // configures the data enable polarity.
                                                   // This parameter can be one of value of @ref LTDC_DE_POLARITY 
    uint32_t            PCPolarity;                // configures the pixel clock polarity.
                                                   // This parameter can be one of value of @ref LTDC_PC_POLARITY 
    uint32_t            HorizontalSync;            // configures the number of Horizontal synchronization width.
                                                   // This parameter must be a number between Min_Data = 0x000 and Max_Data = 0xFFF. 
    uint32_t            VerticalSync;              // configures the number of Vertical synchronization height.
                                                   // This parameter must be a number between Min_Data = 0x000 and Max_Data = 0x7FF. 
    uint32_t            AccumulatedHBP;            // configures the accumulated horizontal back porch width.
                                                   // This parameter must be a number between Min_Data = LTDC_HorizontalSync and Max_Data = 0xFFF. 
    uint32_t            AccumulatedVBP;            // configures the accumulated vertical back porch height.
                                                   // This parameter must be a number between Min_Data = LTDC_VerticalSync and Max_Data = 0x7FF. 
    uint32_t            AccumulatedActiveW;        // configures the accumulated active width.
                                                   // This parameter must be a number between Min_Data = LTDC_AccumulatedHBP and Max_Data = 0xFFF. 
    uint32_t            AccumulatedActiveH;        // configures the accumulated active height.
                                                   // This parameter must be a number between Min_Data = LTDC_AccumulatedVBP and Max_Data = 0x7FF. 
    uint32_t            TotalWidth;                // configures the total width.
                                                   // This parameter must be a number between Min_Data = LTDC_AccumulatedActiveW and Max_Data = 0xFFF. 
    uint32_t            TotalHeigh;                // configures the total height.
                                                   // This parameter must be a number between Min_Data = LTDC_AccumulatedActiveH and Max_Data = 0x7FF. 
    LTDC_ColorTypeDef   Backcolor;                 // Configures the background color. 
} LTDC_InitTypeDef;
typedef enum
{
    HAL_UNLOCKED = 0x00U,
    HAL_LOCKED = 0x01U
} HAL_LockTypeDef;
typedef struct
{
    LTDC_TypeDef* Instance;                // LTDC Register base address                
    LTDC_InitTypeDef            Init;                     // LTDC parameters                           
    LTDC_LayerCfgTypeDef        LayerCfg[MAX_LAYER];      // LTDC Layers parameters                    
    HAL_LockTypeDef             Lock;                     // LTDC Lock                                 
    __IO HAL_LTDC_StateTypeDef  State;                    // LTDC state                                
    __IO uint32_t               ErrorCode;                // LTDC Error code                           
} LTDC_HandleTypeDef;

#define LTDC_LAYER(__HANDLE__, __LAYER__)              ((LTDC_Layer_TypeDef *)((uint32_t)(((uint32_t)((__HANDLE__)->Instance)) + 0x84U + (0x80U*(__LAYER__)))))
#define LTDC_PIXEL_FORMAT_ARGB8888        0x00000000U   /*!< ARGB8888 LTDC pixel format */
#define LTDC_PIXEL_FORMAT_RGB888          0x00000001U   /*!< RGB888 LTDC pixel format   */
#define LTDC_PIXEL_FORMAT_RGB565          0x00000002U   /*!< RGB565 LTDC pixel format   */
#define LTDC_PIXEL_FORMAT_ARGB1555        0x00000003U   /*!< ARGB1555 LTDC pixel format */
#define LTDC_PIXEL_FORMAT_ARGB4444        0x00000004U   /*!< ARGB4444 LTDC pixel format */
#define LTDC_PIXEL_FORMAT_L8              0x00000005U   /*!< L8 LTDC pixel format       */
#define LTDC_PIXEL_FORMAT_AL44            0x00000006U   /*!< AL44 LTDC pixel format     */
#define LTDC_PIXEL_FORMAT_AL88            0x00000007U   /*!< AL88 LTDC pixel format     */
#define LTDC_MAX_LAYER_NUMBER             ((uint32_t) 2)
#define LTDC_BLENDING_FACTOR1_PAxCA       ((uint32_t)0x00000600U)   // Blending factor : Cte Alpha x Pixel Alpha
#define LTDC_BLENDING_FACTOR2_PAxCA       ((uint32_t)0x00000007U)   // Blending factor : Cte Alpha x Pixel Alpha
#define LTDC_PCPOLARITY_IPC               ((uint32_t)0x00000000U)   // input pixel clock.
#define LTDC_DEPOLARITY_AL                0x00000000U   /*!< Data Enable, is active low. */
#define LTDC_DEPOLARITY_AH                LTDC_GCR_DEPOL            /*!< Data Enable, is active high. */
#define LTDC_VSPOLARITY_AH                LTDC_GCR_VSPOL            /*!< Vertical Synchronization is active high. */
#define LTDC_VSPOLARITY_AL                0x00000000U   /*!< Vertical Synchronization is active low. */
#define LTDC_HSPOLARITY_AL                0x00000000U   /*!< Horizontal Synchronization is active low. */
#define LTDC_HSPOLARITY_AH                LTDC_GCR_HSPOL            /*!< Horizontal Synchronization is active high. */
#define LTDC_IT_TE                        LTDC_IER_TERRIE           /*!< LTDC Transfer Error Interrupt  */
#define LTDC_IT_FU                        LTDC_IER_FUIE             /*!< LTDC FIFO Underrun Interrupt   */


typedef enum
{
    HAL_OK = 0x00U,
    HAL_ERROR = 0x01U,
    HAL_BUSY = 0x02U,
    HAL_TIMEOUT = 0x03U
} HAL_StatusTypeDef;
typedef struct
{
    uint32_t PLLI2SN;    // Specifies the multiplication factor for PLLI2S VCO output clock.
                         // This parameter must be a number between Min_Data = 50 and Max_Data = 432.
                         // This parameter will be used only when PLLI2S is selected as Clock Source I2S or SAI 

    uint32_t PLLI2SR;    // Specifies the division factor for I2S clock.
                         // This parameter must be a number between Min_Data = 2 and Max_Data = 7.
                         // This parameter will be used only when PLLI2S is selected as Clock Source I2S or SAI 

    uint32_t PLLI2SQ;    // Specifies the division factor for SAI1 clock.
                         // This parameter must be a number between Min_Data = 2 and Max_Data = 15.
                         // This parameter will be used only when PLLI2S is selected as Clock Source SAI 

    uint32_t PLLI2SP;    // Specifies the division factor for SPDIF-RX clock.
                         // This parameter must be a value of @ref RCCEx_PLLI2SP_Clock_Divider.
                         // This parameter will be used only when PLLI2S is selected as Clock Source SPDIF-RX 
} RCC_PLLI2SInitTypeDef;
typedef struct
{
    uint32_t PLLSAIN;    // Specifies the multiplication factor for PLLI2S VCO output clock.
                         // This parameter must be a number between Min_Data = 50 and Max_Data = 432.
                         // This parameter will be used only when PLLSAI is selected as Clock Source SAI or LTDC 

    uint32_t PLLSAIQ;    // Specifies the division factor for SAI1 clock.
                         // This parameter must be a number between Min_Data = 2 and Max_Data = 15.
                         // This parameter will be used only when PLLSAI is selected as Clock Source SAI or LTDC 

    uint32_t PLLSAIR;    // specifies the division factor for LTDC clock
                         // This parameter must be a number between Min_Data = 2 and Max_Data = 7.
                         // This parameter will be used only when PLLSAI is selected as Clock Source LTDC 

    uint32_t PLLSAIP;    // Specifies the division factor for 48MHz clock.
                         // This parameter must be a value of @ref RCCEx_PLLSAIP_Clock_Divider
                         // This parameter will be used only when PLLSAI is disabled 
} RCC_PLLSAIInitTypeDef;
typedef struct
{
    uint32_t PeriphClockSelection; // The Extended Clock to be configured.This parameter can be a value of @ref RCCEx_Periph_Clock_Selection 
    RCC_PLLI2SInitTypeDef PLLI2S;  // PLL I2S structure parameters.This parameter will be used only when PLLI2S is selected as Clock Source I2S or SAI 
    RCC_PLLSAIInitTypeDef PLLSAI;  // PLL SAI structure parameters.This parameter will be used only when PLLI2S is selected as Clock Source SAI or LTDC 
    uint32_t PLLI2SDivQ;           // Specifies the PLLI2S division factor for SAI1 clock.This parameter must be a number between Min_Data = 1 and Max_Data = 32
                                   // This parameter will be used only when PLLI2S is selected as Clock Source SAI 
    uint32_t PLLSAIDivQ;           // Specifies the PLLI2S division factor for SAI1 clock.
                                   // This parameter must be a number between Min_Data = 1 and Max_Data = 32
                                   // This parameter will be used only when PLLSAI is selected as Clock Source SAI 
    uint32_t PLLSAIDivR;           // Specifies the PLLSAI division factor for LTDC clock.
                                   // This parameter must be one value of @ref RCCEx_PLLSAI_DIVR 
    uint32_t RTCClockSelection;    // Specifies RTC Clock source Selection.
                                   // This parameter can be a value of @ref RCC_RTC_Clock_Source 
    uint32_t I2sClockSelection;    // Specifies I2S Clock source Selection.
                                   // This parameter can be a value of @ref RCCEx_I2S_Clock_Source 
    uint32_t TIMPresSelection;     // Specifies TIM Clock Prescalers Selection.
                                   // This parameter can be a value of @ref RCCEx_TIM_Prescaler_Selection 
    uint32_t Sai1ClockSelection;   // Specifies SAI1 Clock Prescalers Selection
                                   // This parameter can be a value of @ref RCCEx_SAI1_Clock_Source 
    uint32_t Sai2ClockSelection;   // Specifies SAI2 Clock Prescalers Selection
                                   // This parameter can be a value of @ref RCCEx_SAI2_Clock_Source 
    uint32_t Usart1ClockSelection; // USART1 clock source
                                   // This parameter can be a value of @ref RCCEx_USART1_Clock_Source 
    uint32_t Usart2ClockSelection; // USART2 clock source
                                   // This parameter can be a value of @ref RCCEx_USART2_Clock_Source 
    uint32_t Usart3ClockSelection; // USART3 clock source
                                   // This parameter can be a value of @ref RCCEx_USART3_Clock_Source 

    uint32_t Uart4ClockSelection;  // UART4 clock source
                                   // This parameter can be a value of @ref RCCEx_UART4_Clock_Source 

    uint32_t Uart5ClockSelection;  // UART5 clock source
                                   // This parameter can be a value of @ref RCCEx_UART5_Clock_Source 

    uint32_t Usart6ClockSelection;  // USART6 clock source
                                   // This parameter can be a value of @ref RCCEx_USART6_Clock_Source 

    uint32_t Uart7ClockSelection;  // UART7 clock source
                                   // This parameter can be a value of @ref RCCEx_UART7_Clock_Source 

    uint32_t Uart8ClockSelection;  // UART8 clock source
                                   // This parameter can be a value of @ref RCCEx_UART8_Clock_Source 

    uint32_t I2c1ClockSelection;   // I2C1 clock source
                                   // This parameter can be a value of @ref RCCEx_I2C1_Clock_Source 

    uint32_t I2c2ClockSelection;   // I2C2 clock source
                                   // This parameter can be a value of @ref RCCEx_I2C2_Clock_Source 

    uint32_t I2c3ClockSelection;   // I2C3 clock source
                                   // This parameter can be a value of @ref RCCEx_I2C3_Clock_Source 

    uint32_t I2c4ClockSelection;   // I2C4 clock source
                                   // This parameter can be a value of @ref RCCEx_I2C4_Clock_Source 

    uint32_t Lptim1ClockSelection;   // Specifies LPTIM1 clock source
                                     // This parameter can be a value of @ref RCCEx_LPTIM1_Clock_Source 

    uint32_t CecClockSelection;      // CEC clock source
                                     // This parameter can be a value of @ref RCCEx_CEC_Clock_Source 

    uint32_t Clk48ClockSelection;    // Specifies 48Mhz clock source used by USB OTG FS, RNG and SDMMC
                                     // This parameter can be a value of @ref RCCEx_CLK48_Clock_Source 

    uint32_t Sdmmc1ClockSelection;     // SDMMC1 clock source
                                     // This parameter can be a value of @ref RCCEx_SDMMC1_Clock_Source 

    uint32_t Sdmmc2ClockSelection;     // SDMMC2 clock source
                                     // This parameter can be a value of @ref RCCEx_SDMMC2_Clock_Source 

    uint32_t Dfsdm1ClockSelection;     // DFSDM1 clock source
                                     // This parameter can be a value of @ref RCCEx_DFSDM1_Kernel_Clock_Source 

    uint32_t Dfsdm1AudioClockSelection; // DFSDM1 clock source
                                     // This parameter can be a value of @ref RCCEx_DFSDM1_AUDIO_Clock_Source 
} RCC_PeriphCLKInitTypeDef;
typedef struct
{
    uint32_t VirtualChannelID;             // Virtual channel ID                                                 
    uint32_t ColorCoding;                  // Color coding for LTDC interface
                                           // This parameter can be any value of @ref DSI_Color_Coding           
    uint32_t LooselyPacked;                // Enable or disable loosely packed stream (needed only when using
                                           // 18-bit configuration).
                                           // This parameter can be any value of @ref DSI_LooselyPacked          
    uint32_t Mode;                         // Video mode type
                                           // This parameter can be any value of @ref DSI_Video_Mode_Type        
    uint32_t PacketSize;                   // Video packet size                                                  
    uint32_t NumberOfChunks;               // Number of chunks                                                   
    uint32_t NullPacketSize;               // Null packet size                                                   
    uint32_t HSPolarity;                   // HSYNC pin polarity
                                           // This parameter can be any value of @ref DSI_HSYNC_Polarity         
    uint32_t VSPolarity;                   // VSYNC pin polarity
                                           // This parameter can be any value of @ref DSI_VSYNC_Polarity         
    uint32_t DEPolarity;                   // Data Enable pin polarity
                                           // This parameter can be any value of @ref DSI_DATA_ENABLE_Polarity   
    uint32_t HorizontalSyncActive;         // Horizontal synchronism active duration (in lane byte clock cycles) 
    uint32_t HorizontalBackPorch;          // Horizontal back-porch duration (in lane byte clock cycles)         
    uint32_t HorizontalLine;               // Horizontal line duration (in lane byte clock cycles)               
    uint32_t VerticalSyncActive;           // Vertical synchronism active duration                               
    uint32_t VerticalBackPorch;            // Vertical back-porch duration                                       
    uint32_t VerticalFrontPorch;           // Vertical front-porch duration                                      
    uint32_t VerticalActive;               // Vertical active duration                                           
    uint32_t LPCommandEnable;              // Low-power command enable
                                           // This parameter can be any value of @ref DSI_LP_Command             
    uint32_t LPLargestPacketSize;          // The size, in bytes, of the low power largest packet that
                                            //    can fit in a line during VSA, VBP and VFP regions                  
    uint32_t LPVACTLargestPacketSize;      // The size, in bytes, of the low power largest packet that
                                           //     can fit in a line during VACT region                               
    uint32_t LPHorizontalFrontPorchEnable; // Low-power horizontal front-porch enable
                                           // This parameter can be any value of @ref DSI_LP_HFP                 
    uint32_t LPHorizontalBackPorchEnable;  // Low-power horizontal back-porch enable
                                           // This parameter can be any value of @ref DSI_LP_HBP                 
    uint32_t LPVerticalActiveEnable;       // Low-power vertical active enable
                                           // This parameter can be any value of @ref DSI_LP_VACT                
    uint32_t LPVerticalFrontPorchEnable;   // Low-power vertical front-porch enable
                                           // This parameter can be any value of @ref DSI_LP_VFP                 
    uint32_t LPVerticalBackPorchEnable;    // Low-power vertical back-porch enable
                                           // This parameter can be any value of @ref DSI_LP_VBP                 
    uint32_t LPVerticalSyncActiveEnable;   // Low-power vertical sync active enable
                                           // This parameter can be any value of @ref DSI_LP_VSYNC               
    uint32_t FrameBTAAcknowledgeEnable;    // Frame bus-turn-around acknowledge enable
                                           // This parameter can be any value of @ref DSI_FBTA_acknowledge       
} DSI_VidCfgTypeDef;
typedef struct
{
    uint32_t AutomaticClockLaneControl; // Automatic clock lane control
                                        // This parameter can be any value of @ref DSI_Automatic_Clk_Lane_Control 
    uint32_t TXEscapeCkdiv;             // TX Escape clock division
                                        //   The values 0 and 1 stop the TX_ESC clock generation                    
    uint32_t NumberOfLanes;             // Number of lanes
                                        // This parameter can be any value of @ref DSI_Number_Of_Lanes            
} DSI_InitTypeDef;
typedef struct
{
    DSI_TypeDef* Instance;    // Register base address      
    DSI_InitTypeDef           Init;         // DSI required parameters    
    HAL_LockTypeDef           Lock;         // DSI peripheral status      
    __IO HAL_DSI_StateTypeDef State;        // DSI communication state    
    __IO uint32_t             ErrorCode;    // DSI Error code             
    uint32_t                  ErrorMsk;     // DSI Error monitoring mask  
} DSI_HandleTypeDef;



//______________________________________________________________________
// Touch
//___________________________________________________________________
#define WRITE_REG(REG, VAL)   ((REG) = (VAL))
#define READ_REG(REG)         ((REG))

#define MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))



typedef enum
{
    TS_OK = 0x00, /*!< Touch Ok */
    TS_ERROR = 0x01, /*!< Touch Error */
    TS_TIMEOUT = 0x02, /*!< Touch Timeout */
    TS_DEVICE_NOT_FOUND = 0x03  /*!< Touchscreen device not found */
} TS_StatusTypeDef;

#define TS_I2C_ADDRESS     ((uint16_t)0x54)
#define TS_I2C_ADDRESS_A02 ((uint16_t)0x70)
#define TS_SWAP_NONE                    ((uint8_t) 0x01)
#define TS_SWAP_Y                       ((uint8_t) 0x04)
#define TS_SWAP_XY                      ((uint8_t) 0x08)

#define FT6206_GMODE_REG                0xA4
#define FT6206_G_MODE_INTERRUPT_POLLING 0x00
#define FT6206_P1_XH_REG                0x03
#define FT6206_P2_XH_REG               0x09
#define FT6206_MSB_MASK                 0x0F
#define FT6206_LSB_MASK                 0xFF
#define FT6206_G_MODE_INTERRUPT_TRIGGER 0x01
#define FT6206_G_MODE_INTERRUPT_MASK    0x03
#define FT6206_G_MODE_INTERRUPT_SHIFT   0x00
#define FT6206_I2C_NOT_INITIALIZED      0
#define FT6206_MAX_DETECTABLE_TOUCH     2
#define FT6206_TD_STAT_MASK             0x0F
#define FT6206_TD_STAT_REG              0x02
#define FT6206_ID_VALUE                 0x11
#define FT6206_STATUS_OK                0
#define FT6206_CHIP_ID_REG              0xA8
#define FT6x06_MAX_INSTANCE  2
typedef struct
{
    uint8_t i2cInitialized;

    /* field holding the current number of simultaneous active touches */
    uint8_t currActiveTouchNb;

    /* field holding the touch index currently managed */
    uint8_t currActiveTouchIdx;

} ft6x06_handle_TypeDef;

void ft6x06_TS_GetXY(uint16_t DeviceAddr, uint16_t* X, uint16_t* Y);
void ft6x06_Reset(uint16_t DeviceAddr);
void ft6x06_TS_Start(uint16_t DeviceAddr);
void ft6x06_TS_ClearIT(uint16_t DeviceAddr);
void ft6x06_TS_DisableIT(uint16_t DeviceAddr);
void ft6x06_TS_EnableIT(uint16_t DeviceAddr);
void ft6x06_Init(uint16_t DeviceAddr);
uint8_t ft6x06_TS_DetectTouch(uint16_t DeviceAddr);
uint8_t ft6x06_TS_ITStatus(uint16_t DeviceAddr);
uint8_t ft6x06_GetInstance(uint16_t DeviceAddr);
uint16_t ft6x06_ReadID(uint16_t DeviceAddr);

typedef struct
{
    void       (*Init)(uint16_t);
    uint16_t(*ReadID)(uint16_t);
    void       (*Reset)(uint16_t);
    void       (*Start)(uint16_t);
    uint8_t(*DetectTouch)(uint16_t);
    void       (*GetXY)(uint16_t, uint16_t*, uint16_t*);
    void       (*EnableIT)(uint16_t);
    void       (*ClearIT)(uint16_t);
    uint8_t(*GetITStatus)(uint16_t);
    void       (*DisableIT)(uint16_t);
} TS_DrvTypeDef;


TS_DrvTypeDef ft6x06_ts_drv =
{
  ft6x06_Init,
  ft6x06_ReadID,
  ft6x06_Reset,
  ft6x06_TS_Start,
  ft6x06_TS_DetectTouch,
  ft6x06_TS_GetXY,
  ft6x06_TS_EnableIT,
  ft6x06_TS_ClearIT,
  ft6x06_TS_ITStatus,
  ft6x06_TS_DisableIT
};


typedef enum
{
    HAL_I2C_STATE_RESET = 0x00U,   /*!< Peripheral is not yet Initialized         */
    HAL_I2C_STATE_READY = 0x20U,   /*!< Peripheral Initialized and ready for use  */
    HAL_I2C_STATE_BUSY = 0x24U,   /*!< An internal process is ongoing            */
    HAL_I2C_STATE_BUSY_TX = 0x21U,   /*!< Data Transmission process is ongoing      */
    HAL_I2C_STATE_BUSY_RX = 0x22U,   /*!< Data Reception process is ongoing         */
    HAL_I2C_STATE_LISTEN = 0x28U,   /*!< Address Listen Mode is ongoing            */
    HAL_I2C_STATE_BUSY_TX_LISTEN = 0x29U,   /*!< Address Listen Mode and Data Transmission
                                                   process is ongoing                         */
                                                   HAL_I2C_STATE_BUSY_RX_LISTEN = 0x2AU,   /*!< Address Listen Mode and Data Reception
                                                                                                  process is ongoing                         */
                                                                                                  HAL_I2C_STATE_ABORT = 0x60U,   /*!< Abort user request ongoing                */
                                                                                                  HAL_I2C_STATE_TIMEOUT = 0xA0U,   /*!< Timeout state                             */
                                                                                                  HAL_I2C_STATE_ERROR = 0xE0U    /*!< Error                                     */

} HAL_I2C_StateTypeDef;

#define HAL_I2C_ERROR_INVALID_PARAM     (0x00000200U)    /*!< Invalid Parameters error  */
#define HAL_I2C_ERROR_NONE      (0x00000000U)    /*!< No error              */
#define HAL_I2C_ERROR_TIMEOUT   (0x00000020U)    /*!< Timeout error         */

#define HAL_MAX_DELAY      0xFFFFFFFFU
#define MAX_NBYTE_SIZE      255U

#define I2C_MEMADD_SIZE_8BIT            (0x00000001U)
#define I2C_MEMADD_SIZE_16BIT           (0x00000002U)
#define I2C_FLAG_BUSY                       I2C_ISR_BUSY
#define I2C_FLAG_RXNE                       I2C_ISR_RXNE
#define I2C_FLAG_TCR                        I2C_ISR_TCR
#define I2C_FLAG_STOPF                      I2C_ISR_STOPF
#define I2C_NO_STARTSTOP                    (0x00000000U)
#define I2C_GENERATE_STOP                   (uint32_t)(0x80000000U | I2C_CR2_STOP)
#define I2C_GENERATE_START_READ             (uint32_t)(0x80000000U | I2C_CR2_START | I2C_CR2_RD_WRN)
#define I2C_GENERATE_START_WRITE            (uint32_t)(0x80000000U | I2C_CR2_START)
#define I2C_TIMEOUT_BUSY    (25U)           /*!< 25 ms */
#define IS_I2C_MEMADD_SIZE(SIZE)            (((SIZE) == I2C_MEMADD_SIZE_8BIT) || ((SIZE) == I2C_MEMADD_SIZE_16BIT))
                                       
#define I2C_RELOAD_MODE                    I2C_CR2_RELOAD
#define I2C_AUTOEND_MODE                   I2C_CR2_AUTOEND
#define I2C_SOFTEND_MODE                   (0x00000000U)

#define IS_TRANSFER_MODE(MODE)              (((MODE) == I2C_RELOAD_MODE)            || \
                                            ((MODE) == I2C_AUTOEND_MODE)            || \
                                            ((MODE) == I2C_SOFTEND_MODE))
#define IS_TRANSFER_REQUEST(REQUEST)        (((REQUEST) == I2C_GENERATE_STOP)       || \
                                            ((REQUEST) == I2C_GENERATE_START_READ)  || \
                                            ((REQUEST) == I2C_GENERATE_START_WRITE) || \
                                            ((REQUEST) == I2C_NO_STARTSTOP))
#define I2C_FLAG_TXIS                   I2C_ISR_TXIS
#define I2C_FLAG_TC                     I2C_ISR_TC
#define __HAL_RCC_GPIOE_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOEEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOEEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)
#define __HAL_RCC_GPIOD_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIODEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIODEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)
#define __HAL_RCC_FMC_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB3ENR, RCC_AHB3ENR_FMCEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB3ENR, RCC_AHB3ENR_FMCEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)
#define __HAL_RCC_GPIOJ_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOJEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOJEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)
#define __HAL_RCC_GPIOB_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define DISCOVERY_AUDIO_I2Cx                             I2C4
#define DISCOVERY_EXT_I2Cx                             I2C1
#define DISCOVERY_I2Cx_TIMING                      ((uint32_t)0x40912732)
#define DISCOVERY_AUDIO_I2Cx_SCL_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOD_CLK_ENABLE()
#define DISCOVERY_AUDIO_I2Cx_SDA_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define DISCOVERY_AUDIO_I2Cx_SDA_GPIO_PORT               GPIOB
#define DISCOVERY_AUDIO_I2Cx_SCL_GPIO_PORT               GPIOD
#define DISCOVERY_EXT_I2Cx_SCL_SDA_GPIO_PORT           GPIOB
#define __HAL_RCC_PLLI2S_ENABLE() (RCC->CR |= (RCC_CR_PLLI2SON))
#define __HAL_RCC_PLLI2S_DISABLE() (RCC->CR &= ~(RCC_CR_PLLI2SON))

#define I2C_ADDRESSINGMODE_7BIT         (0x00000001U)
#define I2C_ADDRESSINGMODE_10BIT        (0x00000002U)
#define I2C_DUALADDRESS_DISABLE         (0x00000000U)
#define I2C_DUALADDRESS_ENABLE          I2C_OAR2_OA2EN
#define I2C_GENERALCALL_DISABLE         (0x00000000U)
#define I2C_NOSTRETCH_DISABLE           (0x00000000U)
#define I2C_STATE_NONE                  ((uint32_t)(HAL_I2C_MODE_NONE))                                                        /*!< Default Value                                          */

#define __HAL_I2C_DISABLE(__HANDLE__)                           (CLEAR_BIT((__HANDLE__)->Instance->CR1, I2C_CR1_PE))
#define __HAL_I2C_GET_FLAG(__HANDLE__, __FLAG__) (((((__HANDLE__)->Instance->ISR) & (__FLAG__)) == (__FLAG__)) ? SET : RESET)
#define __HAL_I2C_CLEAR_FLAG(__HANDLE__, __FLAG__) (((__FLAG__) == I2C_FLAG_TXE) ? ((__HANDLE__)->Instance->ISR |= (__FLAG__)) \
                                                                                 : ((__HANDLE__)->Instance->ICR = (__FLAG__)))
#define __HAL_I2C_ENABLE(__HANDLE__)                            (SET_BIT((__HANDLE__)->Instance->CR1,  I2C_CR1_PE))

#define HAL_I2C_ERROR_AF        (0x00000004U)    /*!< ACKF error            */
#define TIMING_CLEAR_MASK   (0xF0FFFFFFU)  /*!< I2C TIMING clear register Mask */
#define I2C_RESET_CR2(__HANDLE__)                 ((__HANDLE__)->Instance->CR2 &= (uint32_t)~((uint32_t)(I2C_CR2_SADD | I2C_CR2_HEAD10R | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_RD_WRN)))
#define I2C_MEM_ADD_MSB(__ADDRESS__)        ((uint8_t)((uint16_t)(((uint16_t)((__ADDRESS__) & (uint16_t)(0xFF00U))) >> 8U)))

#define I2C_FLAG_TXE                    I2C_ISR_TXE
#define I2C_FLAG_AF                     I2C_ISR_NACKF
#define I2C_MEM_ADD_LSB(__ADDRESS__)              ((uint8_t)((uint16_t)((__ADDRESS__) & (uint16_t)(0x00FFU))))

#define __HAL_RCC_I2C1_RELEASE_RESET()   (RCC->APB1RSTR &= ~(RCC_APB1RSTR_I2C1RST))
#define __HAL_RCC_I2C1_FORCE_RESET()     (RCC->APB1RSTR |= (RCC_APB1RSTR_I2C1RST))
#define __HAL_RCC_I2C1_CLK_ENABLE()      do { \
                                           __IO uint32_t tmpreg; \
                                           SET_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C1EN);\
                                           /* Delay after an RCC peripheral clock enabling */ \
                                           tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C1EN);\
                                           UNUSED(tmpreg); \
                                         } while(0)
#define __HAL_RCC_I2C4_FORCE_RESET()     (RCC->APB1RSTR |= (RCC_APB1RSTR_I2C4RST))
#define __HAL_RCC_I2C4_RELEASE_RESET()   (RCC->APB1RSTR &= ~(RCC_APB1RSTR_I2C4RST))
#define __HAL_RCC_I2C4_CLK_ENABLE()      do { \
                                           __IO uint32_t tmpreg; \
                                           SET_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C4EN);\
                                           /* Delay after an RCC peripheral clock enabling */ \
                                           tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C4EN);\
                                           UNUSED(tmpreg); \
                                         } while(0)
#define __HAL_RCC_DMA1_CLK_ENABLE()      do { \
                                           __IO uint32_t tmpreg; \
                                           SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DMA1EN);\
                                           /* Delay after an RCC peripheral clock enabling */ \
                                           tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DMA1EN);\
                                           UNUSED(tmpreg); \
                                         } while(0)
#define RCC_I2SCLKSOURCE_PLLI2S          ((uint32_t)0x00000000U)
#define __HAL_RCC_SYSCFG_CLK_ENABLE()    do { \
                                         __IO uint32_t tmpreg; \
                                         SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);\
                                         /* Delay after an RCC peripheral clock enabling */ \
                                         tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);\
                                         UNUSED(tmpreg); \
                                         } while(0)
#define DISCOVERY_EXT_I2Cx_SCL_SDA_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()
#define DISCOVERY_AUDIO_I2Cx_SDA_GPIO_PORT             GPIOB
#define DISCOVERY_AUDIO_I2Cx_SDA_AF                    GPIO_AF11_I2C4
#define DISCOVERY_AUDIO_I2Cx_SDA_PIN                   GPIO_PIN_7 /*!< PB7 */
#define DISCOVERY_AUDIO_I2Cx_SCL_GPIO_PORT             GPIOD
#define DISCOVERY_AUDIO_I2Cx_SCL_AF                    GPIO_AF4_I2C4
#define DISCOVERY_AUDIO_I2Cx_SCL_PIN                   GPIO_PIN_12 /*!< PD12 */
#define DISCOVERY_AUDIO_I2Cx_EV_IRQn                   I2C4_EV_IRQn
#define DISCOVERY_AUDIO_I2Cx_ER_IRQn                   I2C4_ER_IRQn
#define DISCOVERY_AUDIO_I2Cx_FORCE_RESET()             __HAL_RCC_I2C4_FORCE_RESET()
#define DISCOVERY_AUDIO_I2Cx_RELEASE_RESET()           __HAL_RCC_I2C4_RELEASE_RESET()
#define DISCOVERY_AUDIO_I2Cx_CLK_ENABLE()              __HAL_RCC_I2C4_CLK_ENABLE()
#define DISCOVERY_EXT_I2Cx_SCL_SDA_AF                  GPIO_AF4_I2C1
#define DISCOVERY_EXT_I2Cx_SCL_PIN                     GPIO_PIN_8 /*!< PB8 */
#define DISCOVERY_EXT_I2Cx_SDA_PIN                     GPIO_PIN_9 /*!< PB9 */
#define DISCOVERY_EXT_I2Cx_RELEASE_RESET()             __HAL_RCC_I2C1_RELEASE_RESET()
#define DISCOVERY_EXT_I2Cx_FORCE_RESET()               __HAL_RCC_I2C1_FORCE_RESET()
#define DISCOVERY_EXT_I2Cx_EV_IRQn                     I2C1_EV_IRQn
#define DISCOVERY_EXT_I2Cx_ER_IRQn                     I2C1_ER_IRQn
#define DISCOVERY_EXT_I2Cx_CLK_ENABLE()                __HAL_RCC_I2C1_CLK_ENABLE()
#define DISCOVERY_DMAx_CLK_ENABLE()                    __HAL_RCC_DMA1_CLK_ENABLE()

#define RISING_EDGE           ((uint32_t)0x00100000U)
#define FALLING_EDGE          ((uint32_t)0x00200000U)

#define SDRAM_ERROR                     ((uint8_t)0x01)
#define FMC_SDRAM_DEVICE               FMC_Bank5_6
#define FMC_SDRAM_BANK1                       ((uint32_t)0x00000000U)
#define FMC_SDRAM_MEM_BUS_WIDTH_32            ((uint32_t)0x00000020U)
#define FMC_SDRAM_CLOCK_PERIOD_2              ((uint32_t)0x00000800U)
#define FMC_SDRAM_COLUMN_BITS_NUM_8           ((uint32_t)0x00000000U)
#define FMC_SDRAM_ROW_BITS_NUM_12             ((uint32_t)0x00000004U)
#define FMC_SDRAM_INTERN_BANKS_NUM_4          ((uint32_t)0x00000040U)
#define FMC_SDRAM_CAS_LATENCY_3               ((uint32_t)0x00000180)
#define FMC_SDRAM_WRITE_PROTECTION_DISABLE    ((uint32_t)0x00000000U)
#define FMC_SDRAM_RBURST_ENABLE               ((uint32_t)0x00001000U)
#define FMC_SDRAM_RPIPE_DELAY_0               ((uint32_t)0x00000000U)
#define FMC_SDRAM_CMD_CLK_ENABLE              ((uint32_t)0x00000001U)
#define FMC_SDRAM_CMD_TARGET_BANK1            FMC_SDCMR_CTB1
#define FMC_SDRAM_CMD_LOAD_MODE               ((uint32_t)0x00000004U)
#define FMC_SDRAM_CMD_AUTOREFRESH_MODE        ((uint32_t)0x00000003U)
#define FMC_SDRAM_CMD_PALL                    ((uint32_t)0x00000002U)
#define FMC_SDRAM_BANK2                       ((uint32_t)0x00000001U)

#define REFRESH_COUNT                            ((uint32_t)0x0603)   /* SDRAM refresh counter (100Mhz SD clock) */
#define SDRAM_OK                                 ((uint8_t)0x00)
#define SDCLOCK_PERIOD                           FMC_SDRAM_CLOCK_PERIOD_2
#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000) 
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200) 
#define SDRAM_MEMORY_WIDTH                       FMC_SDRAM_MEM_BUS_WIDTH_32

#define __HAL_RCC_GPIOF_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOFEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOFEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)
#define __HAL_RCC_GPIOG_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOGEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOGEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)
#define __HAL_RCC_GPIOH_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOHEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOHEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)
#define __HAL_RCC_GPIOI_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOIEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOIEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)
#define __HAL_RCC_DMA2_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DMA2EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DMA2EN);\
                                        UNUSED(tmpreg); \
                                      } while(0)
#define __DMAx_CLK_ENABLE                 __HAL_RCC_DMA2_CLK_ENABLE

typedef enum
{
    HAL_SDRAM_STATE_RESET = 0x00U,  /*!< SDRAM not yet initialized or disabled */
    HAL_SDRAM_STATE_READY = 0x01U,  /*!< SDRAM initialized and ready for use   */
    HAL_SDRAM_STATE_BUSY = 0x02U,  /*!< SDRAM internal process is ongoing     */
    HAL_SDRAM_STATE_ERROR = 0x03U,  /*!< SDRAM error state                     */
    HAL_SDRAM_STATE_WRITE_PROTECTED = 0x04U,  /*!< SDRAM device write protected          */
    HAL_SDRAM_STATE_PRECHARGED = 0x05U   /*!< SDRAM device precharged               */

} HAL_SDRAM_StateTypeDef;
typedef enum
{
    HAL_I2C_MODE_NONE = 0x00U,   /*!< No I2C communication on going             */
    HAL_I2C_MODE_MASTER = 0x10U,   /*!< I2C communication is in Master Mode       */
    HAL_I2C_MODE_SLAVE = 0x20U,   /*!< I2C communication is in Slave Mode        */
    HAL_I2C_MODE_MEM = 0x40U    /*!< I2C communication is in Memory Mode       */

} HAL_I2C_ModeTypeDef;
typedef enum
{
    HAL_DMA_STATE_RESET = 0x00U,  /*!< DMA not yet initialized or disabled */
    HAL_DMA_STATE_READY = 0x01U,  /*!< DMA initialized and ready for use   */
    HAL_DMA_STATE_BUSY = 0x02U,  /*!< DMA process is ongoing              */
    HAL_DMA_STATE_TIMEOUT = 0x03U,  /*!< DMA timeout state                   */
    HAL_DMA_STATE_ERROR = 0x04U,  /*!< DMA error state                     */
    HAL_DMA_STATE_ABORT = 0x05U,  /*!< DMA Abort state                     */
}HAL_DMA_StateTypeDef;

typedef struct
{
    uint32_t Timing;              /*!< Specifies the I2C_TIMINGR_register value.
                                    This parameter calculated by referring to I2C initialization
                                           section in Reference manual */

    uint32_t OwnAddress1;         /*!< Specifies the first device own address.
                                    This parameter can be a 7-bit or 10-bit address. */

    uint32_t AddressingMode;      /*!< Specifies if 7-bit or 10-bit addressing mode is selected.
                                    This parameter can be a value of @ref I2C_ADDRESSING_MODE */

    uint32_t DualAddressMode;     /*!< Specifies if dual addressing mode is selected.
                                    This parameter can be a value of @ref I2C_DUAL_ADDRESSING_MODE */

    uint32_t OwnAddress2;         /*!< Specifies the second device own address if dual addressing mode is selected
                                    This parameter can be a 7-bit address. */

    uint32_t OwnAddress2Masks;    /*!< Specifies the acknowledge mask address second device own address if dual addressing mode is selected
                                    This parameter can be a value of @ref I2C_OWN_ADDRESS2_MASKS */

    uint32_t GeneralCallMode;     /*!< Specifies if general call mode is selected.
                                    This parameter can be a value of @ref I2C_GENERAL_CALL_ADDRESSING_MODE */

    uint32_t NoStretchMode;       /*!< Specifies if nostretch mode is selected.
                                    This parameter can be a value of @ref I2C_NOSTRETCH_MODE */

} I2C_InitTypeDef;
typedef struct
{
    uint32_t Channel;              /*!< Specifies the channel used for the specified stream.
                                        This parameter can be a value of @ref DMAEx_Channel_selection                  */

    uint32_t Direction;            /*!< Specifies if the data will be transferred from memory to peripheral,
                                        from memory to memory or from peripheral to memory.
                                        This parameter can be a value of @ref DMA_Data_transfer_direction              */

    uint32_t PeriphInc;            /*!< Specifies whether the Peripheral address register should be incremented or not.
                                        This parameter can be a value of @ref DMA_Peripheral_incremented_mode          */

    uint32_t MemInc;               /*!< Specifies whether the memory address register should be incremented or not.
                                        This parameter can be a value of @ref DMA_Memory_incremented_mode              */

    uint32_t PeriphDataAlignment;  /*!< Specifies the Peripheral data width.
                                        This parameter can be a value of @ref DMA_Peripheral_data_size                 */

    uint32_t MemDataAlignment;     /*!< Specifies the Memory data width.
                                        This parameter can be a value of @ref DMA_Memory_data_size                     */

    uint32_t Mode;                 /*!< Specifies the operation mode of the DMAy Streamx.
                                        This parameter can be a value of @ref DMA_mode
                                        @note The circular buffer mode cannot be used if the memory-to-memory
                                              data transfer is configured on the selected Stream                        */

    uint32_t Priority;             /*!< Specifies the software priority for the DMAy Streamx.
                                        This parameter can be a value of @ref DMA_Priority_level                       */

    uint32_t FIFOMode;             /*!< Specifies if the FIFO mode or Direct mode will be used for the specified stream.
                                        This parameter can be a value of @ref DMA_FIFO_direct_mode
                                        @note The Direct mode (FIFO mode disabled) cannot be used if the
                                              memory-to-memory data transfer is configured on the selected stream       */

    uint32_t FIFOThreshold;        /*!< Specifies the FIFO threshold level.
                                        This parameter can be a value of @ref DMA_FIFO_threshold_level                  */

    uint32_t MemBurst;             /*!< Specifies the Burst transfer configuration for the memory transfers.
                                        It specifies the amount of data to be transferred in a single non interruptible
                                        transaction.
                                        This parameter can be a value of @ref DMA_Memory_burst
                                        @note The burst mode is possible only if the address Increment mode is enabled. */

    uint32_t PeriphBurst;          /*!< Specifies the Burst transfer configuration for the peripheral transfers.
                                        It specifies the amount of data to be transferred in a single non interruptible
                                        transaction.
                                        This parameter can be a value of @ref DMA_Peripheral_burst
                                        @note The burst mode is possible only if the address Increment mode is enabled. */
}DMA_InitTypeDef;
typedef struct __DMA_HandleTypeDef
{
    DMA_Stream_TypeDef* Instance;                                                    /*!< Register base address                  */

    DMA_InitTypeDef            Init;                                                         /*!< DMA communication parameters           */

    HAL_LockTypeDef            Lock;                                                         /*!< DMA locking object                     */

    __IO HAL_DMA_StateTypeDef  State;                                                        /*!< DMA transfer state                     */

    void* Parent;                                                      /*!< Parent object state                    */

    void                       (*XferCpltCallback)(struct __DMA_HandleTypeDef* hdma);     /*!< DMA transfer complete callback         */

    void                       (*XferHalfCpltCallback)(struct __DMA_HandleTypeDef* hdma); /*!< DMA Half transfer complete callback    */

    void                       (*XferM1CpltCallback)(struct __DMA_HandleTypeDef* hdma);   /*!< DMA transfer complete Memory1 callback */

    void                       (*XferM1HalfCpltCallback)(struct __DMA_HandleTypeDef* hdma);   /*!< DMA transfer Half complete Memory1 callback */

    void                       (*XferErrorCallback)(struct __DMA_HandleTypeDef* hdma);    /*!< DMA transfer error callback            */

    void                       (*XferAbortCallback)(struct __DMA_HandleTypeDef* hdma);    /*!< DMA transfer Abort callback            */

    __IO uint32_t               ErrorCode;                                                    /*!< DMA Error code                          */

    uint32_t                    StreamBaseAddress;                                            /*!< DMA Stream Base Address                */

    uint32_t                    StreamIndex;                                                  /*!< DMA Stream Index                       */

}DMA_HandleTypeDef;
typedef struct
{
    uint32_t SDBank;                      /*!< Specifies the SDRAM memory device that will be used.
                                               This parameter can be a value of @ref FMC_SDRAM_Bank                */

    uint32_t ColumnBitsNumber;            /*!< Defines the number of bits of column address.
                                               This parameter can be a value of @ref FMC_SDRAM_Column_Bits_number. */

    uint32_t RowBitsNumber;               /*!< Defines the number of bits of column address.
                                               This parameter can be a value of @ref FMC_SDRAM_Row_Bits_number.    */

    uint32_t MemoryDataWidth;             /*!< Defines the memory device width.
                                               This parameter can be a value of @ref FMC_SDRAM_Memory_Bus_Width.   */

    uint32_t InternalBankNumber;          /*!< Defines the number of the device's internal banks.
                                               This parameter can be of @ref FMC_SDRAM_Internal_Banks_Number.      */

    uint32_t CASLatency;                  /*!< Defines the SDRAM CAS latency in number of memory clock cycles.
                                               This parameter can be a value of @ref FMC_SDRAM_CAS_Latency.        */

    uint32_t WriteProtection;             /*!< Enables the SDRAM device to be accessed in write mode.
                                               This parameter can be a value of @ref FMC_SDRAM_Write_Protection.   */

    uint32_t SDClockPeriod;               /*!< Define the SDRAM Clock Period for both SDRAM devices and they allow
                                               to disable the clock before changing frequency.
                                               This parameter can be a value of @ref FMC_SDRAM_Clock_Period.       */

    uint32_t ReadBurst;                   /*!< This bit enable the SDRAM controller to anticipate the next read
                                               commands during the CAS latency and stores data in the Read FIFO.
                                               This parameter can be a value of @ref FMC_SDRAM_Read_Burst.         */

    uint32_t ReadPipeDelay;               /*!< Define the delay in system clock cycles on read data path.
                                               This parameter can be a value of @ref FMC_SDRAM_Read_Pipe_Delay.    */
} FMC_SDRAM_InitTypeDef;

typedef struct __I2C_HandleTypeDef
{
    I2C_TypeDef* Instance;      /*!< I2C registers base address                */

    I2C_InitTypeDef            Init;           /*!< I2C communication parameters              */

    uint8_t* pBuffPtr;      /*!< Pointer to I2C transfer buffer            */

    uint16_t                   XferSize;       /*!< I2C transfer size                         */

    __IO uint16_t              XferCount;      /*!< I2C transfer counter                      */

    __IO uint32_t              XferOptions;    /*!< I2C sequantial transfer options, this parameter can
                                                    be a value of @ref I2C_XFEROPTIONS */

    __IO uint32_t              PreviousState;  /*!< I2C communication Previous state          */

    HAL_StatusTypeDef(*XferISR)(struct __I2C_HandleTypeDef* hi2c, uint32_t ITFlags, uint32_t ITSources);  /*!< I2C transfer IRQ handler function pointer */

    DMA_HandleTypeDef* hdmatx;        /*!< I2C Tx DMA handle parameters              */

    DMA_HandleTypeDef* hdmarx;        /*!< I2C Rx DMA handle parameters              */

    HAL_LockTypeDef            Lock;           /*!< I2C locking object                        */

    __IO HAL_I2C_StateTypeDef  State;          /*!< I2C communication state                   */

    __IO HAL_I2C_ModeTypeDef   Mode;           /*!< I2C communication mode                    */

    __IO uint32_t              ErrorCode;      /*!< I2C Error code                            */

    __IO uint32_t              AddrEventCount; /*!< I2C Address Event counter                 */

#if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
    void (*MasterTxCpltCallback)(struct __I2C_HandleTypeDef* hi2c);           /*!< I2C Master Tx Transfer completed callback */
    void (*MasterRxCpltCallback)(struct __I2C_HandleTypeDef* hi2c);           /*!< I2C Master Rx Transfer completed callback */
    void (*SlaveTxCpltCallback)(struct __I2C_HandleTypeDef* hi2c);            /*!< I2C Slave Tx Transfer completed callback  */
    void (*SlaveRxCpltCallback)(struct __I2C_HandleTypeDef* hi2c);            /*!< I2C Slave Rx Transfer completed callback  */
    void (*ListenCpltCallback)(struct __I2C_HandleTypeDef* hi2c);             /*!< I2C Listen Complete callback              */
    void (*MemTxCpltCallback)(struct __I2C_HandleTypeDef* hi2c);              /*!< I2C Memory Tx Transfer completed callback */
    void (*MemRxCpltCallback)(struct __I2C_HandleTypeDef* hi2c);              /*!< I2C Memory Rx Transfer completed callback */
    void (*ErrorCallback)(struct __I2C_HandleTypeDef* hi2c);                  /*!< I2C Error callback                        */
    void (*AbortCpltCallback)(struct __I2C_HandleTypeDef* hi2c);              /*!< I2C Abort callback                        */

    void (*AddrCallback)(struct __I2C_HandleTypeDef* hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode);  /*!< I2C Slave Address Match callback */

    void (*MspInitCallback)(struct __I2C_HandleTypeDef* hi2c);                /*!< I2C Msp Init callback                     */
    void (*MspDeInitCallback)(struct __I2C_HandleTypeDef* hi2c);              /*!< I2C Msp DeInit callback                   */

#endif  /* USE_HAL_I2C_REGISTER_CALLBACKS */
} I2C_HandleTypeDef;

#define FMC_SDRAM_TypeDef                 FMC_Bank5_6_TypeDef
#define SDRAM_DMAx_CHANNEL                DMA_CHANNEL_0
#define SDRAM_DMAx_STREAM                 DMA2_Stream0  
#define SDRAM_DMAx_IRQn                   DMA2_Stream0_IRQn

#define DMA_FIFOMODE_DISABLE                     0x00000000U       /*!< FIFO mode disable */
#define __HAL_LINKDMA(__HANDLE__, __PPP_DMA_FIELD__, __DMA_HANDLE__)               \
                        do{                                                      \
                              (__HANDLE__)->__PPP_DMA_FIELD__ = &(__DMA_HANDLE__); \
                              (__DMA_HANDLE__).Parent = (__HANDLE__);             \
                          } while(0)

#define HAL_LTDC_ERROR_NONE               0x00000000U   /*!< LTDC No error             */

#define __HAL_LTDC_ENABLE_IT(__HANDLE__, __INTERRUPT__) ((__HANDLE__)->Instance->IER |= (__INTERRUPT__))
#define __HAL_LTDC_ENABLE(__HANDLE__)    ((__HANDLE__)->Instance->GCR |= LTDC_GCR_LTDCEN)
#define HAL_DMA_ERROR_NONE                       0x00000000U    /*!< No error                               */
#define LCD_BL_CTRL_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOK_CLK_ENABLE()
#define __HAL_RCC_GPIOK_CLK_ENABLE()   do { \
                                        __IO uint32_t tmpreg; \
                                        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOKEN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOKEN);\
                                        UNUSED(tmpreg); \
                                      } while(0)

#define __HAL_DMA_DISABLE(__HANDLE__)     ((__HANDLE__)->Instance->CR &=  ~DMA_SxCR_EN)
#define HAL_TIMEOUT_DMA_ABORT    ((uint32_t)5)  /* 5 ms */
#define HAL_DMA_ERROR_TIMEOUT                    0x00000020U    /*!< Timeout error                          */

#define DMA_FIFOMODE_ENABLE                      DMA_SxFCR_DMDIS   /*!< FIFO mode enable  */
#define HAL_DMA_ERROR_PARAM                      0x00000040U    /*!< Parameter error                        */

typedef enum
{
    LCD_ORIENTATION_PORTRAIT = 0x00, /*!< Portrait orientation choice of LCD screen  */
    LCD_ORIENTATION_LANDSCAPE = 0x01, /*!< Landscape orientation choice of LCD screen */
    LCD_ORIENTATION_INVALID = 0x02  /*!< Invalid orientation choice of LCD screen   */
} LCD_OrientationTypeDef;

#define LCD_OK                         0x00
#define LCD_BL_CTRL_GPIO_PORT           GPIOK
#define LCD_DISP_PIN                    GPIO_PIN_12
#define LCD_DISP_GPIO_PORT              GPIOI
#define LCD_BL_CTRL_PIN                 GPIO_PIN_3
#define LCD_COLOR_WHITE                ((uint32_t) 0xFFFFFFFF)
#define LCD_COLOR_BLACK                ((uint32_t) 0xFF000000)
#define LCD_LayerCfgTypeDef            LTDC_LayerCfgTypeDef
#define LCD_DSI_PIXEL_DATA_FMT_RBG565  DSI_RGB565                // DSI packet pixel format chosen is RGB565 : 16 bpp

#define LCD_OTM8009A_ID                (uint32_t) 0 
#define OTM8009A_480X800_HBP          ((uint16_t)120)           // Horizontal back porch      
#define OTM8009A_480X800_HFP          ((uint16_t)120)           // Horizontal front porch     
#define OTM8009A_480X800_VSYNC        ((uint16_t)12)            // Vertical synchronization   
#define OTM8009A_480X800_VBP          ((uint16_t)12)            // Vertical back porch        
#define OTM8009A_480X800_VFP          ((uint16_t)12)            // Vertical front porch       
#define OTM8009A_480X800_HSYNC        ((uint16_t)63)            // Horizontal synchronization: This value is set to limit value mentionned 
#define OTM8009A_FORMAT_RGB888        ((uint32_t)0x00)          /* Pixel format chosen is RGB888 : 24 bpp */
#define OTM8009A_FORMAT_RBG565        ((uint32_t)0x02)          /* Pixel format chosen is RGB565 : 16 bpp */

typedef struct
{
    uint32_t TextColor; /*!< Specifies the color of text */
    uint32_t BackColor; /*!< Specifies the background color below the text */
    //sFONT* pFont;    /*!< Specifies the font used for the text */

} LCD_DrawPropTypeDef;

#define __HAL_DSI_ENABLE(__HANDLE__) do { \
                                          __IO uint32_t tmpreg = 0x00U; \
                                          SET_BIT((__HANDLE__)->Instance->CR, DSI_CR_EN);\
                                          /* Delay after an DSI Host enabling */ \
                                          tmpreg = READ_BIT((__HANDLE__)->Instance->CR, DSI_CR_EN);\
                                          UNUSED(tmpreg); \
                                        } while(0U)
#define __HAL_DSI_WRAPPER_ENABLE(__HANDLE__) do { \
                                                 __IO uint32_t tmpreg = 0x00U; \
                                                 SET_BIT((__HANDLE__)->Instance->WCR, DSI_WCR_DSIEN);\
                                                 /* Delay after an DSI warpper enabling */ \
                                                 tmpreg = READ_BIT((__HANDLE__)->Instance->WCR, DSI_WCR_DSIEN);\
                                                 UNUSED(tmpreg); \
                                                } while(0U)
#define RCC_PLLSAIDIVR_2                ((uint32_t)0x00000000U)
#define RCC_PLLSAIDIVR_4                RCC_DCKCFGR1_PLLSAIDIVR_0
#define RCC_PLLSAIDIVR_8                RCC_DCKCFGR1_PLLSAIDIVR_1
#define RCC_PLLSAIDIVR_16               RCC_DCKCFGR1_PLLSAIDIVR

#define IS_RCC_PLLSAI_DIVR_VALUE(VALUE) (((VALUE) == RCC_PLLSAIDIVR_2) ||\
                                         ((VALUE) == RCC_PLLSAIDIVR_4) ||\
                                         ((VALUE) == RCC_PLLSAIDIVR_8) ||\
                                         ((VALUE) == RCC_PLLSAIDIVR_16))
#define IS_RCC_PLLSAIR_VALUE(VALUE)      ((2 <= (VALUE)) && ((VALUE) <= 7))
#define IS_RCC_PLLSAI_DIVQ_VALUE(VALUE)  ((1 <= (VALUE)) && ((VALUE) <= 32))
#define IS_RCC_PLLSAIQ_VALUE(VALUE)      ((2 <= (VALUE)) && ((VALUE) <= 15))

#define RCC_PLLSAIP_DIV2                  ((uint32_t)0x00000000U)
#define RCC_PLLSAIP_DIV4                  ((uint32_t)0x00000001U)
#define RCC_PLLSAIP_DIV6                  ((uint32_t)0x00000002U)
#define RCC_PLLSAIP_DIV8                  ((uint32_t)0x00000003U)
#define IS_RCC_PLLSAIP_VALUE(VALUE)       (((VALUE) == RCC_PLLSAIP_DIV2) ||\
                                          ((VALUE) == RCC_PLLSAIP_DIV4) ||\
                                          ((VALUE) == RCC_PLLSAIP_DIV6) ||\
                                          ((VALUE) == RCC_PLLSAIP_DIV8))
#define IS_RCC_PLLI2SQ_VALUE(VALUE)       ((2 <= (VALUE)) && ((VALUE) <= 15))
#define IS_RCC_PLLI2SR_VALUE(VALUE)       ((2 <= (VALUE)) && ((VALUE) <= 7))
#define RCC_PLLI2SP_DIV2                  ((uint32_t)0x00000000U)
#define RCC_PLLI2SP_DIV4                  ((uint32_t)0x00000001U)
#define RCC_PLLI2SP_DIV6                  ((uint32_t)0x00000002U)
#define RCC_PLLI2SP_DIV8                  ((uint32_t)0x00000003U)
#define IS_RCC_PLLI2SP_VALUE(VALUE)       (((VALUE) == RCC_PLLI2SP_DIV2) ||\
                                          ((VALUE) == RCC_PLLI2SP_DIV4) ||\
                                          ((VALUE) == RCC_PLLI2SP_DIV6) ||\
                                          ((VALUE) == RCC_PLLI2SP_DIV8))

#define RCC_I2C4CLKSOURCE_PCLK1        ((uint32_t)0x00000000U)
#define RCC_I2C4CLKSOURCE_SYSCLK       RCC_DCKCFGR2_I2C4SEL_0
#define RCC_I2C4CLKSOURCE_HSI          RCC_DCKCFGR2_I2C4SEL_1
#define RCC_I2C3CLKSOURCE_PCLK1        ((uint32_t)0x00000000U)
#define RCC_I2C3CLKSOURCE_SYSCLK       RCC_DCKCFGR2_I2C3SEL_0
#define RCC_I2C3CLKSOURCE_HSI          RCC_DCKCFGR2_I2C3SEL_1
#define RCC_I2C2CLKSOURCE_PCLK1        ((uint32_t)0x00000000U)
#define RCC_I2C2CLKSOURCE_SYSCLK       RCC_DCKCFGR2_I2C2SEL_0
#define RCC_I2C2CLKSOURCE_HSI          RCC_DCKCFGR2_I2C2SEL_1
#define RCC_I2C1CLKSOURCE_PCLK1        ((uint32_t)0x00000000U)
#define RCC_I2C1CLKSOURCE_SYSCLK       RCC_DCKCFGR2_I2C1SEL_0
#define RCC_I2C1CLKSOURCE_HSI          RCC_DCKCFGR2_I2C1SEL_1

#define IS_RCC_PLLI2S_DIVQ_VALUE(VALUE) ((1 <= (VALUE)) && ((VALUE) <= 32))
#define IS_RCC_I2C4CLKSOURCE(SOURCE)   \
               (((SOURCE) == RCC_I2C4CLKSOURCE_PCLK1) || \
                ((SOURCE) == RCC_I2C4CLKSOURCE_SYSCLK)|| \
                ((SOURCE) == RCC_I2C4CLKSOURCE_HSI))
#define IS_RCC_I2C3CLKSOURCE(SOURCE)   \
               (((SOURCE) == RCC_I2C3CLKSOURCE_PCLK1) || \
                ((SOURCE) == RCC_I2C3CLKSOURCE_SYSCLK)|| \
                ((SOURCE) == RCC_I2C3CLKSOURCE_HSI))
#define IS_RCC_I2C2CLKSOURCE(SOURCE)   \
               (((SOURCE) == RCC_I2C2CLKSOURCE_PCLK1) || \
                ((SOURCE) == RCC_I2C2CLKSOURCE_SYSCLK)|| \
                ((SOURCE) == RCC_I2C2CLKSOURCE_HSI))
#define IS_RCC_I2C1CLKSOURCE(SOURCE)   \
               (((SOURCE) == RCC_I2C1CLKSOURCE_PCLK1) || \
                ((SOURCE) == RCC_I2C1CLKSOURCE_SYSCLK)|| \
                ((SOURCE) == RCC_I2C1CLKSOURCE_HSI))

#define RCC_PERIPHCLK_TIM             ((uint32_t)0x00000010U)
#define RCC_PERIPHCLK_RTC             ((uint32_t)0x00000020U)
#define RCC_PERIPHCLK_USART1          ((uint32_t)0x00000040U)
#define RCC_PERIPHCLK_USART2          ((uint32_t)0x00000080U)
#define RCC_PERIPHCLK_USART3          ((uint32_t)0x00000100U)
#define RCC_PERIPHCLK_UART4           ((uint32_t)0x00000200U)
#define RCC_PERIPHCLK_UART5           ((uint32_t)0x00000400U)
#define RCC_PERIPHCLK_USART6          ((uint32_t)0x00000800U)
#define RCC_PERIPHCLK_UART7           ((uint32_t)0x00001000U)
#define RCC_PERIPHCLK_UART8           ((uint32_t)0x00002000U)
#define RCC_PERIPHCLK_LPTIM1          ((uint32_t)0x00040000U)
#define RCC_PERIPHCLK_CEC             ((uint32_t)0x00400000U)
#define RCC_PERIPHCLK_SDMMC1          ((uint32_t)0x00800000U)
#define RCC_PERIPHCLK_SDMMC2          ((uint32_t)0x04000000U)
#define RCC_PERIPHCLK_DFSDM1           ((uint32_t)0x08000000U)
#define RCC_PERIPHCLK_DFSDM1_AUDIO     ((uint32_t)0x10000000U)

#define IS_RCC_PERIPHCLOCK(SELECTION)  \
               ((((SELECTION) & RCC_PERIPHCLK_I2S)         == RCC_PERIPHCLK_I2S)     || \
                (((SELECTION) & RCC_PERIPHCLK_LTDC)        == RCC_PERIPHCLK_LTDC)    || \
                (((SELECTION) & RCC_PERIPHCLK_TIM)         == RCC_PERIPHCLK_TIM)     || \
                (((SELECTION) & RCC_PERIPHCLK_USART1)      == RCC_PERIPHCLK_USART1)  || \
                (((SELECTION) & RCC_PERIPHCLK_USART2)      == RCC_PERIPHCLK_USART2)  || \
                (((SELECTION) & RCC_PERIPHCLK_USART3)      == RCC_PERIPHCLK_USART3)  || \
                (((SELECTION) & RCC_PERIPHCLK_UART4)       == RCC_PERIPHCLK_UART4)   || \
                (((SELECTION) & RCC_PERIPHCLK_UART5)       == RCC_PERIPHCLK_UART5)   || \
                (((SELECTION) & RCC_PERIPHCLK_USART6)      == RCC_PERIPHCLK_USART6)  || \
                (((SELECTION) & RCC_PERIPHCLK_UART7)       == RCC_PERIPHCLK_UART7)   || \
                (((SELECTION) & RCC_PERIPHCLK_UART8)       == RCC_PERIPHCLK_UART8)   || \
                (((SELECTION) & RCC_PERIPHCLK_I2C1)        == RCC_PERIPHCLK_I2C1)    || \
                (((SELECTION) & RCC_PERIPHCLK_I2C2)        == RCC_PERIPHCLK_I2C2)    || \
                (((SELECTION) & RCC_PERIPHCLK_I2C3)        == RCC_PERIPHCLK_I2C3)    || \
                (((SELECTION) & RCC_PERIPHCLK_I2C4)        == RCC_PERIPHCLK_I2C4)    || \
                (((SELECTION) & RCC_PERIPHCLK_LPTIM1)      == RCC_PERIPHCLK_LPTIM1)  || \
                (((SELECTION) & RCC_PERIPHCLK_SAI1)        == RCC_PERIPHCLK_SAI1)    || \
                (((SELECTION) & RCC_PERIPHCLK_SAI2)        == RCC_PERIPHCLK_SAI2)    || \
                (((SELECTION) & RCC_PERIPHCLK_CLK48)       == RCC_PERIPHCLK_CLK48)   || \
                (((SELECTION) & RCC_PERIPHCLK_CEC)         == RCC_PERIPHCLK_CEC)     || \
                (((SELECTION) & RCC_PERIPHCLK_SDMMC1)      == RCC_PERIPHCLK_SDMMC1)  || \
                (((SELECTION) & RCC_PERIPHCLK_SDMMC2)      == RCC_PERIPHCLK_SDMMC2)  || \
                (((SELECTION) & RCC_PERIPHCLK_DFSDM1)       == RCC_PERIPHCLK_DFSDM1)   || \
                (((SELECTION) & RCC_PERIPHCLK_DFSDM1_AUDIO) == RCC_PERIPHCLK_DFSDM1_AUDIO) || \
                (((SELECTION) & RCC_PERIPHCLK_SPDIFRX)     == RCC_PERIPHCLK_SPDIFRX) || \
                (((SELECTION) & RCC_PERIPHCLK_RTC)         == RCC_PERIPHCLK_RTC))


#define RCC_PERIPHCLK_TIM             ((uint32_t)0x00000010U)
#define RCC_PERIPHCLK_RTC             ((uint32_t)0x00000020U)
#define RCC_PERIPHCLK_USART1          ((uint32_t)0x00000040U)
#define RCC_PERIPHCLK_USART2          ((uint32_t)0x00000080U)
#define RCC_PERIPHCLK_USART3          ((uint32_t)0x00000100U)
#define RCC_PERIPHCLK_UART4           ((uint32_t)0x00000200U)
#define RCC_PERIPHCLK_UART5           ((uint32_t)0x00000400U)
#define RCC_PERIPHCLK_USART6          ((uint32_t)0x00000800U)
#define RCC_PERIPHCLK_UART7           ((uint32_t)0x00001000U)
#define RCC_PERIPHCLK_UART8           ((uint32_t)0x00002000U)
#define RCC_PERIPHCLK_I2C1            ((uint32_t)0x00004000U)
#define RCC_PERIPHCLK_I2C2            ((uint32_t)0x00008000U)
#define RCC_PERIPHCLK_I2C3            ((uint32_t)0x00010000U)
#define RCC_PERIPHCLK_I2C4            ((uint32_t)0x00020000U)
#define RCC_PERIPHCLK_LPTIM1          ((uint32_t)0x00040000U)
#define RCC_PERIPHCLK_SAI1            ((uint32_t)0x00080000U)
#define RCC_PERIPHCLK_SAI2            ((uint32_t)0x00100000U)
#define RCC_PERIPHCLK_CLK48           ((uint32_t)0x00200000U)
#define RCC_PERIPHCLK_CEC             ((uint32_t)0x00400000U)
#define RCC_PERIPHCLK_SDMMC1          ((uint32_t)0x00800000U)
#define RCC_PERIPHCLK_SPDIFRX         ((uint32_t)0x01000000U)
#define RCC_PERIPHCLK_PLLI2S          ((uint32_t)0x02000000U)


#define DMA_CHANNEL_0                     0x00000000U  /*!< DMA Channel 0 */
#define DMA_CHANNEL_1                     0x02000000U  /*!< DMA Channel 1 */
#define DMA_CHANNEL_2                     0x04000000U  /*!< DMA Channel 2 */
#define DMA_CHANNEL_3                     0x06000000U  /*!< DMA Channel 3 */
#define DMA_CHANNEL_4                     0x08000000U  /*!< DMA Channel 4 */
#define DMA_CHANNEL_5                     0x0A000000U  /*!< DMA Channel 5 */
#define DMA_CHANNEL_6                     0x0C000000U  /*!< DMA Channel 6 */
#define DMA_CHANNEL_7                     0x0E000000U  /*!< DMA Channel 7 */
#define DMA_CHANNEL_8                     0x10000000U  /*!< DMA Channel 8 */
#define DMA_CHANNEL_9                     0x12000000U  /*!< DMA Channel 9 */
#define DMA_CHANNEL_10                    0x14000000U  /*!< DMA Channel 10*/
#define DMA_CHANNEL_11                    0x16000000U  /*!< DMA Channel 11*/
#define DMA_CHANNEL_12                    0x18000000U  /*!< DMA Channel 12*/
#define DMA_CHANNEL_13                    0x1A000000U  /*!< DMA Channel 13*/
#define DMA_CHANNEL_14                    0x1C000000U  /*!< DMA Channel 14*/
#define DMA_CHANNEL_15                    0x1E000000U  /*!< DMA Channel 15*/
#define IS_DMA_CHANNEL(CHANNEL) (((CHANNEL) == DMA_CHANNEL_0)  || \
                                 ((CHANNEL) == DMA_CHANNEL_1)  || \
                                 ((CHANNEL) == DMA_CHANNEL_2)  || \
                                 ((CHANNEL) == DMA_CHANNEL_3)  || \
                                 ((CHANNEL) == DMA_CHANNEL_4)  || \
                                 ((CHANNEL) == DMA_CHANNEL_5)  || \
                                 ((CHANNEL) == DMA_CHANNEL_6)  || \
                                 ((CHANNEL) == DMA_CHANNEL_7)  || \
                                 ((CHANNEL) == DMA_CHANNEL_8)  || \
                                 ((CHANNEL) == DMA_CHANNEL_9)  || \
                                 ((CHANNEL) == DMA_CHANNEL_10) || \
                                 ((CHANNEL) == DMA_CHANNEL_11) || \
                                 ((CHANNEL) == DMA_CHANNEL_12) || \
                                 ((CHANNEL) == DMA_CHANNEL_13) || \
                                 ((CHANNEL) == DMA_CHANNEL_14) || \
                                 ((CHANNEL) == DMA_CHANNEL_15)) 
#define DMA_PERIPH_TO_MEMORY                     0x00000000U      /*!< Peripheral to memory direction */
#define DMA_MEMORY_TO_PERIPH                     DMA_SxCR_DIR_0   /*!< Memory to peripheral direction */
#define DMA_MEMORY_TO_MEMORY                     DMA_SxCR_DIR_1   /*!< Memory to memory direction     */
#define IS_DMA_DIRECTION(DIRECTION) (((DIRECTION) == DMA_PERIPH_TO_MEMORY ) || \
                                     ((DIRECTION) == DMA_MEMORY_TO_PERIPH)  || \
                                     ((DIRECTION) == DMA_MEMORY_TO_MEMORY)) 
#define DMA_PINC_ENABLE                          DMA_SxCR_PINC    /*!< Peripheral increment mode enable  */
#define DMA_PINC_DISABLE                         0x00000000U      /*!< Peripheral increment mode disable */
#define IS_DMA_PERIPHERAL_INC_STATE(STATE) (((STATE) == DMA_PINC_ENABLE) || \
                                            ((STATE) == DMA_PINC_DISABLE))
#define DMA_MINC_ENABLE                          DMA_SxCR_MINC    /*!< Memory increment mode enable  */
#define DMA_MINC_DISABLE                         0x00000000U      /*!< Memory increment mode disable */
#define IS_DMA_MEMORY_INC_STATE(STATE) (((STATE) == DMA_MINC_ENABLE)  || \
                                        ((STATE) == DMA_MINC_DISABLE))
#define DMA_PDATAALIGN_BYTE                      0x00000000U        /*!< Peripheral data alignment: Byte     */
#define DMA_PDATAALIGN_HALFWORD                  DMA_SxCR_PSIZE_0   /*!< Peripheral data alignment: HalfWord */
#define DMA_PDATAALIGN_WORD                      DMA_SxCR_PSIZE_1   /*!< Peripheral data alignment: Word     */
#define IS_DMA_PERIPHERAL_DATA_SIZE(SIZE) (((SIZE) == DMA_PDATAALIGN_BYTE)     || \
                                           ((SIZE) == DMA_PDATAALIGN_HALFWORD) || \
                                           ((SIZE) == DMA_PDATAALIGN_WORD))
#define DMA_MDATAALIGN_BYTE                      0x00000000U        /*!< Memory data alignment: Byte     */
#define DMA_MDATAALIGN_HALFWORD                  DMA_SxCR_MSIZE_0   /*!< Memory data alignment: HalfWord */
#define DMA_MDATAALIGN_WORD                      DMA_SxCR_MSIZE_1   /*!< Memory data alignment: Word     */
#define IS_DMA_MEMORY_DATA_SIZE(SIZE) (((SIZE) == DMA_MDATAALIGN_BYTE)     || \
                                       ((SIZE) == DMA_MDATAALIGN_HALFWORD) || \
                                       ((SIZE) == DMA_MDATAALIGN_WORD ))
#define DMA_NORMAL                               0x00000000U       /*!< Normal mode                  */
#define DMA_CIRCULAR                             DMA_SxCR_CIRC     /*!< Circular mode                */
#define DMA_PFCTRL                               DMA_SxCR_PFCTRL   /*!< Peripheral flow control mode */
#define IS_DMA_MODE(MODE) (((MODE) == DMA_NORMAL )  || \
                           ((MODE) == DMA_CIRCULAR) || \
                           ((MODE) == DMA_PFCTRL)) 
#define DMA_PRIORITY_LOW                         0x00000000U    /*!< Priority level: Low       */
#define DMA_PRIORITY_MEDIUM                      DMA_SxCR_PL_0  /*!< Priority level: Medium    */
#define DMA_PRIORITY_HIGH                        DMA_SxCR_PL_1  /*!< Priority level: High      */
#define DMA_PRIORITY_VERY_HIGH                   DMA_SxCR_PL    /*!< Priority level: Very High */
#define IS_DMA_PRIORITY(PRIORITY) (((PRIORITY) == DMA_PRIORITY_LOW )   || \
                                   ((PRIORITY) == DMA_PRIORITY_MEDIUM) || \
                                   ((PRIORITY) == DMA_PRIORITY_HIGH)   || \
                                   ((PRIORITY) == DMA_PRIORITY_VERY_HIGH)) 
#define IS_DMA_FIFO_MODE_STATE(STATE) (((STATE) == DMA_FIFOMODE_DISABLE ) || \
                                       ((STATE) == DMA_FIFOMODE_ENABLE))
#define DMA_FIFO_THRESHOLD_1QUARTERFULL          0x00000000U       /*!< FIFO threshold 1 quart full configuration  */
#define DMA_FIFO_THRESHOLD_HALFFULL              DMA_SxFCR_FTH_0   /*!< FIFO threshold half full configuration     */
#define DMA_FIFO_THRESHOLD_3QUARTERSFULL         DMA_SxFCR_FTH_1   /*!< FIFO threshold 3 quarts full configuration */
#define DMA_FIFO_THRESHOLD_FULL                  DMA_SxFCR_FTH     /*!< FIFO threshold full configuration          */
#define IS_DMA_FIFO_THRESHOLD(THRESHOLD) (((THRESHOLD) == DMA_FIFO_THRESHOLD_1QUARTERFULL ) || \
                                          ((THRESHOLD) == DMA_FIFO_THRESHOLD_HALFFULL)      || \
                                          ((THRESHOLD) == DMA_FIFO_THRESHOLD_3QUARTERSFULL) || \
                                          ((THRESHOLD) == DMA_FIFO_THRESHOLD_FULL))
#define DMA_MBURST_SINGLE                        0x00000000U
#define DMA_MBURST_INC4                          DMA_SxCR_MBURST_0
#define DMA_MBURST_INC8                          DMA_SxCR_MBURST_1
#define DMA_MBURST_INC16                         DMA_SxCR_MBURST
#define IS_DMA_MEMORY_BURST(BURST) (((BURST) == DMA_MBURST_SINGLE) || \
                                    ((BURST) == DMA_MBURST_INC4)   || \
                                    ((BURST) == DMA_MBURST_INC8)   || \
                                    ((BURST) == DMA_MBURST_INC16))
#define DMA_PBURST_SINGLE                        0x00000000U
#define DMA_PBURST_INC4                          DMA_SxCR_PBURST_0
#define DMA_PBURST_INC8                          DMA_SxCR_PBURST_1
#define DMA_PBURST_INC16                         DMA_SxCR_PBURST
#define IS_DMA_PERIPHERAL_BURST(BURST) (((BURST) == DMA_PBURST_SINGLE) || \
                                        ((BURST) == DMA_PBURST_INC4)   || \
                                        ((BURST) == DMA_PBURST_INC8)   || \
                                        ((BURST) == DMA_PBURST_INC16))

#define DSI_VID_MODE_NB_PULSES    0U
#define DSI_VID_MODE_NB_EVENTS    1U
#define DSI_VID_MODE_BURST        2U
#define IS_DSI_VIDEO_MODE_TYPE(VideoModeType)       (((VideoModeType) == DSI_VID_MODE_NB_PULSES) || \
                                                     ((VideoModeType) == DSI_VID_MODE_NB_EVENTS) || \
                                                     ((VideoModeType) == DSI_VID_MODE_BURST))
#define DSI_LP_COMMAND_DISABLE    0x00000000U
#define DSI_LP_COMMAND_ENABLE     DSI_VMCR_LPCE
#define IS_DSI_LP_COMMAND(LPCommand)                (((LPCommand) == DSI_LP_COMMAND_DISABLE) || ((LPCommand) == DSI_LP_COMMAND_ENABLE))

#define DSI_DATA_ENABLE_ACTIVE_HIGH 0x00000000U
#define DSI_DATA_ENABLE_ACTIVE_LOW  DSI_LPCR_DEP
#define DSI_HSYNC_ACTIVE_HIGH       0x00000000U
#define DSI_HSYNC_ACTIVE_LOW        DSI_LPCR_HSP
#define DSI_VSYNC_ACTIVE_HIGH       0x00000000U
#define DSI_VSYNC_ACTIVE_LOW        DSI_LPCR_VSP
#define DSI_LP_HFP_DISABLE        0x00000000U
#define DSI_LP_HFP_ENABLE         DSI_VMCR_LPHFPE
#define DSI_LP_HBP_DISABLE        0x00000000U
#define DSI_LP_HBP_ENABLE         DSI_VMCR_LPHBPE
#define DSI_LP_VACT_DISABLE       0x00000000U
#define DSI_LP_VACT_ENABLE        DSI_VMCR_LPVAE
#define DSI_LP_VFP_DISABLE       0x00000000U
#define DSI_LP_VFP_ENABLE        DSI_VMCR_LPVFPE
#define DSI_LP_VBP_DISABLE       0x00000000U
#define DSI_LP_VBP_ENABLE        DSI_VMCR_LPVBPE
#define DSI_LP_VSYNC_DISABLE     0x00000000U
#define DSI_LP_VSYNC_ENABLE      DSI_VMCR_LPVSAE
#define DSI_FBTAA_DISABLE        0x00000000U
#define DSI_FBTAA_ENABLE         DSI_VMCR_FBTAAE


#define IS_DSI_LP_HFP(LPHFP)                        (((LPHFP) == DSI_LP_HFP_DISABLE) || ((LPHFP) == DSI_LP_HFP_ENABLE))
#define IS_DSI_LP_HBP(LPHBP)                        (((LPHBP) == DSI_LP_HBP_DISABLE) || ((LPHBP) == DSI_LP_HBP_ENABLE))
#define IS_DSI_LP_VACTIVE(LPVActive)                (((LPVActive) == DSI_LP_VACT_DISABLE) || ((LPVActive) == DSI_LP_VACT_ENABLE))
#define IS_DSI_LP_VFP(LPVFP)                        (((LPVFP) == DSI_LP_VFP_DISABLE) || ((LPVFP) == DSI_LP_VFP_ENABLE))
#define IS_DSI_LP_VBP(LPVBP)                        (((LPVBP) == DSI_LP_VBP_DISABLE) || ((LPVBP) == DSI_LP_VBP_ENABLE))
#define IS_DSI_LP_VSYNC(LPVSYNC)                    (((LPVSYNC) == DSI_LP_VSYNC_DISABLE) || ((LPVSYNC) == DSI_LP_VSYNC_ENABLE))
#define IS_DSI_FBTAA(FrameBTAAcknowledge)           (((FrameBTAAcknowledge) == DSI_FBTAA_DISABLE) || ((FrameBTAAcknowledge) == DSI_FBTAA_ENABLE))
#define IS_DSI_DE_POLARITY(DataEnable)              (((DataEnable) == DSI_DATA_ENABLE_ACTIVE_HIGH) || ((DataEnable) == DSI_DATA_ENABLE_ACTIVE_LOW))
#define IS_DSI_VSYNC_POLARITY(VSYNC)                (((VSYNC) == DSI_VSYNC_ACTIVE_HIGH) || ((VSYNC) == DSI_VSYNC_ACTIVE_LOW))
#define IS_DSI_HSYNC_POLARITY(HSYNC)                (((HSYNC) == DSI_HSYNC_ACTIVE_HIGH) || ((HSYNC) == DSI_HSYNC_ACTIVE_LOW))


#define LTDC_HSPOLARITY_AL                0x00000000U   /*!< Horizontal Synchronization is active low. */
#define LTDC_HSPOLARITY_AH                LTDC_GCR_HSPOL            /*!< Horizontal Synchronization is active high. */
#define LTDC_HORIZONTALSYNC               (LTDC_SSCR_HSW >> 16U)    /*!< Horizontal synchronization width. */
#define LTDC_VERTICALSYNC                 LTDC_SSCR_VSH             /*!< Vertical synchronization height. */
#define LTDC_PCPOLARITY_IIPC              LTDC_GCR_PCPOL            /*!< inverted input pixel clock. */

#define IS_LTDC_HSYNC(__HSYNC__)                       ((__HSYNC__)  <= LTDC_HORIZONTALSYNC)
#define IS_LTDC_VSYNC(__VSYNC__)                       ((__VSYNC__)  <= LTDC_VERTICALSYNC)
#define IS_LTDC_AHBP(__AHBP__)                         ((__AHBP__)   <= LTDC_HORIZONTALSYNC)
#define IS_LTDC_AVBP(__AVBP__)                         ((__AVBP__)   <= LTDC_VERTICALSYNC)
#define IS_LTDC_AAW(__AAW__)                           ((__AAW__)    <= LTDC_HORIZONTALSYNC)
#define IS_LTDC_AAH(__AAH__)                           ((__AAH__)    <= LTDC_VERTICALSYNC)
#define IS_LTDC_TOTALW(__TOTALW__)                     ((__TOTALW__) <= LTDC_HORIZONTALSYNC)
#define IS_LTDC_TOTALH(__TOTALH__)                     ((__TOTALH__) <= LTDC_VERTICALSYNC)
#define IS_LTDC_HSPOL(__HSPOL__)                       (((__HSPOL__) == LTDC_HSPOLARITY_AL) || ((__HSPOL__) == LTDC_HSPOLARITY_AH))
#define IS_LTDC_VSPOL(__VSPOL__)                       (((__VSPOL__) == LTDC_VSPOLARITY_AL) || ((__VSPOL__) == LTDC_VSPOLARITY_AH))
#define IS_LTDC_DEPOL(__DEPOL__)                       (((__DEPOL__) == LTDC_DEPOLARITY_AL) || ((__DEPOL__) ==  LTDC_DEPOLARITY_AH))
#define IS_LTDC_PCPOL(__PCPOL__)                       (((__PCPOL__) == LTDC_PCPOLARITY_IPC) || ((__PCPOL__) ==  LTDC_PCPOLARITY_IIPC))

#define IS_I2C_OWN_ADDRESS1(ADDRESS1)             ((ADDRESS1) <= 0x000003FFU)
#define IS_I2C_OWN_ADDRESS2(ADDRESS2)             ((ADDRESS2) <= (uint16_t)0x00FFU)
#define IS_I2C_ADDRESSING_MODE(MODE)    (((MODE) == I2C_ADDRESSINGMODE_7BIT)    || ((MODE) == I2C_ADDRESSINGMODE_10BIT))
#define IS_I2C_DUAL_ADDRESS(ADDRESS)    (((ADDRESS) == I2C_DUALADDRESS_DISABLE) || ((ADDRESS) == I2C_DUALADDRESS_ENABLE))
#define I2C_OA2_NOMASK                  ((uint8_t)0x00U)
#define I2C_OA2_MASK01                  ((uint8_t)0x01U)
#define I2C_OA2_MASK02                  ((uint8_t)0x02U)
#define I2C_OA2_MASK03                  ((uint8_t)0x03U)
#define I2C_OA2_MASK04                  ((uint8_t)0x04U)
#define I2C_OA2_MASK05                  ((uint8_t)0x05U)
#define I2C_OA2_MASK06                  ((uint8_t)0x06U)
#define I2C_OA2_MASK07                  ((uint8_t)0x07U)
#define IS_I2C_OWN_ADDRESS2_MASK(MASK)  (((MASK) == I2C_OA2_NOMASK)  || \
                                         ((MASK) == I2C_OA2_MASK01) || \
                                         ((MASK) == I2C_OA2_MASK02) || \
                                         ((MASK) == I2C_OA2_MASK03) || \
                                         ((MASK) == I2C_OA2_MASK04) || \
                                         ((MASK) == I2C_OA2_MASK05) || \
                                         ((MASK) == I2C_OA2_MASK06) || \
                                         ((MASK) == I2C_OA2_MASK07))
#define I2C_GENERALCALL_DISABLE         (0x00000000U)
#define I2C_GENERALCALL_ENABLE          I2C_CR1_GCEN
#define IS_I2C_GENERAL_CALL(CALL)       (((CALL) == I2C_GENERALCALL_DISABLE) || ((CALL) == I2C_GENERALCALL_ENABLE))
#define I2C_NOSTRETCH_DISABLE           (0x00000000U)
#define I2C_NOSTRETCH_ENABLE            I2C_CR1_NOSTRETCH
#define IS_I2C_NO_STRETCH(STRETCH)      (((STRETCH) == I2C_NOSTRETCH_DISABLE) || ((STRETCH) == I2C_NOSTRETCH_ENABLE))

#define FMC_SDRAM_COLUMN_BITS_NUM_8           ((uint32_t)0x00000000U)
#define FMC_SDRAM_COLUMN_BITS_NUM_9           ((uint32_t)0x00000001U)
#define FMC_SDRAM_COLUMN_BITS_NUM_10          ((uint32_t)0x00000002U)
#define FMC_SDRAM_COLUMN_BITS_NUM_11          ((uint32_t)0x00000003U)
#define FMC_SDRAM_ROW_BITS_NUM_11             ((uint32_t)0x00000000U)
#define FMC_SDRAM_ROW_BITS_NUM_12             ((uint32_t)0x00000004U)
#define FMC_SDRAM_ROW_BITS_NUM_13             ((uint32_t)0x00000008U)
#define FMC_SDRAM_MEM_BUS_WIDTH_8             ((uint32_t)0x00000000U)
#define FMC_SDRAM_MEM_BUS_WIDTH_16            ((uint32_t)0x00000010U)
#define FMC_SDRAM_MEM_BUS_WIDTH_32            ((uint32_t)0x00000020U)
#define FMC_SDRAM_INTERN_BANKS_NUM_2          ((uint32_t)0x00000000U)
#define FMC_SDRAM_INTERN_BANKS_NUM_4          ((uint32_t)0x00000040U)
#define FMC_SDRAM_CAS_LATENCY_1               ((uint32_t)0x00000080U)
#define FMC_SDRAM_CAS_LATENCY_2               ((uint32_t)0x00000100U)
#define FMC_SDRAM_WRITE_PROTECTION_DISABLE    ((uint32_t)0x00000000U)
#define FMC_SDRAM_WRITE_PROTECTION_ENABLE     ((uint32_t)0x00000200U)
#define FMC_SDRAM_CAS_LATENCY_3               ((uint32_t)0x00000180)
#define FMC_SDRAM_CLOCK_DISABLE               ((uint32_t)0x00000000U)
#define FMC_SDRAM_CLOCK_PERIOD_2              ((uint32_t)0x00000800U)
#define FMC_SDRAM_CLOCK_PERIOD_3              ((uint32_t)0x00000C00)
#define FMC_SDRAM_RBURST_DISABLE              ((uint32_t)0x00000000U)
#define FMC_SDRAM_RBURST_ENABLE               ((uint32_t)0x00001000U)
#define FMC_SDRAM_RPIPE_DELAY_0               ((uint32_t)0x00000000U)
#define FMC_SDRAM_RPIPE_DELAY_1               ((uint32_t)0x00002000U)
#define FMC_SDRAM_RPIPE_DELAY_2               ((uint32_t)0x00004000U)
#define FMC_SDRAM_CMD_NORMAL_MODE             ((uint32_t)0x00000000U)
#define FMC_SDRAM_CMD_CLK_ENABLE              ((uint32_t)0x00000001U)
#define FMC_SDRAM_CMD_PALL                    ((uint32_t)0x00000002U)
#define FMC_SDRAM_CMD_AUTOREFRESH_MODE        ((uint32_t)0x00000003U)
#define FMC_SDRAM_CMD_LOAD_MODE               ((uint32_t)0x00000004U)
#define FMC_SDRAM_CMD_SELFREFRESH_MODE        ((uint32_t)0x00000005U)
#define FMC_SDRAM_CMD_POWERDOWN_MODE          ((uint32_t)0x00000006U)
#define FMC_SDRAM_CMD_TARGET_BANK2            FMC_SDCMR_CTB2
#define FMC_SDRAM_CMD_TARGET_BANK1            FMC_SDCMR_CTB1
#define FMC_SDRAM_CMD_TARGET_BANK1_2          ((uint32_t)0x00000018U)



typedef struct
{
    uint32_t CommandMode;                  /*!< Defines the command issued to the SDRAM device.
                                                This parameter can be a value of @ref FMC_SDRAM_Command_Mode.          */

    uint32_t CommandTarget;                /*!< Defines which device (1 or 2) the command will be issued to.
                                                This parameter can be a value of @ref FMC_SDRAM_Command_Target.        */

    uint32_t AutoRefreshNumber;            /*!< Defines the number of consecutive auto refresh command issued
                                                in auto refresh mode.
                                                This parameter can be a value between Min_Data = 1 and Max_Data = 16   */
    uint32_t ModeRegisterDefinition;       /*!< Defines the SDRAM Mode register content                                */
}FMC_SDRAM_CommandTypeDef;
typedef struct
{
    FMC_SDRAM_TypeDef* Instance;  /*!< Register base address                 */

    FMC_SDRAM_InitTypeDef         Init;       /*!< SDRAM device configuration parameters */

    __IO HAL_SDRAM_StateTypeDef   State;      /*!< SDRAM access state                    */

    HAL_LockTypeDef               Lock;       /*!< SDRAM locking object                  */

    DMA_HandleTypeDef* hdma;      /*!< Pointer DMA handler                   */

}SDRAM_HandleTypeDef;
typedef struct
{
    uint32_t LoadToActiveDelay;            /*!< Defines the delay between a Load Mode Register command and
                                                an active or Refresh command in number of memory clock cycles.
                                                This parameter can be a value between Min_Data = 1 and Max_Data = 16  */

    uint32_t ExitSelfRefreshDelay;         /*!< Defines the delay from releasing the self refresh command to
                                                issuing the Activate command in number of memory clock cycles.
                                                This parameter can be a value between Min_Data = 1 and Max_Data = 16  */

    uint32_t SelfRefreshTime;              /*!< Defines the minimum Self Refresh period in number of memory clock
                                                cycles.
                                                This parameter can be a value between Min_Data = 1 and Max_Data = 16  */

    uint32_t RowCycleDelay;                /*!< Defines the delay between the Refresh command and the Activate command
                                                and the delay between two consecutive Refresh commands in number of
                                                memory clock cycles.
                                                This parameter can be a value between Min_Data = 1 and Max_Data = 16  */

    uint32_t WriteRecoveryTime;            /*!< Defines the Write recovery Time in number of memory clock cycles.
                                                This parameter can be a value between Min_Data = 1 and Max_Data = 16  */

    uint32_t RPDelay;                      /*!< Defines the delay between a Precharge Command and an other command
                                                in number of memory clock cycles.
                                                This parameter can be a value between Min_Data = 1 and Max_Data = 16  */

    uint32_t RCDDelay;                     /*!< Defines the delay between the Activate Command and a Read/Write
                                                command in number of memory clock cycles.
                                                This parameter can be a value between Min_Data = 1 and Max_Data = 16  */
}FMC_SDRAM_TimingTypeDef;
typedef struct
{
    __IO uint32_t ISR;   /*!< DMA interrupt status register */
    __IO uint32_t Reserved0;
    __IO uint32_t IFCR;  /*!< DMA interrupt flag clear register */
} DMA_Base_Registers;


static void DSI_ConfigPacketHeader(DSI_TypeDef* DSIx, 
    uint32_t ChannelID, 
    uint32_t DataType, 
    uint32_t Data0,
    uint32_t Data1);


static HAL_StatusTypeDef DMA_CheckFifoParam(DMA_HandleTypeDef* hdma);

HAL_StatusTypeDef FMC_SDRAM_Init(FMC_SDRAM_TypeDef* Device, FMC_SDRAM_InitTypeDef* Init);
HAL_StatusTypeDef FMC_SDRAM_Timing_Init(FMC_SDRAM_TypeDef* Device, FMC_SDRAM_TimingTypeDef* Timing, uint32_t Bank);
HAL_StatusTypeDef  FMC_SDRAM_ProgramRefreshRate(FMC_SDRAM_TypeDef* Device, uint32_t RefreshRate);
HAL_StatusTypeDef FMC_SDRAM_SendCommand(FMC_SDRAM_TypeDef* Device, FMC_SDRAM_CommandTypeDef* Command, uint32_t Timeout);

void HAL_SDRAM_MspInit(SDRAM_HandleTypeDef* hsdram);
HAL_StatusTypeDef HAL_SDRAM_SendCommand(SDRAM_HandleTypeDef* hsdram, FMC_SDRAM_CommandTypeDef* Command, uint32_t Timeout);
HAL_StatusTypeDef HAL_SDRAM_ProgramRefreshRate(SDRAM_HandleTypeDef* hsdram, uint32_t RefreshRate);

HAL_StatusTypeDef HAL_DSI_Start(DSI_HandleTypeDef* hdsi);
static uint32_t DMA_CalcBaseAndBitshift(DMA_HandleTypeDef* hdma);

void HAL_LTDC_MspInit(LTDC_HandleTypeDef* hltdc); 
HAL_StatusTypeDef HAL_LTDC_Init(LTDC_HandleTypeDef* hltdc);
HAL_StatusTypeDef HAL_DMA_Init(DMA_HandleTypeDef* hdma);
HAL_StatusTypeDef HAL_DMA_DeInit(DMA_HandleTypeDef* hdma);

uint8_t BSP_SDRAM_Init(void);
void BSP_SDRAM_MspInit(SDRAM_HandleTypeDef* hsdram, void* Params);
void BSP_SDRAM_Initialization_sequence(uint32_t RefreshCount);
void HAL_GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_Init);


HAL_StatusTypeDef HAL_LTDC_ConfigLayer(LTDC_HandleTypeDef* hltdc, LTDC_LayerCfgTypeDef* pLayerCfg, uint32_t LayerIdx);

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c);
void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c);
HAL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef* hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t* pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_Init(I2C_HandleTypeDef* hi2c);
HAL_StatusTypeDef HAL_I2C_DeInit(I2C_HandleTypeDef* hi2c);
HAL_I2C_StateTypeDef HAL_I2C_GetState(I2C_HandleTypeDef* hi2c);

static void I2Cx_MspInit(I2C_HandleTypeDef* i2c_handler);
static void I2Cx_Error(I2C_HandleTypeDef* i2c_handler, uint8_t Addr);
static void I2Cx_Init(I2C_HandleTypeDef* i2c_handler);
static HAL_StatusTypeDef I2Cx_WriteMultiple(I2C_HandleTypeDef* i2c_handler, uint8_t Addr, uint16_t Reg, uint16_t MemAddress, uint8_t* Buffer, uint16_t Length);

static void I2C_Flush_TXDR(I2C_HandleTypeDef* hi2c);
static HAL_StatusTypeDef I2C_IsAcknowledgeFailed(I2C_HandleTypeDef* hi2c, uint32_t Timeout, uint32_t Tickstart);
static HAL_StatusTypeDef I2C_WaitOnSTOPFlagUntilTimeout(I2C_HandleTypeDef* hi2c, uint32_t Timeout, uint32_t Tickstart);
static HAL_StatusTypeDef I2C_RequestMemoryRead(I2C_HandleTypeDef* hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint32_t Timeout, uint32_t Tickstart);
static HAL_StatusTypeDef I2C_WaitOnTXISFlagUntilTimeout(I2C_HandleTypeDef* hi2c, uint32_t Timeout, uint32_t Tickstart);
static HAL_StatusTypeDef I2C_RequestMemoryWrite(I2C_HandleTypeDef* hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint32_t Timeout, uint32_t Tickstart);
static void I2C_TransferConfig(I2C_HandleTypeDef* hi2c, uint16_t DevAddress, uint8_t Size, uint32_t Mode, uint32_t Request);
static HAL_StatusTypeDef I2C_WaitOnFlagUntilTimeout(I2C_HandleTypeDef* hi2c, uint32_t Flag, FlagStatus Status, uint32_t Timeout, uint32_t Tickstart);


uint8_t BSP_TS_Init(void);
uint8_t  TS_IO_Read(uint8_t Addr, uint8_t Reg);
uint16_t TS_IO_ReadMultiple(uint8_t Addr, uint8_t Reg, uint8_t* Buffer, uint16_t Length);
void TS_IO_Write(uint8_t Addr, uint8_t Reg, uint8_t Value);
void TS_IO_WriteMultiple(uint8_t Addr, uint8_t Reg, uint8_t* Buffer, uint16_t Length);

static HAL_StatusTypeDef I2Cx_ReadMultiple(I2C_HandleTypeDef* i2c_handler, uint8_t Addr, uint16_t Reg, uint16_t MemAddSize, uint8_t* Buffer, uint16_t Length);

HAL_StatusTypeDef HAL_SDRAM_Init(SDRAM_HandleTypeDef* hsdram, FMC_SDRAM_TimingTypeDef* Timing);

static void LTDC_SetConfig(LTDC_HandleTypeDef* hltdc, LTDC_LayerCfgTypeDef* pLayerCfg, uint32_t LayerIdx);

HAL_StatusTypeDef HAL_I2C_Mem_Read(I2C_HandleTypeDef* hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t* pData, uint16_t Size, uint32_t Timeout);

void HAL_DSI_MspInit(void);
void HAL_DSI_MspDeInit(DSI_HandleTypeDef* hdsi);
HAL_StatusTypeDef HAL_DSI_DeInit(DSI_HandleTypeDef* hdsi);
HAL_StatusTypeDef HAL_DSI_ConfigVideoMode(DSI_HandleTypeDef* hdsi, DSI_VidCfgTypeDef* VidCfg);

void TS_IO_Init(void);

uint8_t BSP_LCD_InitEx();
void BSP_LCD_LayerDefaultInit(uint32_t LayerIndex, uint32_t FB_Address);
void BSP_LCD_MspDeInit(void);
void BSP_LCD_Reset(void);
void BSP_LCD_MspInit(void);
void BSP_LCD_MspDeInit(void);

void HAL_NVIC_DisableIRQ(IRQn_Type IRQn);
void HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
HAL_StatusTypeDef HAL_RCCEx_PeriphCLKConfig(RCC_PeriphCLKInitTypeDef* PeriphClkInit);
HAL_StatusTypeDef HAL_LTDCEx_StructInitFromVideoConfig(LTDC_HandleTypeDef* hltdc, DSI_VidCfgTypeDef* VidCfg);
HAL_StatusTypeDef HAL_DSI_Init(DSI_HandleTypeDef* hdsi, DSI_PLLInitTypeDef* PLLInit);

static void DSI_ConfigPacketHeader(DSI_TypeDef* DSIx, uint32_t ChannelID, uint32_t DataType, uint32_t Data0,
    uint32_t Data1);

static HAL_StatusTypeDef HAL_DSI_ShortWrite(DSI_HandleTypeDef* hdsi,
    uint32_t ChannelID,
    uint32_t Mode,
    uint32_t Param1,
    uint32_t Param2);
HAL_StatusTypeDef HAL_DSI_LongWrite(DSI_HandleTypeDef* hdsi,
    uint32_t ChannelID,
    uint32_t Mode,
    uint32_t NbParams,
    uint32_t Param1,
    uint8_t* ParametersTable);


#endif
