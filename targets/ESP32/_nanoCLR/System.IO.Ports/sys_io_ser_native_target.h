//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef SYS_IO_SER_NATIVE_TARGET_H
#define SYS_IO_SER_NASYS_IO_SER_NATIVE_TARGET_HTIVE_H

#include <sys_io_ser_native.h>

// struct representing the UART
typedef struct
{
    uart_port_t UartNum;
    TaskHandle_t UartEventTask;
    QueueHandle_t UartEventQueue;

    uint8_t *TxBuffer;
    uint16_t TxOngoingCount;

    HAL_RingBuffer<uint8_t> RxRingBuffer;
    uint8_t *RxBuffer;
    uint16_t RxBytesToRead;

    bool IsLongRunning;
    bool SignalLevelsInverted;

    uint8_t NewLineChar;
    uint32_t ReceivedBytesThreshold;

} NF_PAL_UART;

////////////////////////////////////////////
// declaration of the the UART PAL structs //
////////////////////////////////////////////
extern NF_PAL_UART Uart0_PAL;
extern NF_PAL_UART Uart1_PAL;
#if defined(UART_NUM_2)
extern NF_PAL_UART Uart2_PAL;
#endif

#endif // SYS_IO_SER_NATIVE_TARGET_H
