#ifndef _DRIVERS_FUNCTIONS_OTM8009A_H_
#define _DRIVERS_FUNCTIONS_OTM8009A_H_ 1

#include "nanoCLR_Types.h"
#include "display_driver.h"

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

#endif  // _DRIVERS_FUNCTIONS_OTM8009A_H_
