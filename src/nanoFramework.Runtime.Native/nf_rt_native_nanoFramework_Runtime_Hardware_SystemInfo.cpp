//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//


#include "nf_rt_native.h"
#include "nanoHAL_ReleaseInfo.h"


HRESULT Library_nf_rt_native_nanoFramework_Runtime_Native_SystemInfo::GetSystemVersion___STATIC__VOID__BYREF_I4__BYREF_I4__BYREF_I4__BYREF_I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();

    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock hbMajor;
        CLR_RT_HeapBlock hbMinor;
        CLR_RT_HeapBlock hbBuild;
        CLR_RT_HeapBlock hbRevision;

        NFReleaseInfo releaseInfo;

        Target_GetReleaseInfo( releaseInfo );

        hbMajor.SetInteger( releaseInfo.Version.usMajor );
        NANOCLR_CHECK_HRESULT(hbMajor.StoreToReference( stack.Arg0(), 0 ));

        hbMinor.SetInteger( releaseInfo.Version.usMinor );
        NANOCLR_CHECK_HRESULT(hbMinor.StoreToReference( stack.Arg1(), 0 ));

        hbBuild.SetInteger( releaseInfo.Version.usBuild );
        NANOCLR_CHECK_HRESULT(hbBuild.StoreToReference( stack.Arg2(), 0 ));

        hbRevision.SetInteger( releaseInfo.Version.usRevision );
        NANOCLR_CHECK_HRESULT(hbRevision.StoreToReference( stack.Arg3(), 0 ));
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_rt_native_nanoFramework_Runtime_Native_SystemInfo::get_OEMString___STATIC__STRING( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();

    NANOCLR_HEADER();
    {
        NFReleaseInfo releaseInfo;

        Target_GetReleaseInfo( releaseInfo );

        NANOCLR_SET_AND_LEAVE(stack.SetResult_String( (char*)releaseInfo.InfoString ));
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_rt_native_nanoFramework_Runtime_Native_SystemInfo::get_OEM___STATIC__U1( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();

    NANOCLR_HEADER();

    // we are not supporting this right now
    //stack.SetResult( OEM_Model_SKU.OEM, DATATYPE_U1 );
    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    //NANOCLR_NOCLEANUP_NOLABEL();
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_rt_native_nanoFramework_Runtime_Native_SystemInfo::get_Model___STATIC__U1( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();

    NANOCLR_HEADER();

    // we are not supporting this right now
    // stack.SetResult( OEM_Model_SKU.Model, DATATYPE_U1 );
    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());
    
    //NANOCLR_NOCLEANUP_NOLABEL();
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_rt_native_nanoFramework_Runtime_Native_SystemInfo::get_SKU___STATIC__U2( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();

    NANOCLR_HEADER();

    // we are not supporting this right now
    // stack.SetResult( OEM_Model_SKU.SKU, DATATYPE_U2 );
    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    //NANOCLR_NOCLEANUP_NOLABEL();
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_rt_native_nanoFramework_Runtime_Native_SystemInfo::get_TargetName___STATIC__STRING( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();

    NANOCLR_HEADER();

    NFReleaseInfo releaseInfo;

    Target_GetReleaseInfo( releaseInfo );

    NANOCLR_SET_AND_LEAVE(stack.SetResult_String( (char*)releaseInfo.TargetName ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_rt_native_nanoFramework_Runtime_Native_SystemInfo::get_Platform___STATIC__STRING( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();

    NANOCLR_HEADER();


    NFReleaseInfo releaseInfo;

    Target_GetReleaseInfo( releaseInfo );

    NANOCLR_SET_AND_LEAVE(stack.SetResult_String( (char*)releaseInfo.PlatformName ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_rt_native_nanoFramework_Runtime_Native_SystemInfo::GetNativeFloatingPointSupport___STATIC__U1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        FloatingPoint floatingPointOption = FloatingPoint_None;

      #if !defined(NANOCLR_EMULATED_FLOATINGPOINT)

        #if (USE_FPU_IS_TRUE == TRUE)

            #if (DP_FLOATINGPOINT == TRUE)
                floatingPointOption = FloatingPoint_DoublePrecisionHardware;
            #else
                floatingPointOption = FloatingPoint_SinglePrecisionHardware;
            #endif 

        #else

            #if (DP_FLOATINGPOINT == TRUE)
                floatingPointOption = FloatingPoint_DoublePrecisionSoftware;
            #else
                floatingPointOption = FloatingPoint_SinglePrecisionSoftware;
            #endif 

        #endif  // (USE_FPU_IS_TRUE == TRUE)

      #endif // defined(NANOCLR_EMULATED_FLOATINGPOINT)

        stack.SetResult_U1((int8_t)floatingPointOption);
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}
