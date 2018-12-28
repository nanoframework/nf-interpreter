#include "mbed.h"
#include "ST7735.h"

#ifdef LOAD_FONT2
#include "Font16.h"
#endif

#ifdef LOAD_FONT4
#include "Font32.h"
#endif

#ifdef LOAD_FONT6
#include "Font64.h"
#endif

#ifdef LOAD_FONT7
#include "Font7s.h"
#endif

#ifdef LOAD_FONT8
#include "Font72.h"
#endif


ST7735::ST7735(PinName cePin, PinName dcPin, PinName mosiPin, PinName sclkPin)
{
    spi = new SPI(mosiPin, NC, sclkPin); // create new SPI instance and initialise
    initSPI();

    // set up other pins as required
    ce = new DigitalOut(cePin, 1);
    dc = new DigitalOut(dcPin, 0);

    // Defaults after power-up
    textsize = 1;
    textcolor = ST7735_GREEN;
    textbgcolor = ST7735_BLACK;
}

// function to initialise SPI peripheral
void ST7735::initSPI()
{
    spi->format(8, 0);         // 8 bits, Mode 0
    spi->frequency(20000000);  // SPI clock frequency
}

// Write data word 
void ST7735::writecommand(char c)
{
    dc->write(0);
    ce->write(0);
    spi->write(c);
    ce->write(1);
}

// Write command word
void ST7735::writedata(char c)
{
    dc->write(1);
    ce->write(0);
    spi->write(c);
    ce->write(1);
}


