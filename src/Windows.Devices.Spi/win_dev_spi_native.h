//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _WIN_DEV_SPI_NATIVE_H_
#define _WIN_DEV_SPI_NATIVE_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Checks.h>

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

struct Library_win_dev_spi_native_Windows_Devices_Spi_SpiBusInfo
{
    static const int FIELD___controllerId = 1;

    NANOCLR_NATIVE_DECLARE(NativeChipSelectLineCount___I4);
    NANOCLR_NATIVE_DECLARE(NativeMaxClockFrequency___I4);
    NANOCLR_NATIVE_DECLARE(NativeMinClockFrequency___I4);

    //--//
};

struct Library_win_dev_spi_native_Windows_Devices_Spi_SpiConnectionSettings
{
    static const int FIELD___csLine = 1;
    static const int FIELD___clockFrequency = 2;
    static const int FIELD___databitLength = 3;
    static const int FIELD___spiMode = 4;
    static const int FIELD___spiSharingMode = 5;
    static const int FIELD___bitOrder = 6;

    //--//
};

struct Library_win_dev_spi_native_Windows_Devices_Spi_SpiController
{
    static const int FIELD___syncLock = 1;
    static const int FIELD___controllerId = 2;

    NANOCLR_NATIVE_DECLARE(GetDeviceSelector___STATIC__STRING);

    //--//
};

struct Library_win_dev_spi_native_Windows_Devices_Spi_SpiControllerManager
{
    static const int FIELD_STATIC__s_syncLock = 0;
    static const int FIELD_STATIC__s_controllersCollection = 1;

    //--//
};

struct Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice
{
    static const int FIELD___deviceId = 1;
    static const int FIELD___connectionSettings = 2;
    static const int FIELD___spiController = 3;
    static const int FIELD___syncLock = 4;
    static const int FIELD___disposedValue = 5;

    NANOCLR_NATIVE_DECLARE(DisposeNative___VOID);
    NANOCLR_NATIVE_DECLARE(NativeTransfer___VOID__SZARRAY_U1__SZARRAY_U1__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeTransfer___VOID__SZARRAY_U2__SZARRAY_U2__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeInit___VOID);
    NANOCLR_NATIVE_DECLARE(NativeOpenDevice___I4);

    //--//

    static HRESULT NativeTransfer(CLR_RT_StackFrame &stack, bool bufferIs16bits);
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Windows_Devices_Spi;

#endif //_WIN_DEV_SPI_NATIVE_H_
