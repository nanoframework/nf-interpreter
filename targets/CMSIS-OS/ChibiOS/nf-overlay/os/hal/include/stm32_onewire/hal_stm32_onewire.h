//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef HAL_NF_STM32_ONEWIRE_H
#define HAL_NF_STM32_ONEWIRE_H

#if (HAL_NF_USE_STM32_ONEWIRE == TRUE)

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

// memory commands //
#define WR_SCRPAD                0x0F    /* write scratchpad */
#define RD_SCRPAD                0xAA    /* read scratchpad  */
#define COPY_SCRPAD              0x55    /* copy scratchpad */
#define RD_MEMORY                0xF0    /* read memory */
#define RD_MEMORY_CRC            0xA5    /* read memory with CRC */
#define CLEAR_MEMORY             0x3C    /* clear memory */
#define SKIP_ROM                 0xCC    /* skip ROM */

/* ROM commands */
#define READ_ROM                 0x33    /* read ROM */
#define MATCH_ROM                0x55    /* match ROM */
#define SEARCH_ROM               0xF0    /* search ROM */
#define COND_SEARCH_ROM          0xEC    /* conditional search ROM */

// General command defines
#define READ_MEMORY_PSW_COMMAND  0x69

#define MAX_PORTNUM              16

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @name    Configuration options
 * @{
 */

/**
 * @brief   Enables the @p oneWireAcquireBus() and @p oneWireReleaseBus() APIs.
 * @note    Disabling this option saves both code and data space.
 */
#if !defined(ONEWIRE_USE_MUTUAL_EXCLUSION)
#define ONEWIRE_USE_MUTUAL_EXCLUSION        TRUE
#endif

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

// forward declarations
typedef struct ONEWIREDriver ONEWIREDriver;

/**
 * @brief   Driver state machine possible states.
 */
typedef enum {
  ONEWIRE_UNINIT,                /* Not initialized.                           */
  ONEWIRE_STOP,                  /* Stopped.                                   */
  ONEWIRE_READY,                 /* Ready.                                     */
  ONEWIRE_ACTIVE                 /* Generating random number.                  */
} oneWireState;

#include <onewire_lld.h>

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
  void oneWireInit(void);
  void oneWireStart(void);
  void oneWireStop(void);
  uint8_t oneWireTouchReset(void);
  bool oneWireTouchBit(bool sendbit);
  uint8_t oneWireTouchByte(uint8_t sendbyte);
  uint8_t oneWireWriteByte(uint8_t sendbyte);
  uint8_t oneWireReadByte(void);
  void oneWireSerialNum(uint8_t* serialnum_buf, uint8_t do_read);
  uint8_t oneWireAccess(void);
  uint8_t oneWireBlock(uint8_t doReset, uint8_t* tran_buf, uint8_t tran_len);
  uint8_t oneWireWriteMemory(uint8_t* buf, uint32_t ln, uint32_t adr);
  uint8_t oneWireCopyScratchpad(uint32_t ln, uint32_t adr);
  uint8_t oneWireWriteScratchpad(uint8_t* buf, uint32_t ln, uint32_t adr);
  uint8_t oneWireReadPage(uint32_t start_pg, uint8_t* finalbuf);
  uint8_t oneWireReadPageCRCEE77(uint32_t page, uint8_t *buff);
  bool oneWireFindFirst (bool doReset, bool alarmOnly);
  bool oneWireFindNext (bool doReset, bool alarmOnly);


#if ONEWIRE_USE_MUTUAL_EXCLUSION == TRUE
  void oneWireAcquireModule(void);
  void oneWireReleaseModule(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* HAL_NF_USE_STM32_ONEWIRE */

#endif /* HAL_NF_STM32_ONEWIRE_H_ */
