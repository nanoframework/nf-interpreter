//
// Copyright (c) 2019 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _XPT2046_H_
#define _XPT2046_H_ 1

#include "TouchDevice.h"
#include "TouchInterface.h"

struct TouchDevice g_TouchDevice;
extern TouchInterface g_TouchInterface;

bool TouchDevice::Initialize()
{
	g_TouchDevice.Initialize();
	ReadsToIgnore = 1;
	ReadsPerSample = 1;
	MaxFilterDistance = 1;	// This is actually squared value of the max distance allowed between two points.
	return true;
}
bool TouchDevice::Enable(GPIO_INTERRUPT_SERVICE_ROUTINE touchIsrProc)
{
	if (touchIsrProc == NULL) {};
	return TRUE;
}
bool TouchDevice::Disable()
{
	return true;
}
TouchPointDevice TouchDevice::GetPoint()
{
	CLR_UINT8  CMD_X_READ = 0b10010000;
	CLR_UINT8 CMD_Y_READ = 0b11010000;

	CLR_INT16 x;
	CLR_INT16 y;
	static CLR_INT16 last_x;
	static CLR_INT16 last_y;

	TouchPointDevice TouchValue;
	CLR_UINT8 irq = 0;

	// irq = gpio_get_level(XPT2046_IRQ);

	if (irq == 0) {

		// gpio_set_level(TP_SPI_CS, 0);
		CLR_UINT8 buf = g_TouchInterface.Write_Read(CMD_X_READ);	//Start x read

		buf = g_TouchInterface.Write_Read(0);  // Read x MSB
		x = buf << 8;
		buf = g_TouchInterface.Write_Read(CMD_Y_READ);	// Until x LSB converted y command can be sent
		x += buf;
		buf = g_TouchInterface.Write_Read(0);  // Read y MSB
		y = buf << 8;
		buf = g_TouchInterface.Write_Read(0);  // Read y LSB
		y += buf;
		//gpio_set_level(TP_SPI_CS, 1);
		/*Normalize Data*/
		x = x >> 3;
		y = y >> 3;

		last_x = x;
		last_y = y;
	}
	else {
		x = last_x;
		y = last_y;
		//avg_last = 0;
		//valid = false;
	}
	TouchValue.x = x;
	TouchValue.y = y;

	return TouchValue;
}
#endif // _XPT2046_H_
