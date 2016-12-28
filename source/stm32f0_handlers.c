#include <stdio.h>

/* Default empty handler */
void Default_Handler(void)
{
#if defined DEBUG
  __BKPT(3);
#endif
  for (;;) { }
}

/* Cortex-M0+ core handlers */
void HardFault_Handler(void) __attribute__ ((weak, alias("Default_Handler")));
void Reset_Handler    (void);
void NMI_Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void SVC_Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void PendSV_Handler   (void) __attribute__ ((weak, alias("Default_Handler")));
void SysTick_Handler  (void);

/* Peripherals handlers */
void WWDG_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void PVD_VDDIO2_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void RTC_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void FLASH_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void RCC_CRS_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI0_1_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI2_3_IRQHandler     (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI4_15_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void TSC_IRQHandler    (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Ch1_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Ch2_3_DMA2_Ch1_2_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Ch4_7_DMA2_Ch3_5_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC1_COMP_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM1_BRK_UP_TRG_COM_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM1_CC_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM2_IRQHandler     (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM3_IRQHandler     (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM6_DAC_IRQHandler     (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM7_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM14_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM15_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM16_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM17_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C1_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C2_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI1_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI2_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void USART1_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void USART2_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void USART3_8_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void CEC_CAN_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));

/* Initialize segments */
extern uint32_t _sidata;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;
extern uint32_t _estack;

typedef struct _DeviceVectors
{
  /* Stack pointer */
  void* pvStack;

  /* Cortex-M handlers */
  void* pfnReset_Handler;
  void* pfnNMI_Handler;
  void* pfnHardFault_Handler;
  void* pfnReserved1_Handler; /* Reserved */
  void* pfnReserved2_Handler; /* Reserved */
  void* pfnReserved3_Handler; /* Reserved */
  void* pfnReserved4_Handler; /* Reserved */
  void* pfnReserved5_Handler; /* Reserved */
  void* pfnReserved6_Handler; /* Reserved */
  void* pfnReserved7_Handler; /* Reserved */
  void* pfnSVC_Handler;
  void* pfnReserved8_Handler; /* Reserved */
  void* pfnReserved9_Handler; /* Reserved */
  void* pfnPendSV_Handler;
  void* pfnSysTick_Handler;

  /* Configurable interrupts */
  void* pfnWWDG_IRQHandler;                   /* Window WatchDog              */
  void* pfnPVD_VDDIO2_IRQHandler;             /* PVD and VDDIO2 through EXTI Line detect */
  void* pfnRTC_IRQHandler;                    /* RTC through the EXTI line    */
  void* pfnFLASH_IRQHandler;                  /* FLASH                        */
  void* pfnRCC_CRS_IRQHandler;                /* RCC and CRS                  */
  void* pfnEXTI0_1_IRQHandler;                /* EXTI Line 0 and 1            */
  void* pfnEXTI2_3_IRQHandler;                /* EXTI Line 2 and 3            */
  void* pfnEXTI4_15_IRQHandler;               /* EXTI Line 4 to 15            */
  void* pfnTSC_IRQHandler;                    /* TSC                          */
  void* pfnDMA1_Ch1_IRQHandler;               /* DMA1 Channel 1               */
  void* pfnDMA1_Ch2_3_DMA2_Ch1_2_IRQHandler;  /* DMA1 Channel 2 and 3 & DMA2 Channel 1 and 2 */
  void* pfnDMA1_Ch4_7_DMA2_Ch3_5_IRQHandler;  /* DMA1 Channel 4 to 7 & DMA2 Channel 3 to 5 */
  void* pfnADC1_COMP_IRQHandler;             /* ADC1, COMP1 and COMP2         */
  void* pfnTIM1_BRK_UP_TRG_COM_IRQHandler;    /* TIM1 Break, Update, Trigger and Commutation */
  void* pfnTIM1_CC_IRQHandler;                /* TIM1 Capture Compare         */
  void* pfnTIM2_IRQHandler;                   /* TIM2                         */
  void* pfnTIM3_IRQHandler;                   /* TIM3                         */
  void* pfnTIM6_DAC_IRQHandler;               /* TIM6 and DAC                 */
  void* pfnTIM7_IRQHandler;                   /* TIM7                         */
  void* pfnTIM14_IRQHandler;                  /* TIM14                        */
  void* pfnTIM15_IRQHandler;                  /* TIM15                        */
  void* pfnTIM16_IRQHandler;                  /* TIM16                        */
  void* pfnTIM17_IRQHandler;                  /* TIM17                        */
  void* pfnI2C1_IRQHandler;                   /* I2C1                         */
  void* pfnI2C2_IRQHandler;                   /* I2C2                         */
  void* pfnSPI1_IRQHandler;                   /* SPI1                         */
  void* pfnSPI2_IRQHandler;                   /* SPI2                         */
  void* pfnUSART1_IRQHandler;                 /* USART1                       */
  void* pfnUSART2_IRQHandler;                 /* USART2                       */
  void* pfnUSART3_8_IRQHandler;               /* USART3, USART4, USART5, USART6, USART7, USART8 */
  void* pfnCEC_CAN_IRQHandler;                 /* CEC and CAN                  */ 

} DeviceVectors;

/* Exception Table */
__attribute__ ((section(".isr_vector"))) const DeviceVectors exception_table =
{
  /* Configure Initial Stack Pointer, using linker-generated symbols */
  (void*) (&_estack),

  (void*) Reset_Handler,
  (void*) NMI_Handler,
  (void*) HardFault_Handler,
  (void*) (0UL), /* Reserved */
  (void*) (0UL), /* Reserved */
  (void*) (0UL), /* Reserved */
  (void*) (0UL), /* Reserved */
  (void*) (0UL), /* Reserved */
  (void*) (0UL), /* Reserved */
  (void*) (0UL), /* Reserved */
  (void*) SVC_Handler,
  (void*) (0UL), /* Reserved */
  (void*) (0UL), /* Reserved */
  (void*) PendSV_Handler,
  (void*) SysTick_Handler,

  /* Configurable interrupts */
  (void*) WWDG_IRQHandler,                   /* Window WatchDog              */
  (void*) PVD_VDDIO2_IRQHandler,             /* PVD and VDDIO2 through EXTI Line detect */
  (void*) RTC_IRQHandler,                    /* RTC through the EXTI line    */
  (void*) FLASH_IRQHandler,                  /* FLASH                        */
  (void*) RCC_CRS_IRQHandler,                /* RCC and CRS                  */
  (void*) EXTI0_1_IRQHandler,                /* EXTI Line 0 and 1            */
  (void*) EXTI2_3_IRQHandler,                /* EXTI Line 2 and 3            */
  (void*) EXTI4_15_IRQHandler,               /* EXTI Line 4 to 15            */
  (void*) TSC_IRQHandler,                    /* TSC                          */
  (void*) DMA1_Ch1_IRQHandler,               /* DMA1 Channel 1               */
  (void*) DMA1_Ch2_3_DMA2_Ch1_2_IRQHandler,  /* DMA1 Channel 2 and 3 & DMA2 Channel 1 and 2 */
  (void*) DMA1_Ch4_7_DMA2_Ch3_5_IRQHandler,  /* DMA1 Channel 4 to 7 & DMA2 Channel 3 to 5 */
  (void*) ADC1_COMP_IRQHandler,             /* ADC1, COMP1 and COMP2         */
  (void*) TIM1_BRK_UP_TRG_COM_IRQHandler,    /* TIM1 Break, Update, Trigger and Commutation */
  (void*) TIM1_CC_IRQHandler,                /* TIM1 Capture Compare         */
  (void*) TIM2_IRQHandler,                   /* TIM2                         */
  (void*) TIM3_IRQHandler,                   /* TIM3                         */
  (void*) TIM6_DAC_IRQHandler,               /* TIM6 and DAC                 */
  (void*) TIM7_IRQHandler,                   /* TIM7                         */
  (void*) TIM14_IRQHandler,                  /* TIM14                        */
  (void*) TIM15_IRQHandler,                  /* TIM15                        */
  (void*) TIM16_IRQHandler,                  /* TIM16                        */
  (void*) TIM17_IRQHandler,                  /* TIM17                        */
  (void*) I2C1_IRQHandler,                   /* I2C1                         */
  (void*) I2C2_IRQHandler,                   /* I2C2                         */
  (void*) SPI1_IRQHandler,                   /* SPI1                         */
  (void*) SPI2_IRQHandler,                   /* SPI2                         */
  (void*) USART1_IRQHandler,                 /* USART1                       */
  (void*) USART2_IRQHandler,                 /* USART2                       */
  (void*) USART3_8_IRQHandler,               /* USART3, USART4, USART5, USART6, USART7, USART8 */
  (void*) CEC_CAN_IRQHandler                 /* CEC and CAN                  */ 
};

extern int main(void);
extern int SystemInit(void);

/* This is called on processor reset to initialize the device and call main() */
void Reset_Handler(void)
{
  uint32_t *pSrc, *pDest;

  /* Initialize the initialized data section */
  pSrc = &_sidata;
  pDest = &_sdata;

  if ((&_sdata != &_edata) && (pSrc != pDest)) {
    for (; pDest < &_edata; pDest++, pSrc++)
      *pDest = *pSrc;
  }

  /* Clear the zero section */
  if ((&_sdata != &_edata) && (pSrc != pDest)) {
    for (pDest = &_sbss; pDest < &_ebss; pDest++)
      *pDest = 0;
  }

  SystemInit();

  main();

  while(1);
}
