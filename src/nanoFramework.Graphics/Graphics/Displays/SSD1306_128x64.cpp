//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "Graphics.h"
#include "DisplayInterface.h"
#include "Display.h"

/*
SSD1306 ar small mono display of either 128x64 or 128x32

The interface is I2C

*/

struct DisplayDriver g_DisplayDriver;
extern DisplayInterface g_DisplayInterface;

enum SSD1306_CMD : CLR_UINT8
{
    Set_Memory_Addressing_Mode = 0x20,

    // Set Column address - 0x21, start adr, end address
    Set_Column_Address = 0x21,

    // Set Page address - 0x22, page start adr, page end address
    Set_Page_address = 0x22,

    Memory_Write = 0x40,

    // Set contrast 0x81 xx
    Set_Contrast = 0x81,

    // Enable / Disable charge pump
    Charge_Pump = 0x8D,

    Set_Segment_remap127 = 0xA1,

    // Display Normal / Inverse
    Set_Normal = 0xA6,
    Set_Inversion = 0xA6,

    // Display On/Off
    Display_OFF = 0xAE,
    Display_ON = 0xAF,

    Set_COM_Scan_x = 0xC8
};

enum _Orientation : CLR_UINT8
{
    MADCTL_MH = 0x04, // sets the Horizontal Refresh, 0=Left-Right and 1=Right-Left
    MADCTL_ML = 0x10, // sets the Vertical Refresh, 0=Top-Bottom and 1=Bottom-Top
    MADCTL_MV = 0x20, // sets the Row/Column Swap, 0=Normal and 1=Swapped
    MADCTL_MX = 0x40, // sets the Column Order, 0=Left-Right and 1=Right-Left
    MADCTL_MY = 0x80, // sets the Row Order, 0=Top-Bottom and 1=Bottom-Top

    MADCTL_BGR = 0x08 // Blue-Green-Red pixel order, 0 = RGB, 1 = BGR
};

bool DisplayDriver::Initialize()
{
    // Required for SPI interface only, I2C does nothing
    g_DisplayInterface.SetCommandMode(1);

    SetupDisplayAttributes();

    // Initialize SSD1306 registers
    g_DisplayInterface.SendCommand(2, SSD1306_CMD::Charge_Pump, 0x14);                // Turn on the charge pump
    g_DisplayInterface.SendCommand(2, SSD1306_CMD::Set_Memory_Addressing_Mode, 0x00); // Set horizontal addressing mode

    SetDefaultOrientation();

    g_DisplayInterface.SendCommand(1, SSD1306_CMD::Display_ON); // Display on

    return true;
}
void DisplayDriver::SetupDisplayAttributes()
{
    // Define the Display resolution
    Attributes.LongerSide = 128;
    Attributes.ShorterSide = 64;
    Attributes.PowerSave = PowerSaveState::NORMAL;
    Attributes.BitsPerPixel = 1;
    g_DisplayInterface.GetTransferBuffer(Attributes.TransferBuffer, Attributes.TransferBufferSize);
    return;
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
            // illegal fall through to Power on
        case PowerSaveState::NORMAL:
            // leave sleep mode
            break;
        case PowerSaveState::SLEEP:
            // enter sleep mode
            break;
    }
    return;
}

void DisplayDriver::DisplayBrightness(CLR_INT16 brightness)
{
    _ASSERTE(brightness >= 0 && brightness <= 100);

    return;
}

bool DisplayDriver::ChangeOrientation(DisplayOrientation orientation)
{
    switch (orientation)
    {
        case PORTRAIT:
        case PORTRAIT180:
            return false;

        case LANDSCAPE:
        case LANDSCAPE180:
            Attributes.Height = Attributes.ShorterSide;
            Attributes.Width = Attributes.LongerSide;

            g_DisplayInterface.SendCommand(1, SSD1306_CMD::Set_Segment_remap127); // Flip the display horizontally
            g_DisplayInterface.SendCommand(1, SSD1306_CMD::Set_COM_Scan_x);       // Flip the display vertically
            break;
    }
    return true;
}

void DisplayDriver::SetDefaultOrientation()
{
    ChangeOrientation(LANDSCAPE);
}

bool DisplayDriver::SetWindow(CLR_INT16 x1, CLR_INT16 y1, CLR_INT16 x2, CLR_INT16 y2)
{
    // Start & End column address
    g_DisplayInterface.SendCommand(3, Set_Column_Address, x1, x2);

    // Start & End page address  0 to 7
    g_DisplayInterface.SendCommand(3, Set_Page_address, (y1 / 8), (y2 / 8));

    return true;
}

void DisplayDriver::Clear()
{
    // Clear the SSD1306 controller
    SetWindow(0, 0, Attributes.Width - 1, Attributes.Height - 1);
    memset(Attributes.TransferBuffer, 0, Attributes.TransferBufferSize);

    for (int row = 0; row < Attributes.Height / 8; row++)
    {
        g_DisplayInterface.WriteToFrameBuffer(Memory_Write, Attributes.TransferBuffer, Attributes.Width);
    }
}
//
//
//
// data[] reference to whole screen bitmap
//
void DisplayDriver::BitBlt(int x, int y, int width, int height, CLR_UINT32 data[])
{
    // // With the current design the Colour data is packed into the lower two bytes of each data array element
    // // 16 bit colour  RRRRRGGGGGGBBBBB mode 565
    ASSERT((x >= 0) && ((x + width) <= Attributes.Width));
    ASSERT((y >= 0) && ((y + height) <= Attributes.Height));

    CLR_UINT16 *pui16Data = (CLR_UINT16 *)data;

    // Round y position down as we can only deal with rows of 8 at a time
    int offfset = y % 8;
    y -= offfset;

    // Increase height for change in y
    height += offfset;

    // Round up height to multiple of 8
    height = (height + 7) & 0xfffffff8;

    // Check not too big
    if ((y + height) > Attributes.Height)
    {
        height = Attributes.Height - y;
    }

    // Find position in buffer for start of data in window
    pui16Data += x + (y * Attributes.Width);

    // Set window for bitblt
    SetWindow(x, y, x + width - 1, y + height - 1);

    CLR_INT16 firstPageToUpdate = y / 8;
    CLR_INT16 lastPageToUpdate = (y + height - 1) / 8;

    CLR_UINT8 *pui8Buf = Attributes.TransferBuffer;
    CLR_UINT32 numberOfBytesPerTransfer = width;

    for (int page = firstPageToUpdate; page <= lastPageToUpdate; page++)
    {
        // Convert internal bitmap to dispay Page of 8 lines
        memset(pui8Buf, 0, 128);
        uint8_t mask = 0x01;

        // For each line in a page
        for (uint iy = 0; iy < 8; iy++)
        {
            CLR_UINT16 *pData = pui16Data;

            // for each pixel of width
            for (int i = 0; i < width; i++)
            {
                if (*pData++)
                {
                    pui8Buf[i] = pui8Buf[i] | mask;
                }
            }
            mask <<= 1;
            pui16Data += Attributes.Width; // Next row
        }

        g_DisplayInterface.WriteToFrameBuffer(Memory_Write, Attributes.TransferBuffer, numberOfBytesPerTransfer);
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
