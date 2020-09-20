//
// Copyright (c) 2020 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "nanoFramework_hardware_esp32_rmt_native.h"

std::map<rmt_channel_t, std::vector<rmt_item32_t>>
    Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtChannel::registredChannels;

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtChannel::
    NativeSetClockDivider___VOID__U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int32_t channel;
    uint8_t clockdiv = (uint8_t)stack.Arg1().NumericByRef().u1;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    channel = (int32_t)pThis[FIELD___channel].NumericByRef().s4;
    if (!CheckChannel(channel))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    rmt_set_clk_div((rmt_channel_t)channel, clockdiv);

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
