
#include "nanohal.h"
#include "nanoCLR_Types.h"
#include "hal.h"
#include "nanohal.h"
#include "Display_driver.h"
#include "Display_driver_OTM8009A.h"
#include "CPU_To_Display_OTM8009A.h"

Display_Driver_OTM8009A OTM8009A_Driver;
DISPLAY_CONTROLLER_CONFIG OTM8009A_Config;
extern CPU_To_Display_OTM8009A CPU_OTM8009A_Driver;


bool Display_Driver_OTM8009A::SetWindow(CLR_UINT16 x1, CLR_UINT16 y1, CLR_UINT16 x2, CLR_UINT16 y2)
{
    CLR_UINT16 x1_x2;
    CLR_UINT16 Addr1, Addr2;

    switch (g_OTM8009A_Display_Driver.lcd_orientation)
    {
    default:
        // Invalid! Fall through to portrait mode
    case PORTRAIT:
        Addr1 = x1;
        Addr2 = y1;
        x1_x2 = (CLR_UINT16)((x2 << 8) + x1);   // pack X-Values into one word            
        break;
    case PORTRAIT180:
        Addr1 = (CLR_UINT16)(SCREEN_SIZE_SHORTER_SIDE - 1 - x1);
        Addr2 = (CLR_UINT16)(SCREEN_SIZE_LONGER_SIDE - 1 - y1);
        x1_x2 = (CLR_UINT16)((Addr1 << 8) + (SCREEN_SIZE_SHORTER_SIDE - 1 - x2));    // pack X-Values into one word
        y1 = (CLR_UINT16)(SCREEN_SIZE_LONGER_SIDE - 1 - y2);
        y2 = Addr2;
        break;
    case LANDSCAPE:
        Addr1 = (CLR_UINT16)(SCREEN_SIZE_SHORTER_SIDE - 1 - y1);
        Addr2 = x1;
        x1_x2 = (CLR_UINT16)((Addr1 << 8) + (SCREEN_SIZE_SHORTER_SIDE - 1 - y2));    // pack X-Values into one word
        y1 = x1;
        y2 = x2;
        break;
    case LANDSCAPE180:
        Addr1 = y1;
        Addr2 = (CLR_UINT16)(SCREEN_SIZE_LONGER_SIDE - 1 - x1);    // pack X-Values into one word
        x1_x2 = (CLR_UINT16)((y2 << 8) + y1);
        y1 = (CLR_UINT16)(SCREEN_SIZE_LONGER_SIDE - 1 - x2);
        y2 = Addr2;
        break;
    }

    CPU_OTM8009A_Driver.SetWindowSize(g_OTM8009A_Display_Driver.lcd_orientation,x1_x2, y1,y2);

    return TRUE;
}
bool Display_Driver_OTM8009A::Initialize()
{
    // Enable CMD2 to access vendor specific commands
    // Enter in command 2 mode and set EXTC to enable address shift function (0x00)
    CPU_OTM8009A_Driver.SendCommandToDisplay( OTM8009A_CMD_NOP, 0x00 );
    CPU_OTM8009A_Driver.SendDataToDisplay((CLR_UINT8 *)(CLR_UINT8[4]){ 0x80, 0x09, 0x01, 0xFF },4);

    // Enter ORISE Command 2
    CPU_OTM8009A_Driver.SendCommandToDisplay( OTM8009A_Commands::OTM8009A_CMD_NOP, 0x80 ); // Shift address to 0x80
    CPU_OTM8009A_Driver.SendDataToDisplay((CLR_UINT8 *)(CLR_UINT8[3]) { 0x80,0x09,0xFF },3);

    // SD_PCH_CTRL - 0xC480h - 129th parameter - Default 0x00
    // Set SD_PT -> Source output level during porch and non-display area to GND
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_NOP, 0x80);
    CPU_OTM8009A_Driver.SendCommandToDisplay(0xC4, 0x30);
    osDelay(10);
    // Not documented
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_NOP, 0x8A);
    CPU_OTM8009A_Driver.SendCommandToDisplay(0xC4, 0x40);
    osDelay(10);

    // PWR_CTRL4 - 0xC4B0h - 178th parameter - Default 0xA8
    // Set gvdd_en_test -> enable GVDD test mode
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_NOP, 0xB1);
    CPU_OTM8009A_Driver.SendCommandToDisplay(0xC5, 0xA9);

    // PWR_CTRL2 - 0xC590h - 146th parameter - Default 0x79
    // Set pump 4 vgh voltage  -> from 15.0v down to 13.0v
    // Set pump 5 vgh voltage   -> from -12.0v downto -9.0v
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_NOP, 0x91);
    CPU_OTM8009A_Driver.SendCommandToDisplay(0xC5, 0x34);

    /* P_DRV_M - 0xC0B4h - 181th parameter - Default 0x00 */
    /* -> Column inversion                                */
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_NOP, 0xB4);
    CPU_OTM8009A_Driver.SendCommandToDisplay(0xC0, 0x50);

    // VCOMDC - 0xD900h - 1st parameter - Default 0x39h
    // VCOM Voltage settings -> from -1.0000v downto -1.2625v 
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_CMD_NOP, 0x00);
    CPU_OTM8009A_Driver.SendCommandToDisplay(0xD9, 0x4E);

    /* Oscillator adjustment for Idle/Normal mode (LPDT only) set to 65Hz (default is 60Hz) */
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_NOP, 0x81);
    CPU_OTM8009A_Driver.SendCommandToDisplay(0xC1, 0x66);

    /* Video mode internal */
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_NOP, 0xA1);
    CPU_OTM8009A_Driver.SendCommandToDisplay(0xC1, 0x08);

    // PWR_CTRL2 - 0xC590h - 147h parameter - Default 0x00
    // Set pump 4&5 x6  -> ONLY VALID when PUMP4_EN_ASDM_HV = "0"
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_NOP, 0x92);
    CPU_OTM8009A_Driver.SendCommandToDisplay(0xC5, 0x01);

    // PWR_CTRL2 - 0xC590h - 150th parameter - Default 0x33h
    // Change pump4 clock ratio -> from 1 line to 1/2 line
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_NOP, 0x95);
    CPU_OTM8009A_Driver.SendCommandToDisplay(0xC5, 0x34);

    // GVDD/NGVDD settings
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_CMD_NOP, 0x00);
    CPU_OTM8009A_Driver.SendDataToDisplay((CLR_UINT8 *)(CLR_UINT8[3]){ 0x79,0x79,0xD8 },3);

    // PWR_CTRL2 - 0xC590h - 149th parameter - Default 0x33h
    // Rewrite the default value !
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_NOP, 0x94);
    CPU_OTM8009A_Driver.SendCommandToDisplay(0xC5, 0x33);

    // Panel display timing Setting 3
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_NOP, 0xA3);
    CPU_OTM8009A_Driver.SendCommandToDisplay(0xC0, 0x1B);

    // Power control 1
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_NOP, 0x82);
    CPU_OTM8009A_Driver.SendCommandToDisplay(0xC5, 0x83);

    // Source driver precharge */
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_NOP, 0x81);
    CPU_OTM8009A_Driver.SendCommandToDisplay(0xC4, 0x83);
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_NOP, 0xA1);
    CPU_OTM8009A_Driver.SendCommandToDisplay(0xC1, 0x0E);
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_NOP, 0xA6);
    CPU_OTM8009A_Driver.SendDataToDisplay((CLR_UINT8 *)(CLR_UINT8[3]) { 0x00, 0x01, 0xB3 }, 3);

    // GOAVST
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_NOP, 0x80);
    CPU_OTM8009A_Driver.SendDataToDisplay((CLR_UINT8 *)(CLR_UINT8[7]){ 0x85,0x01,0x00,0x84,0x01,0x00,0xCE },7);
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_NOP, 0xA0);
    CPU_OTM8009A_Driver.SendDataToDisplay((CLR_UINT8 *)(CLR_UINT8[15]){ 0x18,0x04,0x03,0x39,0x00,0x00,0x00,0x18,0x03,0x03,0x3A,0x00,0x00,0x00,0xCE },15);
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_NOP, 0xB0);
    CPU_OTM8009A_Driver.SendDataToDisplay((CLR_UINT8 *)(CLR_UINT8[15]) { 0x18,0x02,0x03,0x3B,0x00,0x00,0x00,0x18,0x01,0x03,0x3C,0x00,0x00,0x00,0xCE },15);
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_NOP, 0xC0);
    CPU_OTM8009A_Driver.SendDataToDisplay((CLR_UINT8 *)(CLR_UINT8[11]){ 0x01,0x01,0x20,0x20,0x00,0x00,0x01,0x02,0x00,0x00,0xCF },11);
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_NOP, 0xD0);
    CPU_OTM8009A_Driver.SendCommandToDisplay(0xCF, 0x00);
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_NOP, 0x80);
    CPU_OTM8009A_Driver.SendDataToDisplay((CLR_UINT8 *)(CLR_UINT8[11]){ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xCB },11);
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_NOP, 0x90);
    CPU_OTM8009A_Driver.SendDataToDisplay((CLR_UINT8 *)(CLR_UINT8[16]){ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xCB },16);
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_NOP, 0xA0);
    CPU_OTM8009A_Driver.SendDataToDisplay((CLR_UINT8 *)(CLR_UINT8[16]){ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xCB },16);
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_NOP, 0xB0);
    CPU_OTM8009A_Driver.SendDataToDisplay((CLR_UINT8 *)(CLR_UINT8[11]){ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xCB },11);
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_NOP, 0xC0);
    CPU_OTM8009A_Driver.SendDataToDisplay((CLR_UINT8 *)(CLR_UINT8[16]){ 0x00,0x04,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xCB },16);
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_NOP, 0xD0);
    CPU_OTM8009A_Driver.SendDataToDisplay((CLR_UINT8 *)(CLR_UINT8[16]){ 0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0xCB },16);
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_NOP, 0xE0);
    CPU_OTM8009A_Driver.SendDataToDisplay((CLR_UINT8 *)(CLR_UINT8[11]){ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xCB },11);
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_NOP, 0xF0);
    CPU_OTM8009A_Driver.SendDataToDisplay((CLR_UINT8 *)(CLR_UINT8[11]){ 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xCB },11);
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_NOP, 0x80);
    CPU_OTM8009A_Driver.SendDataToDisplay((CLR_UINT8 *)(CLR_UINT8[11]){ 0x00,0x26,0x09,0x0B,0x01,0x25,0x00,0x00,0x00,0x00,0xCC },11);
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_NOP, 0x90);
    CPU_OTM8009A_Driver.SendDataToDisplay((CLR_UINT8 *)(CLR_UINT8[16]){ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x26,0x0A,0x0C,0x02,0xCC },16);
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_NOP, 0xA0);
    CPU_OTM8009A_Driver.SendDataToDisplay((CLR_UINT8 *)(CLR_UINT8[16]){ 0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xCC },16);
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_NOP, 0xB0);
    CPU_OTM8009A_Driver.SendDataToDisplay((CLR_UINT8 *)(CLR_UINT8[11]){ 0x00,0x25,0x0C,0x0A,0x02,0x26,0x00,0x00,0x00,0x00,0xCC },11);
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_NOP, 0xC0);
    CPU_OTM8009A_Driver.SendDataToDisplay((CLR_UINT8 *)(CLR_UINT8[16]){ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x0B,0x09,0x01,0xCC },16);
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_NOP, 0xD0);
    CPU_OTM8009A_Driver.SendDataToDisplay((CLR_UINT8 *)(CLR_UINT8[16]){ 0x26,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xCC },16);

    /////////////////////////////////////////////////////////////////////////////
    // PWR_CTRL1 - 0xc580h - 130th parameter - default 0x00
    // Pump 1 min and max DM
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_NOP, 0x81);
    CPU_OTM8009A_Driver.SendCommandToDisplay(0xC5, 0x66);
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_NOP, 0xB6);
    CPU_OTM8009A_Driver.SendCommandToDisplay(0xF5, 0x06);
    /////////////////////////////////////////////////////////////////////////////

    // Exit CMD2 mode
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_CMD_NOP, 0x00);
    CPU_OTM8009A_Driver.SendDataToDisplay((CLR_UINT8 *)(CLR_UINT8[4]) { 0xFF, 0xFF, 0xFF, 0xFF });

    //*************************************************************************** 
    // Standard DCS Initialization TO KEEP CAN BE DONE IN HSDT                   
    //*************************************************************************** 
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_CMD_NOP, 0x00);  // NOP - goes back to DCS std command ?
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_CMD_NOP, 0x00);  // Gamma correction 2.2+ table (HSDT possible)
    CPU_OTM8009A_Driver.SendDataToDisplay((CLR_UINT8 *)(CLR_UINT8[17]){ 0x00,0x09,0x0F,0x0E,0x07,0x10,0x0B,0x0A,0x04,0x07,0x0B,0x08,0x0F,0x10,0x0A,0x01,0xE1 },17);
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_CMD_NOP, 0x00);  // Gamma correction 2.2- table (HSDT possible)
    CPU_OTM8009A_Driver.SendDataToDisplay((CLR_UINT8 *)(CLR_UINT8[17]){ 0x00,0x09,0x0F,0x0E,0x07,0x10,0x0B,0x0A,0x04,0x07,0x0B,0x08,0x0F,0x10,0x0A,0x01,0xE2 },17);
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_SLPOUT, 0x00);    // Send Sleep Out command to display : no parameter
    osDelay(120);                             // Wait for sleep out exit

    uint32_t ColourCoding = PixelFormat::OTM8009A_FORMAT_RBG565;
    switch (ColourCoding)
    {
    case OTM8009A_FORMAT_RBG565:                        // Set Pixel color format to RGB565
        CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_COLMOD, OTM8009A_COLMOD_RGB565);
        break;
    case OTM8009A_FORMAT_RGB888:                        // Set Pixel color format to RGB888
        CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_COLMOD, OTM8009A_COLMOD_RGB888);
        break;
    default:
        break;
    }
    if (g_OTM8009A_Display_Driver.lcd_orientation == LANDSCAPE)  //  By default the orientation mode is portrait
    {
        // CASET value (Column Address Set) : X direction LCD GRAM boundaries
        // depending on LCD orientation mode and PASET value (Page Address Set) : Y direction
        // LCD GRAM boundaries depending on LCD orientation mode
        // XS[15:0] = 0x000 = 0, XE[15:0] = 0x31F = 799 for landscape mode : apply to CASET
        // YS[15:0] = 0x000 = 0, YE[15:0] = 0x31F = 799 for portrait mode : : apply to PASET
        CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_MADCTR, OTM8009A_MADCTR_MODE_LANDSCAPE);
        CPU_OTM8009A_Driver.SendDataToDisplay((CLR_UINT8 *)(CLR_UINT8[5]){ 0x00, 0x00, 0x03, 0x1F, OTM8009A_Commands::OTM8009A_CMD_CASET },5);
        // XS[15:0] = 0x000 = 0, XE[15:0] = 0x1DF = 479 for portrait mode : apply to CASET
        // YS[15:0] = 0x000 = 0, YE[15:0] = 0x1DF = 479 for landscape mode : apply to PASET
        CPU_OTM8009A_Driver.SendDataToDisplay((CLR_UINT8 *)(CLR_UINT8[5]){ 0x00, 0x00, 0x01, 0xDF, OTM8009A_Commands::OTM8009A_CMD_PASET },5);
    }
    // CABC : Content Adaptive Backlight Control section start >>
    // Note : defaut is 0 (lowest Brightness), 0xFF is highest Brightness, try 0x7F : intermediate value
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_WRDISBV, 0x7F);
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_WRCTRLD, 0x2C);        // defaut is 0, try 0x2C - Brightness Control Block, Display Dimming & BackLight on */
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_WRCABC, 0x02);         // defaut is 0, try 0x02 - image Content based Adaptive Brightness [Still Picture]
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_WRCABCMB, 0xFF);       // defaut is 0 (lowest Brightness), 0xFF is highest Brightness
    // CABC : Content Adaptive Backlight Control section end <</
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_DISPON, 0x00);         // Send Command Display On
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_CMD_NOP, 0x00);  // NOP
    // Send Command GRAM memory write (no parameters) : this initiates frame write via other DSI commands sent by/
    // DSI host from LTDC incoming pixels in video mode/
    CPU_OTM8009A_Driver.SendCommandToDisplay(OTM8009A_Commands::OTM8009A_CMD_RAMWR, 0x00);

    return TRUE;
}
bool Display_Driver_OTM8009A::Uninitialize()
{
    Clear();
    //FIXME: ??
    return TRUE;
}
void Display_Driver_OTM8009A::PowerSave(bool On)
{
    
    // FIXME: eliminate unused by creating dummy use of on
    if (On)
    {
        CPU_OTM8009A_Driver.SendCommandToDisplay(1); 
    }
    else
    {
        CPU_OTM8009A_Driver.SendCommandToDisplay(2);
    }
    return;
}
void Display_Driver_OTM8009A::Clear()
{
    //reset the cursor pos to the begining
    OTM8009A_Driver.m_cursor = 0;
    CPU_OTM8009A_Driver.Clear();
 
}
void Display_Driver_OTM8009A::BitBltEx(int x, int y, int width, int height, CLR_UINT32 data[])
{
    ASSERT((x >= 0) && ((x + width) <= LCD_SCREEN_WIDTH));
    ASSERT((y >= 0) && ((y + height) <= LCD_SCREEN_HEIGHT));

    CLR_UINT16 * StartOfLine_src = (CLR_UINT16*)&data[0];
    SetWindow(x, y, (x + width - 1), (y + height - 1));

    CLR_UINT16 offset = (y * g_OTM8009A_Config.Width) + x;
    StartOfLine_src += offset;

    while (height--)
    {
        //CLR_UINT16 * src;
        //int      Xcnt;
        //src = StartOfLine_src;
        //Xcnt = width;
        //while (Xcnt--)
        //{
        //    // SendDataWord(*src++);

        //}
        //StartOfLine_src += g_OTM8009A_Config.Width;
    }
}
//void Display_Driver_OTM8009A::BitBlt(int width, int height, int widthInWords, CLR_UINT32 data[], bool fUseDelta)
void Display_Driver_OTM8009A::BitBlt(int width, int height, int widthInWords, CLR_UINT32 data[])
{
    _ASSERTE(width == (int)LCD_GetWidth());
    _ASSERTE(height == (int)LCD_GetHeight());
    _ASSERTE(widthInWords == width / (int)PixelsPerWord());

    BitBltEx(0, 0, width, height, data);
}
void Display_Driver_OTM8009A::WriteChar(unsigned char c, int row, int col)
{
    // MUNEEB - Font_Width refers to font 8x8 or 8x15...so limiting it to 8x8.  tinyclr.proj defines that we are using 8x8
    //int width = Font_Width();
    //int height = Font_Height();
    int width = 8;
    int height = 8;

    // convert to LCD pixel coordinates
    row *= height;
    col *= width;

    if (row > (int)(OTM8009A_Config.Height - height)) return;
    if (col > (int)(OTM8009A_Config.Width - width)) return;

    const CLR_UINT8* font = Font_GetGlyph(c);
    // MUNEEB - SEE NOTE ABOVE
    //CLR_UINT16 data[height*width];
    CLR_UINT16 data[8 * 8];
    int i = 0;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            // FIXME: fix to allow compilation, code needs fixing

            // CLR_UINT16 val = OTM8009A_COLOUR::LCD_COLOR_BLACK;
            CLR_UINT16 val = 1;
            // the font data is mirrored
            // if ((font[y] & (1 << (width - x - 1))) != 0) val |= OTM8009A_COLOUR::LCD_COLOR_GREEN;
            if ((font[y] & (1 << (width - x - 1))) != 0) val |= 1;
            data[i] = val;
            i++;
        }
    }
    SetWindow(col, row, col + width - 1, row + height - 1);
    for (i = 0; i < width*height; i++)
    {
        CPU_OTM8009A_Driver.DrawPixel(col,row,data[i]);
    }
}
void Display_Driver_OTM8009A::WriteFormattedChar(unsigned char c)
{
    if (c < 32)
    {
        switch (c)
        {
        case '\b':                      /* backspace, clear previous char and move cursor back */
            if ((OTM8009A_Driver.m_cursor % TextColumns()) > 0)
            {
                OTM8009A_Driver.m_cursor--;
                WriteChar(' ', OTM8009A_Driver.m_cursor / TextColumns(), OTM8009A_Driver.m_cursor % TextColumns());
            }
            break;

        case '\f':                      /* formfeed, clear screen and home cursor */
            //Clear();
            OTM8009A_Driver.m_cursor = 0;
            break;

        case '\n':                      /* newline */
            OTM8009A_Driver.m_cursor += TextColumns();
            OTM8009A_Driver.m_cursor -= (OTM8009A_Driver.m_cursor % TextColumns());
            break;

        case '\r':                      /* carriage return */
            OTM8009A_Driver.m_cursor -= (OTM8009A_Driver.m_cursor % TextColumns());
            break;

        case '\t':                      /* horizontal tab */
            OTM8009A_Driver.m_cursor += (Font_TabWidth() - ((OTM8009A_Driver.m_cursor % TextColumns()) % Font_TabWidth()));
            // deal with line wrap scenario
            if ((OTM8009A_Driver.m_cursor % TextColumns()) < (CLR_UINT32)Font_TabWidth())
            {
                // bring the cursor to start of line
                OTM8009A_Driver.m_cursor -= (OTM8009A_Driver.m_cursor % TextColumns());
            }
            break;

        case '\v':                      /* vertical tab */
            OTM8009A_Driver.m_cursor += TextColumns();
            break;

        default:
            //FIXME:   DEBUTRACE2(TRACE_ALWAYS, "Unrecognized control character in LCD_WriteChar: %2u (0x%02x)\r\n", (unsigned int)c, (unsigned int)c);
            break;
        }
    }
    else
    {
        WriteChar(c, OTM8009A_Driver.m_cursor / TextColumns(), OTM8009A_Driver.m_cursor % TextColumns());
        OTM8009A_Driver.m_cursor++;
    }

    if (OTM8009A_Driver.m_cursor >= (TextColumns() * TextRows()))
    {
        OTM8009A_Driver.m_cursor = 0;
    }
}
CLR_UINT32 Display_Driver_OTM8009A::PixelsPerWord()
{
    return ((8 * sizeof(CLR_UINT32)) / g_OTM8009A_Config.BitsPerPixel);
}
CLR_UINT32 Display_Driver_OTM8009A::TextRows()
{
    return (g_OTM8009A_Config.Height / Font_Height());
}
CLR_UINT32 Display_Driver_OTM8009A::TextColumns()
{
    return (g_OTM8009A_Config.Width / Font_Width());
}
CLR_UINT32 Display_Driver_OTM8009A::WidthInWords()
{
    return ((g_OTM8009A_Config.Width + (PixelsPerWord() - 1)) / PixelsPerWord());
}
CLR_UINT32 Display_Driver_OTM8009A::SizeInWords()
{
    return (WidthInWords() * g_OTM8009A_Config.Height);
}
CLR_UINT32 Display_Driver_OTM8009A::SizeInBytes()
{
    return (SizeInWords() * sizeof(CLR_UINT32));
}
