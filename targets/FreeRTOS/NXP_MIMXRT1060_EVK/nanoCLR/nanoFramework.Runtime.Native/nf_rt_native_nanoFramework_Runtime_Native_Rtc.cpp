//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//Z

#include <nanoHAL.h>
#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Checks.h>
#include <nf_rt_native.h>
#include "fsl_snvs_hp.h"



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// these functions are the real implementation of the 'weak' ones declared at src\CLR\Runtime.Native\nf_rt_native_nanoFramework_Runtime_Native_Rtc_stubs.cpp //
// the stubs for this class are generated by nanoFramework.Runtime.Native                                                                                    //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


HRESULT Library_nf_rt_native_nanoFramework_Runtime_Native_Rtc::Native_RTC_SetSystemTime___STATIC__BOOLEAN__I4__U1__U1__U1__U1__U1__U1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        snvs_hp_rtc_datetime_t rtcDate;

        // Stop the RTC before setting date
        SNVS_HP_RTC_StopTimer(SNVS);

        rtcDate.year = (uint16_t) stack.Arg0().NumericByRef().s4;  
        rtcDate.month = (uint8_t) stack.Arg1().NumericByRef().u1; 
        rtcDate.day = (uint8_t) stack.Arg2().NumericByRef().u1;   
        rtcDate.hour = (uint8_t) stack.Arg4().NumericByRef().u1;  
        rtcDate.minute = (uint8_t )stack.Arg5().NumericByRef().u1;
        rtcDate.second = (uint8_t) stack.Arg6().NumericByRef().u1; 

        // Set new date and start RTC
        SNVS_HP_RTC_SetDatetime(SNVS, &rtcDate);
        SNVS_HP_RTC_StartTimer(SNVS);

        // Return value to the managed application
        stack.SetResult_Boolean(true);
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}
