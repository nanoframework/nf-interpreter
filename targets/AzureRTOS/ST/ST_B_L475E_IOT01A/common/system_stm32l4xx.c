//
// Copyright (c) .NET Foundation and Contributors
// Copyright (c) 2009-2018 ARM Limited - STMicroelectronics. All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <stm32l4xx.h>

#if !defined(HSE_VALUE)
#define HSE_VALUE ((uint32_t)8000000)
#endif

#if !defined(MSI_VALUE)
#define MSI_VALUE ((uint32_t)4000000)
#endif

#if !defined(HSI_VALUE)
#define HSI_VALUE ((uint32_t)16000000)
#endif

#define VECT_TAB_OFFSET 0x00

uint32_t SystemCoreClock = 4000000;

const uint8_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
const uint8_t APBPrescTable[8] = {0, 0, 0, 0, 1, 2, 3, 4};
const uint32_t MSIRangeTable[12] =
    {100000, 200000, 400000, 800000, 1000000, 2000000, 4000000, 8000000, 16000000, 24000000, 32000000, 48000000};

void SystemInit(void)
{
    // FPU settings
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2)); // set CP10 and CP11 Full Access
#endif
    // Reset the RCC clock configuration to the default reset state
    // Set MSION bit
    RCC->CR |= RCC_CR_MSION;

    // Reset CFGR register
    RCC->CFGR = 0x00000000;

    // Reset HSEON, CSSON , HSION, and PLLON bits
    RCC->CR &= (uint32_t)0xEAF6FFFF;

    // Reset PLLCFGR register
    RCC->PLLCFGR = 0x00001000;

    // Reset HSEBYP bit
    RCC->CR &= (uint32_t)0xFFFBFFFF;

    // Disable all interrupts
    RCC->CIER = 0x00000000;

    // Configure the Vector Table location add offset address
#ifdef VECT_TAB_SRAM
    // Vector Table Relocation in Internal SRAM
    SCB->VTOR = SRAM_BASE | VECT_TAB_OFFSET;
#else
    // Vector Table Relocation in Internal FLASH
    SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET;
#endif
}

void SystemCoreClockUpdate(void)
{
    uint32_t tmp = 0, msirange = 0, pllvco = 0, pllr = 2, pllsource = 0, pllm = 2;

    // Get MSI Range frequency
    if ((RCC->CR & RCC_CR_MSIRGSEL) == RESET)
    {
        // MSISRANGE from RCC_CSR applies
        msirange = (RCC->CSR & RCC_CSR_MSISRANGE) >> 8;
    }
    else
    {
        // MSIRANGE from RCC_CR applies
        msirange = (RCC->CR & RCC_CR_MSIRANGE) >> 4;
    }

    // MSI frequency range in HZ
    msirange = MSIRangeTable[msirange];

    // Get SYSCLK source
    switch (RCC->CFGR & RCC_CFGR_SWS)
    {
            // MSI used as system clock source
        case 0x00:
            SystemCoreClock = msirange;
            break;

            // HSI used as system clock source
        case 0x04:
            SystemCoreClock = HSI_VALUE;
            break;

            // HSE used as system clock source
        case 0x08:
            SystemCoreClock = HSE_VALUE;
            break;

        // PLL used as system clock  source
        case 0x0C:

            pllsource = (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC);
            pllm = ((RCC->PLLCFGR & RCC_PLLCFGR_PLLM) >> 4) + 1;

            switch (pllsource)
            {
                case 0x02: // HSI used as PLL clock source
                    pllvco = (HSI_VALUE / pllm);
                    break;

                case 0x03: // HSE used as PLL clock source
                    pllvco = (HSE_VALUE / pllm);
                    break;

                default: // MSI used as PLL clock source
                    pllvco = (msirange / pllm);
                    break;
            }

            pllvco = pllvco * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 8);
            pllr = (((RCC->PLLCFGR & RCC_PLLCFGR_PLLR) >> 25) + 1) * 2;
            SystemCoreClock = pllvco / pllr;
            break;

        default:
            SystemCoreClock = msirange;
            break;
    }

    // Compute HCLK clock frequency
    // Get HCLK prescaler
    tmp = AHBPrescTable[((RCC->CFGR & RCC_CFGR_HPRE) >> 4)];

    // HCLK clock frequency
    SystemCoreClock >>= tmp;
}
