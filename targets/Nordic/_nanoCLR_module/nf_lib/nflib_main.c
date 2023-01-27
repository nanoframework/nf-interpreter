//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <zephyr.h>
#include "nf_lib.h"
#include <drivers/uart.h>
#include <device.h>

#include <stdio.h>
#include <stdlib.h>

#include "nanoCLR_Application.h"
#include "nanoPAL_BlockStorage.h"

CLR_SETTINGS params;

extern void *g_heapbase;
extern uint32_t g_heapsize;

// device handle to wire uart
extern const struct device *g_wire_uart;

static K_THREAD_STACK_DEFINE(wp_thread_stack, /*CONFIG_BT_HCI_TX_STACK_SIZE*/ 3072);
static struct k_thread wp_thread_data;

void ReceiverThread(void *p1, void *p2, void *p3);
int wire_port_init(char *portName);

extern HAL_SYSTEM_CONFIG HalSystemConfig;
void CLRStartupThread(void *arg);

//
// NF Module entry point. This will invoke the nanoFramework and return.
//
int start_nf(const char *wire_uart_name)
{
    int rc = 0;
    printk("nf_lib entry\n");

    //	g_heapsize = (0x8000-32) * 2; //64k - overhead
    g_heapsize = (0x8800 - 32); // 48k - overhead
    //g_heapsize = (0x8000 - 32); // 32k - overhead
    g_heapbase = k_malloc(g_heapsize);

    if (g_heapbase != 0)
    {
        // Set these here for debugger.
        HalSystemConfig.RAM1.Base = (uint32_t)g_heapbase;
        HalSystemConfig.RAM1.Size = g_heapsize;

        //
        // Do this before starting the CLR
        //

        BlockStorageList_Initialize();
        BlockStorage_AddDevices();

        // Open the wire protocol UART device
        //        g_wire_uart = device_get_binding(CONFIG_DBG_PORT_NAME);
        g_wire_uart = device_get_binding(wire_uart_name);

        if (g_wire_uart)
        {
            // Create thread for wire protocol debugger
            k_thread_create(
                &wp_thread_data,
                wp_thread_stack,
                K_THREAD_STACK_SIZEOF(wp_thread_stack),
                ReceiverThread,
                NULL,
                NULL,
                NULL,
                K_PRIO_COOP(3),
                0,
                K_NO_WAIT);
        }
        else
        {
            printk("Wire port %s init failed\n", wire_uart_name);
        }

        printk("Starting .net nf\n");

        // CLR settings to launch CLR thread
        CLR_SETTINGS clrSettings;
        (void)memset(&clrSettings, 0, sizeof(CLR_SETTINGS));

        clrSettings.MaxContextSwitches = 50;
        clrSettings.WaitForDebugger = false;
        clrSettings.EnterDebuggerLoopAfterExit = true;

        CLRStartupThread(&clrSettings);

        printk("ClrStartup returned.\n");

        printk("Clr returned. HALT!!!");

        while (1)
        {
            k_sleep(K_MSEC(1000));
        }
    }
    else
    {
        printk("**** Error failed to allocate managed heap of %d bytes.\n", g_heapsize);
        rc = -1;
    }

    return rc;
}