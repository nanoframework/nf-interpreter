//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

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

#include "nanoCLR_Types.h"

#define GIFPLAINTEXTEXTENSIONSIZE 13
#define GIFAPPEXTENSIONHEADERSIZE 11

struct GifFileHeader // 13 bytes
{
    CLR_UINT8 signature[6];
    CLR_UINT16 LogicScreenWidth;
    CLR_UINT16 LogicScreenHeight;
    //	CLR_UINT16 __packed LogicScreenWidth;
    //  CLR_UINT16 __packed LogicScreenHeight;
    CLR_UINT8 globalcolortablesize : 3; // bit fields in reverse significant order
    CLR_UINT8 sortflag : 1;
    CLR_UINT8 colorresolution : 3;
    CLR_UINT8 globalcolortableflag : 1; // <- most significant
    CLR_UINT8 backgroundcolor;
    CLR_UINT8 pixelaspect;
} __attribute__((packed));

struct GifPaletteEntry
{
    CLR_UINT8 red;
    CLR_UINT8 green;
    CLR_UINT8 blue;
} __attribute__((packed));

struct GifColorTable // palette is up to 3*256 BYTEs
{
    GifPaletteEntry colors[256];
} __attribute__((packed));

struct GifImageDescriptor // 9 bytes
{
    // CLR_UINT8 imageseparator;  //=0x2C
    CLR_UINT16 left;
    CLR_UINT16 top;
    CLR_UINT16 width;
    CLR_UINT16 height;
    // CLR_UINT16 __packed left;
    //   CLR_UINT16 __packed top;
    //   CLR_UINT16 __packed width;
    //   CLR_UINT16 __packed height;
    CLR_UINT8 localcolortablesize : 3; // bit fields in reverse significant order
    CLR_UINT8 reserved : 2;
    CLR_UINT8 sortflag : 1;
    CLR_UINT8 interlaceflag : 1;
    CLR_UINT8 localcolortableflag : 1; // <- most significant
} __attribute__((packed));

struct GifGraphicControlExtension // 6 bytes
{
    // CLR_UINT8 extensionintroducer;  //=0x21
    // CLR_UINT8 graphiccontrollabel;  //=0xF9
    CLR_UINT8 blocksize;
    CLR_UINT8 transparentcolorflag : 1; // bit fields in reverse significant order
    CLR_UINT8 userinputflag : 1;
    CLR_UINT8 disposalmethod : 3;
    CLR_UINT8 reserved : 3; // <- most significant

    CLR_UINT16 delaytime; // in hundreths of a second
    // CLR_UINT16 __packed delaytime;  //in hundreths of a second
    CLR_UINT8 transparentcolorindex;
} __attribute__((packed));

#endif
