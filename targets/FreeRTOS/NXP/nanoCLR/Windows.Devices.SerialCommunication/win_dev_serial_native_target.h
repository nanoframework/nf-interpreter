//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _WIN_DEV_SERIAL_NATIVE_TARGET_H_
#define _WIN_DEV_SERIAL_NATIVE_TARGET_H_

#define LPUART_TX_DMA_CHANNEL 0U

#include <target_windows_devices_serialcommunication_config.h>
#include <win_dev_serial_native.h>

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Checks.h>
#include <corlib_native.h>

#include "fsl_lpuart.h"
#include "fsl_lpuart_edma.h"
#include "fsl_dmamux.h"
#include "fsl_edma.h"

#include "stream_buffer.h"
#include "semphr.h"

typedef struct {
    size_t bytes_received;
    size_t bytes_to_read;
    uint32_t timeout;
    uint32_t f_timeout;
    uint8_t * out_data;
    uint8_t uartNum;
} uart_data_t;
typedef struct
{
    uint8_t dma_num;

    lpuart_config_t uartCfg;
    edma_config_t edmaCfg;
    
    lpuart_edma_handle_t g_lpuartEdmaHandle;
    edma_handle_t g_lpuartTxEdmaHandle;

    HAL_RingBuffer<uint8_t> TxRingBuffer;
    uint8_t* TxBuffer;
    uint16_t TxOngoingCount;
    uint8_t WatchChar;
    SemaphoreHandle_t xRxSemaphore = NULL;
    TaskHandle_t xWriteTaskToNotify = NULL;    
    TaskHandle_t xReadTaskToNotify = NULL;

    StreamBufferHandle_t xReceiveBuffer;
    uint32_t ulNotifiedValue;
    uart_data_t uart_data;
} NF_PAL_UART;

////////////////////////////////////////////
// declaration of the the UART PAL strucs //
////////////////////////////////////////////

/* TODO: This is implementation of one UART devices, need to add the rest of structures or find better way to keep uarts config */

LPUART_Type *const lpuart_bases[] = LPUART_BASE_PTRS;

extern NF_PAL_UART Uart_PAL1;
extern NF_PAL_UART Uart_PAL2;
extern NF_PAL_UART Uart_PAL3;
extern NF_PAL_UART Uart_PAL4;
extern NF_PAL_UART Uart_PAL5;
extern NF_PAL_UART Uart_PAL6;
extern NF_PAL_UART Uart_PAL7;
extern NF_PAL_UART Uart_PAL8;

extern NF_PAL_UART* Uart_PAL[];

#endif  //_WIN_DEV_SERIAL_NATIVE_TARGET_H_
