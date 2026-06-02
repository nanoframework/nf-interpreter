/*
    ChibiOS - Copyright (C) 2006-2026 Giovanni Di Sirio.

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

/**
 * @file    UARTv1/hal_uart_lld.c
 * @brief   RP UART subsystem low level driver source.
 *
 * @addtogroup UART
 * @{
 */

#include "hal.h"

#if (HAL_USE_UART == TRUE) || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

#define UART_LCRH_CFG_FORBIDDEN     (UART_UARTLCR_H_BRK)
#define UART_CR_CFG_FORBIDDEN       (UART_UARTCR_RXE    |                   \
                                     UART_UARTCR_TXE    |                   \
                                     UART_UARTCR_SIRLP  |                   \
                                     UART_UARTCR_SIREN  |                   \
                                     UART_UARTCR_UARTEN)

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/**
 * @brief   UART0 driver identifier.
 */
#if (RP_UART_USE_UART0 == TRUE) || defined(__DOXYGEN__)
UARTDriver UARTD0;
#endif

/**
 * @brief   UART1 driver identifier.
 */
#if (RP_UART_USE_UART1 == TRUE) || defined(__DOXYGEN__)
UARTDriver UARTD1;
#endif

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/**
 * @brief   Driver default configuration.
 * @note    In this implementation it is: 115200-8-N-1, FIFOs enabled.
 */
static const UARTConfig default_config = {
  .txend1_cb  = NULL,
  .txend2_cb  = NULL,
  .rxend_cb   = NULL,
  .rxchar_cb  = NULL,
  .rxerr_cb   = NULL,
  .timeout_cb = NULL,
  .baud       = 115200,
  .UARTLCR_H  = UART_UARTLCR_H_WLEN_8BITS | UART_UARTLCR_H_FEN,
  .UARTCR     = 0U,
  .UARTIFLS   = UART_UARTIFLS_RXIFLSEL_1_2F | UART_UARTIFLS_TXIFLSEL_1_2E,
  .UARTDMACR  = 0U
};

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/**
 * @brief   Puts the receiver in the UART_RX_IDLE state, enabling
 *          receive interrupts for idle character reception.
 *
 * @param[in] uartp     pointer to the @p UARTDriver object
 */
static void uart_enter_rx_idle_loop(UARTDriver *uartp) {

  uartp->uart->UARTIMSC |= UART_UARTIMSC_RXIM | UART_UARTIMSC_RTIM;
}

/**
 * @brief   UART initialization.
 * @details This function must be invoked with interrupts disabled.
 *
 * @param[in] uartp     pointer to the @p UARTDriver object
 */
static void uart_start(UARTDriver *uartp) {
  uint32_t div, idiv, fdiv, cr;
  halfreq_t clock;
  const UARTConfig *cfgp = uartp->config;

  osalDbgAssert(cfgp->baud > 0U, "invalid baud rate");

  clock = halClockGetPointX(RP_CLK_PERI);
  osalDbgAssert(clock > 0U, "no clock");

  div  = (8U * (uint32_t)clock) / cfgp->baud;
  idiv = div >> 7;
  fdiv = ((div & 0x7FU) + 1U) / 2U;

  osalDbgAssert((idiv > 0U) && (idiv <= 0xFFFFU), "invalid baud rate");

  uartp->uart->UARTIBRD  = idiv;
  uartp->uart->UARTFBRD  = fdiv;

  cr = cfgp->UARTCR & ~UART_CR_CFG_FORBIDDEN;

  /* Registers settings, the LCR_H write also latches dividers values.*/
  uartp->uart->UARTLCR_H = cfgp->UARTLCR_H & ~UART_LCRH_CFG_FORBIDDEN;
  uartp->uart->UARTCR    = cr;
  uartp->uart->UARTIFLS  = cfgp->UARTIFLS;
  uartp->uart->UARTDMACR = cfgp->UARTDMACR;

  /* Clearing pending interrupts.*/
  uartp->uart->UARTICR   = uartp->uart->UARTRIS;

  /* Enable RX, timeout, and error interrupts. TX interrupt is enabled
     on demand by uart_lld_start_send().*/
  uartp->uart->UARTIMSC  = UART_UARTIMSC_RXIM  | UART_UARTIMSC_RTIM |
                            UART_UARTIMSC_OEIM  | UART_UARTIMSC_BEIM |
                            UART_UARTIMSC_PEIM  | UART_UARTIMSC_FEIM;

  /* Enabling the peripheral.*/
  uartp->uart->UARTCR    = cr | UART_UARTCR_RXE |
                            UART_UARTCR_TXE | UART_UARTCR_UARTEN;
}

/**
 * @brief   UART de-initialization.
 *
 * @param[in] uartp     pointer to the @p UARTDriver object
 */
static void uart_stop(UARTDriver *uartp) {

  uartp->uart->UARTIMSC = 0U;
  uartp->uart->UARTCR   = 0U;
}

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

