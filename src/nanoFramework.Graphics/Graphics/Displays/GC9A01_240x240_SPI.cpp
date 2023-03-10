//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "Graphics.h"
#include "DisplayInterface.h"
#include "Display.h"

/*

GC9A01 is a 262,144-color single-chip SOC driver for a-TFT liquid crystal display with resolution of
240RGBx240 dots, comprising a 360-channel source driver, a 32-channel gate driver, 129,600 bytes
GRAM for graphic display data of 240RGBx240 dots, and power supply circuit

Full datasheet at https://www.waveshare.com/w/upload/5/5e/GC9A01A.pdf

*/

struct DisplayDriver g_DisplayDriver;
extern DisplayInterface g_DisplayInterface;
extern DisplayInterfaceConfig g_DisplayInterfaceConfig;

enum GC9A01_CMD : CLR_UINT8
{
    ReadDisplayInfo = 0x04,
    ReadDisplayStatus = 0x09,
    EnterSleepMode = 0x10,
    SleepOut = 0x11,
    PartialModeOn = 0x12,
    NormalDisplayMode = 0x13,
    DisplayInversionOff = 0x20,
    DisplayInversionOn = 0x21,
    DisplayOff = 0x28,
    DisplayOn = 0x29,
    ColumnAddressSet = 0x2A,
    PageAddressSet = 0x2B,
    MemoryWrite = 0x2C,
    PartialArea = 0x30,
    VerticalScrollingDefinition = 0x33,
    TearingEffectLineOff = 0x34,
    TearingEffectLineOn = 0x34,
    MemoryAccessControl = 0x36,
    VerticalScrollingStartAddress = 0x37,
    IdleModeOff = 0x38,
    IdleModeOn = 0x39,
    PixelFormatSet = 0x3A,
    WriteMemoryContinue = 0x3C,
    SetTearScanline = 0x44,
    GetScanline = 0x45,
    WriteDisplayBrightness = 0x51,
    WriteCtrlDisplay = 0x53,
    ReadId1 = 0xDA,
    ReadId2 = 0xDB,
    ReadId3 = 0xDC,
    RgbInterfaceSignalControl = 0xB0,
    BlankingPorchControl = 0xB5,
    DisplayFunctionControl = 0xB6,
    TeControl = 0xBA,
    InterfaceControl = 0xF6,
    PowerCriterionControl = 0xC1,
    VcoreVoltageControl = 0xA7,
    Vreg1aVoltageControl = 0xC3,
    Vreg2aVoltageControl = 0xC9,
    FrameRate = 0xE8,
    Spi2DataControl = 0xE9,
    ChargePumpFrequentControl = 0xEC,
    InnerRegisterEnable1 = 0xFE,
    InnerRegisterEnable2 = 0xEF,
    SetGamma1 = 0xF0,
    SetGamma2 = 0xF1,
    SetGamma3 = 0xF2,
    SetGamma4 = 0xF3
};

enum GC9A01_PIXEL_FORMAT : CLR_UINT8
{
    Pixel12Bit = 0x03,
    Pixel16Bit = 0x05,
    Pixel18Bit = 0x06
};

enum GC9A01_MEMORY_ACCESS_CTRL : CLR_UINT8
{
    Portrait = 0x18,
    Portrait180 = 0x28,
    Landscape = 0x48,
    Landscape180 = 0x88
};

void DisplayDriver::SetupDisplayAttributes()
{
    Attributes.LongerSide = g_DisplayInterfaceConfig.Screen.width;
    Attributes.ShorterSide = g_DisplayInterfaceConfig.Screen.height;
    Attributes.Width = Attributes.LongerSide;
    Attributes.Height = Attributes.ShorterSide;
    Attributes.PowerSave = PowerSaveState::NORMAL;
    Attributes.BitsPerPixel = 16;
    g_DisplayInterface.GetTransferBuffer(Attributes.TransferBuffer, Attributes.TransferBufferSize);
    return;
}

