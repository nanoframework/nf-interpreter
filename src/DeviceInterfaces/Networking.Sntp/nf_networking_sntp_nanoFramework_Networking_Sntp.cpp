//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_networking_sntp.h"

HRESULT Library_nf_networking_sntp_nanoFramework_Networking_Sntp::Start___STATIC__VOID(CLR_RT_StackFrame &stack)
{
    (void)stack;

    NANOCLR_HEADER();

#if defined(AZURE_RTOS_THREADX)
    // FIXME: implementation required!
#else
    sntp_init();
#endif

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_networking_sntp_nanoFramework_Networking_Sntp::Stop___STATIC__VOID(CLR_RT_StackFrame &stack)
{
    (void)stack;

    NANOCLR_HEADER();

#if defined(AZURE_RTOS_THREADX)
    // FIXME: implementation required!
#else
    sntp_stop();
#endif

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_networking_sntp_nanoFramework_Networking_Sntp::UpdateNow___STATIC__VOID(CLR_RT_StackFrame &stack)
{
    (void)stack;

    NANOCLR_HEADER();

#if defined(AZURE_RTOS_THREADX)
    // FIXME: implementation required!
#else

    // this is just a stop and start of the SNTP client
    sntp_stop();
    sntp_init();
#endif

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_networking_sntp_nanoFramework_Networking_Sntp::get_IsStarted___STATIC__BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

#if defined(AZURE_RTOS_THREADX)
        // FIXME: implementation required!
#else
        stack.SetResult_Boolean(sntp_enabled());
#endif
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_networking_sntp_nanoFramework_Networking_Sntp::get_Server1___STATIC__STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

#if defined(AZURE_RTOS_THREADX)
        // FIXME: implementation required!
#else
        stack.SetResult_String(sntp_getservername(0));
#endif
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_networking_sntp_nanoFramework_Networking_Sntp::set_Server1___STATIC__VOID__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // get server name as string
        char *serverName = (char *)stack.Arg0().RecoverString();
        FAULT_ON_NULL(serverName);

#if defined(AZURE_RTOS_THREADX)
        // FIXME: implementation required!
#else
        sntp_setservername(0, serverName);
#endif
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_networking_sntp_nanoFramework_Networking_Sntp::get_Server2___STATIC__STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

#if defined(AZURE_RTOS_THREADX)
        // FIXME: implementation required!
#else
        stack.SetResult_String(sntp_getservername(1));
#endif
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_networking_sntp_nanoFramework_Networking_Sntp::set_Server2___STATIC__VOID__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // get server name as string
        char *serverName = (char *)stack.Arg0().RecoverString();
        FAULT_ON_NULL(serverName);

#if defined(AZURE_RTOS_THREADX)
        // FIXME: implementation required!
#else
        sntp_setservername(1, serverName);
#endif
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_networking_sntp_nanoFramework_Networking_Sntp::get_UpdateInterval___STATIC__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(ESP_LWIP)
    // Return current value of update interval
    int32_t value = (int32_t)sntp_get_sync_interval();
    stack.SetResult_I4(value);
#elif defined(AZURE_RTOS_THREADX)
    // FIXME: implementation required!
    stack.SetResult_I4(-1);
#else
    // Return current value of update interval
    int32_t value = SNTP_UPDATE_DELAY;
    stack.SetResult_I4(value);
#endif

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_networking_sntp_nanoFramework_Networking_Sntp::set_UpdateInterval___STATIC__VOID__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(ESP_LWIP)
    uint32_t update_value = (int32_t)stack.Arg0().NumericByRef().s4;
    sntp_set_sync_interval(update_value);
#else
    // FIXME: implementation required!
    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());
#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_networking_sntp_nanoFramework_Networking_Sntp::get_StartupDelay___STATIC__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(AZURE_RTOS_THREADX)
    // FIXME: implementation required!
    stack.SetResult_I4(-1);
#else
    // Return current value of startup delay
    int32_t value = SNTP_STARTUP_DELAY;
    stack.SetResult_I4(value);
#endif

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_networking_sntp_nanoFramework_Networking_Sntp::set_StartupDelay___STATIC__VOID__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    // uint32_t update_value = (int32_t)stack.Arg0().NumericByRef().s4;

    // FIXME: implementation required!
    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_networking_sntp_nanoFramework_Networking_Sntp::get_RetryPeriod___STATIC__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(AZURE_RTOS_THREADX)
    // FIXME: implementation required!
    stack.SetResult_I4(-1);
#else
    // Return current value of retry period
    int32_t value = SNTP_RETRY_TIMEOUT;
    stack.SetResult_I4(value);
#endif

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_networking_sntp_nanoFramework_Networking_Sntp::set_RetryPeriod___STATIC__VOID__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    // uint32_t update_value = (int32_t)stack.Arg0().NumericByRef().s4;

    // FIXME: implementation required!
    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

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
