//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//


#ifndef _WIN_DEV_SPI_NATIVE_H_
#define _WIN_DEV_SPI_NATIVE_H_


#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Checks.h>


///////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH Windows.Devices.Spi.SpiMode (in managed code) !!! //
///////////////////////////////////////////////////////////////////////////////////////

enum DataBitOrder
{
    DataBitOrder_MSB = 0,
    DataBitOrder_LSB
};

///////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH Windows.Devices.Spi.SpiMode (in managed code) !!! //
///////////////////////////////////////////////////////////////////////////////////////

enum SpiModes
{
    SpiModes_Mode0 = 0,
    SpiModes_Mode1,
    SpiModes_Mode2,
    SpiModes_Mode3
};

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

struct Library_win_dev_spi_native_Windows_Devices_Spi_SpiBusInfo
{
    static const int FIELD___chipSelectLineCount = 1;

    NANOCLR_NATIVE_DECLARE(get_MaxClockFrequency___I4);
    NANOCLR_NATIVE_DECLARE(get_MinClockFrequency___I4);

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
    static const int FIELD__s_deviceCollection = 3;

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
    static const int FIELD___syncLock = 1;
    static const int FIELD___spiBus = 2;
    static const int FIELD___deviceId = 3;
    static const int FIELD___connectionSettings = 4;
    static const int FIELD___disposedValue = 5;

    NANOCLR_NATIVE_DECLARE(DisposeNative___VOID);
    NANOCLR_NATIVE_DECLARE(NativeTransfer___VOID__SZARRAY_U1__SZARRAY_U1__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeTransfer___VOID__SZARRAY_U2__SZARRAY_U2__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeInit___VOID);

    //--//

    static HRESULT NativeTransfer(CLR_RT_StackFrame& stack, bool bufferIs16bits);
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Windows_Devices_Spi;

#endif  //_WIN_DEV_SPI_NATIVE_H_
