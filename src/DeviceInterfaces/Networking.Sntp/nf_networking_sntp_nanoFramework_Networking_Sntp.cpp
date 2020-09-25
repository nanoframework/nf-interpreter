//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//


#include "nf_networking_sntp.h"


HRESULT Library_nf_networking_sntp_nanoFramework_Networking_Sntp::Start___STATIC__VOID( CLR_RT_StackFrame& stack )
{
    (void)stack;

    NANOCLR_HEADER();

    sntp_init();

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_networking_sntp_nanoFramework_Networking_Sntp::Stop___STATIC__VOID( CLR_RT_StackFrame& stack )
{
    (void)stack;

    NANOCLR_HEADER();

    sntp_stop();

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_networking_sntp_nanoFramework_Networking_Sntp::UpdateNow___STATIC__VOID( CLR_RT_StackFrame& stack )
{
    (void)stack;

    NANOCLR_HEADER();

    // this is just a stop and start of the SNTP client
    sntp_stop();
    sntp_init();

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_networking_sntp_nanoFramework_Networking_Sntp::get_IsStarted___STATIC__BOOLEAN( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

        stack.SetResult_Boolean(sntp_enabled());
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_networking_sntp_nanoFramework_Networking_Sntp::get_Server1___STATIC__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

        stack.SetResult_String(sntp_getservername(0));
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_networking_sntp_nanoFramework_Networking_Sntp::set_Server1___STATIC__VOID__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

        // get server name as string
        char* serverName = (char*)stack.Arg0().RecoverString();  FAULT_ON_NULL(serverName);

        sntp_setservername(0, serverName);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_networking_sntp_nanoFramework_Networking_Sntp::get_Server2___STATIC__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

        stack.SetResult_String(sntp_getservername(1));
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_networking_sntp_nanoFramework_Networking_Sntp::set_Server2___STATIC__VOID__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

        // get server name as string
        char* serverName = (char*)stack.Arg0().RecoverString();  FAULT_ON_NULL(serverName);

        sntp_setservername(1, serverName);
    }
    NANOCLR_NOCLEANUP();
}

// need this declared as "C" because we are calling it from lwIP
extern "C" 
{
void SetSystemTimeFromUnixEpoch(uint32_t seconds)
{
    HAL_Time_SetUtcTime(((uint64_t)seconds * TIME_CONVERSION__TO_SECONDS) + TIME_UNIX_EPOCH_AS_TICKS);
}
}
