//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//


#include "nf_rt_native.h"
#include <nanoWeak.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// these functions are declared as 'weak' so they can be replaced by the actual ones when they exist on the target platform 
// they should be located at targets\NNNN\YYYY\nanoCLR\nanoFramework.Runtime.Native
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


__nfweak HRESULT Library_nf_rt_native_nanoFramework_Runtime_Native_Rtc::Native_RTC_SetSystemTime___STATIC__BOOLEAN__I4__U1__U1__U1__U1__U1__U1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}
