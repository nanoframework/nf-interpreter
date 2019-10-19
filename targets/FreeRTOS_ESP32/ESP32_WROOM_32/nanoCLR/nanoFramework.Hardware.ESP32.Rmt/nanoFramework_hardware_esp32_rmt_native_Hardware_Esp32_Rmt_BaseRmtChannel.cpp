//
// Copyright (c) 2019 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "nanoFramework_hardware_esp32_rmt_native.h"

#define MAX_RMT_CHANNELS = 8

rmt_channel_t channels[MAX_RMT_CHANNELS] = {};

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_BaseRmtChannel::NativeDispose___STATIC__VOID__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
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

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_BaseRmtChannel::NativeGetChannelStatus___STATIC__I4__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        int_32_t channel = (int32_t)pThis[FIELD___channel].NumericByRefConst().s4;

        if ((channel < 0) || (channel >= MAX_RMT_CHANNELS))
        {
            hr = CLR_E_INDEX_OUT_OF_RANGE;
            NANOCLR_LEAVE();
        }

        uint32_t *pStatus;

        if (rmt_get_status(channel, pStatus) != ESP_OK)
        {
            hr = CLR_E_PROCESS_EXCEPTION;
            NANOCLR_LEAVE();
        }
        stack.SetResult_I4(pStatus);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_BaseRmtChannel::NativeGetClockDivider___STATIC__U1__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        int32_t channel = (int32_t)pThis[FIELD___channel].NumericByRefConst().s4;

        if ((channel < 0) || (channel >= MAX_RMT_CHANNELS))
        {
            hr = CLR_E_INDEX_OUT_OF_RANGE;
            NANOCLR_LEAVE();
        }

        uint8_t *pClkDiv;

        if (rmt_get_clk_div(channel, pClkDiv) != ESP_OK)
        {
            hr = CLR_E_PROCESS_EXCEPTION;
            NANOCLR_LEAVE();
        }

        stack.SetResult_U1(pClkDiv);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_BaseRmtChannel::NativeGetMemoryBlockNumber___STATIC__U1__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        int32_t channel = (int32_t)pThis[FIELD___channel].NumericByRefConst().s4;

        if ((channel < 0) || (channel >= MAX_RMT_CHANNELS))
        {
            hr = CLR_E_INDEX_OUT_OF_RANGE;
            NANOCLR_LEAVE();
        }

        uint8_t *pMemBlkNum;

        if (rmt_get_mem_block_num(channel, pMemBlkNum) != ESP_OK)
        {
            hr = CLR_E_PROCESS_EXCEPTION;
            NANOCLR_LEAVE();
        }

        stack.SetResult_U1(pMemBlkNum);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_BaseRmtChannel::NativeGetMemoryLowPower___STATIC__BOOLEAN__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        int32_t channel = (int32_t)pThis[FIELD___channel].NumericByRefConst().s4;

        if ((channel < 0) || (channel >= MAX_RMT_CHANNELS))
        {
            hr = CLR_E_INDEX_OUT_OF_RANGE;
            NANOCLR_LEAVE();
        }

        bool *pLowPower;

        if (rmt_get_mem_pd(channel, pLowPower) != ESP_OK)
        {
            hr = CLR_E_PROCESS_EXCEPTION;
            NANOCLR_LEAVE();
        }

        stack.SetResult_Boolean(pLowPower);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_BaseRmtChannel::NativeGetSourceClock___STATIC__I4__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        int32_t channel = (int32_t)pThis[FIELD___channel].NumericByRefConst().s4;

        if ((channel < 0) || (channel >= MAX_RMT_CHANNELS))
        {
            hr = CLR_E_INDEX_OUT_OF_RANGE;
            NANOCLR_LEAVE();
        }

        rmt_source_clk_t *pSourceClk;

        if (rmt_get_source_clk(channel, pSourceClk) != ESP_OK)
        {
            hr = CLR_E_PROCESS_EXCEPTION;
            NANOCLR_LEAVE();
        }

        stack.SetResult_I4(pSourceClk);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_BaseRmtChannel::NativeSetClockDivider___STATIC__VOID__I4__U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        int32_t channel = (int32_t)pThis[FIELD___channel].NumericByRefConst().s4;
        uint8_t clkDiv = (uint8_t)pThis[2].NumericByRefConst().u1;

        if ((channel < 0) || (channel >= MAX_RMT_CHANNELS))
        {
            hr = CLR_E_INDEX_OUT_OF_RANGE;
            NANOCLR_LEAVE();
        }

        if ((clkDiv == 0))
        {
            hr = CLR_E_INDEX_OUT_OF_RANGE;
            NANOCLR_LEAVE();
        }

        if (rmt_set_clk_div(channel, clkDiv) != ESP_OK)
        {
            hr = CLR_E_PROCESS_EXCEPTION;
            NANOCLR_LEAVE();
        }

        stack.SetResult()
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_BaseRmtChannel::NativeSetMemoryBlockNumber___STATIC__VOID__I4__U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        int32_t channel = (int32_t)pThis[FIELD___channel].NumericByRefConst().s4;
        uint8_t memBlkNum = (uint8_t)pThis[2].NumericByRefConst().u1;

        if ((channel < 0) || (channel >= MAX_RMT_CHANNELS))
        {
            hr = CLR_E_INDEX_OUT_OF_RANGE;
            NANOCLR_LEAVE();
        }

        if (rmt_set_mem_block_num(channel, memBlkNum) != ESP_OK)
        {
            hr = CLR_E_PROCESS_EXCEPTION;
            NANOCLR_LEAVE();
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_BaseRmtChannel::NativeSetMemoryLowPower___STATIC__VOID__I4__BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        int32_t channel = (int32_t)pThis[FIELD___channel].NumericByRefConst().s4;
        bool lowPower = (bool)pThis[2].NumericByRefConst().u1;

        if (rmt_set_mem_pd(channel, lowpower) != ESP_OK)
        {
            hr = CLR_E_PROCESS_EXCEPTION;
            NANOCLR_LEAVE();
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_BaseRmtChannel::NativeSetSourceClock___STATIC__VOID__I4__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        int32_t channel = (int32_t)pThis[FIELD___channel].NumericByRefConst().s4;
        rmt_source_clk_t sourceClk = (rmt_source_clk_t)pThis[2].NumericByRefConst.s4;

        if (rmt_set_source_clk(channel, sourceClk) != ESP_OK)
        {
            hr = CLR_E_PROCESS_EXCEPTION;
            NANOCLR_LEAVE();
        }
    }

    NANOCLR_NOCLEANUP();
}
