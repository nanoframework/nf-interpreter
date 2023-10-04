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
#include <sl_usbd_core.h>
#include "sl_usbd_class_hid.h"
#include <sli_usbd_class_hid.h>
#include <sli_usbd_class_hid_azurertos.h>

// parameters for task
// static osThreadId_t timer_task_handle;
// static const osThreadAttr_t timer_task_attr = {
//     .name = "HID Timer Thread",
//     .attr_bits = 0,
//     .stack_mem = NULL,
//     .stack_size = (uint32_t)SL_USBD_HID_TIMER_TASK_STACK_SIZE,
//     .cb_mem = NULL,
//     .cb_size = 0,
//     .priority = (osPriority_t)SL_USBD_HID_TIMER_TASK_PRIORITY};
TX_THREAD timer_task_handle;
uint32_t timerThreadStack[SL_USBD_HID_TIMER_TASK_STACK_SIZE / sizeof(uint32_t)];

// parameters for event flags
// static osEventFlagsId_t     input_eventflags_handle[SL_USBD_HID_CLASS_INSTANCE_QUANTITY];
// static osEventFlagsAttr_t   input_eventflags_attr[SL_USBD_HID_CLASS_INSTANCE_QUANTITY];

static TX_EVENT_FLAGS_GROUP input_eventflags_handle[SL_USBD_HID_CLASS_INSTANCE_QUANTITY];

// static osEventFlagsId_t     output_eventflags_handle[SL_USBD_HID_CLASS_INSTANCE_QUANTITY];
// static osEventFlagsAttr_t   output_eventflags_attr[SL_USBD_HID_CLASS_INSTANCE_QUANTITY];

static TX_EVENT_FLAGS_GROUP output_eventflags_handle[SL_USBD_HID_CLASS_INSTANCE_QUANTITY];

#define EVENT_FLAG_COMPLETE 0x1 // signal posted
#define EVENT_FLAG_ABORT    0x2 // signal aborted

// parameters for mutexes
// static osMutexId_t          input_mutex_handle[SL_USBD_HID_CLASS_INSTANCE_QUANTITY];
// static osMutexAttr_t        input_mutex_attr[SL_USBD_HID_CLASS_INSTANCE_QUANTITY];

static TX_MUTEX input_mutex_handle[SL_USBD_HID_CLASS_INSTANCE_QUANTITY];

// static osMutexId_t          output_mutex_handle[SL_USBD_HID_CLASS_INSTANCE_QUANTITY];
// static osMutexAttr_t        output_mutex_attr[SL_USBD_HID_CLASS_INSTANCE_QUANTITY];

static TX_MUTEX output_mutex_handle[SL_USBD_HID_CLASS_INSTANCE_QUANTITY];

// static osMutexId_t          tx_mutex_handle[SL_USBD_HID_CLASS_INSTANCE_QUANTITY];
// static osMutexAttr_t        tx_mutex_attr[SL_USBD_HID_CLASS_INSTANCE_QUANTITY];

static TX_MUTEX tx_mutex_handle[SL_USBD_HID_CLASS_INSTANCE_QUANTITY];

static void usbd_hid_os_timer_task(uint32_t p_arg);

static uint32_t usbd_hid_os_ms_to_ticks(uint32_t milliseconds);

