//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>
#include <targetHAL_Watchdog.h>
#include <target_common.h>

#if (HAL_USE_WDG == TRUE) 

//Watchdog configuration structure required by ChibiOS
#if (STM32_IWDG_IS_WINDOWED == TRUE)
    static WDGConfig wdgConfig = {STM32_IWDG_PR_256, STM32_IWDG_RL(0xFFF), 0x00}; //default is max
#else
    static WDGConfig wdgConfig = {STM32_IWDG_PR_256, STM32_IWDG_RL(0xFFF)}; //default is max
#endif

/**
* Method to initialize the watchdog. In ChibiOS, the watchdog sub-system is 
* initialized when halInit() is called. In this method, we perform 
* further specific initializations and also start the watchdog
*/
void Watchdog_Init()
{
    //setup required watchdog timeout...(for IWATCHDOG_TIMEOUT_MILLIS)
    /*
        There are minor variations in STM32 LSI clock across models (e.g STM32F091 has 40KHz LSI
        and STM32F401 has 32 KHz). Also, across the same model, due to manufacturing differences
        the LSI clock has a minimum and maximum range. Therefore, the timeout is always an approximation
        and not exact
    */        
    uint32_t ticksForTimeout=0;
    uint16_t prescaler[] = {STM32_IWDG_PR_4,STM32_IWDG_PR_8,STM32_IWDG_PR_16,
                            STM32_IWDG_PR_32,STM32_IWDG_PR_64,STM32_IWDG_PR_128,
                            STM32_IWDG_PR_256};    
    uint8_t length = (sizeof(prescaler)/sizeof(prescaler[0]));
    uint16_t dividor= 4;
    for(uint8_t index=0; index < length;index++)
    {
        dividor = 4 << prescaler[index];        
        ticksForTimeout = (IWATCHDOG_CLK_TYPICAL / dividor) * (IWATCHDOG_TIMEOUT_MILLIS/1000) - 1;
        
        if(ticksForTimeout <= 0xFFF /*The max reload register value*/) 
        {
            wdgConfig.pr =   STM32_IWDG_PR_64; //   prescaler[index];
            wdgConfig.rlr = STM32_IWDG_RL(1000);  //ticksForTimeout;
            break;
        }
    }  
    wdgStart(&WDGD1,&wdgConfig);   
}

/**
 * Reset the watchdog. If not done within the timout period,
 * the watchdog will trigger and MCU will reset
 */ 
void Watchdog_Reset()
{        
    wdgReset(&WDGD1);
}

#endif
