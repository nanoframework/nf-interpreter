//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

// This file includes the board specific Network Intialisation

#include <nanoHAL.h>
#include "Esp32_os.h"

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

	// get network configuration
	// FIXME
	
	// TODO currently initialised in  'initialise_wifi_smart_config.c'
	// tcpip_adapter_init(); 
	
	// Callback to Network stack when init complete 
	initfunc(NULL);
}

// Gets the network configuration block from the EPS32 storage
bool GetConfigurationNetwork(Configuration_Network* configurationNetwork)
{
    // copy the config block content to the pointer in the argument
	// FIXME 

    return FALSE;
}

// Stores the network configuration block to the EPS32 storage 
bool StoreConfigurationNetwork(Configuration_Network* configurationNetwork)
{
    // copy the config block content to the pointer in the argument
	// FIXME 

    return FALSE;
}
