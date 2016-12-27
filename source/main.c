// Includes required for CMSIS compatible RTOS
#if defined(FREERTOS) || defined(MBEDRTOS) || defined(RTXRTOS) || defined(RTXRTOS2)

#include "cmsis_os.h"

#if defined(FREERTOS) 
#include "FreeRTOS.h"
#include "task.h"
#endif

#endif

// General incudes and defines according to device series
#if defined STM32F0

#include "stm32f0xx.h"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_tim.h"

#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA

#elif defined STM32F4

#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"

#define LD4_Pin GPIO_PIN_12
#define LD4_GPIO_Port GPIOD
#define LD3_Pin GPIO_PIN_13
#define LD3_GPIO_Port GPIOD
#define LD5_Pin GPIO_PIN_14
#define LD5_GPIO_Port GPIOD
#define LD6_Pin GPIO_PIN_15
#define LD6_GPIO_Port GPIOD
#define BOOT1_Pin GPIO_PIN_2
#define BOOT1_GPIO_Port GPIOB
#define CLK_IN_Pin GPIO_PIN_10
#define CLK_IN_GPIO_Port GPIOB

#endif


// #if defined STM32F4
// RTC_HandleTypeDef hrtc;
// #endif

void SystemClock_Config(void);
void Error_Handler(void);
void MX_GPIO_Init(void);
void StartDefaultTask(void const * argument);

TIM_HandleTypeDef        htim1; 
uint32_t                 uwIncrementState = 0;

// thread ID required for CMSIS compatible RTOS
#if defined(FREERTOS) || defined(MBEDRTOS) || defined(RTXRTOS)
osThreadId defaultTaskHandle;
#endif

int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

#if defined STM32F4
  HAL_DBGMCU_EnableDBGSleepMode();
  HAL_DBGMCU_EnableDBGStopMode();
  HAL_DBGMCU_EnableDBGStandbyMode();
#endif

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  
// CMSIS RTOS defined
#if defined(FREERTOS) || defined(MBEDRTOS) || defined(RTXRTOS) || defined(RTXRTOS2)
 
#if defined(FREERTOS) 

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

#elif defined(MBEDRTOS)

  osKernelInitialize ();

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(StartDefaultTask, osPriorityNormal, 128);
  defaultTaskHandle = osThreadCreate(osThread(StartDefaultTask), NULL);

#elif defined(RTXRTOS) || defined(RTXRTOS2)

  osKernelInitialize ();

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(StartDefaultTask), NULL);
  
#endif

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  while (1) {}

// no OS
#else

  for (;;) {

#if defined STM32F0

      HAL_GPIO_WritePin(GPIOA, LD2_Pin, GPIO_PIN_SET);
      HAL_Delay(250);
      HAL_GPIO_WritePin(GPIOA, LD2_Pin, GPIO_PIN_RESET);
      HAL_Delay(250);

#elif defined STM32F4

      HAL_GPIO_WritePin(GPIOD, LD3_Pin, GPIO_PIN_SET);
      HAL_Delay(250);
      HAL_GPIO_WritePin(GPIOD, LD3_Pin, GPIO_PIN_RESET);
      
      HAL_GPIO_WritePin(GPIOD, LD5_Pin, GPIO_PIN_SET);
      HAL_Delay(250);
      HAL_GPIO_WritePin(GPIOD, LD5_Pin, GPIO_PIN_RESET);

      HAL_GPIO_WritePin(GPIOD, LD6_Pin, GPIO_PIN_SET);
      HAL_Delay(250);
      HAL_GPIO_WritePin(GPIOD, LD6_Pin, GPIO_PIN_RESET);

      HAL_GPIO_WritePin(GPIOD, LD4_Pin, GPIO_PIN_SET);
      HAL_Delay(250);
      HAL_GPIO_WritePin(GPIOD, LD4_Pin, GPIO_PIN_RESET);

#endif
    
  }

