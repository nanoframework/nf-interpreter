/*
 * Copyright (c) .NET Foundation and Contributors
 * See LICENSE file in the project root for full license information.
 */

#include <hal.h>

#if (HAL_USE_UART == TRUE) || defined(__DOXYGEN__)

#define UART_LCRH_CFG_FORBIDDEN (UART_UARTLCR_H_BRK)
#define UART_CR_CFG_FORBIDDEN                                                                                          \
    (UART_UARTCR_RXE | UART_UARTCR_TXE | UART_UARTCR_SIRLP | UART_UARTCR_SIREN | UART_UARTCR_UARTEN)

#if (RP_UART_USE_UART0 == TRUE) || defined(__DOXYGEN__)
UARTDriver UARTD0;
#endif

#if (RP_UART_USE_UART1 == TRUE) || defined(__DOXYGEN__)
UARTDriver UARTD1;
#endif

static const UARTConfig uart_default_config = {
    .txend1_cb = NULL,
    .txend2_cb = NULL,
    .rxend_cb = NULL,
    .rxchar_cb = NULL,
    .rxerr_cb = NULL,
    .timeout_cb = NULL,
    .rx_cm_cb = NULL,
    .baud = 115200,
    .UARTLCR_H = UART_UARTLCR_H_WLEN_8BITS | UART_UARTLCR_H_FEN,
    .UARTCR = 0,
    .UARTIFLS = UART_UARTIFLS_RXIFLSEL_1_2F | UART_UARTIFLS_TXIFLSEL_1_2E,
    .UARTDMACR = 0,
};

static void uart_lld_apply_config(UARTDriver *uartp)
{
    uint32_t div;
    uint32_t idiv;
    uint32_t fdiv;
    uint32_t cr;
    halfreq_t clock;
    const UARTConfig *cfg = (uartp->config == NULL) ? &uart_default_config : uartp->config;

    clock = halClockGetPointX(RP_CLK_PERI);
    osalDbgAssert(clock > 0U, "no clock");

    div = (8U * (uint32_t)clock) / cfg->baud;
    idiv = div >> 7;
    fdiv = ((div & 0x7FU) + 1U) / 2U;

    osalDbgAssert((idiv > 0U) && (idiv <= 0xFFFFU), "invalid baud");

    uartp->uart->UARTIBRD = idiv;
    uartp->uart->UARTFBRD = fdiv;

    cr = cfg->UARTCR & ~UART_CR_CFG_FORBIDDEN;

    uartp->uart->UARTLCR_H = cfg->UARTLCR_H & ~UART_LCRH_CFG_FORBIDDEN;
    uartp->uart->UARTCR = cr;
    uartp->uart->UARTIFLS = cfg->UARTIFLS;
    uartp->uart->UARTDMACR = cfg->UARTDMACR;

    uartp->uart->UARTICR = uartp->uart->UARTRIS;

    // Enable receive, timeout and line error interrupts for callback-driven receive path.
    uartp->uart->UARTIMSC = UART_UARTIMSC_RXIM | UART_UARTIMSC_RTIM | UART_UARTIMSC_OEIM | UART_UARTIMSC_BEIM |
                            UART_UARTIMSC_PEIM | UART_UARTIMSC_FEIM;

    uartp->uart->UARTCR = cr | UART_UARTCR_RXE | UART_UARTCR_TXE | UART_UARTCR_UARTEN;
}

static void uart_lld_irq_handler(UARTDriver *uartp)
{
    uint32_t mis = uartp->uart->UARTMIS;

    // Always clear pending interrupts first.
    uartp->uart->UARTICR = mis;

    // Report line errors.
    if ((mis & (UART_UARTMIS_OEMIS | UART_UARTMIS_BEMIS | UART_UARTMIS_PEMIS | UART_UARTMIS_FEMIS)) != 0U)
    {
        uartflags_t errors = UART_NO_ERROR;

        if ((mis & UART_UARTMIS_OEMIS) != 0U)
        {
            errors |= UART_OVERRUN_ERROR;
        }
        if ((mis & UART_UARTMIS_BEMIS) != 0U)
        {
            errors |= UART_BREAK_DETECTED;
        }
        if ((mis & UART_UARTMIS_PEMIS) != 0U)
        {
            errors |= UART_PARITY_ERROR;
        }
        if ((mis & UART_UARTMIS_FEMIS) != 0U)
        {
            errors |= UART_FRAMING_ERROR;
        }

        _uart_rx_error_isr_code(uartp, errors);
    }

    // Drain RX FIFO and dispatch data either to active receive buffer or rxchar callback.
    while ((uartp->uart->UARTFR & UART_UARTFR_RXFE) == 0U)
    {
        uint8_t c = (uint8_t)uartp->uart->UARTDR;

        if (uartp->rxstate == UART_RX_ACTIVE && uartp->rxbufp != NULL)
        {
            uartp->rxbufp[uartp->rxidx++] = c;

            if (uartp->rxidx >= uartp->rxsize)
            {
                uartp->rxstate = UART_RX_IDLE;
                if (uartp->config != NULL && uartp->config->rxend_cb != NULL)
                {
                    uartp->config->rxend_cb(uartp);
                }
                _uart_wakeup_rx_complete_isr(uartp);
                break;
            }
        }
        else if (uartp->config != NULL && uartp->config->rxchar_cb != NULL)
        {
            uartp->rxbuf = c;
            _uart_rx_idle_code(uartp);
        }
    }
}

