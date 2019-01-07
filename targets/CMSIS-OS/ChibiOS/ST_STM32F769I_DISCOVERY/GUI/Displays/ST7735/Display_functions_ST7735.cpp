
#include "Display_driver_ST7735.h"
#include "Display_Driver.h"

bool LCD_Initialize()
{
    return Display_Driver_ST7735::Initialize();
}
bool LCD_Uninitialize()
{
    return Display_Driver_ST7735::Uninitialize();
}
void LCD_PowerSave(bool On)
{
    Display_Driver_ST7735::PowerSave(On);
}
void LCD_Clear()
{
    Display_Driver_ST7735::Clear();
}
//void LCD_BitBlt(int width, int height, int widthInWords, CLR_UINT32 data[], bool fUseDelta)
void LCD_BitBlt(int width, int height, int widthInWords, CLR_UINT32 data[])
{
    // Display_Driver_ST7735::BitBlt(width, height, widthInWords, data, fUseDelta);
    Display_Driver_ST7735::BitBlt(width, height, widthInWords, data);
}
void LCD_BitBltEx(int x, int y, int width, int height, CLR_UINT32 data[])
{
    Display_Driver_ST7735::BitBltEx(x, y, width, height, data);
}
void LCD_WriteChar(unsigned char c, int row, int col)
{
    Display_Driver_ST7735::WriteChar(c, row, col);
}
void LCD_WriteFormattedChar(unsigned char c)
{
    Display_Driver_ST7735::WriteFormattedChar(c);
}
CLR_INT32 LCD_GetWidth()
{
    //Screen is rotated
    return g_ST7735_Config.Width;
}
CLR_INT32 LCD_GetHeight()
{
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
