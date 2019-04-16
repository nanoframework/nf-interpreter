//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_hardware_stm32_native.h"

// copied from corlib_native_System_DateTime.cpp
static CLR_INT64 s_UTCMask   = ULONGLONGCONSTANT(0x8000000000000000);


// we are using always Alarm 0 (alarm A, if there are two alarms)
#define ALARM_ID    0

// for all these to work we need the RTC to be enabled 
#if !defined(HAL_USE_RTC) 
    #error "Need the RTC to be enabled. Please set CMake option NF_FEATURE_RTC to ON."
#endif


HRESULT Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_RTC::Native_RTC_SetAlarm___STATIC__VOID__U1__U1__U1__U1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        RTCAlarm alarmTime;

        uint32_t value = 0;
        uint32_t alarmRegister = 0;

      #if defined(STM32F7XX) || defined(STM32F3XX) || defined(STM32F0XX) || \
          defined(STM32F4XX) || defined(STM32L0XX) ||  defined(STM32L4XX) || \
          defined(STM32H7XX)
        
        // these series are using driver RTCv2
        // the alarm is set on a register with the various values BCD coded
        // to keep the configuration simple, it's assumed that 
        // it's only possible to set seconds, minutes, hours and date (day)

        // to set the alarm, it has to be stoped first (in case there is one already set)
        rtcSetAlarm(&RTCD1, ALARM_ID, NULL);

        // alarm date (day) tens and units
        value = stack.Arg0().NumericByRef().u1;
        alarmRegister |= ((value % 10) << RTC_ALRMAR_DU_Pos);
        value /= 10;
        alarmRegister |= ((value % 10) << RTC_ALRMAR_DT_Pos);

        // alarm hours tens and units
        value = stack.Arg1().NumericByRef().u1;
        alarmRegister |= ((value % 10) << RTC_ALRMAR_HU_Pos);
        value /= 10;
        alarmRegister |= ((value % 10) << RTC_ALRMAR_HT_Pos);

        // alarm minutes tens and units
        value = stack.Arg2().NumericByRef().u1;
        alarmRegister |= ((value % 10) << RTC_ALRMAR_MNU_Pos);
        value /= 10;
        alarmRegister |= ((value % 10) << RTC_ALRMAR_MNT_Pos);

        // alarm seconds tens and units
        value = stack.Arg3().NumericByRef().u1;
        alarmRegister |= ((value % 10) << RTC_ALRMAR_SU_Pos);
        value /= 10;
        alarmRegister |= ((value % 10) << RTC_ALRMAR_ST_Pos);

        alarmTime.alrmr = alarmRegister;

      #else
        #error "Setting an alarm for this series in not supported. Care to look into it and submit a PR?"
      #endif

      #if defined(STM32F0XX) || defined(STM32F1XX) || defined(STM32F2XX) || \
      defined(STM32F3XX) ||defined(STM32F4XX) || defined(STM32L0XX) || defined(STM32L1XX)
        // clear PWR wake up Flag
        PWR->CR |=  PWR_CSR_WUF;
      #endif

      #if defined(STM32F7XX) || defined(STM32H7XX) || defined(STM32L4XX)

        CLEAR_BIT(RTC->CR, RTC_CR_ALRAIE);
        CLEAR_BIT(RTC->ISR, RTC_ISR_ALRAF);
  
      #endif 

        rtcSetAlarm(&RTCD1, ALARM_ID, &alarmTime);

    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_hardware_stm32_native_nanoFramework_Hardware_Stm32_RTC::GetAlarm___STATIC__mscorlibSystemDateTime( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    RTCAlarm alarmValue;
    RTCDateTime currentTime;
    SYSTEMTIME alarmTime;
    uint32_t value;

    rtcGetAlarm(&RTCD1, ALARM_ID, &alarmValue);

    #if defined(STM32F7XX) || defined(STM32F3XX) || defined(STM32F0XX) || \
        defined(STM32F4XX) || defined(STM32L0XX) ||  defined(STM32L4XX) || \
        defined(STM32H7XX)

    // these series are using driver RTCv2
    // the alarm is set on a register with the various values BCD coded
    // to keep the configuration simple, it's assumed that 
    // it's only possible to set seconds, minutes, hours and date (day)

    // convert from BCD assuming the alarm is always set for a future date/time
    
    // get current time from RTC
    rtcGetTime(&RTCD1, &currentTime);

    // fill in alarmTime struct
    alarmTime.wYear = (unsigned short) (currentTime.year + 1980);   // ChibiOS is counting years since 1980
    alarmTime.wMonth = (unsigned short) currentTime.month;

    // date (day) tens and units
    value = alarmValue.alrmr & RTC_ALRMAR_DT_Msk;
    value = value >> RTC_ALRMAR_DT_Pos;
    alarmTime.wDay =  value * 10;

    value = alarmValue.alrmr & RTC_ALRMAR_DU_Msk;
    value = value >> RTC_ALRMAR_DU_Pos;
    alarmTime.wDay += value;

    // alarm hours tens and units
    value = alarmValue.alrmr & RTC_ALRMAR_HT_Msk;
    value = value >> RTC_ALRMAR_HT_Pos;
    alarmTime.wHour = value * 10;

    value = alarmValue.alrmr & RTC_ALRMAR_HU_Msk;
    value = value >> RTC_ALRMAR_HU_Pos;
    alarmTime.wHour += value;

    // alarm minutes tens and units
    value = alarmValue.alrmr & RTC_ALRMAR_MNT_Msk;
    value = value >> RTC_ALRMAR_MNT_Pos;
    alarmTime.wMinute = value * 10;

    value = alarmValue.alrmr & RTC_ALRMAR_MNU_Msk;
    value = value >> RTC_ALRMAR_MNU_Pos;
    alarmTime.wMinute += value;

    // alarm seconds tens and units
    value = alarmValue.alrmr & RTC_ALRMAR_ST_Msk;
    value = value >> RTC_ALRMAR_ST_Pos;
    alarmTime.wSecond = value * 10;

    value = alarmValue.alrmr & RTC_ALRMAR_SU_Msk;
    value = value >> RTC_ALRMAR_SU_Pos;
    alarmTime.wSecond += value;

    // clear unused fields
    alarmTime.wMilliseconds = 0;
    alarmTime.wDayOfWeek = 0;

    // if alarm day is lower than the current day, means that the alarm was set for a date on the next month
    if(alarmTime.wDay < currentTime.day)
    {
        alarmTime.wMonth++;

        // check for December + 1
        if(alarmTime.wMonth > 12)
        {
            // adjust to January
            alarmTime.wMonth = 1;
        }
    }

    #else
    #error "Setting an alarm for this series in not supported. Care to look into it and submit a PR?"
    #endif

    CLR_RT_HeapBlock& ref = stack.PushValue();

    ref.SetDataId( CLR_RT_HEAPBLOCK_RAW_ID(DATATYPE_DATETIME, 0, 1) );
    ref.ClearData();

    CLR_INT64* pRes = (CLR_INT64*)&ref.NumericByRef().s8;
    *pRes = HAL_Time_ConvertFromSystemTime( &alarmTime ) | s_UTCMask;

    NANOCLR_NOCLEANUP_NOLABEL();
}
