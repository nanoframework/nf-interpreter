//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#ifndef _NANOCLR_DEBUGGING_H_
#define _NANOCLR_DEBUGGING_H_

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <nanoPackStruct.h>
#include <nanoCLR_Types.h>
#include <nanoCLR_Messaging.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

//#if defined(_MSC_VER)
struct CLR_DBG_Commands
{
    static const unsigned int c_Monitor_Ping               = 0x00000000; // The payload is empty, this command is used to let the other side know we are here...
    static const unsigned int c_Monitor_Message            = 0x00000001; // The payload is composed of the string characters, no zero at the end.
    static const unsigned int c_Monitor_ReadMemory         = 0x00000002;
    static const unsigned int c_Monitor_WriteMemory        = 0x00000003;
    static const unsigned int c_Monitor_CheckMemory        = 0x00000004;
    static const unsigned int c_Monitor_EraseMemory        = 0x00000005;
    static const unsigned int c_Monitor_Execute            = 0x00000006;
    static const unsigned int c_Monitor_Reboot             = 0x00000007;
    static const unsigned int c_Monitor_MemoryMap          = 0x00000008;
    static const unsigned int c_Monitor_ProgramExit        = 0x00000009; // The payload is empty, this command is used to tell the PC of a program termination
    static const unsigned int c_Monitor_CheckSignature     = 0x0000000A;
    static const unsigned int c_Monitor_DeploymentMap      = 0x0000000B;
    static const unsigned int c_Monitor_FlashSectorMap     = 0x0000000C;
    static const unsigned int c_Monitor_OemInfo            = 0x0000000E;
    static const unsigned int c_Monitor_QueryConfiguration = 0x0000000F;
    static const unsigned int c_Monitor_UpdateConfiguration= 0x00000010;

    //--//




















    
    struct Monitor_OemInfo
    {
        struct Reply
        {
            NFReleaseInfo   m_releaseInfo;
        };
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // !!! KEEP IN SYNC WITH nanoFramework.Tools.Debugger.WireProtocol.RebootOptions (in managed code) !!! //
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct Monitor_Reboot
    {
        static const unsigned int c_NormalReboot    = 0;
        static const unsigned int c_EnterBootloader = 1;
        static const unsigned int c_ClrOnly   = 2;
        static const unsigned int c_WaitForDebugger   = 4;

        unsigned int m_flags;
    };

    struct Monitor_ReadMemory
    {
        unsigned int m_address;
        unsigned int m_length;

        struct Reply
        {
            unsigned char m_data[ 1 ];
        };
    };

    struct Monitor_WriteMemory
    {
        unsigned int m_address;
        unsigned int m_length;
        unsigned char  m_data[ 1 ];

        struct Reply
        {
            unsigned int ErrorCode;
        };        
    };

    struct Monitor_Signature
    {        
        unsigned int m_keyIndex;
        unsigned int m_length;   
        unsigned char  m_signature[ 1 ];
    };

    struct Monitor_CheckMemory
    {
        unsigned int m_address;
        unsigned int m_length;

        struct Reply
        {
            unsigned int m_crc;
        };
    };

    struct Monitor_EraseMemory
    {
        unsigned int m_address;
        unsigned int m_length;

        struct Reply
        {
            unsigned int ErrorCode;
        };        
    };

    struct Monitor_Execute
    {
        unsigned int m_address;
    };

















    struct Monitor_DeploymentMap
    {    
        static const CLR_UINT32 c_CRC_Erased_Sentinel = 0x0;

        struct FlashSector
        {
            CLR_UINT32 m_start;
            CLR_UINT32 m_length;
            
            CLR_UINT32 m_crc;
        };

        struct Reply
        {

            FlashSector m_data[ 1 ];
        };
    };


    

    struct Monitor_SignatureKeyUpdate
    {
        unsigned int m_keyIndex;
        unsigned char  m_newKeySignature[ 128 ];
        unsigned char  m_newKey[ 260 ];
        unsigned int m_reserveLength;
        unsigned char  m_reserveData[ 1 ];
    };

    //--------------------------------------------------------------//

    static const unsigned int c_Debugging_Execution_BasePtr              = 0x00020000; // Returns the pointer for the ExecutionEngine object.
    static const unsigned int c_Debugging_Execution_ChangeConditions     = 0x00020001; // Sets/resets the state of the debugger.
    static const unsigned int c_Debugging_Execution_SecurityKey          = 0x00020002; // Sets security key.
    static const unsigned int c_Debugging_Execution_Unlock               = 0x00020003; // Unlock the low-level command, for mfg. test programs.
    static const unsigned int c_Debugging_Execution_Allocate             = 0x00020004; // Permanently allocate some memory.
    static const unsigned int c_Debugging_Execution_Breakpoints          = 0x00020005; // Sets breakpoints.
    static const unsigned int c_Debugging_Execution_BreakpointHit        = 0x00020006; // Notification that a breakpoint was hit.
    static const unsigned int c_Debugging_Execution_BreakpointStatus     = 0x00020007; // Queries last breakpoint hit.
    static const unsigned int c_Debugging_Execution_QueryCLRCapabilities = 0x00020008; // Queries capabilities of the CLR.
    static const unsigned int c_Debugging_Execution_SetCurrentAppDomain  = 0x00020009; // Sets the current AppDomain.  This is required before
                                                                                 // performing certain debugging operations, such as
                                                                                 // accessing a static field, or doing function evaluation,

