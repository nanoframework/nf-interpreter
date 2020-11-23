//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _NANO_I2C_DECL_H_
#define _NANO_I2C_DECL_H_ 1

#include "TargetHAL_I2c.h"

#include "CPU_I2C_decl.h"

#define CPU_DEVICE_TYPE_I2C 2

// enum I2CResult
// {
//     I2CResult_OK,
//     I2CResult_FAIL,
//     I2CResult_TIMEOUT
// };

struct I2C_DEVICE_CONFIGURATION
{
    int8_t i2cBus;   // I2C Bus number
    uint8_t address; // Device address on bus
    bool fastMode;   // Speed false=normal(100Khz), true=fast(400khz)
};


// Initialise the i2cConfig structure
// Called on CLR startup
bool nanoI2C_Initialize();
void nanoI2C_Uninitialize();

    // Open I2C bus / device using device configuration
// Register GPIO pins as in use.
// Return result, if S_OK then deviceHandle returned
HRESULT nanoI2C_OpenDevice(I2C_DEVICE_CONFIGURATION &config, uint32_t &deviceHandle);

// Execute a I2C write/read operation
// return status of operation as I2CResult
I2cTransferStatus nanoI2C_WriteRead(
    uint32_t deviceHandle,
    uint8_t *writeBuffer,
    uint16_t writeSize,
    uint8_t *readBuffer,
    uint16_t readSize);

// Close I2C device
// When last device closed bus is closed
HRESULT nanoI2C_CloseDevice(uint32_t deviceHandle);

#endif // _NANO_I2C_DECL_H_
