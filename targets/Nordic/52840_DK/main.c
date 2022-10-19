//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Ericsson AB 2020, all rights reserved
// See LICENSE file in the project root for full license information.
//

#include <zephyr.h>
#include <nf_lib.h>

// fallback define for non-kconfig builds.
#ifndef CONFIG_DBG_PORT_NAME
#define CONFIG_DBG_PORT_NAME "UART_1"
#endif // CONFIG_DBG_PORT_NAME

void main(void)
{
    printk("\nnrf52840DK greets\n");

    // Invoke the NF runtime
    int rc = 0;

    rc = start_nf(CONFIG_DBG_PORT_NAME);
    printk("start_nf returned %d\n", rc);

    while (1)
    {
        k_sleep(K_MSEC(30000));
    }
}
