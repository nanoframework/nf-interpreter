
#include "nanoCLR_Types.h"
#include "..\..\Devices\Includes\display_driver.h"

extern DISPLAY_CONTROLLER_CONFIG g_Config;

bool LCD_Initialize()
{
    return Display_Driver::Initialize();
}
bool LCD_Uninitialize()
{
    return Display_Driver::Uninitialize();
}
void LCD_PowerSave(bool On)
{
    Display_Driver::PowerSave(On);
}
void LCD_Clear()
{
    Display_Driver::Clear();
}
//void LCD_BitBlt(int width, int height, int widthInWords, CLR_UINT32 data[], bool fUseDelta)
void LCD_BitBlt(int width, int height, int widthInWords, CLR_UINT32 data[])
{
    // Display_Driver::BitBlt(width, height, widthInWords, data, fUseDelta);
    Display_Driver::BitBlt(width, height, widthInWords, data);
}
void LCD_BitBltEx(int x, int y, int width, int height, CLR_UINT32 data[])
{
    Display_Driver::BitBltEx(x, y, width, height, data);
}
void LCD_WriteChar(unsigned char c, int row, int col)
{
    Display_Driver::WriteChar(c, row, col);
}
void LCD_WriteFormattedChar(unsigned char c)
{
    Display_Driver::WriteFormattedChar(c);
}
CLR_INT32 LCD_GetWidth()
{
    //Screen is rotated
    return g_Config.Width;
}
CLR_INT32 LCD_GetHeight()
{
    //Screen is rotated
    return g_Config.Height;
}
CLR_INT32 LCD_GetBitsPerPixel()
{
    return g_Config.BitsPerPixel;
}
CLR_UINT32 LCD_GetPixelClockDivider()
{
    return g_Config.PixelClockDivider;
}
CLR_INT32 LCD_GetOrientation()
{
    return g_Config.Orientation;
}
CLR_UINT32 LCD_ConvertColor(CLR_UINT32 color)
{
    return color;
}
