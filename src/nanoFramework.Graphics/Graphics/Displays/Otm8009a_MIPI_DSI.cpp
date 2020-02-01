
//
// Copyright (c) 2019 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "Graphics.h"
#include "DisplayInterface.h"
#include "Display.h"


//
// The OTM8009A, a 16, 777, 216 - color System - on - Chip(SoC) driver LSI designed for smalland medium sizes of
// TFT LCD display, is capable of supporting up to 480xRGBx864(WVGA) in resolution which can be achieved
// by the integrated RAM for graphic data.The 480 - channel source driver has true 8 - bit resolution,
// generates 256 Gamma - corrected values by an internal D / A converter.
// The OTM8009A is able to operate with low IO interface power supply and incorporate with several charge pumps
// to generate various voltage levels that form an on - chip power management system for gate driverand source driver.


#define CommandData(c)   c,(CLR_UINT8 *)(CLR_UINT8[c])  // Macro to simplify visualisation of passing pointer to parameters;

struct DisplayDriver g_DisplayDriver;
extern DisplayInterface g_DisplayInterface;

enum Otm8009A_Orientation : CLR_UINT8
{
};
enum Otm8009A_Command : CLR_UINT8  // One byte commands
{
    NOP = 0x00,         // NOP command
    SWRESET = 0x01,     // Software reset command  - wait 5ms after command to begin programming
    RDNUMED = 0x05,      // Read # of Errors on DSI 
    RDDPM = 0x0A,      // Read Display Power Mode 
    RDDMADCTL = 0x0B,   // Read Display MADCTR command : read memory display access ctrl
    RDDCOLMOD = 0x0C,   // Read Display pixel format
    RDDIM = 0x0D,      // Read Display Image Mode 
    RDDSM = 0x0E,      // Read Display Signal Mode 
    RDDSDR = 0x0F,      // Read Display Self Diagnostic Result 
    SLPIN = 0x10,       // Sleep In command (This command causes the LCD module to enter the minimum power consumption mode)
    SLPOUT = 0x11,      // Sleep Out command (This command turns off sleep mode)
    PTLON = 0x12,       // Partial mode On command
    NORON = 0x13,       // Normal Display On - (Normal display mode on means Partial mode off, Scroll mode Off)
    INVOFF = 0x20,      // Turn off display inversion
    INVON = 0x21,       // Turn on display inversion  (Every bit is inverted from the frame memory to the display.)
    ALLPOFF = 0x22,     // All pixels off
    ALLPON = 0x23,      // All pixels on
    GAMSET = 0x26,      // Gamma set (used to select the desired Gamma curve for the current display)
    DISPOFF = 0x28,     // Display Off command
    DISPON = 0x29,      // Display On command
    CASET = 0x2A,       // Column address set command (used to define area of frame memory where MCU can access)
    PASET = 0x2B,       // Page address set command (used to define area of frame memory where MCU can access)
    RAMWR = 0x2C,       // Memory (GRAM) write command
    RAMRD = 0x2E,       // Memory (GRAM) read command
    PLTAR = 0x30,       // Partial area command (4 parameters)
    TEOFF = 0x34,       // Tearing Effect Line Off command : command with no parameter
    TEEON = 0x35,       // Tearing Effect Line On command : command with 1 parameter 'TELOM'
    MADCTR = 0x36,      // Memory Access control  (defines read/ write scanning direction of frame memory)
    IDMOFF = 0x38,      // Idle mode Off command
    IDMON = 0x39,       // Idle mode On command  (Color expression is reduced)
    COLMOD = 0x3A,      // Interface Pixel format command  (12/16/18/24 bits per pixel)
    RAMWRC = 0x3C,      // Memory write continue command
    RAMRDC = 0x3E,      // Memory read continue command
    WRTESCN = 0x44,     // Write Tearing Effect Scan line command
    RDSCNL = 0x45,      // Read  Tearing Effect Scan line command
    WRDISBV = 0x51,     // CABC Management : ie : Content Adaptive Back light Control in IC OTM8009a
    RDDISBV = 0x52,      // Read Display Brightness 
    WRCTRLD = 0x53,     // Write CTRL Display command (This command is used to control ambient light, brightness and gamma settings)
    RDCTRLD = 0x54,      // Read CTRL Display 
    WRCABC = 0x55,      // Write Content Adaptive Brightness command (off/UI/Still picture/Moving Image)
    RDCABC = 0x56,      // Read Content Adaptive brightness control 
    WRCABCMB = 0x5E,    // Write CABC Minimum Brightness command
    RDCABCMB = 0x5F,   // Read CABC minimum brightness 
    RDABCSDR = 0x68,   // Read Auto-bright-control diag result 
    RDBWLB = 0x70,      // Read black/white low bits  (returns the lowest bits of black and white color characteristics)
    RDBKX = 0x71,      // Read Black "x" 
    RDBKY = 0x72,      // Read Black "y" 
    RDWX = 0x73,      // Read White "x" 
    RDWY = 0x74,      // Read White "y" 
    RDRGLB = 0x75,      // Read Red/Green low bits  (returns the lowest bits of red and green color characteristics.)
    RDRX = 0x76,      // Read RED x 
    RDRY = 0x77,      // Read RED y 
    RDGX = 0x78,      // Read GREEN x 
    RDGY = 0x79,      // Read GREEN y 
    RDBALB = 0x7A,      // Read Blue/Alpha low bits (returns the lowest bits of blue and A color characteristics)
    RDBX = 0x7B,      // Read BLUE x 
    RDBY = 0x7C,      // Read BLUE y 
    RDAX = 0x7D,      // Read Alpha x 
    RDAY = 0x7E,      // Read Alpha y 
    RDDDBS = 0xA1,      // Read DDB Start      ( Supplier ID/Display Module ID)
    RDDDBC = 0xA8,      // Read DDB continue   ( Revision information)
    RDFCS = 0xAA,      // Read First Checksum 
    RDCCS = 0xAF,      // Read Continue checksum 
    RDID1 = 0xDA,      // Read ID1 
    RDID2 = 0xDB,      // Read ID2 
    RDID3 = 0xDC,      // Read ID3 
};
//Customer Command List and Description (Manufacturer Command Set / Command 2)
enum Otm8009A_Command2 :CLR_UINT16 // Two byte commands - (only available in MIPI Low Power (LPDT) mode)
{
    // Command

