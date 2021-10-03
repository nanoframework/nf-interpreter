//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include <stdio.h>

#include <hal.h>
#include <hal_nf_community.h>
//#include "sntp_client.h"
#include "stm_networking.h"

#include <tx_api.h>
//#include "legacy/mqtt.h"
#include "nx_client.h"

#include "azure_config.h"

#include <targetHAL.h>
#include <nanoHAL_v2.h>
#include <nanoPAL_COM.h>
#include <target_stdio_config.h>

// Azure Thread
#define AZURE_THREAD_STACK_SIZE 4096
#define AZURE_THREAD_PRIORITY   6
TX_THREAD azure_thread;
ULONG azure_thread_stack[AZURE_THREAD_STACK_SIZE / sizeof(ULONG)];
void azure_thread_entry(ULONG parameter);

void azure_thread_define(void *first_unused_memory)
{
    (void)first_unused_memory;

    // systick_interval_set(TX_TIMER_TICKS_PER_SECOND);

    // Create Azure thread
    uint16_t status = tx_thread_create(
        &azure_thread,
        "Azure Thread",
        azure_thread_entry,
        0,
        azure_thread_stack,
        AZURE_THREAD_STACK_SIZE,
        AZURE_THREAD_PRIORITY,
        AZURE_THREAD_PRIORITY,
        TX_NO_TIME_SLICE,
        TX_AUTO_START);

    if (status != TX_SUCCESS)
    {
        printf("Azure IoT thread creation failed\r\n");
    }
}

void azure_thread_entry(ULONG parameter)
{
    (void)parameter;
    // UINT status;

    printf("\r\nStarting Azure thread\r\n\r\n");

    // Initialize the network
    if (stm32_network_init(WIFI_SSID, WIFI_PASSWORD, WIFI_MODE) != NX_SUCCESS)
    {
        printf("Failed to initialize the network\r\n");
        return;
    }
    /****
        // Start the SNTP client
        status = sntp_start();
        if (status != NX_SUCCESS)
        {
            printf("Failed to start the SNTP client (0x%02x)\r\n", status);
            return;
        }

        // Wait for an SNTP sync
        status = sntp_sync_wait();
        if (status != NX_SUCCESS)
        {
            printf("Failed to start sync SNTP time (0x%02x)\r\n", status);
            return;
        }

    #ifdef ENABLE_LEGACY_MQTT
        if ((status = azure_iot_mqtt_entry(&nx_ip, &nx_pool, &nx_dns_client, sntp_time_get)))
    #else
        if ((status = azure_iot_nx_client_entry(&nx_ip, &nx_pool, &nx_dns_client, sntp_time)))
    #endif
        {
            printf("Failed to run Azure IoT (0x%04x)\r\n", status);
            return;
        }
    ****/
}
