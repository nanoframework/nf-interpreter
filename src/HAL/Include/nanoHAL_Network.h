//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _NANOHAL_NETWORK_H_
#define _NANOHAL_NETWORK_H_ 1

// Callback for when init done
typedef void(* tcpip_init_done_fn) (void *arg);

void 	nanoHAL_Network_Initialize(tcpip_init_done_fn initfunc);


#endif //_NANOHAL_NETWORK_H_