    static const unsigned int c_Debugging_Thread_Create                  = 0x00020010; // OBSOLETE - Use c_Debugging_Thread_CreateEx instead.
    static const unsigned int c_Debugging_Thread_List                    = 0x00020011; // Lists the active/waiting threads.
    static const unsigned int c_Debugging_Thread_Stack                   = 0x00020012; // Lists the call stack for a thread.
    static const unsigned int c_Debugging_Thread_Kill                    = 0x00020013; // Kills a thread.
    static const unsigned int c_Debugging_Thread_Suspend                 = 0x00020014; // Suspends the execution of a thread.
    static const unsigned int c_Debugging_Thread_Resume                  = 0x00020015; // Resumes the execution of a thread.
    static const unsigned int c_Debugging_Thread_GetException            = 0x00020016; // Gets the current exception.
    static const unsigned int c_Debugging_Thread_Unwind                  = 0x00020017; // Unwinds to given stack frame.
    static const unsigned int c_Debugging_Thread_CreateEx                = 0x00020018; // Creates a new thread but Thread.CurrentThread will return the identity of the passed thread.
    static const unsigned int c_Debugging_Thread_Get                     = 0x00021000; // Gets the current thread.
                                                                 
    static const unsigned int c_Debugging_Stack_Info                     = 0x00020020; // Gets more info on a stack frame.
    static const unsigned int c_Debugging_Stack_SetIP                    = 0x00020021; // Sets the IP on a given thread.
                                                                 
    static const unsigned int c_Debugging_Value_ResizeScratchPad         = 0x00020030; // Resizes the scratchpad area.
    static const unsigned int c_Debugging_Value_GetStack                 = 0x00020031; // Reads a value from the stack frame.
    static const unsigned int c_Debugging_Value_GetField                 = 0x00020032; // Reads a value from an object's field.
    static const unsigned int c_Debugging_Value_GetArray                 = 0x00020033; // Reads a value from an array's element.
    static const unsigned int c_Debugging_Value_GetBlock                 = 0x00020034; // Reads a value from a heap block.
    static const unsigned int c_Debugging_Value_GetScratchPad            = 0x00020035; // Reads a value from the scratchpad area.
    static const unsigned int c_Debugging_Value_SetBlock                 = 0x00020036; // Writes a value to a heap block.
    static const unsigned int c_Debugging_Value_SetArray                 = 0x00020037; // Writes a value to an array's element.
    static const unsigned int c_Debugging_Value_AllocateObject           = 0x00020038; // Creates a new instance of an object.
    static const unsigned int c_Debugging_Value_AllocateString           = 0x00020039; // Creates a new instance of a string.
    static const unsigned int c_Debugging_Value_AllocateArray            = 0x0002003A; // Creates a new instance of an array.
    static const unsigned int c_Debugging_Value_Assign                   = 0x0002003B; // Assigns a value to another value.
                                                                 
    static const unsigned int c_Debugging_TypeSys_Assemblies                = 0x00020040; // Lists all the assemblies in the system.
    static const unsigned int c_Debugging_TypeSys_AppDomains                = 0x00020044; // Lists all the AppDomans loaded.
                                                                 
    static const unsigned int c_Debugging_Resolve_Assembly               = 0x00020050; // Resolves an assembly.
    static const unsigned int c_Debugging_Resolve_Type                   = 0x00020051; // Resolves a type to a string.
    static const unsigned int c_Debugging_Resolve_Field                  = 0x00020052; // Resolves a field to a string.
    static const unsigned int c_Debugging_Resolve_Method                 = 0x00020053; // Resolves a method to a string.
    static const unsigned int c_Debugging_Resolve_VirtualMethod          = 0x00020054; // Resolves a virtual method to the actual implementation.
    static const unsigned int c_Debugging_Resolve_AppDomain              = 0x00020055; // Resolves an AppDomain to it's name, and list its loaded assemblies.









    static const unsigned int c_Debugging_UpgradeToSsl                   = 0x00020069; //

    //--//                                                       
                                                                 
    static const unsigned int c_Debugging_Button_Report                  = 0x00020080; // Reports a button press/release.
    static const unsigned int c_Debugging_Button_Inject                  = 0x00020081; // Injects a button press/release.
                                                                                                                   