#if (RP_UART_USE_UART0 == TRUE) || defined(__DOXYGEN__)
/**
 * @brief   UART0 IRQ handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(RP_UART0_IRQ_HANDLER) {

  OSAL_IRQ_PROLOGUE();
  uart_lld_serve_interrupt(&UARTD0);
  OSAL_IRQ_EPILOGUE();
}
#endif

#if (RP_UART_USE_UART1 == TRUE) || defined(__DOXYGEN__)
/**
 * @brief   UART1 IRQ handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(RP_UART1_IRQ_HANDLER) {

  OSAL_IRQ_PROLOGUE();
  uart_lld_serve_interrupt(&UARTD1);
  OSAL_IRQ_EPILOGUE();
}
#endif

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Low level UART driver initialization.
 *
 * @notapi
 */
void uart_lld_init(void) {

#if RP_UART_USE_UART0 == TRUE
  uartObjectInit(&UARTD0);
  UARTD0.uart = UART0;
  rp_peripheral_reset(RESETS_ALLREG_UART0);
#endif

#if RP_UART_USE_UART1 == TRUE
  uartObjectInit(&UARTD1);
  UARTD1.uart = UART1;
  rp_peripheral_reset(RESETS_ALLREG_UART1);
#endif
}

/**
 * @brief   Configures and activates the UART peripheral.
 *
 * @param[in] uartp     pointer to the @p UARTDriver object
 *
 * @notapi
 */
void uart_lld_start(UARTDriver *uartp) {

  /* Using the default configuration if the application passed a
     NULL pointer.*/
  if (uartp->config == NULL) {
    uartp->config = &default_config;
  }

  if (uartp->state == UART_STOP) {
    /* Enables the peripheral.*/
#if RP_UART_USE_UART0 == TRUE
    if (uartp == &UARTD0) {
      rp_peripheral_unreset(RESETS_ALLREG_UART0);
      nvicEnableVector(RP_UART0_IRQ_NUMBER, RP_IRQ_UART0_PRIORITY);
    }
#endif
#if RP_UART_USE_UART1 == TRUE
    if (uartp == &UARTD1) {
      rp_peripheral_unreset(RESETS_ALLREG_UART1);
      nvicEnableVector(RP_UART1_IRQ_NUMBER, RP_IRQ_UART1_PRIORITY);
    }
#endif
  }

  uartp->rxstate = UART_RX_IDLE;
  uartp->txstate = UART_TX_IDLE;
  uart_start(uartp);
}

/**
 * @brief   Deactivates the UART peripheral.
 *
 * @param[in] uartp     pointer to the @p UARTDriver object
 *
 * @notapi
 */
void uart_lld_stop(UARTDriver *uartp) {

  if (uartp->state == UART_READY) {
    uart_stop(uartp);

#if RP_UART_USE_UART0 == TRUE
    if (uartp == &UARTD0) {
      nvicDisableVector(RP_UART0_IRQ_NUMBER);
      rp_peripheral_reset(RESETS_ALLREG_UART0);
    }
#endif
#if RP_UART_USE_UART1 == TRUE
    if (uartp == &UARTD1) {
      nvicDisableVector(RP_UART1_IRQ_NUMBER);
      rp_peripheral_reset(RESETS_ALLREG_UART1);
    }
#endif
  }
}

/**
 * @brief   Starts a transmission on the UART peripheral.
 * @note    The buffers are organized as uint8_t arrays.
 *
 * @param[in] uartp     pointer to the @p UARTDriver object
 * @param[in] n         number of data frames to send
 * @param[in] txbuf     the pointer to the transmit buffer
 *
 * @notapi
 */
void uart_lld_start_send(UARTDriver *uartp, size_t n, const void *txbuf) {

  uartp->txbufp = (const uint8_t *)txbuf;
  uartp->txsize = n;
  uartp->txidx  = 0U;

  /* Fill the TX FIFO as much as possible right now.*/
  while ((uartp->txidx < uartp->txsize) &&
         ((uartp->uart->UARTFR & UART_UARTFR_TXFF) == 0U)) {
    uartp->uart->UARTDR = uartp->txbufp[uartp->txidx++];
  }

  /* Enable TX interrupt to continue draining the buffer from ISR.*/
  uartp->uart->UARTIMSC |= UART_UARTIMSC_TXIM;
}

/**
 * @brief   Stops any ongoing transmission.
 * @note    Stopping a transmission also suppresses the transmission callbacks.
 *
 * @param[in] uartp     pointer to the @p UARTDriver object
 *
 * @return              The number of data frames not transmitted by the
 *                      stopped transmit operation.
 *
 * @notapi
 */
size_t uart_lld_stop_send(UARTDriver *uartp) {
  size_t remaining;

  /* Disable TX interrupt.*/
  uartp->uart->UARTIMSC &= ~UART_UARTIMSC_TXIM;

  remaining = uartp->txsize - uartp->txidx;
  uartp->txbufp = NULL;
  uartp->txsize = 0U;
  uartp->txidx  = 0U;

  return remaining;
}

