//
// Copyright (c) 2020 The nanoFramework project contributors
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

typedef enum __nfpack SourceClock
{
    SourceClock_APB = 0,
    SourceClock_REF = 1,
} SourceClock;

struct Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtCommand
{
    static const int FIELD___level0    = 1;
    static const int FIELD___level1    = 2;
    static const int FIELD___duration0 = 3;
    static const int FIELD___duration1 = 4;

    //--//
};

struct Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel
{
    static const int FIELD___channel             = 1;
    static const int FIELD___carrierEnabled      = 2;
    static const int FIELD___carrierHighDuration = 3;
    static const int FIELD___carrierLowDuration  = 4;
    static const int FIELD___carrierLevel        = 5;
    static const int FIELD___clockDivider        = 6;
    static const int FIELD___sourceClock         = 7;
    static const int FIELD___commands            = 8;

    NANOCLR_NATIVE_DECLARE(NativeInit___STATIC__VOID__I4);
    NANOCLR_NATIVE_DECLARE(NativeGetIdleLevel___STATIC__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeGetIsChannelIdle___STATIC__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeSetIsChannelIdle___STATIC__VOID__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeSetIdleLevel___STATIC__VOID__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeSetCarrierMode___STATIC__VOID);
    NANOCLR_NATIVE_DECLARE(NativeWriteItems___STATIC__U4__SZARRAY_U1__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeWaitTxDone___STATIC__U4__I4);
    NANOCLR_NATIVE_DECLARE(NativeDispose___STATIC__VOID);
    NANOCLR_NATIVE_DECLARE(NativeGetClockDivider___STATIC__U1);
    NANOCLR_NATIVE_DECLARE(NativeGetSourceClock___STATIC__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeSetClockDivider___STATIC__VOID__U1);

    //--//

    static std::map<rmt_channel_t, std::vector<rmt_item32_t>> registredChannels;

    static CLR_INT32 FindNextChannel();
    static esp_err_t InitChannel(rmt_channel_t channel, gpio_num_t gpio);
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Hardware_Esp32_Rmt;

#endif //_NANOFRAMEWORK_HARDWARE_ESP32_RMT_NATIVE_H_
