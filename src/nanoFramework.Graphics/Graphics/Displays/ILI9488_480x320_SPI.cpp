//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "Graphics.h"
#include "DisplayInterface.h"
#include "Display.h"

/*
IThe ILI9488 is a 16.7M single-chip SoC driver for a-Si TFT liquid crystal display panels with a resolution of
320(RGB) x 480 dots. The ILI9488 is comprised of a 960-channel source driver, a 480-channel gate driver,
345,600 bytes GRAM for graphic data of 320 (RGB) x 480 dots, and power supply circuit.
The ILI9488 supports parallel DBI Type B 8-/9-/16-/18-/24-bit data bus interfaces and DBI Type C 3-/4-line serial
peripheral interfaces (SPI) to input commands. The ILI9488 supports DPI (16-/18-/24-bit) data bus for video image
display. For MIPI*-DSI* high-speed interface mode, the ILI9488 also provides one data lane and one clock lane
that can support up to 500Mbps on MIPI-DSI link.
*/

/*
 Using the default endian order for transferring bytes
 Normal (MSB first, default)
  */

#define CommandData(c)                                                                                                 \
    c, (CLR_UINT8 *)(CLR_UINT8[c]) // Macro to simplify visualisation of passing pointer to parameters;

// #define NUMBER_OF_LINES       8
// #define SPI_MAX_TRANSFER_SIZE (480 * 3 * NUMBER_OF_LINES) // 480 pixels 3 words wide ( 24 bit colour)
// CLR_UINT8 dataBuffer[SPI_MAX_TRANSFER_SIZE];

struct DisplayDriver g_DisplayDriver;
extern DisplayInterface g_DisplayInterface;
extern DisplayInterfaceConfig g_DisplayInterfaceConfig;

enum ILI9488_CMD : CLR_UINT8
{
    NOP = 0x00,
    SOFTWARE_RESET = 0x01,
    POWER_STATE = 0x10,
    Sleep_Out = 0x11,
    Noron = 0x13,
    Invert_On = 0x21,
    Invert_Off = 0x20,
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
    Interface_Signal_Control = 0xB0,
    Frame_Rate_Control_Normal = 0xB1,
    Inversion_Control = 0xB4,
    Display_Function_Control = 0xB6,
    Entry_Mode_Set = 0xB7,
    Power_Control_1 = 0xC0,
    Power_Control_2 = 0xC1,
    VCOM_Control_1 = 0xC5,
    VCOM_Control_2 = 0xC7,
    External_Command = 0xC8,
    Power_Control_A = 0xCB,
    Power_Control_B = 0xCF,
    Positive_Gamma_Correction = 0xE0,
    Negative_Gamma_Correction = 0XE1,
    Driver_Timing_Control_A = 0xE8,
    Driver_Timing_Control_B = 0xEA,
    Set_Image_Function = 0xE9,
    Power_On_Sequence = 0xED,
    Enable_3G = 0xF2,
    Interface_Control = 0xF6,
    Pump_Ratio_Control = 0xF7
};

enum ILI9488_Orientation : CLR_UINT8
{
    MADCTL_MH = 0x04, // sets the Horizontal Refresh, 0=Left-Right and 1=Right-Left
    MADCTL_ML = 0x10, // sets the Vertical Refresh, 0=Top-Bottom and 1=Bottom-Top
    MADCTL_MV = 0x20, // sets the Row/Column Swap, 0=Normal and 1=Swapped
    MADCTL_MX = 0x40, // sets the Column Order, 0=Left-Right and 1=Right-Left
    MADCTL_MY = 0x80, // sets the Row Order, 0=Top-Bottom and 1=Bottom-Top

    MADCTL_BGR = 0x08, // Blue-Green-Red pixel order
};

bool DisplayDriver::Initialize()
{
    // Initialize ILI9488 registers

    SetupDisplayAttributes();

    g_DisplayInterface.SendCommand(
        16,
        Positive_Gamma_Correction,
        0x00,
        0x03,
        0x09,
        0x08,
        0x16,
        0x0A,
        0x3F,
        0x78,
        0x4C,
        0x09,
        0x0A,
        0x08,
        0x16,
        0x1A,
        0x0F);
    g_DisplayInterface.SendCommand(
        16,
        Negative_Gamma_Correction,
        0x00,
        0x16,
        0x19,
        0x03,
        0x0F,
        0x05,
        0x32,
        0x45,
        0x46,
        0x04,
        0x0E,
        0x0D,
        0x35,
        0x37,
        0x0F);
    g_DisplayInterface.SendCommand(3, Power_Control_1, 0x17, 0x15);
    g_DisplayInterface.SendCommand(2, Power_Control_2, 0x41);
    g_DisplayInterface.SendCommand(4, VCOM_Control_1, 0x00, 0x12, 0x80);
    g_DisplayInterface.SendCommand(2, Memory_Access_Control, 0x48);
    g_DisplayInterface.SendCommand(2, Pixel_Format_Set, 0x66); // 18 bit for SPI
    g_DisplayInterface.SendCommand(2, Interface_Signal_Control, 0x80);
    g_DisplayInterface.SendCommand(2, Frame_Rate_Control_Normal, 0xA0);
    g_DisplayInterface.SendCommand(2, Inversion_Control, 0x02);
    g_DisplayInterface.SendCommand(3, Display_Function_Control, 0x02, 0x02, 0x3B);
    g_DisplayInterface.SendCommand(2, Set_Image_Function, 0x00);
    g_DisplayInterface.SendCommand(5, Pump_Ratio_Control, 0xA9, 0x51, 0x2C, 0x82);
    g_DisplayInterface.SendCommand(1, Sleep_Out);
    OS_DELAY(120);
    g_DisplayInterface.SendCommand(1, Display_ON);
    OS_DELAY(25);

    SetDefaultOrientation();

    return true;
}

