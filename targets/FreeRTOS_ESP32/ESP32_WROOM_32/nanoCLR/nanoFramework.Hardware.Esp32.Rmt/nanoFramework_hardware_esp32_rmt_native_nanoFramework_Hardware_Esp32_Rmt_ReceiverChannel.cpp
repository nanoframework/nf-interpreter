//
// Copyright (c) 2020 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "nanoFramework_hardware_esp32_rmt_native.h"

#define DEFAULT_DIVIDER 4

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ReceiverChannel::
    NativeInit___I4__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    
    gpio_num_t gpio_number;
    CLR_INT32 channel;

    gpio_number = (gpio_num_t)stack.Arg1().NumericByRef().s1;

    channel = 
        Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::FindNextChannel();

    if (channel < 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_DRIVER_NOT_REGISTERED); 
    }

    if (InitRxChannel((rmt_channel_t)channel, gpio_number) != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_DRIVER_NOT_REGISTERED);
    }

    stack.SetResult_I4(channel);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ReceiverChannel::
    NativeStart___VOID__BOOLEAN( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        int32_t channel;
        bool clearBuffer;

        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        clearBuffer = stack.Arg1().NumericByRefConst().u1 != 0;

        channel = (int32_t)pThis[FIELD___channel].NumericByRef().s4;

        auto err = rmt_rx_start((rmt_channel_t)channel, clearBuffer);
        if (err != ESP_OK)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ReceiverChannel::
    NativeStop___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        int32_t channel;

        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        channel = (int32_t)pThis[FIELD___channel].NumericByRef().s4;

        auto err = rmt_rx_stop((rmt_channel_t)channel);
        if (err != ESP_OK)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ReceiverChannel::
    NativeGetNext___BOOLEAN__BYREF_nanoFrameworkHardwareEsp32RmtRmtCommand( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        (void)stack;   

         NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ReceiverChannel::
    NativeEnable___VOID__BOOLEAN__U1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        bool enableFilter = stack.Arg1().NumericByRefConst().u1 != 0;
        uint8_t threshold = stack.Arg2().NumericByRefConst().u1;

        int32_t channel = (int32_t)pThis[FIELD___channel].NumericByRef().s4;

        esp_err_t err = rmt_set_rx_filter((rmt_channel_t)channel, enableFilter, threshold);
        if (err != ESP_OK)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ReceiverChannel::
    NativeDispose___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        int32_t channel = (int32_t)pThis[FIELD___channel].NumericByRef().s4;

        if ( Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::
                registredChannels.find((rmt_channel_t)channel) == Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::
                                                                    registredChannels.end())
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        if (rmt_driver_uninstall((rmt_channel_t)channel) != ESP_OK)
        {
            hr = CLR_E_PROCESS_EXCEPTION;
        }

        Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::
            registredChannels.erase((rmt_channel_t)channel);

    }
    NANOCLR_NOCLEANUP();
}

esp_err_t Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ReceiverChannel::
    InitRxChannel(
    rmt_channel_t channel,
    gpio_num_t gpio)
{
    rmt_config_t rmt_rxconfig{};

    rmt_rxconfig.rmt_mode = RMT_MODE_RX;    // Channel mode
    rmt_rxconfig.channel = channel;         // Channel
    rmt_rxconfig.clk_div = DEFAULT_DIVIDER; // Channel counter divider
    rmt_rxconfig.gpio_num = gpio;           // GPIO number
    rmt_rxconfig.mem_block_num = 1;         // Memory block number
    rmt_rxconfig.rx_config = {false, 0, 0};    // Filter

    auto err = rmt_config(&rmt_rxconfig);
    if (err != ESP_OK)
        return err;

    err = rmt_driver_install(channel, 0, 0);
    if (err != ESP_OK)
        return err;

    Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::
        registredChannels.emplace(std::piecewise_construct, std::forward_as_tuple(channel), std::forward_as_tuple());

    return ESP_OK;
}