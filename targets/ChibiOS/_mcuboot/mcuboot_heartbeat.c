//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// LED heartbeat for the MCUboot bootloader.
//
// Blinks the board's user LED for as long as the bootloader is running, so that a long
// operation reads as "working" rather than "hung".
//

#include <ch.h>
#include <hal.h>

#include <mcuboot_board_iface.h>

#if defined(CONFIG_NF_FEATURE_MCUBOOT_HEARTBEAT) && CONFIG_NF_FEATURE_MCUBOOT_HEARTBEAT

// Toggle interval for each cadence, in milliseconds.
#define HEARTBEAT_PERIOD_BOOT_MS     500
#define HEARTBEAT_PERIOD_RECOVERY_MS 100

// Read once per iteration by the heartbeat thread and written by
// mcuboot_heartbeat_set_pattern(). A plain aligned word store, so no lock is required.
static volatile uint32_t s_heartbeat_period_ms = HEARTBEAT_PERIOD_BOOT_MS;

// The loop is a pin toggle and a sleep, so it needs no more than the port's mandatory
// interrupt frame plus a trivial stack frame.
static THD_WORKING_AREA(wa_heartbeat, 256);

static THD_FUNCTION(heartbeat_thd, arg)
{
    (void)arg;

    while (true)
    {
        mcuboot_heartbeat_led_toggle();

        chThdSleepMilliseconds(s_heartbeat_period_ms);
    }
}

void mcuboot_heartbeat_start(void)
{
    chThdCreateStatic(wa_heartbeat, sizeof(wa_heartbeat), NORMALPRIO + 1, heartbeat_thd, NULL);
}

void mcuboot_heartbeat_set_pattern(mcuboot_heartbeat_pattern_t pattern)
{
    s_heartbeat_period_ms =
        (pattern == MCUBOOT_HEARTBEAT_RECOVERY) ? HEARTBEAT_PERIOD_RECOVERY_MS : HEARTBEAT_PERIOD_BOOT_MS;
}

#else // CONFIG_NF_FEATURE_MCUBOOT_HEARTBEAT

void mcuboot_heartbeat_start(void)
{
}

void mcuboot_heartbeat_set_pattern(mcuboot_heartbeat_pattern_t pattern)
{
    (void)pattern;
}

#endif // CONFIG_NF_FEATURE_MCUBOOT_HEARTBEAT

// Weak no-op for boards with no suitable user LED. Boards override this in their own
// mcuboot_heartbeat_led.c.
__attribute__((weak)) void mcuboot_heartbeat_led_toggle(void)
{
}
