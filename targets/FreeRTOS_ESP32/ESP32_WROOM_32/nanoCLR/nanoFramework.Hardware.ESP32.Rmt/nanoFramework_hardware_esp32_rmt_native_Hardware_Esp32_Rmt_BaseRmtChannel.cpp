//
// Copyright (c) 2019 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "nanoFramework_hardware_esp32_rmt_native.h"

#include <map>
#include <vector>

#define MAX_RMT_CHANNELS = 8

rmt_channel_t channels[MAX_RMT_CHANNELS] = {};

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_BaseRmtChannel::NativeDispose___STATIC__VOID__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock* pThis = stack.This(); FAULT_ON_NULL(pThis);
        int32_t channel = (int32_t)pThis[FIELD___channel].NumericByRefConst().s4;

        if (channel < 0)
        {
            hr = CLR_E_INDEX_OUT_OF_RANGE;
            NANOCLR_LEAVE();
        }        

        rmt_channel_t thisChan = channels[channel];
        if (thisChan == null)
        {
            hr = CLR_E_OBJECT_DISPOSED;
            NANOCLR_LEAVE();
        }

        if (rmt_driver_uninstall(thisChan) != ESP_OK)
        {
            hr = CLR_E_PROCESS_EXCEPTION;
        }

        for (int i = 0; i < MAX_RMT_CHANNELS; i++)
        {
            if (channels[i] == thisChan)
            {
                channels[i] = 0;
                break;
            }
        }        
    }    

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_BaseRmtChannel::NativeGetChannelStatus___STATIC__I4__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        
    }

    
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_BaseRmtChannel::NativeGetClockDivider___STATIC__U1__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_BaseRmtChannel::NativeGetMemoryBlockNumber___STATIC__U1__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_BaseRmtChannel::NativeGetMemoryLowPower___STATIC__BOOLEAN__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_BaseRmtChannel::NativeGetSourceClock___STATIC__I4__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_BaseRmtChannel::NativeSetClockDivider___STATIC__VOID__I4__U1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_BaseRmtChannel::NativeSetMemoryBlockNumber___STATIC__VOID__I4__U1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_BaseRmtChannel::NativeSetMemoryLowPower___STATIC__VOID__I4__BOOLEAN( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_BaseRmtChannel::NativeSetSourceClock___STATIC__VOID__I4__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}
