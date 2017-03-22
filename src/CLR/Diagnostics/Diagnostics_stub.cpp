//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "Diagnostics.h"


////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(_WIN32)

__nfweak void CLR_Debug::RedirectToString( std::string* str )
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

#endif

__nfweak HRESULT NANOCLR_DEBUG_PROCESS_EXCEPTION( HRESULT hr, const char* szFunc, const char* szFile, int line )
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
    return hr;
}

__nfweak bool CLR_SafeSprintfV( char*& szBuffer, size_t& iBuffer, const char* format, va_list arg )
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
    // UNDONE: FIXME 
    // int  chars = hal_vsnprintf( szBuffer, iBuffer, format, arg );
    // bool fRes  = (chars >= 0);

    // if(fRes == false) chars = (int)iBuffer;

    // szBuffer += chars; szBuffer[ 0 ] = 0;
    // iBuffer  -= chars;

    // return fRes;
    return true;
}

__nfweak bool CLR_SafeSprintf( char*& szBuffer, size_t& iBuffer, const char* format, ... )
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
    va_list arg;
    bool    fRes;

    va_start( arg, format );

    fRes = CLR_SafeSprintfV( szBuffer, iBuffer, format, arg );

    va_end( arg );

    return fRes;
}

//--//

__nfweak void CLR_Debug::Flush()
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

__nfweak void CLR_Debug::Emit( const char *text, int len )
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

__nfweak int CLR_Debug::PrintfV( const char *format, va_list arg )
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
    return 0;
}

__nfweak int CLR_Debug::Printf( const char *format, ... )
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
    return 0;
}

//--//

__nfweak CLR_UINT32 CLR_ReadTokenCompressed( const CLR_UINT8*& ip, CLR_OPCODE opcode )
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
    return 0;
}

__nfweak const CLR_UINT8* CLR_SkipBodyOfOpcode( const CLR_UINT8* ip, CLR_OPCODE opcode )
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
    return 0;
}

__nfweak const CLR_UINT8* CLR_SkipBodyOfOpcodeCompressed( const CLR_UINT8* ip, CLR_OPCODE opcode )
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
    return 0;
}

#if defined(NANOCLR_TRACE_INSTRUCTIONS)

__nfweak void CLR_RT_Assembly::DumpToken( CLR_UINT32 tk )
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

__nfweak void CLR_RT_Assembly::DumpSignature( CLR_SIG sig )
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

__nfweak void CLR_RT_Assembly::DumpSignature( const CLR_UINT8*& p )
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

__nfweak void CLR_RT_Assembly::DumpSignatureToken( const CLR_UINT8*& p )
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

//--//

__nfweak void CLR_RT_Assembly::DumpOpcode( CLR_RT_StackFrame* stack, CLR_PMETADATA ip )
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

__nfweak void CLR_RT_Assembly::DumpOpcodeDirect( CLR_RT_MethodDef_Instance& call, CLR_PMETADATA ip, CLR_PMETADATA ipStart, int pid )
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

#endif

#if defined(NANOCLR_TRACE_CALLS)

__nfweak void CLR_RT_Assembly::DumpCall( CLR_RT_StackFrame& stack, bool fPush )
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

__nfweak void CLR_RT_Assembly::DumpCall( CLR_RT_MethodDef_Instance& md, const char* szPrefix )
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

#endif 

///////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(NANOCLR_TRACE_ERRORS)

__nfweak void CLR_RT_DUMP::TYPE( const CLR_RT_TypeDef_Index& cls )
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

__nfweak void CLR_RT_DUMP::TYPE( const CLR_RT_ReflectionDef_Index& reflex )
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

__nfweak void CLR_RT_DUMP::METHOD( const CLR_RT_MethodDef_Index& method )
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

__nfweak void CLR_RT_DUMP::FIELD( const CLR_RT_FieldDef_Index& field )
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

__nfweak void CLR_RT_DUMP::OBJECT( CLR_RT_HeapBlock* ptr, const char* text )
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

#endif

//--//

#if defined(NANOCLR_TRACE_EXCEPTIONS)

__nfweak void CLR_RT_DUMP::EXCEPTION( CLR_RT_StackFrame& stack, CLR_RT_HeapBlock& ref )
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

__nfweak void CLR_RT_DUMP::POST_PROCESS_EXCEPTION( CLR_RT_HeapBlock& ref )
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

#endif 

//--//

__nfweak const char* CLR_RT_DUMP::GETERRORMESSAGE( HRESULT hrError )
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
    return NULL;
}

//--//

#if defined(NANOCLR_PROFILE_NEW_CALLS)

__nfweak void* CLR_PROF_CounterCallChain::Prepare( CLR_PROF_Handler* handler )
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
    return NULL;
}

__nfweak void CLR_PROF_CounterCallChain::Complete( CLR_UINT64& t, CLR_PROF_Handler* handler )
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

__nfweak void CLR_PROF_CounterCallChain::Enter( CLR_RT_StackFrame* stack )
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

__nfweak void CLR_PROF_CounterCallChain::Leave()
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}
#endif

//--//

#if defined(NANOCLR_PROFILE_HANDLER)

