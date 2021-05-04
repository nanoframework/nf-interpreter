//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2017 STMicroelectronics.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoCLR_Headers.h>
#include <stm32l4xx_hal.h>
#include <stm32l4xx_ll_gpio.h>
#include <stm32l4xx_ll_bus.h>
#include <stm32l4xx_ll_rcc.h>
#include <stm32l4xx_ll_dma.h>
#include <stm32l4xx_ll_usart.h>
#include <stm32l4xx_ll_rtc.h>
#include <stm32l4xx_ll_pwr.h>
#include <stm32l4xx_ll_utils.h>
#include <stm32l4xx_ll_system.h>
#include <stm32l475e_iot01.h>
#include <stm32l475e_iot01_accelero.h>
#include <stm32l475e_iot01_gyro.h>
#include <stm32l475e_iot01_hsensor.h>
#include <stm32l475e_iot01_magneto.h>
#include <stm32l475e_iot01_psensor.h>
#include <stm32l475e_iot01_tsensor.h>

#include <nanoRingBuffer.h>
#include <platformHAL.h>

// LSE as RTC clock
#define RTC_ASYNCH_PREDIV 0x7F
// LSE as RTC clock
#define RTC_SYNCH_PREDIV 0x00FF

#ifdef HAL_RTC_MODULE_ENABLED
RTC_HandleTypeDef RtcHandle;
#endif

CRC_HandleTypeDef CrcHandle;
UART_HandleTypeDef WProtocolUart;

NanoRingBuffer WPRingBuffer;

// ST DMA buffers need to be aligned with 32 bytes cache page size boundary
#if defined(__GNUC__)
__attribute__((aligned(32)))
#endif
uint8_t rxBuffer[WIRE_PROTOCOL_UART_BUFFER_SIZE];

void System_IniRtc(void)
{
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
    LL_PWR_EnableBkUpAccess();

    // Enable LSI
    LL_RCC_LSI_Enable();
    while (LL_RCC_LSI_IsReady() != 1)
    {
    }

    LL_RCC_ForceBackupDomainReset();
    LL_RCC_ReleaseBackupDomainReset();
    LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSI);

    // Enable RTC Clock
    LL_RCC_EnableRTC();

    LL_RTC_DisableWriteProtection(RTC);
    LL_RTC_EnableInitMode(RTC);
    while (LL_RTC_IsActiveFlag_INIT(RTC) != 1)
    {
    };

    LL_RTC_SetHourFormat(RTC, LL_RTC_HOURFORMAT_24HOUR);
    LL_RTC_SetAsynchPrescaler(RTC, RTC_ASYNCH_PREDIV);
    LL_RTC_SetSynchPrescaler(RTC, RTC_SYNCH_PREDIV);
    LL_RTC_SetAlarmOutEvent(RTC, LL_RTC_ALARMOUT_DISABLE);
    LL_RTC_DisableInitMode(RTC);
    LL_RTC_ClearFlag_RS(RTC);

    while (LL_RTC_IsActiveFlag_RS(RTC) != 1)
    {
    };

    LL_RTC_EnableWriteProtection(RTC);

    RtcHandle.Instance = RTC;
    RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
    RtcHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
    RtcHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
    RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
    RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

    if (HAL_RTC_Init(&RtcHandle) != HAL_OK)
    {
        // Initialization Error
        while (1)
        {
        }
    }
}

void SystemClock_Config(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;

    /* MSI is enabled after System reset, activate PLL with MSI as source */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    RCC_OscInitStruct.MSIState = RCC_MSI_ON;
    RCC_OscInitStruct.MSICalibrationValue = 0;
    RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_11;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
    RCC_OscInitStruct.PLL.PLLM = 6;
    RCC_OscInitStruct.PLL.PLLN = 20;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        /* Initialization Error */
        while (1)
            ;
    }

    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
       clocks dividers */
    RCC_ClkInitStruct.ClockType =
        (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
    {
        /* Initialization Error */
        while (1)
            ;
    }

    __HAL_RCC_PWR_CLK_ENABLE();

    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
    {
        while (1)
            ;
    }

    // LL_SetFlashLatency(LL_FLASH_LATENCY_4);
    // LL_RCC_MSI_Enable();
    // while (LL_RCC_MSI_IsReady() == 0)
    // {
    // };

    // // Main PLL configuration and activation
    // LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_MSI, LL_RCC_PLLM_DIV_1, 40, LL_RCC_PLLR_DIV_2);
    // LL_RCC_PLL_Enable();
    // LL_RCC_PLL_EnableDomain_SYS();
    // while (LL_RCC_PLL_IsReady() == 0)
    // {
    // };

    // // Sysclk activation on the main PLL
    // LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    // LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
    // while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
    // {
    // };

    // // Set APB1 & APB2 prescaler
    // LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
    // LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

    // LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);

    // // Update CMSIS variable
    // LL_SetSystemCoreClock(80000000);
}

