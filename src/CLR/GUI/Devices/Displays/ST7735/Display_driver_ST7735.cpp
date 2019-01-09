
#include "nanohal.h"
#include "nanoCLR_Types.h"
#include "hal.h"
#include "nanohal.h"
#include "Display_driver.h"
#include "Display_driver_ST7735.h"

Display_Driver ST7735_Driver;
DISPLAY_CONTROLLER_CONFIG ST7735_Config;
extern CPU_GRAPHICS_DISPLAY g_Cpu_Display_Driver_ST7735;

bool Display_Driver_ST7735::SetWindow(CLR_UINT16 x1, CLR_UINT16 y1, CLR_UINT16 x2, CLR_UINT16 y2)
{
    CLR_UINT16 x1_x2;       // Send address as word
    CLR_UINT16 Addr1, Addr2;

    switch (ST7735_Driver.lcd_orientation)
    {
    default:
        // Invalid! Fall through to portrait mode
    case PORTRAIT:
        Addr1 = x1;
        Addr2 = y1;
        x1_x2 = (CLR_UINT16)((x2 << 8) + x1);
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
        Addr2 = (CLR_UINT16)(SCREEN_SIZE_LONGER_SIDE - 1 - x1);                     // pack X-Values into one word
        x1_x2 = (CLR_UINT16)((y2 << 8) + y1);
        y1 = (CLR_UINT16)(SCREEN_SIZE_LONGER_SIDE - 1 - x2);
        y2 = Addr2;
        break;
    }
    g_Cpu_Display_Driver_ST7735.SendCommandToDisplay(ST7735_COMMAND::CASET);          // Column addr set
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(x1_x2);
    g_Cpu_Display_Driver_ST7735.SendCommandToDisplay(ST7735_COMMAND::RASET);          // Row addr set
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(y1 + 2);                            // YSTART
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(y2 + 2);                            // YEND


 //-------------------------


    g_Cpu_Display_Driver_ST7735.SendCommandToDisplay(ST7735_COMMAND::RAMWR);          // write to RAM

    return TRUE;
}
bool Display_Driver_ST7735::Initialize()
{
    
    // The display for 1.8" tft has changed slightly over time
    // The tft plastic wrap has different coloured tabs
    // This is coded for the Red Tab
    
    ST7735_Driver.m_cursor = 0;

    g_Cpu_Display_Driver_ST7735.SendCommandToDisplay(ST7735_COMMAND::SWRESET);
    g_Cpu_Display_Driver_ST7735.SendCommandToDisplay(ST7735_COMMAND::SLPOUT);
    g_Cpu_Display_Driver_ST7735.SendCommandToDisplay(ST7735_COMMAND::FRMCTR1);        // Frame rate ctrl - normal mode, 3 args:
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x01);                              //      Rate = fosc/(1x2+40) * (LINE+2C+2D)
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x2C);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x2D);
    g_Cpu_Display_Driver_ST7735.SendCommandToDisplay(ST7735_COMMAND::FRMCTR2);        // Frame rate control - idle mode, 3 args:
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x01);                              //      Rate = fosc/(1x2+40) * (LINE+2C+2D)
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x2C);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x2D);
    g_Cpu_Display_Driver_ST7735.SendCommandToDisplay(ST7735_COMMAND::FRMCTR3);    // Frame rate ctrl - partial mode, 6 args:
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x01);                          //      Dot inversion mode
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x2C);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x2D);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x01);                          //      Line inversion mode
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x2C);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x2D);
    g_Cpu_Display_Driver_ST7735.SendCommandToDisplay(ST7735_COMMAND::INVCTR);     // Column inversion 
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x07);                          //      No inversion
    g_Cpu_Display_Driver_ST7735.SendCommandToDisplay(ST7735_COMMAND::PWCTR1);     // Power control, 3 args, no delay
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0xA2);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x02);                          //      -4.6V
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x84);                          //      AUTO mode
    g_Cpu_Display_Driver_ST7735.SendCommandToDisplay(ST7735_COMMAND::PWCTR2);     // Power control, 1 arg, no delay: 
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0xC5);                          //      VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
    g_Cpu_Display_Driver_ST7735.SendCommandToDisplay(ST7735_COMMAND::PWCTR3);     // Power control, 2 args, no delay:
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x0A);                          //      Opamp current small
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x00);                          //      Boost frequency
    g_Cpu_Display_Driver_ST7735.SendCommandToDisplay(ST7735_COMMAND::PWCTR4);     // Power control, 2 args, no delay :
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x8A);                          //      BCLK/2, Opamp current small & Medium low
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x2A);
    g_Cpu_Display_Driver_ST7735.SendCommandToDisplay(ST7735_COMMAND::PWCTR5);     // Power control, 2 args, no delay:
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x8A);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0xEE);
    g_Cpu_Display_Driver_ST7735.SendCommandToDisplay(ST7735_COMMAND::VMCTR1);     // Power control, 1 arg, no delay:
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x0E);
    g_Cpu_Display_Driver_ST7735.SendCommandToDisplay(ST7735_COMMAND::COLMOD);     // Set color mode, 1 arg, no delay:
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x05);                          //      16-bit color

