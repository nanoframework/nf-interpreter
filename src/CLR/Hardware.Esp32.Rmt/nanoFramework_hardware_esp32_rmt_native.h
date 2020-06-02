//
// Copyright (c) 2020 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _NANOFRAMEWORK_HARDWARE_ESP32_RMT_NATIVE_H_
#define _NANOFRAMEWORK_HARDWARE_ESP32_RMT_NATIVE_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <corlib_native.h>

struct Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_RmtCommand
{
    static const int FIELD__level0 = 1;
    static const int FIELD__level1 = 2;
    static const int FIELD__mDuration0 = 3;
    static const int FIELD__mDuration1 = 4;

    //--//

};

struct Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel
{
    static const int FIELD__commands = 1;
    static const int FIELD___channel = 2;
    static const int FIELD___carrierEnabled = 3;
    static const int FIELD___carrierHighDuration = 4;
    static const int FIELD___carrierLowDuration = 5;
    static const int FIELD___carrierLevel = 6;

    NANOCLR_NATIVE_DECLARE(NativeInit___STATIC__I4__I4);
    NANOCLR_NATIVE_DECLARE(NativeGetIdleLevel___STATIC__BOOLEAN__I4);
    NANOCLR_NATIVE_DECLARE(NativeGetIsChannelIdle___STATIC__BOOLEAN__I4);
    NANOCLR_NATIVE_DECLARE(NativeSetIsChannelIdle___STATIC__VOID__I4__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeSetIdleLevel___STATIC__VOID__I4__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeSetCarrierMode___STATIC__VOID__I4__BOOLEAN__U2__U2__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeWriteItems___STATIC__U4__I4__SZARRAY_U1__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeWaitTxDone___STATIC__U4__I4__I4);
    NANOCLR_NATIVE_DECLARE(NativeDispose___STATIC__VOID__I4);
    NANOCLR_NATIVE_DECLARE(NativeGetClockDivider___STATIC__U1__I4);
    NANOCLR_NATIVE_DECLARE(NativeGetSourceClock___STATIC__BOOLEAN__I4);
    NANOCLR_NATIVE_DECLARE(NativeSetClockDivider___STATIC__VOID__I4__U1);
    NANOCLR_NATIVE_DECLARE(NativeSetSourceClock___STATIC__VOID__I4__BOOLEAN);

    //--//

};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Hardware_Esp32_Rmt;

#endif  //_NANOFRAMEWORK_HARDWARE_ESP32_RMT_NATIVE_H_
