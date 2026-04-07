//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright 2018 Silicon Laboratories Inc. All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <tx_api.h>
#include <targetHAL.h>

#include <em_core.h>
#include <sl_usbd_core.h>
#include <sl_usbd_core_config.h>
#include <sli_usbd_core.h>
#include <sli_usbd_driver.h>

TX_THREAD task_handle;
uint32_t usbdCoreThreadStack[SL_USBD_TASK_STACK_SIZE / sizeof(uint32_t)];
uint8_t message_queue_storage[sizeof(sli_usbd_core_event_t) * SL_USBD_CORE_EVENT_BUS_NBR];

// // parameters for task
// static osThreadId_t         task_handle;
// static const osThreadAttr_t task_attr = {
//   .name       = "USBD Thread",
//   .attr_bits  = 0,
//   .stack_mem  = NULL,
//   .stack_size = (uint32_t)SL_USBD_TASK_STACK_SIZE,
//   .cb_mem     = NULL,
//   .cb_size    = 0,
//   .priority   = (osPriority_t)SL_USBD_TASK_PRIORITY
// };

// parameters for message queue
// static osMessageQueueId_t message_queue_handle;
// static const osMessageQueueAttr_t message_queue_attr =
//     {.name = "USBD Queue", .attr_bits = 0, .cb_mem = NULL, .cb_size = 0, .mq_mem = NULL, .mq_size = 0};

static TX_QUEUE message_queue_handle;

// parameters for event flags
// static osEventFlagsId_t     eventflags_handle[SL_USBD_OPEN_ENDPOINTS_QUANTITY];
// static osEventFlagsAttr_t   eventflags_attr[SL_USBD_OPEN_ENDPOINTS_QUANTITY];

static TX_EVENT_FLAGS_GROUP eventflags_handle[SL_USBD_OPEN_ENDPOINTS_QUANTITY];

#define EVENT_FLAG_COMPLETE 0x1 // signal posted
#define EVENT_FLAG_ABORT    0x2 // signal aborted

// parameters for mutex
// static osMutexId_t mutex_handle[SL_USBD_OPEN_ENDPOINTS_QUANTITY];
// static osMutexAttr_t mutex_attr[SL_USBD_OPEN_ENDPOINTS_QUANTITY];

static TX_MUTEX mutex_handle[SL_USBD_OPEN_ENDPOINTS_QUANTITY];

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void usbd_core_os_task_main(uint32_t p_arg);

static uint32_t usbd_core_os_ms_to_ticks(uint32_t milliseconds);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/*******************************************************************************************************
 *  Create task and queue for task to pend on
 *******************************************************************************************************/
sl_status_t sli_usbd_core_os_create_task(void)
{
    uint16_t status;

    status = tx_queue_create(
        &message_queue_handle,
        "USBD Queue",
        sizeof(sli_usbd_core_event_t),
        message_queue_storage,
        sizeof(message_queue_storage));

    // message_queue_handle =
    //     osMessageQueueNew(SL_USBD_CORE_EVENT_BUS_NBR, sizeof(sli_usbd_core_event_t), &message_queue_attr);

    if (status != TX_SUCCESS)
    {
        return SL_STATUS_FAIL;
    }

    // Create USB device Core thread
    status = tx_thread_create(
        &task_handle,
        "USBD Core Thread",
        usbd_core_os_task_main,
        0,
        usbdCoreThreadStack,
        SL_USBD_TASK_STACK_SIZE,
        SL_USBD_TASK_PRIORITY,
        SL_USBD_TASK_PRIORITY,
        TX_NO_TIME_SLICE,
        TX_AUTO_START);

    if (status != TX_SUCCESS)
    {
        return SL_STATUS_FAIL;
    }

    return SL_STATUS_OK;
}

/*******************************************************************************************************
 *  Create an OS signal
 *******************************************************************************************************/
sl_status_t sli_usbd_core_os_create_endpoint_signal(uint8_t endpoint)
{
    if (endpoint >= SL_USBD_OPEN_ENDPOINTS_QUANTITY)
    {
        return SL_STATUS_FAIL;
    }

    //   eventflags_attr[endpoint].name       = "USBD events";
    //   eventflags_attr[endpoint].attr_bits  = 0;
    //   eventflags_attr[endpoint].cb_mem     = NULL;
    //   eventflags_attr[endpoint].cb_size    = 0;

    //   eventflags_handle[endpoint] = osEventFlagsNew(&eventflags_attr[endpoint]);

    if (tx_event_flags_create(&eventflags_handle[endpoint], "USBD events") != TX_SUCCESS)
    {
        return SL_STATUS_FAIL;
    }

    return SL_STATUS_OK;
}

/*******************************************************************************************************
 *  Delete an OS signal
 *******************************************************************************************************/
sl_status_t sli_usbd_core_os_delete_endpoint_signal(uint8_t endpoint)
{
    if (endpoint >= SL_USBD_OPEN_ENDPOINTS_QUANTITY)
    {
        return SL_STATUS_FAIL;
    }

    if (tx_event_flags_delete(&eventflags_handle[endpoint]) != TX_SUCCESS)
    {
        return SL_STATUS_FAIL;
    }

    return SL_STATUS_OK;
}

/*******************************************************************************************************
 *  Wait for a signal to become available
 *******************************************************************************************************/
