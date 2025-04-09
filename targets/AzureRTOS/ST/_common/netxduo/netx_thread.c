//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <hal.h>
#include <nx_user.h>
#include <nx_api.h>
#include <nxd_bsd.h>
#include <nxd_dns.h>
#include <targetHAL.h>

// only need this when using ETH with ChibiOS MAC driver
#if HAL_USE_MAC == TRUE

__attribute__((noreturn)) void NetXDuoThread_entry(uint32_t parameter)
{
    (void)parameter;

    while (1)
    {
        tx_thread_sleep(TX_TICKS_PER_MILLISEC(5000));

        macPollLinkStatus(&ETHD1);
    }

    // this never returns
}

#endif