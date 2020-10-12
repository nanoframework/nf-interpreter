//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include "hal.h"
#include "fsmc_sdram_lld.h"


// SDRAM Mode definition register defines
#define FMC_SDCMR_MRD_BURST_LENGTH_1             ((uint16_t)0x0000)
#define FMC_SDCMR_MRD_BURST_LENGTH_2             ((uint16_t)0x0001)
#define FMC_SDCMR_MRD_BURST_LENGTH_4             ((uint16_t)0x0002)
#define FMC_SDCMR_MRD_BURST_LENGTH_8             ((uint16_t)0x0004)
#define FMC_SDCMR_MRD_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define FMC_SDCMR_MRD_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define FMC_SDCMR_MRD_CAS_LATENCY_2              ((uint16_t)0x0020)
#define FMC_SDCMR_MRD_CAS_LATENCY_3              ((uint16_t)0x0030)
#define FMC_SDCMR_MRD_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define FMC_SDCMR_MRD_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define FMC_SDCMR_MRD_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)

// FMC_ReadPipe_Delay
#define FMC_ReadPipe_Delay_0               ((uint32_t)0x00000000)
#define FMC_ReadPipe_Delay_1               ((uint32_t)0x00002000)
#define FMC_ReadPipe_Delay_2               ((uint32_t)0x00004000)
#define FMC_ReadPipe_Delay_Mask            ((uint32_t)0x00006000)

// FMC_Read_Burst
#define FMC_Read_Burst_Disable             ((uint32_t)0x00000000)
#define FMC_Read_Burst_Enable              ((uint32_t)0x00001000)
#define FMC_Read_Burst_Mask                ((uint32_t)0x00001000)

// FMC_SDClock_Period
#define FMC_SDClock_Disable                ((uint32_t)0x00000000)
#define FMC_SDClock_Period_2               ((uint32_t)0x00000800)
#define FMC_SDClock_Period_3               ((uint32_t)0x00000C00)
#define FMC_SDClock_Period_Mask            ((uint32_t)0x00000C00)

// FMC_ColumnBits_Number
#define FMC_ColumnBits_Number_8b           ((uint32_t)0x00000000)
#define FMC_ColumnBits_Number_9b           ((uint32_t)0x00000001)
#define FMC_ColumnBits_Number_10b          ((uint32_t)0x00000002)
#define FMC_ColumnBits_Number_11b          ((uint32_t)0x00000003)

// FMC_RowBits_Number
#define FMC_RowBits_Number_11b             ((uint32_t)0x00000000)
#define FMC_RowBits_Number_12b             ((uint32_t)0x00000004)
#define FMC_RowBits_Number_13b             ((uint32_t)0x00000008)

// FMC_SDMemory_Data_Width
#define FMC_SDMemory_Width_8b                ((uint32_t)0x00000000)
#define FMC_SDMemory_Width_16b               ((uint32_t)0x00000010)
#define FMC_SDMemory_Width_32b               ((uint32_t)0x00000020)

// FMC_InternalBank_Number
#define FMC_InternalBank_Number_2          ((uint32_t)0x00000000)
#define FMC_InternalBank_Number_4          ((uint32_t)0x00000040)

// FMC_CAS_Latency
#define FMC_CAS_Latency_1                  ((uint32_t)0x00000080)
#define FMC_CAS_Latency_2                  ((uint32_t)0x00000100)
#define FMC_CAS_Latency_3                  ((uint32_t)0x00000180)

// FMC_Write_Protection
#define FMC_Write_Protection_Disable       ((uint32_t)0x00000000)
#define FMC_Write_Protection_Enable        ((uint32_t)0x00000200)

#define SDRAM_SIZE       (8 * 1024 * 1024)
#define SDRAM_START      ((void *)FSMC_Bank6_MAP_BASE)


// SDRAM driver configuration structure.
static const SDRAMConfig sdram_cfg = {
  .sdcr = (uint32_t) FMC_ColumnBits_Number_8b |
                     FMC_RowBits_Number_12b |
                     FMC_SDMemory_Width_16b |
                     FMC_InternalBank_Number_4 |
                     FMC_CAS_Latency_3 |
                     FMC_Write_Protection_Disable |
                     FMC_SDClock_Period_2 |
                     FMC_Read_Burst_Enable |
                     FMC_ReadPipe_Delay_1,
  .sdtr = (uint32_t) (2   - 1) | // FMC_LoadToActiveDelay = 2 (TMRD: 2 Clock cycles)
                     (7 <<  4) | // FMC_ExitSelfRefreshDelay = 7 (TXSR: min=70ns (7x11.11ns))
                     (4 <<  8) | // FMC_SelfRefreshTime = 4 (TRAS: min=42ns (4x11.11ns) max=120k (ns))
                     (7 << 12) | // FMC_RowCycleDelay = 7 (TRC:  min=70 (7x11.11ns))
                     (3 << 16) | // FMC_WriteRecoveryTime = 2 (TWR:  min=1+ 7ns (1+1x11.11ns))
                     (2 << 20) | // FMC_RPDelay = 2 (TRP:  20ns => 2x11.11ns)
                     (2 << 24),  // FMC_RCDDelay = 2 (TRCD: 20ns => 2x11.11ns)
 // NRFS = 4-1
  .sdcmr = (3 << 5) | (FMC_SDCMR_MRD_BURST_LENGTH_2 |
                       FMC_SDCMR_MRD_BURST_TYPE_SEQUENTIAL |
                       FMC_SDCMR_MRD_CAS_LATENCY_3 |
                       FMC_SDCMR_MRD_OPERATING_MODE_STANDARD |
                       FMC_SDCMR_MRD_WRITEBURST_MODE_SINGLE) << 9,

  .sdrtr = (uint32_t)(683 << 1),
};

void Target_ExternalMemoryInit()
{
    fsmcSdramInit();
    fsmcSdramStart(&SDRAMD, &sdram_cfg);
}
