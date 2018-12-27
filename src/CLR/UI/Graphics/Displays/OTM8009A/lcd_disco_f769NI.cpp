/* Copyright (c) 2010-2011 mbed.org, MIT License
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software
* and associated documentation files (the "Software"), to deal in the Software without
* restriction, including without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or
* substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
* BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
* DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "LCD_DISCO_F769NI.h"

//#define LCD_SCREEN_WIDTH              800
//#define LCD_SCREEN_HEIGHT             480
//#define ARGB8888_BYTE_PER_PIXEL       4

/* LTDC foreground layer address 800x480 in ARGB8888 */
//#define LCD_FG_LAYER_ADDRESS          LCD_FB_START_ADDRESS

/* LTDC background layer address 800x480 in ARGB8888 following the foreground layer */
//#define LCD_BG_LAYER_ADDRESS          LCD_FG_LAYER_ADDRESS + (LCD_SCREEN_WIDTH * LCD_SCREEN_HEIGHT * ARGB8888_BYTE_PER_PIXEL)

//#define INTERNAL_BUFFER_START_ADDRESS LCD_BG_LAYER_ADDRESS + (LCD_SCREEN_WIDTH * LCD_SCREEN_HEIGHT * ARGB8888_BYTE_PER_PIXEL)

// Constructor
LCD_DISCO_F769NI::LCD_DISCO_F769NI()
{
    BSP_LCD_Init();
    BSP_LCD_LayerDefaultInit(0, LCD_FB_START_ADDRESS);
    BSP_LCD_Clear(LCD_COLOR_BLACK);
}

// Destructor
LCD_DISCO_F769NI::~LCD_DISCO_F769NI()
{
    BSP_LCD_Reset();
}

//=================================================================================================================
// Public methods
//=================================================================================================================

uint8_t LCD_DISCO_F769NI::Init(void)
{
    return BSP_LCD_Init();
}

uint8_t LCD_DISCO_F769NI::InitEx(LCD_OrientationTypeDef orientation)
{
    return BSP_LCD_InitEx(orientation);
}

#if defined(USE_LCD_HDMI)
uint8_t LCD_DISCO_F769NI::HDMIInitEx(uint8_t format)
{
    return BSP_LCD_HDMIInitEx(format);
}
#endif /* USE_LCD_HDMI */

void LCD_DISCO_F769NI::Reset(void)
{
    BSP_LCD_Reset();
}

uint32_t LCD_DISCO_F769NI::GetXSize(void)
{
    return BSP_LCD_GetXSize();
}

uint32_t LCD_DISCO_F769NI::GetYSize(void)
{
    return BSP_LCD_GetYSize();
}

void LCD_DISCO_F769NI::SetXSize(uint32_t imageWidthPixels)
{
    BSP_LCD_SetXSize(imageWidthPixels);
}

void LCD_DISCO_F769NI::SetYSize(uint32_t imageHeightPixels)
{
    BSP_LCD_SetYSize(imageHeightPixels);
}

void LCD_DISCO_F769NI::LayerDefaultInit(uint16_t LayerIndex, uint32_t FB_Address)
{
    BSP_LCD_LayerDefaultInit(LayerIndex, FB_Address);
}

void LCD_DISCO_F769NI::SelectLayer(uint32_t LayerIndex)
{
    BSP_LCD_SelectLayer(LayerIndex);
}

void LCD_DISCO_F769NI::SetLayerVisible(uint32_t LayerIndex, FunctionalState State)
{
    BSP_LCD_SetLayerVisible(LayerIndex, State);
}

void LCD_DISCO_F769NI::SetTransparency(uint32_t LayerIndex, uint8_t Transparency)
{
    BSP_LCD_SetTransparency(LayerIndex, Transparency);
}

void LCD_DISCO_F769NI::SetLayerAddress(uint32_t LayerIndex, uint32_t Address)
{
    BSP_LCD_SetLayerAddress(LayerIndex, Address);
}

void LCD_DISCO_F769NI::SetLayerWindow(uint16_t LayerIndex, uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
    BSP_LCD_SetLayerWindow(LayerIndex, Xpos, Ypos, Width, Height);
}

void LCD_DISCO_F769NI::SetColorKeying(uint32_t LayerIndex, uint32_t RGBValue)
{
    BSP_LCD_SetColorKeying(LayerIndex, RGBValue);
}

void LCD_DISCO_F769NI::ResetColorKeying(uint32_t LayerIndex)
{
    BSP_LCD_ResetColorKeying(LayerIndex);
}