// Rather than a bazillion writecommand() and writedata() calls, screen
// initialization commands and arguments are organized in tables
// The table may look bulky, but that's mostly the
// formatting -- storage-wise this is hundreds of bytes more compact
// than the equivalent code.  Companion function follows.
#define DELAY 0x80
static const uint8_t Bcmd[] = {  // Initialization commands for 7735B screens
    18,                       // 18 commands in list:
    ST7735_SWRESET,   DELAY,  //  1: Software reset, no args, w/delay
      50,                     //     50 ms delay
    ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, no args, w/delay
      255,                    //     255 = 500 ms delay
    ST7735_COLMOD , 1 + DELAY,  //  3: Set color mode, 1 arg + delay:
      0x05,                   //     16-bit color
      10,                     //     10 ms delay
    ST7735_FRMCTR1, 3 + DELAY,  //  4: Frame rate control, 3 args + delay:
      0x00,                   //     fastest refresh
      0x06,                   //     6 lines front porch
      0x03,                   //     3 lines back porch
      10,                     //     10 ms delay
    ST7735_MADCTL , 1      ,  //  5: Memory access ctrl (directions), 1 arg:
      0x08,                   //     Row addr/col addr, bottom to top refresh
    ST7735_DISSET5, 2      ,  //  6: Display settings #5, 2 args, no delay:
      0x15,                   //     1 clk cycle nonoverlap, 2 cycle gate
                              //     rise, 3 cycle osc equalize
      0x02,                   //     Fix on VTL
    ST7735_INVCTR , 1      ,  //  7: Display inversion control, 1 arg:
      0x0,                    //     Line inversion
    ST7735_PWCTR1 , 2 + DELAY,  //  8: Power control, 2 args + delay:
      0x02,                   //     GVDD = 4.7V
      0x70,                   //     1.0uA
      10,                     //     10 ms delay
    ST7735_PWCTR2 , 1      ,  //  9: Power control, 1 arg, no delay:
      0x05,                   //     VGH = 14.7V, VGL = -7.35V
    ST7735_PWCTR3 , 2      ,  // 10: Power control, 2 args, no delay:
      0x01,                   //     Opamp current small
      0x02,                   //     Boost frequency
    ST7735_VMCTR1 , 2 + DELAY,  // 11: Power control, 2 args + delay:
      0x3C,                   //     VCOMH = 4V
      0x38,                   //     VCOML = -1.1V
      10,                     //     10 ms delay
    ST7735_PWCTR6 , 2      ,  // 12: Power control, 2 args, no delay:
      0x11, 0x15,
    ST7735_GMCTRP1,16      ,  // 13: Magical unicorn dust, 16 args, no delay:
      0x09, 0x16, 0x09, 0x20, //     (seriously though, not sure what
      0x21, 0x1B, 0x13, 0x19, //      these config values represent)
      0x17, 0x15, 0x1E, 0x2B,
      0x04, 0x05, 0x02, 0x0E,
    ST7735_GMCTRN1,16 + DELAY,  // 14: Sparkles and rainbows, 16 args + delay:
      0x0B, 0x14, 0x08, 0x1E, //     (ditto)
      0x22, 0x1D, 0x18, 0x1E,
      0x1B, 0x1A, 0x24, 0x2B,
      0x06, 0x06, 0x02, 0x0F,
      10,                     //     10 ms delay
    ST7735_CASET  , 4      ,  // 15: Column addr set, 4 args, no delay:
      0x00, 0x02,             //     XSTART = 2
      0x00, 0x81,             //     XEND = 129
    ST7735_RASET  , 4      ,  // 16: Row addr set, 4 args, no delay:
      0x00, 0x02,             //     XSTART = 1
      0x00, 0x81,             //     XEND = 160
    ST7735_NORON  ,   DELAY,  // 17: Normal display on, no args, w/delay
      10,                     //     10 ms delay
    ST7735_DISPON ,   DELAY,  // 18: Main screen turn on, no args, w/delay
      255 },                  //     255 = 500 ms delay

      Rcmd1[] = {                 // Init for 7735R, part 1 (red or green tab)
        15,                       // 15 commands in list:
        ST7735_SWRESET,   DELAY,  //  1: Software reset, 0 args, w/delay
          150,                    //     150 ms delay
        ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, 0 args, w/delay
          255,                    //     500 ms delay
        ST7735_FRMCTR1, 3      ,  //  3: Frame rate ctrl - normal mode, 3 args:
          0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
        ST7735_FRMCTR2, 3      ,  //  4: Frame rate control - idle mode, 3 args:
          0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
        ST7735_FRMCTR3, 6      ,  //  5: Frame rate ctrl - partial mode, 6 args:
          0x01, 0x2C, 0x2D,       //     Dot inversion mode
          0x01, 0x2C, 0x2D,       //     Line inversion mode
        ST7735_INVCTR , 1      ,  //  6: Display inversion ctrl, 1 arg, no delay:
          0x07,                   //     No inversion
        ST7735_PWCTR1 , 3      ,  //  7: Power control, 3 args, no delay:
          0xA2,
          0x02,                   //     -4.6V
          0x84,                   //     AUTO mode
        ST7735_PWCTR2 , 1      ,  //  8: Power control, 1 arg, no delay:
          0xC5,                   //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
        ST7735_PWCTR3 , 2      ,  //  9: Power control, 2 args, no delay:
          0x0A,                   //     Opamp current small
          0x00,                   //     Boost frequency
        ST7735_PWCTR4 , 2      ,  // 10: Power control, 2 args, no delay:
          0x8A,                   //     BCLK/2, Opamp current small & Medium low
          0x2A,
        ST7735_PWCTR5 , 2      ,  // 11: Power control, 2 args, no delay:
          0x8A, 0xEE,
        ST7735_VMCTR1 , 1      ,  // 12: Power control, 1 arg, no delay:
          0x0E,
        ST7735_INVOFF , 0      ,  // 13: Don't invert display, no args, no delay
        ST7735_MADCTL , 1      ,  // 14: Memory access control (directions), 1 arg:
          0xC8,                   //     row addr/col addr, bottom to top refresh
        ST7735_COLMOD , 1      ,  // 15: set color mode, 1 arg, no delay:
          0x05 },                 //     16-bit color

          Rcmd2green[] = {            // Init for 7735R, part 2 (green tab only)
            2,                        //  2 commands in list:
            ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
              0x00, 0x02,             //     XSTART = 0
              0x00, 0x7F + 0x02,        //     XEND = 127
            ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
              0x00, 0x01,             //     XSTART = 0
              0x00, 0x9F + 0x01 },      //     XEND = 159
              Rcmd2red[] = {              // Init for 7735R, part 2 (red tab only)
                2,                        //  2 commands in list:
                ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
                  0x00, 0x00,             //     XSTART = 0
                  0x00, 0x7F,             //     XEND = 127
                ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
                  0x00, 0x00,             //     XSTART = 0
                  0x00, 0x9F },           //     XEND = 159

                  Rcmd3[] = {                 // Init for 7735R, part 3 (red or green tab)
                    4,                        //  4 commands in list:
                    ST7735_GMCTRP1, 16      , //  1: Magical unicorn dust, 16 args, no delay:
                      0x02, 0x1c, 0x07, 0x12,
                      0x37, 0x32, 0x29, 0x2d,
                      0x29, 0x25, 0x2B, 0x39,
                      0x00, 0x01, 0x03, 0x10,
                    ST7735_GMCTRN1, 16      , //  2: Sparkles and rainbows, 16 args, no delay:
                      0x03, 0x1d, 0x07, 0x06,
                      0x2E, 0x2C, 0x29, 0x2D,
                      0x2E, 0x2E, 0x37, 0x3F,
                      0x00, 0x00, 0x02, 0x10,
                    ST7735_NORON  ,    DELAY, //  3: Normal display on, no args, w/delay
                      10,                     //     10 ms delay
                    ST7735_DISPON ,    DELAY, //  4: Main screen turn on, no args w/delay
                      100 };                  //     100 ms delay



                // Companion code to the above tables.  Reads and issues
                // a series of LCD commands stored in PROGMEM byte array.
