//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _SYS_IO_SER_NATIVE_
#define _SYS_IO_SER_NATIVE_

#include <sys_io_ser_native.h>

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

#endif //_SYS_IO_SER_NATIVE_
