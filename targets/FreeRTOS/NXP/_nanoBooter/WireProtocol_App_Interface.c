//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <WireProtocol_MonitorCommands.h>

// defining this array here makes is local helping reduce the image size because of compiler opmitizations
const CommandHandlerLookup c_Lookup_Request[] = {
/*******************************************************************************************************************************************************************/
#define DEFINE_CMD(cmd)                                                                                                \
    {                                                                                                                  \
        CLR_DBG_Commands_c_Monitor_##cmd, &Monitor_##cmd                                                               \
    }
    DEFINE_CMD(Ping),
    DEFINE_CMD(Reboot),
    // //
    DEFINE_CMD(ReadMemory),
    DEFINE_CMD(WriteMemory),
    DEFINE_CMD(CheckMemory),
    DEFINE_CMD(EraseMemory),
    DEFINE_CMD(QueryConfiguration),
    DEFINE_CMD(UpdateConfiguration),
    // //
    // DEFINE_CMD(Execute    ),
    DEFINE_CMD(MemoryMap),
    // //
    // DEFINE_CMD(CheckSignature),
    // //
    DEFINE_CMD(FlashSectorMap),
    // DEFINE_CMD(SignatureKeyUpdate),

    DEFINE_CMD(OemInfo),
    DEFINE_CMD(TargetInfo),

#undef DEFINE_CMD
    /*******************************************************************************************************************************************************************/
};

////////////////////////////////////////////////////

// defining this array here makes is local helping reduce the image size because of compiler opmitizations
const CommandHandlerLookup c_Lookup_Reply[] = {
/*******************************************************************************************************************************************************************/
#define DEFINE_CMD(cmd)                                                                                                \
    {                                                                                                                  \
        CLR_DBG_Commands_c_Monitor_##cmd, &Monitor_##cmd                                                               \
    }
    DEFINE_CMD(Ping),
#undef DEFINE_CMD
    /*******************************************************************************************************************************************************************/
};

uint8_t GetSizeOfLookup_Reply()
{
    return ARRAYSIZE(c_Lookup_Reply);
}

uint8_t GetSizeOfLookup_Request()
{
    return ARRAYSIZE(c_Lookup_Request);
}