void ST7735::commandList(const uint8_t *addr) {
    uint8_t  numCommands, numArgs;
    uint16_t ms;

    numCommands = pgm_read_byte(addr++);   // Number of commands to follow
    while (numCommands--) {                 // For each command...
        writecommand(pgm_read_byte(addr++)); //   Read, issue command
        numArgs = pgm_read_byte(addr++);    //   Number of args to follow
        ms = numArgs & DELAY;          //   If hibit set, delay follows args
        numArgs &= ~DELAY;                   //   Mask out delay bit
        while (numArgs--) {                   //   For each argument...
            writedata(pgm_read_byte(addr++));    //   Read, issue argument
        }

        if (ms) {
            ms = pgm_read_byte(addr++); // Read post-command delay time (ms)
            if (ms == 255) ms = 500;     // If 255, delay for 500 ms
            wait_ms(ms);
        }
    }
}

// Initialization code common to both 'B' and 'R' type displays
void ST7735::commonInit(const uint8_t *cmdList) {
    wait_ms(100);   // Wait for reset to be stable after power-up
    colstart = rowstart = 0; // May be overridden in init func
    if (cmdList) commandList(cmdList);
}

// Initialization for ST7735B screens
void ST7735::initB() {
    commonInit(Bcmd);
}

// Initialization for ST7735R screens (green or red tabs)
void ST7735::initR(uint8_t options) {
    commonInit(Rcmd1);
    if (options == INITR_GREENTAB) {
        commandList(Rcmd2green);
        colstart = 2;
        rowstart = 1;
    }
    else {
        // colstart, rowstart left at default '0' values
        commandList(Rcmd2red);
    }
    commandList(Rcmd3);

    // if black, change MADCTL color filter
    if (options == INITR_BLACKTAB) {
        writecommand(ST7735_MADCTL);
        writedata(0xC0);
    }

    tabcolor = options;
}

void ST7735::setRotation(uint8_t m) {

    writecommand(ST7735_MADCTL);
    rotation = m % 4; // can't be higher than 3
    switch (rotation) {
    case 0:
        if (tabcolor == INITR_BLACKTAB) {
            writedata(MADCTL_MX | MADCTL_MY | MADCTL_RGB);
        }
        else {
            writedata(MADCTL_MX | MADCTL_MY | MADCTL_BGR);
        }
        _width = ST7735_TFTWIDTH;
        _height = ST7735_TFTHEIGHT;
        break;
    case 1:
        if (tabcolor == INITR_BLACKTAB) {
            writedata(MADCTL_MY | MADCTL_MV | MADCTL_RGB);
        }
        else {
            writedata(MADCTL_MY | MADCTL_MV | MADCTL_BGR);
        }
        _width = ST7735_TFTHEIGHT;
        _height = ST7735_TFTWIDTH;
        break;
    case 2:
        if (tabcolor == INITR_BLACKTAB) {
            writedata(MADCTL_RGB);
        }
        else {
            writedata(MADCTL_BGR);
        }
        _width = ST7735_TFTWIDTH;
        _height = ST7735_TFTHEIGHT;
        break;
    case 3:
        if (tabcolor == INITR_BLACKTAB) {
            writedata(MADCTL_MX | MADCTL_MV | MADCTL_RGB);
        }
        else {
            writedata(MADCTL_MX | MADCTL_MV | MADCTL_BGR);
        }
        _width = ST7735_TFTHEIGHT;
        _height = ST7735_TFTWIDTH;
        break;
    }
}

