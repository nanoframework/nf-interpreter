//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"

#if !defined(NANOCLR_EMULATED_FLOATINGPOINT)
    #include "nanoPAL_NativeDouble.h"

    HRESULT Library_corlib_native_System_Math::Abs___STATIC__I4__I4( CLR_RT_StackFrame& stack )
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

    HRESULT Library_corlib_native_System_Math::Max___STATIC__I4__I4__I4( CLR_RT_StackFrame& stack )
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

    HRESULT Library_corlib_native_System_Math::Max___STATIC__R4__R4__R4( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)
        return stack.NotImplementedStub();
      #else

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        float x = stack.Arg0().NumericByRefConst().r4;
        float y = stack.Arg1().NumericByRefConst().r4;
        float res = x >= y ? x : y;

        stack.SetResult_R4( res );

        NANOCLR_NOCLEANUP_NOLABEL();

      #endif 
    }

    HRESULT Library_corlib_native_System_Math::Max___STATIC__R8__R8__R8( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        double x = stack.Arg0().NumericByRefConst().r8;
        double y = stack.Arg1().NumericByRefConst().r8;
        double res = x >= y ? x : y;

        stack.SetResult_R8( res );

        NANOCLR_NOCLEANUP_NOLABEL();

      #else
        return stack.NotImplementedStub();
      #endif 
    }

    HRESULT Library_corlib_native_System_Math::Min___STATIC__I4__I4__I4( CLR_RT_StackFrame& stack )
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

    HRESULT Library_corlib_native_System_Math::Min___STATIC__R4__R4__R4( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)
        return stack.NotImplementedStub();
      #else

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        float x = stack.Arg0().NumericByRefConst().r4;
        float y = stack.Arg1().NumericByRefConst().r4;
        float res = x <= y ? x : y;

        stack.SetResult_R4( res );

        NANOCLR_NOCLEANUP_NOLABEL();

      #endif 
    }

    HRESULT Library_corlib_native_System_Math::Min___STATIC__R8__R8__R8( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        double x = stack.Arg0().NumericByRefConst().r8;
        double y = stack.Arg1().NumericByRefConst().r8;
        double res = x <= y ? x : y;

        stack.SetResult_R8( res );

        NANOCLR_NOCLEANUP_NOLABEL();

      #else
        return stack.NotImplementedStub();
      #endif 
    }

    HRESULT Library_corlib_native_System_Math::Abs___STATIC__R8__R8( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        double d = stack.Arg0().NumericByRefConst().r8;
        double res = fabs( d );

        stack.SetResult_R8( res );

        NANOCLR_NOCLEANUP_NOLABEL();        

      #else
        return stack.NotImplementedStub();
      #endif 
    }

    HRESULT Library_corlib_native_System_Math::Abs___STATIC__R4__R4( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)
        return stack.NotImplementedStub();
      #else

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        float d = stack.Arg0().NumericByRefConst().r4;
        float res = fabsf( d );

        stack.SetResult_R4( res );

        NANOCLR_NOCLEANUP_NOLABEL();        

      #endif 
    }

    HRESULT Library_corlib_native_System_Math::Acos___STATIC__R8__R8( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        double d = stack.Arg0().NumericByRefConst().r8;
        double res = acos( d );

        stack.SetResult_R8( res );

        NANOCLR_NOCLEANUP_NOLABEL();
    
      #else
        return stack.NotImplementedStub();
      #endif
    }

    HRESULT Library_corlib_native_System_Math::Acos___STATIC__R4__R4( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)
        return stack.NotImplementedStub();
      #else

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        float d = stack.Arg0().NumericByRefConst().r4;
        float res = acosf( d );

        stack.SetResult_R4( res );

        NANOCLR_NOCLEANUP_NOLABEL();        

      #endif
    }    

    HRESULT Library_corlib_native_System_Math::Asin___STATIC__R8__R8( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        double d = stack.Arg0().NumericByRefConst().r8;
        double res = asin( d );

        stack.SetResult_R8( res );

        NANOCLR_NOCLEANUP_NOLABEL();

      #else
        return stack.NotImplementedStub();
      #endif
    }

    HRESULT Library_corlib_native_System_Math::Asin___STATIC__R4__R4( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)
        return stack.NotImplementedStub();
      #else

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        float d = stack.Arg0().NumericByRefConst().r4;
        float res = asinf( d );

        stack.SetResult_R4( res );

        NANOCLR_NOCLEANUP_NOLABEL();

      #endif
    }

    HRESULT Library_corlib_native_System_Math::Atan___STATIC__R8__R8( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        double d = stack.Arg0().NumericByRefConst().r8;
        double res = atan( d );

        stack.SetResult_R8( res );

        NANOCLR_NOCLEANUP_NOLABEL();

      #else
        return stack.NotImplementedStub();
      #endif
    }

    HRESULT Library_corlib_native_System_Math::Atan___STATIC__R4__R4( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)
        return stack.NotImplementedStub();
      #else

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        float d = stack.Arg0().NumericByRefConst().r4;
        float res = atanf( d );

        stack.SetResult_R4( res );

        NANOCLR_NOCLEANUP_NOLABEL();

      #endif
    }

    HRESULT Library_corlib_native_System_Math::Atan2___STATIC__R8__R8__R8( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        double x = stack.Arg0().NumericByRefConst().r8;
        double y = stack.Arg1().NumericByRefConst().r8;
        double res = atan2( x, y );

        stack.SetResult_R8( res );

        NANOCLR_NOCLEANUP_NOLABEL();

      #else
        return stack.NotImplementedStub();
      #endif
    }

    HRESULT Library_corlib_native_System_Math::Atan2___STATIC__R4__R4__R4( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)
        return stack.NotImplementedStub();
      #else

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        float x = stack.Arg0().NumericByRefConst().r4;
        float y = stack.Arg1().NumericByRefConst().r4;
        float res = atan2f( x, y );

        stack.SetResult_R4( res );

        NANOCLR_NOCLEANUP_NOLABEL();

      #endif
    }

    HRESULT Library_corlib_native_System_Math::Ceiling___STATIC__R8__R8( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        double d = stack.Arg0().NumericByRefConst().r8;
        double res = ceil( d );

        stack.SetResult_R8( res );

        NANOCLR_NOCLEANUP_NOLABEL();

      #else
        return stack.NotImplementedStub();
      #endif

    }

    HRESULT Library_corlib_native_System_Math::Ceiling___STATIC__R4__R4( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)
        return stack.NotImplementedStub();
      #else

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        float d = stack.Arg0().NumericByRefConst().r4;
        float res = ceilf( d );

        stack.SetResult_R4( res );

        NANOCLR_NOCLEANUP_NOLABEL();

      #endif

    }

    HRESULT Library_corlib_native_System_Math::Cos___STATIC__R8__R8( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        double d = stack.Arg0().NumericByRefConst().r8;
        double res = cos( d );

        stack.SetResult_R8( res );

        NANOCLR_NOCLEANUP_NOLABEL();

      #else
        return stack.NotImplementedStub();
      #endif
    }

    HRESULT Library_corlib_native_System_Math::Cos___STATIC__R4__R4( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)
        return stack.NotImplementedStub();
      #else

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        float d = stack.Arg0().NumericByRefConst().r4;
        float res = cosf( d );

        stack.SetResult_R4( res );

        NANOCLR_NOCLEANUP_NOLABEL();

      #endif
    }

    HRESULT Library_corlib_native_System_Math::Cosh___STATIC__R8__R8( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        double d = stack.Arg0().NumericByRefConst().r8;
        double res = cosh( d );

        stack.SetResult_R8( res );

        NANOCLR_NOCLEANUP_NOLABEL();
     
      #else
        return stack.NotImplementedStub();
      #endif
    }

    HRESULT Library_corlib_native_System_Math::Cosh___STATIC__R4__R4( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)
        return stack.NotImplementedStub();
      #else

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        float d = stack.Arg0().NumericByRefConst().r4;
        float res = coshf( d );

        stack.SetResult_R4( res );

        NANOCLR_NOCLEANUP_NOLABEL();

      #endif
    }

    HRESULT Library_corlib_native_System_Math::IEEERemainder___STATIC__R8__R8__R8( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        double x = stack.Arg0().NumericByRefConst().r8;
        double y = stack.Arg1().NumericByRefConst().r8;
        double res = remainder(x, y);

        stack.SetResult_R8( res );

        NANOCLR_NOCLEANUP_NOLABEL();

      #else
        return stack.NotImplementedStub();
      #endif        
    }

    HRESULT Library_corlib_native_System_Math::IEEERemainder___STATIC__R4__R4__R4( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)
        return stack.NotImplementedStub();
      #else

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        float x = stack.Arg0().NumericByRefConst().r4;
        float y = stack.Arg1().NumericByRefConst().r4;
        float res = remainderf(x, y);

        stack.SetResult_R4( res );

        NANOCLR_NOCLEANUP_NOLABEL();

      #endif        
    }

    HRESULT Library_corlib_native_System_Math::Exp___STATIC__R8__R8( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        double d = stack.Arg0().NumericByRefConst().r8;
        double res = exp( d );

        stack.SetResult_R8( res );

        NANOCLR_NOCLEANUP_NOLABEL();;

      #else
        return stack.NotImplementedStub();
      #endif
    }

    HRESULT Library_corlib_native_System_Math::Exp___STATIC__R4__R4( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)
        return stack.NotImplementedStub();
      #else

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        float d = stack.Arg0().NumericByRefConst().r4;
        float res = expf( d );

        stack.SetResult_R4( res );

        NANOCLR_NOCLEANUP_NOLABEL();;

      #endif
    }

    HRESULT Library_corlib_native_System_Math::Floor___STATIC__R8__R8( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        double d = stack.Arg0().NumericByRefConst().r8;
        double res = floor( d );

        stack.SetResult_R8( res );

        NANOCLR_NOCLEANUP_NOLABEL();

      #else
        return stack.NotImplementedStub();
      #endif
    }

    HRESULT Library_corlib_native_System_Math::Floor___STATIC__R4__R4( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)
        return stack.NotImplementedStub();
      #else

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        float d = stack.Arg0().NumericByRefConst().r4;
        float res = floorf( d );

        stack.SetResult_R4( res );

        NANOCLR_NOCLEANUP_NOLABEL();

      #endif
    }

    HRESULT Library_corlib_native_System_Math::Log___STATIC__R8__R8( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        double d = stack.Arg0().NumericByRefConst().r8;
        double res = log( d );

        stack.SetResult_R8( res );

        NANOCLR_NOCLEANUP_NOLABEL();

      #else
        return stack.NotImplementedStub();
      #endif
    }

    HRESULT Library_corlib_native_System_Math::Log___STATIC__R4__R4( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)
        return stack.NotImplementedStub();
      #else

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        float d = stack.Arg0().NumericByRefConst().r4;
        float res = logf( d );

        stack.SetResult_R4( res );

        NANOCLR_NOCLEANUP_NOLABEL();

      #endif
    }

    HRESULT Library_corlib_native_System_Math::Log10___STATIC__R8__R8( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        double d = stack.Arg0().NumericByRefConst().r8;
        double res = log10( d );

        stack.SetResult_R8( res );

        NANOCLR_NOCLEANUP_NOLABEL();

      #else
        return stack.NotImplementedStub();
      #endif
    }

    HRESULT Library_corlib_native_System_Math::Log10___STATIC__R4__R4( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)
        return stack.NotImplementedStub();
      #else

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        float d = stack.Arg0().NumericByRefConst().r4;
        float res = log10f( d );

        stack.SetResult_R4( res );

        NANOCLR_NOCLEANUP_NOLABEL();

      #endif
    }

    HRESULT Library_corlib_native_System_Math::Pow___STATIC__R8__R8__R8( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        double x = stack.Arg0().NumericByRefConst().r8;
        double y = stack.Arg1().NumericByRefConst().r8;

        double res = pow( x, y );

        stack.SetResult_R8( res );

        NANOCLR_NOCLEANUP_NOLABEL();     
      #else
        return stack.NotImplementedStub();
      #endif
    }

    HRESULT Library_corlib_native_System_Math::Pow___STATIC__R4__R4__R4( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)
        return stack.NotImplementedStub();
      #else

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        float x = stack.Arg0().NumericByRefConst().r4;
        float y = stack.Arg1().NumericByRefConst().r4;

        float res = powf( x, y );

        stack.SetResult_R4( res );

        NANOCLR_NOCLEANUP_NOLABEL();     

      #endif
    }

    HRESULT Library_corlib_native_System_Math::Round___STATIC__R8__R8( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        double d = stack.Arg0().NumericByRefConst().r8;

        double res = round(d);

        stack.SetResult_R8( res );

        NANOCLR_NOCLEANUP_NOLABEL();

      #else
        return stack.NotImplementedStub();
      #endif
    }

    HRESULT Library_corlib_native_System_Math::Round___STATIC__R4__R4( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)
        return stack.NotImplementedStub();
      #else

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        float d = stack.Arg0().NumericByRefConst().r4;

        float res = roundf(d);

        stack.SetResult_R4( res );

        NANOCLR_NOCLEANUP_NOLABEL();

      #endif
    }

    HRESULT Library_corlib_native_System_Math::Sign___STATIC__I4__R8( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        double d = stack.Arg0().NumericByRefConst().r8;
        int32_t res;
        if (d < 0) res =  -1;
        else if (d > 0) res =  +1;
        else res = 0;

        stack.SetResult_I4( res );

        NANOCLR_NOCLEANUP_NOLABEL();     
      #else
        return stack.NotImplementedStub();
      #endif
    }

    HRESULT Library_corlib_native_System_Math::Sign___STATIC__I4__R4( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)
        return stack.NotImplementedStub();
      #else

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        float d = stack.Arg0().NumericByRefConst().r4;
        int32_t res;
        if (d < 0) res =  -1;
        else if (d > 0) res =  +1;
        else res = 0;

        stack.SetResult_I4( res );

        NANOCLR_NOCLEANUP_NOLABEL();     

      #endif
    }

    HRESULT Library_corlib_native_System_Math::Sin___STATIC__R8__R8( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        double d = stack.Arg0().NumericByRefConst().r8;
        double res = sin( d );

        stack.SetResult_R8( res );

        NANOCLR_NOCLEANUP_NOLABEL();

      #else
        return stack.NotImplementedStub();
      #endif
    }

    HRESULT Library_corlib_native_System_Math::Sin___STATIC__R4__R4( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)
        return stack.NotImplementedStub();
      #else

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        float d = stack.Arg0().NumericByRefConst().r4;
        float res = sinf( d );

        stack.SetResult_R4( res );

        NANOCLR_NOCLEANUP_NOLABEL();

      #endif
    }

    HRESULT Library_corlib_native_System_Math::Sinh___STATIC__R8__R8( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        double d = stack.Arg0().NumericByRefConst().r8;
        double res = sinh( d );

        stack.SetResult_R8( res );

        NANOCLR_NOCLEANUP_NOLABEL();

      #else
        return stack.NotImplementedStub();
      #endif
    }

    HRESULT Library_corlib_native_System_Math::Sinh___STATIC__R4__R4( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)
        return stack.NotImplementedStub();
      #else

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        float d = stack.Arg0().NumericByRefConst().r4;
        float res = sinhf( d );

        stack.SetResult_R4( res );

        NANOCLR_NOCLEANUP_NOLABEL();

      #endif
    }

    HRESULT Library_corlib_native_System_Math::Sqrt___STATIC__R8__R8( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        double d = stack.Arg0().NumericByRefConst().r8;
        double res = sqrt( d );

        stack.SetResult_R8( res );

        NANOCLR_NOCLEANUP_NOLABEL();

      #else
        return stack.NotImplementedStub();
      #endif
    }

    HRESULT Library_corlib_native_System_Math::Sqrt___STATIC__R4__R4( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)
        return stack.NotImplementedStub();
      #else

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        float d = stack.Arg0().NumericByRefConst().r4;
        float res = sqrtf( d );

        stack.SetResult_R4( res );

        NANOCLR_NOCLEANUP_NOLABEL();

      #endif
    }

    HRESULT Library_corlib_native_System_Math::Tan___STATIC__R8__R8( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        double d = stack.Arg0().NumericByRefConst().r8;
        double res = tan( d );

        stack.SetResult_R8( res );

        NANOCLR_NOCLEANUP_NOLABEL();
     
      #else
        return stack.NotImplementedStub();
      #endif
    }

    HRESULT Library_corlib_native_System_Math::Tan___STATIC__R4__R4( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)
        return stack.NotImplementedStub();
      #else

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        float d = stack.Arg0().NumericByRefConst().r4;
        float res = tanf( d );

        stack.SetResult_R4( res );

        NANOCLR_NOCLEANUP_NOLABEL();

      #endif
    }

    HRESULT Library_corlib_native_System_Math::Tanh___STATIC__R8__R8( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        double d = stack.Arg0().NumericByRefConst().r8;
        double res = tanh( d );

        stack.SetResult_R8( res );

        NANOCLR_NOCLEANUP_NOLABEL();

      #else
        return stack.NotImplementedStub();
      #endif
    }

    HRESULT Library_corlib_native_System_Math::Tanh___STATIC__R4__R4( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)
        return stack.NotImplementedStub();
      #else

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        float d = stack.Arg0().NumericByRefConst().r4;
        float res = tanhf( d );

        stack.SetResult_R4( res );

        NANOCLR_NOCLEANUP_NOLABEL();

      #endif
    }

    HRESULT Library_corlib_native_System_Math::Truncate___STATIC__R8__R8( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        double d = stack.Arg0().NumericByRefConst().r8;
        double res = 0.0;
        double retVal = System::Math::Truncate(d, res);

        stack.SetResult_R8( retVal );

        NANOCLR_NOCLEANUP_NOLABEL();
     
      #else
        return stack.NotImplementedStub();
      #endif
    }

    HRESULT Library_corlib_native_System_Math::Truncate___STATIC__R4__R4( CLR_RT_StackFrame& stack )
    {
      #if (DP_FLOATINGPOINT == TRUE)
        return stack.NotImplementedStub();
      #else

        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        float d = stack.Arg0().NumericByRefConst().r4;
        float res = 0.0;
        float retVal = modff(d, &res); 

        stack.SetResult_R4( retVal );

        NANOCLR_NOCLEANUP_NOLABEL();

      #endif 

    }

