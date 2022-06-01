//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "Graphics.h"
#include "DisplayInterface.h"
#include "Display.h"

/*
ILI9341 is a 262,144-color single-chip SOC driver for a-TFT liquid crystal display with resolution of 240RGBx320
dots, comprising a 720-channel source driver, a 320-channel gate driver, 172,800 bytes GRAM for graphic
display data of 240RGBx320 dots, and power supply circuit.

ILI9341 supports parallel 8-/9-/16-/18-bit data bus MCU interface, 6-/16-/18-bit data bus RGB interface and
3-/4-line serial peripheral interface (SPI).

ILI9341 supports full color, 8-color display mode and sleep mode for precise power control by software.

Power saving mode:
   1. Sleep
   2. Deep standby

This implementation was initially written for 16 bit colour.
*/

/*
 Using the default endian order for transferring bytes
 Normal (MSB first, default)
  */

#define CommandData(c)                                                                                                 \
    c, (CLR_UINT8 *)(CLR_UINT8[c]) // Macro to simplify visualisation of passing pointer to parameters;

struct DisplayDriver g_DisplayDriver;
extern DisplayInterface g_DisplayInterface;
extern DisplayInterfaceConfig g_DisplayInterfaceConfig;

enum ILI9341_CMD : CLR_UINT8
{
    NOP = 0x00,
    SOFTWARE_RESET = 0x01,
    POWER_STATE = 0x10,
    Sleep_Out = 0x11,
    Invertion_Off = 0x20,
    Invertion_On = 0x21,
    Gamma_Set = 0x26,
    Display_OFF = 0x28,
    Display_ON = 0x29,
    Column_Address_Set = 0x2A,
    Page_Address_Set = 0x2B,
    Memory_Write = 0x2C,
    Colour_Set = 0x2D,
    Memory_Read = 0x2E,
    Partial_Area = 0x30,
    Memory_Access_Control = 0x36,
    Pixel_Format_Set = 0x3A,
    Memory_Write_Continue = 0x3C,
    Write_Display_Brightness = 0x51,
    Frame_Rate_Control_Normal = 0xB1,
    Frame_Rate_Control_2 = 0xB2,
    Frame_Rate_Control_3 = 0xB3,
    Invert_On = 0xB4,
    Display_Function_Control = 0xB6,
    Entry_Mode_Set = 0xB7,
    Power_Control_1 = 0xC0,
    Power_Control_2 = 0xC1,
    Power_Control_3 = 0xC2,
    Power_Control_4 = 0xC3,
    Power_Control_5 = 0xC4,
    VCOM_Control_1 = 0xC5,
    VCOM_Control_2 = 0xC7,
    Power_Control_A = 0xCB,
    Power_Control_B = 0xCF,
    Positive_Gamma_Correction = 0xE0,
    Negative_Gamma_Correction = 0XE1,
    Driver_Timing_Control_A = 0xE8,
    Driver_Timing_Control_B = 0xEA,
    Power_On_Sequence = 0xED,
    Enable_3G = 0xF2,
    Pump_Ratio_Control = 0xF7,
    Power_Control_6 = 0xFC,
};

enum ILI9341_Orientation : CLR_UINT8
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
    // Initialize ST7735S registers

    SetupDisplayAttributes();

    g_DisplayInterface.SendCommand(SOFTWARE_RESET);
    OS_DELAY(50);
    g_DisplayInterface.SendCommand(1, Sleep_Out);
    OS_DELAY(500); // Send Sleep Out command to display : no parameter
    g_DisplayInterface.SendCommand(4, Frame_Rate_Control_Normal, 0x01, 0x2C, 0x2D);
    g_DisplayInterface.SendCommand(4, Frame_Rate_Control_2, 0x01, 0x2C, 0x2D);
    g_DisplayInterface.SendCommand(7, Frame_Rate_Control_3, 0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D);
    g_DisplayInterface.SendCommand(2, Invert_On, 0x07);
    g_DisplayInterface.SendCommand(1, Invertion_On);
    g_DisplayInterface.SendCommand(2, Pixel_Format_Set, 0x55); // 0x55 -> 16 bit
    g_DisplayInterface.SendCommand(4, Power_Control_1, 0xA2, 0x02, 0x84);
    g_DisplayInterface.SendCommand(2, Power_Control_2, 0xC5);
    g_DisplayInterface.SendCommand(3, Power_Control_3, 0x0A, 0x00);
    g_DisplayInterface.SendCommand(3, Power_Control_4, 0x8A, 0x2A);
    g_DisplayInterface.SendCommand(3, Power_Control_5, 0x8A, 0xEE);
    g_DisplayInterface.SendCommand(4, VCOM_Control_1, 0x0E, 0xFF, 0xFF);
    g_DisplayInterface.SendCommand(
        17,
        Positive_Gamma_Correction,
        0x02,
        0x1c,
        0x07,
        0x12,
        0x37,
        0x32,
        0x29,
        0x2d,
        0x29,
        0x25,
        0x2B,
        0x39,
        0x00,
        0x01,
        0x03,
        0x10);
    g_DisplayInterface.SendCommand(
        17,
        Negative_Gamma_Correction,
        0x03,
        0x1d,
        0x07,
        0x06,
        0x2E,
        0x2C,
        0x29,
        0x2D,
        0x2E,
        0x2E,
        0x37,
        0x3F,
        0x00,
        0x00,
        0x02,
        0x10);

    g_DisplayInterface.SendCommand(1, Sleep_Out);
    g_DisplayInterface.SendCommand(1, Display_ON);
    OS_DELAY(100);                          // Send Sleep Out command to display : no parameter
    g_DisplayInterface.SendCommand(1, NOP); // End of sequence
    OS_DELAY(20);                           // Send Sleep Out command to display : no parameter

    SetDefaultOrientation();

    return true;
}