    static const unsigned int c_Debugging_Deployment_Status              = 0x000200B0; // Returns entryPoint and boundary of deployment area.
                                                                 
    static const unsigned int c_Debugging_Info_SetJMC                    = 0x000200C0; // Sets code to be flagged as JMC (Just my code).

    static const unsigned int c_Profiling_Command                        = 0x00030000; // Various incoming commands regarding profiling
    static const unsigned int c_Profiling_Stream                         = 0x00030001; // Stream for MFProfiler information.

    //--//

    struct Debugging_Execution_Unlock
    {
        unsigned char m_command[ 128 ];
        unsigned char m_hash   [ 128 ];
    };

    struct Debugging_Execution_QueryCLRCapabilities
    {
        static const CLR_UINT32 c_CapabilityFlags             = 1;
        static const CLR_UINT32 c_CapabilityVersion           = 3;
        static const CLR_UINT32 c_HalSystemInfo               = 5;
        static const CLR_UINT32 c_ClrInfo                     = 6;
        static const CLR_UINT32 c_TargetReleaseInfo           = 7;
        static const CLR_UINT32 c_InteropNativeAssemblies     = 8;

        static const CLR_UINT32 c_CapabilityFlags_FloatingPoint             = 0x00000001;
        static const CLR_UINT32 c_CapabilityFlags_SourceLevelDebugging      = 0x00000002;
        static const CLR_UINT32 c_CapabilityFlags_AppDomains                = 0x00000004;
        static const CLR_UINT32 c_CapabilityFlags_ExceptionFilters          = 0x00000008;
        static const CLR_UINT32 c_CapabilityFlags_IncrementalDeployment     = 0x00000010;
        static const CLR_UINT32 c_CapabilityFlags_SoftReboot                = 0x00000020;
        static const CLR_UINT32 c_CapabilityFlags_Profiling                 = 0x00000040;
        static const CLR_UINT32 c_CapabilityFlags_Profiling_Allocations     = 0x00000080;
        static const CLR_UINT32 c_CapabilityFlags_Profiling_Calls           = 0x00000100;
        static const CLR_UINT32 c_CapabilityFlags_ThreadCreateEx            = 0x00000400;
        static const CLR_UINT32 c_CapabilityFlags_ConfigBlockRequiresErase  = 0x00000800;
        static const CLR_UINT32 c_CapabilityFlags_HasNanoBooter             = 0x00001000;

        CLR_UINT32 m_cmd;

        struct __nfpack SoftwareVersion
        {
            char BuildDate[ 22 ];
            char CompilerInfo[16];
            unsigned int CompilerVersion;
        };

        struct __nfpack ClrInfo
        {
            NFReleaseInfo m_clrReleaseInfo;
            NFVersion     m_TargetFrameworkVersion;
        };
        
        struct __nfpack NativeAssemblyDetails
        {
            uint32_t CheckSum;
            NFVersion Version;
            uint8_t AssemblyName[128];

        };

        union ReplyUnion
        {
            CLR_UINT32          u_capsFlags;
            SoftwareVersion     u_SoftwareVersion;
            HalSystemInfo       u_HalSystemInfo;
            ClrInfo             u_ClrInfo;
            NFReleaseInfo       u_TargetReleaseInfo;
        };        
    };    

    //--//

    struct Debugging_Messaging_Query
    {
        CLR_RT_HeapBlock_EndPoint::Address m_addr;

        struct Reply
        {
            CLR_UINT32                         m_found;
            CLR_RT_HeapBlock_EndPoint::Address m_addr;
        };
    };

    struct Debugging_Messaging_Send
    {
        CLR_RT_HeapBlock_EndPoint::Address m_addr;
        unsigned char                              m_data[ 1 ];

        struct Reply
        {
            CLR_UINT32                         m_found;
            CLR_RT_HeapBlock_EndPoint::Address m_addr;
        };
    };

    struct Debugging_Messaging_Reply
    {
        CLR_RT_HeapBlock_EndPoint::Address m_addr;
        unsigned char                              m_data[ 1 ];

        struct Reply
        {
            CLR_UINT32                         m_found;
            CLR_RT_HeapBlock_EndPoint::Address m_addr;
        };
    };

    //--//
    

    struct Debugging_Execution_BasePtr
    {
        struct Reply
        {
            CLR_UINT32 m_EE;
        };
    };

    struct Debugging_Execution_ChangeConditions
    {
        CLR_UINT32 FlagsToSet;
        CLR_UINT32 FlagsToReset;

        struct Reply
        {
            CLR_UINT32 CurrentState;
        };
    };

    struct Debugging_Execution_SecurityKey
    {
        CLR_UINT8 m_key[ 32 ];
    };

    struct Debugging_Execution_Allocate
    {
        CLR_UINT32 m_size;

        struct Reply
        {
            CLR_UINT32 m_address;
        };
    };