void ST7735::fillScreen(uint16_t color) {
    fillRect(0, 0, _width, _height, color);
}



void ST7735::pushColor(uint16_t color) {
    dc->write(1);
    ce->write(0);
    spi->write(color >> 8);
    spi->write(color);
    ce->write(1);
}

// fill a rectangle
void ST7735::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {

    // rudimentary clipping (drawChar w/big text requires this)
    if ((x >= _width) || (y >= _height)) return;
    if ((x + w - 1) >= _width)  w = _width - x;
    if ((y + h - 1) >= _height) h = _height - y;

    setAddrWindow(x, y, x + w - 1, y + h - 1);

    uint8_t hi = color >> 8, lo = color;

    dc->write(1);
    ce->write(0);

    for (y = h; y > 0; y--) {
        for (x = w; x > 0; x--) {
            spi->write(hi);
            spi->write(lo);
        }
    }
    ce->write(1);
}



void ST7735::invertDisplay(bool i) {
    writecommand(i ? ST7735_INVON : ST7735_INVOFF);
}

void ST7735::drawPixel(int16_t x, int16_t y, uint16_t color) {

    if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height)) return;

    setAddrWindow(x, y, x + 1, y + 1);
    dc->write(1);
    ce->write(0);
    spi->write(color >> 8);
    spi->write(color);
    ce->write(1);
}

void ST7735::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {

    // Rudimentary clipping
    if ((x >= _width) || (y >= _height)) return;
    if ((y + h - 1) >= _height) h = _height - y;
    setAddrWindow(x, y, x, y + h - 1);

    uint8_t hi = color >> 8, lo = color;

    dc->write(1);
    ce->write(0);
    while (h--) {
        spi->write(hi);
        spi->write(lo);
    }
    ce->write(1);
}

void ST7735::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {

    // Rudimentary clipping
    if ((x >= _width) || (y >= _height)) return;
    if ((x + w - 1) >= _width)  w = _width - x;
    setAddrWindow(x, y, x + w - 1, y);
    uint8_t hi = color >> 8, lo = color;

    dc->write(1);;
    ce->write(0);;

    while (w--) {
        spi->write(hi);
        spi->write(lo);
    }

    ce->write(1);
}

// Pass 8-bit (each) R,G,B, get back 16-bit packed color
uint16_t ST7735::Color565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

// Bresenham's algorithm - thx wikpedia
void ST7735::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        swap(x0, y0);
        swap(x1, y1);
    }

    if (x0 > x1) {
        swap(x0, x1);
        swap(y0, y1);
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1) {
        ystep = 1;
    }
    else {
        ystep = -1;
    }

    for (; x0 <= x1; x0++) {
        if (steep) {
            drawPixel(y0, x0, color);
        }
        else {
            drawPixel(x0, y0, color);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}

// Draw a rectangle
void ST7735::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    drawFastHLine(x, y, w, color);
    drawFastHLine(x, y + h - 1, w, color);
    drawFastVLine(x, y, h, color);
    drawFastVLine(x + w - 1, y, h, color);
}

// Draw a circle outline
void ST7735::drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    drawPixel(x0, y0 + r, color);
    drawPixel(x0, y0 - r, color);
    drawPixel(x0 + r, y0, color);
    drawPixel(x0 - r, y0, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        drawPixel(x0 + x, y0 + y, color);
        drawPixel(x0 - x, y0 + y, color);
        drawPixel(x0 + x, y0 - y, color);
        drawPixel(x0 - x, y0 - y, color);
        drawPixel(x0 + y, y0 + x, color);
        drawPixel(x0 - y, y0 + x, color);
        drawPixel(x0 + y, y0 - x, color);
        drawPixel(x0 - y, y0 - x, color);
    }
}