void LCD_DISCO_F769NI::SetTextColor(uint32_t Color)
{
    BSP_LCD_SetTextColor(Color);
}

uint32_t LCD_DISCO_F769NI::GetTextColor(void)
{
    return BSP_LCD_GetTextColor();
}

void LCD_DISCO_F769NI::SetBackColor(uint32_t Color)
{
    BSP_LCD_SetBackColor(Color);
}

uint32_t LCD_DISCO_F769NI::GetBackColor(void)
{
    return BSP_LCD_GetBackColor();
}

void LCD_DISCO_F769NI::SetFont(sFONT *fonts)
{
    BSP_LCD_SetFont(fonts);
}

sFONT *LCD_DISCO_F769NI::GetFont(void)
{
    return BSP_LCD_GetFont();
}

uint32_t LCD_DISCO_F769NI::ReadPixel(uint16_t Xpos, uint16_t Ypos)
{
    return BSP_LCD_ReadPixel(Xpos, Ypos);
}

void LCD_DISCO_F769NI::Clear(uint32_t Color)
{
    BSP_LCD_Clear(Color);
}

void LCD_DISCO_F769NI::ClearStringLine(uint32_t Line)
{
    BSP_LCD_ClearStringLine(Line);
}

void LCD_DISCO_F769NI::DisplayChar(uint16_t Xpos, uint16_t Ypos, uint8_t Ascii)
{
    BSP_LCD_DisplayChar(Xpos, Ypos, Ascii);
}

void LCD_DISCO_F769NI::DisplayStringAt(uint16_t Xpos, uint16_t Ypos, uint8_t *Text, Text_AlignModeTypdef Mode)
{
    BSP_LCD_DisplayStringAt(Xpos, Ypos, Text, Mode);
}

void LCD_DISCO_F769NI::DisplayStringAtLine(uint16_t Line, uint8_t *ptr)
{
    BSP_LCD_DisplayStringAtLine(Line, ptr);
}

void LCD_DISCO_F769NI::DrawHLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
    BSP_LCD_DrawHLine(Xpos, Ypos, Length);
}

void LCD_DISCO_F769NI::DrawVLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
    BSP_LCD_DrawVLine(Xpos, Ypos, Length);
}

void LCD_DISCO_F769NI::DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    BSP_LCD_DrawLine(x1, y1, x2, y2);
}

void LCD_DISCO_F769NI::DrawRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
    BSP_LCD_DrawRect(Xpos, Ypos, Width, Height);
}

void LCD_DISCO_F769NI::DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
    BSP_LCD_DrawCircle(Xpos, Ypos, Radius);
}

void LCD_DISCO_F769NI::DrawPolygon(pPoint Points, uint16_t PointCount)
{
    BSP_LCD_DrawPolygon(Points, PointCount);
}

void LCD_DISCO_F769NI::DrawEllipse(int Xpos, int Ypos, int XRadius, int YRadius)
{
    BSP_LCD_DrawEllipse(Xpos, Ypos, XRadius, YRadius);
}

void LCD_DISCO_F769NI::DrawBitmap(uint32_t Xpos, uint32_t Ypos, uint8_t *pbmp)
{
    BSP_LCD_DrawBitmap(Xpos, Ypos, pbmp);
}

void LCD_DISCO_F769NI::FillRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
    BSP_LCD_FillRect(Xpos, Ypos, Width, Height);
}

void LCD_DISCO_F769NI::FillCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
    BSP_LCD_FillCircle(Xpos, Ypos, Radius);
}

void LCD_DISCO_F769NI::FillPolygon(pPoint Points, uint16_t PointCount)
{
    BSP_LCD_FillPolygon(Points, PointCount);
}

void LCD_DISCO_F769NI::FillEllipse(int Xpos, int Ypos, int XRadius, int YRadius)
{
    BSP_LCD_FillEllipse(Xpos, Ypos, XRadius, YRadius);
}

void LCD_DISCO_F769NI::DisplayOn(void)
{
    BSP_LCD_DisplayOn();
}

void LCD_DISCO_F769NI::DisplayOff(void)
{
    BSP_LCD_DisplayOff();
}

void LCD_DISCO_F769NI::SetBrightness(uint8_t BrightnessValue)
{
    BSP_LCD_SetBrightness(BrightnessValue);
}

void LCD_DISCO_F769NI::DrawPixel(uint16_t Xpos, uint16_t Ypos, uint32_t RGB_Code)
{
    BSP_LCD_DrawPixel(Xpos, Ypos, RGB_Code);
}

//=================================================================================================================
// Private methods
//=================================================================================================================
