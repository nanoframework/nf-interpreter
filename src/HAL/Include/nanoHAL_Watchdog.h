//
// Copyright (c) 2018 The nanoFramework project contributors
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
    void Watchdog_SetMaxTimeout();
    void Watchdog_SetTimeout(uint16_t timeoutMillis);
    bool Watchdog_Start();
    bool Watchdog_IsRunning();
    void Watchdog_Reset();
    void Watchdog_Stop();
#ifdef __cplusplus
}
#endif

#endif