//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// RP2350 RTC subsystem low level driver, built on the POWMAN "Always-On"
// timer (RP2350 datasheet section 12.10).

#include "hal.h"

#if (HAL_USE_RTC == TRUE) || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

// POWMAN register block (RP2350 datasheet section 12.11/12.10).
#define POWMAN_BASE 0x40100000UL

#define POWMAN_LPOSC_FREQ_KHZ_INT  (*(volatile uint32_t *)(POWMAN_BASE + 0x50UL))
#define POWMAN_LPOSC_FREQ_KHZ_FRAC (*(volatile uint32_t *)(POWMAN_BASE + 0x54UL))
#define POWMAN_SET_TIME_63TO48     (*(volatile uint32_t *)(POWMAN_BASE + 0x60UL))
#define POWMAN_SET_TIME_47TO32     (*(volatile uint32_t *)(POWMAN_BASE + 0x64UL))
#define POWMAN_SET_TIME_31TO16     (*(volatile uint32_t *)(POWMAN_BASE + 0x68UL))
#define POWMAN_SET_TIME_15TO0      (*(volatile uint32_t *)(POWMAN_BASE + 0x6CUL))
#define POWMAN_READ_TIME_UPPER     (*(volatile uint32_t *)(POWMAN_BASE + 0x70UL))
#define POWMAN_READ_TIME_LOWER     (*(volatile uint32_t *)(POWMAN_BASE + 0x74UL))
#define POWMAN_ALARM_TIME_63TO48   (*(volatile uint32_t *)(POWMAN_BASE + 0x78UL))
#define POWMAN_ALARM_TIME_47TO32   (*(volatile uint32_t *)(POWMAN_BASE + 0x7CUL))
#define POWMAN_ALARM_TIME_31TO16   (*(volatile uint32_t *)(POWMAN_BASE + 0x80UL))
#define POWMAN_ALARM_TIME_15TO0    (*(volatile uint32_t *)(POWMAN_BASE + 0x84UL))
#define POWMAN_TIMER               (*(volatile uint32_t *)(POWMAN_BASE + 0x88UL))
#define POWMAN_INTE                (*(volatile uint32_t *)(POWMAN_BASE + 0xE4UL))

// Writes to most POWMAN registers are ignored unless the top 16 bits carry
// this password, to prevent accidental writes (RP2350 datasheet 12.11).
#define POWMAN_PASSWORD_BITS 0x5AFE0000UL

#define POWMAN_TIMER_ALARM_BITS      (1UL << 6)
#define POWMAN_TIMER_ALARM_ENAB_BITS (1UL << 4)
#define POWMAN_TIMER_RUN_BITS        (1UL << 1)
#define POWMAN_TIMER_USE_LPOSC_BITS  (1UL << 8)

#define POWMAN_INTE_TIMER_BITS (1UL << 1)

// Nominal LPOSC frequency, matching the RP2350 power-on-reset default for
// these registers (32.768kHz target for the internal low power oscillator).
#define POWMAN_LPOSC_NOMINAL_FREQ_KHZ_INT  32UL
#define POWMAN_LPOSC_NOMINAL_FREQ_KHZ_FRAC 0xC49CUL

#define MS_PER_DAY 86400000ULL

// days_from_civil/civil_from_days below are expressed in days since 1970-01-01
// The driver's own time base is ms since RTC_BASE_YEAR (1980-01-01, see hal_rtc.h)
#define RTC_BASE_YEAR_EPOCH_DAYS 3652 /* days_from_civil(1980, 1, 1) */

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/**
 * @brief RTC driver identifier.
 */
RTCDriver RTCD1;

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

static void powman_write(volatile uint32_t *reg, uint32_t value)
{
    *reg = POWMAN_PASSWORD_BITS | value;
}

static void powman_set_bits(volatile uint32_t *reg, uint32_t bits)
{
    *reg = POWMAN_PASSWORD_BITS | (*reg | bits);
}

static void powman_clear_bits(volatile uint32_t *reg, uint32_t bits)
{
    *reg = POWMAN_PASSWORD_BITS | (*reg & ~bits);
}

// Returns the current AON timer value in milliseconds since RTC_BASE_YEAR.
static uint64_t powman_now_ms(void)
{
    uint32_t upperWord = POWMAN_READ_TIME_UPPER;
    uint32_t lowerWord;
    for (;;)
    {
        lowerWord = POWMAN_READ_TIME_LOWER;
        uint32_t upperWordRecheck = POWMAN_READ_TIME_UPPER;
        if (upperWordRecheck == upperWord)
        {
            break;
        }
        upperWord = upperWordRecheck;
    }
    return ((uint64_t)upperWord << 32) | lowerWord;
}

