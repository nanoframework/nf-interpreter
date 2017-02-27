//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#ifndef _NANOCLR_APPLICATION_H_
#define _NANOCLR_APPLICATION_H_

typedef struct CLR_SETTINGS
{
	unsigned short  MaxContextSwitches;
	bool            WaitForDebugger;
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
