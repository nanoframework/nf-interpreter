//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"


HRESULT Library_corlib_native_System_ValueType::Equals___BOOLEAN__OBJECT( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    stack.SetResult_Boolean( CLR_RT_HeapBlock::ObjectsEqual( stack.Arg0(), stack.Arg1(), false ) );

    NANOCLR_NOCLEANUP_NOLABEL();
}