    // Timing parameters for an 800 x 480 screen
    // Vertical timings are in terms of "hsyncs" and
    // Horizontal timings are in terms of the Pixel clock
    // Typical 26.37Mhz pixel clock rate (24Mhz, 30.74Mhz) (min, max) 

    DISP_HAAREA = 0x01E0,       // 480 - Horizontal Active area 
    DISP_HBLNKP = 0x002A,       // 42 - Horizontal Blanking Period 
    DISP_HBP = 0x0014,          // 20 - Horizontal Back Porch 
    DISP_HCP = 0x020A,          // 522 - Horizontal Cycle period 
    DISP_HFP = 0x0020,          // 20 - Horizontal Front Porch 
    DISP_HLPW = 0x0002,         // 2 - Horizontal sync pulse width 
    DISP_VAAREA = 0x0320,       // 800 - Vertical Active area 
    DISP_VBLNKP = 0x0020,       // 32 - Vertical Blanking Period 
    DISP_VBP = 0x000F,          // 15 - Vertical Back Porch 
    DISP_VCP = 0x0340,          // 832 Vertical Cycle period 
    DISP_VFP = 0x000A,         // 10 - Vertical Front Porch 
    DISP_VLPW = 0x0001,         // 1 - Vertical sync pulse width 
    DISP_VRR = 0x003C,          // 60 - Vertical Refresh rate 

    // Read Commands
    EXTCCHK = 0xD300,         // Check ExtC 
    PRG_FLAH = 0xF101,         // OTP Program flag state 