bool DisplayDriver::Initialize()
{
    SetupDisplayAttributes();

    g_DisplayInterface.SendCommand(1, GC9A01_CMD::InnerRegisterEnable2);
    g_DisplayInterface.SendCommand(2, 0xEB, 0x14);
    g_DisplayInterface.SendCommand(1, GC9A01_CMD::InnerRegisterEnable1);
    g_DisplayInterface.SendCommand(1, GC9A01_CMD::InnerRegisterEnable2);
    g_DisplayInterface.SendCommand(2, 0xEB, 0x14);
    g_DisplayInterface.SendCommand(2, 0x84, 0x40);
    g_DisplayInterface.SendCommand(2, 0x85, 0xFF);
    g_DisplayInterface.SendCommand(2, 0x86, 0xFF);
    g_DisplayInterface.SendCommand(2, 0x87, 0xFF);
    g_DisplayInterface.SendCommand(2, 0x88, 0x0A);
    g_DisplayInterface.SendCommand(2, 0x89, 0x21);
    g_DisplayInterface.SendCommand(2, 0x8A, 0x00);
    g_DisplayInterface.SendCommand(2, 0x8B, 0x80);
    g_DisplayInterface.SendCommand(2, 0x8C, 0x01);
    g_DisplayInterface.SendCommand(2, 0x8D, 0x01);
    g_DisplayInterface.SendCommand(2, 0x8E, 0xFF);
    g_DisplayInterface.SendCommand(2, 0x8F, 0xFF);

    g_DisplayInterface.SendCommand(3, GC9A01_CMD::DisplayFunctionControl, 0x00, 0x20);

    g_DisplayInterface.SendCommand(2, GC9A01_CMD::PixelFormatSet, GC9A01_PIXEL_FORMAT::Pixel16Bit);
    g_DisplayInterface.SendCommand(5, 0x90, 0x08, 0x08, 0x08, 0x08);
    g_DisplayInterface.SendCommand(2, 0xBD, 0x06);
    g_DisplayInterface.SendCommand(2, 0xBC, 0x00);
    g_DisplayInterface.SendCommand(4, 0xFF, 0x60, 0x01, 0x04);
    g_DisplayInterface.SendCommand(2, GC9A01_CMD::Vreg1aVoltageControl, 0x13);
    g_DisplayInterface.SendCommand(2, 0xC4, 0x13);
    g_DisplayInterface.SendCommand(2, GC9A01_CMD::Vreg2aVoltageControl, 0x22);
    g_DisplayInterface.SendCommand(2, 0xBE, 0x11);
    g_DisplayInterface.SendCommand(3, 0xE1, 0x10, 0x0E);
    g_DisplayInterface.SendCommand(4, 0xDF, 0x21, 0x0C, 0x02);
    g_DisplayInterface.SendCommand(7, 0xF0, 0x45, 0x09, 0x08, 0x08, 0x26, 0x2A);
    g_DisplayInterface.SendCommand(7, 0xF1, 0x43, 0x70, 0x72, 0x36, 0x37, 0x6F);
    g_DisplayInterface.SendCommand(7, 0xF2, 0x45, 0x09, 0x08, 0x08, 0x26, 0x2A);
    g_DisplayInterface.SendCommand(7, 0xF3, 0x43, 0x70, 0x72, 0x36, 0x37, 0x6F);
    g_DisplayInterface.SendCommand(3, 0xED, 0x1B, 0x0B);
    g_DisplayInterface.SendCommand(2, 0xAE, 0x77);
    g_DisplayInterface.SendCommand(2, 0xCD, 0x63);
    g_DisplayInterface.SendCommand(10, 0x70, 0x07, 0x07, 0x04, 0x0E, 0x0F, 0x09, 0x07, 0x08, 0x03);
    g_DisplayInterface.SendCommand(2, GC9A01_CMD::FrameRate, 0x34);
    g_DisplayInterface.SendCommand(13, 0x62, 0x18, 0x0D, 0x71, 0xED, 0x70, 0x70, 0x18, 0x0F, 0x71, 0xEF, 0x70, 0x70);
    g_DisplayInterface.SendCommand(13, 0x63, 0x18, 0x11, 0x71, 0xF1, 0x70, 0x70, 0x18, 0x13, 0x71, 0xF3, 0x70, 0x70);
    g_DisplayInterface.SendCommand(8, 0x64, 0x28, 0x29, 0xF1, 0x01, 0xF1, 0x00, 0x07);
    g_DisplayInterface.SendCommand(11, 0x66, 0x3C, 0x00, 0xCD, 0x67, 0x45, 0x45, 0x10, 0x00, 0x00, 0x00);
    g_DisplayInterface.SendCommand(11, 0x67, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x01, 0x54, 0x10, 0x32, 0x98);
    g_DisplayInterface.SendCommand(8, 0x74, 0x10, 0x85, 0x80, 0x00, 0x00, 0x4E, 0x00);
    g_DisplayInterface.SendCommand(3, 0x98, 0x3E, 0x07);
    g_DisplayInterface.SendCommand(1, 0x35);
    g_DisplayInterface.SendCommand(1, GC9A01_CMD::DisplayInversionOn);

    g_DisplayInterface.SendCommand(1, GC9A01_CMD::SleepOut);
    OS_DELAY(120);
    g_DisplayInterface.SendCommand(1, GC9A01_CMD::DisplayOn);
    OS_DELAY(20);

    return true;
}

