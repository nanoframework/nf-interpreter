//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//


#ifndef _WIN_DEV_SPI_NATIVE_H_
#define _WIN_DEV_SPI_NATIVE_H_


#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Checks.h>
#include <hal.h>


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

    static uint16_t ComputeBaudRate(uint8_t bus, int32_t requestedFrequency, int32_t& actualFrequency);
    static void GetSPIConfig(int busIndex, CLR_RT_HeapBlock* config, SPIConfig* llConfig, bool bufferIs16bits);
    static bool IsLongRunningOperation(uint32_t writeSize, uint32_t readSize, bool bufferIs16bits, float byteTime, uint32_t& estimatedDurationMiliseconds);
    static HRESULT NativeTransfer(CLR_RT_StackFrame& stack, bool bufferIs16bits);
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Windows_Devices_Spi;

// struct representing the SPI 
struct NF_PAL_SPI
{
    SPIDriver*  Driver;
    SPIConfig   Configuration;
    float ByteTime;
    bool SequentialTxRx;

    uint8_t* WriteBuffer;
    uint16_t WriteSize;

    uint8_t* ReadBuffer;
    uint16_t ReadSize;
};

///////////////////////////////////////////
// declaration of the the SPI PAL strucs //
///////////////////////////////////////////
#if STM32_SPI_USE_SPI1
    extern NF_PAL_SPI SPI1_PAL;
#endif
#if STM32_SPI_USE_SPI2
    extern NF_PAL_SPI SPI2_PAL;
#endif
#if STM32_SPI_USE_SPI3
    extern NF_PAL_SPI SPI3_PAL;
#endif
#if STM32_SPI_USE_SPI4
    extern NF_PAL_SPI SPI4_PAL;
#endif
#if STM32_SPI_USE_SPI5
    extern NF_PAL_SPI SPI5_PAL;
#endif
#if STM32_SPI_USE_SPI6
    extern NF_PAL_SPI SPI6_PAL;
#endif

#endif  //_WIN_DEV_SPI_NATIVE_H_
