//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _NF_DT_NATIVE_H_
#define _NF_DT_NATIVE_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>

struct Library_nf_dt_native_System_DateTime
{
    static const int FIELD_STATIC__MinValue = 3;
    static const int FIELD_STATIC__MaxValue = 4;

    static const int FIELD___ticks = 1;

    NANOCLR_NATIVE_DECLARE(_ctor___VOID__I4__I4__I4__I4__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(get_Day___I4);
    NANOCLR_NATIVE_DECLARE(get_DayOfWeek___SystemDayOfWeek);
    NANOCLR_NATIVE_DECLARE(get_DayOfYear___I4);
    NANOCLR_NATIVE_DECLARE(get_Hour___I4);
    NANOCLR_NATIVE_DECLARE(get_Millisecond___I4);
    NANOCLR_NATIVE_DECLARE(get_Minute___I4);
    NANOCLR_NATIVE_DECLARE(get_Month___I4);
    NANOCLR_NATIVE_DECLARE(get_Second___I4);
    NANOCLR_NATIVE_DECLARE(get_Year___I4);
    NANOCLR_NATIVE_DECLARE(DaysInMonth___STATIC__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(get_UtcNow___STATIC__SystemDateTime);
    NANOCLR_NATIVE_DECLARE(get_Today___STATIC__SystemDateTime);

    //--//

    static CLR_INT64* NewObject  ( CLR_RT_StackFrame& stack );
    static CLR_INT64* GetValuePtr( CLR_RT_StackFrame& stack );
    static CLR_INT64* GetValuePtr( CLR_RT_HeapBlock&  ref   );

    static void Expand  ( CLR_RT_StackFrame& stack,       SYSTEMTIME& st );
    static void Compress( CLR_RT_StackFrame& stack, const SYSTEMTIME& st );
};

struct Library_nf_dt_native_System_Globalization_DateTimeFormat
{
    NANOCLR_NATIVE_DECLARE(FormatDigits___STATIC__STRING__I4__I4);

    //--//

};

struct Library_nf_dt_native_System_Globalization_DateTimeFormatInfo
{
    static const int FIELD___generalShortTimePattern = 1;
    static const int FIELD___generalLongTimePattern = 2;
    static const int FIELD___fullDateTimePattern = 3;
    static const int FIELD__CultureInfo = 4;


    //--//

};

struct Library_nf_dt_native_nanoFramework_DateTime_RTC
{
    NANOCLR_NATIVE_DECLARE(Native_RTC_SetSystemTime___STATIC__BOOLEAN__I4__U1__U1__U1__U1__U1__U1);
    NANOCLR_NATIVE_DECLARE(Native_RTC_SetAlarm___STATIC__BOOLEAN__I4__U1__U1__U1__U1__U1__U1);

    //--//

};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_DateTime;

#endif  //_NF_DT_NATIVE_H_
