//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#ifndef _NANOCLR_APPLICATION_H_
#define _NANOCLR_APPLICATION_H_

struct CLR_SETTINGS
{
    unsigned short  MaxContextSwitches;
    bool            WaitForDebugger;
    bool            EnterDebuggerLoopAfterExit;
};

extern void ClrStartup(CLR_SETTINGS params);

extern void ClrExit();

#endif

