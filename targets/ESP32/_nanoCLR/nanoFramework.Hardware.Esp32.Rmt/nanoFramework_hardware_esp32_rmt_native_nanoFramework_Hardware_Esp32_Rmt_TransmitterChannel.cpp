//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <cstring>
#include "nanoFramework_hardware_esp32_rmt_native.h"

typedef Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtChannel RmtChannel;
typedef Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtChannelSettings
    RmtChannelSettings;
typedef Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitChannelSettings
    TransmitterChannelSettings;

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::
    NativeTxInit___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *transmitter_channel_settings = NULL;
        int32_t channel;
        int32_t pin_number;

        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // get a reference to the configs in the managed code instance
        transmitter_channel_settings = pThis[FIELD___transmitterChannelSettings].Dereference();

        channel = transmitter_channel_settings[RmtChannelSettings::FIELD___channel].NumericByRef().s4;
        pin_number = transmitter_channel_settings[RmtChannelSettings::FIELD___pinNumber].NumericByRef().s4;

        if (channel < 0)
        {
            channel = RmtChannel::FindNextChannel();
            if (channel < 0)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_DRIVER_NOT_REGISTERED);
            }
        }

        rmt_config_t rmt_tx_config = RMT_DEFAULT_CONFIG_TX((gpio_num_t)pin_number, (rmt_channel_t)channel);
        rmt_tx_config.clk_div =
            transmitter_channel_settings[RmtChannelSettings::FIELD___clockDivider].NumericByRef().u1;
        rmt_tx_config.mem_block_num =
            transmitter_channel_settings[RmtChannelSettings::FIELD___numberOfMemoryBlocks].NumericByRef().u1;
        rmt_tx_config.tx_config.carrier_en =
            (bool)transmitter_channel_settings[TransmitterChannelSettings::FIELD___enableCarrierWave].NumericByRef().u1;
        rmt_tx_config.tx_config.carrier_freq_hz =
            transmitter_channel_settings[TransmitterChannelSettings::FIELD___carrierWaveFrequency].NumericByRef().u4;

        bool carrier_level =
            (bool)transmitter_channel_settings[TransmitterChannelSettings::FIELD___carrierLevel].NumericByRef().u1;
        rmt_tx_config.tx_config.carrier_level = carrier_level ? RMT_CARRIER_LEVEL_HIGH : RMT_CARRIER_LEVEL_LOW;

        rmt_tx_config.tx_config.carrier_duty_percent =
            transmitter_channel_settings[TransmitterChannelSettings::FIELD___carrierWaveDutyPercentage]
                .NumericByRef()
                .u1;
        rmt_tx_config.tx_config.loop_en =
            (bool)transmitter_channel_settings[TransmitterChannelSettings::FIELD___enableLooping].NumericByRef().u1;
        rmt_tx_config.tx_config.idle_output_en =
            (bool)transmitter_channel_settings[TransmitterChannelSettings::FIELD___enableIdleLevelOutput]
                .NumericByRef()
                .u1;

        bool idle_level =
            (bool)transmitter_channel_settings[TransmitterChannelSettings::FIELD___idleLevel].NumericByRef().u1;
        rmt_tx_config.tx_config.idle_level = idle_level ? RMT_IDLE_LEVEL_HIGH : RMT_IDLE_LEVEL_LOW;

        bool invert_signal =
            (bool)transmitter_channel_settings[RmtChannelSettings::FIELD___signalInverterEnabled].NumericByRef().u1;
        if (invert_signal)
        {
            rmt_tx_config.flags = RMT_CHANNEL_FLAGS_INVERT_SIG;
        }

        auto err = rmt_config(&rmt_tx_config);
        if (err != ESP_OK)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_DRIVER_NOT_REGISTERED);
        }

        err = rmt_driver_install((rmt_channel_t)channel, 0, 0); // no ring buffer needed
        if (err != ESP_OK)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_DRIVER_NOT_REGISTERED);
        }

        RmtChannel::registredChannels.emplace(
            std::piecewise_construct,
            std::forward_as_tuple((rmt_channel_t)channel),
            std::forward_as_tuple());

        stack.SetResult_I4((CLR_INT32)channel);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::
    NativeTxGetIsChannelIdle___BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *transmitter_channel_settings = NULL;
    int32_t channel;
    bool retVal = 0;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // get a reference to the configs in the managed code instance
    transmitter_channel_settings = pThis[FIELD___transmitterChannelSettings].Dereference();
    channel = transmitter_channel_settings[RmtChannelSettings::FIELD___channel].NumericByRef().s4;

    if (!RmtChannel::CheckChannel(channel))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    retVal = ::RMT.conf_ch[(rmt_channel_t)channel].conf1.idle_out_en;

    stack.SetResult_Boolean(retVal);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::
    NativeTxSetLoopingMode___VOID__BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *transmitter_channel_settings = NULL;
    int32_t channel;
    bool enabled;
    esp_err_t err;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // get a reference to the configs in the managed code instance
    transmitter_channel_settings = pThis[FIELD___transmitterChannelSettings].Dereference();
    channel = transmitter_channel_settings[RmtChannelSettings::FIELD___channel].NumericByRef().s4;
    enabled = (bool)transmitter_channel_settings[TransmitterChannelSettings::FIELD___enableLooping].NumericByRef().u1;

    if (!RmtChannel::CheckChannel(channel))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    err = rmt_set_tx_loop_mode((rmt_channel_t)channel, enabled);
    if (err != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::
    NativeTxSetCarrierMode___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int32_t channel;
    bool carrier_en;
    uint32_t carrier_freq_hz;
    uint8_t carrier_duty_percent;
    bool carrier_level;
    esp_err_t err;

    CLR_RT_HeapBlock *transmitter_channel_settings = NULL;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // get a reference to the configs in the managed code instance
    transmitter_channel_settings = pThis[FIELD___transmitterChannelSettings].Dereference();
    channel = transmitter_channel_settings[RmtChannelSettings::FIELD___channel].NumericByRef().s4;
    carrier_en =
        (bool)transmitter_channel_settings[TransmitterChannelSettings::FIELD___enableCarrierWave].NumericByRef().u1;
    carrier_freq_hz =
        transmitter_channel_settings[TransmitterChannelSettings::FIELD___carrierWaveFrequency].NumericByRef().u4;
    carrier_duty_percent =
        transmitter_channel_settings[TransmitterChannelSettings::FIELD___carrierWaveDutyPercentage].NumericByRef().u1;
    carrier_level =
        (bool)transmitter_channel_settings[TransmitterChannelSettings::FIELD___carrierLevel].NumericByRef().u1;

    if (!RmtChannel::CheckChannel(channel))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    uint32_t rmt_source_clk_hz, duty_div, duty_h, duty_l;
    rmt_source_clk_hz = APB_CLK_FREQ; // only APB bus clock is supported
    duty_div = rmt_source_clk_hz / carrier_freq_hz;
    duty_h = duty_div * carrier_duty_percent / 100;
    duty_l = duty_div - duty_h;

    err = rmt_set_tx_carrier(
        (rmt_channel_t)channel,
        carrier_en,
        duty_h,
        duty_l,
        carrier_level ? RMT_CARRIER_LEVEL_HIGH : RMT_CARRIER_LEVEL_LOW);
    if (err != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::
    NativeTxSetIdleLevel___VOID__BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    rmt_channel_t channel;
    bool idle_out_en;
    bool idle_level;
    esp_err_t err;

    CLR_RT_HeapBlock *transmitter_channel_settings = NULL;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // get a reference to the configs in the managed code instance
    transmitter_channel_settings = pThis[FIELD___transmitterChannelSettings].Dereference();
    channel = (rmt_channel_t)transmitter_channel_settings[RmtChannelSettings::FIELD___channel].NumericByRef().s4;
    idle_out_en =
        (bool)transmitter_channel_settings[TransmitterChannelSettings::FIELD___enableIdleLevelOutput].NumericByRef().u1;
    idle_level = (bool)transmitter_channel_settings[TransmitterChannelSettings::FIELD___idleLevel].NumericByRef().u1;

    if (!RmtChannel::CheckChannel(channel))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    err = rmt_set_idle_level(channel, idle_out_en, idle_level ? RMT_IDLE_LEVEL_HIGH : RMT_IDLE_LEVEL_LOW);
    if (err != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::
    NativeTxWriteItems___U4__SZARRAY_U1__BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *transmitter_channel_settings = NULL;
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // get a reference to the configs in the managed code instance
    transmitter_channel_settings = pThis[FIELD___transmitterChannelSettings].Dereference();

    {
        int32_t channel = transmitter_channel_settings[RmtChannelSettings::FIELD___channel].NumericByRef().s4;
        bool wait_tx_done = (bool)stack.Arg2().NumericByRef().u1;

        CLR_RT_HeapBlock_Array *data = stack.Arg1().DereferenceArray();
        char *buffer = (char *)data->GetFirstElement();
        uint32_t bufferLength = data->m_numOfElements;

        auto it = RmtChannel::registredChannels.find(CHANNEL(channel));
        if (it == RmtChannel::registredChannels.end())
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
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

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::
    NativeTxDispose___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *transmitter_channel_settings = NULL;
    int32_t channel;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // get a reference to the configs in the managed code instance
    transmitter_channel_settings = pThis[FIELD___transmitterChannelSettings].Dereference();
    channel = (int32_t)transmitter_channel_settings[RmtChannelSettings::FIELD___channel].NumericByRef().s4;

    if (!RmtChannel::CheckChannel(channel))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    if (rmt_driver_uninstall(CHANNEL(channel)) != ESP_OK)
    {
        hr = CLR_E_PROCESS_EXCEPTION;
    }

    RmtChannel::registredChannels.erase(CHANNEL(channel));

    NANOCLR_NOCLEANUP();
}
