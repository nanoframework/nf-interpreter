//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _WIN_DEV_SERIAL_NATIVE_TARGET_H_
#define _WIN_DEV_SERIAL_NATIVE_TARGET_H_

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
} NF_PAL_UART;

#endif //_WIN_DEV_SERIAL_NATIVE_TARGET_H_
