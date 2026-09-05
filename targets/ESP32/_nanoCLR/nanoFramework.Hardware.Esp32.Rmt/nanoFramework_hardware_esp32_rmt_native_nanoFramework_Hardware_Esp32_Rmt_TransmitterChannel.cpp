//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <cstring>
#include "nanoFramework_hardware_esp32_rmt_native.h"

// reduce line lengths
typedef Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_EncoderData EncoderData;
typedef Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_EncoderSettings
    EncoderSettings;
typedef Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ByteEncoderSettings
    ByteEncoderSettings;
typedef Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_CopyEncoderSettings
    CopyEncoderSettings;
typedef Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtSymbols RmtSymbols;
typedef Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtSymbol RmtSymbol;
typedef Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtChannel RmtChannel;
typedef Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtChannelSettings
    RmtChannelSettings;
typedef Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitChannelSettings
    TransmitterChannelSettings;

HRESULT TXSetCarrier(rmt_channel_handle_t tx_chan, CLR_RT_HeapBlock *transmitter_channel_settings)
{
    NANOCLR_HEADER();

    // Enable carrier ?
    if ((bool)transmitter_channel_settings[TransmitterChannelSettings::FIELD___enableCarrierWave].NumericByRef().u1)
    {
        rmt_carrier_config_t carrier_cfg = {};

        carrier_cfg.frequency_hz =
            transmitter_channel_settings[TransmitterChannelSettings::FIELD___carrierWaveFrequency].NumericByRef().u4;

        carrier_cfg.duty_cycle =
            transmitter_channel_settings[TransmitterChannelSettings::FIELD___carrierWaveDutyPercentage]
                .NumericByRef()
                .r4;
        // scale to 0 -> 1.0
        carrier_cfg.duty_cycle /= 100;

        bool carrier_level =
            (bool)transmitter_channel_settings[TransmitterChannelSettings::FIELD___carrierLevel].NumericByRef().u1;
        carrier_cfg.flags.polarity_active_low = carrier_level ? 0 : 1;

        carrier_cfg.flags.always_on =
            transmitter_channel_settings[TransmitterChannelSettings::FIELD___carrierAlwaysOn].NumericByRef().u1;

        auto err = rmt_apply_carrier(tx_chan, &carrier_cfg);
        if (err != ESP_OK)
        {
            if (err == ESP_ERR_INVALID_ARG)
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER)
            else
                NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
        }
    }
    else
    {
        // Disable (ignore error)
        rmt_apply_carrier(tx_chan, NULL);
    }

    NANOCLR_NOCLEANUP();
}

inline void ManagedSymbolToNative(CLR_RT_HeapBlock *rmt_symbol, rmt_symbol_word_t *nativeRmtSymbol)
{
    nativeRmtSymbol->duration0 = rmt_symbol[RmtSymbol::FIELD___duration0].NumericByRef().u2;
    nativeRmtSymbol->duration1 = rmt_symbol[RmtSymbol::FIELD___duration1].NumericByRef().u2;
    nativeRmtSymbol->level0 = rmt_symbol[RmtSymbol::FIELD___level0].NumericByRef().u2;
    nativeRmtSymbol->level1 = rmt_symbol[RmtSymbol::FIELD___level1].NumericByRef().u2;
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtSymbols::
    NativeRmtSymbolsToBytes___SZARRAY_U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        CLR_RT_HeapBlock *symbolsArrayList;
        CLR_RT_HeapBlock_Array *symbolsArray;
        CLR_RT_HeapBlock_Array *symbol;
        CLR_RT_HeapBlock_Array *nativeArray;
        rmt_symbol_word_t *rmtBuffer;

        int symbolsCount;
        int symbolsListCapacity;

        symbolsArrayList = &pThis[FIELD___symbols];
        FAULT_ON_NULL(symbolsArrayList);

        NANOCLR_CHECK_HRESULT(CLR_RT_ArrayListHelper::ExtractArrayFromArrayList(
            *symbolsArrayList,
            symbolsArray,
            symbolsCount,
            symbolsListCapacity));

        // create target byte array on stack
        CLR_RT_HeapBlock &top = stack.PushValueAndClear();
        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(
            top,
            symbolsCount * sizeof(rmt_symbol_word_t),
            g_CLR_RT_WellKnownTypes.m_UInt8));

        nativeArray = top.DereferenceArray();
        rmtBuffer = (rmt_symbol_word_t *)nativeArray->GetFirstElement();

        for (int i = 0; i < symbolsCount; i++)
        {
            symbol = ((CLR_RT_HeapBlock_Array *)symbolsArray->GetElement(i))->DereferenceArray();
            if (symbol != NULL)
            {
                ManagedSymbolToNative(symbol, rmtBuffer);
                rmtBuffer++;
            }
        }
    }
    NANOCLR_NOCLEANUP();
}

