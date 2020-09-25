//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoCLR_Runtime.h>
#include <nanoCLR_Debugging.h>

#define DEFINE_CMD(cmd)  { CLR_DBG_Debugger::Debugging_##cmd, CLR_DBG_Commands::c_Debugging_##cmd }
#define DEFINE_CMD2(cmd) { CLR_DBG_Debugger::Monitor_##cmd  , CLR_DBG_Commands::c_Monitor_##cmd   }

const CLR_Messaging_CommandHandlerLookup c_Debugger_Lookup_Request[] =
{
    DEFINE_CMD2(Ping       ),
    DEFINE_CMD2(Reboot     ),
    DEFINE_CMD(Execution_QueryCLRCapabilities),

    DEFINE_CMD2(ReadMemory ),
    DEFINE_CMD2(WriteMemory),
    DEFINE_CMD2(EraseMemory),
    DEFINE_CMD2(QueryConfiguration),
    DEFINE_CMD2(UpdateConfiguration),
    //
    DEFINE_CMD2(Execute    ),
    DEFINE_CMD2(MemoryMap  ),
    DEFINE_CMD2(FlashSectorMap),

    DEFINE_CMD(UpgradeToSsl),







};

const CLR_Messaging_CommandHandlerLookup c_Debugger_Lookup_Reply[] =
{
    DEFINE_CMD2(Ping),
};

#undef DEFINE_CMD
#undef DEFINE_CMD2

const CLR_UINT32 c_Debugger_Lookup_Request_count = ARRAYSIZE(c_Debugger_Lookup_Request);
const CLR_UINT32 c_Debugger_Lookup_Reply_count   = ARRAYSIZE(c_Debugger_Lookup_Reply);
