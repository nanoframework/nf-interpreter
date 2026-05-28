/*
 * Copyright (c) .NET Foundation and Contributors
 * See LICENSE file in the project root for full license information.
 */

#ifndef HAL_UART_LLD_H
#define HAL_UART_LLD_H

#if (HAL_USE_UART == TRUE) || defined(__DOXYGEN__)

#include <stddef.h>
#include <stdint.h>

typedef uint32_t uartflags_t;
typedef struct hal_uart_driver UARTDriver;

typedef void (*uartcb_t)(UARTDriver *uartp);
typedef void (*uartccb_t)(UARTDriver *uartp, uint16_t c);
typedef void (*uartecb_t)(UARTDriver *uartp, uartflags_t e);

typedef struct hal_uart_config
{
    uartcb_t txend1_cb;
    uartcb_t txend2_cb;
    uartcb_t rxend_cb;
    uartccb_t rxchar_cb;
    uartecb_t rxerr_cb;
    uartcb_t timeout_cb;
    uartcb_t rx_cm_cb;

    uint32_t baud;
    uint32_t UARTLCR_H;
    uint32_t UARTCR;
    uint32_t UARTIFLS;
    uint32_t UARTDMACR;
} UARTConfig;

#define uart_lld_driver_fields                                                                                            \
    UART_TypeDef *uart;                                                                                                   \
    uint16_t rxbuf;                                                                                                       \
    uint8_t *rxbufp;                                                                                                      \
    size_t rxsize;                                                                                                        \
    size_t rxidx;

struct hal_uart_driver
{
    uartstate_t state;
    uarttxstate_t txstate;
    uartrxstate_t rxstate;
    const UARTConfig *config;
#if (UART_USE_WAIT == TRUE) || defined(__DOXYGEN__)
    bool early;
    thread_reference_t threadrx;
    thread_reference_t threadtx;
#endif
#if (UART_USE_MUTUAL_EXCLUSION == TRUE) || defined(__DOXYGEN__)
    mutex_t mutex;
#endif
    uart_lld_driver_fields
};

#if !defined(RP_UART_USE_UART0) || defined(__DOXYGEN__)
#define RP_UART_USE_UART0 FALSE
#endif

#if !defined(RP_UART_USE_UART1) || defined(__DOXYGEN__)
#define RP_UART_USE_UART1 FALSE
#endif

#if (RP_UART_USE_UART0 == TRUE) && !defined(__DOXYGEN__)
extern UARTDriver UARTD0;
#endif

#if (RP_UART_USE_UART1 == TRUE) && !defined(__DOXYGEN__)
extern UARTDriver UARTD1;
#endif

#ifdef __cplusplus
extern "C"
{
#endif
    void uart_lld_init(void);
    void uart_lld_start(UARTDriver *uartp);
    void uart_lld_stop(UARTDriver *uartp);
    void uart_lld_start_send(UARTDriver *uartp, size_t n, const void *txbuf);
    size_t uart_lld_stop_send(UARTDriver *uartp);
    void uart_lld_start_receive(UARTDriver *uartp, size_t n, void *rxbuf);
    size_t uart_lld_stop_receive(UARTDriver *uartp);
#ifdef __cplusplus
}
#endif

#endif // HAL_USE_UART

#endif // HAL_UART_LLD_H