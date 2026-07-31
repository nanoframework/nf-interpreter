//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_networking_sntp.h"

#if defined(TARGET_HAS_WIFI_ISM43362)
// implemented in targets/ChibiOS/_WiFi/inventek/sntp_ism43362.cpp - see the comment there for why
// this board needs its own synchronous NTP client instead of lwIP's sntp_* API
extern bool Ism43362_Sntp_Sync(const char *serverName);

// this board has no periodic background SNTP service (Start()/UpdateNow() both just perform one
// immediate, synchronous NTP query) - these track enough state to back Server1/Server2/IsStarted
static char s_ism43362SntpServer1[64] = "pool.ntp.org";
static char s_ism43362SntpServer2[64] = "time.nist.gov";
static bool s_ism43362SntpStarted = false;
#endif

HRESULT Library_nf_networking_sntp_nanoFramework_Networking_Sntp::Start___STATIC__VOID(CLR_RT_StackFrame &stack)
{
    (void)stack;

    NANOCLR_HEADER();

#if defined(TARGET_HAS_WIFI_ISM43362)
    s_ism43362SntpStarted = Ism43362_Sntp_Sync(s_ism43362SntpServer1);
    if (!s_ism43362SntpStarted)
    {
        s_ism43362SntpStarted = Ism43362_Sntp_Sync(s_ism43362SntpServer2);
    }
#elif defined(THREADX_RTOS)
#else
    sntp_init();
#endif

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_networking_sntp_nanoFramework_Networking_Sntp::Stop___STATIC__VOID(CLR_RT_StackFrame &stack)
{
    (void)stack;

    NANOCLR_HEADER();

#if defined(TARGET_HAS_WIFI_ISM43362)
    // no background service to stop - just clear the tracked state
    s_ism43362SntpStarted = false;
#elif defined(THREADX_RTOS)
#else
    sntp_stop();
#endif

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_networking_sntp_nanoFramework_Networking_Sntp::UpdateNow___STATIC__VOID(CLR_RT_StackFrame &stack)
{
    (void)stack;

    NANOCLR_HEADER();

#if defined(TARGET_HAS_WIFI_ISM43362)
    s_ism43362SntpStarted = Ism43362_Sntp_Sync(s_ism43362SntpServer1);
    if (!s_ism43362SntpStarted)
    {
        s_ism43362SntpStarted = Ism43362_Sntp_Sync(s_ism43362SntpServer2);
    }
#elif defined(THREADX_RTOS)
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

#if defined(TARGET_HAS_WIFI_ISM43362)
        stack.SetResult_Boolean(s_ism43362SntpStarted);
#elif defined(THREADX_RTOS)
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

#if defined(TARGET_HAS_WIFI_ISM43362)
        stack.SetResult_String(s_ism43362SntpServer1);
#elif defined(THREADX_RTOS)
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

#if defined(TARGET_HAS_WIFI_ISM43362)
        hal_strncpy_s(s_ism43362SntpServer1, sizeof(s_ism43362SntpServer1), serverName, hal_strlen_s(serverName));
#elif defined(THREADX_RTOS)
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

#if defined(TARGET_HAS_WIFI_ISM43362)
        stack.SetResult_String(s_ism43362SntpServer2);
#elif defined(THREADX_RTOS)
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

#if defined(TARGET_HAS_WIFI_ISM43362)
        hal_strncpy_s(s_ism43362SntpServer2, sizeof(s_ism43362SntpServer2), serverName, hal_strlen_s(serverName));
#elif defined(THREADX_RTOS)
#else
        sntp_setservername(1, serverName);
#endif
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
