//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"


HRESULT Library_corlib_native_System_Runtime_Remoting_RemotingServices::IsTransparentProxy___STATIC__BOOLEAN__OBJECT( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    bool              fProxy = false;    

  #if defined(NANOCLR_APPDOMAINS)
    CLR_RT_HeapBlock& obj    = stack.Arg0();
    fProxy = obj.IsTransparentProxy();
  #endif

    stack.SetResult_Boolean( fProxy );

    NANOCLR_NOCLEANUP_NOLABEL();
}