    struct Debugging_UpgradeToSsl
    {
        CLR_UINT32 m_flags;

        struct Reply
        {
            CLR_UINT32 m_success;
        };
    };


    //struct Debugging_MFUpdate_Start
    //{
    //    char       m_provider[64];
    //    CLR_UINT32 m_updateId;
    //    CLR_UINT32 m_updateType;
    //    CLR_UINT32 m_updateSubType;
    //    CLR_UINT32 m_updateSize;
    //    CLR_UINT32 m_updatePacketSize;
    //    CLR_UINT16 m_versionMajor;
    //    CLR_UINT16 m_versionMinor;

    //    struct Reply
    //    {
    //        CLR_INT32 m_updateHandle;
    //    };
    //};

    //struct Debugging_MFUpdate_AuthCommand
    //{
    //    CLR_UINT32 m_updateHandle;
    //    CLR_UINT32 m_authCommand;
    //    CLR_UINT32 m_authArgsSize;
    //    CLR_UINT8  m_authArgs[1];

    //    struct Reply
    //    {
    //        CLR_INT32  m_success;
    //        CLR_UINT32 m_responseSize;
    //        CLR_UINT8  m_response[1];
    //    };
    //};

    //struct Debugging_MFUpdate_Authenticate
    //{
    //    CLR_UINT32 m_updateHandle;
    //    CLR_UINT32 m_authenticationLen;
    //    CLR_UINT8  m_authenticationData[1];

    //    struct Reply
    //    {
    //        CLR_INT32 m_success;
    //    };
    //};

    //struct Debugging_MFUpdate_GetMissingPkts
    //{
    //    CLR_UINT32 m_updateHandle;
    //    
    //    struct Reply
    //    {
    //        CLR_INT32 m_success;
    //        CLR_INT32  m_missingPktCount;
    //        CLR_UINT32 m_missingPkts[1];
    //    };
    //};

    //struct Debugging_MFUpdate_AddPacket
    //{
    //    CLR_INT32 m_updateHandle;
    //    CLR_UINT32 m_packetIndex;
    //    CLR_UINT32 m_packetValidation;
    //    CLR_UINT32 m_packetLength;
    //    CLR_UINT8 m_packetData[1];

    //    struct Reply
    //    {
    //        CLR_UINT32 m_success;
    //    };
    //};

    //struct Debugging_MFUpdate_Install
    //{
    //    CLR_INT32 m_updateHandle;
    //    CLR_UINT32 m_updateValidationSize;
    //    CLR_UINT8 m_updateValidation[1];

    //    struct Reply
    //    {
    //        CLR_UINT32 m_success;
    //    };
    //};
    

    struct Debugging_Execution_BreakpointDef
    {
        static const CLR_UINT16 c_STEP_IN            = 0x0001;
        static const CLR_UINT16 c_STEP_OVER          = 0x0002;
        static const CLR_UINT16 c_STEP_OUT           = 0x0004;
        static const CLR_UINT16 c_HARD               = 0x0008;
        static const CLR_UINT16 c_EXCEPTION_THROWN   = 0x0010;
        static const CLR_UINT16 c_EXCEPTION_CAUGHT   = 0x0020;
        static const CLR_UINT16 c_EXCEPTION_UNCAUGHT = 0x0040;
        static const CLR_UINT16 c_THREAD_TERMINATED  = 0x0080;
        static const CLR_UINT16 c_THREAD_CREATED     = 0x0100;
        static const CLR_UINT16 c_ASSEMBLIES_LOADED  = 0x0200;
        static const CLR_UINT16 c_LAST_BREAKPOINT    = 0x0400;
        static const CLR_UINT16 c_STEP_JMC           = 0x0800;
        static const CLR_UINT16 c_BREAK              = 0x1000;
        static const CLR_UINT16 c_EVAL_COMPLETE      = 0x2000;
        static const CLR_UINT16 c_EXCEPTION_UNWIND   = 0x4000;
        static const CLR_UINT16 c_EXCEPTION_FINALLY  = 0x8000;

        static const CLR_UINT16 c_STEP               = c_STEP_IN | c_STEP_OVER | c_STEP_OUT;

        static const CLR_INT32 c_PID_ANY             = 0x7FFFFFFF;

        static const CLR_UINT32 c_DEPTH_EXCEPTION_FIRST_CHANCE    = 0x00000000;
        static const CLR_UINT32 c_DEPTH_EXCEPTION_USERS_CHANCE    = 0x00000001;
        static const CLR_UINT32 c_DEPTH_EXCEPTION_HANDLER_FOUND   = 0x00000002;

