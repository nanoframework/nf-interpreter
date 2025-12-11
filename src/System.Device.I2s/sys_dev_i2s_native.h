//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef SYS_DEV_I2S_NATIVE_H
#define SYS_DEV_I2S_NATIVE_H

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>
#include <corlib_native.h>

typedef enum __nfpack I2sBitsPerSample
{
    I2sBitsPerSample_Bit8 = 8,
    I2sBitsPerSample_Bit16 = 16,
    I2sBitsPerSample_Bit24 = 24,
    I2sBitsPerSample_Bit32 = 32,
} I2sBitsPerSample;

typedef enum __nfpack I2sChannelFormat
{
    I2sChannelFormat_RightLeft = 0,
    I2sChannelFormat_AllRight = 1,
    I2sChannelFormat_AllLeft = 2,
    I2sChannelFormat_OnlyRight = 3,
    I2sChannelFormat_OnlyLeft = 4,
} I2sChannelFormat;

typedef enum __nfpack I2sCommunicationFormat
{
    I2sCommunicationFormat_I2S = 1,
    I2sCommunicationFormat_Msb = 3,
    I2sCommunicationFormat_PcmShort = 4,
    I2sCommunicationFormat_PcmLong = 12,
    I2sCommunicationFormat_Max = 13,
} I2sCommunicationFormat;

typedef enum __nfpack I2sMode
{
    I2sMode_Master = 1,
    I2sMode_Slave = 2,
    I2sMode_Tx = 4,
    I2sMode_Rx = 8,
    I2sMode_DacBuiltIn = 16,
    I2sMode_AdcBuiltIn = 32,
    I2sMode_Pdm = 64,
} I2sMode;

struct Library_sys_dev_i2s_native_System_Device_I2s_I2sConnectionSettings
{
    static const int FIELD___busId = 1;
    static const int FIELD___sampleRate = 2;
    static const int FIELD___i2sMode = 3;
    static const int FIELD___i2sBitsPerSample = 4;
    static const int FIELD___i2sChannelFormat = 5;
    static const int FIELD___i2sConnectionFormat = 6;
    static const int FIELD___bufferSize = 7;

    //--//
};

struct Library_sys_dev_i2s_native_System_Device_I2s_I2sDevice
{
    static const int FIELD___disposed = 1;
    static const int FIELD___connectionSettings = 2;

    NANOCLR_NATIVE_DECLARE(Read___VOID__SystemSpan_1);
    NANOCLR_NATIVE_DECLARE(Write___VOID__SystemReadOnlySpan_1);
    NANOCLR_NATIVE_DECLARE(NativeInit___VOID);
    NANOCLR_NATIVE_DECLARE(NativeDispose___VOID);

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Device_I2s;

#endif // SYS_DEV_I2S_NATIVE_H
