//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2017 STMicroelectronics.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoCLR_Headers.h>
#include <stm32l4xx_hal.h>
#include <stm32l475e_iot01.h>
#include <stm32l475e_iot01_accelero.h>
#include <stm32l475e_iot01_gyro.h>
#include <stm32l475e_iot01_hsensor.h>
#include <stm32l475e_iot01_magneto.h>
#include <stm32l475e_iot01_psensor.h>
#include <stm32l475e_iot01_tsensor.h>

// LSE as RTC clock
#define RTC_ASYNCH_PREDIV 0x7F
// LSE as RTC clock
#define RTC_SYNCH_PREDIV 0x00FF

#ifdef HAL_RTC_MODULE_ENABLED
RTC_HandleTypeDef RtcHandle;
#endif

CRC_HandleTypeDef CrcHandle;
UART_HandleTypeDef WProtocolUart;
DMA_HandleTypeDef s_DMAHandle;

#ifdef HAL_RTC_MODULE_ENABLED
void System_IniRtc(void)
{
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
#endif // HAL_RTC_MODULE_ENABLED

void SystemClock_Config(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;

    /* MSI is enabled after System reset, activate PLL with MSI as source */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_MSI;
    ;
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
}

void WProtocol_COM_Init(COM_TypeDef COM, UART_HandleTypeDef *huart)
{
    static DMA_HandleTypeDef hdma_tx;
    static DMA_HandleTypeDef hdma_rx;

    GPIO_InitTypeDef gpio_init_structure;

    /* Enable GPIO clock */
    DISCOVERY_COMx_TX_GPIO_CLK_ENABLE(COM);
    DISCOVERY_COMx_RX_GPIO_CLK_ENABLE(COM);

    /* Enable USART clock */
    DISCOVERY_COMx_CLK_ENABLE(COM);

    /* Enable DMA clock */
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* Configure USART Tx as alternate function */
    gpio_init_structure.Pin = DISCOVERY_COM1_TX_PIN;
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init_structure.Pull = GPIO_NOPULL;
    gpio_init_structure.Alternate = DISCOVERY_COM1_TX_AF;
    HAL_GPIO_Init(DISCOVERY_COM1_TX_GPIO_PORT, &gpio_init_structure);

    /* Configure USART Rx as alternate function */
    gpio_init_structure.Pin = DISCOVERY_COM1_RX_PIN;
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Alternate = DISCOVERY_COM1_RX_AF;
    HAL_GPIO_Init(DISCOVERY_COM1_TX_GPIO_PORT, &gpio_init_structure);

    /* USART configuration */
    huart->Instance = DISCOVERY_COM1;
    HAL_UART_Init(huart);

    // Configure the DMA handler for Transmission process
    hdma_tx.Instance = DMA1_Channel4;
    hdma_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_tx.Init.Mode = DMA_NORMAL;
    hdma_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_tx.Init.Request = DMA_REQUEST_2;

    HAL_DMA_Init(&hdma_tx);

    /* Associate the initialized DMA handle to the UART handle */
    __HAL_LINKDMA(huart, hdmatx, hdma_tx);

    /* Configure the DMA handler for reception process */
    hdma_rx.Instance = DMA1_Channel5;
    hdma_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_rx.Init.Mode = DMA_NORMAL;
    hdma_rx.Init.Priority = DMA_PRIORITY_HIGH;
    hdma_rx.Init.Request = DMA_REQUEST_2;

    HAL_DMA_Init(&hdma_rx);

    /* Associate the initialized DMA handle to the the UART handle */
    __HAL_LINKDMA(huart, hdmarx, hdma_rx);

    /*##-4- Configure the NVIC for DMA #########################################*/
    /* NVIC configuration for DMA transfer complete interrupt (USART1_TX) */
    HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);

    /* NVIC configuration for DMA transfer complete interrupt (USART1_RX) */
    HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

    /* NVIC for USART */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
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

    WProtocolUart.Instance = USART1;
    WProtocolUart.Init.BaudRate = 921600;
    WProtocolUart.Init.WordLength = UART_WORDLENGTH_8B;
    WProtocolUart.Init.StopBits = UART_STOPBITS_1;
    WProtocolUart.Init.Parity = UART_PARITY_NONE;
    WProtocolUart.Init.Mode = UART_MODE_TX_RX;
    WProtocolUart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    WProtocolUart.Init.OverSampling = UART_OVERSAMPLING_16;
    WProtocolUart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    WProtocol_COM_Init(COM1, &WProtocolUart);

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
