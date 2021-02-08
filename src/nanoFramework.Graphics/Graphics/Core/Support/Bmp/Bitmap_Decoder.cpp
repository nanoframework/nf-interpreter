//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <string.h>

#include "Graphics.h"

#define BI_RGB       0L
#define BI_RLE8      1L
#define BI_RLE4      2L
#define BI_BITFIELDS 3L
#define BI_JPEG      4L
#define BI_PNG       5L

// Initialization routine for BmpDecoder struct. When it's finished,
// the width / height / encodingType would be already loaded.
HRESULT BmpDecoder::BmpInitOutput(const CLR_UINT8 *src, CLR_UINT32 srcSize)
{
    NANOCLR_HEADER();

    encodingType = BmpUnknown;

    // set up the CLR_RT_ByteArrayReader
    NANOCLR_CHECK_HRESULT(source.Init(src, srcSize));

    const BITMAPFILEHEADER *pbmfh;
    const BITMAPINFOHEADER *pbmih;

    pbmfh = (const BITMAPFILEHEADER *)source.source;
    NANOCLR_CHECK_HRESULT(source.Skip(sizeof(BITMAPFILEHEADER))); // Note that pbmfh->bfSize records the size of the
                                                                  // entire bitmap image, NOT the size of the struct

    if (pbmfh->bfType != 0x4d42 /*'BM'*/)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }
    pbmih = (const BITMAPINFOHEADER *)source.source;
    if (pbmih->biSize < sizeof(BITMAPINFOHEADER))
    {
        // if biSize is smaller than the struct, we'll read into unchartered memory,
        // so we fail
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }
    NANOCLR_CHECK_HRESULT(source.Skip(pbmih->biSize));

    if (pbmih->biPlanes != 1)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    if (pbmih->biHeight > 0)
    {
        height = pbmih->biHeight;
        isTopDown = false;
    }
    else
    {
        height = -pbmih->biHeight;
        isTopDown = true;
    }

    width = pbmih->biWidth;

    if (width <= 0 || height <= 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    // Check for encoding types.
    switch (pbmih->biBitCount)
    {
        case 16:
            if (pbmih->biCompression == BI_BITFIELDS)
            {
                COLORREF bmiColors[3];

                memcpy(bmiColors, source.source, sizeof(bmiColors));

                NANOCLR_CHECK_HRESULT(source.Skip(sizeof(bmiColors)));

                if ((bmiColors[0] /* R */ == 0xF800) && (bmiColors[1] /* G */ == 0x07E0) &&
                    (bmiColors[2] /* B */ == 0x001F))
                {
                    encodingType = Bmp16Bit_565;
                }
            }
            break;
        case 32:
            if (pbmih->biCompression == BI_RGB)
            {
                encodingType = Bmp32Bit_ARGB;
            }
            break;
        case 24:
            if (pbmih->biCompression == BI_RGB)
            {
                encodingType = Bmp24Bit_RGB;
            }
            break;
        case 8:
            if (pbmih->biClrUsed != 0)
            {
                palette = source.source;
                paletteDepth = (CLR_UINT8)(
                    (pbmfh->bfOffBits - sizeof(BITMAPINFOHEADER) - sizeof(BITMAPFILEHEADER)) /
                    pbmih->biClrUsed); // the rest is the palette
                encodingType = Bmp8Bit_Indexed;
            }
            break;
    }

    if (encodingType == BmpUnknown)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
    }

    // Reset the source, and skip to the beginning of payload.
    NANOCLR_CHECK_HRESULT(source.Init(src, srcSize));
    NANOCLR_CHECK_HRESULT(source.Skip(pbmfh->bfOffBits));

    NANOCLR_NOCLEANUP();
}

