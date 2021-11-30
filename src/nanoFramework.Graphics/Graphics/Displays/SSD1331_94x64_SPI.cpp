//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "Graphics.h"
#include "DisplayInterface.h"
#include "Display.h"

/*
SSD1331 is a 96RGB x 64 Dot Matrix OLED/PLED Segment/Common Driver with Controller

SSD1331 supports parallel 8-/9-/16-bit data bus MCU interface and serial peripheral interface (SPI).

SSD1331 supports full color, 8-color display mode and sleep mode for precise power control by software.

Power saving mode:
   1. Sleep
   2. Deep standby

This implementation was initially written for 16 bit colour (665) SPI mode.
*/

struct DisplayDriver g_DisplayDriver;
extern DisplayInterface g_DisplayInterface;

enum SSD1331_CMD : CLR_UINT8
{
    Set_Column_Address = 0x15,
    Set_Row_Address = 0x75,

    Set_Contrast_A = 0x81,
    Set_Contrast_B = 0x82,
    Set_Contrast_C = 0x83,

    Master_Current_Control = 0x87,

    Set_Second_Pre_charge_Speed_A = 0x8A,
    Set_Second_Pre_charge_Speed_B = 0x8B,
    Set_Second_Pre_charge_Speed_C = 0x8C,

    Remap_and_Colour_Depth_setting = 0xA0,

    Set_Display_Start_Line = 0xA1,
    Set_Display_Offset = 0xA2,
    Set_Display_Mode_Normal = 0xA4,
    Set_Display_Mode_Entire_On = 0xA5,
    Set_Display_Mode_Entire_Off = 0xA6,
    Set_Display_Mode_Inverse = 0xA7,

    Set_Multiplex_Ratio = 0xA8,
    Dim_Mode_Setting = 0xAB,
    Set_Master_Configuration = 0xAD,
    Set_Display_On_Dim = 0xAC,
    Set_Display_Off_Sleep = 0xAE,
    Set_Display_On_Normal = 0xAF,
    Power_Save_Mode = 0xB0,
    Phase1_2_Period = 0xB1,
    Display_Clock_Divider = 0xB3,
    Set_Gray_Scale_Table = 0xB8,
    Enable_Linear_Gray_Scale_Table = 0xB9,
    Set_Pre_Charge_Level = 0xBB,
    Set_Vcomh = 0xBE,
    Set_Command_Lock = 0xFD,

    Draw_Line = 0x21,
    Draw_Rectangle = 0x22,
    Copy = 0x23,
    Dim_Window = 0x24,
    Clear_Window = 0x25,
    Fill_Enable_Disable = 0x26,
    Scrolling_Setup = 0x27,
    Deactivate_Scolling = 0x2E,
    Activate_Scrolling = 0x2F
};

enum SSD1331_Orientation : CLR_UINT8
{
    MADCTL_MH = 0x04, // sets the Horizontal Refresh, 0=Left-Right and 1=Right-Left
    MADCTL_ML = 0x10, // sets the Vertical Refresh, 0=Top-Bottom and 1=Bottom-Top
    MADCTL_MV = 0x20, // sets the Row/Column Swap, 0=Normal and 1=Swapped
    MADCTL_MX = 0x40, // sets the Column Order, 0=Left-Right and 1=Right-Left
    MADCTL_MY = 0x80, // sets the Row Order, 0=Top-Bottom and 1=Bottom-Top

    MADCTL_BGR = 0x08 // Blue-Green-Red pixel order
};