void DisplayDriver::SetupDisplayAttributes()
{
    // Define the LCD/TFT resolution
    Attributes.LongerSide = 480;
    Attributes.ShorterSide = 320;
    Attributes.PowerSave = PowerSaveState::NORMAL;
    Attributes.BitsPerPixel = 18;
    g_DisplayInterface.GetTransferBuffer(Attributes.TransferBuffer, Attributes.TransferBufferSize);
    return;
}

bool DisplayDriver::ChangeOrientation(DisplayOrientation orientation)
{
    switch (orientation)
    {
        case DisplayOrientation::DisplayOrientation_Portrait:
            Attributes.Height = Attributes.LongerSide;
            Attributes.Width = Attributes.ShorterSide;
            g_DisplayInterface.SendCommand(2, Memory_Access_Control,
                                           (MADCTL_MX | MADCTL_BGR)); // Portrait + BGR
            break;
        case DisplayOrientation::DisplayOrientation_Portrait180:
            Attributes.Height = Attributes.LongerSide;
            Attributes.Width = Attributes.ShorterSide;
            g_DisplayInterface.SendCommand(2, Memory_Access_Control,
                                           (MADCTL_MY | MADCTL_BGR)); // Portrait 180 + BGR
            break;
        case DisplayOrientation::DisplayOrientation_Landscape:
            Attributes.Height = Attributes.ShorterSide;
            Attributes.Width = Attributes.LongerSide;
            g_DisplayInterface.SendCommand(2, Memory_Access_Control,
                                           (MADCTL_MV | MADCTL_BGR)); // Landscape + BGR
            break;
        case DisplayOrientation::DisplayOrientation_Landscape180:
            Attributes.Height = Attributes.ShorterSide;
            Attributes.Width = Attributes.LongerSide;
            g_DisplayInterface.SendCommand(
                2,
                Memory_Access_Control,
                (MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR)); // Landscape 180 + BGR
            break;
    }
    return true;
}

void DisplayDriver::SetDefaultOrientation()
{
    ChangeOrientation(DisplayOrientation::DisplayOrientation_Landscape180);
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
    SetWindow(0, 0, Attributes.Width - 1, Attributes.Height - 1);

    g_DisplayInterface.SendCommand(1, Memory_Write);
    g_DisplayInterface.FillData16(0, Attributes.Width * Attributes.Height * 3 / 2);
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

void DisplayDriver::BitBlt(
    int srcX,
    int srcY,
    int width,
    int height,
    int stride,
    int screenX,
    int screenY,
    CLR_UINT32 data[])
{
    ASSERT((screenX >= 0) && ((screenX + width) <= Attributes.Width));
    ASSERT((screenY >= 0) && ((screenY + height) <= Attributes.Height));

    SetWindow(screenX, screenY, (screenX + width - 1), (screenY + height - 1));

    g_DisplayInterface.SendCommand(1, Memory_Write);

    uint32_t numPixels = width * height;
    uint32_t count = 0;

    CLR_UINT8 *TransferBuffer = Attributes.TransferBuffer;
    CLR_UINT32 TransferBufferSize = Attributes.TransferBufferSize;

    // only 18/24 bit is supported on SPI
    for (uint32_t i = 0; i < numPixels; i++)
    {
        uint32_t element = data[i / 2]; // Each uint32 stores 2 pixels
        uint16_t color = (i % 2 == 0) ? (element & 0xFFFF) : (element >> 16);

        uint8_t b = color & 0x1F;
        uint8_t g = (color >> 5) & 0x3F;
        uint8_t r = (color >> 11) & 0x1F;

        b = (b << 3) | (b >> 2);
        g = (g << 2) | (g >> 4);
        r = (r << 3) | (r >> 2);

        TransferBuffer[count++] = b;
        TransferBuffer[count++] = g;
        TransferBuffer[count++] = r;

        // can't fit another 3 bytes
        if (count + 3 > TransferBufferSize - 1)
        {
            g_DisplayInterface.SendBytes(TransferBuffer, count);
            count = 0;
        }
    }
    g_DisplayInterface.SendBytes(TransferBuffer, count);
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
