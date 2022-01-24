//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_rt_native.h"

HRESULT Library_nf_rt_native_System_Environment::get_TickCount64___STATIC__I8(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock &ref = stack.PushValue();

    // get pointer to object pushed to the stack
    // and set with value from EE
    ref.Dereference()->NumericByRef().s8 = CLR_RT_ExecutionEngine::GetUptime();

    NANOCLR_NOCLEANUP_NOLABEL();
}
