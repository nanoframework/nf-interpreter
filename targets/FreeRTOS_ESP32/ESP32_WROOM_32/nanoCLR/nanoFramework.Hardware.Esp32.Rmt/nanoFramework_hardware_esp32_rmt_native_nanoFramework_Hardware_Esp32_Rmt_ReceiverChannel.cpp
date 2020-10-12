//
// Copyright (c) 2020 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "nanoFramework_hardware_esp32_rmt_native.h"

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ReceiverChannel::
    NativeRxInit___I4__I4__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    gpio_num_t gpio_number;
    CLR_INT32 channel;
    CLR_INT32 bufferSizeRmtItems;
    CLR_INT32 clockDiv = 80; // Default
    gpio_number = (gpio_num_t)stack.Arg1().NumericByRef().s1;
    bufferSizeRmtItems = (CLR_INT32)stack.Arg2().NumericByRef().s1;

    channel = RmtChannel::FindNextChannel();
    if (channel < 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_DRIVER_NOT_REGISTERED);
    }

    if (InitRxChannel((rmt_channel_t)channel, gpio_number, bufferSizeRmtItems, clockDiv) != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_DRIVER_NOT_REGISTERED);
    }

    stack.SetResult_I4(channel);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ReceiverChannel::
    NativeRxStart___VOID__BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_INT32 channel;
        bool clearBuffer;

        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        clearBuffer = stack.Arg1().NumericByRefConst().u1 != 0;

        channel = pThis[RmtChannel::FIELD___channel].NumericByRef().s4;

        if (clearBuffer)
        {
            RingbufHandle_t ringbufHandle;
            auto err = rmt_get_ringbuf_handle((rmt_channel_t)channel, &ringbufHandle);
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

        auto err = rmt_rx_start((rmt_channel_t)channel, clearBuffer);
        if (err != ESP_OK)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ReceiverChannel::
    NativeRxStop___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_INT32 channel;

        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        channel = pThis[RmtChannel::FIELD___channel].NumericByRef().s4;

        auto err = rmt_rx_stop((rmt_channel_t)channel);
        if (err != ESP_OK)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ReceiverChannel::
    NativeRxGetRingBufferCount___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_INT32 channel;
        esp_err_t err;
        RingbufHandle_t ringbufHandle;

        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        channel = pThis[RmtChannel::FIELD___channel].NumericByRef().s4;

        err = rmt_get_ringbuf_handle((rmt_channel_t)channel, &ringbufHandle);
        if (err != ESP_OK)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        UBaseType_t uxItemsWaiting;
        vRingbufferGetInfo(ringbufHandle, NULL, NULL, NULL, &uxItemsWaiting);

        stack.SetResult_I4(uxItemsWaiting);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ReceiverChannel::
    CreateRmtElement(rmt_item32_t *rmtItem, CLR_RT_HeapBlock *returnArray, CLR_RT_TypeDef_Index &rmtCommandTypeDef)
{
    NANOCLR_HEADER();
    {
        // create a new RmtCommand object and put it on the return array
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(*returnArray, rmtCommandTypeDef));

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
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ReceiverChannel::
    CreateRmtArrayOnStack(
        CLR_RT_StackFrame &stack,
        CLR_INT32 numItems,
        CLR_RT_TypeDef_Index &rmtCommandTypeDef,
        CLR_RT_HeapBlock **arrayDataPtr)
{
    NANOCLR_HEADER();
    {
        if (!g_CLR_RT_TypeSystem.FindTypeDef("RmtCommand", "nanoFramework.Hardware.Esp32.Rmt", rmtCommandTypeDef))
        {
            // Type not defined, build problem
            NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
        }

        // Create Array on top of stackof type "RmtCommand"
        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(
            stack.PushValue(),
            numItems,
            rmtCommandTypeDef)); // g_CLR_RT_WellKnownTypes.m_Object ));

        // Get reference to Array
        CLR_RT_HeapBlock_Array *array = stack.TopValue().DereferenceArray();

        //  Get start of array data, array of CLR_RT_HeapBlock * to RmtCommand structs
        *arrayDataPtr = (CLR_RT_HeapBlock *)array->GetFirstElement();
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ReceiverChannel::
    NativeRxGetAllItems___SZARRAY_nanoFrameworkHardwareEsp32RmtRmtCommand(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_INT32 channel;
        // CLR_RT_HeapBlock *pRetVal = NULL;
        esp_err_t err;
        RingbufHandle_t ringbufHandle;
        CLR_INT32 waitMs;
        rmt_item32_t *rmtItems = NULL;

        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        channel = pThis[RmtChannel::FIELD___channel].NumericByRef().s4;

        err = rmt_get_ringbuf_handle((rmt_channel_t)channel, &ringbufHandle);
        if (err != ESP_OK)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        // Timespan in 100ns ticks
        CLR_INT64 *timespan = Library_corlib_native_System_TimeSpan::GetValuePtr(pThis[FIELD___receiveTimeout]);
        // Convert to milli-secs for wait
        waitMs = (CLR_INT32)(*timespan / TIME_CONVERSION__TO_MILLISECONDS);

        // Get next Rmt item from ring buffer
        size_t length = 0;
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
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ReceiverChannel::
    NativeRxEnableFilter___VOID__BOOLEAN__U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        bool enableFilter = stack.Arg1().NumericByRefConst().u1 != 0;
        uint8_t threshold = stack.Arg2().NumericByRefConst().u1;

        CLR_INT32 channel = pThis[RmtChannel::FIELD___channel].NumericByRef().s4;

        esp_err_t err = rmt_set_rx_filter((rmt_channel_t)channel, enableFilter, threshold);
        if (err != ESP_OK)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ReceiverChannel::
    NativeRxSetIdleThresold___VOID__U2(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        uint16_t threshold = stack.Arg1().NumericByRefConst().u2;

        CLR_INT32 channel = pThis[RmtChannel::FIELD___channel].NumericByRef().s4;

        esp_err_t err = rmt_set_rx_idle_thresh((rmt_channel_t)channel, threshold);
        if (err != ESP_OK)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ReceiverChannel::
    NativeRxDispose___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        CLR_INT32 channel = pThis[RmtChannel::FIELD___channel].NumericByRef().s4;

        if (RmtChannel::registredChannels.find((rmt_channel_t)channel) == RmtChannel::registredChannels.end())
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        if (rmt_driver_uninstall((rmt_channel_t)channel) != ESP_OK)
        {
            hr = CLR_E_PROCESS_EXCEPTION;
        }

        RmtChannel::registredChannels.erase((rmt_channel_t)channel);
    }
    NANOCLR_NOCLEANUP();
}

esp_err_t Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ReceiverChannel::
    InitRxChannel(rmt_channel_t channel, gpio_num_t gpio, int32_t bufferSizeRmtItems, int32_t clockDiv)
{
    rmt_config_t rmt_rxconfig{};

    rmt_rxconfig.rmt_mode = RMT_MODE_RX; // Channel mode
    rmt_rxconfig.channel = channel;      // Channel
    rmt_rxconfig.clk_div = clockDiv;     // Channel counter divider
                                         // 80,000,000 / 80 = 1,000,000 or 1us tick
    rmt_rxconfig.gpio_num = gpio;        // GPIO number
    rmt_rxconfig.mem_block_num = 1;      // number of memory blocks

    rmt_rxconfig.rx_config.filter_en = false; // No filter or theshold
    rmt_rxconfig.rx_config.filter_ticks_thresh = 0;
    rmt_rxconfig.rx_config.idle_threshold = 0;
    auto err = rmt_config(&rmt_rxconfig);
    if (err != ESP_OK)
        return err;

    int32_t ringBufferSize = bufferSizeRmtItems * sizeof(rmt_item32_t);
    err = rmt_driver_install(channel, ringBufferSize, 0);
    if (err != ESP_OK)
        return err;

    RmtChannel::registredChannels.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(channel),
        std::forward_as_tuple());

    return ESP_OK;
}