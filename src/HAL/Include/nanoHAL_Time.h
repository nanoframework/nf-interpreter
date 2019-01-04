//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _NANOHAL_TIME_H_
#define _NANOHAL_TIME_H_ 1

#include <nanoHAL_Types.h>
#include <targetHAL_Time.h>
#include <nanoWeak.h>

/// Our time origin is 1/1/1601 00:00:00.000.000.  In Gregorian Calendar Jan 1, 1601 was also a Monday.
#define BASE_YEAR                   1601
#define BASE_YEAR_LEAPYEAR_ADJUST   388    
#define DAYS_IN_NORMAL_YEAR         365
#define BASE_YEAR_DAYOFWEEK_SHIFT   1 

#define TIME_CONVERSION__TO_MILLISECONDS    10000
#define TIME_CONVERSION__TO_SECONDS         10000000
#define TIME_CONVERSION__TICKUNITS         10000
#define TIME_CONVERSION__ONESECOND         1
#define TIME_CONVERSION__ONEMINUTE         60
#define TIME_CONVERSION__ONEHOUR           3600
#define TIME_CONVERSION__ONEDAY            86400

#define TIMEOUT_ZERO      LONGLONGCONSTANT(0x0000000000000000)
#define TIMEOUT_INFINITE  LONGLONGCONSTANT(0x7FFFFFFFFFFFFFFF)

// UNDONE: FIXME #define TIME_ZONE_OFFSET    ((signed long long)Time_GetTimeZoneOffset() * 600000000)

#define NANOHAL_TIME_CONVERSION_MICRO_TO_SECONDS                1000000

#define TIME_UNIX_EPOCH_AS_TICKS           116444736000000000

/// NOTES: Why origin is at 1/1/1601.
/// Current civil calendar is named as Gregorian calendar after Pope Gregory XIII as he made adjustments
/// in 1582 (read more at wiki http://en.wikipedia.org/wiki/Gregorian_calendar). Rules governing
/// leap years were changed from then. Also in that year month October was 21 days instead of usual 31.
/// This poses a problem on calculating date/time difference, leap years etc before 1582 using simple math.
/// For example 1500 was a leap year using old method while it is not using new. But in reality, as part of the
/// history it was leap year. Default CLR origin 1/1/01 gives wrong date time from years before 1582. For example
/// dates like 10/6/1582 does exist in history (see wiki), while CLR managed date/time will not throw an exception
/// if you are to create that date. To stay safe side 1/1/1601 is taken as origin, as was done for Windows.


uint64_t HAL_Time_SysTicksToTime(unsigned int sysTicks);

/// <summary>
/// System time and date for DateTime managed class.
/// This value will be provided by the system tick or from an RTC if hardware support exists and if the board designer has enabled it in the configuration options.
/// The datePartOnly allows returning only the date part with the time fields zeroed.
/// </summary>
/// <returns>Returns current time in 100ns elapsed since 1/1/1601:00:00:00.000 UTC.</returns>
uint64_t  HAL_Time_CurrentDateTime(bool datePartOnly);

/// <summary>
/// Time according to this system. 
/// </summary>
/// <returns>Returns current time in 100ns elapsed since 1/1/1601:00:00:00.000 UTC.</returns>
uint64_t  HAL_Time_CurrentTime();

/// <summary>
/// Set UTC time of the system. This will be effective immediately.
/// </summary>
/// <param name="utcTime">In 100ns since 1/1/1601:00:00:00.000</param>
void      HAL_Time_SetUtcTime(uint64_t utcTime);

/// <summary>
/// Retrieves time since device was booted.
/// </summary>
/// <returns>Time in 100ns.</returns>
//signed long long HAL_Time_GetMachineTime();

/// <summary>
/// Converts 64bit time value to SystemTime structure. 64bit time is assumed as an offset from 1/1/1601:00:00:00.000 in 100ns.
/// </summary>
/// <returns>True if conversion is successful.</returns>
bool HAL_Time_ToSystemTime(uint64_t time, SYSTEMTIME* systemTime);

/// <summary>
/// Retrieves number of days given a month and a year. Calculates for leap years.
/// </summary>
/// <returns>S_OK if successful.</returns>
HRESULT HAL_Time_DaysInMonth(signed int year, signed int month, signed int* days);

/// <summary>
/// Retrieves number of days since the beginning of the year given a month and a year. Calculates for leap years.
/// </summary>
/// <returns>S_OK if successful.</returns>
HRESULT HAL_Time_AccDaysInMonth(signed int year, signed int month, signed int* days);

/// <summary>
/// Converts SYSTEMTIME structure to 64bit time, which is assumed as an offset from 1/1/1601:00:00:00.000 in 100ns.
/// </summary>
/// <returns>Time value.</returns>
uint64_t HAL_Time_ConvertFromSystemTime(const SYSTEMTIME* systemTime);

/// APIs to convert between types
bool          HAL_Time_TimeSpanToStringEx( const int64_t& ticks, char*& buf, size_t& len );
const char*   HAL_Time_CurrentDateTimeToString();


uint64_t  CPU_MillisecondsToTicks       ( uint64_t ticks  );


uint64_t Time_GetMachineTime();

#endif //_NANOHAL_TIME_H_
