
#include "OTM8009Ah"

bool LCD_Initialize()
{
    return OTM8009A_Driver::Initialize();
}

bool LCD_Uninitialize()
{
    return OTM8009A_Driver::Uninitialize();
}

void LCD_PowerSave(bool On)
{
    OTM8009A_Driver::PowerSave(On);
}

void LCD_Clear()
{
    OTM8009A_Driver::Clear();
}

void LCD_BitBlt(int width, int height, int widthInWords, CLR_UINT32 data[], bool fUseDelta)
{
    OTM8009A_Driver::BitBlt(width, height, widthInWords, data, fUseDelta);
}

void LCD_BitBltEx(int x, int y, int width, int height, CLR_UINT32 data[])
{
    OTM8009A_Driver::BitBltEx(x, y, width, height, data);
}

void LCD_WriteChar(unsigned char c, int row, int col)
{
    OTM8009A_Driver::WriteChar(c, row, col);
}

void LCD_WriteFormattedChar(unsigned char c)
{
    OTM8009A_Driver::WriteFormattedChar(c);
}

CLR_INT32 LCD_GetWidth()
{
    //Screen is rotated
    return g_OTM8009A_Config.Width;
}

CLR_INT32 LCD_GetHeight()
{
    NATIVE_PROFILE_HAL_DRIVERS_DISPLAY();
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
