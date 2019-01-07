
#include "Display_driver_OTM8009A.h"
#include "Display_Driver.h"

bool LCD_Initialize()
{
    return Display_Driver_OTM8009A::Initialize();
}
bool LCD_Uninitialize()
{
    return Display_Driver_OTM8009A::Uninitialize();
}
void LCD_PowerSave(bool On)
{
    Display_Driver_OTM8009A::PowerSave(On);
}
void LCD_Clear()
{
    Display_Driver_OTM8009A::Clear();
}
//void LCD_BitBlt(int width, int height, int widthInWords, CLR_UINT32 data[], bool fUseDelta)
void LCD_BitBlt(int width, int height, int widthInWords, CLR_UINT32 data[])
{
    Display_Driver_OTM8009A::BitBlt(width, height, widthInWords, data);
    //Display_Driver_OTM8009A::BitBlt(width, height, widthInWords, data, fUseDelta);
}
void LCD_BitBltEx(int x, int y, int width, int height, CLR_UINT32 data[])
{
    Display_Driver_OTM8009A::BitBltEx(x, y, width, height, data);
}
void LCD_WriteChar(unsigned char c, int row, int col)
{
    Display_Driver_OTM8009A::WriteChar(c, row, col);
}
void LCD_WriteFormattedChar(unsigned char c)
{
    Display_Driver_OTM8009A::WriteFormattedChar(c);
}
CLR_INT32 LCD_GetWidth()
{
    //Screen is rotated
    return g_OTM8009A_Config.Width;
}
CLR_INT32 LCD_GetHeight()
{
    //Screen is rotated
    return g_OTM8009A_Config.Height;
}
CLR_INT32 LCD_GetBitsPerPixel()
{
    return g_OTM8009A_Config.BitsPerPixel;
}
CLR_UINT32 LCD_GetPixelClockDivider()
{
    return g_OTM8009A_Config.PixelClockDivider;
}
CLR_INT32 LCD_GetOrientation()
{
    return g_OTM8009A_Config.Orientation;
}
CLR_UINT32 LCD_ConvertColor(CLR_UINT32 color)
{
    return color;
}
