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
    Read_Display_Identification_Information = 0x04,
    POWER_STATE = 0x10,
    Sleep_Out = 0x11,
    Noron = 0x13,
    Invert_Off = 0x20,
    Invert_On = 0x21,
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
    Power_On_Sequence = 0xED,
    Enable_3G = 0xF2,
    Interface_Control = 0xF6,
    Pump_Ratio_Control = 0xF7
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
    // Initialize ILI9342 registers

    SetupDisplayAttributes();

    // Now use display manufacturer defaults to not damage display by incorrect voltage selection
    // TODO: Make sure what else parts of initialization need to add and which to expose to managed code
    // For the moment there is an idea what need to add in next version:
    //  - screen mirror to support difefrent display glass layouts
    //  - invert brightness to support normal close and normal open panels
    //  - color mix RGB/BGR to support different layouts of the COG
    //
    // Optional voltage initialization keep it here with description for possible future implementation
    // Power_Control_1 - GVDD 3.0V ~ 6.0V (0x03 ~ 0x3F) step 0.05V
    // g_DisplayInterface.SendCommand(2, Power_Control_1, 0x0D);
    // g_DisplayInterface.SendCommand(2, Power_Control_2, 0x03);
    // VCOM_Control_1 - VCOMH/VCOML voltage +5.5V ~ -1.5V (default 0x31, 0x3C [+3.925 ~ -1.000V])
    // g_DisplayInterface.SendCommand(3, VCOM_Control_1, 0x70, 0x28);
    // g_DisplayInterface.SendCommand(2, VCOM_Control_2, 0xC0);
    g_DisplayInterface.SendCommand(2, Interface_Signal_Control, 0xE0);
    g_DisplayInterface.SendCommand(4, Interface_Control, 0x01, 0x00, 0X00);
    g_DisplayInterface.SendCommand(2, Pixel_Format_Set, 0x55); // 0x55 -> 16 bit
    // keep here gamma setting commented for possible future implementation that require expose to managed code
    /*
    g_DisplayInterface.SendCommand(
        16,
        Positive_Gamma_Correction,
        0x00,
        0x0C,
        0x11,
        0x04,
        0x11,
        0x08,
        0x37,
        0x89,
        0x4C,
        0x06,
        0x0C,
        0x0A,
        0x2E,
        0x34,
        0x0F);
    g_DisplayInterface.SendCommand(
        16,
        Negative_Gamma_Correction,
        0x00,
        0x0B,
        0x11,
        0x05,
        0x13,
        0x09,
        0x33,
        0x67,
        0x48,
        0x07,
        0x0E,
        0x0B,
        0x2E,
        0x33,
        0x0F);
        */
    // set display phisical size/parameters
    g_DisplayInterface.SendCommand(
        5,
        Display_Function_Control,
        0x08,
        0x82,
        (CLR_UINT8)((g_DisplayInterfaceConfig.Screen.height / 8) - 1),
        0x04);

    // set display logical size/parameters
    g_DisplayInterface.SendCommand(
        5,
        Column_Address_Set,
        (CLR_UINT8)(g_DisplayInterfaceConfig.Screen.x >> 8),
        (CLR_UINT8)(g_DisplayInterfaceConfig.Screen.x & 0xFF),
        (CLR_UINT8)((g_DisplayInterfaceConfig.Screen.width - 1) >> 8),
        (CLR_UINT8)((g_DisplayInterfaceConfig.Screen.width - 1) & 0xFF)); // Size = 239
    g_DisplayInterface.SendCommand(
        5,
        Page_Address_Set,
        (CLR_UINT8)(g_DisplayInterfaceConfig.Screen.y >> 8),
        (CLR_UINT8)(g_DisplayInterfaceConfig.Screen.y & 0xFF),
        (CLR_UINT8)((g_DisplayInterfaceConfig.Screen.height - 1) >> 8),
        (CLR_UINT8)((g_DisplayInterfaceConfig.Screen.height - 1) & 0xFF)); // Size = 319

    // Keep commented color/brightness inversion command here for possible futeure implementation
    // g_DisplayInterface.SendCommand(1, Invert_On);

    g_DisplayInterface.SendCommand(1, Sleep_Out);
    OS_DELAY(20); // Send Sleep Out command to display : no parameter
    g_DisplayInterface.SendCommand(1, Display_ON);
    OS_DELAY(200);                          // Send Sleep Out command to display : no parameter
    g_DisplayInterface.SendCommand(1, NOP); // End of sequence
    OS_DELAY(20);                           // Send Sleep Out command to display : no parameter

    SetDefaultOrientation();

    return true;
}

