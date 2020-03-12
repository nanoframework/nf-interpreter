//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Debugging.h>

#define DEFINE_CMD(cmd)  {  CLR_DBG_Debugger::Debugging_##cmd, CLR_DBG_Commands::c_Debugging_##cmd }
#define DEFINE_CMD2(cmd) {  CLR_DBG_Debugger::Monitor_##cmd  , CLR_DBG_Commands_c_Monitor_##cmd    }
#define DEFINE_CMD3(cmd) { &CLR_DBG_Debugger::Profiling_##cmd, CLR_DBG_Commands::c_Profiling_##cmd }

const CLR_Messaging_CommandHandlerLookup c_Debugger_Lookup_Request[] =
{
    DEFINE_CMD(Execution_QueryCLRCapabilities),
    DEFINE_CMD2(Ping       ),
    DEFINE_CMD2(ReadMemory ),
    DEFINE_CMD2(WriteMemory),
    DEFINE_CMD2(CheckMemory),
    DEFINE_CMD2(EraseMemory),
    DEFINE_CMD2(QueryConfiguration),
    DEFINE_CMD2(UpdateConfiguration),
    //
    DEFINE_CMD2(Execute    ),
    DEFINE_CMD2(Reboot     ),
    DEFINE_CMD2(MemoryMap  ),
    DEFINE_CMD2(FlashSectorMap),
    DEFINE_CMD2(DeploymentMap),
    //
    DEFINE_CMD(Execution_BasePtr         ),
    DEFINE_CMD(Execution_ChangeConditions),
    //
    DEFINE_CMD(Execution_Allocate        ),

    DEFINE_CMD(UpgradeToSsl),








#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
    DEFINE_CMD(Execution_SetCurrentAppDomain),
    DEFINE_CMD(Execution_Breakpoints        ),
    DEFINE_CMD(Execution_BreakpointStatus   ),
    //
    DEFINE_CMD(Thread_CreateEx           ),
    DEFINE_CMD(Thread_List               ),
    DEFINE_CMD(Thread_Stack              ),
    DEFINE_CMD(Thread_Kill               ),
    DEFINE_CMD(Thread_Suspend            ),
    DEFINE_CMD(Thread_Resume             ),
    DEFINE_CMD(Thread_GetException       ),
    DEFINE_CMD(Thread_Unwind             ),
    DEFINE_CMD(Thread_Get                ),
    //
    DEFINE_CMD(Stack_Info                ),
    DEFINE_CMD(Stack_SetIP               ),
    //
    DEFINE_CMD(Value_ResizeScratchPad    ),
    DEFINE_CMD(Value_GetStack            ),
    DEFINE_CMD(Value_GetField            ),
    DEFINE_CMD(Value_GetArray            ),
    DEFINE_CMD(Value_GetBlock            ),
    DEFINE_CMD(Value_GetScratchPad       ),
    DEFINE_CMD(Value_SetBlock            ),
    DEFINE_CMD(Value_SetArray            ),
    DEFINE_CMD(Value_AllocateObject      ),
    DEFINE_CMD(Value_AllocateString      ),
    DEFINE_CMD(Value_AllocateArray       ),
    DEFINE_CMD(Value_Assign              ),
    //
    DEFINE_CMD(TypeSys_Assemblies        ),
    DEFINE_CMD(TypeSys_AppDomains        ),
    //
    DEFINE_CMD(Resolve_AppDomain         ),
    DEFINE_CMD(Resolve_Assembly          ),
    DEFINE_CMD(Resolve_Type              ),
    DEFINE_CMD(Resolve_Field             ),
    DEFINE_CMD(Resolve_Method            ),
    DEFINE_CMD(Resolve_VirtualMethod     ),
#endif
    //
#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
    DEFINE_CMD(Deployment_Status         ),
    DEFINE_CMD(Info_SetJMC               ),
#endif    
    //
#if defined(NANOCLR_PROFILE_NEW)
    DEFINE_CMD3(Command),
#endif
    //
};

const CLR_Messaging_CommandHandlerLookup c_Debugger_Lookup_Reply[] =
{
    DEFINE_CMD2(Ping),
};

#undef DEFINE_CMD
#undef DEFINE_CMD2
#undef DEFINE_CMD3

const CLR_UINT32 c_Debugger_Lookup_Request_count = ARRAYSIZE(c_Debugger_Lookup_Request);
const CLR_UINT32 c_Debugger_Lookup_Reply_count   = ARRAYSIZE(c_Debugger_Lookup_Reply);

