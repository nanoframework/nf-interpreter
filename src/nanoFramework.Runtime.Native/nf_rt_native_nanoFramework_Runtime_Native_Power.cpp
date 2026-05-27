//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_rt_native.h"
#include <nanoCLR_Debugging.h>

HRESULT Library_nf_rt_native_nanoFramework_Runtime_Native_Power::
    NativeReboot___STATIC__VOID__nanoFrameworkRuntimeNativeRebootOption(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    RebootOption rebootOption = (RebootOption)stack.Arg0().NumericByRef().u4;

    g_CLR_RT_ExecutionEngine.Reboot(rebootOption);

    NANOCLR_NOCLEANUP_NOLABEL();
}
