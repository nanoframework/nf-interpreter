//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "CorLib.h"

HRESULT Library_corlib_native_System_Runtime_CompilerServices_Unsafe::As___STATIC__GENERICTYPE__OBJECT(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock &arg = stack.Arg0();

    // Set the argument as the return value (ldarg.0; ret)
    stack.PushValueAndAssign(arg);

    NANOCLR_NOCLEANUP();
}