    // Read write commands
    AIEEN = 0xD700,            // AIE Enable 
    AIESET = 0xC900,         // AIE Setting 
    CABCSET1 = 0xC700,         // CABC settting 
    CABCSET2 = 0xC800,         // CABC settting 
    CEEN = 0xD680,            // CE Enable 
    CESET1 = 0xD400,         // CE Correction settings 1 
    CESET2 = 0xD500,         // CE Correction settings 2 
    GAMB = 0xEE00,            // Gamma correction (Blue) 
    GAMG = 0xED00,            // Gamma correction (Green) 
    GAMR = 0xEC00,            // Gamma correction (Red) 
    GMCT10N = 0xE800,
    GMCT10P = 0xE700,         // Gamma Correction 1.0 
    GMCT18N = 0xE400,
    GMCT18P = 0xE300,         // Gamma correction (1.8) 
    GMCT22N = 0xE200,         // Negative Gamma correction (2.2) 
    GMCT22P = 0xE100,         // Positive Gamma correction (2.2) 
    GMCT25N = 0xE600,
    GMCT25P = 0xE500,         // Gamma correction (2.5) 
    GOACLKA1 = 0xCEA0,
    GOACLKA2 = 0xCEA7,
    GOACLKA3 = 0xCEB0,
    GOACLKA4 = 0xCEB7,
    GOACLKB1 = 0xCEC0,
    GOACLKB2 = 0xCEC7,
    GOACLKB3 = 0xCED0,
    GOACLKB4 = 0xCED7,
    GOACLKC1 = 0xCF80,
    GOACLKC2 = 0xCF87,
    GOACLKC3 = 0xCF90,
    GOACLKC4 = 0xCF97,
    GOACLKD1 = 0xCFA0,
    GOACLKD2 = 0xCFA7,
    GOACLKD3 = 0xCFB0,
    GOACLKD4 = 0xCFB7,
    GOAECLK = 0xCFC0,         // ECLK setting 
    GOAGPSET = 0xCE9C,         // GOA Group settting 
    GOAOPT1 = 0xCFC6,         // Other options 1 
    GOATGOPT = 0xCFC7,         // Signal toggle option 
    GOAVEND = 0xCE90,         // GOA VEND setting 
    GOAVST = 0xCE80,         // GOA VST setting 
    GVDDSET = 0xD800,         // Vdd setting 
    IF_PARA1 = 0xB280,         // IF Parameter 1 
    IF_PARA2 = 0xB282,         // IF Parameter 2 
    MIPISET1 = 0xB080,         // MIPI Setting 1 
    MIPISET2 = 0xB0A1,         // MIPI Setting 2 
    OSC_ADJ = 0xC181,         // Oscillator adjustment for idle/normal mode 
    OTPSEL = 0xA000,         // OTP Select region 
    P_DRV_M = 0xC0B4,         // Panel driving mode 
    PADPARA = 0xB390,         // IO pad parameter 
    PTSP1 = 0xC092,            // Panel timing setting parameter 1 
    PTSP2 = 0xC094,            // Panel timing setting parameter 2 
    PWMPARA1 = 0xC680,         // pwm parameter 1
    PWMPARA2 = 0xC6B0,         // pwm parameter 2 
    PWMPARA3 = 0xC6B1,         // pwm parameter 3 
    PWMPARA4 = 0xC6B2,         // pwm parameter 4 
    PWMPARA5 = 0xC6B3,         // pwm parameter 5 
    PWMPARA6 = 0xC6B4,         // pwm parameter 6 
    PWR_CTRL1 = 0xC580,         // Power control setting 1 
    PWR_CTRL2 = 0xC590,         // Power control setting 2 
    PWR_CTRL3 = 0xC5A0,         // Power control setting 3 
    PWR_CTRL4 = 0xC5B0,         // Power control setting 4 
    RAMPWRSET = 0xB3C0,         // RAM Power setting 
    RGB_VIDEO_SET = 0xC1A1,      // Set RGB / BRG mode 
    SD_CTRL = 0xC0A2,         // Source driver timing setting 
    SD_PCH_CTRL = 0xC480,      // Set source driver precharage 
    TSP1 = 0xC080,            // TCON power setting 
    VCOMDC = 0xD900,         // VCOM voltage setting 
    WRDDB = 0xD200,            // Write DDB setting 
    WRID1 = 0xD000,            // Write ID1 
    WRID2 = 0xD100,            // Write ID2 

    // Write Commands
    ADRSFT = 0x0000,         // Address Shift Function 
    CMD2ENA1 = 0xFF00,         // Enable multibyte commands 
    CMD2ENA2 = 0xFF80,         // Enable vendor (Orise) commands 
    NVMIN = 0xEB00,            // NV Memory write 

};


#define OTM8009A_COLMOD_RGB565             0x55
#define OTM8009A_COLMOD_RGB888             0x77
#define OTM8009A_MADCTR_MODE_PORTRAIT      0x00
#define OTM8009A_MADCTR_MODE_LANDSCAPE     0x60  /* MY = 0, MX = 1, MV = 1, ML = 0, RGB = 0 */

