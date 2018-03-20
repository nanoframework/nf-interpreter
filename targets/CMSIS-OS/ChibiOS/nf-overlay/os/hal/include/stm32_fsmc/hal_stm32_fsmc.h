/*
    ChibiOS/HAL - Copyright (C) 2014 Uladzimir Pylinsky aka barthess

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

// FSMC Driver subsystem low level driver header.

#ifndef HAL_FSMC_H_
#define HAL_FSMC_H_

#if (HAL_USE_FSMC == TRUE)

#include "stm32_registry.h"

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

// map to STM32 HAL defines
#define _MC_Bank1_R_BASE                    FMC_Bank1_R_BASE
#define _MC_Bank1E_R_BASE                   FMC_Bank1E_R_BASE
#define _MC_Bank2_R_BASE                    FMC_Bank2_R_BASE
#define _MC_Bank3_R_BASE                    FMC_Bank3_R_BASE
#define _MC_Bank4_R_BASE                    FMC_Bank4_R_BASE
#define _MC_Bank5_6_R_BASE                  FMC_Bank5_6_R_BASE

#define FMC_NORSRAM_TypeDef                 FMC_Bank1_TypeDef
#define FMC_NAND_TypeDef                    FMC_Bank3_TypeDef
#define FMC_SDRAM_TypeDef                   FMC_Bank5_6_TypeDef


// Base address of banks
#define _MC_Bank1_MAP_BASE                ((uint32_t) 0x60000000)
#define _MC_Bank2_MAP_BASE                ((uint32_t) 0x70000000)
#define _MC_Bank3_MAP_BASE                ((uint32_t) 0x80000000)
#define _MC_Bank4_MAP_BASE                ((uint32_t) 0x90000000)
#define _MC_Bank5_MAP_BASE                ((uint32_t) 0xC0000000)
#define _MC_Bank6_MAP_BASE                ((uint32_t) 0xD0000000)

// Sub-banks of bank1
#define _MC_SUBBANK_OFFSET                (1024 * 1024 * 64)
#define _MC_Bank1_1_MAP                   (_MC_Bank1_MAP_BASE)
#define _MC_Bank1_2_MAP                   (_MC_Bank1_1_MAP + _MC_SUBBANK_OFFSET)
#define _MC_Bank1_3_MAP                   (_MC_Bank1_2_MAP + _MC_SUBBANK_OFFSET)
#define _MC_Bank1_4_MAP                   (_MC_Bank1_3_MAP + _MC_SUBBANK_OFFSET)

// Bank 2 (NAND)
#define _MC_Bank2_MAP_COMMON             (_MC_Bank2_MAP_BASE + 0)
#define _MC_Bank2_MAP_ATTR               (_MC_Bank2_MAP_BASE + 0x8000000)

#define _MC_Bank2_MAP_COMMON_DATA        (_MC_Bank2_MAP_COMMON + 0)
#define _MC_Bank2_MAP_COMMON_CMD         (_MC_Bank2_MAP_COMMON + 0x10000)
#define _MC_Bank2_MAP_COMMON_ADDR        (_MC_Bank2_MAP_COMMON + 0x20000)

#define _MC_Bank2_MAP_ATTR_DATA          (_MC_Bank2_MAP_ATTR + 0)
#define _MC_Bank2_MAP_ATTR_CMD           (_MC_Bank2_MAP_ATTR + 0x10000)
#define _MC_Bank2_MAP_ATTR_ADDR          (_MC_Bank2_MAP_ATTR + 0x20000)

// Bank 3 (NAND)
#define _MC_Bank3_MAP_COMMON             (_MC_Bank3_MAP_BASE + 0)
#define _MC_Bank3_MAP_ATTR               (_MC_Bank3_MAP_BASE + 0x8000000)

#define _MC_Bank3_MAP_COMMON_DATA        (_MC_Bank3_MAP_COMMON + 0)
#define _MC_Bank3_MAP_COMMON_CMD         (_MC_Bank3_MAP_COMMON + 0x10000)
#define _MC_Bank3_MAP_COMMON_ADDR        (_MC_Bank3_MAP_COMMON + 0x20000)

#define _MC_Bank3_MAP_ATTR_DATA          (_MC_Bank3_MAP_ATTR + 0)
#define _MC_Bank3_MAP_ATTR_CMD           (_MC_Bank3_MAP_ATTR + 0x10000)
#define _MC_Bank3_MAP_ATTR_ADDR          (_MC_Bank3_MAP_ATTR + 0x20000)

// Bank 4 (PC card)
#define _MC_Bank4_MAP_COMMON             (_MC_Bank4_MAP_BASE + 0)
#define _MC_Bank4_MAP_ATTR               (_MC_Bank4_MAP_BASE + 0x8000000)
#define _MC_Bank4_MAP_IO                 (_MC_Bank4_MAP_BASE + 0xC000000)


/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

// FSMC driver enable switch.
// If set to TRUE the support for the Flexible Memory Controller is included
#if !defined(STM32_FSMC_USE_FSMC1)
#define STM32_FSMC_USE_FSMC1             FALSE
#endif

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/
#if !STM32_FSMC_USE_FSMC1
#error "FSMC driver activated but no FSMC peripheral assigned"
#endif

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

// Type of a structure representing an FSMC driver.
typedef struct FSMCDriver FSMCDriver;

// Driver state machine possible states.
typedef enum
{
  FSMC_UNINIT = 0,                   /**< Not initialized.                   */
  FSMC_STOP = 1,                     /**< Stopped.                           */
  FSMC_READY = 2,                    /**< Ready.                             */
} FSMCstate_t;

