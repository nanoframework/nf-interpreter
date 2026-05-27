//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "Core.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

HRESULT CLR_RT_HeapBlock_LockRequest::CreateInstance( CLR_RT_HeapBlock_Lock* lock, CLR_RT_SubThread* sth, const CLR_INT64& timeExpire, bool fForce )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_LockRequest* req = EVENTCACHE_EXTRACT_NODE(g_CLR_RT_EventCache,CLR_RT_HeapBlock_LockRequest,DATATYPE_LOCK_REQUEST_HEAD); CHECK_ALLOCATION(req);

    req->m_subthreadWaiting = sth;
    req->m_timeExpire       = timeExpire; CLR_RT_ExecutionEngine::InvalidateTimerCache();
    req->m_fForce           = fForce;

    lock->m_requests.LinkAtBack( req );

    sth->ChangeLockRequestCount( +1 );

    NANOCLR_SET_AND_LEAVE(CLR_E_THREAD_WAITING);

    NANOCLR_NOCLEANUP();
}

