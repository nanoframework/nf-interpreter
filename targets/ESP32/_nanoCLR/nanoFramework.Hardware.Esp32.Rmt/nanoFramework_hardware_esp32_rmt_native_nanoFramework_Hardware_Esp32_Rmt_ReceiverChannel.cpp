//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nanoFramework_hardware_esp32_rmt_native.h"

// typedefs for managed types to improve readability and size of lines
typedef Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtChannelSettings
    RmtChannelSettings;
typedef Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ReceiverChannelSettings
    ReceiverChannelSettings;
typedef Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtChannel RmtChannel;
typedef Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtSymbol ManagedRmtSymbol;

static HRESULT RXSetDemodulation(rmt_channel_handle_t rx_chan, CLR_RT_HeapBlock *receiver_channel_settings)
{
    NANOCLR_HEADER();

    // pointers
    CLR_RT_HeapBlock *settings;

    // primitives
    bool enableDemodulation;

    settings = receiver_channel_settings;
    enableDemodulation = settings[ReceiverChannelSettings::FIELD___enableDemodulation].NumericByRef().u1 != 0;

    if (enableDemodulation)
    {
#if SOC_RMT_SUPPORT_RX_DEMODULATION
        rmt_carrier_config_t rx_carrier_cfg;
        esp_err_t err;

        rx_carrier_cfg = {};
        rx_carrier_cfg.frequency_hz = settings[ReceiverChannelSettings::FIELD___carrierWaveFrequency].NumericByRef().u4;

        rx_carrier_cfg.duty_cycle =
            settings[ReceiverChannelSettings::FIELD___carrierWaveDutyPercentage].NumericByRef().r4;
        rx_carrier_cfg.duty_cycle /= 100.0;

        rx_carrier_cfg.flags.polarity_active_low =
            (bool)settings[ReceiverChannelSettings::FIELD___carrierLevel].NumericByRef().u1 ? 0 : 1;

        err = rmt_apply_carrier(rx_chan, &rx_carrier_cfg);
        NANOCLR_CHECK_HRESULT(RmtChannel::RmtMapEspErrToClrErr(err));
#else
        NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif
    }

    NANOCLR_NOCLEANUP();
}

