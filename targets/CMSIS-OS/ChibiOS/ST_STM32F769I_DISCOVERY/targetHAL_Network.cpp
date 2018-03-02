//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

// This file includes the board specific Network Intialisation

#include <nanoHAL.h>

#include "lwipthread.h"

extern "C" void set_signal_sock_function( void (*funcPtr)() );

//
// Callback from LWIP on event
//
void sys_signal_sock_event()
{
     Events_Set(SYSTEM_EVENT_FLAG_SOCKET);
}

void nanoHAL_Network_Initialize(tcpip_init_done_fn initfunc)
{
    // Initialise the Lwip CLR signal callback
    set_signal_sock_function( &sys_signal_sock_event );

    // Start Lwip thread in CHibios bindings using default options    
    lwipInit(NULL);

    // Callback to Network stack when init complete 
    initfunc(NULL);
}