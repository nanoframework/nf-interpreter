//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _SPI_TO_TOUCHPANEL_H_
#define _SPI_TO_TOUCHPANEL_H_ 1

#include "nanoCLR_Types.h"
#include <nanoPAL.h>
#include <target_platform.h>
#include "TouchInterface.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
             
bool TouchInterface::Initialize()
{

    return true;
}

CLR_UINT8 TouchInterface::Write_Read(CLR_UINT8 value)
{
    return 1;
}
#endif //_SPI_TO_TOUCHPANEL_H_
#pragma GCC diagnostic pop