// ST7735R Gamma Sequence
    // Part 1 - Displays with Red Tab
    g_Cpu_Display_Driver_ST7735.SendCommandToDisplay(ST7735_COMMAND::CASET);      // Column addr set
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x00);                          // Column.. (0,127)
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x02);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x00);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x7f + 0x02);
    g_Cpu_Display_Driver_ST7735.SendCommandToDisplay(ST7735_COMMAND::RASET);      // Row addr set
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x00);                          //  Row.. (0,159)
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x01);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x00);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x9F + 0x01);

    // Part 2 - Displays with Red Tab
    g_Cpu_Display_Driver_ST7735.SendCommandToDisplay(ST7735_COMMAND::RASET);      // Column addr set
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x00);                          // Column.. (0,127)
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x00);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x00);                         
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x7F);
    g_Cpu_Display_Driver_ST7735.SendCommandToDisplay(ST7735_COMMAND::RASET);      // Row addr set
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x00);                          //  Row.. (0,159)
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x02);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x00);                         
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x9F);

    // Part 3 - Displays with Red Tab
    g_Cpu_Display_Driver_ST7735.SendCommandToDisplay(ST7735_COMMAND::GMCTRP1);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x02);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x1D);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x07);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x12);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x37);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x32);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x29);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x2D);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x29);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x25);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x2B);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x39);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x00);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x01);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x03);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x10);
    g_Cpu_Display_Driver_ST7735.SendCommandToDisplay(ST7735_COMMAND::GMCTRN1);     // Init for 7735R
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x03);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x1D);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x07);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x06);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x2E);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x2C);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x29);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x2D);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x2E);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x2E);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x37);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x3F);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x00);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x00);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x02);
    g_Cpu_Display_Driver_ST7735.SendDataToDisplay(0x10);

    g_Cpu_Display_Driver_ST7735.SendCommandToDisplay(ST7735_COMMAND::NORON);        // Normal display on
    g_Cpu_Display_Driver_ST7735.SendCommandToDisplay(ST7735_COMMAND::DISPON);       // Main screen turn on

    Clear();
    return TRUE;
}
bool Display_Driver_ST7735::Uninitialize()
{
    Clear();
    return TRUE;
}
void Display_Driver_ST7735::PowerSave(bool On)
{
    if (On)
    {
        g_Cpu_Display_Driver_ST7735.SendCommandToDisplay(ST7735_COMMAND::SLPIN);     
    }
    else
    {
        g_Cpu_Display_Driver_ST7735.SendCommandToDisplay(ST7735_COMMAND::SLPOUT);
    }
    return;
}
void Display_Driver_ST7735::Clear()
{
    SetWindow(0, 0, LCD_HW_WIDTH - 1, LCD_HW_HEIGHT - 1);
    for (int i = 0; i < LCD_HW_WIDTH*LCD_HW_HEIGHT; i++)
    {
        g_Cpu_Display_Driver_ST7735.SendDataToDisplay(ST7735_COLOUR::BLACK);
    }
    //reset the cursor pos to the begining
    ST7735_Driver.m_cursor = 0;
} //done
void Display_Driver_ST7735::BitBltEx(int x, int y, int width, int height, CLR_UINT32 data[])
{
    ASSERT((x >= 0) && ((x + width) <= LCD_SCREEN_WIDTH));
    ASSERT((y >= 0) && ((y + height) <= LCD_SCREEN_HEIGHT));

    CLR_UINT16 * StartOfLine_src = (CLR_UINT16*)&data[0];
    SetWindow(x, y, (x + width - 1), (y + height - 1));

    CLR_UINT16 offset = (y * ST7735_Config.Width) + x;
    StartOfLine_src += offset;

    while (height--)
    {
        CLR_UINT16 * src;
        int      Xcnt;
        src = StartOfLine_src;
        Xcnt = width;
        while (Xcnt--)
        {
            g_Cpu_Display_Driver_ST7735.SendDataToDisplay(*src++);
        }
        StartOfLine_src += ST7735_Config.Width;
    }
}
//void Display_Driver_ST7735::BitBlt(int width, int height, int widthInWords, CLR_UINT32 data[], bool fUseDelta)
void Display_Driver_ST7735::BitBlt(int width, int height, int widthInWords, CLR_UINT32 data[])
{
    _ASSERTE(width == (int)LCD_GetWidth());
    _ASSERTE(height == (int)LCD_GetHeight());
    _ASSERTE(widthInWords == width / (int)PixelsPerWord());

    BitBltEx(0, 0, width, height, data);
} //done
void Display_Driver_ST7735::WriteChar(unsigned char c, int row, int col)
{
    // MUNEEB - Font_Width refers to font 8x8 or 8x15...so limiting it to 8x8.  tinyclr.proj defines that we are using 8x8
    //int width = Font_Width();
    //int height = Font_Height();
    int width = 8;
    int height = 8;

    // convert to LCD pixel coordinates
    row *= height;
    col *= width;

    if (row > (int)(ST7735_Config.Height - height)) return;
    if (col > (int)(ST7735_Config.Width - width)) return;

    const CLR_UINT8* font = Font_GetGlyph(c);
    // MUNEEB - SEE NOTE ABOVE
    //CLR_UINT16 data[height*width];
    CLR_UINT16 data[8 * 8];
    int i = 0;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            CLR_UINT16 val = ST7735_COLOUR::BLACK;
            // the font data is mirrored
            if ((font[y] & (1 << (width - x - 1))) != 0) val |= ST7735_COLOUR::GREEN;
            data[i] = val;
            i++;
        }
    }
    SetWindow(col, row, col + width - 1, row + height - 1);
    for (i = 0; i < width*height; i++)
    {
        g_Cpu_Display_Driver_ST7735.SendDataToDisplay(data[i]);
    }
} //done
void Display_Driver_ST7735::WriteFormattedChar(unsigned char c)
{
    if (c < 32)
    {
        switch (c)
        {
        case '\b':                      /* backspace, clear previous char and move cursor back */
            if ((ST7735_Driver.m_cursor % TextColumns()) > 0)
            {
                ST7735_Driver.m_cursor--;
                WriteChar(' ', ST7735_Driver.m_cursor / TextColumns(), ST7735_Driver.m_cursor % TextColumns());
            }
            break;

        case '\f':                      /* formfeed, clear screen and home cursor */
            //Clear();
            ST7735_Driver.m_cursor = 0;
            break;

        case '\n':                      /* newline */
            ST7735_Driver.m_cursor += TextColumns();
            ST7735_Driver.m_cursor -= (ST7735_Driver.m_cursor % TextColumns());
            break;

        case '\r':                      /* carriage return */
            ST7735_Driver.m_cursor -= (ST7735_Driver.m_cursor % TextColumns());
            break;

        case '\t':                      /* horizontal tab */
            ST7735_Driver.m_cursor += (Font_TabWidth() - ((ST7735_Driver.m_cursor % TextColumns()) % Font_TabWidth()));
            // deal with line wrap scenario
            if ((ST7735_Driver.m_cursor % TextColumns()) < (CLR_UINT32)Font_TabWidth())
            {
                // bring the cursor to start of line
                ST7735_Driver.m_cursor -= (ST7735_Driver.m_cursor % TextColumns());
            }
            break;

        case '\v':                      /* vertical tab */
            ST7735_Driver.m_cursor += TextColumns();
            break;

        default:
            //FIXME:   DEBUTRACE2(TRACE_ALWAYS, "Unrecognized control character in LCD_WriteChar: %2u (0x%02x)\r\n", (unsigned int)c, (unsigned int)c);
            break;
        }
    }
    else
    {
        WriteChar(c, ST7735_Driver.m_cursor / TextColumns(), ST7735_Driver.m_cursor % TextColumns());
        ST7735_Driver.m_cursor++;
    }

    if (ST7735_Driver.m_cursor >= (TextColumns() * TextRows()))
    {
        ST7735_Driver.m_cursor = 0;
    }
} //done
CLR_UINT32 Display_Driver_ST7735::PixelsPerWord()
{
    return ((8 * sizeof(CLR_UINT32)) / ST7735_Config.BitsPerPixel);
} //done
CLR_UINT32 Display_Driver_ST7735::TextRows()
{
    return (ST7735_Config.Height / Font_Height());
} //done
CLR_UINT32 Display_Driver_ST7735::TextColumns()
{
    return (ST7735_Config.Width / Font_Width());
} //done
CLR_UINT32 Display_Driver_ST7735::WidthInWords()
{
    return ((ST7735_Config.Width + (PixelsPerWord() - 1)) / PixelsPerWord());
} //done
CLR_UINT32 Display_Driver_ST7735::SizeInWords()
{
    return (WidthInWords() * ST7735_Config.Height);
} //done
CLR_UINT32 Display_Driver_ST7735::SizeInBytes()
{
    return (SizeInWords() * sizeof(CLR_UINT32));
} //done