bool DisplayDriver::Uninitialize()
{
    Clear();
    return true;
}

void DisplayDriver::Clear()
{
    SetWindow(0, 0, Attributes.Width - 1, Attributes.Height - 1);
    g_DisplayInterface.SendCommand(1, GC9A01_CMD::MemoryWrite);
    g_DisplayInterface.FillData16(0, Attributes.Width * Attributes.Height);
}

void DisplayDriver::DisplayBrightness(CLR_INT16 brightness)
{
    _ASSERTE(brightness >= 0 && brightness <= 100);
    g_DisplayInterface.SendCommand(2, GC9A01_CMD::WriteDisplayBrightness, (CLR_UINT8)brightness);
}

bool DisplayDriver::SetWindow(CLR_INT16 x1, CLR_INT16 y1, CLR_INT16 x2, CLR_INT16 y2)
{
    CLR_UINT8 buff[4];
    buff[0] = ((x1 + g_DisplayInterfaceConfig.Screen.x) >> 8) & 0xFF;
    buff[1] = (x1 + g_DisplayInterfaceConfig.Screen.x) & 0xFF;
    buff[2] = ((x2 + g_DisplayInterfaceConfig.Screen.x) >> 8) & 0xFF;
    buff[3] = (x2 + g_DisplayInterfaceConfig.Screen.x) & 0xFF;
    g_DisplayInterface.SendCommand(5, GC9A01_CMD::ColumnAddressSet, buff[0], buff[1], buff[2], buff[3]);

    buff[0] = ((y1 + g_DisplayInterfaceConfig.Screen.y) >> 8) & 0xFF;
    buff[1] = (y1 + g_DisplayInterfaceConfig.Screen.y) & 0xFF;
    buff[2] = ((y2 + g_DisplayInterfaceConfig.Screen.y) >> 8) & 0xFF;
    buff[3] = (y2 + g_DisplayInterfaceConfig.Screen.y) & 0xFF;
    g_DisplayInterface.SendCommand(5, GC9A01_CMD::PageAddressSet, buff[0], buff[1], buff[2], buff[3]);

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

    g_DisplayInterface.SendCommand(1, GC9A01_CMD::MemoryWrite);
    g_DisplayInterface.SendData16Windowed((CLR_UINT16 *)&data[0], srcX, srcY, width, height, stride, true);
}

void DisplayDriver::PowerSave(PowerSaveState powerState)
{
    switch (powerState)
    {
        default: // Illegal fall through to Power on
        case PowerSaveState::NORMAL:
            g_DisplayInterface.SendCommand(1, GC9A01_CMD::SleepOut);
            OS_DELAY(120);
            break;
        case PowerSaveState::SLEEP:
            g_DisplayInterface.SendCommand(1, GC9A01_CMD::EnterSleepMode);
            OS_DELAY(120);
            break;
    }
    return;
}

void DisplayDriver::SetDefaultOrientation()
{
    ChangeOrientation(DisplayOrientation::DisplayOrientation_Landscape);
}

bool DisplayDriver::ChangeOrientation(DisplayOrientation orientation)
{
    CLR_UINT8 orientationByte = GC9A01_MEMORY_ACCESS_CTRL::Portrait;
    switch (orientation)
    {
        case DisplayOrientation::DisplayOrientation_Portrait:
            orientationByte = GC9A01_MEMORY_ACCESS_CTRL::Portrait;
            break;
        case DisplayOrientation::DisplayOrientation_Portrait180:
            orientationByte = GC9A01_MEMORY_ACCESS_CTRL::Portrait180;
            break;
        case DisplayOrientation::DisplayOrientation_Landscape:
            orientationByte = GC9A01_MEMORY_ACCESS_CTRL::Landscape;
            break;
        case DisplayOrientation::DisplayOrientation_Landscape180:
            orientationByte = GC9A01_MEMORY_ACCESS_CTRL::Portrait180;
            break;
    }

    g_DisplayInterface.SendCommand(2, GC9A01_CMD::MemoryAccessControl, orientationByte);
    return true;
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