static IRAM_ATTR bool rmt_rx_done_callback(
    rmt_channel_handle_t channel,
    const rmt_rx_done_event_data_t *edata,
    void *user_data)
{
    // primitives
    BaseType_t high_task_wakeup;

    // handles
    QueueHandle_t cb_receive_queue;

    high_task_wakeup = pdFALSE;
    cb_receive_queue = (QueueHandle_t)user_data;

    // send the received RMT symbols to the parser task
    xQueueSendFromISR(cb_receive_queue, edata, &high_task_wakeup);

    // Wake up receive thread
    Events_Set(SYSTEM_EVENT_FLAG_RMT_RX);

    // return whether any task is woken up
    return high_task_wakeup == pdTRUE;
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ReceiverChannel::
    NativeRxInit___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    rmt_channel_handle_t rx_chan_handle = NULL;
    QueueHandle_t receive_queue = NULL;
    rmt_rx_event_callbacks_t cbs;
    rmt_rx_channel_config_t rx_chan_config = {};
    uint8_t *buffer = NULL;
    CLR_INT32 buffer_size;
    esp_err_t err;

    CLR_RT_HeapBlock *receiver_channel_settings;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // get a reference to the configs in the managed code instance
    receiver_channel_settings = pThis[FIELD___receiverChannelSettings].Dereference();

    // Select default source clock for the target ESP32 type.
    rx_chan_config.clk_src = RMT_CLK_SRC_DEFAULT;

    // Driver will allocate the interrupt
    rx_chan_config.intr_priority = 0;

    // Gpio number for RMT rx
    rx_chan_config.gpio_num =
        (gpio_num_t)receiver_channel_settings[RmtChannelSettings::FIELD___pinNumber].NumericByRef().s4;

    // Tick resolution, i.e., 1Mhz : 1 tick = 1 µs
    rx_chan_config.resolution_hz =
        receiver_channel_settings[RmtChannelSettings::FIELD___resolutionHz].NumericByRef().s4;

    // Number of memory blocks * CONFIG_SOC_RMT_MEM_WORDS_PER_CHANNEL
    rx_chan_config.mem_block_symbols =
        receiver_channel_settings[RmtChannelSettings::FIELD___numberOfMemoryBlocks].NumericByRef().u1;
    rx_chan_config.mem_block_symbols *= CONFIG_SOC_RMT_MEM_WORDS_PER_CHANNEL;

    // Invert signal input ?
    rx_chan_config.flags.invert_in =
        receiver_channel_settings[RmtChannelSettings::FIELD___signalInverterEnabled].NumericByRef().u1;

    // Don't use DMA for RX channel, as it is not supported on all ESP32 targets
    // maybe we can extend later to support DMA for RX channel on targets that support it
    rx_chan_config.flags.with_dma = false;
    rx_chan_config.flags.io_loop_back = false;

    err = rmt_new_rx_channel(&rx_chan_config, &rx_chan_handle);
    NANOCLR_CHECK_HRESULT(RmtChannel::RmtMapEspErrToClrErr(err));

    // Enable Demodulation if feature available on target ?
    NANOCLR_CHECK_HRESULT(RXSetDemodulation(rx_chan_handle, receiver_channel_settings));

    receive_queue = xQueueCreate(1, sizeof(rmt_rx_done_event_data_t));
    if (receive_queue == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    cbs = {.on_recv_done = rmt_rx_done_callback};

    // Register event for RX done events
    err = rmt_rx_register_event_callbacks(rx_chan_handle, &cbs, receive_queue);
    NANOCLR_CHECK_HRESULT(RmtChannel::RmtMapEspErrToClrErr(err));

    // Buffer size from managed code is in number symbols to buffer
    buffer_size = receiver_channel_settings[ReceiverChannelSettings::FIELD___bufferSize].NumericByRef().s4;
    buffer_size *= sizeof(rmt_symbol_word_t);

    buffer = (uint8_t *)platform_malloc(buffer_size);
    if (buffer == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    // Enable RX channel
    err = rmt_enable(rx_chan_handle);
    NANOCLR_CHECK_HRESULT(RmtChannel::RmtMapEspErrToClrErr(err));

    RmtChannel::AddRegisteredRxChannel(rx_chan_handle, receive_queue, buffer, buffer_size);

    // Make sure all handles and buffers are disposed if system restarted(Debugging)
    HAL_AddSoftRebootHandler(RmtChannel::UninitializeAll);

    stack.SetResult_I4((CLR_INT32)(intptr_t)rx_chan_handle);

    // ownership transferred to the registry
    rx_chan_handle = NULL;
    receive_queue = NULL;
    buffer = NULL;

    NANOCLR_CLEANUP();

    if (buffer != NULL)
    {
        platform_free(buffer);
    }

    if (receive_queue != NULL)
    {
        vQueueDelete(receive_queue);
    }

    if (rx_chan_handle != NULL)
    {
        rmt_disable(rx_chan_handle);
        rmt_del_channel(rx_chan_handle);
    }

    NANOCLR_CLEANUP_END();
}

HRESULT StartReceive(
    rmt_channel_handle_t rx_handle,
    CLR_RT_HeapBlock *receiver_channel_settings,
    RmtChannelInfo *rcInfo)
{
    NANOCLR_HEADER();

    rmt_receive_config_t rx_config;
    esp_err_t err;

    rx_config = {};
    rx_config.signal_range_max_ns =
        receiver_channel_settings[ReceiverChannelSettings::FIELD___idleThreshold].NumericByRef().u4;

    rx_config.signal_range_min_ns =
        receiver_channel_settings[ReceiverChannelSettings::FIELD___filterThreshold].NumericByRef().u4;

    rx_config.flags.en_partial_rx = false;

    // Enable if not enabled, ignore result, as it may already be enabled
    rmt_enable(rx_handle);

    err = rmt_receive(rx_handle, rcInfo->rxBuffer, rcInfo->rxBufferSize, &rx_config);
    if (err != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(RmtChannel::RmtMapEspErrToClrErr(err));
    }

    NANOCLR_NOCLEANUP();
}

//
//  Start continuous receive (non-blocking)
//
HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ReceiverChannel::
    NativeStartReceive___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    rmt_channel_handle_t rx_handle = NULL;
    CLR_RT_HeapBlock *pThis;
    CLR_RT_HeapBlock *receiver_channel_settings;
    RmtChannelInfo *rcInfo;

    pThis = stack.This();
    FAULT_ON_NULL(pThis);

    receiver_channel_settings = pThis[FIELD___receiverChannelSettings].Dereference();
    rx_handle = (rmt_channel_handle_t)receiver_channel_settings[RmtChannelSettings::FIELD___handle].NumericByRef().s4;

    rcInfo = RmtChannel::FindRegisteredRmtChannel(rx_handle, RmtChannelType::Receive);
    if (rcInfo == nullptr)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    if (rcInfo->isBlockingReceiving || rcInfo->isContinuousReceiving)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    NANOCLR_CHECK_HRESULT(StartReceive(rx_handle, receiver_channel_settings, rcInfo));

    rcInfo->isContinuousReceiving = true;

    NANOCLR_NOCLEANUP();
}

//
//  Stop continuous receive
//
HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ReceiverChannel::
    NativeStopReceive___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    // handles
    rmt_channel_handle_t rx_handle;

    // pointers
    CLR_RT_HeapBlock *pThis;
    CLR_RT_HeapBlock *receiver_channel_settings;
    RmtChannelInfo *rcInfo;

    rx_handle = NULL;
    pThis = stack.This();
    FAULT_ON_NULL(pThis);

    receiver_channel_settings = pThis[FIELD___receiverChannelSettings].Dereference();
    rx_handle = (rmt_channel_handle_t)receiver_channel_settings[RmtChannelSettings::FIELD___handle].NumericByRef().s4;

    rcInfo = RmtChannel::FindRegisteredRmtChannel(rx_handle, RmtChannelType::Receive);
    if (rcInfo == nullptr)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    if (!rcInfo->isContinuousReceiving)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    // Disable and ignore result
    rmt_disable(rx_handle);
    rcInfo->isContinuousReceiving = false;

    // discard any completion event queued before the channel stopped
    xQueueReset(rcInfo->rxQueue);

    NANOCLR_NOCLEANUP();
}