        static const CLR_UINT32 c_DEPTH_STEP_NORMAL               = 0x00000010;
        static const CLR_UINT32 c_DEPTH_STEP_RETURN               = 0x00000020;
        static const CLR_UINT32 c_DEPTH_STEP_CALL                 = 0x00000030;
        static const CLR_UINT32 c_DEPTH_STEP_EXCEPTION_FILTER     = 0x00000040;
        static const CLR_UINT32 c_DEPTH_STEP_EXCEPTION_HANDLER    = 0x00000050;
        static const CLR_UINT32 c_DEPTH_STEP_INTERCEPT            = 0x00000060;
        static const CLR_UINT32 c_DEPTH_STEP_EXIT                 = 0x00000070;

        static const CLR_UINT32 c_DEPTH_UNCAUGHT                  = 0xFFFFFFFF;


        CLR_UINT16             m_id;
        CLR_UINT16             m_flags;

        CLR_INT32              m_pid;
        CLR_UINT32             m_depth;

        //
        // m_IPStart, m_IPEnd are used for optimizing stepping operations.
        // A STEP_IN | STEP_OVER breakpoint will be hit in the given stack frame
        // only if the IP is outside of the given range [m_IPStart m_IPEnd).
        //
        CLR_UINT32             m_IPStart;
        CLR_UINT32             m_IPEnd;

        CLR_RT_MethodDef_Index m_md;
        CLR_UINT32             m_IP;

        CLR_RT_TypeDef_Index   m_td;

        CLR_UINT32             m_depthExceptionHandler;
    };

    struct Debugging_Execution_Breakpoints
    {
        CLR_UINT32                        m_flags;

        Debugging_Execution_BreakpointDef m_data[ 1 ];
    };

    struct Debugging_Execution_BreakpointHit
    {
        Debugging_Execution_BreakpointDef m_hit;
    };

    struct Debugging_Execution_BreakpointStatus
    {
        struct Reply
        {
            Debugging_Execution_BreakpointDef m_lastHit;
        };
    };

    struct Debugging_Execution_SetCurrentAppDomain
    {
        CLR_UINT32 m_id;
    };

    //--//--//

    struct Debugging_Thread_CreateEx
    {
        CLR_RT_MethodDef_Index m_md;
        int                    m_scratchPad;
        CLR_UINT32             m_pid;

        struct Reply
        {
            CLR_UINT32 m_pid;
        };
    };

    //--//

    struct Debugging_Thread_List
    {
        struct Reply
        {
            CLR_UINT32 m_num;
            CLR_UINT32 m_pids[ 1 ];
        };
    };

    //--//

    struct Debugging_Thread_Stack
    {
        CLR_UINT32 m_pid;

        struct Reply
        {
            struct Call
            {
                CLR_RT_MethodDef_Index m_md;
                CLR_UINT32             m_IP;
#if defined(NANOCLR_APPDOMAINS)
                CLR_UINT32             m_appDomainID;
                CLR_UINT32             m_flags;
#endif

            };

            CLR_UINT32 m_num;
            CLR_UINT32 m_status;
            CLR_UINT32 m_flags;
            Call       m_data[ 1 ];
        };
    };

    struct Debugging_Thread_Kill
    {
        CLR_UINT32 m_pid;

        struct Reply
        {
            int m_result;
        };
    };

    struct Debugging_Thread_Suspend
    {
        CLR_UINT32 m_pid;
    };

    struct Debugging_Thread_Resume
    {
        CLR_UINT32 m_pid;
    };

    struct Debugging_Thread_Get
    {
        CLR_UINT32 m_pid;

        //
        // The reply is an array of Debugging_Value
        //
    };

    struct Debugging_Thread_GetException
    {
        CLR_UINT32 m_pid;

        //
        // The reply is an array of Debugging_Value
        //
    };

    struct Debugging_Thread_Unwind
    {
        CLR_UINT32 m_pid;
        CLR_UINT32 m_depth;
    };

    //--//

    struct Debugging_Stack_Info
    {
        CLR_UINT32 m_pid;
        CLR_UINT32 m_depth;

        struct Reply
        {
            CLR_RT_MethodDef_Index m_md;
            CLR_UINT32             m_IP;
            CLR_UINT32             m_numOfArguments;
            CLR_UINT32             m_numOfLocals;
            CLR_UINT32             m_depthOfEvalStack;
        };
    };

    struct Debugging_Stack_SetIP
    {
        CLR_UINT32 m_pid;
        CLR_UINT32 m_depth;

        CLR_UINT32 m_IP;
        CLR_UINT32 m_depthOfEvalStack;
    };

    //--//

    struct Debugging_Value
    {
        CLR_RT_HeapBlock*       m_referenceID;
        CLR_UINT32              m_dt;                // CLR_RT_HeapBlock::DataType ()
        CLR_UINT32              m_flags;             // CLR_RT_HeapBlock::DataFlags()
        CLR_UINT32              m_size;              // CLR_RT_HeapBlock::DataSize ()

        //
        // For primitive types
        //
        CLR_UINT8               m_builtinValue[ 128 ]; // Space for string preview...

