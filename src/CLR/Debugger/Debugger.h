//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _DEBUGGER_H_
#define _DEBUGGER_H_

// enum with CLR debugging commands
// backwards compatible with .NETMF
typedef enum CLR_DBG_Commands_Debugging
{
    CLR_DBG_Commands_c_Debugging_Execution_BasePtr              = 0x00020000, // Returns the pointer for the ExecutionEngine object.
    CLR_DBG_Commands_c_Debugging_Execution_ChangeConditions     = 0x00020001, // Sets/resets the state of the debugger.
    CLR_DBG_Commands_c_Debugging_Execution_SecurityKey          = 0x00020002, // Sets security key.
    CLR_DBG_Commands_c_Debugging_Execution_Unlock               = 0x00020003, // Unlock the low-level command, for mfg. test programs.
    CLR_DBG_Commands_c_Debugging_Execution_Allocate             = 0x00020004, // Permanently allocate some memory.
    CLR_DBG_Commands_c_Debugging_Execution_Breakpoints          = 0x00020005, // Sets breakpoints.
    CLR_DBG_Commands_c_Debugging_Execution_BreakpointHit        = 0x00020006, // Notification that a breakpoint was hit.
    CLR_DBG_Commands_c_Debugging_Execution_BreakpointStatus     = 0x00020007, // Queries last breakpoint hit.
    CLR_DBG_Commands_c_Debugging_Execution_QueryCLRCapabilities = 0x00020008, // Queries capabilities of the CLR.
    CLR_DBG_Commands_c_Debugging_Execution_SetCurrentAppDomain  = 0x00020009, // Sets the current AppDomain.  This is required before
                                                                                 // performing certain debugging operations, such as
                                                                                 // accessing a static field, or doing function evaluation,

    CLR_DBG_Commands_c_Debugging_Thread_Create                  = 0x00020010, // OBSOLETE - Use c_Debugging_Thread_CreateEx instead.
    CLR_DBG_Commands_c_Debugging_Thread_List                    = 0x00020011, // Lists the active/waiting threads.
    CLR_DBG_Commands_c_Debugging_Thread_Stack                   = 0x00020012, // Lists the call stack for a thread.
    CLR_DBG_Commands_c_Debugging_Thread_Kill                    = 0x00020013, // Kills a thread.
    CLR_DBG_Commands_c_Debugging_Thread_Suspend                 = 0x00020014, // Suspends the execution of a thread.
    CLR_DBG_Commands_c_Debugging_Thread_Resume                  = 0x00020015, // Resumes the execution of a thread.
    CLR_DBG_Commands_c_Debugging_Thread_GetException            = 0x00020016, // Gets the current exception.
    CLR_DBG_Commands_c_Debugging_Thread_Unwind                  = 0x00020017, // Unwinds to given stack frame.
    CLR_DBG_Commands_c_Debugging_Thread_CreateEx                = 0x00020018, // Creates a new thread but Thread.CurrentThread will return the identity of the passed thread.
    CLR_DBG_Commands_c_Debugging_Thread_Get                     = 0x00021000, // Gets the current thread.
                                                                 
    CLR_DBG_Commands_c_Debugging_Stack_Info                     = 0x00020020, // Gets more info on a stack frame.
    CLR_DBG_Commands_c_Debugging_Stack_SetIP                    = 0x00020021, // Sets the IP on a given thread.
                                                                 
    CLR_DBG_Commands_c_Debugging_Value_ResizeScratchPad         = 0x00020030, // Resizes the scratchpad area.
    CLR_DBG_Commands_c_Debugging_Value_GetStack                 = 0x00020031, // Reads a value from the stack frame.
    CLR_DBG_Commands_c_Debugging_Value_GetField                 = 0x00020032, // Reads a value from an object's field.
    CLR_DBG_Commands_c_Debugging_Value_GetArray                 = 0x00020033, // Reads a value from an array's element.
    CLR_DBG_Commands_c_Debugging_Value_GetBlock                 = 0x00020034, // Reads a value from a heap block.
    CLR_DBG_Commands_c_Debugging_Value_GetScratchPad            = 0x00020035, // Reads a value from the scratchpad area.
    CLR_DBG_Commands_c_Debugging_Value_SetBlock                 = 0x00020036, // Writes a value to a heap block.
    CLR_DBG_Commands_c_Debugging_Value_SetArray                 = 0x00020037, // Writes a value to an array's element.
    CLR_DBG_Commands_c_Debugging_Value_AllocateObject           = 0x00020038, // Creates a new instance of an object.
    CLR_DBG_Commands_c_Debugging_Value_AllocateString           = 0x00020039, // Creates a new instance of a string.
    CLR_DBG_Commands_c_Debugging_Value_AllocateArray            = 0x0002003A, // Creates a new instance of an array.
    CLR_DBG_Commands_c_Debugging_Value_Assign                   = 0x0002003B, // Assigns a value to another value.
                                                                 
    CLR_DBG_Commands_c_Debugging_TypeSys_Assemblies                 = 0x00020040, // Lists all the assemblies in the system.
    CLR_DBG_Commands_c_Debugging_TypeSys_AppDomains                 = 0x00020044, // Lists all the AppDomans loaded.
                                                                
    CLR_DBG_Commands_c_Debugging_Resolve_Assembly               = 0x00020050, // Resolves an assembly.
    CLR_DBG_Commands_c_Debugging_Resolve_Type                   = 0x00020051, // Resolves a type to a string.
    CLR_DBG_Commands_c_Debugging_Resolve_Field                  = 0x00020052, // Resolves a field to a string.
    CLR_DBG_Commands_c_Debugging_Resolve_Method                 = 0x00020053, // Resolves a method to a string.
    CLR_DBG_Commands_c_Debugging_Resolve_VirtualMethod          = 0x00020054, // Resolves a virtual method to the actual implementation.
    CLR_DBG_Commands_c_Debugging_Resolve_AppDomain              = 0x00020055, // Resolves an AppDomain to it's name, and list its loaded assemblies.


    CLR_DBG_Commands_c_Debugging_MFUpdate_Start                 = 0x00020056, // 
    CLR_DBG_Commands_c_Debugging_MFUpdate_AddPacket             = 0x00020057, // 
    CLR_DBG_Commands_c_Debugging_MFUpdate_Install               = 0x00020058, // 
    CLR_DBG_Commands_c_Debugging_MFUpdate_AuthCommand           = 0x00020059, // 
    CLR_DBG_Commands_c_Debugging_MFUpdate_Authenticate          = 0x00020060, // 
    CLR_DBG_Commands_c_Debugging_MFUpdate_GetMissingPkts        = 0x00020061, // 

    CLR_DBG_Commands_c_Debugging_UpgradeToSsl                   = 0x00020069, //

    //--//                                                       
                                                                 
    CLR_DBG_Commands_c_Debugging_Button_Report                  = 0x00020080, // Reports a button press/release.
    CLR_DBG_Commands_c_Debugging_Button_Inject                  = 0x00020081, // Injects a button press/release.
                                                                                                                   
    CLR_DBG_Commands_c_Debugging_Deployment_Status              = 0x000200B0, // Returns entryPoint and boundary of deployment area.
                                                                 
    CLR_DBG_Commands_c_Debugging_Info_SetJMC                    = 0x000200C0, // Sets code to be flagged as JMC (Just my code).

    CLR_DBG_Commands_c_Profiling_Command                        = 0x00030000, // Various incoming commands regarding profiling
    CLR_DBG_Commands_c_Profiling_Stream                         = 0x00030001, // Stream for MFProfiler information.

}CLR_DBG_Commands_Debugging;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH nanoFramework.Tools.Debugger.WireProtocol.AccessMemoryErrorCodes (in managed code) !!! //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// enum with AccessMemoryErrorCodes
typedef enum AccessMemoryErrorCodes
{
    AccessMemoryErrorCode_NoError                           = 0x0001,       // no error
    AccessMemoryErrorCode_PermissionDenied                  = 0x0010,       // permission denied to execute operation
    AccessMemoryErrorCode_FailedToAllocateReadBuffer        = 0x0020,       // failed to allocate buffer for read operation. better check heap
    AccessMemoryErrorCode_RequestedOperationFailed          = 0x0030,       // the requested operation failed

}AccessMemoryErrorCodes;

#endif //_DEBUGGER_H_
