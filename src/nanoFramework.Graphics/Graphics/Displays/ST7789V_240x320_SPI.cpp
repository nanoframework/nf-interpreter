//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "Graphics.h"
#include "DisplayInterface.h"
#include "Display.h"

/*
ST7789V is a 262,144-color single-chip SOC driver for a-TFT liquid crystal display with resolution of 240RGBx320
dots, comprising a 720-channel source driver, a 320-channel gate driver, 172,800 bytes GRAM for graphic
display data of 240RGBx320 dots, and power supply circuit.

ST7789V supports parallel 8-/9-/16-/18-bit data bus MCU interface, 6-/16-/18-bit data bus RGB interface and
3-/4-line serial peripheral interface (SPI).

ST7789V supports full color, 8-color display mode and sleep mode for precise power control by software.

Power saving mode:
   1. Sleep
   2. Deep standby

This implementation was initially written for 16 bit colour, SPI interface of a ESP32-WROVER-KIT-v3

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

enum ST7789V_CMD : CLR_UINT8
{
    NOP = 0x00,
    SOFTWARE_RESET = 0x01,
    Sleep_IN = 0x10,
    Sleep_OUT = 0x11,
    Invertion_Off = 0x20,
    Invertion_On = 0x21,
    Display_OFF = 0x28,
    Display_ON = 0x29,
    Column_Address_Set = 0x2A,
    Row_Address_Set = 0x2B,
    Memory_Write = 0x2C,
    Memory_Read = 0x2E,
    Partial_Area = 0x30,
    Memory_Access_Control = 0x36,
    Pixel_Format_Set = 0x3A,
    Memory_Write_Continue = 0x3C,
    Write_Display_Brightness = 0x51,
    Porch_Setting = 0xB2,
    Gate_Control = 0xB7,
    VCOMS_Setting = 0xBB,
    LCM_Control = 0xC0,
    VDV_VRH_Command_Enable = 0xC2,
    VRH_Set = 0xC3,
    VDV_Set = 0xC4,
    Frame_Rate_Control = 0xC6,
    Power_Control_1 = 0xD0,
    Positive_Voltage_Gamma = 0xE0,
    Negative_Voltage_Gamma = 0xE1,
    Read_ID1 = 0xDA,
    Read_ID2 = 0xDB,
    Read_ID3 = 0xDC,
};

enum ST7789V_Orientation : CLR_UINT8
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
    // Initialize ST7789V registers

    SetupDisplayAttributes();

    // g_DisplayInterface.SendCommand(2, Memory_Access_Control, MADCTL_MV | MADCTL_MX | MADCTL_BGR);
    SetDefaultOrientation();

    g_DisplayInterface.SendCommand(2, Pixel_Format_Set, 0x55);
    g_DisplayInterface.SendCommand(6, Porch_Setting, 0x0c, 0x0c, 0x00, 0x33, 0x33);
    g_DisplayInterface.SendCommand(2, Gate_Control, 0x35);
    g_DisplayInterface.SendCommand(2, VCOMS_Setting, 0x2B);
    g_DisplayInterface.SendCommand(2, LCM_Control, 0x2C);
    g_DisplayInterface.SendCommand(3, VDV_VRH_Command_Enable, 0x01, 0xFF);
    g_DisplayInterface.SendCommand(2, VRH_Set, 0x11);
    g_DisplayInterface.SendCommand(2, VDV_Set, 0x20);
    g_DisplayInterface.SendCommand(2, Frame_Rate_Control, 0x0f); // 39Hz
    g_DisplayInterface.SendCommand(3, Power_Control_1, 0xA4, 0xA1);
    g_DisplayInterface.SendCommand(
        15,
        Positive_Voltage_Gamma,
        0xD0,
        0x00,
        0x05,
        0x0E,
        0x15,
        0x0D,
        0x37,
        0x43,
        0x47,
        0x09,
        0x15,
        0x12,
        0x16,
        0x19);
    g_DisplayInterface.SendCommand(
        15,
        Negative_Voltage_Gamma,
        0xD0,
        0x00,
        0x05,
        0x0D,
        0x0C,
        0x06,
        0x2D,
        0x44,
        0x40,
        0x0E,
        0x1C,
        0x18,
        0x16,
        0x19);
    g_DisplayInterface.SendCommand(1, Invertion_On);
    g_DisplayInterface.SendCommand(1, Sleep_OUT);
    OS_DELAY(20); // Send Sleep Out command to display : no parameter
    g_DisplayInterface.SendCommand(1, Display_ON);
    OS_DELAY(20);                           // Send Display ON command to display : no parameter
    g_DisplayInterface.SendCommand(1, NOP); // End of sequence
    OS_DELAY(20);

    return true;
}
void DisplayDriver::SetupDisplayAttributes()
{
    // Define the LCD/TFT resolution
    Attributes.LongerSide = 320;
    Attributes.ShorterSide = 240;
    Attributes.PowerSave = PowerSaveState::NORMAL;
    Attributes.BitsPerPixel = 16;
    g_DisplayInterface.GetTransferBuffer(Attributes.TransferBuffer, Attributes.TransferBufferSize);
    return;
}

bool DisplayDriver::ChangeOrientation(DisplayOrientation orientation)
{
    switch (orientation)
    {
        case DisplayOrientation::DisplayOrientation_Portrait:
        case DisplayOrientation::DisplayOrientation_Portrait180:
            return false;

        case DisplayOrientation::DisplayOrientation_Landscape:
        case DisplayOrientation::DisplayOrientation_Landscape180:
            Attributes.Height = Attributes.ShorterSide;
            Attributes.Width = Attributes.LongerSide;
            g_DisplayInterface.SendCommand(2, Memory_Access_Control,
                                           (MADCTL_ML | MADCTL_BGR)); // Landscape  + BGR
            break;
    }
    return true;
}

void DisplayDriver::SetDefaultOrientation()
{
    ChangeOrientation(DisplayOrientation::DisplayOrientation_Landscape);
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
            g_DisplayInterface.SendCommand(3, Sleep_IN, 0x00, 0x00); // leave sleep mode
            break;
        case PowerSaveState::SLEEP:
            g_DisplayInterface.SendCommand(3, Sleep_IN, 0x00, 0x01); // enter sleep mode
            break;
    }
    return;
}

void DisplayDriver::Clear()
{
    // Clear the ST7789V2 controller frame
    SetWindow(0, 0, Attributes.Width - 1, Attributes.Height - 1);

    g_DisplayInterface.SendCommand(1, Memory_Write);

    g_DisplayInterface.FillData16(0, Attributes.Width * Attributes.Height);
}

void DisplayDriver::DisplayBrightness(CLR_INT16 brightness)
{
    _ASSERTE(brightness >= 0 && brightness <= 100);
    g_DisplayInterface.SendCommand(2, Write_Display_Brightness, (CLR_UINT8)brightness);
    return;
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
        Row_Address_Set,
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
    // 16 bit colour  RRRRRGGGGGGBBBBB mode 565

    ASSERT((screenX >= 0) && ((screenX + width) <= Attributes.Width));
    ASSERT((screenY >= 0) && ((screenY + height) <= Attributes.Height));

    SetWindow(screenX, screenY, (screenX + width - 1), (screenY + height - 1));

    g_DisplayInterface.SendCommand(1, Memory_Write);

    g_DisplayInterface.SendData16Windowed((CLR_UINT16 *)&data[0], srcX, srcY, width, height, stride, true);

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
