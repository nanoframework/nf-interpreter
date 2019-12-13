//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _WIN_DEV_SERIAL_NATIVE_TARGET_H_
#define _WIN_DEV_SERIAL_NATIVE_TARGET_H_

#define UART_INTERRUPT_PRIO 4U

#include "FreeRTOS.h"
#include <nanoHAL.h>
#include <target_windows_devices_serialcommunication_config.h>
#include <win_dev_serial_native.h>

#include "fsl_lpuart.h"

typedef struct {
  uint8_t uartNum;

  lpuart_config_t uartCfg;
  lpuart_transfer_t xfer;

  HAL_RingBuffer<uint8_t> TxRingBuffer;
  uint8_t *TxBuffer;
  uint16_t TxOngoingCount;

  HAL_RingBuffer<uint8_t> RxRingBuffer;
  uint8_t *RxBuffer;
  uint16_t RxBytesToRead;
  uint8_t WatchChar;

  TaskHandle_t xRTaskToNotify;
  TaskHandle_t xWTaskToNotify;

} NF_PAL_UART;

#endif //_WIN_DEV_SERIAL_NATIVE_TARGET_H_
