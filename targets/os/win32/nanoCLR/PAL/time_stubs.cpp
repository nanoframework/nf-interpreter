//
// Copyright (c) .NET Foundation and Contributors
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

// signed __int64 Time_GetUtcTime()
// {
//     return 0;
// }

signed __int64 Time_SetUtcTime( signed __int64 UtcTime, bool calibrate )
{
    return 0;
}

signed __int64 Time_GetLocalTime()
{
    return 0;
}

signed int Time_GetTimeZoneOffset()
{
    return 0;
}

signed int Time_SetTimeZoneOffset(signed int offset)
{
    return 0;
}

signed __int64 Time_GetTickCount()
{
    return 0;
}

bool HAL_Time_ToSystemTime(unsigned long long time, SYSTEMTIME* systemTime)
{
    return FALSE;
}

uint64_t HAL_Time_ConvertFromSystemTime(const SYSTEMTIME* systemTime)
{
    return 0;
}

HRESULT HAL_Time_DaysInMonth(signed int year, signed int month, signed int* days)
{
    return CLR_E_NOTIMPL;
}

HRESULT HAL_Time_AccDaysInMonth(signed int year, signed int month, signed int* days)
{
    return CLR_E_NOTIMPL;
}

bool Utility_SafeSprintfV( char*& szBuffer, size_t& iBuffer, const char* format, va_list arg )
{
    return FALSE;
}

bool Utility_SafeSprintf( char*& szBuffer, size_t& iBuffer, const char* format, ... )
{
    return FALSE;
}

bool HAL_Time_TimeSpanToStringEx( const signed __int64& ticks, char*& buf, size_t& len )
{
    return FALSE;
}

const char* HAL_Time_TimeSpanToString( const signed __int64& ticks )
{
    return NULL;
}

bool Time_DateTimeToStringEx( const signed __int64& time, char*& buf, size_t& len )
{   
    return FALSE;
}

const char* Time_DateTimeToString( const signed __int64& time)
{
    return NULL;
}

const char* HAL_Time_CurrentDateTimeToString()
{
    return NULL;
}
