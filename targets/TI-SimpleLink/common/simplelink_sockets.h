//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Texas Instruments Incorporated.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _SIMPLELINK_SOCKETS_H_
#define _SIMPLELINK_SOCKETS_H_

#include <nanoHAL.h>
#include <nanoHAL_ConfigurationManager.h>

#define NATIVE_PROFILE_PAL_NETWORK()

#ifdef __cplusplus
extern "C" {
#endif

bool SimpleLink_SOCKETS_Initialize();
bool SimpleLink_SOCKETS_Uninitialize();

void Status_callback();
void Link_callback(bool linkUp);
// void PostAddressChanged(void* arg);
// void PostAvailabilityOn(void* arg);
// void PostAvailabilityOff(void* arg);

#ifdef __cplusplus
}
#endif

#endif // _SIMPLELINK_SOCKETS_H_ 1
