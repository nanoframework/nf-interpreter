//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the

#ifndef DISPLAY_H
#define DISPLAY_H

#include "nanoCLR_Types.h"

enum DisplayOrientation : CLR_INT16
{
    DisplayOrientation_Portrait = 0,
    DisplayOrientation_Portrait180 = 1,
    DisplayOrientation_Landscape = 2,
    DisplayOrientation_Landscape180 = 3,
};

enum GraphicDriverCommandType : CLR_UINT8
{
    GraphicDriverCommandType_Sleep = 0,
    GraphicDriverCommandType_Command = 1,
};

enum SetWindowType : CLR_UINT8
{
    SetWindowType_NoWindowing = 0,
    SetWindowType_X8bitsY1Bit = 1,
    SetWindowType_X8bitsY8Bits = 2,
    SetWindowType_X16bitsY16Bit = 3,
};

enum PixelFormat : CLR_UINT8
{
    FORMAT_RGB888 = 0, // Pixel format chosen is RGB888 : 24 bpp
    FORMAT_RBG565 = 2, // Pixel format chosen is RGB565 : 16 bpp
};

enum PowerSaveState : CLR_UINT8
{
    NORMAL = 0,
    SLEEP = 1
};

struct DisplayAttributes
{
    CLR_UINT8 *TransferBuffer = NULL;
    CLR_UINT32 TransferBufferSize;
    PowerSaveState PowerSave;
    DisplayOrientation Orientation; // Future
    CLR_INT16 Height;
    CLR_INT16 Width;
    CLR_INT16 BitsPerPixel;
    CLR_INT16 LongerSide;  // Pixels
    CLR_INT16 ShorterSide; // Pixels
};

struct DisplayDriver
{
    DisplayAttributes Attributes;

    void SetupDisplayAttributes();
    bool Initialize();
    bool Uninitialize();
    void Clear();
    void DisplayBrightness(CLR_INT16 brightness);
    bool SetWindow(CLR_INT16 x1, CLR_INT16 y1, CLR_INT16 x2, CLR_INT16 y2);
    void BitBlt(int srcX, int srcY, int width, int height, int stride, int screenX, int screenY, CLR_UINT32 data[]);
    void PowerSave(PowerSaveState powerState);
    void SetDefaultOrientation();
    bool ChangeOrientation(DisplayOrientation orientation);

    CLR_UINT32 PixelsPerWord();
    CLR_UINT32 WidthInWords();
    CLR_UINT32 SizeInWords();
    CLR_UINT32 SizeInBytes();
};

#endif // DISPLAY_H