// Structure representing an FSMC driver.
struct FSMCDriver
{
  // Driver state.
  FSMCstate_t               state;

#if STM32_SRAM_USE_FSMC_SRAM1
  FMC_NORSRAM_TypeDef     *sram1;
#endif
#if STM32_SRAM_USE_FSMC_SRAM2
  FMC_NORSRAM_TypeDef     *sram2;
#endif
#if STM32_SRAM_USE_FSMC_SRAM3
  FMC_NORSRAM_TypeDef     *sram3;
#endif
#if STM32_SRAM_USE_FSMC_SRAM4
  FMC_NORSRAM_TypeDef     *sram4;
#endif
#if STM32_NAND_USE_FSMC_NAND1
  FMC_NAND_TypeDef         *nand1;
#endif
#if STM32_NAND_USE_FSMC_NAND2
  FMC_NAND_TypeDef         *nand2;
#endif
#if (defined(STM32F427xx) || defined(STM32F437xx) || \
     defined(STM32F429xx) || defined(STM32F439xx) || \
     defined(STM32F7))
  #if STM32_USE_FSMC_SDRAM
    FMC_SDRAM_TypeDef       *sdram;
  #endif
#endif
};

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

// From STMicroelectronics Cube HAL
// SDRAM Mode definition register defines
#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)

// FMC_SDRAM_Read_Pipe_Delay FMC SDRAM Read Pipe Delay
#define FMC_SDRAM_RPIPE_DELAY_0               ((uint32_t)0x00000000U)
#define FMC_SDRAM_RPIPE_DELAY_1               ((uint32_t)0x00002000U)
#define FMC_SDRAM_RPIPE_DELAY_2               ((uint32_t)0x00004000U)
//#define FSMC_ReadPipe_Delay_Mask            ((uint32_t)0x00006000)

// FMC_SDRAM_Read_Burst FMC SDRAM Read Burst
#define FMC_SDRAM_RBURST_DISABLE              ((uint32_t)0x00000000U)
#define FMC_SDRAM_RBURST_ENABLE               ((uint32_t)0x00001000U)
// #define FSMC_Read_Burst_Mask                ((uint32_t)0x00001000)

// FMC_SDRAM_Clock_Period FMC SDRAM Clock Period
#define FMC_SDRAM_CLOCK_DISABLE               ((uint32_t)0x00000000U)
#define FMC_SDRAM_CLOCK_PERIOD_2              ((uint32_t)0x00000800U)
#define FMC_SDRAM_CLOCK_PERIOD_3              ((uint32_t)0x00000C00)
// #define FSMC_SDClock_Period_Mask            ((uint32_t)0x00000C00)

// FMC_SDRAM_Column_Bits_number FMC SDRAM Column Bits number 
#define FMC_SDRAM_COLUMN_BITS_NUM_8           ((uint32_t)0x00000000U)
#define FMC_SDRAM_COLUMN_BITS_NUM_9           ((uint32_t)0x00000001U)
#define FMC_SDRAM_COLUMN_BITS_NUM_10          ((uint32_t)0x00000002U)
#define FMC_SDRAM_COLUMN_BITS_NUM_11          ((uint32_t)0x00000003U)

// FMC_SDRAM_Row_Bits_number FMC SDRAM Row Bits number
#define FMC_SDRAM_ROW_BITS_NUM_11             ((uint32_t)0x00000000U)
#define FMC_SDRAM_ROW_BITS_NUM_12             ((uint32_t)0x00000004U)
#define FMC_SDRAM_ROW_BITS_NUM_13             ((uint32_t)0x00000008U)

// FMC_SDRAM_Memory_Bus_Width FMC SDRAM Memory Bus Width
#define FMC_SDRAM_MEM_BUS_WIDTH_8             ((uint32_t)0x00000000U)
#define FMC_SDRAM_MEM_BUS_WIDTH_16            ((uint32_t)0x00000010U)
#define FMC_SDRAM_MEM_BUS_WIDTH_32            ((uint32_t)0x00000020U)

// FMC_SDRAM_Internal_Banks_Number FMC SDRAM Internal Banks Number
#define FMC_SDRAM_INTERN_BANKS_NUM_2          ((uint32_t)0x00000000U)
#define FMC_SDRAM_INTERN_BANKS_NUM_4          ((uint32_t)0x00000040U)

// FMC_SDRAM_CAS_Latency FMC SDRAM CAS Latency
#define FMC_SDRAM_CAS_LATENCY_1               ((uint32_t)0x00000080U)
#define FMC_SDRAM_CAS_LATENCY_2               ((uint32_t)0x00000100U)
#define FMC_SDRAM_CAS_LATENCY_3               ((uint32_t)0x00000180)

// FMC_SDRAM_Write_Protection FMC SDRAM Write Protection
#define FMC_SDRAM_WRITE_PROTECTION_DISABLE    ((uint32_t)0x00000000U)
#define FMC_SDRAM_WRITE_PROTECTION_ENABLE     ((uint32_t)0x00000200U)

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#include "fsmc_nand_lld.h"
#include "fsmc_sdram_lld.h"
#include "fsmc_sram_lld.h"

#if STM32_FSMC_USE_FSMC1 && !defined(__DOXYGEN__)
extern FSMCDriver FSMCD1;
#endif

#ifdef __cplusplus
extern "C" {
#endif

  void stm32FsmcInit(void);
  void stm32FsmcStart(FSMCDriver *fsmc);
  void stm32FsmcStop(FSMCDriver *fsmc);

#ifdef __cplusplus
}
#endif

#endif /* HAL_USE_FSMC */

#endif /* HAL_FSMC_H_ */