// Initialize HID OS interface
sl_status_t sli_usbd_hid_os_init(void)
{
    uint32_t class_nbr;
    uint16_t status;

    for (class_nbr = 0u; class_nbr < SL_USBD_HID_CLASS_INSTANCE_QUANTITY; class_nbr++)
    {
        // tx_mutex_attr[class_nbr].name = "HID Tx mutex";
        // tx_mutex_attr[class_nbr].attr_bits = 0;
        // tx_mutex_attr[class_nbr].cb_mem = NULL;
        // tx_mutex_attr[class_nbr].cb_size = 0;

        // tx_mutex_handle[class_nbr] = osMutexNew(&tx_mutex_attr[class_nbr]);

        if (tx_mutex_create(&tx_mutex_handle[class_nbr], "HID Tx mutex", TX_NO_INHERIT) != TX_SUCCESS)
        {
            return SL_STATUS_FAIL;
        }

        // output_mutex_attr[class_nbr].name = "HID Output mutex";
        // output_mutex_attr[class_nbr].attr_bits = 0;
        // output_mutex_attr[class_nbr].cb_mem = NULL;
        // output_mutex_attr[class_nbr].cb_size = 0;

        // output_mutex_handle[class_nbr] = osMutexNew(&output_mutex_attr[class_nbr]);

        if (tx_mutex_create(&output_mutex_handle[class_nbr], "HID Output mutex", TX_NO_INHERIT) != TX_SUCCESS)
        {
            return SL_STATUS_FAIL;
        }

        // input_mutex_attr[class_nbr].name = "HID Input mutex";
        // input_mutex_attr[class_nbr].attr_bits = 0;
        // input_mutex_attr[class_nbr].cb_mem = NULL;
        // input_mutex_attr[class_nbr].cb_size = 0;

        // input_mutex_handle[class_nbr] = osMutexNew(&input_mutex_attr[class_nbr]);

        if (tx_mutex_create(&input_mutex_handle[class_nbr], "HID Input mutex", TX_NO_INHERIT) != TX_SUCCESS)
        {
            return SL_STATUS_FAIL;
        }

        // input_eventflags_attr[class_nbr].name       = "HID input events";
        // input_eventflags_attr[class_nbr].attr_bits  = 0;
        // input_eventflags_attr[class_nbr].cb_mem     = NULL;
        // input_eventflags_attr[class_nbr].cb_size    = 0;

        // input_eventflags_handle[class_nbr] = osEventFlagsNew(&input_eventflags_attr[class_nbr]);

        if (tx_event_flags_create(&input_eventflags_handle[class_nbr], "HID input events") != TX_SUCCESS)
        {
            return SL_STATUS_FAIL;
        }

        // output_eventflags_attr[class_nbr].name       = "HID output events";
        // output_eventflags_attr[class_nbr].attr_bits  = 0;
        // output_eventflags_attr[class_nbr].cb_mem     = NULL;
        // output_eventflags_attr[class_nbr].cb_size    = 0;

        // output_eventflags_handle[class_nbr] = osEventFlagsNew(&output_eventflags_attr[class_nbr]);

        if (tx_event_flags_create(&output_eventflags_handle[class_nbr], "HID output events") != TX_SUCCESS)
        {
            return SL_STATUS_FAIL;
        }
    }

    status = tx_thread_create(
        &timer_task_handle,
        "HID Timer Thread",
        usbd_hid_os_timer_task,
        0,
        timerThreadStack,
        SL_USBD_HID_TIMER_TASK_STACK_SIZE,
        SL_USBD_HID_TIMER_TASK_PRIORITY,
        SL_USBD_HID_TIMER_TASK_PRIORITY,
        TX_NO_TIME_SLICE,
        TX_AUTO_START);

    if (status != TX_SUCCESS)
    {
        return SL_STATUS_FAIL;
    }

    // timer_task_handle = osThreadNew(usbd_hid_os_timer_task, NULL, &timer_task_attr);

    // if (timer_task_handle == NULL)
    // {
    //     return SL_STATUS_FAIL;
    // }

    return SL_STATUS_OK;
}

// Lock class input report
sl_status_t sli_usbd_hid_os_lock_input(uint8_t class_nbr)
{
    if (tx_mutex_get(&input_mutex_handle[class_nbr], TX_WAIT_FOREVER) != TX_SUCCESS)
    {
        return SL_STATUS_FAIL;
    }

    return SL_STATUS_OK;
}

// Unlock class input report
sl_status_t sli_usbd_hid_os_unlock_input(uint8_t class_nbr)
{
    if (tx_mutex_put(&input_mutex_handle[class_nbr]) != TX_SUCCESS)
    {
        return SL_STATUS_FAIL;
    }

    return SL_STATUS_OK;
}

// Abort class output report
sl_status_t sli_usbd_hid_os_pend_abort_output(uint8_t class_nbr)
{
    if (tx_event_flags_set(&output_eventflags_handle[class_nbr], EVENT_FLAG_ABORT, TX_OR) != TX_SUCCESS)
    {
        return SL_STATUS_FAIL;
    }

    return SL_STATUS_OK;
}

