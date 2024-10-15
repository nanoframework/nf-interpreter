//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef SYS_DEV_SPI_NATIVE_H
#define SYS_DEV_SPI_NATIVE_H

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>
#include <corlib_native.h>

typedef enum __nfpack DataFlow
{
    DataFlow_MsbFirst = 0,
    DataFlow_LsbFirst = 1,
} DataFlow;

// TODO: as this is already existing in Windows.Devices.Spi, this should be uncommented
// once it will be fully removed.
// typedef enum __nfpack SpiMode
// {
//     SpiMode_Mode0 = 0,
//     SpiMode_Mode1 = 1,
//     SpiMode_Mode2 = 2,
//     SpiMode_Mode3 = 3,
// } SpiMode;

struct Library_sys_dev_spi_native_System_Device_Spi_SpiBusInfo
{
    static const int FIELD___controllerId = 1;

    NANOCLR_NATIVE_DECLARE(NativeMaxClockFrequency___I4);
    NANOCLR_NATIVE_DECLARE(NativeMinClockFrequency___I4);

    //--//
};

struct Library_sys_dev_spi_native_System_Device_Spi_SpiConnectionSettings
{
    static const int FIELD___csLine = 1;
    static const int FIELD___clockFrequency = 2;
    static const int FIELD___databitLength = 3;
    static const int FIELD___spiMode = 4;
    static const int FIELD___spiSharingMode = 5;
    static const int FIELD___dataFlow = 6;
    static const int FIELD___busId = 7;
    static const int FIELD___chipSelectLineActiveState = 8;

    //--//
};

struct Library_sys_dev_spi_native_System_Device_Spi_SpiDevice
{
    static const int FIELD___deviceId = 1;
    static const int FIELD___connectionSettings = 2;
    static const int FIELD___disposedValue = 3;
    static const int FIELD___syncLock = 4;
    static const int FIELD___bufferSingleOperation = 5;

    NANOCLR_NATIVE_DECLARE(DisposeNative___VOID);
    NANOCLR_NATIVE_DECLARE(NativeTransfer___VOID__SystemSpanByte__SystemSpanByte__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeTransfer___VOID__SZARRAY_U2__SZARRAY_U2__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeInit___VOID);
    NANOCLR_NATIVE_DECLARE(NativeOpenDevice___I4);

    //--//

    static HRESULT NativeTransfer(CLR_RT_StackFrame &stack, bool bufferIs16bits);
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Device_Spi;

#endif // SYS_DEV_SPI_NATIVE_H