// Howard Hinnant's civil_from_days / days_from_civil algorithms,
// used to convert between a calendar date and days since 1970-01-01
static int64_t days_from_civil(int64_t year, unsigned month, unsigned day)
{
    year -= (month <= 2) ? 1 : 0;

    int64_t era = (year >= 0 ? year : year - 399) / 400;
    unsigned yearOfEra = (unsigned)(year - era * 400);
    unsigned monthsSinceMarch = (month > 2) ? (month - 3U) : (month + 9U);
    unsigned dayOfYear = (153U * monthsSinceMarch + 2U) / 5U + day - 1U;
    unsigned dayOfEra = yearOfEra * 365U + yearOfEra / 4U - yearOfEra / 100U + dayOfYear;

    return era * 146097 + (int64_t)dayOfEra - 719468;
}

static void civil_from_days(int64_t daysSinceUnixEpoch, int *yearOut, unsigned *monthOut, unsigned *dayOut)
{
    int64_t z = daysSinceUnixEpoch + 719468;

    int64_t era = (z >= 0 ? z : z - 146096) / 146097;
    unsigned dayOfEra = (unsigned)(z - era * 146097);
    unsigned yearOfEra = (dayOfEra - dayOfEra / 1460U + dayOfEra / 36524U - dayOfEra / 146096U) / 365U;
    int64_t year = (int64_t)yearOfEra + era * 400;
    unsigned dayOfYear = dayOfEra - (365U * yearOfEra + yearOfEra / 4U - yearOfEra / 100U);
    unsigned monthsSinceMarch = (5U * dayOfYear + 2U) / 153U;

    *dayOut = dayOfYear - (153U * monthsSinceMarch + 2U) / 5U + 1U;
    *monthOut = (monthsSinceMarch < 10U) ? (monthsSinceMarch + 3U) : (monthsSinceMarch - 9U);
    // Undo the "year starts in March" shift from days_from_civil.
    *yearOut = (int)(year + (*monthOut <= 2U ? 1 : 0));
}

static uint64_t rtcdt_to_ms64(const RTCDateTime *dateTime)
{
    int64_t daysSinceBaseYear =
        days_from_civil((int64_t)dateTime->year + RTC_BASE_YEAR, dateTime->month, dateTime->day) -
        RTC_BASE_YEAR_EPOCH_DAYS;

    return (uint64_t)daysSinceBaseYear * MS_PER_DAY + (uint64_t)dateTime->millisecond;
}

static void ms64_to_rtcdt(uint64_t milliseconds, RTCDateTime *dateTime)
{
    // Rebased to days since 1970 here, matching what civil_from_days and the
    // weekday calculation below expect.
    int64_t daysSinceUnixEpoch = (int64_t)(milliseconds / MS_PER_DAY) + RTC_BASE_YEAR_EPOCH_DAYS;
    uint32_t millisecondOfDay = (uint32_t)(milliseconds % MS_PER_DAY);
    int year;
    unsigned month, day;

    civil_from_days(daysSinceUnixEpoch, &year, &month, &day);

    dateTime->year = (uint32_t)(year - RTC_BASE_YEAR);
    dateTime->month = month;
    dateTime->day = day;
    dateTime->millisecond = millisecondOfDay;
    dateTime->dstflag = 0;

    unsigned dayOfWeekFromSunday = (unsigned)(((daysSinceUnixEpoch % 7) + 7 + 4) % 7);
    dateTime->dayofweek = ((dayOfWeekFromSunday + 6U) % 7U) + 1U;
}

#if (RTC_ALARMS > 0) || defined(__DOXYGEN__)

