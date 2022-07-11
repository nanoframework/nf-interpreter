// Copyright Skyworks Solutions, Inc. All Rights Reserved.

#ifndef _COM_SKY_NF_DEV_SPI_NATIVE_H_
#define _COM_SKY_NF_DEV_SPI_NATIVE_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>
#include <corlib_native.h>

typedef enum __nfpack DataFlow
{
    DataFlow_MsbFirst = 0,
    DataFlow_LsbFirst = 1,
} DataFlow;

// moved to src\PAL\Include\CPU_SPI_decl.h for convenience
// typedef enum __nfpack SpiBusConfiguration
// {
//     SpiBusConfiguration_FullDuplex = 0,
//     SpiBusConfiguration_HalfDuplex = 1,
//     SpiBusConfiguration_Simplex = 2,
// } SpiBusConfiguration;

// moved to src\PAL\Include\CPU_SPI_decl.h for convenience
// typedef enum __nfpack SpiMode
// {
//     SpiMode_Mode0 = 0,
//     SpiMode_Mode1 = 1,
//     SpiMode_Mode2 = 2,
//     SpiMode_Mode3 = 3,
// } SpiMode;

struct Library_com_sky_nf_dev_spi_native_Com_SkyworksInc_NanoFramework_Devices_Spi_SpiBaseConfiguration
{
    static const int FIELD___csLine = 1;
    static const int FIELD___clockFrequency = 2;
    static const int FIELD___databitLength = 3;
    static const int FIELD___spiMode = 4;
    static const int FIELD___dataFlow = 5;
    static const int FIELD___busConfiguration = 6;

    //--//

};

struct Library_com_sky_nf_dev_spi_native_Com_SkyworksInc_NanoFramework_Devices_Spi_SpiBus
{
    static const int FIELD_STATIC___busConnectionSettings = 0;

    static const int FIELD___syncLock = 1;
    static const int FIELD___bufferSingleOperation = 2;

    NANOCLR_NATIVE_DECLARE(NativeTransfer___VOID__I4__SystemSpanByte__SystemSpanByte__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeTransfer___VOID__I4__SZARRAY_U2__SZARRAY_U2__BOOLEAN);

    //--//

};

struct Library_com_sky_nf_dev_spi_native_ThisAssembly
{
    static const int FIELD_STATIC__GitCommitDate = 1;

    //--//

};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Com_SkyworksInc_NanoFramework_Devices_Spi;

#endif  //_COM_SKY_NF_DEV_SPI_NATIVE_H_
