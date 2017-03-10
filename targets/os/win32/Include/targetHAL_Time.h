//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#ifndef _TARGET_HAL_TIME_H_
#define _TARGET_HAL_TIME_H_ 1


UINT32 HAL_Time_CurrentSysTicks();






///// <summary>
///// Converts 64bit time value to SystemTime structure. 64bit time is assumed as an offset from 1/1/1601:00:00:00.000 in 100ns.
///// </summary>
///// <returns>True if conversion is successful.</returns>
//BOOL       Time_ToSystemTime(TIME time, SYSTEMTIME* systemTime);
//
///// <summary>
///// Converts SystemTime structure to 64bit time, which is assumed as an offset from 1/1/1601:00:00:00.000 in 100ns.
///// </summary>
///// <returns>Time value.</returns>
//TIME       Time_FromSystemTime(const SYSTEMTIME* systemTime);
//
///// <summary>
///// Retrieves time since device was booted. 
///// </summary>
///// <returns>Time in 100ns.</returns>
//INT64 HAL_Time_GetMachineTime();
//
///// <summary>
///// Offset from GMT.
///// </summary>
///// <returns>In minutes, for example Pacific Time would be GMT-8 = -480.</returns>
//INT32 Time_GetTimeZoneOffset();
//
///// <summary>
///// UTC time according to this system. 
///// </summary>
///// <returns>Returns current UTC time in 100ns elapsed since 1/1/1601:00:00:00.000 UTC.</returns>
//INT64       Time_GetUtcTime();
//
///// <summary>
///// Local time according to the Time subsystem.
///// </summary>
///// <returns>Local time in 100ns elapsed since 1/1/1601:00:00:00.000 local time.</returns>
//INT64       Time_GetLocalTime();
//
///// <summary>
///// Retrieves number of days since the beginning of the year given a month and a year. Calculates for leap years.
///// </summary>
///// <returns>S_OK if successful.</returns>
//HRESULT    Time_AccDaysInMonth(INT32 year, INT32 month, INT32* days);
//
///// <summary>
///// Retrieves number of days given a month and a year. Calculates for leap years.
///// </summary>
///// <returns>S_OK if successful.</returns>
//HRESULT    Time_DaysInMonth(INT32 year, INT32 month, INT32* days);
//
///// APIs to convert between types
//BOOL       Time_TimeSpanToStringEx( const INT64& ticks, LPSTR& buf, size_t& len );
//LPCSTR     Time_TimeSpanToString( const INT64& ticks );
//BOOL       Time_DateTimeToStringEx( const INT64& time, LPSTR& buf, size_t& len );
//LPCSTR     Time_DateTimeToString( const INT64& time);
//LPCSTR     Time_CurrentDateTimeToString();


#endif //_TARGET_HAL_TIME_H_