// Wait for output report data transfer to complete
sl_status_t sli_usbd_hid_os_pend_output(uint8_t class_nbr, uint16_t timeout_ms)
{
    uint32_t ticks;
    uint32_t status;
    uint32_t actual_events;

    if (timeout_ms == 0)
    {
        ticks = TX_WAIT_FOREVER;
    }
    else
    {
        ticks = usbd_hid_os_ms_to_ticks(timeout_ms);
    }

    status = tx_event_flags_get(
        &output_eventflags_handle[class_nbr],
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

// Signal that output report data is available
sl_status_t sli_usbd_hid_os_post_output(uint8_t class_nbr)
{
    if (tx_event_flags_set(&output_eventflags_handle[class_nbr], EVENT_FLAG_COMPLETE, TX_OR) != TX_SUCCESS)
    {
        return SL_STATUS_FAIL;
    }

    return SL_STATUS_OK;
}

// Lock class output report
sl_status_t sli_usbd_hid_os_lock_output(uint8_t class_nbr)
{
    if (tx_mutex_get(&output_mutex_handle[class_nbr], TX_WAIT_FOREVER) != TX_SUCCESS)
    {
        return SL_STATUS_FAIL;
    }

    return SL_STATUS_OK;
}

// Unlock class output report
sl_status_t sli_usbd_hid_os_unlock_output(uint8_t class_nbr)
{
    if (tx_mutex_put(&output_mutex_handle[class_nbr]) != TX_SUCCESS)
    {
        return SL_STATUS_FAIL;
    }

    return SL_STATUS_OK;
}

// Lock class transmit
sl_status_t sli_usbd_hid_os_lock_tx(uint8_t class_nbr)
{
    if (tx_mutex_get(&tx_mutex_handle[class_nbr], TX_WAIT_FOREVER) != TX_SUCCESS)
    {
        return SL_STATUS_FAIL;
    }

    return SL_STATUS_OK;
}

// Unlock class transmit
sl_status_t sli_usbd_hid_os_unlock_tx(uint8_t class_nbr)
{
    if (tx_mutex_put(&tx_mutex_handle[class_nbr]) != TX_SUCCESS)
    {
        return SL_STATUS_FAIL;
    }

    return SL_STATUS_OK;
}

// Wait for input report data transfer to complete
sl_status_t sli_usbd_hid_os_pend_input(uint8_t class_nbr, uint16_t timeout_ms)
{
    uint32_t ticks;
    uint32_t status;
    uint32_t actual_events;

    if (timeout_ms == 0)
    {
        ticks = TX_WAIT_FOREVER;
    }
    else
    {
        ticks = usbd_hid_os_ms_to_ticks(timeout_ms);
    }

    status = tx_event_flags_get(
        &input_eventflags_handle[class_nbr],
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

// Abort any operation on input report
sl_status_t sli_usbd_hid_os_pend_abort_input(uint8_t class_nbr)
{
    if (tx_event_flags_set(&input_eventflags_handle[class_nbr], EVENT_FLAG_ABORT, TX_OR) != TX_SUCCESS)
    {
        return SL_STATUS_FAIL;
    }

    return SL_STATUS_OK;
}

// Signal that input report data transfer has completed
sl_status_t sli_usbd_hid_os_post_input(uint8_t class_nbr)
{
    if (tx_event_flags_set(&input_eventflags_handle[class_nbr], EVENT_FLAG_COMPLETE, TX_OR) != TX_SUCCESS)
    {
        return SL_STATUS_FAIL;
    }

    return SL_STATUS_OK;
}

// usbd_hid_os_timer_task()
// OS - dependent shell task to process periodic HID input reports.
// **@param
// p_arg: Pointer to task initialization argument.
// @note(1)
// Assumes tick rate frequency is greater than or equal to 250 Hz.
// Otherwise, timer task scheduling rate will NOT be correct.
// @note(2)
// Timer task MUST delay without failure.
// (a)Failure to delay timer task will prevent some HID report task(s) operation(s) from functioning correctly.
// Thus, timer task is assumed to be successfully delayed since NO error handling *could be performed to counteract
// failure.
static void usbd_hid_os_timer_task(uint32_t p_arg)
{
    uint32_t dly_tick;
    (void)p_arg;

    // Delay task at 4 ms rate (see Note #1).
    dly_tick = usbd_hid_os_ms_to_ticks(4);

    while (true)
    {
        tx_thread_sleep(dly_tick);
        sli_usbd_hid_report_timer_task_handler();
    }
}

// Converts milliseconds to kernel ticks.
static uint32_t usbd_hid_os_ms_to_ticks(uint32_t milliseconds)
{
    return TX_TICKS_PER_MILLISEC(milliseconds);
}
