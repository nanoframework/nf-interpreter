//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// ChibiOS implementation of the deployment confirmation watchdog timer hooks (nanoCLR side).
// See MCUboot/docs/confirmation-timeout.md.

#include <ch.h>
#include <hal.h>
#include <nanoHAL_v2.h>
#include <MCUboot_StartupPolicy.h>

// allocated only while armed
static virtual_timer_t *s_confirmTimer = NULL;

static void ConfirmTimerExpired(virtual_timer_t *vtp, void *arg)
{
    (void)vtp;
    (void)arg;

    // deployment wasn't confirmed in time: reset so MCUboot reverts it
    NVIC_SystemReset();
}

bool nf_mcuboot_confirm_timer_start(uint32_t seconds)
{
    if (s_confirmTimer != NULL)
    {
        return false;
    }

    // keep the interval inside sysinterval_t
    const uint64_t maxSeconds = (uint64_t)TIME_MAX_INTERVAL / CH_CFG_ST_FREQUENCY;

    if ((uint64_t)seconds > maxSeconds)
    {
        seconds = (uint32_t)maxSeconds;
    }

    s_confirmTimer = (virtual_timer_t *)platform_malloc(sizeof(virtual_timer_t));

    if (s_confirmTimer == NULL)
    {
        return false;
    }

    chVTObjectInit(s_confirmTimer);
    chVTSet(s_confirmTimer, TIME_S2I(seconds), ConfirmTimerExpired, NULL);

    return true;
}

void nf_mcuboot_confirm_timer_stop(void)
{
    if (s_confirmTimer != NULL)
    {
        chVTReset(s_confirmTimer);
        platform_free(s_confirmTimer);
        s_confirmTimer = NULL;
    }
}