// Output to the out buffer the image in the native screen format.
// Note that the CLR_GFX_Bitmap _must_ be the same size as the bmp
// Also, BmpInitOutput() _must_ be called BEFORE BmpStartOutput()
//
// Note also, that it is the responsibility of the caller to reject bitmap with
// abnormal width / height / size prior to calling this function (i.e. by using
// BitmapDescription_Initialize() ) These BmpStartOutput functions *ASSUME* that
// these values are within a reasonable range so that none of the calculation
// would overflow.
//
HRESULT BmpDecoder::BmpStartOutput(CLR_GFX_Bitmap *bitmap)
{
    NANOCLR_HEADER();

    CLR_UINT32 config;
    BmpOutputHelperParam param;
    GFX_Rect rect;

    ASSERT(width > 0);
    ASSERT(height > 0);
    ASSERT(bitmap);
    // Assert the right size
    ASSERT(bitmap->m_bm.m_width == (CLR_UINT32)width);
    ASSERT(bitmap->m_bm.m_height == (CLR_UINT32)height);
    // Assert there's no clipping
    ASSERT(bitmap->m_palBitmap.clipping.left == 0);
    ASSERT(bitmap->m_palBitmap.clipping.top == 0);
    ASSERT(bitmap->m_palBitmap.clipping.right == width);
    ASSERT(bitmap->m_palBitmap.clipping.bottom == height);

    if (!source.IsValid())
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    param.encodingType = encodingType;
    param.source = &source;

    switch (encodingType)
    {
        case Bmp16Bit_565:
            param.srcWidthInBytes = ((width * 16 + 31) / 32) * 4; // each line is 4-byte aligned in size
            break;

        case Bmp32Bit_ARGB:
            param.srcWidthInBytes = ((width * 32 + 31) / 32) * 4; // each line is 4-byte aligned in size
            break;

        case Bmp24Bit_RGB:
            param.srcWidthInBytes = ((width * 24 + 31) / 32) * 4; // each line is 4-byte aligned in size
            break;

        case Bmp8Bit_Indexed:
            param.srcWidthInBytes = ((width * 8 + 31) / 32) * 4; // each line is 4-byte aligned in size
            param.palette = palette;
            param.paletteDepth = paletteDepth;
            break;

        case BmpUnknown:
            NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
            break;

        default:
            NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
    }

    // fail if we won't have enough bytes in the source
    if (source.sourceSize < param.srcWidthInBytes * height)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }

    rect.left = 0;
    rect.top = 0;
    rect.right = width - 1;
    rect.bottom = height - 1;

    config = PAL_GFX_Bitmap::c_SetPixelsConfig_NoClip | PAL_GFX_Bitmap::c_SetPixelsConfig_NoClipChecks;

    if (!isTopDown)
    {
        config |= PAL_GFX_Bitmap::c_SetPixelsConfig_ReverseY;
    }

#if __GNUC__ > 6
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
#endif
    bitmap->SetPixelsHelper(rect, config, (GFX_SetPixelsCallback)&BmpOutputHelper, &param);
#if __GNUC__ > 6
#pragma GCC diagnostic pop
#endif

    NANOCLR_NOCLEANUP();
}

CLR_UINT32 BmpDecoder::BmpOutputHelper(CLR_UINT32 flags, CLR_UINT16 &opacity, void *param)
{
    BmpOutputHelperParam *myParam = (BmpOutputHelperParam *)param;

    if (flags & PAL_GFX_Bitmap::c_SetPixels_NewRow)
    {
        myParam->srcRGB = (CLR_UINT8 *)myParam->source->source;
        myParam->source->Skip(myParam->srcWidthInBytes);
    }

    opacity = PAL_GFX_Bitmap::c_OpacityOpaque;

    CLR_UINT32 r = 0;
    CLR_UINT32 g = 0;
    CLR_UINT32 b = 0;

    switch (myParam->encodingType)
    {
        case Bmp8Bit_Indexed:
        {
            int index = (int)*((CLR_UINT8 *)myParam->srcRGB) * myParam->paletteDepth;

            b = (CLR_UINT32)myParam->palette[index + 0];
            g = (CLR_UINT32)myParam->palette[index + 1];
            r = (CLR_UINT32)myParam->palette[index + 2];

            myParam->srcRGB += 1;
            break;
        }
        case Bmp16Bit_565:
        {
            CLR_UINT32 color16 = *((CLR_UINT16 *)(myParam->srcRGB));
            r = (color16 >> 11) << 3;
            if ((r & 0x8) != 0)
                r |= 0x7; // Copy LSB
            g = ((color16 >> 5) & 0x3f) << 2;
            if ((g & 0x4) != 0)
                g |= 0x3; // Copy LSB
            b = (color16 & 0x1f) << 3;
            if ((b & 0x8) != 0)
                b |= 0x7; // Copy LSB

            myParam->srcRGB += 2;
            break;
        }
        case Bmp24Bit_RGB:
        {
            r = myParam->srcRGB[2];
            g = myParam->srcRGB[1];
            b = myParam->srcRGB[0];

            myParam->srcRGB += 3;
            break;
        }
        case Bmp32Bit_ARGB:
        {
            r = myParam->srcRGB[2];
            g = myParam->srcRGB[1];
            b = myParam->srcRGB[0];

            myParam->srcRGB += 4;
            break;
        }
        case BmpUnknown:
            break;
        default:
            break;
    }

    return r | (g << 8) | (b << 16);
}
