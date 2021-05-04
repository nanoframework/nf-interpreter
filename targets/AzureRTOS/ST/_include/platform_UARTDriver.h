//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _PLATFORM_UART_DRIVER_H_
#define _PLATFORM_UART_DRIVER_H_

#include <nanoCLR_Headers.h>
#include <nanoHAL_v2.h>
#include <nanoRingBuffer.h>

#include <platform.h>
#include <targetHAL.h>

#include <stm32l4xx_ll_usart.h>

#ifdef __cplusplus
extern "C"
{
#endif

extern NanoRingBuffer WPRingBuffer;
extern TX_EVENT_FLAGS_GROUP wpUartEvent;

void WP_Reception_Callback();
void USART_Error_Callback(void);
uint32_t nano_HAL_UART_ReadTimeout(uint8_t *ptr, uint32_t requestedSize, uint32_t timeout);

#ifdef __cplusplus
}
#endif

#endif //_PLATFORM_UART_DRIVER_H_
