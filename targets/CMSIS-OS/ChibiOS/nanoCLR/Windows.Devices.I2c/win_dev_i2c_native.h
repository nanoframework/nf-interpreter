//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _WIN_DEV_I2C_NATIVE_H_
#define _WIN_DEV_I2C_NATIVE_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Checks.h>
#include <hal.h>

struct Library_win_dev_i2c_native_Windows_Devices_I2c_I2cConnectionSettings
{
    static const int FIELD___slaveAddress = 1;
    static const int FIELD___busSpeed = 2;
    static const int FIELD___sharingMode = 3;


    //--//

};

struct Library_win_dev_i2c_native_Windows_Devices_I2c_I2cController
{
    static const int FIELD_STATIC__s_instance = 0;
    static const int FIELD_STATIC__s_deviceCollection = 1;


    //--//

};

struct Library_win_dev_i2c_native_Windows_Devices_I2c_I2cDevice
{
    static const int FIELD___syncLock = 1;
    static const int FIELD___deviceId = 2;
    static const int FIELD___connectionSettings = 3;
    static const int FIELD___disposed = 4;

    NANOCLR_NATIVE_DECLARE(NativeInit___VOID);
    NANOCLR_NATIVE_DECLARE(DisposeNative___VOID);
    NANOCLR_NATIVE_DECLARE(NativeTransmit___WindowsDevicesI2cI2cTransferResult__SZARRAY_U1__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(GetDeviceSelector___STATIC__STRING);

    //--//
    static void GetConfig(CLR_RT_HeapBlock* managedConfig, I2CConfig* llConfig);
};

struct Library_win_dev_i2c_native_Windows_Devices_I2c_I2cTransferResult
{
    static const int FIELD___bytesTransferred = 1;
    static const int FIELD___status = 2;


    //--//

};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Windows_Devices_I2c;

// struct representing the I2C 
struct NF_PAL_I2C
{
    I2CDriver*  Driver;
    I2CConfig   Configuration;
    thread_t*   WorkingThread;
    i2caddr_t   Address;

    uint8_t* WriteBuffer;
    uint8_t  WriteSize;

    uint8_t* ReadBuffer;
    uint8_t  ReadSize;
};

///////////////////////////////////////////
// declaration of the the I2C PAL strucs //
///////////////////////////////////////////
#if STM32_I2C_USE_I2C1
    extern NF_PAL_I2C I2C1_PAL;
#endif
#if STM32_I2C_USE_I2C2
    extern NF_PAL_I2C I2C2_PAL;
#endif
#if STM32_I2C_USE_I2C3
    extern NF_PAL_I2C I2C3_PAL;
#endif
#if STM32_I2C_USE_I2C4
    extern NF_PAL_I2C I2C4_PAL;
#endif


/////////////////////////////////////
// I2C Tx buffers                 //
// these live in the target folder //
/////////////////////////////////////
extern uint8_t I2C1_WriteBuffer[];
extern uint8_t I2C2_WriteBuffer[];
extern uint8_t I2C3_WriteBuffer[];
extern uint8_t I2C4_WriteBuffer[];


/////////////////////////////////////
// I2C Rx buffers                  //
// these live in the target folder //
/////////////////////////////////////
extern uint8_t I2C1_ReadBuffer[];
extern uint8_t I2C2_ReadBuffer[];
extern uint8_t I2C3_ReadBuffer[];
extern uint8_t I2C4_ReadBuffer[];


// the following macro defines a function that initializes an I2C struct
// it gets called in the Windows_Devices_I2c_I2cDevice::NativeInit function
#define I2C_INIT(num, tx_buffer_size, rx_buffer_size) void Init_I2C##num() { \
    I2C##num##_PAL.WriteBuffer = I2C##num##_WriteBuffer; \
    I2C##num##_PAL.ReadBuffer = I2C##num##_ReadBuffer; \
}

// when a I2C is defined the declarations bellow will have the real function/configuration 
// in the target folder @ target_windows_devices_i2c_config.cpp
void Init_I2C1();
void Init_I2C2();
void Init_I2C3();
void Init_I2C4();

// the following macro defines a function that un initializes an I2C struct
// it gets called in the Windows_Devices_I2c_I2cDevice::NativeDispose function
#define I2C_UNINIT(num) void UnInit_I2C##num() { return; }

// when a I2C is defined the declarations bellow will have the real function/configuration 
// in the target folder @ target_windows_devices_i2c_config.cpp
void UnInit_I2C1();
void UnInit_I2C2();
void UnInit_I2C3();
void UnInit_I2C4();

#endif  //_WIN_DEV_I2C_NATIVE_H_
