//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//


#ifndef _WIN_DEV_SPI_NATIVE_H_
#define _WIN_DEV_SPI_NATIVE_H_


#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Checks.h>
#include <Esp32_os.h>

// # buses but first can't be used as SPI flash
#define NUM_SPI_BUSES   3
#define MAX_SPI_DEVICES 3

// Max clock frequency is 20 MHz
#define MAX_CLOCK_FREQUENCY 20000000

struct nfSpiBusConfig
{
    bool                spiBusInited;
    int                 deviceId[MAX_SPI_DEVICES];            // Used to find correct device in config
    spi_device_handle_t deviceHandles[MAX_SPI_DEVICES];       // Max 3 devices per bus (IDF limit )
};

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

    static spi_device_interface_config_t GetConfig(int bus, CLR_RT_HeapBlock* config);
    static int Add_Spi_Device(int bus, CLR_RT_HeapBlock* pThis);
    static bool GetDevice( CLR_RT_HeapBlock* pThis, uint8_t * pBus, int * pDeviceIndex);
    
    static HRESULT NativeTransfer( CLR_RT_StackFrame& stack, bool data16 );

};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Windows_Devices_Spi;

#endif  //_WIN_DEV_SPI_NATIVE_H_