// Finds the smallest second-of-day (0..86399) >= minSecondOfDay that
// satisfies the hour/minute/second alarm mask against wantedTime. Returns
// false if none exists in [minSecondOfDay, 86399].
static bool find_time_of_day(
    const RTCDateTime *wantedTime,
    rtcdtmask_t mask,
    uint32_t minSecondOfDay,
    uint32_t *secondOfDayOut)
{
    uint32_t wantedSecondOfDay = wantedTime->millisecond / 1000U;
    uint32_t wantedHour = wantedSecondOfDay / 3600U;
    uint32_t wantedMinute = (wantedSecondOfDay / 60U) % 60U;
    uint32_t wantedSecond = wantedSecondOfDay % 60U;

    bool hourFixed = RTC_ALARM_TEST_MATCH(mask, RTC_DT_ALARM_HOUR);
    bool minuteFixed = RTC_ALARM_TEST_MATCH(mask, RTC_DT_ALARM_MINUTE);
    bool secondFixed = RTC_ALARM_TEST_MATCH(mask, RTC_DT_ALARM_SECOND);

    if (!hourFixed && !minuteFixed && !secondFixed)
    {
        // Every second of the day matches
        if (minSecondOfDay > 86399U)
        {
            return false;
        }
        *secondOfDayOut = minSecondOfDay;
        return true;
    }

    uint32_t hourLow = hourFixed ? wantedHour : 0U, hourHigh = hourFixed ? wantedHour : 23U;
    uint32_t minuteLow = minuteFixed ? wantedMinute : 0U, minuteHigh = minuteFixed ? wantedMinute : 59U;
    uint32_t secondLow = secondFixed ? wantedSecond : 0U, secondHigh = secondFixed ? wantedSecond : 59U;

    for (uint32_t hour = hourLow; hour <= hourHigh; hour++)
    {
        for (uint32_t minute = minuteLow; minute <= minuteHigh; minute++)
        {
            for (uint32_t second = secondLow; second <= secondHigh; second++)
            {
                uint32_t candidateSecondOfDay = hour * 3600U + minute * 60U + second;
                if (candidateSecondOfDay >= minSecondOfDay)
                {
                    *secondOfDayOut = candidateSecondOfDay;
                    return true;
                }
            }
        }
    }
    return false;
}

static bool date_matches(const RTCDateTime *candidate, const RTCDateTime *wantedTime, rtcdtmask_t mask)
{
    if (RTC_ALARM_TEST_MATCH(mask, RTC_DT_ALARM_YEAR) && candidate->year != wantedTime->year)
    {
        return false;
    }
    if (RTC_ALARM_TEST_MATCH(mask, RTC_DT_ALARM_MONTH) && candidate->month != wantedTime->month)
    {
        return false;
    }
    if (RTC_ALARM_TEST_MATCH(mask, RTC_DT_ALARM_DAY) && candidate->day != wantedTime->day)
    {
        return false;
    }
    if (RTC_ALARM_TEST_MATCH(mask, RTC_DT_ALARM_DOTW) && candidate->dayofweek != wantedTime->dayofweek)
    {
        return false;
    }
    return true;
}

// Finds the next absolute AON time (ms since RTC_BASE_YEAR) strictly after
// fromMilliseconds that satisfies the alarm mask against wantedTime.
static uint64_t next_match_ms(const RTCDateTime *wantedTime, rtcdtmask_t mask, uint64_t fromMilliseconds)
{
    RTCDateTime candidate;
    uint32_t secondOfDay;

    uint64_t dayStartMilliseconds = (fromMilliseconds / MS_PER_DAY) * MS_PER_DAY;
    uint32_t currentSecondOfDay = (uint32_t)((fromMilliseconds - dayStartMilliseconds) / 1000U);

    ms64_to_rtcdt(dayStartMilliseconds, &candidate);
    if (date_matches(&candidate, wantedTime, mask) &&
        find_time_of_day(wantedTime, mask, currentSecondOfDay + 1U, &secondOfDay))
    {
        return dayStartMilliseconds + (uint64_t)secondOfDay * 1000U;
    }

    if (!find_time_of_day(wantedTime, mask, 0U, &secondOfDay))
    {
        return 0;
    }

    for (uint32_t dayOffset = 1U; dayOffset <= 366U; dayOffset++)
    {
        uint64_t candidateDayStartMilliseconds = dayStartMilliseconds + (uint64_t)dayOffset * MS_PER_DAY;
        ms64_to_rtcdt(candidateDayStartMilliseconds, &candidate);
        if (date_matches(&candidate, wantedTime, mask))
        {
            return candidateDayStartMilliseconds + (uint64_t)secondOfDay * 1000U;
        }
    }

    return 0;
}

static void rtc_disable_alarm(RTCDriver *rtcp)
{
    (void)rtcp;
    powman_clear_bits(&POWMAN_INTE, POWMAN_INTE_TIMER_BITS);
    powman_clear_bits(&POWMAN_TIMER, POWMAN_TIMER_ALARM_ENAB_BITS);
}

