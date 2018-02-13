//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

// This file includes the board specific Network Intialisation

#include <nanoHAL.h>

#include "lwipthread.h"

void nanoHAL_Network_Initialize(tcpip_init_done_fn initfunc)
{
    // Start Lwip thread in CHibios bindings using default options   
    // can pass options to setup ethernet interfaces, NULL use defaults
    lwipInit(NULL);

    // Callback to Network stack when init complete (tcpip_init_done)
    initfunc(NULL);
}