void DisplayDriver::SetupDisplayAttributes()
{
    // Define the LCD/TFT resolution
    Attributes.LongerSide = g_DisplayInterfaceConfig.Screen.width;
    Attributes.ShorterSide = g_DisplayInterfaceConfig.Screen.height;
    Attributes.PowerSave = PowerSaveState::NORMAL;
    Attributes.BitsPerPixel = 16;
    g_DisplayInterface.GetTransferBuffer(Attributes.TransferBuffer, Attributes.TransferBufferSize);
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
            g_DisplayInterface.SendCommand(
                2,
                Memory_Access_Control,
                (MADCTL_MY | MADCTL_MX | MADCTL_BGR)); // Landscape  + BGR
            break;
    }
    return true;
}

void DisplayDriver::SetDefaultOrientation()
{
    ChangeOrientation(LANDSCAPE);
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
            g_DisplayInterface.SendCommand(3, POWER_STATE, 0x00, 0x00); // leave sleep mode
            break;
        case PowerSaveState::SLEEP:
            g_DisplayInterface.SendCommand(3, POWER_STATE, 0x00, 0x01); // enter sleep mode
            break;
    }
    return;
}

void DisplayDriver::Clear()
{
    // Clear the ILI9342 controller frame
    SetWindow(0, 0, Attributes.Width - 1, Attributes.Height - 1);

    // Clear buffer
    memset(Attributes.TransferBuffer, 0x00, Attributes.TransferBufferSize);
    int totalBytesToClear = Attributes.Width * Attributes.Height * 2;
    int fullTransferBuffersCount = totalBytesToClear / Attributes.TransferBufferSize;
    int remainderTransferBuffer = totalBytesToClear % Attributes.TransferBufferSize;

    CLR_UINT8 command = Memory_Write;
    g_DisplayInterface.SendCommand(1, command);
    for (int i = 0; i < fullTransferBuffersCount; i++)
    {
        g_DisplayInterface.SendBytes(Attributes.TransferBuffer, Attributes.TransferBufferSize);
    }

    if (remainderTransferBuffer > 0)
    {
        g_DisplayInterface.SendBytes(Attributes.TransferBuffer, remainderTransferBuffer);
    }
}

void DisplayDriver::DisplayBrightness(CLR_INT16 brightness)
{
    _ASSERTE(brightness >= 0 && brightness <= 100);
    g_DisplayInterface.SendCommand(2, Write_Display_Brightness, (CLR_UINT8)brightness);
}

bool DisplayDriver::SetWindow(CLR_INT16 x1, CLR_INT16 y1, CLR_INT16 x2, CLR_INT16 y2)
{
    CLR_UINT8 Column_Address_Set_Data[4];
    Column_Address_Set_Data[0] = ((x1 + g_DisplayInterfaceConfig.Screen.x) >> 8) & 0xFF;
    Column_Address_Set_Data[1] = (x1 + g_DisplayInterfaceConfig.Screen.x) & 0xFF;
    Column_Address_Set_Data[2] = ((x2 + g_DisplayInterfaceConfig.Screen.x) >> 8) & 0xFF;
    Column_Address_Set_Data[3] = (x2 + g_DisplayInterfaceConfig.Screen.x) & 0xFF;
    g_DisplayInterface.SendCommand(
        5,
        Column_Address_Set,
        Column_Address_Set_Data[0],
        Column_Address_Set_Data[1],
        Column_Address_Set_Data[2],
        Column_Address_Set_Data[3]);

    CLR_UINT8 Page_Address_Set_Data[4];
    Page_Address_Set_Data[0] = ((y1 + g_DisplayInterfaceConfig.Screen.y) >> 8) & 0xFF;
    Page_Address_Set_Data[1] = (y1 + g_DisplayInterfaceConfig.Screen.y) & 0xFF;
    Page_Address_Set_Data[2] = ((y2 + g_DisplayInterfaceConfig.Screen.y) >> 8) & 0xFF;
    Page_Address_Set_Data[3] = (y2 + g_DisplayInterfaceConfig.Screen.y) & 0xFF;
    g_DisplayInterface.SendCommand(
        5,
        Page_Address_Set,
        Page_Address_Set_Data[0],
        Page_Address_Set_Data[1],
        Page_Address_Set_Data[2],
        Page_Address_Set_Data[3]);
    return true;
}

void DisplayDriver::BitBlt(int x, int y, int width, int height, CLR_UINT32 data[])
{
    // 16 bit colour  RRRRRGGGGGGBBBBB mode 565

    ASSERT((x >= 0) && ((x + width) <= Attributes.Width));
    ASSERT((y >= 0) && ((y + height) <= Attributes.Height));

    SetWindow(x, y, (x + width - 1), (y + height - 1));

    CLR_UINT16 *StartOfLine_src = (CLR_UINT16 *)&data[0];
    CLR_UINT8 *transferBufferIndex = Attributes.TransferBuffer;
    CLR_UINT32 transferBufferCount = Attributes.TransferBufferSize;
    CLR_UINT8 command = Memory_Write;

    g_DisplayInterface.SendCommand(1, command);
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
        StartOfLine_src += width;
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
