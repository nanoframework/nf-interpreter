#ifndef ST7735_H
#define ST7735_H

#include "mbed.h"

// Comment out the #defines below with // to stop that font being loaded
// If all fonts are loaded the total space required is ablout 17890 bytes

#define LOAD_FONT2 // Small font, needs ~3092 bytes in FLASH
#define LOAD_FONT4 // Medium font, needs ~8126 bytes in FLASH
#define LOAD_FONT6 // Large font, needs ~4404 bytes in FLASH
#define LOAD_FONT7 // 7 segment font, needs ~3652 bytes in FLASH
#define LOAD_FONT8 // Large font needs ~10kbytes, only 1234567890:.

#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#define pgm_read_word(addr) (*(const unsigned short *)(addr))

#define swap(a, b) { int16_t t = a; a = b; b = t; }

#define INITR_GREENTAB  0x0
#define INITR_REDTAB    0x1
#define INITR_BLACKTAB  0x2

#define ST7735_TFTWIDTH  128
#define ST7735_TFTHEIGHT 160

#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09

#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13

#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_RAMRD   0x2E

#define ST7735_PTLAR   0x30
#define ST7735_COLMOD  0x3A
#define ST7735_MADCTL  0x36

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5

#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD

#define ST7735_PWCTR6  0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

// Color definitions
#define ST7735_BLACK      0x0000
#define ST7735_BLUE       0x001F
#define ST7735_RED        0xF800
#define ST7735_GREEN      0x07E0
#define ST7735_CYAN       0x07FF
#define ST7735_MAGENTA    0xF81F
#define ST7735_YELLOW     0xFFE0
#define ST7735_WHITE      0xFFFF
#define ST7735_GREY       0x39C4
#define ST7735_NAVY       0x000F
#define ST7735_DARKGREEN  0x03E0
#define ST7735_MAROON     0x7800
#define ST7735_PURPLE     0x780F
#define ST7735_OLIVE      0x7BE0
#define ST7735_LIGHTGREY  0xC618
#define ST7735_DARKGREY   0x7BEF
#define ST7735_BROWN      0xFD20
#define ST7735_PINK       0xF81F

#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH  0x04

class ST7735
{

public:
    /** Create a ST7735 object connected to the specified pins
    *
    * @param ce   Pin connected to chip enable
    * @param dc   Pin connected to data/command select
    * @param mosi Pin connected to data input
    * @param sclk Pin connected to serial clock
    *
    * Reset pin from LCD module is connected to reset pin microcontroller
    */

    ST7735(PinName cePin, PinName dcPin, PinName mosiPin, PinName sclkPin);

private:
    void writecommand(char c);
    void writedata(char c);
    void initSPI();
    void commonInit(const uint8_t *cmdList);
    void commandList(const uint8_t *addr);
    void pushColor(uint16_t color);

public:
    uint8_t tabcolor;
    uint8_t colstart, rowstart; // some displays need this changed
    void initB();
    void initR(uint8_t options);
    void setRotation(uint8_t m);
    void fillScreen(uint16_t color);
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
    void invertDisplay(bool i);
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
    void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
    uint16_t Color565(uint8_t r, uint8_t g, uint8_t b);
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
    void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);
    void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
    void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);
    void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
    void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
    void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
    void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
    void setCursor(int16_t x, int16_t y);
    void setTextSize(uint8_t s);
    void setTextColor(uint16_t c);
    void setTextColor(uint16_t c, uint16_t b);
    void setTextWrap(bool w);
    uint8_t getRotation(void);
    void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
    int drawUnicode(unsigned int uniCode, int x, int y, int font);
    int drawNumber(long long_num, int poX, int poY, int font);
    int drawChar(char c, int x, int y, int font);
    int drawString(char *string, int poX, int poY, int font);
    int drawCentreString(char *string, int dX, int poY, int font);
    int drawRightString(char *string, int dX, int poY, int font);
    int drawFloat(float floatNumber, int decimal, int poX, int poY, int font);



private:
    SPI*    spi;
    DigitalOut* ce;
    DigitalOut* rst;
    DigitalOut* dc;

protected:
    static const int16_t WIDTH = ST7735_TFTWIDTH;   // This is the 'raw' display w/h - never changes
    static const int16_t HEIGHT = ST7735_TFTHEIGHT;
    int16_t _width, _height; // Display w/h as modified by current rotation
    uint16_t textcolor, textbgcolor;
    uint8_t textsize;
    uint8_t rotation;
    bool wrap; // If set, 'wrap' text at right edge of display 
    int16_t cursor_x;
    int16_t cursor_y;
};

#endif
