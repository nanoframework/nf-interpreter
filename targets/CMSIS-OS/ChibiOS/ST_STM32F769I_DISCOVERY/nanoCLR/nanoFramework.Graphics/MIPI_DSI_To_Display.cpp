//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

// -------------------------------------------------------
// For the STM32F769IDiscovery evaluation board
// Code adapted from online MBED example DISCO-F769NI_TOUCHSCREEN
// This driver is used to drive directly in video mode a LCD TFT using the DSI interface.


#include "DisplayInterface.h"
#include <nanoPAL.h>
#include <target_platform.h>
#include "stm32f769i_discovery_lcd.h"

#define UNUSED(X) (void)X      /* To avoid gcc/g++ warnings */

struct DisplayInterface g_DisplayInterface;

// This is the Display Interface block
bool DisplayInterface::InitializeDisplayInterface()
{
    BSP_LCD_Init();
    return true;
}
void DisplayInterface::GetTransferBuffer(CLR_UINT8*& TransferBuffer, CLR_UINT32& TransferBufferSize)
{
    UNUSED(TransferBuffer);
    UNUSED(TransferBufferSize);
    return;
}
void DisplayInterface::SendCommandByte(CLR_UINT8 command)
{
    DSI_IO_WriteCmd(0, (uint8_t*)&command);
    return;
}
void DisplayInterface::SendCommandWord(CLR_UINT16 commandWord)
{
    UNUSED(commandWord);
    return;
}
void DisplayInterface::SendData(CLR_UINT8 data[], CLR_UINT32 dataCount)
{
    DSI_IO_WriteCmd(dataCount, (uint8_t*)data);
    return;
}
void DisplayInterface::SendCommandByteAndData(CLR_UINT8 command, CLR_UINT8 data[], CLR_UINT32 dataCount)
{
    UNUSED(command);
    UNUSED(data);
    UNUSED(dataCount);
    return;
}
void DisplayInterface::SendCommandWordAndData(CLR_UINT16 command, CLR_UINT8 data[], CLR_UINT32 dataCount)
{
    UNUSED(command);
    UNUSED(data);
    UNUSED(dataCount);
    return;
}
void DisplayInterface::DisplayBacklight(bool on) // true = on
{
    UNUSED(on);

}
void DisplayInterface::DisplayCommandDelay(CLR_INT16 delay)
{
    OS_DELAY(delay);
    return;
}

