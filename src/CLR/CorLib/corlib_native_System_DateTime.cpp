//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "CorLib.h"
#include <nanoCLR_Interop.h>

///////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH DateTime.DateTimePart (in managed code) !!! //
///////////////////////////////////////////////////////////////////////
enum DateTimePart
{
    DateTimePart_Year,
    DateTimePart_Month,
    DateTimePart_Day,
    DateTimePart_DayOfWeek,
    DateTimePart_DayOfYear,
    DateTimePart_Hour,
    DateTimePart_Minute,
    DateTimePart_Second,
    DateTimePart_Millisecond
};

////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH DateTime._UTCMask (in managed code)  !!! //
// !!! KEEP IN SYNC WITH DateTime._tickMask (in managed code) !!! //
////////////////////////////////////////////////////////////////////
CLR_INT64 s_UTCMask   = ULONGLONGCONSTANT(0x8000000000000000);
CLR_INT64 s_TickMask  = ULONGLONGCONSTANT(0x7FFFFFFFFFFFFFFF);

HRESULT Library_corlib_native_System_DateTime::_ctor___VOID__I4__I4__I4__I4__I4__I4__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock* pArg = &(stack.Arg1());
    SYSTEMTIME        st;

    NANOCLR_CLEAR(st);
    st.wYear         = pArg[ 0 ].NumericByRef().s4;
    st.wMonth        = pArg[ 1 ].NumericByRef().s4;
    st.wDay          = pArg[ 2 ].NumericByRef().s4;
    st.wHour         = pArg[ 3 ].NumericByRef().s4;
    st.wMinute       = pArg[ 4 ].NumericByRef().s4;
    st.wSecond       = pArg[ 5 ].NumericByRef().s4;
    st.wMilliseconds = pArg[ 6 ].NumericByRef().s4;

    /// Our current supported range is between 1601 and 3000. Years before 1582 requires different calculation (see explanation
    /// in time_decl.h), same way years after 3000 will not hold the simple arithmetic which we are using. More complex calculations
    /// outside these range are not worth the CPU cycle and codesize.
    // some checks below have to use the parameter because the st struct uses unsigned types
    if (
        (st.wYear < 1601) || 
        (st.wYear > 3000) ||
        (st.wMonth < 1) || 
        (st.wMonth > 12) ||
        (st.wDay < 1) || 
        (st.wDay > 31) ||
        (pArg[ 3 ].NumericByRef().s4 < 0) ||
        (st.wHour > 23) ||
        (pArg[ 4 ].NumericByRef().s4 < 0) ||
        (st.wMinute > 59) ||
        (pArg[ 5 ].NumericByRef().s4 < 0) ||
        (st.wSecond > 59) ||
        (st.wMilliseconds < pArg[ 6 ].NumericByRef().s4) ||
        (st.wMilliseconds > 1000)) 
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }
    
    Compress( stack, st );

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_DateTime::GetDateTimePart___I4__SystemDateTimeDateTimePart( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    signed int days;
    SYSTEMTIME st;

    DateTimePart dateTimePart  = (DateTimePart)stack.Arg1().NumericByRef().s4;

    if( Expand( stack, st ) )
    {
        switch (dateTimePart)
        {
            case DateTimePart_Year:
                stack.SetResult_I4( st.wYear );
                break;

            case DateTimePart_Month :
                stack.SetResult_I4( st.wMonth );
                break;

            case DateTimePart_Day:
                stack.SetResult_I4( st.wDay );
                break;

            case DateTimePart_DayOfWeek:
                stack.SetResult_I4( st.wDayOfWeek );
                break;

            case DateTimePart_DayOfYear:
                NANOCLR_CHECK_HRESULT( HAL_Time_AccDaysInMonth( st.wYear, st.wMonth, &days ) );
                days += st.wDay;
                stack.SetResult_I4( days );
                break;

            case DateTimePart_Hour:
                stack.SetResult_I4( st.wHour );
                break;

            case DateTimePart_Minute:
                stack.SetResult_I4( st.wMinute );
                break;

            case DateTimePart_Second:
                stack.SetResult_I4( st.wSecond );
                break;

            case DateTimePart_Millisecond:
                stack.SetResult_I4( st.wMilliseconds );
                break;

            default:
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                break;
        }
    }
    else
    {
        // expand call failed for whatever reason
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_DateTime::DaysInMonth___STATIC__I4__I4__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_INT32 year  = stack.Arg0().NumericByRef().s4;
    CLR_INT32 month = stack.Arg1().NumericByRef().s4;
    CLR_INT32 days  = 0;

    NANOCLR_CHECK_HRESULT( HAL_Time_DaysInMonth( year, month, &days ) );

    stack.SetResult_I4( days );

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_DateTime::get_UtcNow___STATIC__SystemDateTime( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_TypeDescriptor dtType;
    CLR_INT64*            val;

    CLR_RT_HeapBlock& ref = stack.PushValue();

    // initialize <DateTime> type descriptor
    NANOCLR_CHECK_HRESULT( dtType.InitializeFromType( g_CLR_RT_WellKnownTypes.m_DateTime ) );

    // create an instance of <DateTime>
    NANOCLR_CHECK_HRESULT( g_CLR_RT_ExecutionEngine.NewObject( ref, dtType.m_handlerCls ) );

    val = GetValuePtr( ref );
    
    // load with full date&time
    // including UTC flag
    *val = HAL_Time_CurrentDateTime(false) | s_UTCMask;

    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_DateTime::get_Today___STATIC__SystemDateTime( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_TypeDescriptor dtType;
    CLR_INT64*            val;

    CLR_RT_HeapBlock& ref = stack.PushValue();

    // initialize <DateTime> type descriptor
    NANOCLR_CHECK_HRESULT( dtType.InitializeFromType( g_CLR_RT_WellKnownTypes.m_DateTime ) );

    // create an instance of <DateTime>
    NANOCLR_CHECK_HRESULT( g_CLR_RT_ExecutionEngine.NewObject( ref, dtType.m_handlerCls ) );

    val = GetValuePtr( ref );

    // load with date part only
    // including UTC flag
    *val = HAL_Time_CurrentDateTime(true) | s_UTCMask;

    NANOCLR_NOCLEANUP();
}

//--//

CLR_INT64* Library_corlib_native_System_DateTime::GetValuePtr( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    return GetValuePtr( stack.Arg0() );
}

CLR_INT64* Library_corlib_native_System_DateTime::GetValuePtr( CLR_RT_HeapBlock& ref )
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_RT_HeapBlock* obj = &ref;
    CLR_DataType      dt  = obj->DataType();

    if(dt == DATATYPE_OBJECT || dt == DATATYPE_BYREF)
    {
        obj = obj->Dereference(); if(!obj) return NULL;
        dt = obj->DataType();
    }

    // after dereferencing the object if it's pointing to another Object 
    // need to do it again because this DateTime instance is most likely boxed
    if(dt == DATATYPE_OBJECT)
    {
        obj = obj->Dereference(); if(!obj) return NULL;
        dt = obj->DataType();
    }

    if(dt == DATATYPE_DATETIME)
    {
        return (CLR_INT64*)&obj->NumericByRef().s8;
    }

    if(dt == DATATYPE_I8)
    {
        return (CLR_INT64*)&obj->NumericByRef().s8;
    }

    if(dt == DATATYPE_VALUETYPE && obj->ObjectCls().m_data == g_CLR_RT_WellKnownTypes.m_DateTime.m_data)
    {
        return (CLR_INT64*)&obj[ FIELD___ticks ].NumericByRef().s8;
    }

    return NULL;
}

bool Library_corlib_native_System_DateTime::Expand( CLR_RT_StackFrame& stack, SYSTEMTIME& st  )
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_INT64* val = GetValuePtr( stack );

    if(val)
    {
        CLR_INT64 ticks = *val & s_TickMask;
        HAL_Time_ToSystemTime( ticks, &st );

        return true;
    }

    return false;
}

// Compress function always creates UTC time.
void Library_corlib_native_System_DateTime::Compress( CLR_RT_StackFrame& stack, const SYSTEMTIME& st )
{
    NATIVE_PROFILE_CLR_CORE();
    CLR_INT64* val = GetValuePtr( stack );

    if(val) 
    {
        *val = HAL_Time_ConvertFromSystemTime( &st ) | s_UTCMask;
    }
}
