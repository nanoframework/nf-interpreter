//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2016 STMicroelectronics.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef ONEWIRE_LLD_H
#define ONEWIRE_LLD_H

#if (HAL_NF_USE_STM32_ONEWIRE == TRUE)

#include "hal_uart_lld.h"

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

//character to send on the UART to mimic 1-Wire bus signals
#define IWIRE_WR0           0x00    // 1-Wire write 0 time slot
#define IWIRE_WR1           0xFF    // 1-Wire write 1 time slot
#define IWIRE_RD            0xFF    // 1-Wire read time slot

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/


/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/


/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/**
 * @brief   Structure representing an ONEWIRE driver.
 */
typedef struct ONEWIREDriver {
    // UART to use for 1-Wire comm
    UARTDriver*             UartDriver;

    // UART config struct
    UARTConfig              UartConfig;

    // Driver state.
    oneWireState            State;

    // TX buffer
    uint8_t*                TxBuffer;

    // RX buffer
    uint8_t*                 RxBuffer;

    // semaphore for Tx operation completed
    binary_semaphore_t      TxCompleted;
    
    // semaphore for Rx operation completed
    binary_semaphore_t      RxCompleted;


#if ONEWIRE_USE_MUTUAL_EXCLUSION
    // Mutex protecting the peripheral
    mutex_t                 Lock;
#endif // ONEWIRE_USE_MUTUAL_EXCLUSION

} ONEWIREDriver;

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
  void oneWire_lld_init(void);
  void oneWire_lld_start(void);
  void oneWire_lld_stop(void);
  void oneWire_lld_UART9600(void); 
  void oneWire_lld_UART115200(void);
  uint8_t oneWire_lld_TouchReset(void);
  bool oneWire_lld_TouchBit(bool sendbit);
  uint8_t oneWire_lld_TouchByte(uint8_t sendbyte);

#if (ONEWIRE_USE_MUTUAL_EXCLUSION == TRUE)

  void oneWire_lld_aquire();
  void oneWire_lld_release();

#endif

  void ConfigPins_UART1(void);
  void ConfigPins_UART2(void);
  void ConfigPins_UART3(void);
  void ConfigPins_UART4(void);
  void ConfigPins_UART5(void);
  void ConfigPins_UART6(void);
  void ConfigPins_UART7(void);
  void ConfigPins_UART8(void);

  /////////////////////////////////////
  // UART Tx buffers                 //
  // these live in the target folder //
  /////////////////////////////////////
  extern uint8_t Uart1_TxBuffer[];
  extern uint8_t Uart2_TxBuffer[];
  extern uint8_t Uart3_TxBuffer[];
  extern uint8_t Uart4_TxBuffer[];
  extern uint8_t Uart5_TxBuffer[];
  extern uint8_t Uart6_TxBuffer[];
  extern uint8_t Uart7_TxBuffer[];
  extern uint8_t Uart8_TxBuffer[];

  /////////////////////////////////////
  // UART Rx buffers                 //
  // these live in the target folder //
  /////////////////////////////////////
  extern uint8_t Uart1_RxBuffer[];
  extern uint8_t Uart2_RxBuffer[];
  extern uint8_t Uart3_RxBuffer[];
  extern uint8_t Uart4_RxBuffer[];
  extern uint8_t Uart5_RxBuffer[];
  extern uint8_t Uart6_RxBuffer[];
  extern uint8_t Uart7_RxBuffer[];
  extern uint8_t Uart8_RxBuffer[];

#ifdef __cplusplus
}
#endif

#endif /* HAL_NF_USE_STM32_ONEWIRE */

#endif /* ONEWIRE_LLD_H */
