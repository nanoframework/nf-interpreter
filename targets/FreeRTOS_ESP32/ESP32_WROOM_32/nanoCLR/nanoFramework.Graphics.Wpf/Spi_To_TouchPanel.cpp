//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _SPI_TO_TOUCHPANEL_H_
#define _SPI_TO_TOUCHPANEL_H_ 1

#include "nanoCLR_Types.h"
#include <nanoPAL.h>
#include <target_platform.h>
#include <Esp32_os.h>

#include "TouchInterface.h"


bool TouchInterface::Initialize()
{
	// Setup SPI configuration

	return true;
}

CLR_UINT8 TouchInterface::Write_Read(CLR_UINT8 value)
{

	if (value == 0) {};
	 // write read value to spi 
	return 0;
}


#endif //_SPI_TO_TOUCHPANEL_H_
