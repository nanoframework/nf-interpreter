////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _DRIVERS_DISPLAY_DRIVER_H_
#define _DRIVERS_DISPLAY_DRIVER_H_ 1

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

typedef enum
{
    PORTRAIT,
    PORTRAIT180,
    LANDSCAPE,
    LANDSCAPE180
} SCREEN_ORIENTATION;
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
struct DISPLAY_DRIVER
{
    CLR_UINT32 m_cursor;
    SCREEN_ORIENTATION lcd_orientation;

    //FIXME:  static CLR_UINT16 VIDEO_RAM[(DISP_YMAX* DISP_XMAX )] ;
    static bool Initialize();
    static bool Uninitialize();
    static void PowerSave(bool On);
    static void Clear();
    static void BitBltEx(int x, int y, int width, int height, CLR_UINT32 data[]);
    //    static void BitBlt(int width, int height, int widthInWords, CLR_UINT32 data[], bool fUseDelta);
    static void BitBlt(int width, int height, int widthInWords, CLR_UINT32 data[]);
    static void WriteChar(unsigned char c, int row, int col);
    static void WriteFormattedChar(unsigned char c);

private:
    static bool SetWindow(CLR_UINT16 x0, CLR_UINT16 y0, CLR_UINT16 x1, CLR_UINT16 y1);
    static CLR_UINT32 PixelsPerWord();
    static CLR_UINT32 TextRows();
    static CLR_UINT32 TextColumns();
    static CLR_UINT32 WidthInWords();
    static CLR_UINT32 SizeInWords();
    static CLR_UINT32 SizeInBytes();
};
#endif // _DRIVERS_DISPLAY_DRIVER_H_
