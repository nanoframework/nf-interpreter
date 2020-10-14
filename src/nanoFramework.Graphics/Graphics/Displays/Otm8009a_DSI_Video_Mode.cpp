//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "Graphics.h"
#include "DisplayInterface.h"
#include "Display.h"

//
// The OTM8009A, a 16,777,216 - color System - on - Chip(SoC) driver LSI designed for smalland medium sizes of
// TFT LCD display, is capable of supporting up to 480xRGBx864(WVGA) in resolution which can be achieved
// by the integrated RAM for graphic data.The 480 - channel source driver has true 8 - bit resolution,
// generates 256 Gamma - corrected values by an internal D / A converter.
// The OTM8009A is able to operate with low IO interface power supply and incorporate with several charge pumps
// to generate various voltage levels that form an on - chip power management system for gate driverand source driver.



// Packet Transmission
// ------------------- 
// Display Command Set(DCS) is used from the MCU to the display module.
// Both Short Packet(SPa) and Long packet(LPa) transmission is used.
// 
// REFERENCE : Section "6.2.4.2. Packet transmissions" of OTM8009A PDF by OriseTech
//


struct DisplayDriver g_DisplayDriver;
extern DisplayInterface g_DisplayInterface;


// List of OTM8009A used commands                                  
// Detailed in OTM8009A Data Sheet 'DATA_SHEET_OTM8009A_V0 92.pdf' 
// Version of 14 June 2012                                         
#define COLMOD_RGB565 0x55     // COLMOD  pixel format
#define OTM8009A_CMD_SLPOUT 0x11    // Sleep Out command 
#define OTM8009A_CMD_DISPON 0x29  // Display On command 
#define OTM8009A_CMD_RAMWR 0x2C // Memory (GRAM) write command 
#define OTM8009A_CMD_RAMRD 0x2E // Memory (GRAM) read command  
#define OTM8009A_CMD_WRTESCN 0x44 // Write Tearing Effect Scan line command 
#define OTM8009A_CMD_WRCTRLD 0x53  // Write CTRL Display command                
#define OTM8009A_CMD_WRCABC 0x55   // Write Content Adaptive Brightness command 
#define OTM8009A_CMD_WRCABCMB 0x5E // Write CABC Minimum Brightness command     
#define OTM8009A_CMD_WRDISBV 0x51  // Write Display Brightness command          

#define COLMOD  0x3A    // Interface Pixel format command  (12/16/18/24 bits per pixel)
#define CASET  0x2A     // Column address set command (used to define area of frame memory where MCU can access)
#define PASET  0x2B     // Page address set command (used to define area of frame memory where MCU can access)
#define MADCTR  0x36    // Memory Access control  (defines read/ write scanning direction of frame memory)

#define Register0xFF 0xFF
#define Register0x00 0x00