void ST7735::drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        if (cornername & 0x4) {
            drawPixel(x0 + x, y0 + y, color);
            drawPixel(x0 + y, y0 + x, color);
        }
        if (cornername & 0x2) {
            drawPixel(x0 + x, y0 - y, color);
            drawPixel(x0 + y, y0 - x, color);
        }
        if (cornername & 0x8) {
            drawPixel(x0 - y, y0 + x, color);
            drawPixel(x0 - x, y0 + y, color);
        }
        if (cornername & 0x1) {
            drawPixel(x0 - y, y0 - x, color);
            drawPixel(x0 - x, y0 - y, color);
        }
    }
}

void ST7735::fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    drawFastVLine(x0, y0 - r, 2 * r + 1, color);
    fillCircleHelper(x0, y0, r, 3, 0, color);
}

// Used to do circles and roundrects
void ST7735::fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color) {

    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        if (cornername & 0x1) {
            drawFastVLine(x0 + x, y0 - y, 2 * y + 1 + delta, color);
            drawFastVLine(x0 + y, y0 - x, 2 * x + 1 + delta, color);
        }
        if (cornername & 0x2) {
            drawFastVLine(x0 - x, y0 - y, 2 * y + 1 + delta, color);
            drawFastVLine(x0 - y, y0 - x, 2 * x + 1 + delta, color);
        }
    }
}


// Draw a triangle
void ST7735::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
    drawLine(x0, y0, x1, y1, color);
    drawLine(x1, y1, x2, y2, color);
    drawLine(x2, y2, x0, y0, color);
}

// Fill a triangle
void ST7735::fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {

    int16_t a, b, y, last;

    // Sort coordinates by Y order (y2 >= y1 >= y0)
    if (y0 > y1) {
        swap(y0, y1); swap(x0, x1);
    }
    if (y1 > y2) {
        swap(y2, y1); swap(x2, x1);
    }
    if (y0 > y1) {
        swap(y0, y1); swap(x0, x1);
    }

    if (y0 == y2) { // Handle awkward all-on-same-line case as its own thing
        a = b = x0;
        if (x1 < a)      a = x1;
        else if (x1 > b) b = x1;
        if (x2 < a)      a = x2;
        else if (x2 > b) b = x2;
        drawFastHLine(a, y0, b - a + 1, color);
        return;
    }

    int16_t
        dx01 = x1 - x0,
        dy01 = y1 - y0,
        dx02 = x2 - x0,
        dy02 = y2 - y0,
        dx12 = x2 - x1,
        dy12 = y2 - y1,
        sa = 0,
        sb = 0;

    // For upper part of triangle, find scanline crossings for segments
    // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
    // is included here (and second loop will be skipped, avoiding a /0
    // error there), otherwise scanline y1 is skipped here and handled
    // in the second loop...which also avoids a /0 error here if y0=y1
    // (flat-topped triangle).
    if (y1 == y2) last = y1;   // Include y1 scanline
    else         last = y1 - 1; // Skip it

    for (y = y0; y <= last; y++) {
        a = x0 + sa / dy01;
        b = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;
        /* longhand:
        a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        */
        if (a > b) swap(a, b);
        drawFastHLine(a, y, b - a + 1, color);
    }

    // For lower part of triangle, find scanline crossings for segments
    // 0-2 and 1-2.  This loop is skipped if y1=y2.
    sa = dx12 * (y - y1);
    sb = dx02 * (y - y0);
    for (; y <= y2; y++) {
        a = x1 + sa / dy12;
        b = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;
        /* longhand:
        a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        */
        if (a > b) swap(a, b);
        drawFastHLine(a, y, b - a + 1, color);
    }
}

// Draw a rounded rectangle
void ST7735::drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
    // smarter version
    drawFastHLine(x + r, y, w - 2 * r, color); // Top
    drawFastHLine(x + r, y + h - 1, w - 2 * r, color); // Bottom
    drawFastVLine(x, y + r, h - 2 * r, color); // Left
    drawFastVLine(x + w - 1, y + r, h - 2 * r, color); // Right
    // draw four corners
    drawCircleHelper(x + r, y + r, r, 1, color);
    drawCircleHelper(x + w - r - 1, y + r, r, 2, color);
    drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color);
    drawCircleHelper(x + r, y + h - r - 1, r, 8, color);
}

