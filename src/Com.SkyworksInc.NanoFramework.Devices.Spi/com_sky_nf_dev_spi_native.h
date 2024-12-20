// Copyright Skyworks Solutions, Inc. All Rights Reserved.

#ifndef _COM_SKY_NF_DEV_SPI_NATIVE_H_
#define _COM_SKY_NF_DEV_SPI_NATIVE_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>
#include <corlib_native.h>

// defined src\System.Device.Spi\sys_dev_spi_native.h
// typedef enum __nfpack DataFlow
// {
//     DataFlow_MsbFirst = 0,
//     DataFlow_LsbFirst = 1,
// } DataFlow;

typedef enum __nfpack SpiError
{
    SpiError_Unknown = 0,
    SpiError_Timeout = 1,
    SpiError_Aborted = 2,
} SpiError;

// defined src\System.Device.Spi\sys_dev_spi_native.h
typedef enum __nfpack SpiPhasePolarityMode
{
    SpiPhasePolarityMode_Mode0 = 0,
    SpiPhasePolarityMode_Mode1 = 1,
    SpiPhasePolarityMode_Mode2 = 2,
    SpiPhasePolarityMode_Mode3 = 3,
} SpiPhasePolarityMode;

// defined src\System.Device.Spi\sys_dev_spi_native.h
typedef enum __nfpack SpiWireMode
{
    SpiWireMode_FullDuplex = 0,
    SpiWireMode_HalfDuplex = 1,
    SpiWireMode_Simplex = 2,
} SpiWireMode;

struct Library_com_sky_nf_dev_spi_native_Com_SkyworksInc_NanoFramework_Devices_Spi_SpiBaseConfiguration
{
    static const int FIELD___clockFrequency = 1;
    static const int FIELD___databitLength = 2;
    static const int FIELD___spiPhasePolarityMode = 3;
    static const int FIELD___dataFlow = 4;
    static const int FIELD___spiWireMode = 5;

    //--//
};

struct Library_com_sky_nf_dev_spi_native_Com_SkyworksInc_NanoFramework_Devices_Spi_SpiBus
{
    static const int FIELD_STATIC___busConnectionSettings = 0;

    static const int FIELD___syncLock = 1;
    static const int FIELD___bufferSingleOperation = 2;

    NANOCLR_NATIVE_DECLARE(
        NativeTransfer___VOID__I4__SystemSpanByte__SystemSpanByte__BOOLEAN__ComSkyworksIncNanoFrameworkDevicesSpiSpiBaseConfiguration);
    NANOCLR_NATIVE_DECLARE(NativeReportBusSettingsChanged___VOID__I4);
    NANOCLR_NATIVE_DECLARE(NativeGetBusSpeed___I4__I4__ComSkyworksIncNanoFrameworkDevicesSpiSpiBaseConfiguration);

    //--//

    static HRESULT ExecuteTransfer(CLR_RT_StackFrame &stack);
    static HRESULT ThrowError(CLR_RT_StackFrame &stack, CLR_INT32 errorCode);
};

struct Library_com_sky_nf_dev_spi_native_Com_SkyworksInc_NanoFramework_Devices_Spi_SpiException
{
    static const int FIELD___errorCode = 5;

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Com_SkyworksInc_NanoFramework_Devices_Spi;

#endif //_COM_SKY_NF_DEV_SPI_NATIVE_H_
