//
// Copyright (c) 2020 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <vector>
#include <map>
#include <cstring>
#include "nanoFramework_hardware_esp32_rmt_native.h"


#define CHANNEL(ch)                             static_cast<rmt_channel_t>(ch)
#define DEFAULT_DEVIDER                         4

static std::map<rmt_channel_t, std::vector<rmt_item32_t>> Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::registredChannels;

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::NativeInit___STATIC__I4__I4(CLR_RT_StackFrame& stack)
{
    NANOCLR_HEADER()
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        
        uint8_t gpio_number = (uint8_t)stack.Arg0().NumericByRef().s1;
        
        auto ch = find_next_channel();
        if (ch < 0) 
        {
            hr = CLR_E_DRIVER_NOT_REGISTERED;
            NANOCLR_LEAVE();
        }
        if (init_channel((rmt_channel_t)ch, (gpio_num_t)gpio_number) != ESP_OK) 
        {
            hr = CLR_E_DRIVER_NOT_REGISTERED;
            NANOCLR_LEAVE();
        }
        stack.SetResult_I4(ch);       
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::NativeGetIdleLevel___STATIC__BOOLEAN__I4(CLR_RT_StackFrame& stack)
{
    NANOCLR_HEADER()
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        int32_t channel = (int32_t)stack.Arg0().NumericByRef().s4;

        bool retVal = 0; 
        if (channel < 0) 
        {
            hr = CLR_E_INDEX_OUT_OF_RANGE;
            NANOCLR_LEAVE();
        }
        {
            if (registredChannels.find(CHANNEL(channel)) == registredChannels.end()) 
            {
                hr = CLR_E_OBJECT_DISPOSED;
                NANOCLR_LEAVE();
            }
            retVal = ::RMT.conf_ch[(rmt_channel_t)channel].conf1.idle_out_lv;

            stack.SetResult_Boolean(retVal);   
        }
    }
    NANOCLR_NOCLEANUP();
}
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::NativeGetIsChannelIdle___STATIC__BOOLEAN__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER()
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        int32_t channel = (int32_t)stack.Arg0().NumericByRef().s4;

        bool retVal = 0; 
        if (channel < 0) 
        {
            hr = CLR_E_INDEX_OUT_OF_RANGE;
            NANOCLR_LEAVE();
        }
        {
            if (registredChannels.find(CHANNEL(channel)) == registredChannels.end()) 
            {
                hr = CLR_E_OBJECT_DISPOSED;
                NANOCLR_LEAVE();
            }

            retVal = ::RMT.conf_ch[(rmt_channel_t)channel].conf1.idle_out_en;

            stack.SetResult_Boolean(retVal); 
        }        
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::NativeSetIsChannelIdle___STATIC__VOID__I4__BOOLEAN( CLR_RT_StackFrame& stack)
{
    NANOCLR_HEADER()
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        int32_t channel = (int32_t)stack.Arg0().NumericByRef().s4;
        bool enabled = (bool)stack.Arg1().NumericByRef().u1;

        if (channel < 0) 
        {
            hr = CLR_E_INDEX_OUT_OF_RANGE;
            NANOCLR_LEAVE();
        }
        {
            if (registredChannels.find(CHANNEL(channel)) == registredChannels.end()) 
            {
                hr = CLR_E_OBJECT_DISPOSED;
                NANOCLR_LEAVE();
            }
            ::RMT.conf_ch[(rmt_channel_t)channel].conf1.idle_out_en = enabled;
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::NativeSetIdleLevel___STATIC__VOID__I4__BOOLEAN(CLR_RT_StackFrame& stack)
{
   NANOCLR_HEADER()
   {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        int32_t channel = (int32_t)stack.Arg0().NumericByRef().s4;
        bool idle_lvl = (bool)stack.Arg1().NumericByRef().u1;

        if(channel < 0) 
        {
            hr = CLR_E_INDEX_OUT_OF_RANGE;
            NANOCLR_LEAVE();
        }
        {
            if (registredChannels.find(CHANNEL(channel)) == registredChannels.end()) 
            {
                hr = CLR_E_OBJECT_DISPOSED;
                NANOCLR_LEAVE();
            }
            ::RMT.conf_ch[(rmt_channel_t)channel].conf1.idle_out_lv = idle_lvl;
        }
   }
   NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::NativeSetCarrierMode___STATIC__VOID__I4__BOOLEAN__U2__U2__BOOLEAN( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER()
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        int32_t channel = (int32_t)stack.Arg0().NumericByRef().s4;
        bool carier_en = (bool)stack.Arg1().NumericByRef().u1;
        uint16_t high_lvl = (uint16_t)stack.Arg2().NumericByRef().u2;
        uint16_t low_level = (uint16_t)stack.Arg3().NumericByRef().u2;
        bool carier_lvl = (bool)stack.Arg4().NumericByRef().u1;

        if (channel < 0) 
        {
            hr = CLR_E_INDEX_OUT_OF_RANGE;
            NANOCLR_LEAVE();
        }
        {
            if (registredChannels.find(CHANNEL(channel)) == registredChannels.end()) 
            {
                hr = CLR_E_OBJECT_DISPOSED;
                NANOCLR_LEAVE();
            }
            auto &ch = ::RMT.conf_ch[(rmt_channel_t)channel];
            ch.conf0.carrier_en = carier_en;
            ch.conf0.carrier_out_lv = (uint32_t)carier_lvl;
            auto &cdc = ::RMT.carrier_duty_ch[(rmt_channel_t)channel];
            cdc.high = high_lvl;
            cdc.low = low_level;
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::NativeWriteItems___STATIC__U4__I4__SZARRAY_U1__BOOLEAN( signed int channel, CLR_RT_TypedArray_UINT8 data, bool wait_tx_done, HRESULT &hr )
{
    NANOCLR_HEADER()
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        int32_t channel = (int32_t)stack.Arg0().NumericByRef().s4;
        bool wait_tx_done = (bool)stack.Arg2().NumericByRef().u1;

        CLR_RT_HeapBlock_Array* data = stack.Arg1().DereferenceArray();
        char* buffer = (char*)data->GetFirstElement();
        uint32_t bufferLength = data->m_numOfElements;

        if (channel < 0)
        {
            hr = CLR_E_INDEX_OUT_OF_RANGE;
            NANOCLR_LEAVE();
        }
        {
            auto it = registredChannels.find(CHANNEL(channel));
            if (it == registredChannels.end()) 
            {
                hr = CLR_E_OBJECT_DISPOSED;
                NANOCLR_LEAVE();
            }
            
            const auto element_size = sizeof(rmt_item32_t);
            if (bufferLength % element_size != 0) 
            {
                hr = CLR_E_SERIALIZATION_VIOLATION;
                NANOCLR_LEAVE();
            }
            auto &d = it->second;
            d.assign(bufferLength / element_size, rmt_item32_t{});
            std::memcpy(d.data(), buffer, bufferLength);
            int result = rmt_write_items((rmt_channel_t)channel, d.data(), d.size(), wait_tx_done);

            stack.SetResult_U4(result);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::NativeWaitTxDone___STATIC__U4__I4__I4( CLR_RT_StackFrame& stack  )
{
   NANOCLR_HEADER()
   {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        int32_t channel = (int32_t)stack.Arg0().NumericByRef().s4;
        int32_t wait_time = (int32_t)stack.Arg1().NumericByRef().s4;

        int result = ESP_ERR_TIMEOUT;
        if (channel < 0) 
        {
            hr = CLR_E_INDEX_OUT_OF_RANGE;
            NANOCLR_LEAVE();
        }
        {
            if (registredChannels.find(CHANNEL(channel)) == registredChannels.end()) 
            {
                hr = CLR_E_OBJECT_DISPOSED;
                NANOCLR_LEAVE();
            }
            result = rmt_wait_tx_done((rmt_channel_t)channel, wait_time);

            stack.SetResult_U4(result); 
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::NativeDispose___STATIC__VOID__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER()
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        int32_t channel = (int32_t)stack.Arg0().NumericByRef().s4;

        if (channel < 0) 
        {
            hr = CLR_E_INDEX_OUT_OF_RANGE;
            NANOCLR_LEAVE();
        }
        if (registredChannels.find(CHANNEL(channel)) == registredChannels.end()) 
        {
            hr = CLR_E_OBJECT_DISPOSED;
            NANOCLR_LEAVE();
        }

        if (rmt_driver_uninstall(CHANNEL(channel)) != ESP_OK) 
        {
            hr = CLR_E_PROCESS_EXCEPTION;
        }
        
        registredChannels.erase(CHANNEL(channel));
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::NativeGetClockDivider___STATIC__U1__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER()
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        int32_t channel = (int32_t)stack.Arg0().NumericByRef().s4;

        unsigned char retVal = 0; 
        if (channel < 0) 
        {
            hr = CLR_E_INDEX_OUT_OF_RANGE;
            NANOCLR_LEAVE();
        }
        {
            if (registredChannels.find(CHANNEL(channel)) == registredChannels.end()) 
            {
                hr = CLR_E_OBJECT_DISPOSED;
                NANOCLR_LEAVE();
            }
            retVal = ::RMT.conf_ch[(rmt_channel_t)channel].conf0.div_cnt;

            stack.SetResult_U1(retVal);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::NativeGetSourceClock___STATIC__BOOLEAN__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER()
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        int32_t channel = (int32_t)stack.Arg0().NumericByRef().s4;

        bool retVal = false;
        if (channel < 0) {
            hr = CLR_E_INDEX_OUT_OF_RANGE;
            NANOCLR_LEAVE();
        }
        {
            if (registredChannels.find(CHANNEL(channel)) == registredChannels.end()) 
            {
                hr = CLR_E_OBJECT_DISPOSED;
                NANOCLR_LEAVE();
            }
            retVal = ::RMT.conf_ch[(rmt_channel_t)channel].conf1.ref_always_on;

            stack.SetResult_Boolean(retVal);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::NativeSetClockDivider___STATIC__VOID__I4__U1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER()
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        int32_t channel = (int32_t)stack.Arg0().NumericByRef().s4;
        u_int8_t clockdiv = (u_int8_t)stack.Arg1().NumericByRef().u1;

        if (channel < 0) 
        {
            hr = CLR_E_INDEX_OUT_OF_RANGE;
            NANOCLR_LEAVE();
        }
        {
            if (registredChannels.find(CHANNEL(channel)) == registredChannels.end()) 
            {
                hr = CLR_E_OBJECT_DISPOSED;
                NANOCLR_LEAVE();
            }
            rmt_set_clk_div((rmt_channel_t)channel, clockdiv);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::NativeSetSourceClock___STATIC__VOID__I4__BOOLEAN( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER()
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        int32_t channel = (int32_t)stack.Arg0().NumericByRef().s4;
        bool is80MhzMode (bool)stack.Arg1().NumericByRef().u1;

        if (channel < 0) 
        {
            hr = CLR_E_INDEX_OUT_OF_RANGE;
            NANOCLR_LEAVE();
        }
        {
            if (registredChannels.find(CHANNEL(channel)) == registredChannels.end()) 
            {
                hr = CLR_E_OBJECT_DISPOSED;
                NANOCLR_LEAVE();
            }
            ::RMT.conf_ch[(rmt_channel_t)channel].conf1.ref_always_on = is80MhzMode;
        }
    }
    NANOCLR_NOCLEANUP();
}

esp_err_t Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::init_channel(rmt_channel_t channel, gpio_num_t gpio) 
{
    rmt_config_t rmt_tx
    {
        RMT_MODE_TX,
        channel,
        DEFAULT_DEVIDER,
        gpio,
        1,
        {
            false,
            38000,
            50,
            RMT_CARRIER_LEVEL_HIGH,
            false,
            RMT_IDLE_LEVEL_LOW,
            true
        }
    };
    auto err = rmt_config(&rmt_tx);
    if (err != ESP_OK) return err;
    err = rmt_driver_install(channel, 0, 0);
    if (err != ESP_OK) return err;
    registredChannels.emplace(std::piecewise_construct,
                         std::forward_as_tuple(channel),
                         std::forward_as_tuple());
    return ESP_OK;
}

signed int Transmitter::find_next_channel() 
{
    for (signed int ch = RMT_CHANNEL_0; ch < RMT_CHANNEL_MAX ; ++ch) 
    {
        if (registredChannels.find(CHANNEL(ch)) == registredChannels.end()) 
        {
            return ch;
        }
    }
    return -1;
}
