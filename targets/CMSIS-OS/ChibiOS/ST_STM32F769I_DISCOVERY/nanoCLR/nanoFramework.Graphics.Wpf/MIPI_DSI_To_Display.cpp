//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

// -------------------------------------------------------
// For the STM32F769IDiscovery evaluation board
// Code adapted from online MBED example DISCO-F769NI_TOUCHSCREEN
// -------------------------------------------------------

#include "DisplayInterface.h"
#include <nanoPAL.h>
#include <target_platform.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

struct DisplayInterface g_DisplayInterface;

// This is the Display Interface block
bool DisplayInterface::InitializeDisplayInterface()
{
	// CPIO Initialization
	// DSI Initialization
	// LTDC Initialization
	// LCD clock configuration
	return true;
}
void DisplayInterface::GetTransferBuffer(CLR_UINT8*& TransferBuffer, CLR_UINT32& TransferBufferSize)
{
	return;
}
void DisplayInterface::SendCommandByte(CLR_UINT8 command)
{
	return;
}
void DisplayInterface::SendCommandWord(CLR_UINT16 commandWord)
{
	return;
}
void DisplayInterface::SendData(CLR_UINT8 data[], CLR_UINT32 dataCount)
{
	return;
}
void DisplayInterface::SendCommandByteAndData(CLR_UINT8 command, CLR_UINT8 data[], CLR_UINT32 dataCount)
{
	return;
}
void DisplayInterface::SendCommandWordAndData(CLR_UINT16 command, CLR_UINT8 data[], CLR_UINT32 dataCount)
{
	return;
}
void DisplayInterface::DisplayBacklight(bool on) // true = on
{
}
void DisplayInterface::DisplayCommandDelay(CLR_INT16 delay)
{
	OS_DELAY(delay);
	return;
}
#pragma GCC diagnostic pop
