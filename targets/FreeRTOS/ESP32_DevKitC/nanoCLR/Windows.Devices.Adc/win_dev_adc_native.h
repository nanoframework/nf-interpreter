//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _WIN_DEV_ADC_NATIVE_H_
#define _WIN_DEV_ADC_NATIVE_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Checks.h>

struct Library_win_dev_adc_native_Windows_Devices_Adc_AdcChannel
{
    static const int FIELD___syncLock = 1;
    static const int FIELD___channelNumber = 2;
    static const int FIELD___adcController = 3;
    static const int FIELD___disposed = 4;

    NANOCLR_NATIVE_DECLARE(NativeReadValue___I4);
    NANOCLR_NATIVE_DECLARE(NativeDisposeChannel___VOID);

    //--//

};

struct Library_win_dev_adc_native_Windows_Devices_Adc_AdcController
{
    static const int FIELD___syncLock = 1;
    static const int FIELD___controllerId = 2;
    static const int FIELD___channelMode = 3;
    static const int FIELD__s_deviceCollection = 4;

    NANOCLR_NATIVE_DECLARE(NativeOpenChannel___VOID__I4);
    NANOCLR_NATIVE_DECLARE(NativeGetChannelCount___I4);
    NANOCLR_NATIVE_DECLARE(NativeGetMaxValue___I4);
    NANOCLR_NATIVE_DECLARE(NativeGetMinValue___I4);
    NANOCLR_NATIVE_DECLARE(NativeIsChannelModeSupported___BOOLEAN__I4);
    NANOCLR_NATIVE_DECLARE(NativeGetResolutionInBits___I4);
    NANOCLR_NATIVE_DECLARE(NativeInit___VOID);
    NANOCLR_NATIVE_DECLARE(GetDeviceSelector___STATIC__STRING);

    //--//

};

struct Library_win_dev_adc_native_Windows_Devices_Adc_AdcControllerManager
{
    static const int FIELD_STATIC___syncLock = 0;
    static const int FIELD_STATIC__s_controllersCollection = 1;


    //--//

};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Windows_Devices_Adc;

#endif  //_WIN_DEV_ADC_NATIVE_H_
