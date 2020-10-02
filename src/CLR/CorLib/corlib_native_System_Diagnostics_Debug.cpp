//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "CorLib.h"


HRESULT Library_corlib_native_System_Diagnostics_Debug::WriteLineNative___STATIC__VOID__STRING__BOOLEAN( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    const char * szText0 = stack.Arg0().RecoverString();
    bool addLineFeed = (bool)stack.Arg1().NumericByRef().u1;

    if(!szText0) szText0 = "<null>";

    CLR_Debug::Emit( szText0, -1 );

    if(addLineFeed)
    {
        CLR_Debug::Emit( "\r\n" , -1 );
    }
    
    NANOCLR_NOCLEANUP_NOLABEL();
}