void DisplayDriver::SetupDisplayAttributes()
{
    // Define the LCD/TFT resolution
    if (g_DisplayInterfaceConfig.Screen.width > g_DisplayInterfaceConfig.Screen.height)
    {
        Attributes.LongerSide = g_DisplayInterfaceConfig.Screen.width;
        Attributes.ShorterSide = g_DisplayInterfaceConfig.Screen.height;
    }
    else
    {
        Attributes.LongerSide = g_DisplayInterfaceConfig.Screen.height;
        Attributes.ShorterSide = g_DisplayInterfaceConfig.Screen.width;
    }
    Attributes.PowerSave = PowerSaveState::NORMAL;
    Attributes.BitsPerPixel = 16;
    g_DisplayInterface.GetTransferBuffer(Attributes.TransferBuffer, Attributes.TransferBufferSize);
    return;
}

// display rotation definitions
#define dmac_PORTRAIT000  (MADCTL_MX)
#define dmac_PORTRAIT180  (MADCTL_MY)
#define dmac_LANDSCAPE000 (MADCTL_MV)
#define dmac_LANDSCAPE180 (MADCTL_MV | MADCTL_MX | MADCTL_MY)
bool DisplayDriver::ChangeOrientation(DisplayOrientation orientation)
{
    CLR_UINT8 dMAC = MADCTL_BGR;
    // define logic resolution
    switch (orientation)
    {
        case DisplayOrientation::DisplayOrientation_Portrait:
        case DisplayOrientation::DisplayOrientation_Portrait180:
            Attributes.Height = Attributes.LongerSide;
            Attributes.Width = Attributes.ShorterSide;
            break;
        case DisplayOrientation::DisplayOrientation_Landscape:
        case DisplayOrientation::DisplayOrientation_Landscape180:
            Attributes.Height = Attributes.ShorterSide;
            Attributes.Width = Attributes.LongerSide;
            break;
    }

    // set physical resolution
    switch (orientation)
    {
        case DisplayOrientation::DisplayOrientation_Portrait:
            dMAC |= dmac_PORTRAIT000;
            break;
        case DisplayOrientation::DisplayOrientation_Portrait180:
            dMAC |= dmac_PORTRAIT180;
            break;
        case DisplayOrientation::DisplayOrientation_Landscape:
            dMAC |= dmac_LANDSCAPE000;
            break;
        case DisplayOrientation::DisplayOrientation_Landscape180:
            dMAC |= dmac_LANDSCAPE180;
            break;
    }
    g_DisplayInterface.SendCommand(1, NOP); // End of sequence
    OS_DELAY(20);                           // Send Sleep Out command to display : no parameter

    g_DisplayInterface.SendCommand(2, Memory_Access_Control, dMAC); // Set memory access mode
    OS_DELAY(20);                                                   // Send Sleep Out command to display : no parameter
    g_DisplayInterface.SendCommand(1, Memory_Write);
    OS_DELAY(20); // Send Sleep Out command to display : no parameter
    return true;
}

void DisplayDriver::SetDefaultOrientation()
{
    // Change default orientation to display defaults
    ChangeOrientation(DisplayOrientation::DisplayOrientation_Portrait);
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

    g_DisplayInterface.SendCommand(1, Memory_Write);

    g_DisplayInterface.FillData16(0, Attributes.Width * Attributes.Height);
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
