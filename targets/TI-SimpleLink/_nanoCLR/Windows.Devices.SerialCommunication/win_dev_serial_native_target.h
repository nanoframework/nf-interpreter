//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _WIN_DEV_SERIAL_NATIVE_TARGET_H_
#define _WIN_DEV_SERIAL_NATIVE_TARGET_H_

#include <target_windows_devices_serialcommunication_config.h>
#include <win_dev_serial_native.h>

#include <ti/drivers/UART2.h>
#include <ti_drivers_config.h>

// struct representing the UART
typedef struct
{
    UART2_Handle UartDriver;
    UART2_Params UartParams;
    uint8_t UartNum;

    HAL_RingBuffer<uint8_t> TxRingBuffer;
    uint8_t *TxBuffer;
    uint16_t TxOngoingCount;

    uint16_t RxBytesToRead;

    bool IsLongRunning;

    uint8_t WatchChar;
} NF_PAL_UART;

////////////////////////////////////////////
// declaration of the the UART PAL strucs //
////////////////////////////////////////////
#if defined(NF_SERIAL_COMM_TI_USE_UART1) && (NF_SERIAL_COMM_TI_USE_UART1 == TRUE)
extern NF_PAL_UART Uart1_PAL;
#endif

/////////////////////////////////////
// UART Tx buffers                 //
// these live in the target folder //
/////////////////////////////////////
extern uint8_t Uart1_TxBuffer[];

#define UART_TX_BUFFER_SIZE(num) UART##num##_TX_SIZE

#endif //_WIN_DEV_SERIAL_NATIVE_TARGET_H_
