

#include "nanohal.h"
#include "nanoCLR_Types.h"
#include "ST7735.h"
#include  "hal.h"

#include  "nanohal.h"


ST7735_Driver g_ST7735_Driver;

void ST7735_Driver::SendCmdWord(CLR_UINT16 cmd)
{
    dc->write(1);
    ce->write(0);
    spi->write(c);
    ce->write(1);
    return;
}
void ST7735_Driver::SendDataWord(CLR_UINT16 value)
{
    dc->write(0);
    ce->write(0);
    spi->write(c);
    ce->write(1);
    return;
}
void ST7735_Driver::SendCommandData(const CLR_UINT16 data[], CLR_UINT16 count)
{
    int i;
    for (i = 0; i < count; i += 2)
    {
        (void)SendCmdWord((CLR_UINT16)data[i]);
        (void)SendDataWord((CLR_UINT16)data[i + 1]);
    }
}
bool ST7735_Driver::SetWindow(CLR_UINT16 x1, CLR_UINT16 y1, CLR_UINT16 x2, CLR_UINT16 y2)
{
    CLR_UINT16 x1_x2;
    CLR_UINT16 Addr1, Addr2;

    switch (g_ST7735_Driver.lcd_orientation)
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

    SendCmdWord(ST7735_CASET);          // Column addr set
    SendDataWord(0x00);
    SendDataWord(x0 + colstart);        // XSTART 
    SendDataWord(0x00);
    SendDataWord(x1 + colstart);        // XEND

    SendCmdWord(ST7735_RASET);          // Row addr set
    SendDataWord(0x00);
    SendDataWord(y0 + rowstart);        // YSTART
    SendDataWord(0x00);
    SendDataWord(y1 + rowstart);        // YEND

    SendCmdWord(ST7735_RAMWR);          // write to RAM


    //Set Window
    (void)SendCmdWord(0x0044);
    (void)SendDataWord((CLR_UINT16)x1_x2);
    (void)SendCmdWord(0x0045);
    (void)SendDataWord((CLR_UINT16)y1);
    (void)SendCmdWord(0x0046);
    (void)SendDataWord((CLR_UINT16)y2);
    // Set Start Address counter
    (void)SendCmdWord(0x004e);
    (void)SendDataWord((CLR_UINT16)Addr1);
    (void)SendCmdWord(0x004f);
    (void)SendDataWord((CLR_UINT16)Addr2);
    (void)SendCmdWord(0x0022);

    return TRUE;
}
bool ST7735_Driver::Initialize()
{
    g_ST7735_Driver.m_cursor = 0;

    SendCmdWord(SLPOUT);        //Sleep exit 
    osDelay(250);

    SendCmdWord(FRMCTR1);       // Frame rate ctrl - normal mode, 3 args:
    SendDataWord(0x01);         //      Rate = fosc/(1x2+40) * (LINE+2C+2D)
    SendDataWord(0x2C);
    SendDataWord(0x2D);

    SendCmdWord(FRMCTR2);       // Frame rate control - idle mode, 3 args:
    SendDataWord(0x01);         //      Rate = fosc/(1x2+40) * (LINE+2C+2D)
    SendDataWord(0x2C);
    SendDataWord(0x2D);

    SendCmdWord(FRMCTR3);       // Frame rate ctrl - partial mode, 6 args:
    SendDataWord(0x01);         //      Dot inversion mode
    SendDataWord(0x2C);
    SendDataWord(0x2D);
    SendDataWord(0x01);         //      Line inversion mode
    SendDataWord(0x2C);
    SendDataWord(0x2D);

    SendCmdWord(INVCTR);        // Column inversion 
    SendDataWord(0x07);         //      No inversion

// Power up sequence               
    SendCmdWord(PWCTR1);        // Power control, 3 args, no delay
    SendDataWord(0xA2);
    SendDataWord(0x02);         //      -4.6V
    SendDataWord(0x84);         //      AUTO mode

    SendCmdWord(PWCTR2);        // Power control, 1 arg, no delay: 
    SendDataWord(0xC5);         //      VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD

    SendCmdWord(PWCTR3);        // Power control, 2 args, no delay:
    SendDataWord(0x0A);         //      Opamp current small
    SendDataWord(0x00);         //      Boost frequency

    SendCmdWord(PWCTR4);        // Power control, 2 args, no delay :
    SendDataWord(0x8A);         //      BCLK/2, Opamp current small & Medium low
    SendDataWord(0x2A);

    SendCmdWord(PWCTR5);        // Power control, 2 args, no delay:
    SendDataWord(0x8A);
    SendDataWord(0xEE);

    SendCmdWord(VMCTR1);        // Power control, 1 arg, no delay:
    SendDataWord(0x0E);

    SendCmdWord(COLMOD);        // Set color mode, 1 arg, no delay:
    SendDataWord(0x05);        //      16-bit color

    SendDataWord(MadctlMx |
        MadctlMy |
        MadctlBgr);            //  Row addr/col addr, bottom to top refresh

// ST7735R Gamma Sequence
    SendCmdWord(CASET);
    SendDataWord(0x00);
    SendDataWord(0x00);
    SendDataWord(0x00);
    SendDataWord(0x7f);

#define GMCTRP1 0xE0            // Init for 7735R, GMCTRP1
    SendCmdWord(GMCTRP1);      
    SendDataWord(0x0f);
    SendDataWord(0x1a);
    SendDataWord(0x0f);
    SendDataWord(0x18);
    SendDataWord(0x2f);
    SendDataWord(0x28);
    SendDataWord(0x20);
    SendDataWord(0x22);
    SendDataWord(0x1f);
    SendDataWord(0x1b);
    SendDataWord(0x23);
    SendDataWord(0x37);
    SendDataWord(0x00);
    SendDataWord(0x07);
    SendDataWord(0x02);
    SendDataWord(0x10);

    SendCmdWord(GMCTRN1);           // Init for 7735R, GMCTRN     
    SendDataWord(0x0f);
    SendDataWord(0x1b);
    SendDataWord(0x0f);
    SendDataWord(0x17);
    SendDataWord(0x33);
    SendDataWord(0x2c);
    SendDataWord(0x29);
    SendDataWord(0x2e);
    SendDataWord(0x30);
    SendDataWord(0x30);
    SendDataWord(0x39);
    SendDataWord(0x3f);
    SendDataWord(0x00);
    SendDataWord(0x07);
    SendDataWord(0x03);
    SendDataWord(0x10);

    // Green
    SendCmdWord(RASET);         // Column addr set, 4 args, no delay:                
    SendDataWord(0x00);         //      XSTART = 0 
    SendDataWord(0x00);
    SendDataWord(0x00);         //      XEND = 127 
    SendDataWord(0x7F + 0x02);
    SendCmdWord(RASET);         // Column addr set, 4 args, no delay:
    SendDataWord(0x00);         //      XSTART = 0
    SendDataWord(0x02);
    SendDataWord(0x00);         //      XEND = 159
    SendDataWord(0x9F + 0x01);


    SendCmdWord(0xF0);                      //Enable test command  

    SendDataWord(0x01);

    SendCmdWord(0xF6);                      //Disable ram power save mode 
    SendDataWord(0x00);

    SendCmdWord(MADCTL);                    // Memory access control (directions), 1 arg
    SendDataWord( MADCTL_MV | MADCTL_MY);

    SendCmdWord(ST7735_DISPON);             // Main screen turn on, no args w/delay

    osDelay(50);
    Clear();


    return TRUE;
}
bool ST7735_Driver::Uninitialize()
{
    Clear();
    return TRUE;
}
void ST7735_Driver::PowerSave(bool On)
{
    return;
} 
void ST7735_Driver::Clear()
{
    SetWindow(0, 0, LCD_HW_WIDTH - 1, LCD_HW_HEIGHT - 1);
    for (int i = 0; i < LCD_HW_WIDTH*LCD_HW_HEIGHT; i++)
    {
        SendDataWord(LCD_COLOR_BKG);
    }
    //reset the cursor pos to the begining
    g_ST7735_Driver.m_cursor = 0;
} //done
void ST7735_Driver::BitBltEx(int x, int y, int width, int height, CLR_UINT32 data[])
{
    ASSERT((x >= 0) && ((x + width) <= LCD_SCREEN_WIDTH));
    ASSERT((y >= 0) && ((y + height) <= LCD_SCREEN_HEIGHT));

    CLR_UINT16 * StartOfLine_src = (CLR_UINT16*)&data[0];
    SetWindow(x, y, (x + width - 1), (y + height - 1));

    CLR_UINT16 offset = (y * g_ST7735_Config.Width) + x;
    StartOfLine_src += offset;

    while (height--)
    {
        CLR_UINT16 * src;
        int      Xcnt;
        src = StartOfLine_src;
        Xcnt = width;
        while (Xcnt--)
        {
            SendDataWord(*src++);
        }
        StartOfLine_src += g_ST7735_Config.Width;
    }
}
void ST7735_Driver::BitBlt(int width, int height, int widthInWords, CLR_UINT32 data[], bool fUseDelta)
{
    _ASSERTE(width == LCD_GetWidth());
    _ASSERTE(height == LCD_GetHeight());
    _ASSERTE(widthInWords == width / PixelsPerWord());

    BitBltEx(0, 0, width, height, data);
} //done
void ST7735_Driver::WriteChar(unsigned char c, int row, int col)
{
    // MUNEEB - Font_Width refers to font 8x8 or 8x15...so limiting it to 8x8.  tinyclr.proj defines that we are using 8x8
    //int width = Font_Width();
    //int height = Font_Height();
    int width = 8;
    int height = 8;

    // convert to LCD pixel coordinates
    row *= height;
    col *= width;

    if (row > (g_ST7735_Config.Height - height)) return;
    if (col > (g_ST7735_Config.Width - width)) return;

    const CLR_UINT8* font = Font_GetGlyph(c);
    // MUNEEB - SEE NOTE ABOVE
    //CLR_UINT16 data[height*width];
    CLR_UINT16 data[8 * 8];
    int i = 0;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            CLR_UINT16 val = LCD_COLOR_BKG;
            // the font data is mirrored
            if ((font[y] & (1 << (width - x - 1))) != 0) val |= LCD_COLOR_TEXT;
            data[i] = val;
            i++;
        }
    }
    SetWindow(col, row, col + width - 1, row + height - 1);
    for (i = 0; i < width*height; i++)
    {
        SendDataWord(data[i]);
    }
} //done
void ST7735_Driver::WriteFormattedChar(unsigned char c)
{
    if (c < 32)
    {
        switch (c)
        {
        case '\b':                      /* backspace, clear previous char and move cursor back */
            if ((g_ST7735_Driver.m_cursor % TextColumns()) > 0)
            {
                g_ST7735_Driver.m_cursor--;
                WriteChar(' ', g_ST7735_Driver.m_cursor / TextColumns(), g_ST7735_Driver.m_cursor % TextColumns());
            }
            break;

        case '\f':                      /* formfeed, clear screen and home cursor */
            //Clear();
            g_ST7735_Driver.m_cursor = 0;
            break;

        case '\n':                      /* newline */
            g_ST7735_Driver.m_cursor += TextColumns();
            g_ST7735_Driver.m_cursor -= (g_ST7735_Driver.m_cursor % TextColumns());
            break;

        case '\r':                      /* carriage return */
            g_ST7735_Driver.m_cursor -= (g_ST7735_Driver.m_cursor % TextColumns());
            break;

        case '\t':                      /* horizontal tab */
            g_ST7735_Driver.m_cursor += (Font_TabWidth() - ((g_ST7735_Driver.m_cursor % TextColumns()) % Font_TabWidth()));
            // deal with line wrap scenario
            if ((g_ST7735_Driver.m_cursor % TextColumns()) < Font_TabWidth())
            {
                // bring the cursor to start of line
                g_ST7735_Driver.m_cursor -= (g_ST7735_Driver.m_cursor % TextColumns());
            }
            break;

        case '\v':                      /* vertical tab */
            g_ST7735_Driver.m_cursor += TextColumns();
            break;

        default:
            DEBUG_TRACE2(TRACE_ALWAYS, "Unrecognized control character in LCD_WriteChar: %2u (0x%02x)\r\n", (unsigned int)c, (unsigned int)c);
            break;
        }
    }
    else
    {
        WriteChar(c, g_ST7735_Driver.m_cursor / TextColumns(), g_ST7735_Driver.m_cursor % TextColumns());
        g_ST7735_Driver.m_cursor++;
    }

    if (g_ST7735_Driver.m_cursor >= (TextColumns() * TextRows()))
    {
        g_ST7735_Driver.m_cursor = 0;
    }
} //done
CLR_UINT32 ST7735_Driver::PixelsPerWord()
{
    return ((8 * sizeof(CLR_UINT32)) / g_ST7735_Config.BitsPerPixel);
} //done
CLR_UINT32 ST7735_Driver::TextRows()
{
    return (g_ST7735_Config.Height / Font_Height());
} //done
CLR_UINT32 ST7735_Driver::TextColumns()
{
    return (g_ST7735_Config.Width / Font_Width());
} //done
CLR_UINT32 ST7735_Driver::WidthInWords()
{
    return ((g_ST7735_Config.Width + (PixelsPerWord() - 1)) / PixelsPerWord());
} //done
CLR_UINT32 ST7735_Driver::SizeInWords()
{
    return (WidthInWords() * g_ST7735_Config.Height);
} //done
CLR_UINT32 ST7735_Driver::SizeInBytes()
{
    return (SizeInWords() * sizeof(CLR_UINT32));
} //done