#endif  //// no RTOS defined

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{
#if defined STM32F0

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI48;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
#if defined(FREERTOS) || defined(MBEDRTOS)
  HAL_NVIC_SetPriority(SysTick_IRQn, 3, 0);
#else
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
#endif
  

#elif defined STM32F4

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
#if defined(FREERTOS) || defined(MBEDRTOS)
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
#else
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
#endif

#endif

}

void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

#if defined STM32F0
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

#elif defined STM32F4

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : BOOT1_Pin */
  GPIO_InitStruct.Pin = BOOT1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BOOT1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : CLK_IN_Pin */
  GPIO_InitStruct.Pin = CLK_IN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(CLK_IN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD4_Pin LD3_Pin LD5_Pin LD6_Pin */
  GPIO_InitStruct.Pin = LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

#endif

}

#if defined(FREERTOS) || defined(MBEDRTOS) || defined(RTXRTOS)|| defined(RTXRTOS2)
/* StartDefaultTask function */
void StartDefaultTask(void const * argument)
{

  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {

#if defined STM32F0

      HAL_GPIO_WritePin(GPIOA, LD2_Pin, GPIO_PIN_SET);
      osDelay(250);
      HAL_GPIO_WritePin(GPIOA, LD2_Pin, GPIO_PIN_RESET);
      osDelay(250);

#elif defined STM32F4

      HAL_GPIO_WritePin(GPIOD, LD3_Pin, GPIO_PIN_SET);
      osDelay(250);
      HAL_GPIO_WritePin(GPIOD, LD3_Pin, GPIO_PIN_RESET);
      
      HAL_GPIO_WritePin(GPIOD, LD5_Pin, GPIO_PIN_SET);
      osDelay(250);
      HAL_GPIO_WritePin(GPIOD, LD5_Pin, GPIO_PIN_RESET);

      HAL_GPIO_WritePin(GPIOD, LD6_Pin, GPIO_PIN_SET);
      osDelay(250);
      HAL_GPIO_WritePin(GPIOD, LD6_Pin, GPIO_PIN_RESET);

      HAL_GPIO_WritePin(GPIOD, LD4_Pin, GPIO_PIN_SET);
      osDelay(250);
      HAL_GPIO_WritePin(GPIOD, LD4_Pin, GPIO_PIN_RESET);

#endif

  }
  /* USER CODE END 5 */ 
}
#endif

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
}

#if defined(FREERTOS) && (!defined(MBEDRTOS))

void SysTick_Handler(void)
{

#if defined(FREERTOS) 
  osSystickHandler();
#elif defined(MBEDRTOS)

#else
  HAL_SYSTICK_IRQHandler();
#endif

}

#endif

#if defined(MBEDRTOS)
void error(void){}
#endif

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{

  /* System interrupt init*/
  /* SVC_IRQn interrupt configuration */
#if defined STM32F0
  __HAL_RCC_SYSCFG_CLK_ENABLE();

  /* System interrupt init*/
  /* SVC_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SVC_IRQn, 0, 0);
  /* PendSV_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(PendSV_IRQn, 0, 0);
  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

#elif defined STM32F4
  
  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* System interrupt init*/
  /* MemoryManagement_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
  /* BusFault_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
  /* UsageFault_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
  /* SVCall_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SVCall_IRQn, 0, 0);
  /* DebugMonitor_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DebugMonitor_IRQn, 0, 0);

#if defined(FREERTOS) || defined(MBEDRTOS)
  /* PendSV_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(PendSV_IRQn, 15, 0);
  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
#else
  /* PendSV_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(PendSV_IRQn, 0, 0);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
#endif

#endif  
}

/**
  * @brief  This function configures the TIM1 as a time base source. 
  *         The time source is configured  to have 1ms time base with a dedicated 
  *         Tick interrupt priority. 
  * @note   This function is called  automatically at the beginning of program after
  *         reset by HAL_Init() or at any time when clock is configured, by HAL_RCC_ClockConfig(). 
  * @param  TickPriority: Tick interrupt priorty.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{ 
#if defined STM32F0

  RCC_ClkInitTypeDef    clkconfig;
  uint32_t              uwTimclock = 0;
  uint32_t              uwPrescalerValue = 0;
  uint32_t              pFLatency;
  
  /*Configure the TIM1 IRQ priority */
  HAL_NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn, TickPriority ,0); 
  
  /* Enable the TIM1 global Interrupt */
  HAL_NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn); 
  
  /* Enable TIM1 clock */
  __HAL_RCC_TIM1_CLK_ENABLE();
  
  /* Get clock configuration */
  HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);
  
  /* Compute TIM1 clock */
  uwTimclock = HAL_RCC_GetPCLK1Freq();
   
  /* Compute the prescaler value to have TIM1 counter clock equal to 1MHz */
  uwPrescalerValue = (uint32_t) ((uwTimclock / 1000000) - 1);
  
  /* Initialize TIM1 */
  htim1.Instance = TIM1;
  
  /* Initialize TIMx peripheral as follow:
  + Period = [(TIM1CLK/1000) - 1]. to have a (1/1000) s time base.
  + Prescaler = (uwTimclock/1000000 - 1) to have a 1MHz counter clock.
  + ClockDivision = 0
  + Counter direction = Up
  */
  htim1.Init.Period = (1000000 / 1000) - 1;
  htim1.Init.Prescaler = uwPrescalerValue;
  htim1.Init.ClockDivision = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  if(HAL_TIM_Base_Init(&htim1) == HAL_OK)
  {
    /* Start the TIM time Base generation in interrupt mode */
    return HAL_TIM_Base_Start_IT(&htim1);
  }
  
