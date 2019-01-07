
#ifndef _DRIVERS_DISPLAY_ST7735_H_
#define _DRIVERS_DISPLAY_ST7735_H_ 1

#include "nanohal.h"
#include "nanoCLR_Types.h"
#include "cmsis_os.h"
#include "ch.h"
#include "hal.h"
#include "Display_Driver.h"

#define LCD_HW_HEIGHT 160  // Portrait height
#define LCD_HW_WIDTH  128  // Portrait width
#define LCD_ORIENTATION	   		  LANDSCAPE
#define SCREEN_SIZE_LONGER_SIDE   160           // The maximum resolution of longer longer side of physical LCD
#define SCREEN_SIZE_SHORTER_SIDE  128           // The maximum resolution of  shorter longer side of physical LCD

typedef enum
{
    PORTRAIT,
    PORTRAIT180,
    LANDSCAPE,
    LANDSCAPE180
} SCREEN_ORIENTATION;
enum  ST7735_COMMAND : CLR_UINT8 {
    INITR_GREENTAB = 0x0,
    INITR_REDTAB = 0x1,
    INITR_BLACKTAB = 0x2,
    NOP = 0x00,
    SWRESET = 0x01,
    RDDID = 0x04,
    RDDST = 0x09,                   // Read Display Status
    SLPIN = 0x10,                   //Sleep in & booster off
    SLPOUT = 0x11,                  // Sleep out & booster on
    PTLON = 0x12,                   // Partial Mode on
    NORON = 0x13,                   // Partial Off(Normal)
    INVOFF = 0x20,                  // Display Inversion Off
    INVON = 0x21,                   // Display Inversion On
    DISPOFF = 0x28,                 // Disply On
    DISPON = 0x29,                  // Display Off
    CASET = 0x2A,                   // Column address set
    RASET = 0x2B,                   // Row address set
    RAMWR = 0x2C,                   // Memory write
    RAMRD = 0x2E,                   // Memory read
    PTLAR = 0x30,                   // Partial start/end address set
    COLMOD = 0x3A,
    MADCTL = 0x36,                  // Memory access data control
    FRMCTR1 = 0xB1,
    FRMCTR2 = 0xB2,
    FRMCTR3 = 0xB3,
    INVCTR = 0xB4,
    DISSET5 = 0xB6,
    PWCTR1 = 0xC0,
    PWCTR2 = 0xC1,
    PWCTR3 = 0xC2,
    PWCTR4 = 0xC3,
    PWCTR5 = 0xC4,
    VMCTR1 = 0xC5,
    RDID1 = 0xDA,
    RDID2 = 0xDB,
    RDID3 = 0xDC,
    RDID4 = 0xDD,
    PWCTR6 = 0xFC,
    GMCTRP1 = 0xE0,
    GMCTRN1 = 0xE1

};
enum  ST7735_COLOUR : CLR_UINT16 {
    BLACK = 0x0000,
    BLUE = 0x001F,
    RED = 0xF800,
    GREEN = 0x07E0,
    CYAN = 0x07FF,
    MAGENTA = 0xF81F,
    YELLOW = 0xFFE0,
    WHITE = 0xFFFF,
    GREY = 0x39C4,
    NAVY = 0x000F,
    DARKGREEN = 0x03E0,
    MAROON = 0x7800,
    PURPLE = 0x780F,
    OLIVE = 0x7BE0,
    LIGHTGREY = 0xC618,
    DARKGREY = 0x7BEF,
    BROWN = 0xFD20,
    PINK = 0xF81F
};
enum  ST7735_MADCTL : CLR_UINT8 {       // Memory access data control
    RGB = 0x00,
    BGR = 0x08,
    MH = 0x04,
    ML = 0x10,
    MV = 0x20,
    MX = 0x40,
    MY = 0x80
};
struct Display_Driver_ST7735
{
    CLR_UINT32 m_cursor;
    SCREEN_ORIENTATION lcd_orientation;

    //FIXME:  static CLR_UINT16 VIDEO_RAM[(DISP_YMAX* DISP_XMAX )] ;
    static bool Initialize();
    static bool Uninitialize();
    static void PowerSave(bool On);
    static void Clear();
    static void BitBltEx(int x, int y, int width, int height, CLR_UINT32 data[]);
//    static void BitBlt(int width, int height, int widthInWords, CLR_UINT32 data[], bool fUseDelta);
    static void BitBlt(int width, int height, int widthInWords, CLR_UINT32 data[]);
    static void WriteChar(unsigned char c, int row, int col);
    static void WriteFormattedChar(unsigned char c);

private:
    static bool SetWindow(CLR_UINT16 x0, CLR_UINT16 y0, CLR_UINT16 x1, CLR_UINT16 y1);
    static CLR_UINT32 PixelsPerWord();
    static CLR_UINT32 TextRows();
    static CLR_UINT32 TextColumns();
    static CLR_UINT32 WidthInWords();
    static CLR_UINT32 SizeInWords();
    static CLR_UINT32 SizeInBytes();
};
extern Display_Driver_ST7735 g_ST7735_Driver;
extern DISPLAY_CONTROLLER_CONFIG g_ST7735_Config;

#endif  // _DRIVERS_DISPLAY_ST7735_H_



