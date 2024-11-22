//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>
#include <stm32_registry.h>
#include <hal_nf_community.h>

extern void Target_ExternalMemoryConfigMPU();

// SRAM1 base address
#define SRAM1_SIZE_128K  (1UL << 16) // 2^17 bytes
#define MPU_REGION_SRAM1 MPU_REGION_1

void Target_ConfigNonCacheableMemory()
{
    // region
    MPU->RNR = MPU_REGION_SRAM1;

    // base address
    MPU->RBAR = SRAM1_BASE;

    // size and other configs
    MPU->RASR =
        ((uint32_t)MPU_RASR_ATTR_AP_RW_RW | MPU_RASR_ATTR_NON_CACHEABLE | MPU_RASR_ATTR_S | MPU_RASR_SIZE_128K |
         MPU_RASR_ENABLE);
}

void Target_ConfigMPU()
{
    // disable MPU
    HAL_MPU_Disable();

    // config MPU for external memory
    Target_ExternalMemoryConfigMPU();

    // config MPU for non cacheable memory
    Target_ConfigNonCacheableMemory();

    // enable MPU
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}
