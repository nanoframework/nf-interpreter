//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_Power.h>
#include <nanoHAL_v2.h>
#include <target_platform.h>
#include <cmsis_os.h>
#include <hal.h>
#include <ch.h>

inline void CPU_Reset(){ NVIC_SystemReset(); };

inline bool CPU_IsSoftRebootSupported() { return true; };

// CPU sleep is not currently implemented in this target
inline void CPU_Sleep(SLEEP_LEVEL_type level, uint64_t wakeEvents) { (void)level; (void)wakeEvents;  };

void CPU_SetPowerMode(PowerLevel_type powerLevel)
{
    switch(powerLevel)
    {
        case PowerLevel__Off:
            // gracefully shutdown everything
            nanoHAL_Initialize_C();

            chSysDisable();

            // set power controll register to power down deep sleep
          #if defined(STM32F0XX) || defined(STM32F1XX) || defined(STM32F2XX) || \
            defined(STM32F3XX) ||defined(STM32F4XX) || defined(STM32L0XX) || defined(STM32L1XX)

            SET_BIT(PWR->CR, PWR_CR_PDDS);

          #endif

          #if defined(STM32F7XX) || defined(STM32H7XX) || defined(STM32L4XX)

            SET_BIT(PWR->CR1, PWR_CR1_PDDS);

          #endif

            // set SLEEPDEEP bit of Cortex SCR
            SET_BIT(SCB->SCR, ((uint32_t)SCB_SCR_SLEEPDEEP_Msk));
        
            // wait for interrupt, and the execution dies here
            __WFI();

            break;

        default:
            // all the other power modes are unsupported here
            break;
    }
}
