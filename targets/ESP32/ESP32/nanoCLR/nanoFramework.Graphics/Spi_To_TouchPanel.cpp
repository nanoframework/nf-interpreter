//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#define UNUSED(x)  (void)x

#ifndef SPI_TO_TOUCHPANEL_H
#define SPI_TO_TOUCHPANEL_H

#include "nanoCLR_Types.h"
#include <nanoPAL.h>
#include <target_platform.h>
#include <esp32_idf.h>

#include "TouchInterface.h"
             
bool TouchInterface::Initialize()
{
    // Setup SPI configuration

    return true;
}

CLR_UINT8* TouchInterface::Write_Read(CLR_UINT8* valuesToSend, CLR_UINT16 numberOfValuesToSend, CLR_UINT16 numberValuesExpected)
{

    UNUSED(valuesToSend);
    UNUSED(numberOfValuesToSend);
    UNUSED(numberValuesExpected);


    return 0;
}


#endif //SPI_TO_TOUCHPANEL_H