CLR_UINT8 lcdRegData1[] = { 0x80,0x09,0x01,0xFF };
CLR_UINT8 lcdRegData2[] = { 0x80,0x09,0xFF };
CLR_UINT8 lcdRegData3[] = { 0x00,0x09,0x0F,0x0E,0x07,0x10,0x0B,0x0A,0x04,0x07,0x0B,0x08,0x0F,0x10,0x0A,0x01,0xE1 };
CLR_UINT8 lcdRegData4[] = { 0x00,0x09,0x0F,0x0E,0x07,0x10,0x0B,0x0A,0x04,0x07,0x0B,0x08,0x0F,0x10,0x0A,0x01,0xE2 };
CLR_UINT8 lcdRegData5[] = { 0x79,0x79,0xD8 };
CLR_UINT8 lcdRegData6[] = { 0x00,0x01,0xB3 };
CLR_UINT8 lcdRegData7[] = { 0x85,0x01,0x00,0x84,0x01,0x00,0xCE };
CLR_UINT8 lcdRegData8[] = { 0x18,0x04,0x03,0x39,0x00,0x00,0x00,0x18,0x03,0x03,0x3A,0x00,0x00,0x00,0xCE };
CLR_UINT8 lcdRegData9[] = { 0x18,0x02,0x03,0x3B,0x00,0x00,0x00,0x18,0x01,0x03,0x3C,0x00,0x00,0x00,0xCE };
CLR_UINT8 lcdRegData10[] = { 0x01,0x01,0x20,0x20,0x00,0x00,0x01,0x02,0x00,0x00,0xCF };
CLR_UINT8 lcdRegData11[] = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xCB };
CLR_UINT8 lcdRegData12[] = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xCB };
CLR_UINT8 lcdRegData13[] = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xCB };
CLR_UINT8 lcdRegData14[] = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xCB };
CLR_UINT8 lcdRegData15[] = { 0x00,0x04,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xCB };
CLR_UINT8 lcdRegData16[] = { 0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0xCB };
CLR_UINT8 lcdRegData17[] = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xCB };
CLR_UINT8 lcdRegData18[] = { 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xCB };
CLR_UINT8 lcdRegData19[] = { 0x00,0x26,0x09,0x0B,0x01,0x25,0x00,0x00,0x00,0x00,0xCC };
CLR_UINT8 lcdRegData20[] = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x26,0x0A,0x0C,0x02,0xCC };
CLR_UINT8 lcdRegData21[] = { 0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xCC };
CLR_UINT8 lcdRegData22[] = { 0x00,0x25,0x0C,0x0A,0x02,0x26,0x00,0x00,0x00,0x00,0xCC };
CLR_UINT8 lcdRegData23[] = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x0B,0x09,0x01,0xCC };
CLR_UINT8 lcdRegData24[] = { 0x26,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xCC };
CLR_UINT8 lcdRegData25[] = { 0xFF,0xFF,0xFF,0xFF };

CLR_UINT8 lcdRegData27[] = { 0x00, 0x00, 0x03, 0x1F, Otm8009A_Command::CASET };
CLR_UINT8 lcdRegData28[] = { 0x00, 0x00, 0x01, 0xDF, Otm8009A_Command::PASET };

CLR_UINT8 ShortRegData1[] = { 0x00 };
CLR_UINT8 ShortRegData2[] = { 0x80 };
CLR_UINT8 ShortRegData3[] = { 0xC4, 0x30 };
CLR_UINT8 ShortRegData4[] = { 0x8A };
CLR_UINT8 ShortRegData5[] = { 0x40 };
CLR_UINT8 ShortRegData6[] = { 0xB1 };
CLR_UINT8 ShortRegData7[] = { 0xA9 };
CLR_UINT8 ShortRegData8[] = { 0x91 };
CLR_UINT8 ShortRegData9[] = { 0x34 };
CLR_UINT8 ShortRegData10[] = { 0xB4 };
CLR_UINT8 ShortRegData11[] = { 0x50 };
CLR_UINT8 ShortRegData12[] = { 0x4E };
CLR_UINT8 ShortRegData13[] = { 0x81 };
CLR_UINT8 ShortRegData14[] = { 0x66 };
CLR_UINT8 ShortRegData15[] = { 0xA1 };
CLR_UINT8 ShortRegData16[] = { 0x08 };
CLR_UINT8 ShortRegData17[] = { 0x92 };
CLR_UINT8 ShortRegData18[] = { 0x01 };
CLR_UINT8 ShortRegData19[] = { 0x95 };
CLR_UINT8 ShortRegData20[] = { 0x94 };
CLR_UINT8 ShortRegData21[] = { 0x33 };
CLR_UINT8 ShortRegData22[] = { 0xA3 };
CLR_UINT8 ShortRegData23[] = { 0x1B };
CLR_UINT8 ShortRegData24[] = { 0x82 };
CLR_UINT8 ShortRegData25[] = { 0x83 };
CLR_UINT8 ShortRegData26[] = { 0x83 };
CLR_UINT8 ShortRegData27[] = { 0x0E };
CLR_UINT8 ShortRegData28[] = { 0xA6 };
CLR_UINT8 ShortRegData29[] = { 0xA0 };
CLR_UINT8 ShortRegData30[] = { 0xB0 };
CLR_UINT8 ShortRegData31[] = { 0xC0 };
CLR_UINT8 ShortRegData32[] = { 0xD0 };
CLR_UINT8 ShortRegData33[] = { 0x90 };
CLR_UINT8 ShortRegData34[] = { 0xE0 };
CLR_UINT8 ShortRegData35[] = { 0xF0 };
CLR_UINT8 ShortRegData36[] = { Otm8009A_Command::SLPOUT, 0x00 };
CLR_UINT8 ShortRegData37[] = { Otm8009A_Command::COLMOD, OTM8009A_COLMOD_RGB565 };
CLR_UINT8 ShortRegData38[] = { Otm8009A_Command::COLMOD, OTM8009A_COLMOD_RGB888 };
CLR_UINT8 ShortRegData39[] = { OTM8009A_MADCTR_MODE_LANDSCAPE };
CLR_UINT8 ShortRegData40[] = { Otm8009A_Command::WRDISBV, 0x7F };
CLR_UINT8 ShortRegData41[] = { Otm8009A_Command::WRCTRLD, 0x2C };
CLR_UINT8 ShortRegData42[] = { Otm8009A_Command::WRCABC, 0x02 };
CLR_UINT8 ShortRegData43[] = { Otm8009A_Command::WRCABCMB, 0xFF };
CLR_UINT8 ShortRegData44[] = { Otm8009A_Command::DISPON, 0x00 };
CLR_UINT8 ShortRegData45[] = { Otm8009A_Command::RAMWR, 0x00 };
CLR_UINT8 ShortRegData46[] = { 0xCF, 0x00 };
CLR_UINT8 ShortRegData47[] = { 0x66 };
CLR_UINT8 ShortRegData48[] = { 0xB6 };
CLR_UINT8 ShortRegData49[] = { 0x06 };