inline HRESULT ManagedByteEncoderToNative(
    CLR_RT_HeapBlock *byteEncoderSettings,
    rmt_bytes_encoder_config_t &encoder_config)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *rmt_symbol_settings;

    rmt_symbol_settings = byteEncoderSettings[ByteEncoderSettings::FIELD___bit0].Dereference();
    FAULT_ON_NULL(rmt_symbol_settings);
    ManagedSymbolToNative(rmt_symbol_settings, &encoder_config.bit0);

    rmt_symbol_settings = byteEncoderSettings[ByteEncoderSettings::FIELD___bit1].Dereference();
    FAULT_ON_NULL(rmt_symbol_settings);
    ManagedSymbolToNative(rmt_symbol_settings, &encoder_config.bit1);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterEncodedChannel::
    NativeTxInit___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *transmitter_channel_settings = NULL;
        CLR_RT_HeapBlock *encoder_settings = NULL;
        rmt_channel_handle_t tx_chan = NULL;
        int32_t pin_number;
        rmt_encoder_handle_t encoder_handle;

        int numEncoders;
        int encoderType;
        CLR_RT_HeapBlock_Array *encoderArray;

        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // get a reference to the configs in the managed code instance
        transmitter_channel_settings = pThis[FIELD___transmitterChannelSettings].Dereference();

        pin_number = transmitter_channel_settings[RmtChannelSettings::FIELD___pinNumber].NumericByRef().s4;

        rmt_tx_channel_config_t rmt_tx_config = rmt_tx_channel_config_t();

        rmt_tx_config.clk_src = RMT_CLK_SRC_DEFAULT;
        rmt_tx_config.gpio_num = (gpio_num_t)pin_number;

        rmt_tx_config.resolution_hz =
            transmitter_channel_settings[RmtChannelSettings::FIELD___resolutionHz].NumericByRef().u4;

        rmt_tx_config.mem_block_symbols =
            (size_t)transmitter_channel_settings[RmtChannelSettings::FIELD___numberOfMemoryBlocks].NumericByRef().u1;

        // Multiply by number of RMT symbols per block based on target esp32 type
        rmt_tx_config.mem_block_symbols *= CONFIG_SOC_RMT_MEM_WORDS_PER_CHANNEL;

        rmt_tx_config.trans_queue_depth = 4;

        rmt_tx_config.flags.invert_out =
            (bool)transmitter_channel_settings[RmtChannelSettings::FIELD___signalInverterEnabled].NumericByRef().u1;

        auto err = rmt_new_tx_channel(&rmt_tx_config, &tx_chan);
        if (err != ESP_OK)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_DRIVER_NOT_REGISTERED);
        }

        // Enable carrier ?
        NANOCLR_CHECK_HRESULT(TXSetCarrier(tx_chan, transmitter_channel_settings));

        // Default encoder type (copy)
        encoderType = EncoderType::EncoderType_None;

        encoderArray = pThis[FIELD___encoderSettings].DereferenceArray();
        FAULT_ON_NULL(encoderArray);

        numEncoders = encoderArray->m_numOfElements;
        if (numEncoders == 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        NANOCLR_CHECK_HRESULT(
            RmtChannel::RmtMapEspErrToClrErr(rmt_new_multi_stage_encoder(&encoder_handle, numEncoders)));

        for (int index = 0; index < numEncoders; index++)
        {
            CLR_RT_HeapBlock *encoder = ((CLR_RT_HeapBlock *)encoderArray->GetElement(index));

            // Get EncoderSettings reference
            encoder_settings = encoder->Dereference();
            FAULT_ON_NULL(encoder_settings);

            encoderType = encoder_settings[EncoderSettings::FIELD___encoderType].NumericByRef().u1;

            switch (encoderType)
            {
                case EncoderType_Byte:
                {
                    rmt_bytes_encoder_config_t encoder_config = {};

                    NANOCLR_CHECK_HRESULT(ManagedByteEncoderToNative(encoder_settings, encoder_config));

                    encoder_config.flags.msb_first =
                        (bool)encoder_settings[ByteEncoderSettings::FIELD___msbFirst].NumericByRef().u1;

                    err = rmt_add_byte_encoder(encoder_handle, 1, encoder_config);
                    NANOCLR_CHECK_HRESULT(RmtChannel::RmtMapEspErrToClrErr(err));
                }
                break;

                case EncoderType_Copy:
                {
                    uint symbolsByteLen = 0;
                    rmt_symbol_word_t *symbols = NULL;
                    rmt_copy_encoder_config_t encoder_config = {};

                    CLR_RT_HeapBlock *rmtSymbolsObj =
                        encoder_settings[CopyEncoderSettings::FIELD___symbols].Dereference();
                    if (rmtSymbolsObj != NULL)
                    {
                        CLR_RT_HeapBlock_Array *rmtByteSymbols =
                            rmtSymbolsObj[RmtSymbols::FIELD___nativeSymbolsBuffer].DereferenceArray();
                        FAULT_ON_NULL(rmtByteSymbols);
                        symbols = (rmt_symbol_word_t *)rmtByteSymbols->GetFirstElement();
                        symbolsByteLen = rmtByteSymbols->m_numOfElements;
                    }

                    NANOCLR_CHECK_HRESULT(RmtChannel::RmtMapEspErrToClrErr(
                        rmt_add_copy_encoder(encoder_handle, 1, encoder_config, symbols, symbolsByteLen)));
                }
                break;

                default:
                    NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
            }
        }

        // Enable channel, ignore any error, as it may already be enabled
        rmt_enable(tx_chan);

        RmtChannel::AddRegisteredTxChannel(tx_chan, 0, encoder_handle);

        // Make sure handles are disposed if system restarted(Debugging)
        HAL_AddSoftRebootHandler(RmtChannel::UninitializeAll);

        // Return channel handle
        stack.SetResult_I4((CLR_INT32)tx_chan);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterEncodedChannel::
    NativeTxGetIsChannelIdle___BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *transmitter_channel_settings = NULL;
    rmt_channel_handle_t handle;
    bool retVal = false;
    esp_err_t err;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // get a reference to the configs in the managed code instance
    transmitter_channel_settings = pThis[FIELD___transmitterChannelSettings].Dereference();
    handle = (rmt_channel_handle_t)transmitter_channel_settings[RmtChannelSettings::FIELD___handle].NumericByRef().s4;

    if (!RmtChannel::CheckChannel(handle))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    err = rmt_tx_wait_all_done((rmt_channel_handle_t)handle, portMAX_DELAY);
    if (err == ESP_OK)
    {
        // All transactions flushed so channel must be idle
        retVal = true;
    }

    stack.SetResult_Boolean(retVal);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterEncodedChannel::
    NativeTxSetCarrierMode___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        rmt_channel_handle_t handle;

        CLR_RT_HeapBlock *transmitter_channel_settings = NULL;

        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // get a reference to the configs in the managed code instance
        transmitter_channel_settings = pThis[FIELD___transmitterChannelSettings].Dereference();
        handle =
            (rmt_channel_handle_t)transmitter_channel_settings[RmtChannelSettings::FIELD___handle].NumericByRef().s4;

        if (!RmtChannel::CheckChannel(handle))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        NANOCLR_CHECK_HRESULT(TXSetCarrier((rmt_channel_handle_t)handle, transmitter_channel_settings));
    }
    NANOCLR_NOCLEANUP();
}