//
//  Non-blocking poll for received symbols (only valid in continuous mode)
//
HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ReceiverChannel::
    NativeTryGetReceived___SZARRAY_nanoFrameworkHardwareEsp32RmtRmtSymbol(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    // handles
    rmt_channel_handle_t rx_handle;

    // pointers
    CLR_RT_HeapBlock *pThis;
    CLR_RT_HeapBlock *receiver_channel_settings;
    RmtChannelInfo *rcInfo;
    CLR_RT_HeapBlock *arrayDataPtr;

    // primitives
    int numItems;

    // structs
    rmt_rx_done_event_data_t rx_data;
    CLR_RT_TypeDef_Index rmtSymbolTypeDef;
    rmt_symbol_word_t *pData;

    rx_handle = NULL;
    arrayDataPtr = NULL;
    numItems = 0;
    rx_data = {};
    rmtSymbolTypeDef = CLR_RT_TypeDef_Index();
    pData = NULL;

    pThis = stack.This();
    FAULT_ON_NULL(pThis);

    receiver_channel_settings = pThis[FIELD___receiverChannelSettings].Dereference();
    rx_handle = (rmt_channel_handle_t)receiver_channel_settings[RmtChannelSettings::FIELD___handle].NumericByRef().s4;

    rcInfo = RmtChannel::FindRegisteredRmtChannel(rx_handle, RmtChannelType::Receive);
    if (rcInfo == nullptr)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    if (rcInfo->isBlockingReceiving)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    if (!rcInfo->isContinuousReceiving)
    {
        stack.SetResult_Object(NULL);
        NANOCLR_SET_AND_LEAVE(S_OK);
    }

    if (xQueueReceive(rcInfo->rxQueue, &rx_data, 0) != pdTRUE)
    {
        stack.SetResult_Object(NULL);
        NANOCLR_SET_AND_LEAVE(S_OK);
    }

    if (rx_data.num_symbols == 0)
    {
        // queue the next receive operation before returning
        NANOCLR_CHECK_HRESULT(StartReceive(rx_handle, receiver_channel_settings, rcInfo));

        stack.SetResult_Object(NULL);
        NANOCLR_SET_AND_LEAVE(S_OK);
    }

    numItems = rx_data.num_symbols;

    NANOCLR_CHECK_HRESULT(CreateRmtArrayOnStack(stack, numItems, rmtSymbolTypeDef, &arrayDataPtr));

    pData = rx_data.received_symbols;
    while (numItems-- > 0)
    {
        NANOCLR_CHECK_HRESULT(CreateRmtElement(pData, arrayDataPtr, rmtSymbolTypeDef));
        pData++;
        arrayDataPtr++;
    }

    // Restart receive for next batch of symbols
    NANOCLR_CHECK_HRESULT(StartReceive(rx_handle, receiver_channel_settings, rcInfo));

    NANOCLR_NOCLEANUP();
}

