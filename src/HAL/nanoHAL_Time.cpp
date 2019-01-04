//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_Time.h>

const int CummulativeDaysForMonth[13] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
const int DaysInMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

#define IS_LEAP_YEAR(y)             (((y % 4 == 0) && (y % 100 != 0)) || (y % 400 == 0))
#define NUMBER_OF_LEAP_YEARS(y)     ((((y - 1) / 4) - ((y - 1) / 100) + ((y - 1) / 400)) - BASE_YEAR_LEAPYEAR_ADJUST) /// Number of leap years until base year, not including the target year itself.
#define NUMBER_OF_YEARS(y)          (y - BASE_YEAR)

#define YEARS_TO_DAYS(y)            ((NUMBER_OF_YEARS(y) * DAYS_IN_NORMAL_YEAR) + NUMBER_OF_LEAP_YEARS(y))
#define MONTH_TO_DAYS(y, m)         (CummulativeDaysForMonth[m - 1] + ((IS_LEAP_YEAR(y) && (m > 2)) ? 1 : 0))
#define DAYS_IN_MONTH(y, m)         (DaysInMonth[m - 1] + ((IS_LEAP_YEAR(y) && (m == 2)) ? 1 : 0))

#define TIMEUNIT_TO_MINUTES         600000000
#define TIMEUNIT_TO_MILLISECONDS    10000
#define MILLISECONDS_TO_SECONDS     1000
#define SECONDS_TO_MINUTES          60
#define MINUTES_TO_HOUR             60
#define HOURS_TO_DAY                24

uint64_t  HAL_Time_CurrentTime()
{ 
    return HAL_Time_SysTicksToTime( HAL_Time_CurrentSysTicks() ); 
};

/// <summary>
/// Converts a SYSTEMTIME value to HAL time value
/// </summary>
uint64_t HAL_Time_ConvertFromSystemTime(const SYSTEMTIME* systemTime)
{
    uint64_t r = YEARS_TO_DAYS(systemTime->wYear) + MONTH_TO_DAYS(systemTime->wYear, systemTime->wMonth) + systemTime->wDay - 1;
    r = (((( (r * HOURS_TO_DAY) + systemTime->wHour) * MINUTES_TO_HOUR + systemTime->wMinute) * SECONDS_TO_MINUTES + systemTime->wSecond ) * MILLISECONDS_TO_SECONDS + systemTime->wMilliseconds) * TIMEUNIT_TO_MILLISECONDS;

    return r;    
}

bool HAL_Time_ToSystemTime(uint64_t time, SYSTEMTIME* systemTime)
{
    uint32_t ytd = 0;
    uint32_t mtd = 0;
    
    time /= TIMEUNIT_TO_MILLISECONDS;
    systemTime->wMilliseconds = time % MILLISECONDS_TO_SECONDS;
    time /= MILLISECONDS_TO_SECONDS;
    systemTime->wSecond = time % SECONDS_TO_MINUTES;
    time /= SECONDS_TO_MINUTES;
    systemTime->wMinute = time % MINUTES_TO_HOUR;
    time /= MINUTES_TO_HOUR;
    systemTime->wHour = time % HOURS_TO_DAY;
    time /= HOURS_TO_DAY;

    systemTime->wDayOfWeek = (time + BASE_YEAR_DAYOFWEEK_SHIFT) % 7;
    systemTime->wYear = (unsigned short)(time / DAYS_IN_NORMAL_YEAR + BASE_YEAR);
    ytd = YEARS_TO_DAYS(systemTime->wYear);
    if (ytd > time)
    {
        systemTime->wYear--;
        ytd = YEARS_TO_DAYS(systemTime->wYear);
    }

    time -= ytd;

    systemTime->wMonth = (unsigned short)(time / 31 + 1);
    mtd = MONTH_TO_DAYS(systemTime->wYear, systemTime->wMonth + 1);

    if (time >= mtd)
    {
        systemTime->wMonth++;        
    }

    mtd = MONTH_TO_DAYS(systemTime->wYear, systemTime->wMonth);

    systemTime->wDay = (unsigned short)(time - mtd + 1); 

    return true;    
}

HRESULT HAL_Time_AccDaysInMonth(signed int year, signed int month, signed int* days)
{
    *days = MONTH_TO_DAYS(year, month);

    return S_OK;
}

HRESULT HAL_Time_DaysInMonth(signed int year, signed int month, signed int* days)
{
    *days = DAYS_IN_MONTH(year, month);

    return S_OK;
}
uint64_t Time_GetMachineTime()
{
    return HAL_Time_SysTicksToTime(HAL_Time_CurrentSysTicks());
}