// Fill a rounded rectangle
void ST7735::fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
    // smarter version
    fillRect(x + r, y, w - 2 * r, h, color);

    // draw four corners
    fillCircleHelper(x + w - r - 1, y + r, r, 1, h - 2 * r - 1, color);
    fillCircleHelper(x + r, y + r, r, 2, h - 2 * r - 1, color);
}

void ST7735::setCursor(int16_t x, int16_t y) {
    cursor_x = x;
    cursor_y = y;
}

void ST7735::setTextSize(uint8_t s) {
    textsize = (s > 0) ? s : 1;
}


void ST7735::setTextColor(uint16_t c) {
    // For 'transparent' background, we'll set the bg 
    // to the same as fg instead of using a flag
    textcolor = textbgcolor = c;
}

void ST7735::setTextColor(uint16_t c, uint16_t b) {
    textcolor = c;
    textbgcolor = b;
}

void ST7735::setTextWrap(bool w) {
    wrap = w;
}

uint8_t ST7735::getRotation(void) {
    return rotation;
}

void ST7735::drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {

    int16_t i, j, byteWidth = (w + 7) / 8;

    for (j = 0; j < h; j++) {
        for (i = 0; i < w; i++) {
            if (pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
                drawPixel(x + i, y + j, color);
            }
        }
    }
}


/***************************************************************************************
** Function name:           drawUnicode
** Descriptions:            draw a unicode
***************************************************************************************/
int ST7735::drawUnicode(unsigned int uniCode, int x, int y, int font)
{

    if (font) uniCode -= 32;

    unsigned int width = 0;
    unsigned int height = 0;
    unsigned int flash_address = 0;
    int8_t gap = 0;

    //   if (size == 1) {
    //     flash_address = pgm_read_word(&chrtbl_f8[uniCode]);
    //     width = pgm_read_byte(widtbl_f8+uniCode);
    //     height = chr_hgt_f8;
    //     gap = 1;
    //   }
#ifdef LOAD_FONT2
    if (font == 2) {
        flash_address = pgm_read_word(&chrtbl_f16[uniCode]);
        width = pgm_read_byte(widtbl_f16 + uniCode);
        height = chr_hgt_f16;
        gap = 1;
    }
#endif
    //   if (font == 3) {
    //     flash_address = pgm_read_word(&chrtbl_f24[uniCode]);
    //     width = pgm_read_byte(widtbl_f24+uniCode);
    //     height = chr_hgt_f24;
    //     gap = 0;
    //   }
#ifdef LOAD_FONT4
    if (font == 4) {
        flash_address = pgm_read_word(&chrtbl_f32[uniCode]);
        width = pgm_read_byte(widtbl_f32 + uniCode);
        height = chr_hgt_f32;
        gap = -3;
    }
#endif
    //   if (font == 5) {
    //     flash_address = pgm_read_word(&chrtbl_f48[uniCode]);
    //     width = pgm_read_byte(widtbl_f48+uniCode);
    //     height = chr_hgt_f48;
    //     gap = -3;
    //   }
#ifdef LOAD_FONT6
    if (font == 6) {
        flash_address = pgm_read_word(&chrtbl_f64[uniCode]);
        width = pgm_read_byte(widtbl_f64 + uniCode);
        height = chr_hgt_f64;
        gap = -3;
    }
#endif
#ifdef LOAD_FONT7
    if (font == 7) {
        flash_address = pgm_read_word(&chrtbl_f7s[uniCode]);
        width = pgm_read_byte(widtbl_f7s + uniCode);
        height = chr_hgt_f7s;
        gap = 2;
    }
#endif
#ifdef LOAD_FONT8
    if (font == 8) {
        flash_address = pgm_read_word(&chrtbl_f72[uniCode]);
        width = pgm_read_byte(widtbl_f72 + uniCode);
        height = chr_hgt_f72;
        gap = 2;
    }
#endif


    int w = (width + 7) / 8;
    int pX = 0;
    int pY = y;
    //int color   = 0;
    char line = 0;

    //fillRect(x,pY,width+gap,height,textbgcolor);

    for (int i = 0; i < height; i++)
    {
        if (textcolor != textbgcolor) {
            if (textsize == 1) drawFastHLine(x, pY, width + gap, textbgcolor);
            else fillRect(x, pY, (width + gap)*textsize, textsize, textbgcolor);
        }
        for (int k = 0; k < w; k++)
        {
            line = pgm_read_byte(flash_address + w * i + k);
            if (line) {
                if (textsize == 1) {
                    pX = x + k * 8;
                    if (line & 0x80) drawPixel(pX, pY, textcolor);
                    if (line & 0x40) drawPixel(pX + 1, pY, textcolor);
                    if (line & 0x20) drawPixel(pX + 2, pY, textcolor);
                    if (line & 0x10) drawPixel(pX + 3, pY, textcolor);
                    if (line & 0x8) drawPixel(pX + 4, pY, textcolor);
                    if (line & 0x4) drawPixel(pX + 5, pY, textcolor);
                    if (line & 0x2) drawPixel(pX + 6, pY, textcolor);
                    if (line & 0x1) drawPixel(pX + 7, pY, textcolor);
                }
                else {
                    pX = x + k * 8 * textsize;
                    if (line & 0x80) fillRect(pX, pY, textsize, textsize, textcolor);
                    if (line & 0x40) fillRect(pX + textsize, pY, textsize, textsize, textcolor);
                    if (line & 0x20) fillRect(pX + 2 * textsize, pY, textsize, textsize, textcolor);
                    if (line & 0x10) fillRect(pX + 3 * textsize, pY, textsize, textsize, textcolor);
                    if (line & 0x8) fillRect(pX + 4 * textsize, pY, textsize, textsize, textcolor);
                    if (line & 0x4) fillRect(pX + 5 * textsize, pY, textsize, textsize, textcolor);
                    if (line & 0x2) fillRect(pX + 6 * textsize, pY, textsize, textsize, textcolor);
                    if (line & 0x1) fillRect(pX + 7 * textsize, pY, textsize, textsize, textcolor);
                }
            }
        }
        pY += textsize;
    }
    return (width + gap)*textsize;        // x +
}