bool DisplayDriver::Initialize()
{
    SetupDisplayAttributes();

    // Enter in command 2 mode and set EXTC to enable address shift function (0x00) 
    g_DisplayInterface.SendCommand(2, 0x00, 0x00);
    g_DisplayInterface.SendCommand(4, Register0xFF,0x80, 0x09, 0x01);

    // Enter ORISE Command 2 
    g_DisplayInterface.SendCommand(2, 0x00, 0x00);     //?
    g_DisplayInterface.SendCommand(2, Register0x00, 0x80);
    g_DisplayInterface.SendCommand(3, Register0xFF, 0x80, 0x09);

    // SD_PCH_CTRL - 0xC480h - 129th parameter - Default 0x00
    // Set SD_PT
    // -> Source output level during porch and non-display area to GND
    g_DisplayInterface.SendCommand(2, 0x00, 0x80);
    g_DisplayInterface.SendCommand(2, 0xC4, 0x30);
    OS_DELAY(10);
    // Not documented
    g_DisplayInterface.SendCommand(2, 0x00, 0x8A);
    g_DisplayInterface.SendCommand(2, 0xC4, 0x40);

    OS_DELAY(10);

    // PWR_CTRL4 - 0xC4B0h - 178th parameter - Default 0xA8
    // Set gvdd_en_test
    // -> enable GVDD test mode !!!
    g_DisplayInterface.SendCommand(2, 0x00, 0xB1);
    g_DisplayInterface.SendCommand(2, 0xC5, 0xA9);

    // PWR_CTRL2 - 0xC590h - 146th parameter - Default 0x79
    // Set pump 4 vgh voltage
    // -> from 15.0v down to 13.0v
    // Set pump 5 vgh voltage
    // -> from -12.0v downto -9.0v
    g_DisplayInterface.SendCommand(2, 0x00, 0x91);
    g_DisplayInterface.SendCommand(2, 0xC5, 0x34);

    // P_DRV_M - 0xC0B4h - 181th parameter - Default 0x00
    // -> Column inversion
    g_DisplayInterface.SendCommand(2, 0x00, 0xB4);
    g_DisplayInterface.SendCommand(2, 0xC0, 0x50);

    // VCOMDC - 0xD900h - 1st parameter - Default 0x39h
    // VCOM Voltage settings
    // -> from -1.0000v downto -1.2625v
    g_DisplayInterface.SendCommand(2, 0x00, 0x00);
    g_DisplayInterface.SendCommand(2, 0xD9, 0x4E);

    // Oscillator adjustment for Idle/Normal mode (LPDT only) set to 65Hz (default is 60Hz) 
    g_DisplayInterface.SendCommand(2, 0x00, 0x81);
    g_DisplayInterface.SendCommand(2, 0xC1, 0x66);

    // Video mode internal 
    g_DisplayInterface.SendCommand(2, 0x00, 0xA1);
    g_DisplayInterface.SendCommand(2, 0xC1, 0x08);

    // PWR_CTRL2 - 0xC590h - 147h parameter - Default 0x00 
    // Set pump 4&5 x6                                     
    // -> ONLY VALID when PUMP4_EN_ASDM_HV = "0"           
    g_DisplayInterface.SendCommand(2, 0x00, 0x92);
    g_DisplayInterface.SendCommand(2, 0xC5, 0x01);

    // PWR_CTRL2 - 0xC590h - 150th parameter - Default 0x33h 
    // Change pump4 clock ratio                              
    // -> from 1 line to 1/2 line                            
    g_DisplayInterface.SendCommand(2, 0x00, 0x95);
    g_DisplayInterface.SendCommand(2, 0xC5, 0x34);

    // GVDD/NGVDD settings 
    g_DisplayInterface.SendCommand(2, 0x00, 0x00);
    g_DisplayInterface.SendCommand(3, 0xD8,0x79, 0x79 );

    // PWR_CTRL2 - 0xC590h - 149th parameter - Default 0x33h 
    // Rewrite the default value !                           
    g_DisplayInterface.SendCommand(2, 0x00, 0x94);
    g_DisplayInterface.SendCommand(2, 0xC5, 0x33);

    // Panel display timing Setting 3 
    g_DisplayInterface.SendCommand(2, 0x00, 0xA3);
    g_DisplayInterface.SendCommand(2, 0xC0, 0x1B);

    // Power control 1 
    g_DisplayInterface.SendCommand(2, 0x00, 0x82);
    g_DisplayInterface.SendCommand(2, 0xC5, 0x83);

    // Source driver precharge 
    g_DisplayInterface.SendCommand(2, 0x00, 0x81);
    g_DisplayInterface.SendCommand(2, 0xC4, 0x83);

    g_DisplayInterface.SendCommand(2, 0x00, 0xA1);
    g_DisplayInterface.SendCommand(2, 0xC1, 0x0E);

    g_DisplayInterface.SendCommand(2, 0x00, 0xA6);

    // GOAVST 
    g_DisplayInterface.SendCommand(2, 0x00, 0x80);
    g_DisplayInterface.SendCommand(7, 0xCE, 0x85, 0x01, 0x00, 0x84, 0x01, 0x00);

    g_DisplayInterface.SendCommand(2, 0x00, 0xA0);
    g_DisplayInterface.SendCommand(15, 0xCE, 0x18, 0x04, 0x03, 0x39, 0x00, 0x00, 0x00, 0x18, 0x03, 0x03, 0x3A, 0x00, 0x00, 0x00);

    g_DisplayInterface.SendCommand(2, 0x00, 0xB0);
    g_DisplayInterface.SendCommand(15, 0xCE, 0x18, 0x02, 0x03, 0x3B, 0x00, 0x00, 0x00, 0x18, 0x01, 0x03, 0x3C, 0x00, 0x00, 0x00);

    g_DisplayInterface.SendCommand(2, 0x00, 0xC0);
    g_DisplayInterface.SendCommand(11, 0xCF, 0x01, 0x01, 0x20, 0x20, 0x00, 0x00, 0x01, 0x02, 0x00, 0x00);

    g_DisplayInterface.SendCommand(2, 0x00, 0xD0);
    g_DisplayInterface.SendCommand(2, 0xCF, 0x00);

    g_DisplayInterface.SendCommand(2, 0x00, 0x80);
    g_DisplayInterface.SendCommand(11, 0xCB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);

    g_DisplayInterface.SendCommand(2, 0x00, 0x90);
    g_DisplayInterface.SendCommand(16, 0xCB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);

    g_DisplayInterface.SendCommand(2, 0x00, 0xA0);
    g_DisplayInterface.SendCommand(16, 0xCB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);

    g_DisplayInterface.SendCommand(2, 0x00, 0xB0);
    g_DisplayInterface.SendCommand(11, 0xCB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);

    g_DisplayInterface.SendCommand(2, 0x00, 0xC0);
    g_DisplayInterface.SendCommand(16, 0xCB, 0x00, 0x04, 0x04, 0x04, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);

    g_DisplayInterface.SendCommand(2, 0x00, 0xD0);
    g_DisplayInterface.SendCommand(16, 0xCB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x04, 0x04, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00);

    g_DisplayInterface.SendCommand(2, 0x00, 0xE0);
    g_DisplayInterface.SendCommand(11, 0xCB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);

    g_DisplayInterface.SendCommand(2, 0x00, 0xF0);
    g_DisplayInterface.SendCommand(11, 0xCB, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);

    g_DisplayInterface.SendCommand(2, 0x00, 0x80);
    g_DisplayInterface.SendCommand(11, 0xCC, 0x00, 0x26, 0x09, 0x0B, 0x01, 0x25, 0x00, 0x00, 0x00, 0x00);

    g_DisplayInterface.SendCommand(2, 0x00, 0x90);
    g_DisplayInterface.SendCommand(16, 0xCC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x26, 0x0A, 0x0C, 0x02);

    g_DisplayInterface.SendCommand(2, 0x00, 0xA0);
    g_DisplayInterface.SendCommand(16, 0xCC, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);

    g_DisplayInterface.SendCommand(2, 0x00, 0xB0);
    g_DisplayInterface.SendCommand(11, 0xCC, 0x00, 0x25, 0x0C, 0x0A, 0x02, 0x26, 0x00, 0x00, 0x00, 0x00);

    g_DisplayInterface.SendCommand(2, 0x00, 0xC0);
    g_DisplayInterface.SendCommand(16, 0xCC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x25, 0x0B, 0x09, 0x01);

    g_DisplayInterface.SendCommand(2, 0x00, 0xD0);
    g_DisplayInterface.SendCommand(16, 0xCC, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);

    // PWR_CTRL1 - 0xc580h - 130th parameter - default 0x00 
    // Pump 1 min and max DM                                
    g_DisplayInterface.SendCommand(2, 0x00, 0x81);
    g_DisplayInterface.SendCommand(2, 0xC5, 0x66);
    g_DisplayInterface.SendCommand(2, 0x00, 0xB6);
    g_DisplayInterface.SendCommand(2, 0xF5, 0x06);

    // CABC LEDPWM frequency adjusted to 19,5kHz 
    g_DisplayInterface.SendCommand(2, 0x00, 0xB1);
    g_DisplayInterface.SendCommand(2, 0xC6, 0x06);

    // Exit CMD2 mode 
    g_DisplayInterface.SendCommand(2, 0x00, 0x00);
    g_DisplayInterface.SendCommand(4, 0xFF, 0xFF, 0xFF, 0xFF);

    // NOP - goes back to DCS std command ? 
    g_DisplayInterface.SendCommand(2, 0x00, 0x00);

    // Gamma correction 2.2+ table (HSDT possible) 
    g_DisplayInterface.SendCommand(2, 0x00, 0x00);
    g_DisplayInterface.SendCommand(17, 0xE1, 0x00, 0x09, 0x0F, 0x0E, 0x07, 0x10, 0x0B, 0x0A, 0x04, 0x07, 0x0B, 0x08, 0x0F, 0x10, 0x0A, 0x01);

    // Gamma correction 2.2- table (HSDT possible) 
    g_DisplayInterface.SendCommand(2, 0x00, 0x00);
    g_DisplayInterface.SendCommand(17, 0xE2, 0x00, 0x09, 0x0F, 0x0E, 0x07, 0x10, 0x0B, 0x0A, 0x04, 0x07, 0x0B, 0x08, 0x0F, 0x10, 0x0A, 0x01);

    // Send Sleep Out command to display : no parameter 
    g_DisplayInterface.SendCommand(2, OTM8009A_CMD_SLPOUT, 0x00);

    // Wait for sleep out exit 
    OS_DELAY(120);

    // Set Pixel color format to RGB565 
    g_DisplayInterface.SendCommand(2, COLMOD, COLMOD_RGB565);

    //* CABC : Content Adaptive Backlight Control section start >> 
    // Note : defaut is 0 (lowest Brightness), 0xFF is highest Brightness, try 0x7F : intermediate value 
    g_DisplayInterface.SendCommand(2, OTM8009A_CMD_WRDISBV, 0x7F);

    // defaut is 0, try 0x2C - Brightness Control Block, Display Dimming & BackLight on 
    g_DisplayInterface.SendCommand(2, OTM8009A_CMD_WRCTRLD, 0x2C);

    // defaut is 0, try 0x02 - image Content based Adaptive Brightness [Still Picture] 
    g_DisplayInterface.SendCommand(2, OTM8009A_CMD_WRCABC, 0x02);

    // defaut is 0 (lowest Brightness), 0xFF is highest Brightness 
    g_DisplayInterface.SendCommand(2, OTM8009A_CMD_WRCABCMB, 0xFF);

    g_DisplayInterface.SendCommand(2, OTM8009A_CMD_DISPON, 0x00);                       // Send Command Display On 
    g_DisplayInterface.SendCommand(2, 0x00, 0x00);                                      // NOP command 

    // Send Command GRAM memory write (no parameters) : this initiates frame write via other DSI commands sent by 
    // DSI host from LTDC incoming pixels in video mode 
    g_DisplayInterface.SendCommand(2, OTM8009A_CMD_RAMWR, 0x00);

    // Setup to Landscape
    //___________________
    SetDefaultOrientation();
    g_DisplayInterface.SendCommand(2, MADCTR, 0x60);
    g_DisplayInterface.SendCommand(5, CASET, 0x00, 0x00, 0x03, 0x1F);
    g_DisplayInterface.SendCommand(5, PASET, 0x00, 0x00, 0x01, 0xDF);

    return 0;
}

void DisplayDriver::SetupDisplayAttributes()
{
    // Define the LCD/TFT resolution
    Attributes.LongerSide = 800;
    Attributes.ShorterSide = 480;
    Attributes.PowerSave = PowerSaveState::NORMAL;
    Attributes.BitsPerPixel = 16;

}
void DisplayDriver::SetDefaultOrientation()
{
    Attributes.Height = Attributes.ShorterSide;
    Attributes.Width = Attributes.LongerSide;
    return;
}
void DisplayDriver::DisplayBrightness(CLR_INT16 brightness)
{
    _ASSERTE(brightness >= 0 && brightness <= 100);
}
void DisplayDriver::BitBlt(int x, int y, int width, int height, CLR_UINT32 data[])
{
    UNUSED(x);
    UNUSED(y);
    UNUSED(width);
    UNUSED(height);

    CLR_UINT8* pui8Data = (CLR_UINT8*)data;
    CLR_UINT32 dataSize = width * height * 2;

    g_DisplayInterface.WriteToFrameBuffer(0,pui8Data, dataSize);
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