        //
        // For DATATYPE_STRING
        //
        CLR_UINT32              m_bytesInString;
        const char*                  m_charsInString;

        //
        // For DATATYPE_VALUETYPE or DATATYPE_CLASSTYPE
        //
        CLR_RT_TypeDef_Index    m_td;

        //
        // For DATATYPE_SZARRAY
        //
        CLR_UINT32              m_array_numOfElements;
        CLR_UINT32              m_array_depth;
        CLR_RT_TypeDef_Index    m_array_typeIndex;

        //
        // For values from an array.
        //
        CLR_RT_HeapBlock_Array* m_arrayref_referenceID;
        CLR_UINT32              m_arrayref_index;
    };

    struct Debugging_Value_ResizeScratchPad
    {
        CLR_UINT32 m_size;
    };

    struct Debugging_Value_GetStack
    {
        static const CLR_UINT32 c_Local     = 0;
        static const CLR_UINT32 c_Argument  = 1;
        static const CLR_UINT32 c_EvalStack = 2;

        CLR_UINT32 m_pid;
        CLR_UINT32 m_depth;
        CLR_UINT32 m_kind;
        CLR_UINT32 m_index;


        //
        // The reply is an array of Debugging_Value
        //
    };

    struct Debugging_Value_GetField
    {
        CLR_RT_HeapBlock*     m_heapblock;
        CLR_UINT32            m_offset;
        CLR_RT_FieldDef_Index m_fd;


        //
        // The reply is an array of Debugging_Value
        //
    };

    struct Debugging_Value_GetArray
    {
        CLR_RT_HeapBlock* m_heapblock;
        CLR_UINT32        m_index;


        //
        // The reply is an array of Debugging_Value
        //
    };

    struct Debugging_Value_GetBlock
    {
        CLR_RT_HeapBlock* m_heapblock;


        //
        // The reply is an array of Debugging_Value
        //
    };

    struct Debugging_Value_GetScratchPad
    {
        CLR_UINT32 m_idx;


        //
        // The reply is an array of Debugging_Value
        //
    };

    struct Debugging_Value_SetBlock
    {
        CLR_RT_HeapBlock* m_heapblock;
        CLR_UINT32        m_dt;                // CLR_RT_HeapBlock::DataType ()
        CLR_UINT8         m_builtinValue[ 8 ];
    };

    struct Debugging_Value_SetArray
    {
        CLR_RT_HeapBlock_Array* m_heapblock;
        CLR_UINT32              m_index;
        CLR_UINT8               m_builtinValue[ 8 ];
    };

    //--//

    struct Debugging_Value_AllocateObject
    {
        int                  m_index;
        CLR_RT_TypeDef_Index m_td;

        //
        // The reply is an array of Debugging_Value
        //
    };

    struct Debugging_Value_AllocateString
    {
        int        m_index;
        CLR_UINT32 m_size;

        //
        // The reply is an array of Debugging_Value
        //
    };

    struct Debugging_Value_AllocateArray
    {
        int                  m_index;
        CLR_RT_TypeDef_Index m_td;
        CLR_UINT32           m_depth;
        CLR_UINT32           m_numOfElements;

        //
        // The reply is an array of Debugging_Value
        //
    };

    struct Debugging_Value_Assign
    {
        CLR_RT_HeapBlock* m_heapblockSrc;
        CLR_RT_HeapBlock* m_heapblockDst;

        //
        // The reply is an array of Debugging_Value
        //
    };

    //--//

    struct Debugging_TypeSys_Assemblies
    {        
        //
        // The reply is just an array of CLR_RT_Assembly_Index.
        //
    };

    struct Debugging_TypeSys_AppDomains
    {
        //
        // The reply is just an array of AppDomainIDs
        //
    };    

    //--//

    struct Debugging_Resolve_AppDomain
    {        
        CLR_UINT32 m_id;        

        struct Reply
        {
            CLR_UINT32 m_state;
            char       m_szName[ 512 ];
            CLR_UINT32 m_assemblies[ 1 ]; //Array of CLR_RT_Assembly_Index
        };
    };

    struct Debugging_Resolve_Assembly
    {
         CLR_RT_Assembly_Index m_idx;

         struct Reply
         {
              CLR_UINT32         m_flags;
              CLR_RECORD_VERSION m_version;
              char               m_szName[ 512 ];
         };
    };

    struct Debugging_Resolve_Type
    {
        CLR_RT_TypeDef_Index m_td;

        struct Reply
        {
            char m_type[ 512 ];
        };
    };

    struct Debugging_Resolve_Field
    {
        CLR_RT_FieldDef_Index m_fd;

        struct Reply
        {
            CLR_RT_TypeDef_Index m_td;
            CLR_UINT32           m_index;
            char                 m_name[ 512 ];
        };
    };

    struct Debugging_Resolve_Method
    {
        CLR_RT_MethodDef_Index m_md;

