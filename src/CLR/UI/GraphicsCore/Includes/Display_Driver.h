////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _DRIVERS_LCD_DECL_H_
#define _DRIVERS_LCD_DECL_H_ 1

#include "nanoCLR_Types.h"

#define LCD_SCREEN_WIDTH    LCD_GetWidth()
#define LCD_SCREEN_HEIGHT   LCD_GetHeight()
#define LCD_SCREEN_BPP      LCD_GetBitsPerPixel()
#define LCD_SCREEN_ORIENTATION LCD_GetOrientation()

#define LCD_SCREEN_PIXELS_PER_WORD      (32/LCD_SCREEN_BPP)
#define LCD_SCREEN_WIDTH_IN_WORDS       ((LCD_SCREEN_WIDTH + LCD_SCREEN_PIXELS_PER_WORD - 1) / LCD_SCREEN_PIXELS_PER_WORD)
#define LCD_SCREEN_SIZE_IN_WORDS        (LCD_SCREEN_WIDTH_IN_WORDS * LCD_SCREEN_HEIGHT)
#define LCD_SCREEN_SIZE_IN_BYTES        (LCD_SCREEN_SIZE_IN_WORDS * 4)

#define LCD_NO_PIXEL_CLOCK_DIVIDER      0

struct DISPLAY_CONTROLLER_REGISTER_EXTRA
{
    CLR_UINT32 RegAddr;
    CLR_UINT32 RegValue;
};

struct DISPLAY_CONTROLLER_CONFIG
{
    CLR_UINT32 Width;
    CLR_UINT32 Height;
    bool EnableTFT;
    bool EnableColor;
    bool PixelPolarity;
    bool FirstLineMarkerPolarity;
    bool LinePulsePolarity;
    bool ShiftClockPolarity;
    bool OutputEnablePolarity;
    bool ClockIdleEnable;
    bool ClockSelectEnable;

    CLR_UINT8 PixelClockDivider;
    CLR_UINT8 BusWidth;
    CLR_UINT8 BitsPerPixel;
    CLR_UINT8 Orientation;

    CLR_UINT8 HorizontalSyncPulseWidth;
    CLR_UINT8 HorizontalSyncWait1;
    CLR_UINT8 HorizontalSyncWait2;
    CLR_UINT8 VerticalSyncPulseWidth;
    CLR_UINT8 VerticalSyncWait1;
    CLR_UINT8 VerticalSyncWait2;
    DISPLAY_CONTROLLER_REGISTER_EXTRA RegisterExtras[8];
};

extern DISPLAY_CONTROLLER_CONFIG g_LcdController_Config;

//--//  Display Driver  //--//

bool LCD_Initialize();
bool LCD_Uninitialize();
void LCD_Clear();
void LCD_BitBltEx(int x, int y, int width, int height, CLR_UINT32 data[]);
void LCD_BitBlt(int width, int height, int widthInWords, CLR_UINT32 data[], bool fUseDelta);
void LCD_WriteChar(unsigned char c, int row, int col);
void LCD_WriteFormattedChar(unsigned char c);
CLR_INT32 LCD_GetWidth();
CLR_INT32 LCD_GetHeight();
CLR_INT32 LCD_GetBitsPerPixel();
CLR_INT32 LCD_GetOrientation();
void LCD_PowerSave(bool On);
CLR_UINT32 LCD_ConvertColor(CLR_UINT32 color);


//--//  Display Controller  //--//

bool LCD_Controller_Initialize(DISPLAY_CONTROLLER_CONFIG& config);
bool LCD_Controller_Uninitialize();
bool LCD_Controller_Enable(bool fEnable);
CLR_UINT32* LCD_GetFrameBuffer();

//--//  Text Font (for WriteChar*) //--//

CLR_INT32 Font_Height();
CLR_INT32 Font_Width();
CLR_INT32 Font_TabWidth();
const CLR_UINT8* Font_GetGlyph(unsigned char c);

#endif // _DRIVERS_LCD_DECL_H_

