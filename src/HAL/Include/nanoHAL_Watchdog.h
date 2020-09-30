//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#ifndef _NANOHAL_WATCHDOG_H_
#define _NANOHAL_WATCHDOG_H_ 1

#include <nanoWeak.h>   

#ifdef __cplusplus
extern "C" {
#endif
    //Watchdog functions
    void Watchdog_Init();
    void Watchdog_Reset();
#ifdef __cplusplus
}
#endif

#endif
