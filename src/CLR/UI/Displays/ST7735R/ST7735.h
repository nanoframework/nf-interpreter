
#ifndef _DRIVERS_DISPLAY_ST7735_H_
#define _DRIVERS_DISPLAY_ST7735_H_ 1

#include "nanohal.h"
#include "nanoCLR_Types.h"
#include "cmis_os.h"
#include "ch.h"
#include "hal.h"

// LCD Size Defines
#define LCD_HW_HEIGHT 160  // Portrait height
#define LCD_HW_WIDTH  120  // Portrait width

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"

#pragma region 7735 register definitions
    #define INITR_GREENTAB  0x0
    #define INITR_REDTAB    0x1
    #define INITR_BLACKTAB  0x2

    #define TFTWIDTH  128
    #define TFTHEIGHT 160

    #define NOP     0x00
    #define SWRESET 0x01
    #define RDDID   0x04
    #define RDDST   0x09

    #define SLPIN   0x10
    #define SLPOUT  0x11
    #define PTLON   0x12
    #define NORON   0x13

    #define INVOFF  0x20
    #define INVON   0x21
    #define DISPOFF 0x28
    #define DISPON  0x29
    #define CASET   0x2A
    #define RASET   0x2B
    #define RAMWR   0x2C
    #define RAMRD   0x2E

    #define PTLAR   0x30
    #define COLMOD  0x3A
    #define MADCTL  0x36

    #define FRMCTR1 0xB1
    #define FRMCTR2 0xB2
    #define FRMCTR3 0xB3
    #define INVCTR  0xB4
    #define DISSET5 0xB6

    #define PWCTR1  0xC0
    #define PWCTR2  0xC1
    #define PWCTR3  0xC2
    #define PWCTR4  0xC3
    #define PWCTR5  0xC4
    #define VMCTR1  0xC5

    #define RDID1   0xDA
    #define RDID2   0xDB
    #define RDID3   0xDC
    #define RDID4   0xDD

    #define PWCTR6  0xFC

    #define GMCTRP1 0xE0
    #define GMCTRN1 0xE1

    #define MADCTL_MY  0x80
    #define MADCTL_MX  0x40
    #define MADCTL_MV  0x20
    #define MADCTL_ML  0x10
    #define MADCTL_RGB 0x00
    #define MADCTL_BGR 0x08
    #define MADCTL_MH  0x04
#pragma endregion

#pragma region 7735 colour definitions
    #define LCD_COLOR_RGB(R,G,B) ((CLR_UINT16)((((R) / 8) << 11) + (((G) / 4) << 5) + ((B) / 8)))
    #define LCD_COLOR_DARK_BLUE     LCD_COLOR_RGB(0, 0, 152)  
    #define LCD_COLOR_BRIGHT_BLUE   LCD_COLOR_RGB(222, 219, 255)
    #define LCD_COLOR_BLUE          LCD_COLOR_RGB(0, 0, 255)  
    #define LCD_COLOR_CYAN          LCD_COLOR_RGB(0, 255, 255)
    #define LCD_COLOR_BRIGHT_YELLOW LCD_COLOR_RGB(255, 220, 120)
    #define LCD_COLOR_YELLOW        LCD_COLOR_RGB(255, 255, 0) 
    #define LCD_COLOR_ORANGE        LCD_COLOR_RGB(255, 152, 96)
    #define LCD_COLOR_BRIGHT_RED    LCD_COLOR_RGB(255, 28, 24)  
    #define LCD_COLOR_RED           LCD_COLOR_RGB(255, 0, 0)  
    #define LCD_COLOR_DARK_RED      LCD_COLOR_RGB(152, 0, 0)  
    #define LCD_COLOR_MAGENTA       LCD_COLOR_RGB(255, 0, 255)  
    #define LCD_COLOR_BRIGHT_GREEN  LCD_COLOR_RGB(152, 255, 152)
    #define LCD_COLOR_GREEN         LCD_COLOR_RGB(0, 255, 0)  
    #define LCD_COLOR_DARK_GREEN    LCD_COLOR_RGB(0, 128, 0)  
    #define LCD_COLOR_BRIGHT_GREY   LCD_COLOR_RGB(48, 48, 48)  
    #define LCD_COLOR_LIGHT_GREY    LCD_COLOR_RGB(120, 120, 120)
    #define LCD_COLOR_GREY          LCD_COLOR_RGB(24, 24, 24)   
    #define LCD_COLOR_WHITE         LCD_COLOR_RGB(255, 255, 255)
    #define LCD_COLOR_BLACK         LCD_COLOR_RGB(0, 0, 0)
    #define LCD_COLOR_BKG			LCD_COLOR_BLACK
    #define LCD_COLOR_TEXT			LCD_COLOR_GREEN
#pragma endregion

#define LCD_ORIENTATION	   				  PORTRAIT
#define SCREEN_SIZE_LONGER_SIDE  160        // The maximum resolution of longer longer side of physical LCD
#define SCREEN_SIZE_SHORTER_SIDE  128       // The maximum resolution of  shorter longer side of physical LCD

typedef enum
{
    PORTRAIT,
    PORTRAIT180,
    LANDSCAPE,
    LANDSCAPE180
} SCREEN_ORIENTATION;

struct ST7735_Driver
{
    CLR_UINT32 m_cursor;
    SCREEN_ORIENTATION lcd_orientation;

    //static CLR_UINT16 VIDEO_RAM[(DISP_YMAX* DISP_XMAX )] ;
    static bool Initialize();
    static bool Uninitialize();
    static void PowerSave(bool On);
    static void Clear();
    static void BitBltEx(int x, int y, int width, int height, CLR_UINT32 data[]);
    static void BitBlt(int width, int height, int widthInWords, CLR_UINT32 data[], bool fUseDelta);
    static void WriteChar(unsigned char c, int row, int col);
    static void WriteFormattedChar(unsigned char c);

private:
    static bool SetWindow(CLR_UINT16 x0, CLR_UINT16 y0, CLR_UINT16 x1, CLR_UINT16 y1);
    static void SendCmdWord(CLR_UINT16 cmd);
    static void SendDataWord(CLR_UINT16 data);
    static void SendCommandData(const CLR_UINT16 data[], CLR_UINT16 count);
    static CLR_UINT32 PixelsPerWord();
    static CLR_UINT32 TextRows();
    static CLR_UINT32 TextColumns();
    static CLR_UINT32 WidthInWords();
    static CLR_UINT32 SizeInWords();
    static CLR_UINT32 SizeInBytes();
};

extern ST7735_Driver g_ST7735_Driver;
extern DISPLAY_CONTROLLER_CONFIG g_ST7735_Config;

extern spi
extern SPI_CONFIGURATION g_ST7735_SPI_Config;


#pragma GCC diagnostic pop
#endif  // _DRIVERS_DISPLAY_ST7735_H_



