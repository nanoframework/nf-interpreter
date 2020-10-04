//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "CorLib.h"

#if !defined(_WIN32)
#include <nf_rt_native.h>
#endif

HRESULT Library_corlib_native_System_Console::OutNative___STATIC__VOID__STRING__BOOLEAN( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

#if defined(_WIN32)
    return stack.NotImplementedStub();
#else
    NANOCLR_CHECK_HRESULT(Library_corlib_native_System_Diagnostics_Debug::WriteLineNative___STATIC__VOID__STRING__BOOLEAN( stack ));
#endif

    NANOCLR_NOCLEANUP();    
}
