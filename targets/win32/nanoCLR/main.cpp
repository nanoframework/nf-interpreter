//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "stdafx.h"

#include <nanoCLR_Application.h>
#include <iostream>

//
// UNDONE: Feature configuration
//

#define NF_CLR_FEATURE_ALL

#if defined(NF_CLR_FEATURE_ALL)

#pragma comment(lib, "WireProtocol.lib")

#pragma comment(lib, "Debugger.lib")
#pragma comment(lib, "Diagnostics.lib")
#pragma comment(lib, "Hardware.lib")
#pragma comment(lib, "InterruptHandler.lib")
#pragma comment(lib, "Messaging.lib")
#pragma comment(lib, "RPC.lib")
#pragma comment(lib, "Serialization.lib")

#else

#pragma comment(                                                                                                       \
    lib,                                                                                                               \
    "WireProtocol.lib") // UNDONE: FIXME: SUPPORT_ComputeCRC required by TypeSystem.cpp, CLR_RT_HeapBlock

#pragma comment(lib, "Debugger_stub.lib")
#pragma comment(lib, "Diagnostics_stub.lib")
#pragma comment(lib, "Hardware_stub.lib")
#pragma comment(lib, "InterruptHandler_stub.lib")
#pragma comment(lib, "IOPort_stub.lib")
#pragma comment(lib, "Messaging_stub.lib")
#pragma comment(lib, "RPC_stub.lib")
#pragma comment(lib, "Serialization_stub.lib")

#endif

/////////////////////////////////////////////////////////////////////////////

// All solutions are expected to provide an implementation of this
bool Target_GetReleaseInfo(NFReleaseInfo &releaseInfo)
{
    NFReleaseInfo::Init(
        releaseInfo,
        VERSION_MAJOR,
        VERSION_MINOR,
        VERSION_BUILD,
        VERSION_REVISION,
        OEMSYSTEMINFOSTRING,
        hal_strlen_s(OEMSYSTEMINFOSTRING),
        TARGETNAMESTRING,
        hal_strlen_s(TARGETNAMESTRING),
        PLATFORMNAMESTRING,
        hal_strlen_s(PLATFORMNAMESTRING));

    return true; // alternatively, return false if you didn't initialize the releaseInfo structure.
}

/////////////////////////////////////////////////////////////////////////////
//

int _tmain(int argc, _TCHAR *argv[])
{
    wprintf(L"\n.NET nanoFramework nanoCLR WIN32 v");
    std::cout << VERSION_STRING;
    wprintf(L"\r\nCopyright (c) 2020 .NET Foundation and Contributors\r\n\r\n");

    // initialize nanoHAL
    nanoHAL_Initialize();

    CLR_SETTINGS clrSettings;
    ZeroMemory(&clrSettings, sizeof(clrSettings));

    clrSettings.MaxContextSwitches = 50;
    clrSettings.WaitForDebugger = false;
    clrSettings.EnterDebuggerLoopAfterExit = false;

    // fill arguments from command line
    clrSettings.StartArgs.resize(argc - 1);

    for (int i = 0; i < argc - 1; i++)
    {
        std::wstring argv_ = std::wstring(argv[1 + i]);
        clrSettings.StartArgs[i] = argv_;
    }

    ClrStartup(clrSettings);

#if !defined(BUILD_RTM)
    CLR_Debug::Printf("Exiting.\r\n");
#endif

    return 0;
}