void WProtocol_COM_Init()
{
    // Enable GPIO clock
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);

    // Configure Tx Pin as : Alternate function, High Speed, Push pull, Pull up
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_6, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_0_7(GPIOB, LL_GPIO_PIN_6, LL_GPIO_AF_7);
    LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_6, LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_6, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_6, LL_GPIO_PULL_UP);

    // Configure Rx Pin as : Alternate function, High Speed, Push pull, Pull up
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_7, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_0_7(GPIOB, LL_GPIO_PIN_7, LL_GPIO_AF_7);
    LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_7, LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_7, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_7, LL_GPIO_PULL_UP);

    // USART configuration
    LL_USART_Disable(USART1);
    LL_USART_SetTransferDirection(USART1, LL_USART_DIRECTION_TX_RX);
    LL_USART_ConfigCharacter(USART1, LL_USART_DATAWIDTH_8B, LL_USART_PARITY_NONE, LL_USART_STOPBITS_1);
    LL_USART_SetHWFlowCtrl(USART1, LL_USART_HWCONTROL_NONE);
    LL_USART_SetOverSampling(USART1, LL_USART_OVERSAMPLING_16);
    LL_USART_SetBaudRate(USART1, LL_RCC_GetUSARTClockFreq(LL_RCC_USART1_CLKSOURCE), LL_USART_OVERSAMPLING_16, 921600);

    LL_USART_Enable(USART1);

    // Enable RXNE and Error interrupts
    LL_USART_EnableIT_RXNE(USART1);
    LL_USART_EnableIT_ERROR(USART1);

    // NVIC Configuration for USART interrupts
    //  - Set priority for USARTx_IRQn
    //  - Enable USARTx_IRQn
    NVIC_SetPriority(USART1_IRQn, 0);
    NVIC_EnableIRQ(USART1_IRQn);

    // Enable USART clock
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);
    LL_RCC_SetUSARTClockSource(LL_RCC_USART2_CLKSOURCE_PCLK1);

    // Enable DMA clock
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

    // ##-4- Configure the NVIC for DMA
    // NVIC configuration for DMA transfer complete interrupt (USART1_TX)
    NVIC_SetPriority(DMA1_Channel4_IRQn, 0);
    NVIC_EnableIRQ(DMA1_Channel4_IRQn);

    // Configure the DMA functional parameters for transmission
    LL_DMA_ConfigTransfer(
        DMA1,
        LL_DMA_CHANNEL_4,
        LL_DMA_DIRECTION_MEMORY_TO_PERIPH | LL_DMA_PRIORITY_HIGH | LL_DMA_MODE_NORMAL | LL_DMA_PERIPH_NOINCREMENT |
            LL_DMA_MEMORY_INCREMENT | LL_DMA_PDATAALIGN_BYTE | LL_DMA_MDATAALIGN_BYTE);
    LL_DMA_ConfigAddresses(
        DMA1,
        LL_DMA_CHANNEL_4,
        (uint32_t)NULL,
        LL_USART_DMA_GetRegAddr(USART2, LL_USART_DMA_REG_DATA_TRANSMIT),
        LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_4));
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_4, 0);
    LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_4, LL_DMA_REQUEST_2);
}

void BoardInit(bool initSensors, bool initGpios)
{
    // regular ST HAL inits
    HAL_Init();

    SystemClock_Config();

    // System_IniRtc();

    // only init GPIOs if required
    if (initGpios)
    {
        BSP_LED_Init(LED2);

        BSP_PB_Init(BUTTON_USER, BUTTON_MODE_GPIO);
    }

    // Initilize the RX ring buffer
    NanoRingBuffer_Initialize(&WPRingBuffer, rxBuffer, WIRE_PROTOCOL_UART_BUFFER_SIZE);

    WProtocol_COM_Init(&WProtocolUart);

    // only init board sensors if requested
    if (initSensors)
    {
        BSP_ACCELERO_Init();
        BSP_GYRO_Init();
        BSP_MAGNETO_Init();
        BSP_GYRO_Init();
        BSP_MAGNETO_Init();
        BSP_HSENSOR_Init();
        BSP_TSENSOR_Init();
        BSP_PSENSOR_Init();
    }

    // config CRC32 unit
    CrcHandle.Instance = CRC;
    CrcHandle.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
    CrcHandle.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
    CrcHandle.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
    CrcHandle.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;

    HAL_CRC_Init(&CrcHandle);
}
