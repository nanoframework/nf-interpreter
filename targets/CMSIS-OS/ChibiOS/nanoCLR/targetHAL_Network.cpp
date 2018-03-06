//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

// This file includes the platform and board specific Network Intialisation

#include <nanoHAL.h>
#include "lwipthread.h"


// this is the declaration for the callback implement in nf_sys_arch.c 
extern "C" void set_signal_sock_function( void (*funcPtr)() );

//
// Callback from lwIP on event
//
void sys_signal_sock_event()
{
     Events_Set(SYSTEM_EVENT_FLAG_SOCKET);
}

void nanoHAL_Network_Initialize(tcpip_init_done_fn initfunc)
{
    // Initialise the lwIP CLR signal callback
    set_signal_sock_function( &sys_signal_sock_event );

    // Start lwIP thread in ChibiOS bindings using default options
    //lwipInit(NULL));

    const uint8_t localMACAddress[6] = { 0, 0x80U, 0xe1U, 0x01U, 0x35U, 0x56U };

    struct lwipthread_opts lwipOptions = { 
        (uint8_t *)localMACAddress,
        0, //IP4_ADDR(&ip, 192, 168, 1, 199),
        0, //IP4_ADDR(&netmask, 255, 255, 255, 0),
        0, //IP4_ADDR(&gateway, 192, 168, 1, 2),
        NET_ADDRESS_DHCP, //   NET_ADDRESS_STATIC, 
        "nanodevice"};

    // Start lwIP thread in ChibiOS bindings using the above options
    lwipInit(&lwipOptions);

    // Callback to Network stack when init complete 
    initfunc(NULL);
}