//
//  Blocking Receive()
//
HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ReceiverChannel::
    NativeReceive___SZARRAY_nanoFrameworkHardwareEsp32RmtRmtSymbol(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int64_t *timeoutEvent;
    CLR_RT_HeapBlock hbTimeout;
    bool eventResult = true;
    rmt_rx_done_event_data_t rx_data;
    bool cancelReceive = false;
    rmt_channel_handle_t rx_handle = NULL;
    RmtChannelInfo *rcInfo = nullptr;
    CLR_RT_HeapBlock *receiver_channel_settings;

    // Get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // get a reference to the configs in the managed code instance
    receiver_channel_settings = pThis[FIELD___receiverChannelSettings].Dereference();
    rx_handle = (rmt_channel_handle_t)receiver_channel_settings[RmtChannelSettings::FIELD___handle].NumericByRef().s4;

    // Recall saved receiver channel info by handle
    rcInfo = RmtChannel::FindRegisteredRmtChannel(rx_handle, RmtChannelType::Receive);
    if (rcInfo == nullptr)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    if (rcInfo->isContinuousReceiving || rcInfo->isBlockingReceiving)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    if (stack.m_customState == 0)
    {

        NANOCLR_CHECK_HRESULT(StartReceive(rx_handle, receiver_channel_settings, rcInfo));

        // Timespan in 100ns ticks
        int64_t *timeoutTicks = Library_corlib_native_System_TimeSpan::GetValuePtr(
            receiver_channel_settings[ReceiverChannelSettings::FIELD___receiveTimeout]);

        // setup timeout
        hbTimeout.SetInteger((CLR_INT64)*timeoutTicks);

        rcInfo->isBlockingReceiving = true;
    }

    // Load expire time to timeoutEvent
    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeoutEvent));

    while (eventResult)
    {
        // wait for event, exits with error CLR_E_THREAD_WAITING if no event yet
        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeoutEvent, Event_RmtRx, eventResult));

        if (!eventResult)
        {
            cancelReceive = true;

            // event timeout
            NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
        }

        // Receive event occured
        // Check Message queue for this channel
        // otherwise event from another channel so restart wait
        if (xQueueReceive(rcInfo->rxQueue, &rx_data, 0) == pdTRUE)
        {
            break;
        }

        // loop and repeat wait
    } // while eventResult

    // pop "hbTimeout" heap block from stack
    stack.PopValue();

    // Return RmtSymbol array to user
    // if no symbols return null
    if (rx_data.num_symbols == 0)
    {
        stack.SetResult_Object(NULL);
    }
    else
    {
        CLR_RT_HeapBlock *arrayDataPtr = NULL;
        CLR_RT_TypeDef_Index rmtSymbolTypeDef;

        int numItems = rx_data.num_symbols;

        // Build Header block for Array of RmtSymbol objects on top of stack
        // Returns the rmtSysmbol type and pointer to first data ptr in array
        NANOCLR_CHECK_HRESULT(CreateRmtArrayOnStack(stack, numItems, rmtSymbolTypeDef, &arrayDataPtr));

        rmt_symbol_word_t *pData = rx_data.received_symbols;
        while (numItems-- > 0)
        {
            // Add RMT item to managed array
            NANOCLR_CHECK_HRESULT(CreateRmtElement(pData, arrayDataPtr, rmtSymbolTypeDef));

            // Next item
            pData++;
            arrayDataPtr++;
        }
    }

    NANOCLR_CLEANUP();

    if (rcInfo != nullptr)
    {
        rcInfo->isBlockingReceiving = false;

        if (cancelReceive)
        {
            rmt_disable(rx_handle);

            // discard any completion event queued before the receive was cancelled
            xQueueReset(rcInfo->rxQueue);

            rmt_enable(rx_handle);
        }
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ReceiverChannel::
    CreateRmtElement(rmt_symbol_word_t *rmtItem, CLR_RT_HeapBlock *returnArray, CLR_RT_TypeDef_Index &rmtSymbolTypeDef)
{
    HRESULT hr = S_OK;

    // create a new RmtSymbol object and put it on the return array
    hr = g_CLR_RT_ExecutionEngine.NewObjectFromIndex(*returnArray, rmtSymbolTypeDef);
    if (hr == S_OK)
    {
        // Set each value in the struct from the rmt_symbol_word_t data
        CLR_RT_HeapBlock *dref = returnArray->Dereference();
        CLR_RT_HeapBlock &duration0FieldRef = dref[ManagedRmtSymbol::FIELD___duration0];
        CLR_RT_HeapBlock &level0FieldRef = dref[ManagedRmtSymbol::FIELD___level0];
        CLR_RT_HeapBlock &duration1FieldRef = dref[ManagedRmtSymbol::FIELD___duration1];
        CLR_RT_HeapBlock &level1FieldRef = dref[ManagedRmtSymbol::FIELD___level1];

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
        CLR_RT_TypeDef_Index &rmtSymbolTypeDef,
        CLR_RT_HeapBlock **arrayDataPtr)
{
    HRESULT hr = S_OK;

    if (!g_CLR_RT_TypeSystem.FindTypeDef("RmtSymbol", "nanoFramework.Hardware.Esp32.Rmt", rmtSymbolTypeDef))
    {
        // Type not defined, build problem
        return CLR_E_NOT_SUPPORTED;
    }

    // Create Array on top of stackof type "RmtSymbol"
    hr = CLR_RT_HeapBlock_Array::CreateInstance(stack.PushValue(), numItems, rmtSymbolTypeDef);
    if (hr == S_OK)
    {
        // Get reference to Array
        CLR_RT_HeapBlock_Array *array = stack.TopValue().DereferenceArray();

        //  Get start of array data, array of CLR_RT_HeapBlock * to RmtSymbol structs
        *arrayDataPtr = (CLR_RT_HeapBlock *)array->GetFirstElement();
    }

    return hr;
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ReceiverChannel::
    NativeRxDispose___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    rmt_channel_handle_t handle;
    CLR_RT_HeapBlock *receiver_channel_settings;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // get a reference to the configs in the managed code instance
    receiver_channel_settings = pThis[FIELD___receiverChannelSettings].Dereference();
    handle = (rmt_channel_handle_t)receiver_channel_settings[RmtChannelSettings::FIELD___handle].NumericByRef().s4;

    if (!RmtChannel::CheckChannel(handle))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    RmtChannel::EraseChannel(handle);

    NANOCLR_NOCLEANUP();
}
