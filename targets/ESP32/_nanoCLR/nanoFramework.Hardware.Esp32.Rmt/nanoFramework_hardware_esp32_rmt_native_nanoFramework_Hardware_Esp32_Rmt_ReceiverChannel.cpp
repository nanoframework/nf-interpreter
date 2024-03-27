//
// Copyright (c) 2020 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "nanoFramework_hardware_esp32_rmt_native.h"

// typedefs for manged types to improve readability
typedef Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtChannelSettings
    RmtChannelSettings;
typedef Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ReceiverChannelSettings
    ReceiverChannelSettings;
typedef Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtChannel RmtChannel;
typedef Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtCommand ManagedRmtCommand;

//--//

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ReceiverChannel::
    NativeRxInit___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int32_t channel;
    int32_t pin_number = 0;
    int32_t ring_buff_size;
    esp_err_t err;

    CLR_RT_HeapBlock *receiver_channel_settings;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // get a reference to the configs in the managed code instance
    receiver_channel_settings = pThis[FIELD___receiverChannelSettings].Dereference();

    channel = receiver_channel_settings[RmtChannelSettings::FIELD___channel].NumericByRef().s4;
    pin_number = receiver_channel_settings[RmtChannelSettings::FIELD___pinNumber].NumericByRef().s4;

    if (channel < 0)
    {
        channel = RmtChannel::FindNextChannel();
        if (channel < 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_DRIVER_NOT_REGISTERED);
        }
    }

    {
        rmt_config_t rmt_rx_config = GetNewRmtRxConfig((gpio_num_t)pin_number, (rmt_channel_t)channel);

        rmt_rx_config.clk_div = receiver_channel_settings[RmtChannelSettings::FIELD___clockDivider].NumericByRef().u1;
        rmt_rx_config.mem_block_num =
            receiver_channel_settings[RmtChannelSettings::FIELD___numberOfMemoryBlocks].NumericByRef().u1;
        rmt_rx_config.rx_config.idle_threshold =
            receiver_channel_settings[ReceiverChannelSettings::FIELD___idleThreshold].NumericByRef().u2;
        rmt_rx_config.rx_config.filter_en =
            receiver_channel_settings[ReceiverChannelSettings::FIELD___enableFilter].NumericByRef().u1 != 0;
        rmt_rx_config.rx_config.filter_ticks_thresh =
            receiver_channel_settings[ReceiverChannelSettings::FIELD___filterThreshold].NumericByRef().u1;

#if SOC_RMT_SUPPORT_RX_DEMODULATION

        rmt_rx_config.rx_config.rm_carrier =
            receiver_channel_settings[ReceiverChannelSettings::FIELD___enableDemodulation].NumericByRef().u1 != 0;
        rmt_rx_config.rx_config.carrier_freq_hz =
            receiver_channel_settings[ReceiverChannelSettings::FIELD___carrierWaveFrequency].NumericByRef().u4;
        rmt_rx_config.rx_config.carrier_duty_percent =
            receiver_channel_settings[ReceiverChannelSettings::FIELD___carrierWaveDutyPercentage].NumericByRef().u1;
        rmt_rx_config.rx_config.carrier_level =
            (bool)receiver_channel_settings[ReceiverChannelSettings::FIELD___carrierLevel].NumericByRef().u1
                ? RMT_CARRIER_LEVEL_HIGH
                : RMT_CARRIER_LEVEL_LOW;

#endif

        err = rmt_config(&rmt_rx_config);
        if (err != ESP_OK)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_DRIVER_NOT_REGISTERED);
        }
    }

    ring_buff_size = receiver_channel_settings[RmtChannelSettings::FIELD___bufferSize].NumericByRef().s4;
    ring_buff_size *= sizeof(rmt_item32_t);

    err = rmt_driver_install((rmt_channel_t)channel, ring_buff_size, 0);
    if (err != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_DRIVER_NOT_REGISTERED);
    }

    RmtChannel::registredChannels.emplace(
        std::piecewise_construct,
        std::forward_as_tuple((rmt_channel_t)channel),
        std::forward_as_tuple());

    stack.SetResult_I4((CLR_INT32)channel);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ReceiverChannel::
    NativeRxStart___VOID__BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *receiver_channel_settings = NULL;
    CLR_INT32 channel;
    bool clearBuffer;
    esp_err_t err;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // get a reference to the configs in the managed code instance
    receiver_channel_settings = pThis[FIELD___receiverChannelSettings].Dereference();

    channel = receiver_channel_settings[RmtChannelSettings::FIELD___channel].NumericByRef().s4;

    clearBuffer = stack.Arg1().NumericByRefConst().u1 != 0;

    if (clearBuffer)
    {
        RingbufHandle_t ringbufHandle;

        err = rmt_get_ringbuf_handle((rmt_channel_t)channel, &ringbufHandle);
        if (err != ESP_OK)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        // Clear ring buffer
        size_t length = 0;
        rmt_item32_t *rmtItems;

        while ((rmtItems = (rmt_item32_t *)xRingbufferReceive(ringbufHandle, &length, 0)) != NULL)
        {
            // Release items back to Ringbuffer
            vRingbufferReturnItem(ringbufHandle, (void *)rmtItems);
        };
    }

    err = rmt_rx_start((rmt_channel_t)channel, clearBuffer);
    if (err != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ReceiverChannel::
    NativeRxStop___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *receiver_channel_settings = NULL;
    CLR_INT32 channel;
    esp_err_t err;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // get a reference to the configs in the managed code instance
    receiver_channel_settings = pThis[FIELD___receiverChannelSettings].Dereference();
    channel = receiver_channel_settings[RmtChannelSettings::FIELD___channel].NumericByRef().s4;

    err = rmt_rx_stop((rmt_channel_t)channel);
    if (err != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ReceiverChannel::
    NativeRxGetRingBufferCount___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_INT32 channel;
    esp_err_t err;
    RingbufHandle_t ringbufHandle;
    UBaseType_t uxItemsWaiting;

    CLR_RT_HeapBlock *receiver_channel_settings = NULL;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // get a reference to the configs in the managed code instance
    receiver_channel_settings = pThis[FIELD___receiverChannelSettings].Dereference();
    channel = receiver_channel_settings[RmtChannelSettings::FIELD___channel].NumericByRef().s4;

    err = rmt_get_ringbuf_handle((rmt_channel_t)channel, &ringbufHandle);
    if (err != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    vRingbufferGetInfo(ringbufHandle, NULL, NULL, NULL, NULL, &uxItemsWaiting);

    stack.SetResult_I4(uxItemsWaiting);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ReceiverChannel::
    CreateRmtElement(rmt_item32_t *rmtItem, CLR_RT_HeapBlock *returnArray, CLR_RT_TypeDef_Index &rmtCommandTypeDef)
{
    HRESULT hr = S_OK;

    // create a new RmtCommand object and put it on the return array
    hr = g_CLR_RT_ExecutionEngine.NewObjectFromIndex(*returnArray, rmtCommandTypeDef);
    if (hr == S_OK)
    {
        // Set each value in the struct from the rmt_item32_t data
        CLR_RT_HeapBlock *dref = returnArray->Dereference();
        CLR_RT_HeapBlock &duration0FieldRef = dref[ManagedRmtCommand::FIELD___duration0];
        CLR_RT_HeapBlock &level0FieldRef = dref[ManagedRmtCommand::FIELD___level0];
        CLR_RT_HeapBlock &duration1FieldRef = dref[ManagedRmtCommand::FIELD___duration1];
        CLR_RT_HeapBlock &level1FieldRef = dref[ManagedRmtCommand::FIELD___level1];

        int32_t *d0 = (int32_t *)&duration0FieldRef.NumericByRef().s4;
        *d0 = rmtItem->duration0;

        int32_t *l0 = (int32_t *)&level0FieldRef.NumericByRef().u1;
        *l0 = rmtItem->level0;

        int32_t *d1 = (int32_t *)&duration1FieldRef.NumericByRef().s4;
        *d1 = rmtItem->duration1;

        int32_t *l1 = (int32_t *)&level1FieldRef.NumericByRef().u1;
        *l1 = rmtItem->level1;
    }

    return hr;
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ReceiverChannel::
    CreateRmtArrayOnStack(
        CLR_RT_StackFrame &stack,
        CLR_INT32 numItems,
        CLR_RT_TypeDef_Index &rmtCommandTypeDef,
        CLR_RT_HeapBlock **arrayDataPtr)
{
    HRESULT hr = S_OK;

    if (!g_CLR_RT_TypeSystem.FindTypeDef("RmtCommand", "nanoFramework.Hardware.Esp32.Rmt", rmtCommandTypeDef))
    {
        // Type not defined, build problem
        return CLR_E_NOT_SUPPORTED;
    }

    // Create Array on top of stackof type "RmtCommand"
    hr = CLR_RT_HeapBlock_Array::CreateInstance(stack.PushValue(), numItems, rmtCommandTypeDef);

    if (hr == S_OK)
    {
        // Get reference to Array
        CLR_RT_HeapBlock_Array *array = stack.TopValue().DereferenceArray();

        //  Get start of array data, array of CLR_RT_HeapBlock * to RmtCommand structs
        *arrayDataPtr = (CLR_RT_HeapBlock *)array->GetFirstElement();
    }

    return hr;
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ReceiverChannel::
    NativeRxGetAllItems___SZARRAY_nanoFrameworkHardwareEsp32RmtRmtCommand(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_INT32 channel;
    esp_err_t err;
    RingbufHandle_t ringbufHandle;
    CLR_INT32 waitMs;
    CLR_INT64 *timespan;
    size_t length = 0;
    rmt_item32_t *rmtItems = NULL;

    CLR_RT_HeapBlock *receiver_channel_settings = NULL;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // get a reference to the configs in the managed code instance
    receiver_channel_settings = pThis[FIELD___receiverChannelSettings].Dereference();
    channel = receiver_channel_settings[RmtChannelSettings::FIELD___channel].NumericByRef().s4;

    err = rmt_get_ringbuf_handle((rmt_channel_t)channel, &ringbufHandle);
    if (err != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // Timespan in 100ns ticks
    timespan = Library_corlib_native_System_TimeSpan::GetValuePtr(
        receiver_channel_settings[ReceiverChannelSettings::FIELD___receiveTimeout]);
    // Convert to milli-secs for wait
    waitMs = (CLR_INT32)(*timespan / TIME_CONVERSION__TO_MILLISECONDS);

    // Get next Rmt item from ring buffer
    rmtItems = (rmt_item32_t *)xRingbufferReceive(ringbufHandle, &length, pdMS_TO_TICKS(waitMs));
    if (rmtItems)
    {
        if (length > 0)
        {
            CLR_RT_HeapBlock *arrayDataPtr;
            CLR_RT_TypeDef_Index rmtCommandTypeDef;

            int numItems = length / sizeof(rmt_item32_t);

            // Build Header block for Array of RmtCommands objects on top of stack
            // Returns the rmtCommand type and pointer to first data ptr in array
            NANOCLR_CHECK_HRESULT(CreateRmtArrayOnStack(stack, numItems, rmtCommandTypeDef, &arrayDataPtr));

            rmt_item32_t *pData = rmtItems;
            while (numItems-- > 0)
            {
                // Add RMT item to managed array
                NANOCLR_CHECK_HRESULT(CreateRmtElement(pData, arrayDataPtr, rmtCommandTypeDef));

                // Next item
                pData++;
                arrayDataPtr++;
            }
        }

        // Release items back to Ringbuffer
        vRingbufferReturnItem(ringbufHandle, (void *)rmtItems);
    }

    if (length == 0)
    {
        stack.SetResult_Object(NULL);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ReceiverChannel::
    NativeRxEnableFilter___VOID__BOOLEAN__U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_INT32 channel;
    bool enableFilter;
    uint8_t threshold;
    esp_err_t err;

    CLR_RT_HeapBlock *receiver_channel_settings = NULL;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    enableFilter = stack.Arg1().NumericByRefConst().u1 != 0;
    threshold = stack.Arg2().NumericByRefConst().u1;

    // get a reference to the configs in the managed code instance
    receiver_channel_settings = pThis[FIELD___receiverChannelSettings].Dereference();
    channel = receiver_channel_settings[RmtChannelSettings::FIELD___channel].NumericByRef().s4;

    err = rmt_set_rx_filter((rmt_channel_t)channel, enableFilter, threshold);
    if (err != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ReceiverChannel::
    NativeRxSetIdleThresold___VOID__U2(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    uint16_t threshold;
    CLR_INT32 channel;
    esp_err_t err;

    CLR_RT_HeapBlock *receiver_channel_settings = NULL;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    threshold = stack.Arg1().NumericByRefConst().u2;

    // get a reference to the configs in the managed code instance
    receiver_channel_settings = pThis[FIELD___receiverChannelSettings].Dereference();
    channel = receiver_channel_settings[RmtChannelSettings::FIELD___channel].NumericByRef().s4;

    err = rmt_set_rx_idle_thresh((rmt_channel_t)channel, threshold);
    if (err != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ReceiverChannel::
    NativeRxDispose___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_INT32 channel;

    CLR_RT_HeapBlock *receiver_channel_settings = NULL;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // get a reference to the configs in the managed code instance
    receiver_channel_settings = pThis[FIELD___receiverChannelSettings].Dereference();
    channel = receiver_channel_settings[RmtChannelSettings::FIELD___channel].NumericByRef().s4;

    if (RmtChannel::registredChannels.find((rmt_channel_t)channel) == RmtChannel::registredChannels.end())
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    if (rmt_driver_uninstall((rmt_channel_t)channel) != ESP_OK)
    {
        hr = CLR_E_PROCESS_EXCEPTION;
    }

    RmtChannel::registredChannels.erase((rmt_channel_t)channel);

    NANOCLR_NOCLEANUP();
}

rmt_config_t Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ReceiverChannel::
    GetNewRmtRxConfig(gpio_num_t pin, rmt_channel_t channel)
{
    rmt_config_t config = rmt_config_t();

    config.rmt_mode = RMT_MODE_RX;
    config.channel = channel;
    config.gpio_num = pin;
    config.clk_div = 80;
    config.mem_block_num = 1;
    config.flags = 0;

    config.rx_config = rmt_rx_config_t();
    config.rx_config.idle_threshold = 12000;
    config.rx_config.filter_ticks_thresh = 100;
    config.rx_config.filter_en = true;

#if SOC_RMT_SUPPORT_RX_DEMODULATION
    config.rx_config.rm_carrier = true;
    config.rx_config.carrier_freq_hz = 38000;
    config.rx_config.carrier_duty_percent = 33;
    config.rx_config.carrier_level = RMT_CARRIER_LEVEL_HIGH;
#endif

    return config;
}