#elif defined STM32F4
 
  RCC_ClkInitTypeDef    clkconfig;
  uint32_t              uwTimclock = 0;
  uint32_t              uwPrescalerValue = 0;
  uint32_t              pFLatency;
  
  /*Configure the TIM1 IRQ priority */
  HAL_NVIC_SetPriority(TIM1_UP_TIM10_IRQn, TickPriority ,0); 
  
  /* Enable the TIM1 global Interrupt */
  HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn); 
  
  /* Enable TIM1 clock */
  __HAL_RCC_TIM1_CLK_ENABLE();
  
  /* Get clock configuration */
  HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);
  
  /* Compute TIM1 clock */
  uwTimclock = 2*HAL_RCC_GetPCLK2Freq();
   
  /* Compute the prescaler value to have TIM1 counter clock equal to 1MHz */
  uwPrescalerValue = (uint32_t) ((uwTimclock / 1000000) - 1);
  
  /* Initialize TIM1 */
  htim1.Instance = TIM1;
  
  /* Initialize TIMx peripheral as follow:
  + Period = [(TIM1CLK/1000) - 1]. to have a (1/1000) s time base.
  + Prescaler = (uwTimclock/1000000 - 1) to have a 1MHz counter clock.
  + ClockDivision = 0
  + Counter direction = Up
  */
  htim1.Init.Period = (1000000 / 1000) - 1;
  htim1.Init.Prescaler = uwPrescalerValue;
  htim1.Init.ClockDivision = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  if(HAL_TIM_Base_Init(&htim1) == HAL_OK)
  {
    /* Start the TIM time Base generation in interrupt mode */
    return HAL_TIM_Base_Start_IT(&htim1);
  }
  
#endif  
   
  /* Return function status */
  return HAL_ERROR;
}

/**
  * @brief  Suspend Tick increment.
  * @note   Disable the tick increment by disabling TIM1 update interrupt.
  * @param  None
  * @retval None
  */
void HAL_SuspendTick(void)
{
  /* Disable TIM1 update Interrupt */
  __HAL_TIM_DISABLE_IT(&htim1, TIM_IT_UPDATE);                                                  
}

/**
  * @brief  Resume Tick increment.
  * @note   Enable the tick increment by Enabling TIM1 update interrupt.
  * @param  None
  * @retval None
  */
void HAL_ResumeTick(void)
{
  /* Enable TIM1 Update interrupt */
  __HAL_TIM_ENABLE_IT(&htim1, TIM_IT_UPDATE);
}

#if defined STM32F0
void TIM1_BRK_UP_TRG_COM_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim1);
}
#elif defined STM32F4
void TIM1_UP_TIM10_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim1);
}
#endif
