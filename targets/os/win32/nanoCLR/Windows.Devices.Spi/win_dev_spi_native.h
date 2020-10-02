//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//


#ifndef _WIN_DEV_SPI_NATIVE_H_
#define _WIN_DEV_SPI_NATIVE_H_


#include "stdafx.h"


struct Library_win_dev_spi_native_Windows_Devices_Spi_SpiBusInfo
{
    static const int FIELD___ChipSelectLineCount = 1;

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
    static const int FIELD_STATIC__DeviceCollection = 0;


    //--//

};

struct Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice
{
    static const int FIELD___syncLock = 1;
    static const int FIELD___spiBus = 2;
    static const int FIELD___deviceId = 3;
    static const int FIELD___connectionSettings = 4;
    static const int FIELD___disposedValue = 5;

    NANOCLR_NATIVE_DECLARE(NativeTransfer___VOID__SZARRAY_U1__SZARRAY_U1__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeTransfer___VOID__SZARRAY_U2__SZARRAY_U2__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeInit___VOID);
    NANOCLR_NATIVE_DECLARE(DisposeNative___VOID);
    NANOCLR_NATIVE_DECLARE(GetDeviceSelector___STATIC__STRING);

    //--//

    //static uint16_t ComputePrescaler (uint8_t bus, int32_t requestedFrequency);
    //static nfSPIConfig GetConfig(int bus, CLR_RT_HeapBlock* config);
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Windows_Devices_Spi;

#endif  //_WIN_DEV_SPI_NATIVE_H_
