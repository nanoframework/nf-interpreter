//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _SYS_DEV_ADC_NATIVE_H_
#define _SYS_DEV_ADC_NATIVE_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Checks.h>

typedef enum __nfpack AdcChannelMode
{
    AdcChannelMode_SingleEnded = 0,
    AdcChannelMode_Differential = 1,
} AdcChannelMode;

struct Library_sys_dev_adc_native_System_Device_Adc_AdcChannel
{
    static const int FIELD___syncLock = 2;
    static const int FIELD___channelNumber = 3;
    static const int FIELD___disposed = 4;

    NANOCLR_NATIVE_DECLARE(NativeReadValue___I4);
    NANOCLR_NATIVE_DECLARE(NativeDisposeChannel___VOID);

    //--//
};

struct Library_sys_dev_adc_native_System_Device_Adc_AdcController
{
    static const int FIELD___syncLock = 1;
    static const int FIELD___channelMode = 2;

    NANOCLR_NATIVE_DECLARE(NativeOpenChannel___VOID__I4);
    NANOCLR_NATIVE_DECLARE(NativeGetChannelCount___I4);
    NANOCLR_NATIVE_DECLARE(NativeGetMaxValue___I4);
    NANOCLR_NATIVE_DECLARE(NativeGetMinValue___I4);
    NANOCLR_NATIVE_DECLARE(NativeIsChannelModeSupported___BOOLEAN__I4);
    NANOCLR_NATIVE_DECLARE(NativeGetResolutionInBits___I4);
    NANOCLR_NATIVE_DECLARE(NativeInit___VOID);

    //--//
};

struct Library_sys_dev_adc_native_System_Device_Adc_AdcChannelBase
{
    static const int FIELD___adcController = 1;

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Device_Adc;

#endif //_SYS_DEV_ADC_NATIVE_H_
