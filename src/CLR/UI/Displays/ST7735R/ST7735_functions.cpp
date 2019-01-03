
#include "ST7735.h"

bool LCD_Initialize()
{
    return ST7735_Driver::Initialize();
}

bool LCD_Uninitialize()
{
    return ST7735_Driver::Uninitialize();
}

void LCD_PowerSave(bool On)
{
    ST7735_Driver::PowerSave(On);
}

void LCD_Clear()
{
    ST7735_Driver::Clear();
}

void LCD_BitBlt(int width, int height, int widthInWords, CLR_UINT32 data[], bool fUseDelta)
{
    ST7735_Driver::BitBlt(width, height, widthInWords, data, fUseDelta);
}

void LCD_BitBltEx(int x, int y, int width, int height, CLR_UINT32 data[])
{
    ST7735_Driver::BitBltEx(x, y, width, height, data);
}

void LCD_WriteChar(unsigned char c, int row, int col)
{
    ST7735_Driver::WriteChar(c, row, col);
}

void LCD_WriteFormattedChar(unsigned char c)
{
    ST7735_Driver::WriteFormattedChar(c);
}

CLR_INT32 LCD_GetWidth()
{
    //Screen is rotated
    return g_ST7735_Config.Width;
}

CLR_INT32 LCD_GetHeight()
{
    NATIVE_PROFILE_HAL_DRIVERS_DISPLAY();
    //Screen is rotated
    return g_ST7735_Config.Height;
}

CLR_INT32 LCD_GetBitsPerPixel()
{
    return g_ST7735_Config.BitsPerPixel;
}

CLR_UINT32 LCD_GetPixelClockDivider()
{
    return g_ST7735_Config.PixelClockDivider;
}

CLR_INT32 LCD_GetOrientation()
{
    return g_ST7735_Config.Orientation;
}

CLR_UINT32 LCD_ConvertColor(CLR_UINT32 color)
{
    return color;
}
