/*
 * Copyright (c) Ericsson AB 2020, all rights reserved
 */

#include <zephyr.h>
#include "nf_lib.h"

// fallback define for non-kconfig builds.
#ifndef CONFIG_DBG_PORT_NAME
#define CONFIG_DBG_PORT_NAME "UART_1"
#endif // CONFIG_DBG_PORT_NAME

//========================================================
// Program Entry Point
//========================================================
void main(void)
{
    printk("\nnrf9160DK greets\n");

    // Invoke the NF runtime
    int rc = 0;

    rc = start_nf(CONFIG_DBG_PORT_NAME);
    printk("start_nf returned %d\n", rc);

    while (1)
    {
        k_sleep(K_MSEC(30000));
    }
}