// Common function to transmit data, used by both TxWriteSymbolItems and TxWriteEncoder
void SetupTxConfig(CLR_RT_HeapBlock *transmitter_channel_settings, rmt_transmit_config_t &txConfig)
{
    txConfig = {};
    txConfig.flags.eot_level =
        (bool)transmitter_channel_settings[TransmitterChannelSettings::FIELD___idleLevel].NumericByRef().u1;
    txConfig.flags.queue_nonblocking = false;

#if SOC_RMT_SUPPORT_TX_LOOP_COUNT
    if ((bool)transmitter_channel_settings[TransmitterChannelSettings::FIELD___enableLooping].NumericByRef().u1)
    {
        txConfig.loop_count =
            transmitter_channel_settings[TransmitterChannelSettings::FIELD___loopCount].NumericByRef().s4;
    }
#endif
}

static bool WaitForTxDone(rmt_channel_handle_t chanHandle)
{
    return (rmt_tx_wait_all_done((rmt_channel_handle_t)chanHandle, portMAX_DELAY) == ESP_OK);
}

// Common function to transmit data
HRESULT TransmitRmtChannel(
    rmt_transmit_config_t &txConfig,
    rmt_channel_handle_t chanHandle,
    rmt_encoder_handle_t encoder,
    bool wait_tx_done)
{
    NANOCLR_HEADER();

    esp_err_t err = rmt_transmit(
        (rmt_channel_handle_t)chanHandle,
        encoder,
        (const void *)1, // note: payload can't be 0 although we are not using it
        1,
        &txConfig);

    if (err != ESP_OK)
    {
        if (err == ESP_ERR_INVALID_STATE)
            hr = CLR_E_INVALID_PARAMETER;
        else
            hr = CLR_E_INVALID_OPERATION;

        NANOCLR_LEAVE();
    }

    if (wait_tx_done)
    {
        if (!WaitForTxDone(chanHandle))
        {
            hr = CLR_E_INVALID_PARAMETER;
            NANOCLR_LEAVE();
        }
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterEncodedChannel::
    NativeTxWriteSymbolItems___U4__SZARRAY_U1__BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *transmitter_channel_settings = NULL;
        rmt_transmit_config_t txConfig;
        rmt_channel_handle_t chanHandle;
        rmt_multi_stage_encoder_t *multiEncoder;
        bool wait_tx_done;

        CLR_RT_HeapBlock_Array *data = NULL; // managed buffer passed in
        char *buffer = NULL;
        uint32_t bufferLength = 0;

        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // get a reference to the configs in the managed code instance
        transmitter_channel_settings = pThis[FIELD___transmitterChannelSettings].Dereference();
        chanHandle =
            (rmt_channel_handle_t)transmitter_channel_settings[RmtChannelSettings::FIELD___handle].NumericByRef().u4;
        wait_tx_done = (bool)stack.Arg2().NumericByRef().u1;

        RmtChannelInfo *tci = RmtChannel::FindRegisteredRmtChannel(chanHandle, RmtChannelType::Transmit);
        if (tci == nullptr)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        // Get encoder handle for channel
        multiEncoder = __containerof(tci->txEncoder, rmt_multi_stage_encoder_t, base);

        // Validate that only 1 encoder is configured and that it is a copy encoder
         if (
            multiEncoder->numberSteps != 1 ||
            multiEncoder->encoders == NULL ||
            multiEncoder->encoders[0].encType != RmtEncoderType_Copy)
        {
            // Only 1 encoder is supported for this function
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        // Get tx byte buffer address & size
        data = stack.Arg1().DereferenceArray();
        FAULT_ON_NULL(data);
        buffer = (char *)data->GetFirstElement();
        bufferLength = data->m_numOfElements;

        // Check that buffer length is a multiple of symbol size
        if (bufferLength % sizeof(rmt_symbol_word_t) != 0)
        {
            hr = CLR_E_SERIALIZATION_VIOLATION;
            NANOCLR_LEAVE();
        }

        {
            // Make sure any previous calls are completed before we start a new one, 
            // as we are going to overwrite the buffer.
            if (!WaitForTxDone(chanHandle))
            {
                hr = CLR_E_INVALID_PARAMETER;
                NANOCLR_LEAVE();
            }

            // Copy data to vector buffer
            auto &d = tci->txBuffer;

            d.assign(bufferLength, 0);
            std::memcpy(d.data(), buffer, bufferLength);

            // Set first encoder(copy) with data buffer, there should only be 1 encoder
            multiEncoder->encoders[0].data = (rmt_symbol_word_t *)d.data();
            multiEncoder->encoders[0].dataLen = bufferLength;

            // Set up Tx config
            SetupTxConfig(transmitter_channel_settings, txConfig);

            // Reset encoder before we start
            rmt_encoder_reset((rmt_encoder_handle_t)multiEncoder);

            // Reset encoder & RMT transmit
            NANOCLR_CHECK_HRESULT(
                TransmitRmtChannel(txConfig, chanHandle, (rmt_encoder_handle_t)multiEncoder, wait_tx_done));
        }

        stack.SetResult_U4(0); // return value not used
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterEncodedChannel::
    NativeTxWriteEncoder___U4__BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *transmitter_channel_settings = NULL;
        rmt_transmit_config_t txConfig;
        rmt_channel_handle_t chanHandle;
        rmt_multi_stage_encoder_t *multiEncoder;
        bool wait_tx_done;

        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // get a reference to the configs in the managed code instance
        transmitter_channel_settings = pThis[FIELD___transmitterChannelSettings].Dereference();
        chanHandle =
            (rmt_channel_handle_t)transmitter_channel_settings[RmtChannelSettings::FIELD___handle].NumericByRef().u4;
        wait_tx_done = (bool)stack.Arg1().NumericByRef().u1;

        RmtChannelInfo *tci = RmtChannel::FindRegisteredRmtChannel(chanHandle, RmtChannelType::Transmit);
        if (tci == nullptr)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        multiEncoder = __containerof(tci->txEncoder, rmt_multi_stage_encoder_t, base);

        CLR_RT_HeapBlock_Array *encoderDataArray = pThis[FIELD___encoderData].DereferenceArray();
        FAULT_ON_NULL(encoderDataArray);

        // Get number of encoderData objects
        uint32_t numEncoderData = encoderDataArray->m_numOfElements;

        // Loop through encoderData objects and set data for each encoder step that requires data
        // copy encoder which already contain data will be skipped
        int encDataIndex = 0;

        // For each encoder
        for (int encIndex = 0; encIndex < multiEncoder->numberSteps; encIndex++)
        {
            // Ptr to current encoder index
            rmt_multi_item_t *mstep = multiEncoder->encoders + encIndex;
            int dataLen = 0;
            uint8_t *data = NULL;

            // Check we have encoder data for next encoder step
            if (encDataIndex >= numEncoderData)
            {
                // no more data so stop here
                break;
            }

            // Does encoder step require data ?
            if (mstep->data != NULL)
            {
                // Next encoder
                continue;
            }

            // Get next encoderData object
            CLR_RT_HeapBlock *arrayItem = (CLR_RT_HeapBlock *)encoderDataArray->GetElement(encDataIndex);
            FAULT_ON_NULL(arrayItem);
            CLR_RT_HeapBlock *encoderData = arrayItem->Dereference();

            mstep->loopCount = encoderData[EncoderData::FIELD___loop].NumericByRef().s4;
            CLR_RT_HeapBlock_Array *dataBuffer = encoderData[EncoderData::FIELD___data].DereferenceArray();
            FAULT_ON_NULL(dataBuffer);  
 
            data = dataBuffer->GetFirstElement();
            dataLen = dataBuffer->m_numOfElements;

            // Validate data length for copy encoders
            if (mstep->encType == RmtEncoderType_Copy)
            {
                if ((dataLen % sizeof(rmt_symbol_word_t)) != 0)
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                }
            }

            // Set encoder data
            mstep->data = data;
            mstep->dataLen = dataLen;

            encDataIndex++;
        } // for

        // Set up Tx config
        SetupTxConfig(transmitter_channel_settings, txConfig);

        // Reset encoder before we start
        rmt_encoder_reset((rmt_encoder_handle_t)multiEncoder);

        // Reset encoder and start RMT transmit
        NANOCLR_CHECK_HRESULT(
            TransmitRmtChannel(txConfig, chanHandle, (rmt_encoder_handle_t)multiEncoder, wait_tx_done));

        stack.SetResult_U4(0);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterEncodedChannel::
    NativeTxDispose___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *transmitter_channel_settings = NULL;
        rmt_channel_handle_t handle;

        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // get a reference to the configs in the managed code instance
        transmitter_channel_settings = pThis[FIELD___transmitterChannelSettings].Dereference();
        handle =
            (rmt_channel_handle_t)transmitter_channel_settings[RmtChannelSettings::FIELD___handle].NumericByRef().u4;

        RmtChannel::EraseChannel(handle);
    }
    NANOCLR_NOCLEANUP();
}
