//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the

#ifndef DISPLAY_H
#define DISPLAY_H

#include "nanoCLR_Types.h"

enum DisplayOrientation : CLR_INT16
{
    PORTRAIT,
    PORTRAIT180,
    LANDSCAPE,
    LANDSCAPE180
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
    void BitBlt(int x, int y, int width, int height, CLR_UINT32 data[]);
    void PowerSave(PowerSaveState powerState);
    void SetDefaultOrientation();
    bool ChangeOrientation(DisplayOrientation orientation);

    CLR_UINT32 PixelsPerWord();
    CLR_UINT32 WidthInWords();
    CLR_UINT32 SizeInWords();
    CLR_UINT32 SizeInBytes();
};

#endif // DISPLAY_H