//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef WIN_DEV_SERIAL_NATIVE_TARGET_H
#define WIN_DEV_SERIAL_NATIVE_TARGET_H

#include <win_dev_serial_native.h>

// struct representing the UART
typedef struct
{
    uart_port_t UartNum;
    TaskHandle_t UartEventTask;
    QueueHandle_t UartEventQueue;

    CLR_RT_HeapBlock *SerialDevice;

    HAL_RingBuffer<uint8_t> TxRingBuffer;
    uint8_t *TxBuffer;
    uint16_t TxOngoingCount;

    HAL_RingBuffer<uint8_t> RxRingBuffer;
    uint8_t *RxBuffer;
    uint16_t RxBytesToRead;

    bool IsLongRunning;
} NF_PAL_UART__;

#endif // WIN_DEV_SERIAL_NATIVE_TARGET_H
