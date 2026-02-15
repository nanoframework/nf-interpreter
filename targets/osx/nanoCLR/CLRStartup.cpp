//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <iostream>

// TODO: Integrate shared CLR startup from src/CLR/Startup/CLRStartup.cpp once
// VIRTUAL_DEVICE code paths are made host-portable on macOS.

extern "C"
{
    typedef struct CLR_SETTINGS
    {
        unsigned short MaxContextSwitches;
        bool WaitForDebugger;
        bool EnterDebuggerLoopAfterExit;
        bool RevertToBooterOnFault;
#if defined(VIRTUAL_DEVICE)
        bool PerformGarbageCollection;
        bool PerformHeapCompaction;
#endif
    } CLR_SETTINGS;

    void ClrExit()
    {
    }

    void ClrReboot()
    {
    }

    void ClrStartup(CLR_SETTINGS params)
    {
        (void)params;

        std::cerr << "TODO: ClrStartup for macOS host target is not implemented in this PR skeleton.\n";
    }
}
