//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "board.h"
#include "fsl_lpuart_freertos.h"
#include "fsl_lpuart.h"

#include <nanoHAL_v2.h>
#include <WireProtocol.h>
#include <WireProtocol_Message.h>
#include <WireProtocol_Transport.h>

bool WP_Initialise();

static bool WP_Port_Intitialised = false;
static lpuart_rtos_handle_t handle;
static struct _lpuart_handle t_handle;
uint8_t background_buffer[1024];

lpuart_rtos_config_t lpuart_config = {
    .baudrate =
        490384, // NOTE: is should be 921600 but due to bug in FreeLink serial port configuration real baud is 490384
    .parity = kLPUART_ParityDisabled,
    .stopbits = kLPUART_OneStopBit,
    .buffer = background_buffer,
    .buffer_size = sizeof(background_buffer),
};

bool WP_Initialise()
{
    NVIC_SetPriority(BOARD_UART_IRQ, 5);

    lpuart_config.srcclk = BOARD_DebugConsoleSrcFreq();
    lpuart_config.base = LPUART1;

    int ret = LPUART_RTOS_Init(&handle, &t_handle, &lpuart_config);
    WP_Port_Intitialised = (ret == 0);

    if (!WP_Port_Intitialised)
    {
        vTaskSuspend(NULL);
    }

    return WP_Port_Intitialised;
}

void WP_Serial_ReceiveBytes(uint8_t **ptr, uint32_t *size)
{
    // TODO: Initialise Port if not already done, Wire Protocol should be calling this directly at startup
    if (!WP_Port_Intitialised)
    {
        WP_Initialise();
    }

    // save for later comparison
    uint32_t requestedSize = *size;

    // check for request with 0 size
    if (*size)
    {
        size_t read = 0;

#if CONFIG_NF_WP_CHANNEL_NETWORK && CONFIG_NF_WP_CHANNEL_SERIAL && !defined(I_AM_NANOBOOTER)
        size_t available = LPUART_TransferGetRxRingBufferLength(lpuart_config.base, &t_handle);

        if (available == 0)
        {
            vTaskDelay(pdMS_TO_TICKS(10));

            return;
        }

        if (requestedSize > available)
        {
            requestedSize = (uint32_t)available;
        }
#endif

        LPUART_RTOS_Receive(&handle, *ptr, requestedSize, &read);

        *ptr += read;
        *size -= read;
    }
}

uint8_t WP_Serial_TransmitMessage(WP_Message *message)
{
    if (!WP_Port_Intitialised)
    {
        WP_Initialise();
    }

    if (kStatus_Success != LPUART_RTOS_Send(&handle, (uint8_t *)&message->m_header, sizeof(message->m_header)))
    {
        return false;
    }

    // if there is anything on the payload send it to the output stream
    if (message->m_header.m_size && message->m_payload)
    {
        if (kStatus_Success != LPUART_RTOS_Send(&handle, (uint8_t *)message->m_payload, message->m_header.m_size))
        {
            return false;
        }
    }

    return true;
}

#if CONFIG_NF_WP_CHANNEL_NETWORK && !defined(I_AM_NANOBOOTER)

///////////////////////////////////////////////////////////////////////////////////////////////////
// transmission lock
// messages are transmitted from the CLR thread and from the receiver thread, the lock makes sure that the header and
// payload of a message are never interleaved with another one

// lifecycle of the transmission mutex, which is created on first use
typedef enum WP_TxMutexState
{
    WP_TxMutexState_NotCreated = 0,
    WP_TxMutexState_Creating = 1,
    WP_TxMutexState_Ready = 2,

} WP_TxMutexState;

static SemaphoreHandle_t s_txMutex = NULL;
static volatile WP_TxMutexState s_txMutexState = WP_TxMutexState_NotCreated;

bool WP_Transport_Lock(void)
{
    // create the mutex on first use, if that fails the next attempt tries again
    while (__atomic_load_n(&s_txMutexState, __ATOMIC_ACQUIRE) != WP_TxMutexState_Ready)
    {
        WP_TxMutexState expected = WP_TxMutexState_NotCreated;

        if (__atomic_compare_exchange_n(
                &s_txMutexState,
                &expected,
                WP_TxMutexState_Creating,
                false,
                __ATOMIC_ACQ_REL,
                __ATOMIC_ACQUIRE))
        {
            SemaphoreHandle_t mutex = xSemaphoreCreateMutex();

            if (mutex == NULL)
            {
                __atomic_store_n(&s_txMutexState, WP_TxMutexState_NotCreated, __ATOMIC_RELEASE);

                return false;
            }

            s_txMutex = mutex;
            __atomic_store_n(&s_txMutexState, WP_TxMutexState_Ready, __ATOMIC_RELEASE);
        }
        else
        {
            // being created by another thread
            taskYIELD();
        }
    }

    return xSemaphoreTake(s_txMutex, portMAX_DELAY) == pdTRUE;
}

void WP_Transport_Unlock(void)
{
    xSemaphoreGive(s_txMutex);
}

#endif // CONFIG_NF_WP_CHANNEL_NETWORK