__nfweak void CLR_PROF_Handler::Constructor()
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

#if defined(NANOCLR_PROFILE_NEW_CALLS)
__nfweak void CLR_PROF_Handler::Constructor( CLR_PROF_CounterCallChain& target )
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}
#endif

__nfweak void CLR_PROF_Handler::Destructor()
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}


__nfweak void CLR_PROF_Handler::Init( void* target )
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}


//--//

__nfweak void CLR_PROF_Handler::Calibrate()
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

__nfweak void CLR_PROF_Handler::SuspendTime()
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

__nfweak CLR_UINT64 CLR_PROF_Handler::GetFrozenTime()
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
    return 0;
}

__nfweak CLR_UINT64 CLR_PROF_Handler::ResumeTime()
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
    return 0;
}

__nfweak CLR_UINT64 CLR_PROF_Handler::ResumeTime( CLR_INT64 t )
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
    return 0;
}

#endif //#if defined(NANOCLR_PROFILE_HANDLER)

////////////////////////////////////////////////////////////////////////////////

//--//

#if defined(NANOCLR_PROFILE_NEW)

__nfweak CLR_PRF_Profiler g_CLR_PRF_Profiler;

__nfweak HRESULT CLR_PRF_Profiler::CreateInstance()
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
    return S_OK;
}

__nfweak HRESULT CLR_PRF_Profiler::DeleteInstance()
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
    return S_OK;
}

__nfweak HRESULT CLR_PRF_Profiler::Profiler_Cleanup()
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
    return S_OK;
}

__nfweak void CLR_PRF_Profiler::SendMemoryLayout()
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

__nfweak HRESULT CLR_PRF_Profiler::DumpHeap()
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
    return S_OK;
}

__nfweak HRESULT CLR_PRF_Profiler::DumpRoots()
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
    return S_OK;
}

__nfweak void CLR_PRF_Profiler::DumpRoot(CLR_RT_HeapBlock* root, CLR_UINT32 type, CLR_UINT32 flags, CLR_RT_MethodDef_Index* source)
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

__nfweak void CLR_PRF_Profiler::DumpObject(CLR_RT_HeapBlock* ptr)
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

__nfweak CLR_RT_HeapBlock* CLR_PRF_Profiler::FindReferencedObject(CLR_RT_HeapBlock* ref)
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
    return NULL;
}

__nfweak void CLR_PRF_Profiler::DumpEndOfRefsList()
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

__nfweak void CLR_PRF_Profiler::DumpPointer(void* ptr)
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

__nfweak void CLR_PRF_Profiler::DumpSingleReference(CLR_RT_HeapBlock* ptr)
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

__nfweak void CLR_PRF_Profiler::DumpListOfReferences(CLR_RT_HeapBlock* firstItem, CLR_UINT16 count)
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

__nfweak void CLR_PRF_Profiler::DumpListOfReferences(CLR_RT_DblLinkedList& list)
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

//--//

__nfweak void CLR_PRF_Profiler::Timestamp()
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

//--//

__nfweak HRESULT CLR_PRF_Profiler::RecordContextSwitch(CLR_RT_Thread* nextThread)
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
    return S_OK;
}

__nfweak HRESULT CLR_PRF_Profiler::RecordFunctionCall(CLR_RT_Thread* th, CLR_RT_MethodDef_Index md)
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
    return S_OK;
}

__nfweak HRESULT CLR_PRF_Profiler::RecordFunctionReturn(CLR_RT_Thread* th, CLR_PROF_CounterCallChain& prof)
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
    return S_OK;
}

//--//

__nfweak void CLR_PRF_Profiler::TrackObjectCreation( CLR_RT_HeapBlock* ptr )
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

__nfweak void CLR_PRF_Profiler::TrackObjectDeletion( CLR_RT_HeapBlock* ptr )
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

__nfweak void CLR_PRF_Profiler::TrackObjectRelocation()
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

__nfweak void CLR_PRF_Profiler::RecordGarbageCollectionBegin()
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

__nfweak void CLR_PRF_Profiler::RecordGarbageCollectionEnd()
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

__nfweak void CLR_PRF_Profiler::RecordHeapCompactionBegin()
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

__nfweak void CLR_PRF_Profiler::RecordHeapCompactionEnd()
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

//--//

__nfweak void CLR_PRF_Profiler::SendTrue()
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

__nfweak void CLR_PRF_Profiler::SendFalse()
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

__nfweak void CLR_PRF_Profiler::PackAndWriteBits(CLR_UINT32 value)
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

__nfweak void CLR_PRF_Profiler::PackAndWriteBits(const CLR_RT_TypeDef_Index& typeDef)
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

__nfweak void CLR_PRF_Profiler::PackAndWriteBits(const CLR_RT_MethodDef_Index& methodDef)
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
}

//--//

__nfweak HRESULT CLR_PRF_Profiler::Stream_Send()
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
    return S_OK;
}

__nfweak HRESULT CLR_PRF_Profiler::Stream_Flush()
{
    NATIVE_PROFILE_CLR_DIAGNOSTICS();
    return S_OK;
}

#endif //#if defined(NANOCLR_PROFILE_NEW)
