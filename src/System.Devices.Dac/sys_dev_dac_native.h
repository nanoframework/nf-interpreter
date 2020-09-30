//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _SYS_DEV_DAC_NATIVE_H_
#define _SYS_DEV_DAC_NATIVE_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Checks.h>

struct Library_sys_dev_dac_native_System_Devices_Dac_DacController
{
    static const int FIELD___syncLock = 1;
    static const int FIELD___controllerId = 2;
    static const int FIELD__s_deviceCollection = 3;

    NANOCLR_NATIVE_DECLARE(NativeOpenChannel___VOID__I4);
    NANOCLR_NATIVE_DECLARE(NativeGetChannelCount___I4);
    NANOCLR_NATIVE_DECLARE(NativeGetResolutionInBits___I4);
    NANOCLR_NATIVE_DECLARE(NativeInit___VOID);
    NANOCLR_NATIVE_DECLARE(GetDeviceSelector___STATIC__STRING);

    //--//

};

struct Library_sys_dev_dac_native_System_Devices_Dac_DacChannel
{
    static const int FIELD___syncLock = 1;
    static const int FIELD___channelNumber = 2;
    static const int FIELD___dacController = 3;
    static const int FIELD___disposed = 4;

    NANOCLR_NATIVE_DECLARE(NativeWriteValue___VOID__U2);
    NANOCLR_NATIVE_DECLARE(NativeDispose___VOID__BOOLEAN);

    //--//

};

struct Library_sys_dev_dac_native_System_Devices_Dac_DacControllerManager
{
    static const int FIELD_STATIC___syncLock = 0;
    static const int FIELD_STATIC__s_controllersCollection = 1;

    //--//

};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Devices_Dac;

#endif  //_SYS_DEV_DAC_NATIVE_H_