        struct Reply
        {
            CLR_RT_TypeDef_Index m_td;
            char                 m_method[ 512 ];
        };
    };

    struct Debugging_Resolve_VirtualMethod
    {
        CLR_RT_MethodDef_Index m_md;
        CLR_RT_HeapBlock*      m_obj;

        struct Reply
        {
            CLR_RT_MethodDef_Index m_md;
        };
    };

    //--//

    struct Debugging_Deployment_Status
    {    
        struct FlashSector
        {
            CLR_UINT32 Start;
            CLR_UINT32 Length;
        };

        struct Reply
        {
            CLR_UINT32 EntryPoint;
            CLR_UINT32 StorageStart;
            CLR_UINT32 StorageLength;
            
            // FlashSector SectorData[ 1 ];
        };
    };

    //--//

    struct Debugging_Info_SetJMC
    {
        CLR_UINT32 m_fIsJMC;
        CLR_UINT32 m_kind;    // CLR_ReflectionType

        union
        {
            CLR_RT_Assembly_Index  m_assm;
            CLR_RT_TypeDef_Index   m_type;
            CLR_RT_MethodDef_Index m_method;
            CLR_UINT32             m_raw;
        } m_data;
    };

    struct Profiling_Command
    {
        static const CLR_UINT8 c_Command_ChangeConditions = 0x01;
        static const CLR_UINT8 c_Command_FlushStream      = 0x02;

        CLR_UINT8 m_command;

        struct Reply
        {
            CLR_UINT32 m_raw;
        };
    };

    struct Profiling_ChangeConditions
    {
        CLR_UINT32 m_set;
        CLR_UINT32 m_reset;
    };

    struct Profiling_Stream
    {
        CLR_UINT16 m_seqId;
        CLR_UINT16 m_bitLen;
    };
    
};
//#endif

struct CLR_DBG_Debugger
{
    CLR_Messaging*              m_messaging;
    static BlockStorageDevice*  m_deploymentStorageDevice;


    //--//

    static void Debugger_WaitForCommands();

    static HRESULT CreateInstance();

    HRESULT Debugger_Initialize( COM_HANDLE port );

    static HRESULT DeleteInstance();

    void Debugger_Cleanup();

    static void BroadcastEvent( unsigned int cmd, unsigned int payloadSize, unsigned char* payload, unsigned int flags );

    void PurgeCache     ();

private:

    bool CheckPermission( ByteAddress address, int mode );

    bool AccessMemory( CLR_UINT32 location, unsigned int lengthInBytes, unsigned char* buf, int mode, unsigned int* errorCode );

#if defined(NANOCLR_APPDOMAINS)
    CLR_RT_AppDomain*  GetAppDomainFromID ( CLR_UINT32 id );
#endif

    CLR_RT_StackFrame* CheckStackFrame    ( CLR_INT32 pid, CLR_UINT32 depth, bool& isInline                                        );
    HRESULT            CreateListOfThreads(                 CLR_DBG_Commands::Debugging_Thread_List ::Reply*& cmdReply, int& totLen );
    HRESULT            CreateListOfCalls  ( CLR_INT32 pid, CLR_DBG_Commands::Debugging_Thread_Stack::Reply*& cmdReply, int& totLen );

    CLR_RT_Assembly*   IsGoodAssembly( CLR_IDX                       idxAssm                                  );
    bool               CheckTypeDef  ( const CLR_RT_TypeDef_Index&   td     , CLR_RT_TypeDef_Instance&   inst );
    bool               CheckFieldDef ( const CLR_RT_FieldDef_Index&  fd     , CLR_RT_FieldDef_Instance&  inst );
    bool               CheckMethodDef( const CLR_RT_MethodDef_Index& md     , CLR_RT_MethodDef_Instance& inst );

    bool               GetValue( WP_Message* msg, CLR_RT_HeapBlock* ptr, CLR_RT_HeapBlock* reference, CLR_RT_TypeDef_Instance* pTD );

    bool AllocateAndQueueMessage( CLR_UINT32 cmd, unsigned int length, unsigned char* data, CLR_RT_HeapBlock_EndPoint::Port port, CLR_RT_HeapBlock_EndPoint::Address addr, CLR_UINT32 found );

    bool ProcessHeader                           ( WP_Message* msg );
    bool ProcessPayload                          ( WP_Message* msg );

       
public:  
    static CLR_RT_Thread* GetThreadFromPid ( CLR_INT32 pid );

    static bool Monitor_Ping                            ( WP_Message* msg );
    static bool Monitor_Reboot                          ( WP_Message* msg );
    static bool Debugging_Execution_QueryCLRCapabilities( WP_Message* msg );

