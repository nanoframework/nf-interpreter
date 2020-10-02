//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the

#ifndef _DISPLAY_H_
#define _DISPLAY_H_ 1

#include "nanoCLR_Types.h"

#define LCD_COLOUR_RGB(R,G,B) ((CLR_UINT16)((((R) / 8) << 11) + (((G) / 4) << 5) + ((B) / 8)))

enum DisplayOrientation : CLR_INT16
{
    PORTRAIT,
    PORTRAIT180,
    LANDSCAPE,
    LANDSCAPE180
};
enum PixelFormat : CLR_UINT8 {
    FORMAT_RGB888 = 0,         // Pixel format chosen is RGB888 : 24 bpp
    FORMAT_RBG565 = 2,         // Pixel format chosen is RGB565 : 16 bpp
};
enum PowerSaveState : CLR_UINT8 {
    NORMAL = 0,
    SLEEP = 1
};
struct DisplayAttributes
{
    CLR_UINT8* TransferBuffer = NULL;
    CLR_UINT32 TransferBufferSize;
    PowerSaveState PowerSave;
    DisplayOrientation Orientation;  //Future
    CLR_INT16 Height;
    CLR_INT16 Width;
    CLR_INT16 BitsPerPixel;
    CLR_INT16 LongerSide;  // Pixels
    CLR_INT16 ShorterSide; // Pixels
};
struct DisplayDriver
{
    //CLR_UINT32* g_FrameBuffer;         // = NULL   -- copied from netmg ( not sure it is needed based on new implementation)
    //CLR_UINT32 g_FrameBufferSize;      // = 0;      -- copied from netmg ( not sure it is needed)
    //bool   g_LcdInitialized;           // = false;  ---copied from netmg ( not sure it is needed)

    DisplayAttributes Attributes;

    void SetupDisplayAttributes();
    bool Initialize();
    bool Uninitialize();
    void Clear();
    void DisplayBrightness(CLR_INT16 brightness);
    void BitBlt(int x, int y, int width, int height, CLR_UINT32 data[]);
    void PowerSave(PowerSaveState powerState);
    void SetDefaultOrientation();
    CLR_UINT32 PixelsPerWord();
    CLR_UINT32 WidthInWords();
    CLR_UINT32 SizeInWords();
    CLR_UINT32 SizeInBytes();
};
#endif 