sl_status_t sli_usbd_core_os_pend_endpoint_signal(uint8_t endpoint, uint16_t timeout_ms)
{
    uint32_t ticks;
    uint32_t status;
    uint32_t actual_events;

    if (endpoint >= SL_USBD_OPEN_ENDPOINTS_QUANTITY)
    {
        return SL_STATUS_FAIL;
    }

    if (timeout_ms == 0)
    {
        ticks = TX_WAIT_FOREVER;
    }
    else
    {
        ticks = usbd_core_os_ms_to_ticks(timeout_ms);
    }

    status = tx_event_flags_get(
        &eventflags_handle[endpoint],
        EVENT_FLAG_COMPLETE | EVENT_FLAG_ABORT,
        TX_OR_CLEAR,
        &actual_events,
        ticks);

    if (status == TX_NO_EVENTS)
    {
        return SL_STATUS_TIMEOUT;
    }

    if (status != TX_SUCCESS)
    {
        return SL_STATUS_FAIL;
    }

    if ((actual_events & EVENT_FLAG_ABORT) == EVENT_FLAG_ABORT)
    {
        return SL_STATUS_ABORT;
    }

    return SL_STATUS_OK;
}

/*******************************************************************************************************
 *  Abort any wait operation on signal
 *******************************************************************************************************/
sl_status_t sli_usbd_core_os_abort_endpoint_signal(uint8_t endpoint)
{
    if (endpoint >= SL_USBD_OPEN_ENDPOINTS_QUANTITY)
    {
        return SL_STATUS_FAIL;
    }

    if (tx_event_flags_set(&eventflags_handle[endpoint], EVENT_FLAG_ABORT, TX_OR) != TX_SUCCESS)
    {
        return SL_STATUS_FAIL;
    }

    return SL_STATUS_OK;
}

/*******************************************************************************************************
 *  Make a signal available
 *******************************************************************************************************/
sl_status_t sli_usbd_core_os_post_endpoint_signal(uint8_t endpoint)
{
    if (endpoint >= SL_USBD_OPEN_ENDPOINTS_QUANTITY)
    {
        return SL_STATUS_FAIL;
    }

    if (tx_event_flags_set(&eventflags_handle[endpoint], EVENT_FLAG_COMPLETE, TX_OR) != TX_SUCCESS)
    {
        return SL_STATUS_FAIL;
    }

    return SL_STATUS_OK;
}

/*******************************************************************************************************
 *  Create an OS resource to use as an endpoint lock
 *******************************************************************************************************/
sl_status_t sli_usbd_core_os_create_endpoint_lock(uint8_t endpoint)
{
    if (endpoint >= SL_USBD_OPEN_ENDPOINTS_QUANTITY)
    {
        return SL_STATUS_FAIL;
    }

    if (tx_mutex_create(&mutex_handle[endpoint], "USBD mutex", TX_NO_INHERIT) != TX_SUCCESS)
    {
        return SL_STATUS_FAIL;
    }

    return SL_STATUS_OK;
}

/*******************************************************************************************************
 *  Delete the OS resource used as an endpoint lock
 *******************************************************************************************************/
sl_status_t sli_usbd_core_os_delete_endpoint_lock(uint8_t endpoint)
{
    if (endpoint >= SL_USBD_OPEN_ENDPOINTS_QUANTITY)
    {
        return SL_STATUS_FAIL;
    }

    if (tx_mutex_delete(&mutex_handle[endpoint]) != TX_SUCCESS)
    {
        return SL_STATUS_FAIL;
    }

    return SL_STATUS_OK;
}

/*******************************************************************************************************
 *  Wait for an endpoint to become available and acquire its lock
 *******************************************************************************************************/
sl_status_t sli_usbd_core_os_acquire_endpoint_lock(uint8_t endpoint)
{
    if (endpoint >= SL_USBD_OPEN_ENDPOINTS_QUANTITY)
    {
        return SL_STATUS_FAIL;
    }

    if (tx_mutex_get(&mutex_handle[endpoint], TX_WAIT_FOREVER) != TX_SUCCESS)
    {
        return SL_STATUS_FAIL;
    }

    return SL_STATUS_OK;
}

/*******************************************************************************************************
 *  Release an endpoint lock
 *******************************************************************************************************/
sl_status_t sli_usbd_core_os_release_endpoint_lock(uint8_t endpoint)
{
    if (endpoint >= SL_USBD_OPEN_ENDPOINTS_QUANTITY)
    {
        return SL_STATUS_FAIL;
    }

    if (tx_mutex_put(&mutex_handle[endpoint]) != TX_SUCCESS)
    {
        return SL_STATUS_FAIL;
    }

    return SL_STATUS_OK;
}

/*******************************************************************************************************
 *   Wait until a core event is ready
 *******************************************************************************************************/
sl_status_t sli_usbd_core_os_get_core_event(void *p_event)
{
    if (tx_queue_receive(&message_queue_handle, p_event, TX_WAIT_FOREVER) != TX_SUCCESS)
    {
        return SL_STATUS_FAIL;
    }

    return SL_STATUS_OK;
}

/*******************************************************************************************************
 *   Queues core event
 *******************************************************************************************************/
sl_status_t sli_usbd_core_os_put_core_event(void *p_event)
{
    if (tx_queue_send(&message_queue_handle, p_event, TX_NO_WAIT) != TX_SUCCESS)
    {
        return SL_STATUS_FAIL;
    }

    return SL_STATUS_OK;
}

// usbd_core_os_task_main()
// OS-dependent shell task to process USB core events.
// @param p_arg: Pointer to task initialization argument
static void usbd_core_os_task_main(uint32_t p_arg)
{
    (void)p_arg;

    while (true)
    {
        sli_usbd_core_task_handler();
    }
}

// usbd_core_os_ms_to_ticks()
// Converts milliseconds to kernel ticks
// @param milliseconds: milliseconds to convert to ticks
// @return Number of ticks
static uint32_t usbd_core_os_ms_to_ticks(uint32_t milliseconds)
{
    return TX_TICKS_PER_MILLISEC(milliseconds);
}
