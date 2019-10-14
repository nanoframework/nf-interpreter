//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _NANOCLR_APPLICATION_H_
#define _NANOCLR_APPLICATION_H_

#include <stdbool.h>

typedef struct CLR_SETTINGS
{
	// this is the maximum number of context switches that execution engine thread scheduler can handle
	// higher number: more threads and timers can be handled
	unsigned short  MaxContextSwitches;

	// set this to TRUE if the default behaviour is for the execution engine to wait for a debugger to be connected
	// when building is set for RTM this configuration is ignored
	bool            WaitForDebugger;

	// set this to TRUE if a connection from a debugger is to be awaited after the execution engine terminates
	// this is required for launching a debug session in Visual Studio
	// when building is set for RTM this configuration is ignored
	bool            EnterDebuggerLoopAfterExit;
}CLR_SETTINGS;

#ifdef __cplusplus
extern "C" {
#endif

	extern void ClrStartup(CLR_SETTINGS params);

	extern void ClrExit();

#ifdef __cplusplus
}
#endif

#endif
