//
// Copyright (c) 2020 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "nanoFramework_hardware_esp32_rmt_native.h"

std::map<rmt_channel_t, std::vector<rmt_item32_t>>
    Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtChannel::registredChannels;

typedef Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtChannelSettings
    RmtChannelSettings;

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtChannel::
    NativeSetGpioPin___VOID__I4__U1__I4__BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    rmt_channel_t channel = CHANNEL(stack.Arg1().NumericByRef().s4);
    uint8_t mode = stack.Arg2().NumericByRef().u1;
    int32_t pin = stack.Arg3().NumericByRef().s4;
    bool invert_signal = (bool)stack.Arg4().NumericByRef().u1;

    rmt_mode_t rmt_mode;
    if (mode == 0x00)
    {
        rmt_mode = RMT_MODE_RX;
    }
    else
    {
        rmt_mode = RMT_MODE_TX;
    }

    auto err = rmt_set_gpio(channel, rmt_mode, (gpio_num_t)pin, invert_signal);
    if (err != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtChannel::
    NativeSetClockDivider___VOID__U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *channel_settings = NULL;
    int32_t channel;
    uint8_t clockdiv = (uint8_t)stack.Arg1().NumericByRef().u1;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // get a reference to the configs in the managed code instance
    channel_settings = pThis[FIELD___settings].Dereference();
    channel = channel_settings[RmtChannelSettings::FIELD___channel].NumericByRef().s4;

    if (!CheckChannel(channel))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    rmt_set_clk_div((rmt_channel_t)channel, clockdiv);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtChannel::
    NativeSetNumberOfMemoryBlocks___VOID__U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *channel_settings = NULL;
        int32_t channel;
        uint8_t rmt_mem_num;

        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // get a reference to the configs in the managed code instance
        channel_settings = pThis[FIELD___settings].Dereference();
        channel = channel_settings[RmtChannelSettings::FIELD___channel].NumericByRef().s4;

        if (!CheckChannel(channel))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        rmt_mem_num = (uint8_t)channel_settings[RmtChannelSettings::FIELD___numberOfMemoryBlocks].NumericByRef().u1;
        auto err = rmt_set_mem_block_num((rmt_channel_t)channel, rmt_mem_num);
        if (err != ESP_OK)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    NANOCLR_NOCLEANUP();
}

//
//  Search map for next free Channel and return channel number.
//  return -1 if no free channels.
//
CLR_INT32 Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtChannel::FindNextChannel()
{
    for (signed int channel = RMT_CHANNEL_0; channel < RMT_CHANNEL_MAX; ++channel)
    {
        if (registredChannels.find(CHANNEL(channel)) == registredChannels.end())
        {
            return channel;
        }
    }
    return -1;
}

//
//  Check if channel is present in map
//  return true if present (valid)
//
bool Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtChannel::CheckChannel(
    int channel)
{
    if (registredChannels.find(CHANNEL(channel)) == registredChannels.end())
    {
        return false;
    }

    return true;
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtChannel::
    NativeGetSourceClockFrequency___STATIC__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

// Currently we use the default clock for all targets. This is 80Mhz except for H2 where it is 32Mhz.
#if defined(CONFIG_IDF_TARGET_ESP32H2)
    stack.SetResult_I4(32000000);
#else
    stack.SetResult_I4(80000000);
#endif

    NANOCLR_NOCLEANUP_NOLABEL();
}
