//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _DRIVERS_I2C_DECL_H_
#define _DRIVERS_I2C_DECL_H_ 1

enum I2cTransferStatus
{
    I2cTransferStatus_FullTransfer = 0,
    I2cTransferStatus_ClockStretchTimeout,
    I2cTransferStatus_PartialTransfer,
    I2cTransferStatus_SlaveAddressNotAcknowledged,
    I2cTransferStatus_UnknownError
};

bool CPU_I2C_Initialize(uint8_t i2cBus, bool fastSpeed);
bool CPU_I2C_Uninitialize(uint8_t i2cBus);

void CPU_I2C_GetPins(uint8_t i2cBusIndex, GPIO_PIN &clockPin, GPIO_PIN &dataPin);

I2cTransferStatus CPU_I2C_WriteRead(
    uint8_t i2cBus,
    int slaveAddress,
    unsigned char *writeData,
    int writeSize,
    unsigned char *readData,
    int readSize);


#endif // _DRIVERS_I2C_DECL_H_