    static bool Monitor_ReadMemory                      ( WP_Message* msg );
    static bool Monitor_WriteMemory                     ( WP_Message* msg );
    static bool Monitor_CheckMemory                     ( WP_Message* msg );
    static bool Monitor_EraseMemory                     ( WP_Message* msg );
    static bool Monitor_Execute                         ( WP_Message* msg );
    static bool Monitor_MemoryMap                       ( WP_Message* msg );
    static bool Monitor_FlashSectorMap                  ( WP_Message* msg );
    static bool Monitor_DeploymentMap                   ( WP_Message* msg );
    static bool Monitor_QueryConfiguration              ( WP_Message* msg );
    static bool Monitor_UpdateConfiguration             ( WP_Message* msg );

                                             
    static bool Debugging_Execution_BasePtr             ( WP_Message* msg );
    static bool Debugging_Execution_ChangeConditions    ( WP_Message* msg );

    static bool Debugging_Execution_Allocate            ( WP_Message* msg );

    static bool Debugging_UpgradeToSsl                  ( WP_Message* msg );








#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
    static bool Debugging_Execution_Breakpoints         ( WP_Message* msg );
    static bool Debugging_Execution_BreakpointStatus    ( WP_Message* msg );
    static bool Debugging_Execution_SetCurrentAppDomain ( WP_Message* msg );

    static bool Debugging_Thread_CreateEx               ( WP_Message* msg );
    static bool Debugging_Thread_List                   ( WP_Message* msg );
    static bool Debugging_Thread_Stack                  ( WP_Message* msg );
    static bool Debugging_Thread_Kill                   ( WP_Message* msg );
    static bool Debugging_Thread_Suspend                ( WP_Message* msg );
    static bool Debugging_Thread_Resume                 ( WP_Message* msg );
    static bool Debugging_Thread_GetException           ( WP_Message* msg );
    static bool Debugging_Thread_Unwind                 ( WP_Message* msg );
    static bool Debugging_Thread_Get                    ( WP_Message* msg );
                                             
    static bool Debugging_Stack_Info                    ( WP_Message* msg );
    static bool Debugging_Stack_SetIP                   ( WP_Message* msg );
                                             
    static bool Debugging_Value_ResizeScratchPad        ( WP_Message* msg );
    static bool Debugging_Value_GetStack                ( WP_Message* msg );
    static bool Debugging_Value_GetField                ( WP_Message* msg );
    static bool Debugging_Value_GetArray                ( WP_Message* msg );
    static bool Debugging_Value_GetBlock                ( WP_Message* msg );
    static bool Debugging_Value_GetScratchPad           ( WP_Message* msg );
    static bool Debugging_Value_SetBlock                ( WP_Message* msg );
    static bool Debugging_Value_SetArray                ( WP_Message* msg );
    static bool Debugging_Value_AllocateObject          ( WP_Message* msg );
    static bool Debugging_Value_AllocateString          ( WP_Message* msg );
    static bool Debugging_Value_AllocateArray           ( WP_Message* msg );
    static bool Debugging_Value_Assign                  ( WP_Message* msg );
                                             
    static bool Debugging_TypeSys_Assemblies            ( WP_Message* msg );
    static bool Debugging_TypeSys_AppDomains            ( WP_Message* msg );
                                             
    static bool Debugging_Resolve_AppDomain             ( WP_Message* msg );
    static bool Debugging_Resolve_Assembly              ( WP_Message* msg );
    static bool Debugging_Resolve_Type                  ( WP_Message* msg );
    static bool Debugging_Resolve_Field                 ( WP_Message* msg );
    static bool Debugging_Resolve_Method                ( WP_Message* msg );
    static bool Debugging_Resolve_VirtualMethod         ( WP_Message* msg );
#endif //#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

    static bool Debugging_Deployment_Status             ( WP_Message* msg );

#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
    static bool Debugging_Info_SetJMC                   ( WP_Message* msg );
    
    bool Debugging_Info_SetJMC_Type                     ( const CLR_RT_TypeDef_Index&   idx, bool fJMC );
    bool Debugging_Info_SetJMC_Method                   ( const CLR_RT_MethodDef_Index& idx, bool fJMC );
#endif //#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

    static bool Profiling_Command                       ( WP_Message* msg );
    bool Profiling_ChangeConditions                     ( WP_Message* msg );
    bool Profiling_FlushStream                          ( WP_Message* msg );
};

//--//

extern CLR_UINT32        g_scratchDebugger[];
extern CLR_UINT32        g_scratchDebuggerMessaging[];
extern CLR_DBG_Debugger *g_CLR_DBG_Debugger;

//--//

extern const CLR_Messaging_CommandHandlerLookup c_Debugger_Lookup_Request[];
extern const CLR_Messaging_CommandHandlerLookup c_Debugger_Lookup_Reply[];
extern const CLR_UINT32 c_Debugger_Lookup_Request_count;
extern const CLR_UINT32 c_Debugger_Lookup_Reply_count;

//--//

#endif // _NANOCLR_DEBUGGING_H_