/***************************************************************************************
** Function name:           drawNumber unsigned with size
** Descriptions:            drawNumber
***************************************************************************************/
int ST7735::drawNumber(long long_num, int poX, int poY, int font)
{
    char tmp[10];
    if (long_num < 0) sprintf(tmp, "%li", long_num);
    else sprintf(tmp, "%lu", long_num);
    return drawString(tmp, poX, poY, font);
}

/***************************************************************************************
** Function name:           drawChar
** Descriptions:            draw char
***************************************************************************************/
int ST7735::drawChar(char c, int x, int y, int font)
{
    return drawUnicode(c, x, y, font);
}

/***************************************************************************************
** Function name:           drawString
** Descriptions:            draw string
***************************************************************************************/
int ST7735::drawString(char *string, int poX, int poY, int font)
{
    int sumX = 0;

    while (*string)
    {
        int xPlus = drawChar(*string, poX, poY, font);
        sumX += xPlus;
        *string++;
        poX += xPlus;                            /* Move cursor right       */
    }
    return sumX;
}

/***************************************************************************************
** Function name:           drawCentreString
** Descriptions:            draw string across centre
***************************************************************************************/
int ST7735::drawCentreString(char *string, int dX, int poY, int font)
{
    int sumX = 0;
    int len = 0;
    char *pointer = string;
    char ascii;

    while (*pointer)
    {
        ascii = *pointer;
        //if (font==0)len += 1+pgm_read_byte(widtbl_log+ascii);
        //if (font==1)len += 1+pgm_read_byte(widtbl_f8+ascii-32);
#ifdef LOAD_FONT2
        if (font == 2)len += 1 + pgm_read_byte(widtbl_f16 + ascii - 32);
#endif
        //if (font==3)len += 1+pgm_read_byte(widtbl_f48+ascii-32)/2;
#ifdef LOAD_FONT4
        if (font == 4)len += pgm_read_byte(widtbl_f32 + ascii - 32) - 3;
#endif
        //if (font==5) len += pgm_read_byte(widtbl_f48+ascii-32)-3;
#ifdef LOAD_FONT6
        if (font == 6) len += pgm_read_byte(widtbl_f64 + ascii - 32) - 3;
#endif
#ifdef LOAD_FONT7
        if (font == 7) len += pgm_read_byte(widtbl_f7s + ascii - 32) + 2;
#endif
#ifdef LOAD_FONT8
        if (font == 8) len += pgm_read_byte(widtbl_f72 + ascii - 32) + 2;
#endif
        *pointer++;
    }
    len = len * textsize;
    int poX = dX - len / 2;

    if (poX < 0) poX = 0;

    while (*string)
    {

        int xPlus = drawChar(*string, poX, poY, font);
        sumX += xPlus;
        *string++;
        poX += xPlus;                  /* Move cursor right            */
    }

    return sumX;
}