bool DisplayDriver::Initialize()
{
    // Set SPI interface to Command mode for all command bytes
    g_DisplayInterface.SetCommandMode(1);

    // Initialize SSD1331 registers
    SetupDisplayAttributes();

    g_DisplayInterface.SendCommand(1, SSD1331_CMD::Set_Display_Off_Sleep);

    // Orientation & colour mapping
    SetDefaultOrientation();

    g_DisplayInterface.SendCommand(2, SSD1331_CMD::Set_Display_Start_Line, 0x00);

    g_DisplayInterface.SendCommand(2, SSD1331_CMD::Set_Display_Offset, 0x00);

    g_DisplayInterface.SendCommand(1, SSD1331_CMD::Set_Display_Mode_Normal);

    g_DisplayInterface.SendCommand(2, SSD1331_CMD::Set_Multiplex_Ratio, 0x3F);

    g_DisplayInterface.SendCommand(2, SSD1331_CMD::Set_Master_Configuration, 0x8E);

    g_DisplayInterface.SendCommand(2, SSD1331_CMD::Power_Save_Mode, 0x0B);

    g_DisplayInterface.SendCommand(2, SSD1331_CMD::Phase1_2_Period, 0x31);

    g_DisplayInterface.SendCommand(2, SSD1331_CMD::Display_Clock_Divider, 0xF0);

    g_DisplayInterface.SendCommand(2, SSD1331_CMD::Set_Second_Pre_charge_Speed_A, 0x64);

    g_DisplayInterface.SendCommand(2, SSD1331_CMD::Set_Second_Pre_charge_Speed_B, 0x78);

    g_DisplayInterface.SendCommand(2, SSD1331_CMD::Set_Second_Pre_charge_Speed_C, 0x64);

    g_DisplayInterface.SendCommand(2, SSD1331_CMD::Set_Pre_Charge_Level, 0x3A);

    g_DisplayInterface.SendCommand(2, SSD1331_CMD::Set_Vcomh, 0x3E);

    g_DisplayInterface.SendCommand(2, SSD1331_CMD::Master_Current_Control, 0x06);

    g_DisplayInterface.SendCommand(2, SSD1331_CMD::Set_Contrast_A, 0x91);

    g_DisplayInterface.SendCommand(2, SSD1331_CMD::Set_Contrast_B, 0x50);

    g_DisplayInterface.SendCommand(2, SSD1331_CMD::Set_Contrast_C, 0x7D);

    g_DisplayInterface.SendCommand(1, SSD1331_CMD::Set_Display_On_Normal);

    return true;
}

void DisplayDriver::SetupDisplayAttributes()
{
    // Define the LCD/TFT resolution
    Attributes.LongerSide = 96;
    Attributes.ShorterSide = 64;
    Attributes.PowerSave = PowerSaveState::NORMAL;
    Attributes.BitsPerPixel = 16;
    g_DisplayInterface.GetTransferBuffer(Attributes.TransferBuffer, Attributes.TransferBufferSize);
    return;
}

bool DisplayDriver::ChangeOrientation(DisplayOrientation orientation)
{
    uint8_t options = 0;

    options |= 0x40; // RGB mapping, 65K colour

    switch (orientation)
    {
        case PORTRAIT:
        case PORTRAIT180:
            return false;

        case LANDSCAPE180:
            options |= 0x70;
            Attributes.Height = Attributes.ShorterSide;
            Attributes.Width = Attributes.LongerSide;
            break;

        case LANDSCAPE:
            options |= 0x72;
            Attributes.Height = Attributes.ShorterSide;
            Attributes.Width = Attributes.LongerSide;
            break;
    }

    g_DisplayInterface.SendCommand(1, SSD1331_CMD::Remap_and_Colour_Depth_setting);
    g_DisplayInterface.SendCommand(1, options);
    return true;
}

void DisplayDriver::SetDefaultOrientation()
{
    ChangeOrientation(LANDSCAPE);
}

bool DisplayDriver::SetWindow(CLR_INT16 x1, CLR_INT16 y1, CLR_INT16 x2, CLR_INT16 y2)
{
    // Start & End column address
    g_DisplayInterface.SendCommand(3, SSD1331_CMD::Set_Column_Address, x1, x2);

    // Start & End row address
    g_DisplayInterface.SendCommand(3, SSD1331_CMD::Set_Row_Address, y1, y2);

    return true;
}

bool DisplayDriver::Uninitialize()
{
    Clear();
    // Anything else to Uninitialize?
    return TRUE;
}

