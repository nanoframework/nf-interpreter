//
// Copyright (c) 2018 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "nanoFramework_hardware_esp32_native.h"

#define MAX_HRTIMERS 10

esp_timer_handle_t hrtimers[MAX_HRTIMERS] = {};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH nanoFramework.Hardware.Esp32.HighResTimerEventType (in managed code) !!! //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum HighResTimerEventType
{
    TimerExpired = 101
};

static int FindNextTimerIndex()
{
    for( int index=0; index<MAX_HRTIMERS; index++)
    {
        if ( hrtimers[index] == 0 ) return index;
    }

    return -1;
}

static void HRtimer_callback(void* arg)
{
    esp_timer_handle_t timer_handle = hrtimers[(int) arg];
    PostManagedEvent( EVENT_CUSTOM, HighResTimerEventType::TimerExpired, 0, (uint32_t)timer_handle );
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_HighResTimer::NativeEspTimerCreate___I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        int index = FindNextTimerIndex();
        if ( index < 0) NANOCLR_SET_AND_LEAVE(CLR_E_TOO_MANY_OPEN_HANDLES);

        esp_timer_handle_t out_handle;
        esp_timer_create_args_t create_args = {};
        create_args.callback = &HRtimer_callback;
        create_args.arg = (void*)index;
        
        esp_err_t err = esp_timer_create(&create_args, &out_handle);
        if ( err != ESP_OK)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
        hrtimers[index] = out_handle;
        stack.SetResult_I4((int32_t)out_handle);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_HighResTimer::NativeEspTimerDispose___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis); 
        esp_timer_handle_t timer = (esp_timer_handle_t)pThis[ FIELD___timerHandle ].NumericByRefConst().s4;
        esp_timer_delete(timer);

        for( int index=0; index<MAX_HRTIMERS; index++)
        {
            if ( hrtimers[index] == timer ) 
            {
                hrtimers[index] = 0;
                break;
            }
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_HighResTimer::NativeStop___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis); 
        if(pThis[FIELD___disposedValue ].NumericByRef().u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        esp_timer_handle_t timer = (esp_timer_handle_t)pThis[ FIELD___timerHandle ].NumericByRefConst().s4;
        esp_timer_stop(timer);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_HighResTimer::NativeStartOneShot___VOID__U8( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis); 

        if(pThis[FIELD___disposedValue ].NumericByRef().u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        esp_timer_handle_t timer = (esp_timer_handle_t)pThis[ FIELD___timerHandle ].NumericByRefConst().s4;
        uint64_t timeout_us = (uint64_t)stack.Arg1().NumericByRef().u8;
        
        esp_timer_stop(timer);
        esp_timer_start_once(timer, timeout_us);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_HighResTimer::NativeStartPeriodic___VOID__U8( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis); 

        if(pThis[FIELD___disposedValue ].NumericByRef().u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        esp_timer_handle_t timer = (esp_timer_handle_t)pThis[ FIELD___timerHandle ].NumericByRefConst().s4;
        uint64_t period_us = (uint64_t)stack.Arg1().NumericByRef().u8;
 
        esp_timer_stop(timer);
        esp_timer_start_periodic(timer, period_us);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_HighResTimer::NativeGetCurrent___STATIC__U8( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        uint64_t timer_val = esp_timer_get_time();
        stack.SetResult_U8(timer_val);
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}
