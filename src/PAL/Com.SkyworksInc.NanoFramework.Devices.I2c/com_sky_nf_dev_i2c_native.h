// Copyright Skyworks Solutions, Inc. All Rights Reserved.

#ifndef _COM_SKY_NF_DEV_I2C_NATIVE_H_
#define _COM_SKY_NF_DEV_I2C_NATIVE_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>
#include <corlib_native.h>

typedef enum __nfpack I2cBusSpeed
{
    I2cBusSpeed_StandardMode = 0,
    I2cBusSpeed_FastMode = 1,
} I2cBusSpeed;

typedef enum __nfpack I2cTransferStatus
{
    I2cTransferStatus_UnknownError = 0,
    I2cTransferStatus_ClockStretchTimeout = 1,
    I2cTransferStatus_PartialTransfer = 2,
    I2cTransferStatus_SlaveAddressNotAcknowledged = 3,
} I2cTransferStatus;

struct Library_com_sky_nf_dev_i2c_native_Com_SkyworksInc_NanoFramework_Devices_I2c_I2cBus
{
    static const int FIELD_STATIC___busSpeed = 0;

    static const int FIELD___syncLock = 1;
    static const int FIELD___buffer = 2;

    NANOCLR_NATIVE_DECLARE(NativeTransmit___VOID__I4__U1__SystemSpanByte__SystemSpanByte);

    //--//
};

struct Library_com_sky_nf_dev_i2c_native_Com_SkyworksInc_NanoFramework_Devices_I2c_I2cException
{
    static const int FIELD___bytesTransferred = 5;
    static const int FIELD___status = 6;

    //--//
};

struct Library_com_sky_nf_dev_i2c_native_ThisAssembly
{
    static const int FIELD_STATIC__GitCommitDate = 1;

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Com_SkyworksInc_NanoFramework_Devices_I2c;

#endif //_COM_SKY_NF_DEV_I2C_NATIVE_H_
