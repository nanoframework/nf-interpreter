//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//


#include "nf_rt_native.h"

HRESULT Library_nf_rt_native_nanoFramework_Runtime_Native_Power::NativeReboot___STATIC__VOID( CLR_RT_StackFrame& stack )
{
    (void)stack;

    NANOCLR_HEADER();
    {
        g_CLR_RT_ExecutionEngine.Reboot(true);
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}
