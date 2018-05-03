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

    static uint16_t ComputePrescaler (uint8_t bus, int32_t requestedFrequency);
    static void GetConfig(int busIndex, CLR_RT_HeapBlock* config, SPIConfig* llConfig, bool bufferIs16bits);
    static HRESULT NativeTransfer(CLR_RT_StackFrame& stack, bool bufferIs16bits);
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Windows_Devices_Spi;

// struct representing the SPI 
struct NF_PAL_SPI
{
    SPIDriver*  Driver;
    SPIConfig   Configuration;
    bool SequentialTxRx;

    uint8_t* WriteBuffer;
    uint16_t WriteBufferSize;
    uint16_t WriteSize;

    uint8_t* ReadBuffer;
    uint16_t ReadBufferSize;
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


/////////////////////////////////////
// SPI Tx buffers                  //
// these live in the target folder //
/////////////////////////////////////
extern uint8_t SPI1_WriteBuffer[];
extern uint8_t SPI2_WriteBuffer[];
extern uint8_t SPI3_WriteBuffer[];
extern uint8_t SPI4_WriteBuffer[];
extern uint8_t SPI5_WriteBuffer[];
extern uint8_t SPI6_WriteBuffer[];


/////////////////////////////////////
// SPI Rx buffers                  //
// these live in the target folder //
/////////////////////////////////////
extern uint8_t SPI1_ReadBuffer[];
extern uint8_t SPI2_ReadBuffer[];
extern uint8_t SPI3_ReadBuffer[];
extern uint8_t SPI4_ReadBuffer[];
extern uint8_t SPI5_ReadBuffer[];
extern uint8_t SPI6_ReadBuffer[];


// the following macro defines a function that initializes an SPI struct
// it gets called in the Windows_Devices_SPI_SPIDevice::NativeInit function
#define SPI_INIT(num, tx_buffer_size, rx_buffer_size) void Init_SPI##num() { \
    SPI##num##_PAL.WriteBuffer = SPI##num##_WriteBuffer; \
    SPI##num##_PAL.WriteBufferSize = tx_buffer_size; \
    SPI##num##_PAL.ReadBuffer = SPI##num##_ReadBuffer; \
    SPI##num##_PAL.ReadBufferSize = rx_buffer_size; \
    SPI##num##_PAL.SequentialTxRx = false; \
}

// when a SPI is defined the declarations bellow will have the real function/configuration 
// in the target folder @ target_windows_devices_SPI_config.cpp
void Init_SPI1();
void Init_SPI2();
void Init_SPI3();
void Init_SPI4();
void Init_SPI5();
void Init_SPI6();

// the following macro defines a function that un initializes an SPI struct
// it gets called in the Windows_Devices_SPI_SPIDevice::NativeDispose function
#define SPI_UNINIT(num) void UnInit_SPI##num() { return; }

// when a SPI is defined the declarations bellow will have the real function/configuration 
// in the target folder @ target_windows_devices_SPI_config.cpp
void UnInit_SPI1();
void UnInit_SPI2();
void UnInit_SPI3();
void UnInit_SPI4();
void UnInit_SPI5();
void UnInit_SPI6();

#endif  //_WIN_DEV_SPI_NATIVE_H_
