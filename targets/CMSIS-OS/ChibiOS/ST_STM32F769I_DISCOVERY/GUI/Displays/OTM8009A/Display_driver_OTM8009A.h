
#ifndef _DRIVERS_DISPLAY_OTM8009A_H_
#define _DRIVERS_DISPLAY_OTM8009A_H_ 1

#include "nanohal.h"
#include "nanoCLR_Types.h"
#include "cmsis_os.h"
#include "ch.h"
#include "hal.h"
#include "Display_Driver.h"

// LCD Size Defines
#define LCD_HW_HEIGHT 480  // Portrait height
#define LCD_HW_WIDTH  800  // Portrait width

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"

#define LCD_ORIENTATION	   		  LANDSCAPE
#define SCREEN_SIZE_LONGER_SIDE   800        // The maximum resolution of longer longer side of physical LCD
#define SCREEN_SIZE_SHORTER_SIDE  480       // The maximum resolution of  shorter longer side of physical LCD

typedef enum
{
    PORTRAIT,
    PORTRAIT180,
    LANDSCAPE,
    LANDSCAPE180
} SCREEN_ORIENTATION;
struct Display_Driver_OTM8009A
{
    CLR_UINT32 m_cursor;
    SCREEN_ORIENTATION lcd_orientation;

    //static CLR_UINT16 VIDEO_RAM[(DISP_YMAX* DISP_XMAX )] ;
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
enum PixelFormat : CLR_UINT8 {
    OTM8009A_FORMAT_RGB888 = 0,         // Pixel format chosen is RGB888 : 24 bpp
    OTM8009A_FORMAT_RBG565 = 2          // Pixel format chosen is RGB565 : 16 bpp
};
enum TimingParameters : CLR_UINT8 {
    OTM8009A_480X800_HSYNC = 63,                      // Horizontal synchronization: This value is set to limit value mentioned n otm8009a spec to fit with USB functional clock configuration constraints
    OTM8009A_480X800_HBP = 120,                       // Horizontal back porch
    OTM8009A_480X800_HFP = 120,                       // Horizontal front porch
    OTM8009A_480X800_VSYNC = 12,                      // Vertical synchronization
    OTM8009A_480X800_VBP = 12,                        // Vertical back porch
    OTM8009A_480X800_VFP = 12,                        // Vertical front porch
    OTM8009A_800X480_HSYNC = OTM8009A_480X800_HSYNC,  // Horizontal synchronization
    OTM8009A_800X480_HBP = OTM8009A_480X800_HBP,      // Horizontal back porch
    OTM8009A_800X480_HFP = OTM8009A_480X800_HFP,      // Horizontal front porch
    OTM8009A_800X480_VSYNC = OTM8009A_480X800_VSYNC,  // Vertical synchronization
    OTM8009A_800X480_VBP = OTM8009A_480X800_VBP,      // Vertical back porch
    OTM8009A_800X480_VFP = OTM8009A_480X800_VFP       // Vertical front porch
};
enum OTM8009A_Commands : CLR_UINT8 {
    OTM8009A_CMD_NOP = 0x00,        // NOP command
    OTM8009A_CMD_SWRESET = 0x01,    // Sw reset command
    OTM8009A_CMD_RDDMADCTL = 0x0B,  // Read Display MADCTR command : read memory display access ctrl
    OTM8009A_CMD_RDDCOLMOD = 0x0C,  // Read Display pixel format
    OTM8009A_CMD_SLPIN = 0x10,      // Sleep In command
    OTM8009A_CMD_SLPOUT = 0x11,     // Sleep Out command
    OTM8009A_CMD_PTLON = 0x12,      // Partial mode On command
    OTM8009A_CMD_DISPOFF = 0x28,    // Display Off command
    OTM8009A_CMD_DISPON = 0x29,     // Display On command
    OTM8009A_CMD_CASET = 0x2A,      // Column address set command
    OTM8009A_CMD_PASET = 0x2B,      // Page address set command
    OTM8009A_CMD_RAMWR = 0x2C,      // Memory (GRAM) write command
    OTM8009A_CMD_RAMRD = 0x2E,      // Memory (GRAM) read command
    OTM8009A_CMD_PLTAR = 0x30,      // Partial area command (4 parameters)
    OTM8009A_CMD_TEOFF = 0x34,      // Tearing Effect Line Off command : command with no parameter
    OTM8009A_CMD_TEEON = 0x35,      // Tearing Effect Line On command : command with 1 parameter 'TELOM'
    OTM8009A_CMD_MADCTR = 0x36,             // Memory Access write control command
    OTM8009A_CMD_IDMOFF = 0x38,     // Idle mode Off command
    OTM8009A_CMD_IDMON = 0x39,      // Idle mode On command
    OTM8009A_CMD_COLMOD = 0x3A,      // Interface Pixel format command
    OTM8009A_CMD_RAMWRC = 0x3C,             // Memory write continue command
    OTM8009A_CMD_RAMRDC = 0x3E,             // Memory read continue command
    OTM8009A_CMD_WRTESCN = 0x44,            // Write Tearing Effect Scan line command
    OTM8009A_CMD_RDSCNL = 0x45,              // Read  Tearing Effect Scan line command
    OTM8009A_CMD_WRDISBV = 0x51,        // Write Display Brightness command
    OTM8009A_CMD_WRCTRLD = 0x53,        // Write CTRL Display command
    OTM8009A_CMD_WRCABC = 0x55,         // Write Content Adaptive Brightness command
    OTM8009A_CMD_WRCABCMB = 0x5E        // Write CABC Minimum Brightness command

};
enum OTM8009A_TELOM : CLR_UINT8 {
    OTM8009A_TEEON_TELOM_VBLANKING_INFO_ONLY = 0x00,
    OTM8009A_TEEON_TELOM_VBLANKING_AND_HBLANKING_INFO = 0x01
};
enum OTM8009A_MADCTR : CLR_UINT8 {          // Possible used values of MADCTR
    OTM8009A_MADCTR_MODE_PORTRAIT = 0x00,
    OTM8009A_MADCTR_MODE_LANDSCAPE = 0x60   // MY = 0, MX = 1, MV = 1, ML = 0, RGB = 0
};
enum OTM8009A_MADCTR_COLMOD : CLR_UINT8 {   // Possible values of COLMOD parameter corresponding to used pixel formats        
    OTM8009A_COLMOD_RGB565 = 0x55,
    OTM8009A_COLMOD_RGB888 = 0x77,
};
enum OTM8009A_FREQUENCY_DIVIDER : CLR_UINT8 {
    OTM8009A_480X800_FREQUENCY_DIVIDER = 2      // LCD Frequency divider
};
enum OTM8009A_COLOUR : CLR_UINT32 {         // ARGB8888 format
    LCD_COLOR_BLUE = 0xFF0000FF,
    LCD_COLOR_GREEN = 0xFF00FF00,
    LCD_COLOR_RED = 0xFFFF0000,
    LCD_COLOR_CYAN = 0xFF00FFFF,
    LCD_COLOR_MAGENTA = 0xFFFF00FF,
    LCD_COLOR_YELLOW = 0xFFFFFF00,
    LCD_COLOR_LIGHTBLUE = 0xFF8080FF,
    LCD_COLOR_LIGHTGREEN = 0xFF80FF80,
    LCD_COLOR_LIGHTRED = 0xFFFF8080,
    LCD_COLOR_LIGHTCYAN = 0xFF80FFFF,
    LCD_COLOR_LIGHTMAGENTA = 0xFFFF80FF,
    LCD_COLOR_LIGHTYELLOW = 0xFFFFFF80,
    LCD_COLOR_DARKBLUE = 0xFF000080,
    LCD_COLOR_DARKGREEN = 0xFF008000,
    LCD_COLOR_DARKRED = 0xFF800000,
    LCD_COLOR_DARKCYAN = 0xFF008080,
    LCD_COLOR_DARKMAGENTA = 0xFF800080,
    LCD_COLOR_DARKYELLOW = 0xFF808000,
    LCD_COLOR_WHITE = 0xFFFFFFFF,
    LCD_COLOR_LIGHTGRAY = 0xFFD3D3D3,
    LCD_COLOR_GRAY = 0xFF808080,
    LCD_COLOR_DARKGRAY = 0xFF404040,
    LCD_COLOR_BLACK = 0xFF000000,
    LCD_COLOR_BROWN = 0xFFA52A2A,
    LCD_COLOR_ORANGE = 0xFFFFA500,
    LCD_COLOR_TRANSPARENT = 0xFF000000
};

extern Display_Driver_OTM8009A g_OTM8009A_Display_Driver;
extern DISPLAY_CONTROLLER_CONFIG g_OTM8009A_Config;

#pragma GCC diagnostic pop
#endif  // _DRIVERS_DISPLAY_OTM8009A_H_




