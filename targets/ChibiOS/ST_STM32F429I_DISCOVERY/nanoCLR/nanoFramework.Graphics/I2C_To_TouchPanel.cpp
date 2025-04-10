//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef I2C_TO_TOUCHPANEL_H
#define I2C_TO_TOUCHPANEL_H

#include <nanoCLR_Types.h>
#include <nanoPAL.h>
#include <target_platform.h>
#include <TouchInterface.h>

// This code module is written for a STM32F429I DISCOVERY board with display 240x320 TFT and touch screen
// The touch screen driver IC is a STMPE811QTR
#define STMPE811QTR_I2C_Address 0x41

extern I2CDriver touchDriver;
static CLR_UINT16 I2C_Address;

bool TouchInterface::Initialize()
{
    // The FT6206 is connected to I2C4 of the STM32F769.
    // This is already configured by the chibios system in the file
    // mcuconf.h

    // #define STM32_I2C_USE_I2C4                  TRUE

    I2C_Address = STMPE811QTR_I2C_Address;

    return true;
}
CLR_UINT8 *TouchInterface::Write_Read(
    CLR_UINT8 *valuesToSend,
    CLR_UINT16 numberOfValuesToSend,
    CLR_UINT16 numberValuesExpected)
{
    // TO BE DEVELOPED
    (void)valuesToSend;
    (void)numberOfValuesToSend;
    (void)numberValuesExpected;

    // CLR_UINT8* receivedValues = nullptr;
    // msg_t result = i2cMasterTransmitTimeout(&touchDriver, I2C_Address, valuesToSend, numberOfValuesToSend,
    // receivedValues, numberValuesExpected, TIME_MS2I(20)); return receivedValues;

    return 0U;
}

#endif // I2C_TO_TOUCHPANEL_H