// Programs the alarm comparator to fire at alarmMilliseconds and enables it.
// The comparator must be disabled while its registers are written
static void rtc_arm_alarm(uint64_t alarmMilliseconds)
{
    powman_clear_bits(&POWMAN_TIMER, POWMAN_TIMER_ALARM_ENAB_BITS);
    powman_write(&POWMAN_ALARM_TIME_15TO0, (uint32_t)(alarmMilliseconds & 0xFFFFUL));
    powman_write(&POWMAN_ALARM_TIME_31TO16, (uint32_t)((alarmMilliseconds >> 16) & 0xFFFFUL));
    powman_write(&POWMAN_ALARM_TIME_47TO32, (uint32_t)((alarmMilliseconds >> 32) & 0xFFFFUL));
    powman_write(&POWMAN_ALARM_TIME_63TO48, (uint32_t)((alarmMilliseconds >> 48) & 0xFFFFUL));

    powman_clear_bits(&POWMAN_TIMER, POWMAN_TIMER_ALARM_BITS);
    powman_set_bits(&POWMAN_INTE, POWMAN_INTE_TIMER_BITS);
    powman_set_bits(&POWMAN_TIMER, POWMAN_TIMER_ALARM_ENAB_BITS);
}

#endif /* RTC_ALARMS > 0 */

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

#if (RTC_ALARMS > 0) || defined(__DOXYGEN__)
/**
 * @brief   POWMAN timer alarm interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(RP_POWMAN_IRQ_TIMER_HANDLER)
{
    OSAL_IRQ_PROLOGUE();

    // The alarm comparator is level-sensitive (alarm_time >= current_time),
    // so it must be disabled before the latched flag is cleared
    rtc_disable_alarm(&RTCD1);
    powman_clear_bits(&POWMAN_TIMER, POWMAN_TIMER_ALARM_BITS);

    if ((RTCD1.mask & RTC_ALARM_NON_REPEATING) != RTC_ALARM_NON_REPEATING)
    {
        uint64_t nextAlarmMilliseconds = next_match_ms(&RTCD1.alarm, RTCD1.mask, powman_now_ms());
        if (nextAlarmMilliseconds != 0)
        {
            rtc_arm_alarm(nextAlarmMilliseconds);
        }
    }

#if RTC_SUPPORTS_CALLBACKS == TRUE
    if (RTCD1.callback != NULL)
    {
        RTCD1.callback(&RTCD1, RTC_EVENT_ALARM);
    }
#endif

    OSAL_IRQ_EPILOGUE();
}
#endif

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Enable access to the POWMAN AON timer.
 *
 * @notapi
 */
void rtc_lld_init(void)
{
    // RTC object initialization.
    rtcObjectInit(&RTCD1);

    // Callback initially disabled.
    RTCD1.callback = NULL;

#if (RTC_ALARMS > 0)
    RTCD1.mask = RTC_ALARM_DISABLE_ALL_MATCHING;
#endif

    // POWMAN is in the Always-On power domain: it is not part of the RESETS
    // block and survives ordinary chip resets by design

    powman_write(&POWMAN_LPOSC_FREQ_KHZ_INT, POWMAN_LPOSC_NOMINAL_FREQ_KHZ_INT);
    powman_write(&POWMAN_LPOSC_FREQ_KHZ_FRAC, POWMAN_LPOSC_NOMINAL_FREQ_KHZ_FRAC);
    powman_set_bits(&POWMAN_TIMER, POWMAN_TIMER_USE_LPOSC_BITS);

    if ((POWMAN_TIMER & POWMAN_TIMER_RUN_BITS) == 0)
    {
        // Timer not running: either first power-up of the AON domain, or a
        // previous explicit stop.
        powman_set_bits(&POWMAN_TIMER, POWMAN_TIMER_RUN_BITS);
    }

    rtc_disable_alarm(&RTCD1);
    powman_clear_bits(&POWMAN_TIMER, POWMAN_TIMER_ALARM_BITS);

    nvicEnableVector(RP_POWMAN_IRQ_TIMER_NUMBER, RP_IRQ_RTC_PRIORITY);
}

/**
 * @brief   Set current time.
 * @note    Fractional seconds part will be silently ignored. There is no
 *          possibility to set it on RP2350 platform.
 * @note    The function can be called from any context.
 *
 * @param[in] rtcp      pointer to RTC driver structure
 * @param[in] timespec  pointer to a @p RTCDateTime structure
 *
 * @notapi
 */
