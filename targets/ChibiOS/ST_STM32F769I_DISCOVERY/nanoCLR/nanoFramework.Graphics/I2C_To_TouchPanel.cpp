//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _SPI_TO_TOUCHPANEL_H_
#define _SPI_TO_TOUCHPANEL_H_ 1

#include "nanoCLR_Types.h"
#include <nanoPAL.h>
#include <target_platform.h>
#include "TouchInterface.h"
#include "hal_i2c.h"
#include "CPU_GPIO_decl.h"
#include "Core_Cm7.h"
#include "STM32f769xx.h"

#define UNUSED(x)  (void)x

// This code module is written for a STM32F769I DISCOVERY board 
// with MB1166 daughter board (K.O.D Optica Technology 480x800 TFT and touch screen)
// The touch screen driver IC is a FT6206 by Focal Tech.
#define FT6206_I2C_Address  0x54

extern  I2CDriver touchDriver;
static CLR_UINT16 I2C_Address;


bool TouchInterface::Initialize()
{
    // The FT6206 is connected to I2C4 of the STM32F769.
    // This is already configured by the chibios system in the file
    // mcuconf.h

    // #define STM32_I2C_USE_I2C4                  TRUE


    I2C_Address = FT6206_I2C_Address;


    return true;
}
CLR_UINT8* TouchInterface::Write_Read(CLR_UINT8* valuesToSend, CLR_UINT16 numberOfValuesToSend, CLR_UINT16 numberValuesExpected)
{
    // TO BE DEVELOPED
    UNUSED(valuesToSend);
    UNUSED(numberOfValuesToSend);
    UNUSED(numberValuesExpected);

    //CLR_UINT8* receivedValues = NULL;
    //msg_t result = i2cMasterTransmitTimeout(&touchDriver, I2C_Address, valuesToSend, numberOfValuesToSend, receivedValues, numberValuesExpected, TIME_MS2I(20));
    //return receivedValues;

    return 0U;

}
#endif //_SPI_TO_TOUCHPANEL_H_


