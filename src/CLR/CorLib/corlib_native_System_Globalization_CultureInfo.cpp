//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"


HRESULT Library_corlib_native_System_Globalization_CultureInfo::get_CurrentUICultureInternal___STATIC__SystemGlobalizationCultureInfo( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    stack.PushValue().SetObjectReference( g_CLR_RT_ExecutionEngine.m_currentUICulture );

    NANOCLR_NOCLEANUP_NOLABEL();
}
