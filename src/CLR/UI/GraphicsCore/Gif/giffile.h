////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////#ifndef _GIFFILE_H_
#ifndef _GIFFILE_H_
#define _GIFFILE_H_

/**************************************************************************\
* 
*
* Module Name:
*
*   giffile.hpp
*
* Abstract:
*
*   Header file with gif file structures.
*
* Revision History:
*
*   6/8/1999 t-aaronl
*       Created it.
*
\**************************************************************************/

#define GIFPLAINTEXTEXTENSIONSIZE 13
#define GIFAPPEXTENSIONHEADERSIZE 11


struct __packed GifFileHeader  //13 bytes
{
    uint8_t signature[6];
    uint16_t __packed LogicScreenWidth;
    uint16_t __packed LogicScreenHeight;
    uint8_t globalcolortablesize: 3;  //bit fields in reverse significant order
    uint8_t sortflag: 1;
    uint8_t colorresolution: 3;
    uint8_t globalcolortableflag: 1;  // <- most significant
    uint8_t backgroundcolor;
    uint8_t pixelaspect;
};

 struct __packed GifPaletteEntry
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

 struct __packed GifColorTable  //palette is up to 3*256 BYTEs
{
    GifPaletteEntry colors[256];
};

struct __packed GifImageDescriptor  //9 bytes
{
  //uint8_t imageseparator;  //=0x2C
    uint16_t __packed left;
    uint16_t __packed top;
    uint16_t __packed width;
    uint16_t __packed height;
    uint8_t localcolortablesize: 3;  //bit fields in reverse significant order
    uint8_t reserved: 2;
    uint8_t sortflag: 1;
    uint8_t interlaceflag: 1;
    uint8_t localcolortableflag: 1;  // <- most significant
};

struct __packed GifGraphicControlExtension  //6 bytes
{
  //uint8_t extensionintroducer;  //=0x21
  //uint8_t graphiccontrollabel;  //=0xF9
    uint8_t blocksize;
    uint8_t transparentcolorflag: 1;  //bit fields in reverse significant order
    uint8_t userinputflag: 1;
    uint8_t disposalmethod: 3;
    uint8_t reserved: 3;  // <- most significant

    uint16_t __packed delaytime;  //in hundreths of a second
    uint8_t transparentcolorindex;
};



#endif