/**
 * @brief   Starts a receive operation on the UART peripheral.
 * @note    The buffers are organized as uint8_t arrays.
 *
 * @param[in] uartp     pointer to the @p UARTDriver object
 * @param[in] n         number of data frames to receive
 * @param[out] rxbuf    the pointer to the receive buffer
 *
 * @notapi
 */
void uart_lld_start_receive(UARTDriver *uartp, size_t n, void *rxbuf) {

  uartp->rxbufp = (uint8_t *)rxbuf;
  uartp->rxsize = n;
  uartp->rxidx  = 0U;
}

/**
 * @brief   Stops any ongoing receive operation.
 * @note    Stopping a receive operation also suppresses the receive callbacks.
 *
 * @param[in] uartp     pointer to the @p UARTDriver object
 *
 * @return              The number of data frames not received by the
 *                      stopped receive operation.
 *
 * @notapi
 */
size_t uart_lld_stop_receive(UARTDriver *uartp) {
  size_t remaining;

  if (uartp->rxsize > uartp->rxidx) {
    remaining = uartp->rxsize - uartp->rxidx;
  }
  else {
    remaining = 0U;
  }

  uartp->rxbufp = NULL;
  uartp->rxsize = 0U;
  uartp->rxidx  = 0U;

  /* Re-enter idle loop.*/
  uart_enter_rx_idle_loop(uartp);

  return remaining;
}

/**
 * @brief   Common IRQ handler.
 * @note    This is a public function because it can be called from
 *          shared interrupt handlers.
 *
 * @param[in] uartp     pointer to the @p UARTDriver object
 *
 * @notapi
 */
void uart_lld_serve_interrupt(UARTDriver *uartp) {
  uint32_t mis;

  mis = uartp->uart->UARTMIS;

  /* Clearing all pending interrupts.*/
  uartp->uart->UARTICR = mis;

  /* Error interrupts.*/
  if ((mis & (UART_UARTMIS_OEMIS | UART_UARTMIS_BEMIS |
              UART_UARTMIS_PEMIS | UART_UARTMIS_FEMIS)) != 0U) {
    uartflags_t errors = UART_NO_ERROR;

    if ((mis & UART_UARTMIS_OEMIS) != 0U) {
      errors |= UART_OVERRUN_ERROR;
    }
    if ((mis & UART_UARTMIS_BEMIS) != 0U) {
      errors |= UART_BREAK_DETECTED;
    }
    if ((mis & UART_UARTMIS_PEMIS) != 0U) {
      errors |= UART_PARITY_ERROR;
    }
    if ((mis & UART_UARTMIS_FEMIS) != 0U) {
      errors |= UART_FRAMING_ERROR;
    }

    _uart_rx_error_isr_code(uartp, errors);
  }

  /* TX FIFO interrupt — continue draining the TX buffer.*/
  if ((mis & UART_UARTMIS_TXMIS) != 0U) {

    /* Feed more data into the TX FIFO.*/
    while ((uartp->txidx < uartp->txsize) &&
           ((uartp->uart->UARTFR & UART_UARTFR_TXFF) == 0U)) {
      uartp->uart->UARTDR = uartp->txbufp[uartp->txidx++];
    }

    /* All data has been written to the FIFO.*/
    if (uartp->txidx >= uartp->txsize) {

      /* Disable TX interrupt.*/
      uartp->uart->UARTIMSC &= ~UART_UARTIMSC_TXIM;

      /* Buffer transmit complete callback.*/
      _uart_tx1_isr_code(uartp);

      /* Physical end of transmission — wait for BUSY to clear would
         require polling; signal tx2 immediately as the FIFO has been
         fully loaded. For true physical-end detection a separate
         mechanism would be needed.*/
      _uart_tx2_isr_code(uartp);
    }
  }

  /* Receive timeout interrupt.*/
  if ((mis & UART_UARTMIS_RTMIS) != 0U) {
    if (uartp->rxstate == UART_RX_IDLE) {
      _uart_timeout_isr_code(uartp);
    }
  }

  /* RX FIFO interrupt or receive timeout — drain available data.*/
  if ((mis & (UART_UARTMIS_RXMIS | UART_UARTMIS_RTMIS)) != 0U) {

    while ((uartp->uart->UARTFR & UART_UARTFR_RXFE) == 0U) {
      uint16_t c = (uint16_t)(uartp->uart->UARTDR & 0xFFU);

      if (uartp->rxstate == UART_RX_ACTIVE) {
        /* Store into active receive buffer.*/
        uartp->rxbufp[uartp->rxidx++] = (uint8_t)c;

        if (uartp->rxidx >= uartp->rxsize) {
          /* Receive buffer complete, uses the standard macro which
             handles state transition, idle loop re-entry, callback,
             and thread wakeup.*/
          _uart_rx_complete_isr_code(uartp);
          break;
        }
      }
      else {
        /* Idle state, dispatch single character.*/
        uartp->rxbuf = c;
        _uart_rx_idle_code(uartp);
      }
    }
  }
}

#endif /* HAL_USE_UART */

/** @} */