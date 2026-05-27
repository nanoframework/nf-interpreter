//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_rt_native.h"

HRESULT Library_nf_rt_native_System_Environment::get_TickCount64___STATIC__I8(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    int64_t ticksValue = CLR_RT_ExecutionEngine::GetUptime() / (int64_t)TIME_CONVERSION__TO_MILLISECONDS;

    stack.SetResult_I8(ticksValue);

    NANOCLR_NOCLEANUP_NOLABEL();
}
