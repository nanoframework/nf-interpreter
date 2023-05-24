//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <target_platform.h>
#include <targetHAL.h>
#include <WireProtocol_Message.h>

#include <platform.h>
#include <tx_api.h>

#include <sl_usbd_core.h>
#include <sl_usbd_class_cdc.h>
#include <sl_usbd_class_cdc_acm.h>
#include <sl_usbd_class_cdc_acm_instances.h>

#define TASK_DELAY_MS 250u

__attribute__((noreturn)) void ReceiverThread_entry(uint32_t parameter)
{
    (void)parameter;

#if HAL_WP_USE_USB_CDC == TRUE

#if !defined(BUILD_RTM)
    sl_status_t status = SL_STATUS_OK;
#endif

    bool conn = false;

    const uint32_t xDelay = TX_TICKS_PER_MILLISEC(TASK_DELAY_MS);
#endif

    tx_thread_sleep(50);

    WP_Message_PrepareReception();

    // loop until thread receives a request to terminate
    while (1)
    {

#if HAL_WP_USE_USB_CDC == TRUE

        // Wait until device is in configured state
#if !defined(BUILD_RTM)
        status =
#endif
            sl_usbd_cdc_acm_is_enabled(sl_usbd_cdc_acm_acm0_number, &conn);
        _ASSERTE(status == SL_STATUS_OK);

        // while ((conn != true) || ((line_state & SL_USBD_CDC_ACM_CTRL_DTR) == 0))
        while ((conn != true))
        {
            // Delay Task
            tx_thread_sleep(xDelay);

#if !defined(BUILD_RTM)
            status =
#endif
                sl_usbd_cdc_acm_is_enabled(sl_usbd_cdc_acm_acm0_number, &conn);
            _ASSERTE(status == SL_STATUS_OK);
        }
#endif

        WP_Message_Process();

        // pass control to the OS
        tx_thread_sleep(TX_TICKS_PER_MILLISEC(10));
    }

    // this function never returns
}
