////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _BMP_H_
#define _BMP_H_

#include "nanoCLR_Types.h"
#include "nanoCLR_Runtime.h"


#if !defined(_WIN32)
// For non-windows build, we need to define the Bitmap header structures.
// (These are defined in wingdi.h which is indirectly linked in windows build.)

 struct __packed BITMAPFILEHEADER {
    CLR_UINT16 bfType;
    CLR_UINT32 bfSize;
    CLR_UINT16 bfReserved1;
    CLR_UINT16 bfReserved2;
    CLR_UINT32 bfOffBits;
};

 struct __packed BITMAPINFOHEADER {
    CLR_UINT32 biSize;
    CLR_INT32  biWidth;
    CLR_INT32  biHeight;
    CLR_UINT16 biPlanes;
    CLR_UINT16 biBitCount;
    CLR_UINT32 biCompression;
    CLR_UINT32 biSizeImage;
    CLR_INT32  biXPelsPerMeter;
    CLR_INT32  biYPelsPerMeter;
    CLR_UINT32 biClrUsed;
    CLR_UINT32 biClrImportant;
};

#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L
#define BI_JPEG       4L
#define BI_PNG        5L

typedef CLR_UINT32 COLORREF;

#endif //#if !defined(PLATFORM_WINDOWS_EMULATOR)

enum BmpEncodingType
{
    BmpUnknown      = 0,
    Bmp16Bit_565    = 1,
    Bmp24Bit_RGB    = 2,
    Bmp8Bit_Indexed = 3,
    Bmp32Bit_ARGB   = 4,    
};

struct BmpDecoder
{
    int             width;
    int             height;
    BmpEncodingType encodingType;

    HRESULT BmpInitOutput ( const CLR_UINT8* src, CLR_UINT32 srcSize );
    HRESULT BmpStartOutput( CLR_GFX_Bitmap* bitmap );

    static CLR_UINT32 BmpOutputHelper(  CLR_UINT32 flags, CLR_UINT16& opacity, void* param );

private:
    CLR_RT_ByteArrayReader source;
    const CLR_UINT8*           palette;
    CLR_UINT8                  paletteDepth;
    bool                   isTopDown;
};

#endif
