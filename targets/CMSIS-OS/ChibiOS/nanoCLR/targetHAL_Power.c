//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>
#include <hal_nf_community.h>
#include <nanoHAL_Power.h>
#include <nanoHAL_v2.h>
#include <target_platform.h>
#include <cmsis_os.h>

#if (HAL_USE_FSMC == TRUE)
#include <fsmc_sdram_lld.h>
#include <fsmc_sram_lld.h>
#endif

uint32_t WakeupReasonStore;

inline void CPU_Reset()
{ 
    NVIC_SystemReset(); 
};

inline bool CPU_IsSoftRebootSupported()
{ 
    return true; 
};

// CPU sleep is not currently implemented in this target
inline void CPU_Sleep(SLEEP_LEVEL_type level, uint64_t wakeEvents)
{ 
    (void)level;
    (void)wakeEvents;

};

void CPU_SetPowerMode(PowerLevel_type powerLevel)
{
    // default to false
    bool success = false;

    switch(powerLevel)
    {
        case PowerLevel__Off:
            // stop watchdog
            wdgStop(&WDGD1);

            #if (HAL_USE_FSMC == TRUE)
                // shutdown memory
                #if (STM32_USE_FSMC_SDRAM == TRUE)
				  fsmcSdramStop(&SDRAMD);
			    #endif
			    #if (STM32_USE_FSMC_SRAM == TRUE)
				    #if (STM32_SRAM_USE_FSMC_SRAM1 == TRUE)
					    fsmcSramStop(&SRAMD1);
				    #endif
				    #if (STM32_SRAM_USE_FSMC_SRAM2 == TRUE)
					    fsmcSramStop(&SRAMD2);
				    #endif
				    #if (STM32_SRAM_USE_FSMC_SRAM3 == TRUE)
					    fsmcSramStop(&SRAMD3);
				    #endif
				    #if (STM32_SRAM_USE_FSMC_SRAM4 == TRUE)
					    fsmcSramStop(&SRAMD4);
				    #endif
			    #endif
            #endif

            // gracefully shutdown everything
            nanoHAL_Uninitialize_C();

            chSysDisable();

            /////////////////////////////////////////////////////////////////////////
            // stop the idependent watchdog, for series where the option is available
          #if defined(STM32L4XX)

            (void)success;
            // TODO FIXME this code needs to follow the same workflow as the STM32F7
            CLEAR_BIT(FLASH->OPTR, FLASH_OPTR_IWDG_STDBY);

          #elif defined(STM32F7XX)
            
            // only need to change this option bit if not already done
            if((FLASH->OPTCR & FLASH_OPTCR_IWDG_STDBY))
            {
                // developer notes:
                // follow workflow recommended @ 3.4.2 Option bytes programming (from programming manual)
                // Authorizes the Option Byte register programming
                FLASH->OPTKEYR = FLASH_OPT_KEY1;
                FLASH->OPTKEYR = FLASH_OPT_KEY2;

                // wait 500ms for any flash operation to be completed
                success = FLASH_WaitForLastOperation(500);

                if(success)
                {
                    // write option value (clear the FLASH_OPTCR_IWDG_STDBY)
                    CLEAR_BIT(FLASH->OPTCR, FLASH_OPTCR_IWDG_STDBY);
                    
                    // set the option start bit
                    FLASH->OPTCR |= FLASH_OPTCR_OPTSTRT;

                    // Data synchronous Barrier, forcing the CPU to respect the sequence of instruction without optimization
                    __DSB();

                    // wait 100ms for the flash operation to be completed
                    success = FLASH_WaitForLastOperation(100);
                }

                // Set the OPTLOCK Bit to lock the FLASH Option Byte Registers access
                FLASH->OPTCR |= FLASH_OPTCR_OPTLOCK;
            }
            (void)success;

          #elif defined(STM32H7XX)

            (void)success;
            // TODO FIXME this code needs to follow the same workflow as the STM32F7
            CLEAR_BIT(FLASH->OPTSR, FLASH_OPTSR_FZ_IWDG_SDBY);

          #else
            (void)success;
          #endif

            ///////////////////////////////////////////////////////
            // set alarm interrupt enable
            // set power control register to: power down deep sleep
            ///////////////////////////////////////////////////////

          #if defined(STM32F0XX) || defined(STM32F1XX) || defined(STM32F2XX) || \
            defined(STM32F3XX) ||defined(STM32F4XX) || defined(STM32L0XX) || defined(STM32L1XX)
            SET_BIT(RTC->CR, RTC_CR_ALRAIE);
            SET_BIT(PWR->CR, PWR_CR_PDDS);
          #endif

          #if defined(STM32F7XX)
            
            //////////////////////////////////////////////////////////////////////////////////////////////////////
            // workaround recommended in section 2.2.2 at STM32F77xxx errata document (DM00257543 - ES0334 Rev 5) //
            PWR->CSR1 |= PWR_CSR1_EIWUP;
            //////////////////////////////////////////////////////////////////////////////////////////////////////

            SET_BIT(PWR->CR1, PWR_CR1_PDDS);

          #endif

          #if defined(STM32H7XX)
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
