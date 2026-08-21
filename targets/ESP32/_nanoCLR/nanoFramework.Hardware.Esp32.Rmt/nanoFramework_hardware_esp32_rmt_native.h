//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _NANOFRAMEWORK_HARDWARE_ESP32_RMT_NATIVE_H_
#define _NANOFRAMEWORK_HARDWARE_ESP32_RMT_NATIVE_H_

#include <map>
#include <vector>

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>
#include <corlib_native.h>

#include <driver/rmt_tx.h>
#include <driver/rmt_rx.h>
#include <driver/rmt_encoder.h>
#include "rmt_multi_stage_encoder.h"
#include "esp32_rmt_native.h"

typedef enum __nfpack EncoderType
{
    EncoderType_None = 0,
    EncoderType_Copy = 1,
    EncoderType_Byte = 2,
} EncoderType;

typedef enum __nfpack LedType
{
    LedType_WS2811 = 0,
    LedType_WS2812 = 1,
    LedType_WS2813 = 2,
    LedType_WS2815 = 3,
    LedType_SK2812 = 4,
} LedType;

struct Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ByteEncoderSettings
{
    static const int FIELD___bit0 = 2;
    static const int FIELD___bit1 = 3;
    static const int FIELD___msbFirst = 4;
    static const int FIELD___byteLen = 5;

    //--//
};

struct Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtSymbol
{
    static const int FIELD___level0 = 1;
    static const int FIELD___level1 = 2;
    static const int FIELD___duration0 = 3;
    static const int FIELD___duration1 = 4;

    //--//
};

struct Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_CopyEncoderSettings
{
    static const int FIELD___symbols = 2;

    //--//
};

struct Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_EncoderData
{
    static const int FIELD___data = 1;
    static const int FIELD___loop = 2;

    //--//
};

struct Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_EncoderSettings
{
    static const int FIELD___encoderType = 1;

    //--//
};

struct Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtChannelSettings
{
    static const int FIELD___handle = 1;
    static const int FIELD___pinNumber = 2;
    static const int FIELD___resolutionHz = 3;
    static const int FIELD___numberOfMemoryBlocks = 4;
    static const int FIELD___signalInverterEnabled = 5;

    //--//
};

struct Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitChannelSettings
{
    static const int FIELD___enableCarrierWave = 6;
    static const int FIELD___carrierLevel = 7;
    static const int FIELD___carrierWaveFrequency = 8;
    static const int FIELD___carrierWaveDutyPercentage = 9;
    static const int FIELD___carrierAlwaysOn = 10;
    static const int FIELD___enableLooping = 11;
    static const int FIELD___loopCount = 12;
    static const int FIELD___enableIdleLevelOutput = 13;
    static const int FIELD___idleLevel = 14;

    //--//
};

struct Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_LedTransmitChannel
{
    static const int FIELD___transmitChannel = 1;
    static const int FIELD__disposedValue = 2;

    //--//
};

struct Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ReceiverChannel
{
    static const int FIELD___receiverChannelSettings = 2;

    NANOCLR_NATIVE_DECLARE(NativeRxInit___I4);
    NANOCLR_NATIVE_DECLARE(NativeReceive___SZARRAY_nanoFrameworkHardwareEsp32RmtRmtSymbol);
    NANOCLR_NATIVE_DECLARE(NativeTryGetReceived___SZARRAY_nanoFrameworkHardwareEsp32RmtRmtSymbol);
    NANOCLR_NATIVE_DECLARE(NativeRxDispose___VOID);
    NANOCLR_NATIVE_DECLARE(NativeStartReceive___VOID);
    NANOCLR_NATIVE_DECLARE(NativeStopReceive___VOID);

    //--//

    static HRESULT CreateRmtArrayOnStack(
        CLR_RT_StackFrame &stack,
        CLR_INT32 numItems,
        CLR_RT_TypeDef_Index &rmtSymbolTypeDef,
        CLR_RT_HeapBlock **arrayDataPtr);

    static HRESULT CreateRmtElement(
        rmt_symbol_word_t *rmtItem,
        CLR_RT_HeapBlock *returnArray,
        CLR_RT_TypeDef_Index &rmtSymbolTypeDef);
};

struct Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ReceiverChannelSettings
{
    static const int FIELD___idleThreshold = 6;
    static const int FIELD___filterThreshold = 7;
    static const int FIELD___receiveTimeout = 8;
    static const int FIELD___enableDemodulation = 9;
    static const int FIELD___carrierWaveFrequency = 10;
    static const int FIELD___carrierWaveDutyPercentage = 11;
    static const int FIELD___carrierLevel = 12;
    static const int FIELD___bufferSize = 13;

    //--//
};

struct Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtChannel
{
    static const int FIELD___settings = 1;

    //--//

    static std::map<rmt_channel_handle_t, RmtChannelInfo> registeredRmtChannels;

    static void UninitializeAll();
    static void AddRegisteredTxChannel(rmt_channel_handle_t h, int bufferSize, rmt_encoder_handle_t encoder);
    static void AddRegisteredRxChannel(
        rmt_channel_handle_t h,
        QueueHandle_t receive_queue,
        uint8_t *buffer,
        CLR_INT32 bufferSize);

    static RmtChannelInfo *FindRegisteredRmtChannel(rmt_channel_handle_t handle, RmtChannelType channelType);

    static bool CheckChannel(rmt_channel_handle_t channel);
    static void EraseChannel(rmt_channel_handle_t h);

    static HRESULT RmtMapEspErrToClrErr(esp_err_t err);
};

struct Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtSymbols
{
    static const int FIELD___symbols = 1;
    static const int FIELD___nativeSymbolsBuffer = 2;

    NANOCLR_NATIVE_DECLARE(NativeRmtSymbolsToBytes___SZARRAY_U1);

    //--//
};

struct Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtUtils
{
    NANOCLR_NATIVE_DECLARE(
        DecodeSymbolArrayToBytes___STATIC__SZARRAY_U1__SZARRAY_nanoFrameworkHardwareEsp32RmtRmtSymbol__I4__BOOLEAN__I4__I4__BOOLEAN);

    //--//
};

struct Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitSyncManager
{
    static const int FIELD__disposedValue = 1;
    static const int FIELD___txChannelArray = 2;

    NANOCLR_NATIVE_DECLARE(NativeCreateSyncManager___VOID__SZARRAY_I4);
    NANOCLR_NATIVE_DECLARE(NativeDisposeSyncManager___VOID);
    NANOCLR_NATIVE_DECLARE(NativeResetSyncManager___VOID);

    //--//
};

struct Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel
{
    static const int FIELD___disposed = 5;

    //--//
};

struct Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterEncodedChannel
{
    static const int FIELD___transmitterChannelSettings = 2;
    static const int FIELD___encoderSettings = 3;
    static const int FIELD___encoderData = 4;

    NANOCLR_NATIVE_DECLARE(NativeTxInit___I4);
    NANOCLR_NATIVE_DECLARE(NativeTxGetIsChannelIdle___BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeTxSetCarrierMode___VOID);
    NANOCLR_NATIVE_DECLARE(NativeTxWriteSymbolItems___U4__SZARRAY_U1__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeTxWriteEncoder___U4__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeTxDispose___VOID);

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Hardware_Esp32_Rmt;

#endif //_NANOFRAMEWORK_HARDWARE_ESP32_RMT_NATIVE_H_