#else

    /// No floating point 
    HRESULT Library_corlib_native_System_Math::Abs___STATIC__I4__I4( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::Max___STATIC__I4__I4__I4( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::Max___STATIC__R4__R4__R4( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::Max___STATIC__R8__R8__R8( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::Min___STATIC__I4__I4__I4( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::Min___STATIC__R4__R4__R4( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::Min___STATIC__R8__R8__R8( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::Abs___STATIC__R8__R8( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::Abs___STATIC__R4__R4( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::Acos___STATIC__R8__R8( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }
    
    HRESULT Library_corlib_native_System_Math::Acos___STATIC__R4__R4( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::Asin___STATIC__R8__R8( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::Asin___STATIC__R4__R4( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::Atan___STATIC__R8__R8( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::Atan___STATIC__R4__R4( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::Atan2___STATIC__R8__R8__R8( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::Atan2___STATIC__R4__R4__R4( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }
    
    HRESULT Library_corlib_native_System_Math::Ceiling___STATIC__R8__R8( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        CLR_INT64  d = stack.Arg0().NumericByRefConst().r8;

        CLR_INT64 res = (CLR_INT64)(d + (CLR_INT64)CLR_RT_HeapBlock::HB_DoubleMask) & (~CLR_RT_HeapBlock::HB_DoubleMask);

        stack.SetResult_I8( res );

        NANOCLR_NOCLEANUP_NOLABEL();
    }

    HRESULT Library_corlib_native_System_Math::Ceiling___STATIC__R4__R4( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }
    
    HRESULT Library_corlib_native_System_Math::Cos___STATIC__R8__R8( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::Cos___STATIC__R4__R4( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::Cosh___STATIC__R8__R8( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::Cosh___STATIC__R4__R4( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::IEEERemainder___STATIC__R8__R8__R8( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::IEEERemainder___STATIC__R4__R4__R4( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::Exp___STATIC__R8__R8( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::Exp___STATIC__R4__R4( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::Floor___STATIC__R8__R8( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        CLR_INT64 d = stack.Arg0().NumericByRefConst().r8;
        CLR_INT64 res = (CLR_INT64)( d  & (~CLR_RT_HeapBlock::HB_DoubleMask) );
        stack.SetResult_I8( res );

        NANOCLR_NOCLEANUP_NOLABEL();
    }

    HRESULT Library_corlib_native_System_Math::Floor___STATIC__R4__R4( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::Log___STATIC__R8__R8( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::Log___STATIC__R4__R4( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::Log10___STATIC__R8__R8( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::Log10___STATIC__R4__R4( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::Pow___STATIC__R8__R8__R8( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::Pow___STATIC__R4__R4__R4( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::Round___STATIC__R8__R8( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        CLR_INT64 d = stack.Arg0().NumericByRefConst().r8;

        //for negative number we have to be banker's round, if -0.5, round to 0, but 0.5 to 0
        if (d <0) d =d + 1; 
        CLR_INT64 res = (CLR_INT64)(d + (CLR_INT64)(CLR_RT_HeapBlock::HB_DoubleMask>>1) ) & (~CLR_RT_HeapBlock::HB_DoubleMask);

        stack.SetResult_I8( res );


        NANOCLR_NOCLEANUP_NOLABEL();
    }

    HRESULT Library_corlib_native_System_Math::Round___STATIC__R4__R4( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::Sign___STATIC__I4__R8( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::Sign___STATIC__I4__R4( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::Sin___STATIC__R8__R8( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::Sin___STATIC__R4__R4( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::Sinh___STATIC__R8__R8( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::Sinh___STATIC__R4__R4( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::Sqrt___STATIC__R8__R8( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::Sqrt___STATIC__R4__R4( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::Tan___STATIC__R8__R8( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::Tan___STATIC__R4__R4( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::Tanh___STATIC__R8__R8( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::Tanh___STATIC__R4__R4( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::Truncate___STATIC__R8__R8( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

    HRESULT Library_corlib_native_System_Math::Truncate___STATIC__R4__R4( CLR_RT_StackFrame& stack )
    {
        NATIVE_PROFILE_CLR_CORE();
        NANOCLR_HEADER();

        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

        NANOCLR_NOCLEANUP();
    }

#endif  // NANOCLR_EMULATED_FLOATINGPOINT