void rtc_lld_set_time(RTCDriver *rtcp, const RTCDateTime *timespec)
{
    (void)rtcp;
    uint64_t milliseconds = rtcdt_to_ms64(timespec);

    /* Entering a reentrant critical zone.*/
    syssts_t sts = osalSysGetStatusAndLockX();

    // SET_TIME_* may only be written while the timer is stopped
    powman_clear_bits(&POWMAN_TIMER, POWMAN_TIMER_RUN_BITS);
    powman_write(&POWMAN_SET_TIME_15TO0, (uint32_t)(milliseconds & 0xFFFFUL));
    powman_write(&POWMAN_SET_TIME_31TO16, (uint32_t)((milliseconds >> 16) & 0xFFFFUL));
    powman_write(&POWMAN_SET_TIME_47TO32, (uint32_t)((milliseconds >> 32) & 0xFFFFUL));
    powman_write(&POWMAN_SET_TIME_63TO48, (uint32_t)((milliseconds >> 48) & 0xFFFFUL));
    powman_set_bits(&POWMAN_TIMER, POWMAN_TIMER_RUN_BITS);

    /* Leaving a reentrant critical zone.*/
    osalSysRestoreStatusX(sts);
}

/**
 * @brief   Get current time.
 * @note    The function can be called from any context.
 *
 * @param[in]  rtcp      pointer to RTC driver structure
 * @param[out] timespec pointer to a @p RTCDateTime structure
 *
 * @notapi
 */
void rtc_lld_get_time(RTCDriver *rtcp, RTCDateTime *timespec)
{
    (void)rtcp;
    ms64_to_rtcdt(powman_now_ms(), timespec);
}

#if (RTC_ALARMS > 0) || defined(__DOXYGEN__)
/**
 * @brief   Set alarm time.
 * @note    The alarm time can be partially specified by leaving fields as
 *          zero and excluding them from the mask.
 * @note    The function can be called from any context.
 *
 * @param[in] rtcp      pointer to RTC driver structure.
 * @param[in] alarm     alarm identifier. Can be 0.
 * @param[in] alarmspec pointer to a @p RTCAlarm structure.
 *
 * @notapi
 */
void rtc_lld_set_alarm(RTCDriver *rtcp, rtcalarm_t alarm, const RTCAlarm *alarmspec)
{
    (void)alarm;
    const rtcdtmask_t dtmask = alarmspec->mask;

    if (dtmask == RTC_ALARM_DISABLE_ALL_MATCHING)
    {
        syssts_t sts = osalSysGetStatusAndLockX();
        rtc_disable_alarm(rtcp);
        rtcp->mask = RTC_ALARM_DISABLE_ALL_MATCHING;
        osalSysRestoreStatusX(sts);
        return;
    }

    uint64_t nextAlarmMilliseconds = next_match_ms(&alarmspec->alarm, dtmask, powman_now_ms());

    syssts_t sts = osalSysGetStatusAndLockX();

    if (nextAlarmMilliseconds != 0)
    {
        rtc_arm_alarm(nextAlarmMilliseconds);
    }
    else
    {
        rtc_disable_alarm(rtcp);
    }

    rtcp->alarm = alarmspec->alarm;
    rtcp->mask = dtmask;

    osalSysRestoreStatusX(sts);
}

/**
 * @brief   Get alarm time.
 * @note    The function can be called from any context.
 *
 * @param[in]  rtcp       pointer to RTC driver structure
 * @param[in]  alarm      alarm identifier
 * @param[in]  alarmspec pointer to a @p RTCAlarm structure
 *
 * @notapi
 */
void rtc_lld_get_alarm(RTCDriver *rtcp, rtcalarm_t alarm, RTCAlarm *alarmspec)
{
    (void)alarm;
    alarmspec->alarm = rtcp->alarm;
    alarmspec->mask = rtcp->mask;
}
#endif /* RTC_ALARMS > 0 */

#if RTC_SUPPORTS_CALLBACKS == TRUE
/**
 * @brief   Enables or disables RTC callbacks.
 * @details This function enables or disables callbacks. Use a @p NULL pointer
 *          in order to disable a callback.
 * @note    The function can be called from any context.
 *
 * @param[in] rtcp      pointer to RTC driver structure
 * @param[in] callback  callback function pointer or @p NULL
 *
 * @notapi
 */
void rtc_lld_set_callback(RTCDriver *rtcp, rtccb_t callback)
{
    rtcp->callback = callback;
}
#endif /* RTC_SUPPORTS_CALLBACKS == TRUE */

#endif /* HAL_USE_RTC */
