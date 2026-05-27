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

#include <driver/rmt.h>

// Reduce line lengths
#define CHANNEL(channel) static_cast<rmt_channel_t>(channel)

typedef enum __nfpack ChannelMode
{
    ChannelMode_Receive = 0,
    ChannelMode_Transmit = 1,
} ChannelMode;

typedef enum __nfpack SourceClock
{
    SourceClock_APB = 0,
    SourceClock_REF = 1,
} SourceClock;

struct Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtChannelSettings
{
    static const int FIELD___channel = 1;
    static const int FIELD___pinNumber = 2;
    static const int FIELD___clockDivider = 3;
    static const int FIELD___numberOfMemoryBlocks = 4;
    static const int FIELD___bufferSize = 5;
    static const int FIELD___signalInverterEnabled = 6;

    //--//
};

struct Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtCommand
{
    static const int FIELD___level0 = 1;
    static const int FIELD___level1 = 2;
    static const int FIELD___duration0 = 3;
    static const int FIELD___duration1 = 4;

    //--//
};

struct Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ReceiverChannel
{
    static const int FIELD___receiverChannelSettings = 2;

    NANOCLR_NATIVE_DECLARE(NativeRxInit___I4);
    NANOCLR_NATIVE_DECLARE(NativeRxStart___VOID__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeRxStop___VOID);
    NANOCLR_NATIVE_DECLARE(NativeRxGetRingBufferCount___I4);
    NANOCLR_NATIVE_DECLARE(NativeRxGetAllItems___SZARRAY_nanoFrameworkHardwareEsp32RmtRmtCommand);
    NANOCLR_NATIVE_DECLARE(NativeRxEnableFilter___VOID__BOOLEAN__U1);
    NANOCLR_NATIVE_DECLARE(NativeRxSetIdleThresold___VOID__U2);
    NANOCLR_NATIVE_DECLARE(NativeRxDispose___VOID);

    //--//

    static rmt_config_t GetNewRmtRxConfig(gpio_num_t pin, rmt_channel_t channel);

    static HRESULT CreateRmtArrayOnStack(
        CLR_RT_StackFrame &stack,
        CLR_INT32 numItems,
        CLR_RT_TypeDef_Index &rmtCommandTypeDef,
        CLR_RT_HeapBlock **arrayDataPtr);

    static HRESULT CreateRmtElement(
        rmt_item32_t *rmtItem,
        CLR_RT_HeapBlock *returnArray,
        CLR_RT_TypeDef_Index &rmtCommandTypeDef);
};

struct Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_ReceiverChannelSettings
{
    static const int FIELD___idleThreshold = 7;
    static const int FIELD___enableFilter = 8;
    static const int FIELD___filterThreshold = 9;
    static const int FIELD___receiveTimeout = 10;
    static const int FIELD___enableDemodulation = 11;
    static const int FIELD___carrierWaveFrequency = 12;
    static const int FIELD___carrierWaveDutyPercentage = 13;
    static const int FIELD___carrierLevel = 14;

    //--//
};

struct Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtChannel
{
    static const int FIELD___settings = 1;

    NANOCLR_NATIVE_DECLARE(NativeSetGpioPin___VOID__I4__U1__I4__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeSetClockDivider___VOID__U1);
    NANOCLR_NATIVE_DECLARE(NativeSetNumberOfMemoryBlocks___VOID__U1);
    NANOCLR_NATIVE_DECLARE(NativeGetSourceClockFrequency___STATIC__I4);

    //--//

    static std::map<rmt_channel_t, std::vector<rmt_item32_t>> registredChannels;

    static bool CheckChannel(int channel);
    static CLR_INT32 FindNextChannel();
};

struct Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitChannelSettings
{
    static const int FIELD___enableCarrierWave = 7;
    static const int FIELD___carrierLevel = 8;
    static const int FIELD___carrierWaveFrequency = 9;
    static const int FIELD___carrierWaveDutyPercentage = 10;
    static const int FIELD___enableLooping = 11;
    static const int FIELD___loopCount = 12;
    static const int FIELD___enableIdleLevelOutput = 13;
    static const int FIELD___idleLevel = 14;

    //--//
};

struct Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel
{
    static const int FIELD___transmitterChannelSettings = 2;
    static const int FIELD___commands = 3;

    NANOCLR_NATIVE_DECLARE(NativeTxInit___I4);
    NANOCLR_NATIVE_DECLARE(NativeTxGetIsChannelIdle___BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeTxSetLoopingMode___VOID__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeTxSetLoopCount___VOID__I4);
    NANOCLR_NATIVE_DECLARE(NativeTxSetCarrierMode___VOID);
    NANOCLR_NATIVE_DECLARE(NativeTxSetIdleLevel___VOID__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeTxWriteItems___U4__SZARRAY_U1__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeTxDispose___VOID);

    //--//

    static rmt_config_t GetNewRmtTxConfig(gpio_num_t pin, rmt_channel_t channel);
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Hardware_Esp32_Rmt;

#endif //_NANOFRAMEWORK_HARDWARE_ESP32_RMT_NATIVE_H_
