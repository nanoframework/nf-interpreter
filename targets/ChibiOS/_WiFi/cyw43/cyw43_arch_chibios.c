//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// CYW43 architecture layer for ChibiOS/RT.
// Provides initialization, synchronization primitives, and poll scheduling.

#include <ch.h>
#include <hal.h>
#include "cyw43_configport.h"
#include "cyw43.h"
#include "cyw43_country.h"
#include "cyw43_internal.h"
#include "cyw43_spi.h"

// ---------------------------------------------------------------------------
// MAC address — matches Pico SDK: returns cyw43_state.mac (set from OTP
// during firmware init).  Before init, returns zeros.
// ---------------------------------------------------------------------------
void cyw43_hal_get_mac(int itf, uint8_t mac[6])
{
    (void)itf;
    memcpy(mac, cyw43_state.mac, 6);
}

// CLR event system — fire SYSTEM_EVENT_FLAG_WIFI_STATION to wake NativeConnect
extern void Events_Set(uint32_t events);
#define SYSTEM_EVENT_FLAG_WIFI_STATION 0x01000000

// ---------------------------------------------------------------------------
// Synchronization primitives
// ---------------------------------------------------------------------------
// Recursive lock: CYW43 driver nests CYW43_THREAD_ENTER/EXIT, so we track
// the owner thread and a recursion depth. Only the outermost unlock releases
// the underlying ChibiOS mutex.
static mutex_t cyw43_mutex;
static volatile thread_t *cyw43_lock_owner = NULL;
static volatile int cyw43_lock_depth = 0;

binary_semaphore_t cyw43_sdpcm_sem;
binary_semaphore_t cyw43_ioctl_sem;

void cyw43_thread_enter(void)
{
    thread_t *self = chThdGetSelfX();
    if (cyw43_lock_owner == self)
    {
        // Same thread — just bump the depth
        cyw43_lock_depth++;
        return;
    }
    chMtxLock(&cyw43_mutex);
    cyw43_lock_owner = self;
    cyw43_lock_depth = 1;
}

void cyw43_thread_exit(void)
{
    if (--cyw43_lock_depth == 0)
    {
        cyw43_lock_owner = NULL;
        chMtxUnlock(&cyw43_mutex);
    }
}

// Forward declarations
static void (*cyw43_poll_func)(void);
static event_source_t cyw43_poll_event;

// Wait functions for SDPCM send stalls and ioctl response polling.
//
// IMPORTANT: These must NOT release the CYW43 mutex.  The callers
// (cyw43_do_ioctl, cyw43_sdpcm_send_common) already poll the device
// inline via cyw43_ll_sdpcm_poll_device().  If we release the mutex
// here, the background poll thread can run cyw43_ll_process_packets()
// which consumes response packets (including CONTROL_HEADER) that the
// ioctl loop is waiting for — CONTROL_HEADER has no handler in
// process_packets, so the response is silently discarded and the
// ioctl times out.  Keeping the mutex held avoids this race while the
// inline polling in the caller handles all packet types correctly.
void cyw43_sdpcm_wait(void)
{
    chThdSleep(TIME_MS2I(1));
}

void cyw43_ioctl_wait(void)
{
    chThdSleep(TIME_MS2I(1));
}

// Poll thread
static thread_t *cyw43_poll_thread = NULL;

// CYW43 poll thread — runs CYW43 internal polling when signaled
#define CYW43_POLL_THREAD_STACK_SIZE 2048
static THD_WORKING_AREA(wa_cyw43_poll_thread, CYW43_POLL_THREAD_STACK_SIZE);

static THD_FUNCTION(cyw43_poll_thread_func, arg)
{
    (void)arg;
    event_listener_t el;

    chRegSetThreadName("cyw43poll");
    chEvtRegister(&cyw43_poll_event, &el, 0);

    while (true)
    {
        // Wake on event signal OR every 10 ms for periodic polling.
        // The CYW43 interrupt pin (GP24) is not wired to a GPIO IRQ in
        // our ChibiOS port, so regular polling is the only way to drain
        // async events (scan results, link state, etc.).
        chEvtWaitAnyTimeout(EVENT_MASK(0), TIME_MS2I(10));
        chEvtGetAndClearEvents(EVENT_MASK(0));

        if (cyw43_poll_func != NULL)
        {
            // Use try-lock: if main thread holds the lock (e.g. sending
            // an ioctl), skip this poll cycle — the main thread will
            // release the lock during its CYW43_DO_IOCTL_WAIT and we
            // will poll on the next cycle.
            if (chMtxTryLock(&cyw43_mutex))
            {
                cyw43_lock_owner = chThdGetSelfX();
                cyw43_lock_depth = 1;

                cyw43_poll_func();

                cyw43_lock_depth = 0;
                cyw43_lock_owner = NULL;
                chMtxUnlock(&cyw43_mutex);

                // Fire Event_Wifi_Station every ~500ms (50 polls × 10ms) to
                // wake NativeConnect's WaitEvents loop for periodic re-check.
                {
                    static uint32_t event_fire_counter = 0;
                    if (++event_fire_counter >= 50) {
                        event_fire_counter = 0;
                        Events_Set(SYSTEM_EVENT_FLAG_WIFI_STATION);
                    }
                }
            }
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
// Architecture initialization (lightweight — no hardware access)
// ---------------------------------------------------------------------------
static volatile int cyw43_hw_initialized = 0;

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

    // NOTE: cyw43_init() and cyw43_wifi_set_up() are deferred to
    // cyw43_ensure_wifi_up() so that CLR startup is not blocked by
    // the ~500ms SPI firmware load.
}

// ---------------------------------------------------------------------------
// Lazy WiFi hardware initialization — called before scan/connect
// ---------------------------------------------------------------------------

int cyw43_ensure_wifi_up_impl(void)
{
    if (cyw43_hw_initialized)
    {
        return 0;
    }

    // cyw43_init() only initializes driver state and sets GPIO modes.
    // It does NOT do SPI communication — that happens inside
    // cyw43_wifi_set_up() → cyw43_ensure_up() → cyw43_ll_bus_init().
    cyw43_init(&cyw43_state);

    // Power on WiFi, do SPI bus init, load firmware, enable STA interface.
    cyw43_wifi_set_up(&cyw43_state, CYW43_ITF_STA, true, CYW43_COUNTRY_WORLDWIDE);

    cyw43_hw_initialized = 1;

    return (cyw43_state.itf_state != 0) ? 0 : -1;
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
