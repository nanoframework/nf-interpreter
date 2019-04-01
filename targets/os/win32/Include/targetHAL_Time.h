//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#ifndef _TARGET_HAL_TIME_H_
#define _TARGET_HAL_TIME_H_ 1


#define HAL_Time_CurrentSysTicks    HAL_Time_CurrentSysTicks
unsigned int HAL_Time_CurrentSysTicks();






///// <summary>
///// Converts 64bit time value to SystemTime structure. 64bit time is assumed as an offset from 1/1/1601:00:00:00.000 in 100ns.
///// </summary>
///// <returns>True if conversion is successful.</returns>
//bool       Time_ToSystemTime(TIME time, SYSTEMTIME* systemTime);
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
//signed __int64 HAL_Time_GetMachineTime();
//
///// <summary>
///// Offset from GMT.
///// </summary>
///// <returns>In minutes, for example Pacific Time would be GMT-8 = -480.</returns>
//signed int Time_GetTimeZoneOffset();
//
///// <summary>
///// UTC time according to this system. 
///// </summary>
///// <returns>Returns current UTC time in 100ns elapsed since 1/1/1601:00:00:00.000 UTC.</returns>
//signed __int64       Time_GetUtcTime();
//
///// <summary>
///// Local time according to the Time subsystem.
///// </summary>
///// <returns>Local time in 100ns elapsed since 1/1/1601:00:00:00.000 local time.</returns>
//signed __int64       Time_GetLocalTime();
//
///// <summary>
///// Retrieves number of days since the beginning of the year given a month and a year. Calculates for leap years.
///// </summary>
///// <returns>S_OK if successful.</returns>
//HRESULT    Time_AccDaysInMonth(signed int year, signed int month, signed int* days);
//
///// <summary>
///// Retrieves number of days given a month and a year. Calculates for leap years.
///// </summary>
///// <returns>S_OK if successful.</returns>
//HRESULT    Time_DaysInMonth(signed int year, signed int month, signed int* days);
//
///// APIs to convert between types
//bool       Time_TimeSpanToStringEx( const signed __int64& ticks, char*& buf, size_t& len );
//const char*     Time_TimeSpanToString( const signed __int64& ticks );
//bool       Time_DateTimeToStringEx( const signed __int64& time, char*& buf, size_t& len );
//const char*     Time_DateTimeToString( const signed __int64& time);
//const char*     Time_CurrentDateTimeToString();


#endif //_TARGET_HAL_TIME_H_
