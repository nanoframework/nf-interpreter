//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef SYS_IO_SER_NATIVE_
#define SYS_IO_SER_NATIVE_

#define UART_INTERRUPT_PRIO 4U

#include "FreeRTOS.h"
#include <nanoHAL.h>
#include <target_windows_devices_serialcommunication_config.h>
#include <sys_io_ser_native.h>

#include "fsl_lpuart.h"

typedef struct
{
    uint8_t uartNum;

    lpuart_config_t uartCfg;
    lpuart_transfer_t xfer;

    uint8_t *TxBuffer;
    uint16_t TxOngoingCount;

    HAL_RingBuffer<uint8_t> RxRingBuffer;
    uint8_t *RxBuffer;
    uint16_t RxBytesToRead;
    uint8_t WatchChar;
    uint8_t NewLineChar;

    TaskHandle_t xRTaskToNotify;
    TaskHandle_t xWTaskToNotify;

} NF_PAL_UART;

#endif // SYS_IO_SER_NATIVE_
