//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef SYS_IO_SER_NATIVE_TARGET_H
#define SYS_IO_SER_NATIVE_TARGET_H

#include <target_system_io_ports_config.h>
#include <sys_io_ser_native.h>

#include <ti/drivers/UART2.h>
#include <ti_drivers_config.h>

// struct representing the UART
typedef struct
{
    UART2_Handle UartDriver;
    UART2_Params UartParams;
    uint8_t UartNum;

    Task_Handle WorkingTask;

    uint8_t *TxBuffer;
    uint16_t TxOngoingCount;

    HAL_RingBuffer<uint8_t> RxRingBuffer;
    uint8_t *RxBuffer;
    uint16_t RxBytesToRead;

    bool IsLongRunning;

    uint8_t WatchChar;
    uint8_t NewLineChar;
} NF_PAL_UART;

////////////////////////////////////////////
// declaration of the the UART PAL strucs //
////////////////////////////////////////////
#if defined(NF_SERIAL_COMM_TI_USE_UART1) && (NF_SERIAL_COMM_TI_USE_UART1 == TRUE)
extern NF_PAL_UART Uart1_PAL;
#endif

#endif // SYS_IO_SER_NATIVE_TARGET_H
