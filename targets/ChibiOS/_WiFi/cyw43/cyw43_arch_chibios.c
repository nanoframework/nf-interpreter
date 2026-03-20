//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// CYW43 architecture layer for ChibiOS/RT.
// Provides initialization, synchronization primitives, and poll scheduling.

#include <ch.h>
#include <hal.h>
#include "cyw43_configport.h"

// ---------------------------------------------------------------------------
// Synchronization primitives (declared extern in configport.h)
// ---------------------------------------------------------------------------
mutex_t cyw43_mutex;
binary_semaphore_t cyw43_sdpcm_sem;
binary_semaphore_t cyw43_ioctl_sem;

// Poll dispatch function and event
static void (*cyw43_poll_func)(void) = NULL;
static event_source_t cyw43_poll_event;
static thread_t *cyw43_poll_thread = NULL;

// CYW43 poll thread — runs CYW43 internal polling when signaled
#define CYW43_POLL_THREAD_STACK_SIZE 1024
static THD_WORKING_AREA(wa_cyw43_poll_thread, CYW43_POLL_THREAD_STACK_SIZE);

static THD_FUNCTION(cyw43_poll_thread_func, arg)
{
    (void)arg;
    event_listener_t el;

    chRegSetThreadName("cyw43poll");
    chEvtRegister(&cyw43_poll_event, &el, 0);

    while (true)
    {
        chEvtWaitAny(EVENT_MASK(0));
        chEvtGetAndClearEvents(EVENT_MASK(0));

        if (cyw43_poll_func != NULL)
        {
            cyw43_poll_func();
        }
    }
}

// Called by the CYW43 driver when it needs its poll function called
void cyw43_schedule_internal_poll_dispatch(void (*func)(void))
{
    cyw43_poll_func = func;
    chEvtBroadcast(&cyw43_poll_event);
}

// Called after poll completes — can signal waiting threads
void cyw43_post_poll_hook(void)
{
    chBSemSignal(&cyw43_sdpcm_sem);
    chBSemSignal(&cyw43_ioctl_sem);
}

// ---------------------------------------------------------------------------
// Architecture initialization
// ---------------------------------------------------------------------------
void cyw43_arch_init(void)
{
    // Initialize synchronization primitives
    chMtxObjectInit(&cyw43_mutex);
    chBSemObjectInit(&cyw43_sdpcm_sem, true);
    chBSemObjectInit(&cyw43_ioctl_sem, true);

    // Initialize poll event source
    chEvtObjectInit(&cyw43_poll_event);

    // Create the CYW43 poll thread
    cyw43_poll_thread = chThdCreateStatic(
        wa_cyw43_poll_thread,
        sizeof(wa_cyw43_poll_thread),
        NORMALPRIO + 1,
        cyw43_poll_thread_func,
        NULL);
}

void cyw43_arch_deinit(void)
{
    if (cyw43_poll_thread != NULL)
    {
        chThdTerminate(cyw43_poll_thread);
        chEvtBroadcast(&cyw43_poll_event); // Wake thread to see termination flag
        cyw43_poll_thread = NULL;
    }
    cyw43_poll_func = NULL;
}

// ---------------------------------------------------------------------------
// WiFi connection state tracking
// ---------------------------------------------------------------------------
static volatile int cyw43_wifi_connected = 0;
static volatile int cyw43_connect_result = -1;

// Called from the lwIP thread or CYW43 event handler when link state changes
void cyw43_set_wifi_connected(int connected, int result)
{
    cyw43_wifi_connected = connected;
    cyw43_connect_result = result;
}

int cyw43_is_wifi_connected(void)
{
    return cyw43_wifi_connected;
}

int cyw43_get_connect_result(void)
{
    return cyw43_connect_result;
}

// WiFi scan state
static volatile int cyw43_scan_active = 0;

void cyw43_set_scan_active(int active)
{
    cyw43_scan_active = active;
}

int cyw43_is_scan_active(void)
{
    return cyw43_scan_active;
}