/***************************************************************************************
** Function name:           drawRightString
** Descriptions:            draw string right justified
***************************************************************************************/
int ST7735::drawRightString(char *string, int dX, int poY, int font)
{
    int sumX = 0;
    int len = 0;
    char *pointer = string;
    char ascii;

    while (*pointer)
    {
        ascii = *pointer;
        //if (font==0)len += 1+pgm_read_byte(widtbl_log+ascii);
        //if (font==1)len += 1+pgm_read_byte(widtbl_f8+ascii-32);
#ifdef LOAD_FONT2
        if (font == 2)len += 1 + pgm_read_byte(widtbl_f16 + ascii - 32);
#endif
        //if (font==3)len += 1+pgm_read_byte(widtbl_f48+ascii-32)/2;
#ifdef LOAD_FONT4
        if (font == 4)len += pgm_read_byte(widtbl_f32 + ascii - 32) - 3;
#endif
        //if (font==5) len += pgm_read_byte(widtbl_f48+ascii-32)-3;
#ifdef LOAD_FONT6
        if (font == 6) len += pgm_read_byte(widtbl_f64 + ascii - 32) - 3;
#endif
#ifdef LOAD_FONT7
        if (font == 7) len += pgm_read_byte(widtbl_f7s + ascii - 32) + 2;
#endif
#ifdef LOAD_FONT8
        if (font == 8) len += pgm_read_byte(widtbl_f72 + ascii - 32) + 2;
#endif
        *pointer++;
    }

    len = len * textsize;
    int poX = dX - len;

    if (poX < 0) poX = 0;

    while (*string)
    {

        int xPlus = drawChar(*string, poX, poY, font);
        sumX += xPlus;
        *string++;
        poX += xPlus;          /* Move cursor right            */
    }

    return sumX;
}

/***************************************************************************************
** Function name:           drawFloat
** Descriptions:            drawFloat
***************************************************************************************/
int ST7735::drawFloat(float floatNumber, int decimal, int poX, int poY, int font)
{
    unsigned long temp = 0;
    float decy = 0.0;
    float rounding = 0.5;

    float eep = 0.000001;

    int sumX = 0;
    int xPlus = 0;

    if (floatNumber - 0.0 < eep)       // floatNumber < 0
    {
        xPlus = drawChar('-', poX, poY, font);
        floatNumber = -floatNumber;

        poX += xPlus;
        sumX += xPlus;
    }

    for (unsigned char i = 0; i < decimal; ++i)
    {
        rounding /= 10.0;
    }

    floatNumber += rounding;

    temp = (long)floatNumber;


    xPlus = drawNumber(temp, poX, poY, font);

    poX += xPlus;
    sumX += xPlus;

    if (decimal > 0)
    {
        xPlus = drawChar('.', poX, poY, font);
        poX += xPlus;                            /* Move cursor right            */
        sumX += xPlus;
    }
    else
    {
        return sumX;
    }

    decy = floatNumber - temp;
    for (unsigned char i = 0; i < decimal; i++)
    {
        decy *= 10;                                /* for the next decimal         */
        temp = decy;                               /* get the decimal              */
        xPlus = drawNumber(temp, poX, poY, font);

        poX += xPlus;                              /* Move cursor right            */
        sumX += xPlus;
        decy -= temp;
    }
    return sumX;
}

void ST7735::setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {

    writecommand(ST7735_CASET); // Column addr set
    writedata(0x00);
    writedata(x0 + colstart);     // XSTART 
    writedata(0x00);
    writedata(x1 + colstart);     // XEND

    writecommand(ST7735_RASET); // Row addr set
    writedata(0x00);
    writedata(y0 + rowstart);     // YSTART
    writedata(0x00);
    writedata(y1 + rowstart);     // YEND

    writecommand(ST7735_RAMWR); // write to RAM
}


