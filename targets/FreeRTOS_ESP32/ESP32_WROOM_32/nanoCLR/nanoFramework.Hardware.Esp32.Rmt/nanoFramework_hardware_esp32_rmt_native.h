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

#include <rmt.h>

// Reduce line lengths
#define ManagedRmtCommand Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtCommand
#define RmtChannel        Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtChannel
#define CHANNEL(channel)  static_cast<rmt_channel_t>(channel)

typedef enum __nfpack SourceClock
{
    SourceClock_APB = 0,
    SourceClock_REF = 1,
} SourceClock;

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
    static const int FIELD___receiveTimeout = 4;

    NANOCLR_NATIVE_DECLARE(NativeRxInit___I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(NativeRxStart___VOID__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeRxStop___VOID);
    NANOCLR_NATIVE_DECLARE(NativeRxGetRingBufferCount___I4);
    NANOCLR_NATIVE_DECLARE(NativeRxGetAllItems___SZARRAY_nanoFrameworkHardwareEsp32RmtRmtCommand);
    NANOCLR_NATIVE_DECLARE(NativeRxEnableFilter___VOID__BOOLEAN__U1);
    NANOCLR_NATIVE_DECLARE(NativeRxSetIdleThresold___VOID__U2);
    NANOCLR_NATIVE_DECLARE(NativeRxDispose___VOID);

    //--//

    static HRESULT CreateRmtArrayOnStack(
        CLR_RT_StackFrame &stack,
        CLR_INT32 numItems,
        CLR_RT_TypeDef_Index &rmtCommandTypeDef,
        CLR_RT_HeapBlock **arrayDataPtr);
    static HRESULT CreateRmtElement(
        rmt_item32_t *rmtItem,
        CLR_RT_HeapBlock *returnArray,
        CLR_RT_TypeDef_Index &rmtCommandTypeDef);

    static esp_err_t InitRxChannel(rmt_channel_t channel, gpio_num_t gpio, int32_t rmtBufferSize, int32_t clockDiv);
};

struct Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtChannel
{
    static const int FIELD___channel = 1;
    static const int FIELD___clockDivider = 2;
    static const int FIELD___sourceClock = 3;

    NANOCLR_NATIVE_DECLARE(NativeSetClockDivider___VOID__U1);

    //--//

    static std::map<rmt_channel_t, std::vector<rmt_item32_t>> registredChannels;

    static bool CheckChannel(int channel);
    static CLR_INT32 FindNextChannel();
};

struct Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel
{
    static const int FIELD___carrierEnabled = 4;
    static const int FIELD___carrierHighDuration = 5;
    static const int FIELD___carrierLowDuration = 6;
    static const int FIELD___carrierLevel = 7;
    static const int FIELD___commands = 8;

    NANOCLR_NATIVE_DECLARE(NativeInit___VOID__I4);
    NANOCLR_NATIVE_DECLARE(NativeGetIdleLevel___BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeGetIsChannelIdle___BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeSetIsChannelIdle___VOID__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeSetIdleLevel___VOID__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeSetCarrierMode___VOID);
    NANOCLR_NATIVE_DECLARE(NativeWriteItems___U4__SZARRAY_U1__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeWaitTxDone___U4__I4);
    NANOCLR_NATIVE_DECLARE(NativeDispose___VOID);
    NANOCLR_NATIVE_DECLARE(NativeGetSourceClock___BOOLEAN);

    //--//

    static esp_err_t InitTxChannel(rmt_channel_t channel, gpio_num_t gpio);
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Hardware_Esp32_Rmt;

#endif //_NANOFRAMEWORK_HARDWARE_ESP32_RMT_NATIVE_H_
