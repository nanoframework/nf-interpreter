//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef NANOCLR_APPLICATION_H
#define NANOCLR_APPLICATION_H

#include <stdbool.h>

typedef struct CLR_SETTINGS
{
    // this is the maximum number of context switches that execution engine thread scheduler can handle
    // higher number: more threads and timers can be handled
    unsigned short MaxContextSwitches;

    // set this to TRUE if the default behaviour is for the execution engine to wait for a debugger to be connected
    // when building is set for RTM this configuration is ignored
    bool WaitForDebugger;

    // set this to TRUE if a connection from a debugger is to be awaited after the execution engine terminates
    // this is required for launching a debug session in Visual Studio
    // when building is set for RTM this configuration is ignored
    bool EnterDebuggerLoopAfterExit;

    // Set this to TRUE if the device is to reboot to nanoBoother (or proprietary bootloader) in case the there is a
    // fault in loading the application.
    // This option is only available when building is set for RTM
    bool RevertToBooterOnFault;

#if defined(VIRTUAL_DEVICE)
    bool PerformGarbageCollection;
    bool PerformHeapCompaction;
#endif

} CLR_SETTINGS;

#ifdef __cplusplus
extern "C"
{
#endif

    extern void ClrStartup(CLR_SETTINGS params);

    extern void ClrExit();

#ifdef __cplusplus
}
#endif

#endif
