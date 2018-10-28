//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"

#if !defined(NANOCLR_EMULATED_FLOATINGPOINT)
    #include "nanoPAL_NativeDouble.h"

    HRESULT Library_corlib_native_System_MathInternal::Abs___STATIC__I4__I4( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)
        stack.NotImplementedStub();  
      #else

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        float d = stack.Arg0().NumericByRefConst().s4;
        float res = fabs( d );

        stack.SetResult_I4( res );

        NANOCLR_NOCLEANUP_NOLABEL();        

      #endif 
    }

    HRESULT Library_corlib_native_System_MathInternal::Max___STATIC__I4__I4__I4( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)
        return stack.NotImplementedStub();
      #else

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        float x = stack.Arg0().NumericByRefConst().s4;
        float y = stack.Arg1().NumericByRefConst().s4;
        float res = x >= y ? x : y;

        stack.SetResult_I4( res );

        NANOCLR_NOCLEANUP_NOLABEL();

      #endif 
    }

    HRESULT Library_corlib_native_System_MathInternal::Min___STATIC__I4__I4__I4( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)
        return stack.NotImplementedStub();
      #else

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        float x = stack.Arg0().NumericByRefConst().s4;
        float y = stack.Arg1().NumericByRefConst().s4;
        float res = x <= y ? x : y;

        stack.SetResult_I4( res );

        NANOCLR_NOCLEANUP_NOLABEL();

      #endif 
    }

#else

    /// No floating point
    HRESULT Library_corlib_native_System_MathInternal::Abs___STATIC__I4__I4( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_MathInternal::Max___STATIC__I4__I4__I4( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_MathInternal::Min___STATIC__I4__I4__I4( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

#endif  // NANOCLR_EMULATED_FLOATINGPOINT
