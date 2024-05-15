//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <target_platform.h>

#include <nanoHAL_v2.h>
#include <WireProtocol.h>
#include <WireProtocol_Message.h>

#include <platform.h>
#include <tx_api.h>

#if HAL_WP_USE_SERIAL == TRUE
#include <sl_iostream_init_instances.h>
#include <sl_iostream_handles.h>
#elif HAL_WP_USE_USB_CDC == TRUE

#include <sl_usbd_core.h>
#include <sl_usbd_class_cdc.h>
#include <sl_usbd_class_cdc_acm.h>
#include <sl_usbd_class_cdc_acm_instances.h>
#endif

#if defined(TRACE_MASK) && (TRACE_MASK & TRACE_VERBOSE) != 0
// used WP_Message_Process() and methods it calls to avoid flooding TRACE
extern uint32_t traceLoopCounter;
#endif

void WP_ReceiveBytes(uint8_t **ptr, uint32_t *size)
{
    // save for later comparison
    uint32_t requestedSize = *size;
    sl_status_t requestResult;

#if HAL_WP_USE_SERIAL == TRUE
    size_t bytesRead;
#elif HAL_WP_USE_USB_CDC == TRUE
    bool conn = false;
    uint32_t bytesRead;
#endif

    // check for requests with 0 size
    if (*size)
    {
#if HAL_WP_USE_USB_CDC == TRUE
        // check if device is connected
        sl_usbd_cdc_acm_is_enabled(sl_usbd_cdc_acm_acm0_number, &conn);

        if (!conn)
        {
            // device is not connected
            return;
        }
#endif

#if HAL_WP_USE_SERIAL == TRUE
        // blocking receive as SL API does not support non-blocking
        requestResult = sl_iostream_read(sl_iostream_vcom_handle, *ptr, requestedSize, &bytesRead);
#elif HAL_WP_USE_USB_CDC == TRUE
        requestResult = sl_usbd_cdc_acm_read(sl_usbd_cdc_acm_acm0_number, *ptr, requestedSize, 200, &bytesRead);
#endif

        // Warning: Including TRACE_VERBOSE will NOT output the following TRACE on every loop of the statemachine to
        // avoid flooding the trace.
        TRACE_LIMIT(TRACE_VERBOSE, 100, "RXMSG: Expecting %d bytes, received %d.\n", requestedSize, read);

#if HAL_WP_USE_SERIAL == TRUE
        if (requestResult == SL_STATUS_EMPTY)
        {
            // hang here for a bit
            tx_thread_sleep(10);
        }
        else
#endif
            if (requestResult == SL_STATUS_OK)
        {
            // update pointer and size
            *ptr += bytesRead;
            *size -= bytesRead;
        }
    }

    return;
}

uint8_t WP_TransmitMessage(WP_Message *message)
{
#if HAL_WP_USE_USB_CDC == TRUE
    bool conn = false;
    uint32_t dummy = 0;
#endif

    TRACE_WP_HEADER(WP_TXMSG, message);

#if HAL_WP_USE_USB_CDC == TRUE
    // check if device is connected
    sl_usbd_cdc_acm_is_enabled(sl_usbd_cdc_acm_acm0_number, &conn);

    if (!conn)
    {
        // device is not connected
        return false;
    }
#endif

#if HAL_WP_USE_SERIAL == TRUE
    // non-blocking transmit
    if (sl_iostream_write(sl_iostream_vcom_handle, (uint8_t *)&message->m_header, sizeof(message->m_header)) !=
        SL_STATUS_OK)
#elif HAL_WP_USE_USB_CDC == TRUE
    if (sl_usbd_cdc_acm_write(
            sl_usbd_cdc_acm_acm0_number,
            (uint8_t *)&message->m_header,
            sizeof(message->m_header),
            20,
            &dummy) != SL_STATUS_OK)
#endif

    {
        return false;
    }

    // if there is anything on the payload send it to the output stream
    if (message->m_header.m_size && message->m_payload)
    {
#if HAL_WP_USE_SERIAL == TRUE
        // non-blocking transmit
        if (sl_iostream_write(sl_iostream_vcom_handle, message->m_payload, message->m_header.m_size) != SL_STATUS_OK)
#elif HAL_WP_USE_USB_CDC == TRUE
        if (sl_usbd_cdc_acm_write(
                sl_usbd_cdc_acm_acm0_number,
                message->m_payload,
                message->m_header.m_size,
                200,
                &dummy) != SL_STATUS_OK)
#endif
        {
            return false;
        }
    }

    return true;
}