void DisplayDriver::PowerSave(PowerSaveState powerState)
{
    switch (powerState)
    {
        default:
            // Illegal fall through to Power on
        case PowerSaveState::NORMAL:
            g_DisplayInterface.SendCommand(1, SSD1331_CMD::Power_Save_Mode);
            g_DisplayInterface.SendCommand(1, 0x0B);
            break;
        case PowerSaveState::SLEEP:
            g_DisplayInterface.SendCommand(1, SSD1331_CMD::Power_Save_Mode);
            g_DisplayInterface.SendCommand(1, 0x1A);
            break;
    }
    return;
}

void DisplayDriver::Clear()
{
    // Clear the SSD1331 controller
    g_DisplayInterface.SendCommand(1, SSD1331_CMD::Clear_Window);
    g_DisplayInterface.SendCommand(1, 0);
    g_DisplayInterface.SendCommand(1, 0);
    g_DisplayInterface.SendCommand(1, Attributes.Width - 1);
    g_DisplayInterface.SendCommand(1, Attributes.Height - 1);
    OS_DELAY(1);
    return;
}

void DisplayDriver::DisplayBrightness(CLR_INT16 brightness)
{
    _ASSERTE(brightness >= 0 && brightness <= 100);
    //    g_DisplayInterface.SendCommand(2,Write_Display_Brightness, (CLR_UINT8)brightness);
    return;
}

void DisplayDriver::BitBlt(int x, int y, int width, int height, CLR_UINT32 data[])
{
    // 16 bit colour  RRRRRGGGGGGBBBBB mode 565

    ASSERT((x >= 0) && ((x + width) <= Attributes.Width));
    ASSERT((y >= 0) && ((y + height) <= Attributes.Height));

    CLR_UINT16 *StartOfLine_src = (CLR_UINT16 *)&data[0];

    SetWindow(x, y, (x + width - 1), (y + height - 1));

    // Position to offset in data[] for stat of window
    CLR_UINT16 offset = (y * Attributes.Width) + x;
    StartOfLine_src += offset;

    CLR_UINT8 *transferBufferIndex = Attributes.TransferBuffer;
    CLR_UINT32 transferBufferCount = Attributes.TransferBufferSize;

    while (height--)
    {
        CLR_UINT16 *src;
        int xCount;

        src = StartOfLine_src;
        xCount = width;

        while (xCount--)
        {
            CLR_UINT16 dt = *src++;
            *transferBufferIndex++ = (dt >> 8);
            *transferBufferIndex++ = dt & 0xff;
            transferBufferCount -= 2;

            // Send over SPI if no room for another 2 bytes
            if (transferBufferCount < 1)
            {
                // Transfer buffer full, send it
                g_DisplayInterface.SendBytes(
                    Attributes.TransferBuffer,
                    (Attributes.TransferBufferSize - transferBufferCount));

                // Reset transfer ptrs/count
                transferBufferIndex = Attributes.TransferBuffer;
                transferBufferCount = Attributes.TransferBufferSize;
            }
        }

        // Next row in data[]
        StartOfLine_src += Attributes.Width;
    }

    // Send remaining data in transfer buffer to SPI
    if (transferBufferCount < Attributes.TransferBufferSize)
    {
        // Transfer buffer full, send it
        g_DisplayInterface.SendBytes(Attributes.TransferBuffer, (Attributes.TransferBufferSize - transferBufferCount));
    }

    return;
}

CLR_UINT32 DisplayDriver::PixelsPerWord()
{
    return (32 / Attributes.BitsPerPixel);
}

CLR_UINT32 DisplayDriver::WidthInWords()
{
    return ((Attributes.Width + (PixelsPerWord() - 1)) / PixelsPerWord());
}

CLR_UINT32 DisplayDriver::SizeInWords()
{
    return (WidthInWords() * Attributes.Height);
}

CLR_UINT32 DisplayDriver::SizeInBytes()
{
    return (SizeInWords() * sizeof(CLR_UINT32));
}