bool DisplayDriver::Initialize()
{

    {
        /***********************OTM8009A Initialization********************************/

          /* Initialize the OTM8009A LCD Display IC Driver (KoD LCD IC Driver)
          *  depending on configuration set in 'hdsivideo_handle'.
          */
        //   OTM8009A_Init(OTM8009A_FORMAT_RBG565, orientation);

             /***********************End OTM8009A Initialization****************************/
    }




    // Initialize OTM8009A registers
    g_DisplayInterface.SendCommandByte(Otm8009A_Command::SWRESET);
    g_DisplayInterface.DisplayCommandDelay(10);

    // MIPI Enable access command 2 registers to access vendor specific commands
    //   - Write Register 0xFFh with parameter 0x80h + 0x09h + 0x01h
    //   - Write Register 0x00h with parameter 0x80h
    //   - Write Register 0xFFh with parameter 0x80h + 0x09h 
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData2, 1);
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command2::CMD2ENA1, lcdRegData1, 3);

    // Other  Enable access command 2 registers 
       //   Write Register 0xFF00h with parameter 0x0080h
       //   Write Register 0xFF01h with parameter 0x0009h
       //   Write Register 0xFF02h with parameter 0x0001h
       //  --
       //   Write Register 0xFF80h with parameter 0x0080h
       //   Write Register 0xFF81h with parameter 0x0009h
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData2, 1);
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command2::CMD2ENA2, lcdRegData2, 2);

    // SD_PCH_CTRL - 0xC480h - 129th parameter - Default 0x00          
    // Set SD_PT -> Source output level during porch and non-display area to GND 
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData2, 1);
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command2::SD_PCH_CTRL, ShortRegData3, 1);

    g_DisplayInterface.DisplayCommandDelay(10);

    // Not documented 
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData4, 1);
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)0xC4, ShortRegData5, 1);

    g_DisplayInterface.DisplayCommandDelay(10);

    // PWR_CTRL4 - 0xC4B0h - 178th parameter - Default 0xA8 
    // Set gvdd_en_test -> enable GVDD test mode !!!                         
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData6, 1);
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)0xC5, ShortRegData7, 1);

    // PWR_CTRL2 - 0xC590h - 146th parameter - Default 0x79      
    // Set pump 4 vgh voltage  -> from 15.0v down to 13.0v                               
    // Set pump 5 vgh voltage  -> from -12.0v downto -9.0v                               
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData8, 1);
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)0xC5, ShortRegData9, 1);

    // P_DRV_M - 0xC0B4h - 181th parameter - Default 0x00 
    // -> Column inversion                                
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData10, 1);
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)0xC0, ShortRegData11, 1);

    // VCOMDC - 0xD900h - 1st parameter - Default 0x39h 
    // VCOM Voltage settings  -> from -1.0000v downto -1.2625v                 
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData1, 1);
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)0xD9, ShortRegData12, 1);

    // Oscillator adjustment for Idle/Normal mode (LPDT only) set to 65Hz (default is 60Hz) 
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData13, 1);
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)0xC1, ShortRegData14, 1);

    // Video mode internal 
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData15, 1);
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)0xC1, ShortRegData16, 1);

    // PWR_CTRL2 - 0xC590h - 147h parameter - Default 0x00 
    // Set pump 4 & 5 x6   -> ONLY VALID when PUMP4_EN_ASDM_HV = "0"           
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData17, 1);
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)0xC5, ShortRegData18, 1);

    // PWR_CTRL2 - 0xC590h - 150th parameter - Default 0x33h 
    // Change pump4 clock ratio -> from 1 line to 1/2 line                            
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData19, 1);
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)0xC5, ShortRegData9, 1);

    // GVDD/NGVDD settings 
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData1, 1);
    g_DisplayInterface.SendData(lcdRegData5, 3);

    // PWR_CTRL2 - 0xC590h - 149th parameter - Default 0x33h 
    // Rewrite the default value !                           
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData20, 1);
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)0xC5, ShortRegData21, 1);

    // Panel display timing Setting 3 
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData22, 1);
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)0xC0, ShortRegData23, 1);

    // Power control 1 
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData24, 1);
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)0xC5, ShortRegData25, 1);

    // Source driver precharge 
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData13, 1);
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)0xC4, ShortRegData26, 1);

    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData15, 1);
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)0xC1, ShortRegData27, 1);

    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData28, 1);
    g_DisplayInterface.SendData(lcdRegData6, 3);

    // GOAVST 
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData2, 1);
    g_DisplayInterface.SendData(lcdRegData7, 7);

    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData29, 1);
    g_DisplayInterface.SendData(lcdRegData8, 15);

    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData30, 1);
    g_DisplayInterface.SendData(lcdRegData9, 15);

    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData31, 1);
    g_DisplayInterface.SendData(lcdRegData10, 11);

    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData32, 1);
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)0xCF, ShortRegData46, 1);

    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData2, 1);
    g_DisplayInterface.SendData(lcdRegData11, 11);

    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData33, 1);
    g_DisplayInterface.SendData(lcdRegData12, 16);

    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData29, 1);
    g_DisplayInterface.SendData(lcdRegData13, 16);

    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData30, 1);
    g_DisplayInterface.SendData(lcdRegData14, 11);

    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData31, 1);
    g_DisplayInterface.SendData(lcdRegData15, 16);

    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData32, 1);
    g_DisplayInterface.SendData(lcdRegData16, 16);

    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData34, 1);
    g_DisplayInterface.SendData(lcdRegData17, 11);

    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData35, 1);
    g_DisplayInterface.SendData(lcdRegData18, 11);

    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData2, 1);
    g_DisplayInterface.SendData(lcdRegData19, 11);

    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData33, 1);
    g_DisplayInterface.SendData(lcdRegData20, 16);

    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData29, 1);
    g_DisplayInterface.SendData(lcdRegData21, 16);

    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData30, 1);
    g_DisplayInterface.SendData(lcdRegData22, 11);

    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData31, 1);
    g_DisplayInterface.SendData(lcdRegData23, 16);

    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData32, 1);
    g_DisplayInterface.SendData(lcdRegData24, 16);

    // PWR_CTRL1 - 0xC580 - 130th parameter - default 0x00 
    // Pump 1 min and max DM                                

    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData13, 1);
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)0xC5, ShortRegData47, 1);
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData48, 1);
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)0xF5, ShortRegData49, 1);

    // Exit CMD2 mode 
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData1, 1);
    g_DisplayInterface.SendData(lcdRegData25, 4);

    // Standard DCS Initialization TO KEEP CAN BE DONE IN HSDT                   

    // NOP - goes back to DCS std command ? 
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData1, 1);

    // Gamma correction 2.2+ table (HSDT possible) 
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData1, 1);
    g_DisplayInterface.SendData(lcdRegData3, 17);

    // Gamma correction 2.2- table (HSDT possible) 
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData1, 1);
    g_DisplayInterface.SendData(lcdRegData4, 17);

    // Send Sleep Out command to display : no parameter 
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData36, 1);

    // Wait for sleep out exit 
    g_DisplayInterface.DisplayCommandDelay(120);

    // Set Pixel color format to RGB565 
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData37, 1);

    // Send command to configure display in landscape orientation mode. By default the orientation mode is portrait
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData39, 1);

    // CASET value(Column Address Set) : X direction LCD GRAM boundaries
    // depending on LCD orientation modeand PASET value(Page Address Set) : Y direction
    // LCD GRAM boundaries depending on LCD orientation mode
    // XS[15:0] = 0x000 = 0, XE[15:0] = 0x31F = 799 for landscape mode : apply to CASET
    // YS[15:0] = 0x000 = 0, YE[15:0] = 0x31F = 799 for portrait mode : : apply to PASET
    g_DisplayInterface.SendData(lcdRegData27, 5);

    // XS[15:0] = 0x000 = 0, XE[15:0] = 0x1DF = 479 for portrait mode : apply to CASET
    // YS[15:0] = 0x000 = 0, YE[15:0] = 0x1DF = 479 for landscape mode : apply to PASET
    g_DisplayInterface.SendData(lcdRegData28, 5);

    //* CABC : Content Adaptive Backlight Control section start >> 
    // Note : defaut is 0 (lowest Brightness), 0xFF is highest Brightness, try 0x7F : intermediate value 
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData40, 1);

    // defaut is 0, try 0x2C - Brightness Control Block, Display Dimming & BackLight on 
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData41, 1);

    // defaut is 0, try 0x02 - image Content based Adaptive Brightness [Still Picture] 
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData42, 1);

    // defaut is 0 (lowest Brightness), 0xFF is highest Brightness 
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData43, 1);

    //* CABC : Content Adaptive Backlight Control section end << 

    // Send Command Display On 
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData44, 1);

    // NOP command 
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData1, 1);

    // Send Command GRAM memory write (no parameters) : this initiates frame write via other DSI commands sent by 
    // DSI host from LTDC incoming pixels in video mode 
    g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::NOP, ShortRegData45, 1);

    // g_DisplayInterface.SendCommandByte(CMD_SOFTWARE_RESET);
    // g_DisplayInterface.SendCommandByte(CMD_Display_OFF);

    SetupDisplayAttributes();

    return true;
}
void DisplayDriver::SetupDisplayAttributes()
{
    // Define the LCD/TFT resolution
    Attributes.LongerSide = 320;
    Attributes.ShorterSide = 240;
    Attributes.PowerSave = PowerSaveState::NORMAL;
    Attributes.BitsPerPixel = 16;

    ChangeOrientation(DisplayOrientation::LANDSCAPE);

}
bool DisplayDriver::Uninitialize()
{
    Clear();
    // Anything else to Uninitialize?
    return TRUE;
}
bool DisplayDriver::ChangeOrientation(DisplayOrientation newOrientation)
{
    Attributes.Orientation = newOrientation;
    //CLR_UINT8  CMD_Memory_Access_Control_Data_Portrait[] = { 0x48 };
    //CLR_UINT8  CMD_Memory_Access_Control_Data_Portrait180[] = { 0x88 };
    //CLR_UINT8  CMD_Memory_Access_Control_Data_Landscape[] = { 0xE8 };
    //CLR_UINT8  CMD_Memory_Access_Control_Data_Landscape180[] = { 0x28 };

    switch (newOrientation)
    {
    default:
        // Invalid! Fall through to portrait mode
    case DisplayOrientation::PORTRAIT:
        Attributes.Height = Attributes.LongerSide;
        Attributes.Width = Attributes.ShorterSide;
        // Change landscape code to suit switch statement
        //      g_DisplayInterface.SendCommandByteAndData(ShortRegData39);
        //      g_DisplayInterface.SendCommandByteAndData(lcdRegData27);
        //      g_DisplayInterface.SendCommandByteAndData(lcdRegData28);
        break;
    case DisplayOrientation::PORTRAIT180:
        Attributes.Height = Attributes.LongerSide;
        Attributes.Width = Attributes.ShorterSide;
        // Change landscape code to suit switch statement
        //      g_DisplayInterface.SendCommandByteAndData(ShortRegData39);
        //      g_DisplayInterface.SendCommandByteAndData(lcdRegData27);
        //      g_DisplayInterface.SendCommandByteAndData(lcdRegData28);
        break;
    case DisplayOrientation::LANDSCAPE:
        Attributes.Height = Attributes.ShorterSide;
        Attributes.Width = Attributes.LongerSide;

        g_DisplayInterface.SendCommandByteAndData((CLR_UINT8)Otm8009A_Command::MADCTR, ShortRegData39, 1);
        //   g_DisplayInterface.SendCommandByteAndData(lcdRegData27);
        //   g_DisplayInterface.SendCommandByteAndData(lcdRegData28);

        break;
    case DisplayOrientation::LANDSCAPE180:
        Attributes.Height = Attributes.ShorterSide;
        Attributes.Width = Attributes.LongerSide;

        // Change landscape code to suit switch statement
        //      g_DisplayInterface.SendCommandByteAndData(ShortRegData39);
        //      g_DisplayInterface.SendCommandByteAndData(lcdRegData27);
        //      g_DisplayInterface.SendCommandByteAndData(lcdRegData28);

        break;
    }

    return true;
}
bool DisplayDriver::SetWindow(CLR_UINT16 x1, CLR_UINT16 y1, CLR_UINT16 x2, CLR_UINT16 y2)
{
    CLR_UINT16 Addr1, Addr2;

    switch (Attributes.Orientation)
    {
    default:
        // Invalid! Fall through to portrait mode
    case PORTRAIT:
        Addr1 = x1;
        Addr2 = y1;
        break;
    case PORTRAIT180:
        Addr1 = (CLR_UINT16)(Attributes.ShorterSide - 1 - x1);
        Addr2 = (CLR_UINT16)(Attributes.LongerSide - 1 - y1);
        y1 = (CLR_UINT16)(Attributes.LongerSide - 1 - y2);
        y2 = Addr2;
        break;
    case LANDSCAPE:
        Addr1 = (CLR_UINT16)(Attributes.ShorterSide - 1 - y1);
        Addr2 = x1;
        y1 = x1;
        y2 = x2;
        break;
    case LANDSCAPE180:
        Addr1 = y1;
        Addr2 = (CLR_UINT16)(Attributes.ShorterSide - 1 - x1);    // pack X-Values into one word
        y1 = (CLR_UINT16)(Attributes.LongerSide - 1 - x2);
        y2 = Addr2;
        break;
    }


    //This is wrong, still from ILI9341
    CLR_UINT8  CMD_Column_Address_Set_Data[4];
    CMD_Column_Address_Set_Data[0] = (Addr1 >> 8) & 0xFF;
    CMD_Column_Address_Set_Data[1] = Addr1 & 0xFF;
    CMD_Column_Address_Set_Data[2] = (Addr2 >> 8) & 0xFF;
    CMD_Column_Address_Set_Data[3] = Addr2 & 0xFF;

    CLR_UINT8  CMD_Page_Address_Set_Data[4];
    CMD_Page_Address_Set_Data[0] = (y1 >> 8) & 0xFF;
    CMD_Page_Address_Set_Data[1] = y1 & 0xFF;
    CMD_Page_Address_Set_Data[2] = (y2 >> 8) & 0xFF;
    CMD_Page_Address_Set_Data[3] = y2 & 0xFF;

    ASSERT(CMD_Column_Address_Set_Data[0] = CMD_Column_Address_Set_Data[0])
        ASSERT(CMD_Page_Address_Set_Data[0] = CMD_Page_Address_Set_Data[0])

        // Fix, code is for ili9641 not otm8009a
     //   g_DisplayInterface.SendCommandByteAndData(Otm8009A_Command::CASET, CMD_Column_Address_Set_Data, 4);
     //   g_DisplayInterface.SendCommandByteAndData(Otm8009A_Command::PASET, CMD_Page_Address_Set_Data, 4);
     //   g_DisplayInterface.SendCommand(Otm8009A_Command::);

        return true;
}
void DisplayDriver::PowerSave(PowerSaveState powerState)
{
    //CLR_UINT8 CMD_POWER_STATE_Data_ON[] = { 0x0000 };
    //CLR_UINT8 CMD_POWER_STATE_Data_SLEEP[] = { 0x0001 };
    switch (powerState)
    {
    default:
        // illegal fall through to Power on
    case PowerSaveState::NORMAL:
        //g_DisplayInterface.SendCommand(Otm8009A_Command::SLPOUT);  // leave sleep mode
        break;
    case PowerSaveState::SLEEP:
        //g_DisplayInterface.SendCommand(Otm8009A_Command::SLPIN);  // sleep mode
        break;
    }
    return;
}
void DisplayDriver::Clear()
{
    //reset the cursor pos to the begining
    // Clear the ILI9341 controller 

}
void DisplayDriver::DisplayBrightness(CLR_INT16 brightness)
{
    _ASSERTE(brightness >= 0 && brightness <= 100);
    // CLR_UINT8  CMD_Write_Display_Brightness_Data[] = { (CLR_UINT8)brightness };
    //g_DisplayInterface.SendCommandByteAndData(CMD_Write_Display_Brightness, CMD_Write_Display_Brightness_Data, 1);
}
void DisplayDriver::BitBlt(int width, int height, int widthInWords, CLR_UINT32 data[])
{
    _ASSERTE(width == (int)Attributes.Width);
    _ASSERTE(height == (int)Attributes.Height);
    _ASSERTE(widthInWords == width / (int)PixelsPerWord());

    BitBltEx(0, 0, width, height, data);
}
void DisplayDriver::BitBltEx(int x, int y, int width, int height, CLR_UINT32 data[])
{
    ASSERT(x = x); ASSERT(y = y); ASSERT(width = width); ASSERT(height = height); ASSERT(data[0] = 0);
}
CLR_UINT32 DisplayDriver::PixelsPerWord()
{
    return (32 / Attributes.BitsPerPixel);
}
CLR_UINT32 DisplayDriver::WidthInWords()
{
    return ((Attributes.Width + (PixelsPerWord() - 1)) / PixelsPerWord());
}
CLR_UINT32 DisplayDriver::SizeInWords()
{
    return (WidthInWords() * Attributes.Height);
}
CLR_UINT32 DisplayDriver::SizeInBytes()
{
    return (SizeInWords() * sizeof(CLR_UINT32));
}
CLR_UINT32 DisplayDriver::ConvertColor(CLR_UINT32 color)
{
    // Only require code if there is a requirement not already supported by the core.
    // 16Bits per pixel is supported by core software which is use by this ILI9341 implementation.
    // 18 bits per pixel colour is possible on ILI9341 controller but would require 
    // changes to the InitializeDisplayRegisters() routine and Graphics memory used
    return color;
}



















/////////////


