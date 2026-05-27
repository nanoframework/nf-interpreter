//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef SYS_DEV_I2C_SLAVE_NATIVE_TARGET_H
#define SYS_DEV_I2C_SLAVE_NATIVE_TARGET_H

#include <sys_dev_i2c_slave_native.h>
#include <string.h>
#include <targetPAL.h>
#include <targetPAL_I2c.h>
#include <Esp32_DeviceMapping.h>
#include <target_system_device_i2c_slave_config.h>
#include <esp32_idf.h>

// struct for I2C Slave transaction
typedef struct
{
    i2c_port_t BusNum;
    TaskHandle_t I2cSlaveWorkerTask;
    StaticTask_t I2cSlaveWorkerTaskBuffer;
    StackType_t *I2cSlaveWorkerTaskStack;

    uint8_t *Buffer;
    uint16_t RequestedBytes;
    int16_t BytesTransferred;

    int64_t TimeoutTicks;

} NF_PAL_I2CSLAVE;

//////////////////////////////////////////////////
// declaration of the the I2C Slave PAL structs //
//////////////////////////////////////////////////
#if SOC_I2C_NUM > 0
extern NF_PAL_I2CSLAVE I2cSlave0_PAL;
#endif
#if SOC_I2C_NUM > 1
extern NF_PAL_I2CSLAVE I2cSlave1_PAL;
#endif

#endif // SYS_DEV_I2C_SLAVE_NATIVE_TARGET_H
