//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "stdafx.h"

HRESULT Time_Initialize()
{
    return CLR_E_NOTIMPL;
}

HRESULT Time_Uninitialize()
{
    return CLR_E_NOTIMPL;
}

// INT64 Time_GetUtcTime()
// {
//     return 0;
// }

INT64 Time_SetUtcTime( INT64 UtcTime, bool calibrate )
{
    return 0;
}

INT64 Time_GetLocalTime()
{
    return 0;
}

INT32 Time_GetTimeZoneOffset()
{
    return 0;
}

INT32 Time_SetTimeZoneOffset(INT32 offset)
{
    return 0;
}

INT64 Time_GetTickCount()
{
    return 0;
}

BOOL HAL_Time_ToSystemTime(INT64 time, SYSTEMTIME* systemTime)
{
    return FALSE;
}

INT64 HAL_Time_FromSystemTime(const SYSTEMTIME* systemTime)
{
    return 0;
}

HRESULT HAL_Time_DaysInMonth(INT32 year, INT32 month, INT32* days)
{
    return CLR_E_NOTIMPL;
}

HRESULT HAL_Time_AccDaysInMonth(INT32 year, INT32 month, INT32* days)
{
    return CLR_E_NOTIMPL;
}

BOOL Utility_SafeSprintfV( LPSTR& szBuffer, size_t& iBuffer, LPCSTR format, va_list arg )
{
    return FALSE;
}

BOOL Utility_SafeSprintf( LPSTR& szBuffer, size_t& iBuffer, LPCSTR format, ... )
{
    return FALSE;
}

BOOL HAL_Time_TimeSpanToStringEx( const INT64& ticks, LPSTR& buf, size_t& len )
{
    return FALSE;
}

LPCSTR HAL_Time_TimeSpanToString( const INT64& ticks )
{
    return NULL;
}

BOOL Time_DateTimeToStringEx( const INT64& time, LPSTR& buf, size_t& len )
{   
    return FALSE;
}

LPCSTR Time_DateTimeToString( const INT64& time)
{
    return NULL;
}

LPCSTR HAL_Time_CurrentDateTimeToString()
{
    return NULL;
}