#if (RP_UART_USE_UART0 == TRUE) || defined(__DOXYGEN__)
OSAL_IRQ_HANDLER(RP_UART0_IRQ_HANDLER)
{
    OSAL_IRQ_PROLOGUE();
    uart_lld_irq_handler(&UARTD0);
    OSAL_IRQ_EPILOGUE();
}
#endif

#if (RP_UART_USE_UART1 == TRUE) || defined(__DOXYGEN__)
OSAL_IRQ_HANDLER(RP_UART1_IRQ_HANDLER)
{
    OSAL_IRQ_PROLOGUE();
    uart_lld_irq_handler(&UARTD1);
    OSAL_IRQ_EPILOGUE();
}
#endif

void uart_lld_init(void)
{
#if (RP_UART_USE_UART0 == TRUE)
    uartObjectInit(&UARTD0);
    UARTD0.uart = UART0;
    UARTD0.rxbuf = 0;
    UARTD0.rxbufp = NULL;
    UARTD0.rxsize = 0;
    UARTD0.rxidx = 0;
    rp_peripheral_reset(RESETS_ALLREG_UART0);
#endif

#if (RP_UART_USE_UART1 == TRUE)
    uartObjectInit(&UARTD1);
    UARTD1.uart = UART1;
    UARTD1.rxbuf = 0;
    UARTD1.rxbufp = NULL;
    UARTD1.rxsize = 0;
    UARTD1.rxidx = 0;
    rp_peripheral_reset(RESETS_ALLREG_UART1);
#endif
}

void uart_lld_start(UARTDriver *uartp)
{
    if (uartp->state == UART_STOP)
    {
#if (RP_UART_USE_UART0 == TRUE)
        if (uartp == &UARTD0)
        {
            rp_peripheral_unreset(RESETS_ALLREG_UART0);
            nvicEnableVector(RP_UART0_IRQ_NUMBER, RP_IRQ_UART0_PRIORITY);
        }
#endif
#if (RP_UART_USE_UART1 == TRUE)
        if (uartp == &UARTD1)
        {
            rp_peripheral_unreset(RESETS_ALLREG_UART1);
            nvicEnableVector(RP_UART1_IRQ_NUMBER, RP_IRQ_UART1_PRIORITY);
        }
#endif
    }

    uart_lld_apply_config(uartp);
}

void uart_lld_stop(UARTDriver *uartp)
{
    if (uartp->state == UART_READY)
    {
        uartp->uart->UARTIMSC = 0;
        uartp->uart->UARTCR = 0;

#if (RP_UART_USE_UART0 == TRUE)
        if (uartp == &UARTD0)
        {
            nvicDisableVector(RP_UART0_IRQ_NUMBER);
            rp_peripheral_reset(RESETS_ALLREG_UART0);
        }
#endif
#if (RP_UART_USE_UART1 == TRUE)
        if (uartp == &UARTD1)
        {
            nvicDisableVector(RP_UART1_IRQ_NUMBER);
            rp_peripheral_reset(RESETS_ALLREG_UART1);
        }
#endif
    }

    uartp->rxbuf = 0;
    uartp->rxbufp = NULL;
    uartp->rxsize = 0;
    uartp->rxidx = 0;
}

void uart_lld_start_send(UARTDriver *uartp, size_t n, const void *txbuf)
{
    const uint8_t *buffer = (const uint8_t *)txbuf;

    for (size_t i = 0; i < n; i++)
    {
        while ((uartp->uart->UARTFR & UART_UARTFR_TXFF) != 0U)
            ;
        uartp->uart->UARTDR = buffer[i];
    }

    while ((uartp->uart->UARTFR & UART_UARTFR_BUSY) != 0U)
        ;

    _uart_tx1_isr_code(uartp);
    _uart_tx2_isr_code(uartp);
}

size_t uart_lld_stop_send(UARTDriver *uartp)
{
    (void)uartp;
    return 0;
}

void uart_lld_start_receive(UARTDriver *uartp, size_t n, void *rxbuf)
{
    uartp->rxbufp = (uint8_t *)rxbuf;
    uartp->rxsize = n;
    uartp->rxidx = 0;
}

size_t uart_lld_stop_receive(UARTDriver *uartp)
{
    size_t remaining = 0;

    if (uartp->rxsize > uartp->rxidx)
    {
        remaining = uartp->rxsize - uartp->rxidx;
    }

    uartp->rxbuf = 0;
    uartp->rxbufp = NULL;
    uartp->rxsize = 0;
    uartp->rxidx = 0;

    return remaining;
}

#endif // HAL_USE_UART