//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//


#ifndef _WINDOWS_DEVICES_SPI_NATIVE_H_
#define _WINDOWS_DEVICES_SPI_NATIVE_H_

#include <nanoCLR_Interop.h>
struct Library_windows_devices_spi_native_Windows_Devices_Spi_SpiBusInfo
{
    static const int FIELD__SOMETHING_WRONG_WITH_THIS_FIELD_POSSIBLY_MISSING_BACKING_FIELD = 1;

    NANOCLR_NATIVE_DECLARE(get_MaxClockFrequency___I4);
    NANOCLR_NATIVE_DECLARE(get_MinClockFrequency___I4);

    //--//

};

struct Library_windows_devices_spi_native_Windows_Devices_Spi_SpiConnectionSettings
{
    static const int FIELD___csLine = 1;
    static const int FIELD___clockFrequency = 2;
    static const int FIELD___databitLength = 3;
    static const int FIELD___spiMode = 4;
    static const int FIELD___spiSharingMode = 5;


    //--//

};

struct Library_windows_devices_spi_native_Windows_Devices_Spi_SpiDevice
{
    static const int FIELD_STATIC__Rnd = 0;

    static const int FIELD___spiBus = 1;
    static const int FIELD___deviceId = 2;
    static const int FIELD__SOMETHING_WRONG_WITH_THIS_FIELD_POSSIBLY_MISSING_BACKING_FIELD = 3;

    NANOCLR_NATIVE_DECLARE(NativeTransfer___VOID__STRING__SZARRAY_U1__SZARRAY_U1__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeTransfer___VOID__STRING__SZARRAY_U2__SZARRAY_U2__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeInit___VOID__STRING__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(GetDeviceSelector___STATIC__STRING);

    //--//

};



extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Windows_Devices_Spi;

#endif  //_WINDOWS_DEVICES_SPI_NATIVE_H_
