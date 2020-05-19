//-----------------------------------------------------------------------------
//
//                   ** WARNING! ** 
//    This file was generated automatically by a tool.
//    Re-running the tool will overwrite this file.
//    You should copy this file to a custom location
//    before adding any customization in the copy to
//    prevent loss of your changes when the tool is
//    re-run.
//
//-----------------------------------------------------------------------------
#include <vector>
#include <map>
#include <cstring>
#include "nanoFramework_hardware_esp32_rmt_native.h"

#define MY_CLEANUP()							hr = S_OK; nanoCLR_Cleanup: return
#define MY_CLEANUP_D(r)							hr = S_OK; nanoCLR_Cleanup: return r
#define CHANNEL(ch)                             static_cast<rmt_channel_t>(ch)

#define DEFAULT_DEVIDER                         4

static std::map<rmt_channel_t, std::vector<rmt_item32_t>> Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::registredChannels;

signed int Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::NativeInit___STATIC__I4__I4(signed int channel, HRESULT &hr)
{
     auto ch = find_next_channel();
    if (ch < 0) {
        hr = CLR_E_DRIVER_NOT_REGISTERED;
        NANOCLR_LEAVE();
    }
    if (init_channel((rmt_channel_t)ch, (gpio_num_t)gpio_number) != ESP_OK) {
        hr = CLR_E_DRIVER_NOT_REGISTERED;
        NANOCLR_LEAVE();
    }
    
    MY_CLEANUP_D(ch);
}

bool Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::NativeGetIdleLevel___STATIC__BOOLEAN__I4( signed int channel, HRESULT &hr )
{
    bool retVal = 0; 
    if (channel < 0) {
        hr = CLR_E_INDEX_OUT_OF_RANGE;
        NANOCLR_LEAVE();
    }
    {
        if (registredChannels.find(CHANNEL(channel)) == registredChannels.end()) {
            hr = CLR_E_OBJECT_DISPOSED;
            NANOCLR_LEAVE();
        }
        retVal = ::RMT.conf_ch[(rmt_channel_t)channel].conf1.idle_out_lv;
    }
    MY_CLEANUP_D(retVal);
}

bool Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::NativeGetIsChannelIdle___STATIC__BOOLEAN__I4( signed int channel, HRESULT &hr )
{
    bool retVal = 0; 
    if (channel < 0) {
        hr = CLR_E_INDEX_OUT_OF_RANGE;
        NANOCLR_LEAVE();
    }
    {
        if (registredChannels.find(CHANNEL(channel)) == registredChannels.end()) {
            hr = CLR_E_OBJECT_DISPOSED;
            NANOCLR_LEAVE();
        }

        retVal = ::RMT.conf_ch[(rmt_channel_t)channel].conf1.idle_out_en;
    }
    MY_CLEANUP_D(retVal);
}

void Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::NativeSetIsChannelIdle___STATIC__VOID__I4__BOOLEAN( signed int channel, bool enabled, HRESULT &hr)
{
    if (channel < 0) {
        hr = CLR_E_INDEX_OUT_OF_RANGE;
        NANOCLR_LEAVE();
    }
    {
        if (registredChannels.find(CHANNEL(channel)) == registredChannels.end()) {
            hr = CLR_E_OBJECT_DISPOSED;
            NANOCLR_LEAVE();
        }
        ::RMT.conf_ch[(rmt_channel_t)channel].conf1.idle_out_en = enabled;
    }
    MY_CLEANUP()
}

void Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::NativeSetIdleLevel___STATIC__VOID__I4__BOOLEAN(signed int channel, bool idle_lvl, HRESULT &hr)
{
   if (channel < 0) {
        hr = CLR_E_INDEX_OUT_OF_RANGE;
        NANOCLR_LEAVE();
    }
    {
        if (registredChannels.find(CHANNEL(channel)) == registredChannels.end()) {
            hr = CLR_E_OBJECT_DISPOSED;
            NANOCLR_LEAVE();
        }
        ::RMT.conf_ch[(rmt_channel_t)channel].conf1.idle_out_lv = idle_lvl;
    }
    MY_CLEANUP();
}

void Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::NativeSetCarrierMode___STATIC__VOID__I4__BOOLEAN__U2__U2__BOOLEAN( signed int channel, bool carier_en,
                                                                                    unsigned short high_lvl, unsigned short low_level, int carier_lvl, HRESULT &hr )
{
    if (channel < 0) {
        hr = CLR_E_INDEX_OUT_OF_RANGE;
        NANOCLR_LEAVE();
    }
    {
        if (registredChannels.find(CHANNEL(channel)) == registredChannels.end()) {
            hr = CLR_E_OBJECT_DISPOSED;
            NANOCLR_LEAVE();
        }
        auto &ch = ::RMT.conf_ch[(rmt_channel_t)channel];
        ch.conf0.carrier_en = carier_en;
        ch.conf0.carrier_out_lv = carier_lvl;
        auto &cdc = ::RMT.carrier_duty_ch[(rmt_channel_t)channel];
        cdc.high = high_lvl;
        cdc.low = low_level;
    }
    MY_CLEANUP();
}

void Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::NativeWriteItems___STATIC__U4__I4__SZARRAY_U1__BOOLEAN( signed int channel, CLR_RT_TypedArray_UINT8 data, bool wait_tx_done, HRESULT &hr )
{
    if (channel < 0) {
        hr = CLR_E_INDEX_OUT_OF_RANGE;
        NANOCLR_LEAVE();
    }
    {
        auto it = registredChannels.find(CHANNEL(channel));
        if (it == registredChannels.end()) {
            hr = CLR_E_OBJECT_DISPOSED;
            NANOCLR_LEAVE();
        }
        
        const auto element_size = sizeof(rmt_item32_t);
        if (data.GetSize() % element_size != 0) {
            hr = CLR_E_SERIALIZATION_VIOLATION;
            NANOCLR_LEAVE();
        }
        auto &d = it->second;
        d.assign(data.GetSize() / element_size, rmt_item32_t{});
        std::memcpy(d.data(), data.GetBuffer(), data.GetSize());
        rmt_write_items((rmt_channel_t)channel, d.data(), d.size(), wait_tx_done);
    }
    MY_CLEANUP();
}

uint Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::NativeWaitTxDone___STATIC__U4__I4__I4( signed int channel, int wait_time, HRESULT &hr )
{
   int result = ESP_ERR_TIMEOUT;
    if (channel < 0) {
        hr = CLR_E_INDEX_OUT_OF_RANGE;
        NANOCLR_LEAVE();
    }
    {
        if (registredChannels.find(CHANNEL(channel)) == registredChannels.end()) {
            hr = CLR_E_OBJECT_DISPOSED;
            NANOCLR_LEAVE();
        }
        result = rmt_wait_tx_done((rmt_channel_t)channel, wait_time);
    }
    MY_CLEANUP(result);
}

void Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::NativeDispose___STATIC__VOID__I4( signed int channel, HRESULT &hr )
{
    if (channel < 0) {
        hr = CLR_E_INDEX_OUT_OF_RANGE;
        NANOCLR_LEAVE();
    }
    if (registredChannels.find(CHANNEL(channel)) == registredChannels.end()) {
        hr = CLR_E_OBJECT_DISPOSED;
        NANOCLR_LEAVE();
    }

    if (rmt_driver_uninstall(CHANNEL(channel)) != ESP_OK) {
        hr = CLR_E_PROCESS_EXCEPTION;
    }
    
    registredChannels.erase(CHANNEL(channel));

    MY_CLEANUP();
}

unsigned char Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::NativeGetClockDivider___STATIC__U1__I4( signed int channel, HRESULT &hr )
{
   unsigned char retVal = 0; 
    if (channel < 0) {
        hr = CLR_E_INDEX_OUT_OF_RANGE;
        NANOCLR_LEAVE();
    }
    {
        if (registredChannels.find(CHANNEL(channel)) == registredChannels.end()) {
            hr = CLR_E_OBJECT_DISPOSED;
            NANOCLR_LEAVE();
        }
        retVal = ::RMT.conf_ch[(rmt_channel_t)channel].conf0.div_cnt;
    }
    MY_CLEANUP_D(retVal);
}

bool Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::NativeGetSourceClock___STATIC__BOOLEAN__I4( signed int channel, HRESULT &hr )
{
   bool retVal = false;
    if (channel < 0) {
        hr = CLR_E_INDEX_OUT_OF_RANGE;
        NANOCLR_LEAVE();
    }
    {
        if (registredChannels.find(CHANNEL(channel)) == registredChannels.end()) {
            hr = CLR_E_OBJECT_DISPOSED;
            NANOCLR_LEAVE();
        }
        retVal = ::RMT.conf_ch[(rmt_channel_t)channel].conf1.ref_always_on; 
    }
    MY_CLEANUP_D(retVal);
}

void Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::NativeSetClockDivider___STATIC__VOID__I4__U1( signed int channel, unsigned char clockdiv, HRESULT &hr )
{
   if (channel < 0) {
        hr = CLR_E_INDEX_OUT_OF_RANGE;
        NANOCLR_LEAVE();
    }
    {
        if (registredChannels.find(CHANNEL(channel)) == registredChannels.end()) {
            hr = CLR_E_OBJECT_DISPOSED;
            NANOCLR_LEAVE();
        }
        rmt_set_clk_div((rmt_channel_t)channel, clockdiv);
    }
    MY_CLEANUP();
}

void Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::NativeSetSourceClock___STATIC__VOID__I4__BOOLEAN( signed int channel, bool is80MhzMode, HRESULT &hr )
{
    if (channel < 0) {
        hr = CLR_E_INDEX_OUT_OF_RANGE;
        NANOCLR_LEAVE();
    }
    {
        if (registredChannels.find(CHANNEL(channel)) == registredChannels.end()) {
            hr = CLR_E_OBJECT_DISPOSED;
            NANOCLR_LEAVE();
        }
        ::RMT.conf_ch[(rmt_channel_t)channel].conf1.ref_always_on = is80MhzMode;
    }
    MY_CLEANUP();
}

esp_err_t Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::init_channel(rmt_channel_t channel, gpio_num_t gpio) {
    rmt_config_t rmt_tx{
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

signed int Transmitter::find_next_channel() {
    for (signed int ch = RMT_CHANNEL_0; ch < RMT_CHANNEL_MAX ; ++ch) {
        if (registredChannels.find(CHANNEL(ch)) == registredChannels.end()) {
            return ch;
        }
    }
    return -1;
}
