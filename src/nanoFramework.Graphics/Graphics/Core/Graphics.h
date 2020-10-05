//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_ 1

#include <string.h>

#include "nanoCLR_Types.h"
#include "nanoCLR_Interop.h"
#include "nanoCLR_Runtime.h"
#include "nanoCLR_Platformdef.h"
#include "nanoCLR_Runtime__HeapBlock.h"
#include "nanoCLR_Checks.h"
#include "nanoPAL.h"
#include "nanoHAL_Time.h"
#include "nanoPAL_events.h"
#include "nanohal.h"
#include "nanoHAL_v2.h"
#include "corlib_native.h"

#include "GraphicsMemoryHeap.h"

#include "nf_rt_events_native.h"
#include "nf_errors_exceptions.h"

#include "Display.h"

#define UNUSED(X) (void)X /* To avoid gcc/g++ warnings */

// ???? Redefined below but gets past the compile error of not defined
// CLR_GFX_Font needs CLR_GFX_Bitmap  and CLR_GFX_Bitmap needs CLR_GFX_Font
struct CLR_GFX_Bitmap;
struct CLR_GFX_Font;

#define __min(a, b) (((a) < (b)) ? (a) : (b))
#define abs(a)      (((a) < 0) ? -(a) : (a))
struct CLR_GFX_BitmapDescription
{
    static const CLR_UINT16 c_ReadOnly = 0x0001;
    static const CLR_UINT16 c_Compressed = 0x0002;
    static const CLR_UINT8 c_CompressedRun = 0x80;
    static const CLR_UINT8 c_CompressedRunSet = 0x40;
    static const CLR_UINT8 c_CompressedRunLengthMask = 0x3f;
    static const CLR_UINT8 c_UncompressedRunLength = 7;
    static const CLR_UINT8 c_CompressedRunOffset = c_UncompressedRunLength + 1;

    // static // Note that these type definitions has to match the ones defined in Bitmap.BitmapImageType enum defined
    // in Graphics.cs
    static const CLR_UINT8 c_Type_nanoCLRBitmap = 0;
    static const CLR_UINT8 c_TypeGif = 1;
    static const CLR_UINT8 c_TypeJpeg = 2;
    static const CLR_UINT8 c_TypeBmp = 3; // The windows .bmp format

    // When m_bitsPerPixel == c_NativeBpp it means that the data in this bitmap is in the native PAL graphics
    // format, the exact bit depth and format is something the CLR is abstracted away from.
    static const int c_NativeBpp = 0;
    static const CLR_INT32 c_MaxWidth = 524287; // 0x7ffff;
    static const CLR_INT32 c_MaxHeight = 65535; // 0x0ffff;

    CLR_UINT32 m_width;
    CLR_UINT32 m_height;
    CLR_UINT16 m_flags;
    CLR_UINT8 m_bitsPerPixel;
    CLR_UINT8 m_type;

    bool BitmapDescription_Initialize(int width, int height, int bitsPerPixel);
    int GetTotalSize() const;
    int GetWidthInWords() const;
};
struct CLR_GFX_FontMetrics
{
    CLR_UINT16 m_height;
    CLR_INT16 m_offset; // The bitmap could be actually smaller than the logical font height.
    CLR_INT16 m_ascent;
    CLR_INT16 m_descent;
    CLR_INT16 m_internalLeading;
    CLR_INT16 m_externalLeading;
    CLR_INT16 m_aveCharWidth;
    CLR_INT16 m_maxCharWidth;
};
struct CLR_GFX_FontMetricsExtended : public CLR_GFX_FontMetrics
{
    int m_offsetX;
    int m_offsetY;
    int m_marginLeft;
    int m_marginRight;
};
struct CLR_GFX_FontDescription
{
    static const CLR_UINT16 c_Bold = 0x0001;
    static const CLR_UINT16 c_Italic = 0x0002;
    static const CLR_UINT16 c_Underline = 0x0004;
    static const CLR_UINT16 c_FontEx = 0x0008;
    static const CLR_UINT16 c_AntiAliasMask = 0x00F0;
    static const CLR_UINT16 c_AntiAliasShift = 4;

    CLR_GFX_FontMetrics m_metrics;
    CLR_UINT16 m_ranges;
    CLR_UINT16 m_characters;
    CLR_UINT16 m_flags;
    CLR_UINT16 m_pad;

    CLR_UINT32 GetRangeSize() const;
    CLR_UINT32 GetCharacterSize() const;
    CLR_UINT32 GetCharacterExSize() const;
    CLR_UINT32 GetRangeExSize() const;
};
struct CLR_GFX_FontDescriptionEx
{
    CLR_UINT32 m_antiAliasSize;
    CLR_UINT32 GetAntiAliasSize() const;
};
struct CLR_GFX_FontCharacterRange
{
    CLR_UINT32 m_indexOfFirstFontCharacter;
    CLR_UINT16 m_firstChar;
    CLR_UINT16 m_lastChar;
    CLR_UINT32 m_rangeOffset; // the x-offset into the bitmap for this range.
};
struct CLR_GFX_FontCharacterRangeEx
{
    CLR_UINT32 m_offsetAntiAlias;
};
struct CLR_GFX_FontCharacter
{
    CLR_UINT16 m_offset;
    CLR_INT8 m_marginLeft;
    CLR_INT8 m_marginRight;
};
struct CLR_GFX_FontCharacterEx
{
    static const CLR_UINT16 c_offsetNoAntiAlias = 0xFFFF;
    CLR_UINT16 m_offsetAntiAlias;
};
struct CLR_GFX_FontCharacterInfo
{
    bool isValid;
    CLR_INT8 marginLeft;
    CLR_INT8 marginRight;
    CLR_UINT32 offset;
    CLR_UINT16 innerWidth;
    CLR_UINT16 width;
    CLR_UINT16 height;
    CLR_UINT8 *antiAlias;
    CLR_UINT8 iAntiAlias;
};
struct GFX_Rect
{
    int left;
    int top;
    int right;
    int bottom;

    int Width() const
    {
        return right - left + 1;
    }
    int Height() const
    {
        return bottom - top + 1;
    }
};
struct GFX_Brush
{
    // GFX_Brush is a structure to capture all information needed to perform a fill.
    // Note:
    // To create a solid brush, make gradientStartColor == gradientEndColor
    // To create a empty (no fill) brush, make opacity = c_ColorTransparent
    // To create a linear gradient brush, fill in the start and end information

    CLR_UINT32 gradientStartColor;
    int gradientStartX;
    int gradientStartY;
    CLR_UINT32 gradientEndColor;
    int gradientEndX;
    int gradientEndY;
    CLR_UINT16 opacity;
};
struct GFX_Pen
{
    CLR_UINT32 color;
    int thickness;
};
struct Draw4PointsRoundedRectParams
{
    int x1;
    int y1;
    int x2;
    int y2;
    GFX_Pen *pen;
    GFX_Brush *brush;
    int lastFillOffsetY;
};
struct Draw4PointsEllipseParams
{
    int centerX;
    int centerY;
    GFX_Pen *pen;
    GFX_Brush *brush;
    int lastFillOffsetY;
};
struct ConvertToNativeHelperParam
{
    CLR_UINT32 *srcFirstWord;
    CLR_UINT32 srcWidthInWords;
    union {
        CLR_UINT32 *srcCur1BppWord;
        CLR_UINT16 *srcCur16BppPixel;
    };
    CLR_UINT32 srcCur1BppPixelMask;
};
struct RADIAN
{
    float cos;
    float sin;
};
struct DivHelper
{
    // DivHelper is a support class used internally to assist calculating gradient fills
    // DivHelper uses the midpoint technique to perform integer division using only addition and few multiplications.
    // This is useful when we need to calculate y = a*x / b + c over a range of x values. All we need is a starting
    // point where the value of y is known.
    // Usage:
    // For example, we have f(x) = d*x / e + f, where we know f(n) = m
    // We construct the DivHelper struct by
    //          DivHelper helper(d, e, m)
    // By calling helper.Next(), we will get the result of f(n), f(n+1), f(n+2) ... etc.
    // Note that if b is 0, Next() will always return initY
    int a;
    int b;
    int D;
    int y;
    int stride;
    int DIncStride;
    int DIncStridePlus1;
    bool first;

    DivHelper(int _a, int _b, int initY)
    {
        // a and b are the constant in the formula y = a*x / b + c
        //       initY is the initial value of Y
        a = _a;
        b = _b;
        if (b != 0)
            stride = a / b;
        DIncStride = 2 * (a - b * stride);
        DIncStridePlus1 = 2 * (a - b * (stride + 1));
        Reset(initY);
    }
    int Next() // return the next y value.
    {
        if (first)
        {
            if (b == 0)
                return y; // if b == 0, first will always be true

            first = false;
            return y;
        }
        if (D > 0)
        {
            D += DIncStride;
            y += stride;
        }
        else
        {
            D += DIncStridePlus1;
            y += stride + 1;
        }
        return y;
    }
    void Reset(int initY) // set a new initY value.
    {
        y = initY;
        D = 2 * a - b * (2 * stride + 1);
        first = true;
    }
};
static const RADIAN radian[] = {
    {1.0f, 0.0f},
    {0.999847695156391f, 0.017452406437284f},
    {0.999390827019096f, 0.034899496702501f},
    {0.998629534754574f, 0.052335956242944f},
    {0.997564050259824f, 0.069756473744125f},
    {0.996194698091746f, 0.087155742747658f},
    {0.994521895368273f, 0.104528463267653f},
    {0.992546151641322f, 0.121869343405147f},
    {0.990268068741570f, 0.139173100960065f},
    {0.987688340595138f, 0.156434465040231f},
    {0.984807753012208f, 0.173648177666930f},
    {0.981627183447664f, 0.190808995376545f},
    {0.978147600733806f, 0.207911690817759f},
    {0.974370064785235f, 0.224951054343865f},
    {0.970295726275996f, 0.241921895599668f},
    {0.965925826289068f, 0.258819045102521f},
    {0.961261695938319f, 0.275637355816999f},
    {0.956304755963035f, 0.292371704722737f},
    {0.951056516295154f, 0.309016994374947f},
    {0.945518575599317f, 0.325568154457157f},
    {0.939692620785908f, 0.342020143325669f},
    {0.933580426497202f, 0.358367949545300f},
    {0.927183854566787f, 0.374606593415912f},
    {0.920504853452440f, 0.390731128489274f},
    {0.913545457642601f, 0.406736643075800f},
    {0.906307787036650f, 0.422618261740699f},
    {0.898794046299167f, 0.438371146789077f},
    {0.891006524188368f, 0.453990499739547f},
    {0.882947592858927f, 0.469471562785891f},
    {0.874619707139396f, 0.484809620246337f},
    {0.866025403784439f, 0.500000000000000f},
    {0.857167300702112f, 0.515038074910054f},
    {0.848048096156426f, 0.529919264233205f},
    {0.838670567945424f, 0.544639035015027f},
    {0.829037572555042f, 0.559192903470747f},
    {0.819152044288992f, 0.573576436351046f},
    {0.809016994374947f, 0.587785252292473f},
    {0.798635510047293f, 0.601815023152048f},
    {0.788010753606722f, 0.615661475325658f},
    {0.777145961456971f, 0.629320391049837f},
    {0.766044443118978f, 0.642787609686539f},
    {0.754709580222772f, 0.656059028990507f},
    {0.743144825477394f, 0.669130606358858f},
    {0.731353701619171f, 0.681998360062498f},
    {0.719339800338651f, 0.694658370458997f},
    {0.707106781186548f, 0.707106781186547f},
    {0.694658370458997f, 0.719339800338651f},
    {0.681998360062498f, 0.731353701619170f},
    {0.669130606358858f, 0.743144825477394f},
    {0.656059028990507f, 0.754709580222772f},
    {0.642787609686539f, 0.766044443118978f},
    {0.629320391049838f, 0.777145961456971f},
    {0.615661475325658f, 0.788010753606722f},
    {0.601815023152048f, 0.798635510047293f},
    {0.587785252292473f, 0.809016994374947f},
    {0.573576436351046f, 0.819152044288992f},
    {0.559192903470747f, 0.829037572555042f},
    {0.544639035015027f, 0.838670567945424f},
    {0.529919264233205f, 0.848048096156426f},
    {0.515038074910054f, 0.857167300702112f},
    {0.500000000000000f, 0.866025403784439f},
    {0.484809620246337f, 0.874619707139396f},
    {0.469471562785891f, 0.882947592858927f},
    {0.453990499739547f, 0.891006524188368f},
    {0.438371146789077f, 0.898794046299167f},
    {0.422618261740699f, 0.906307787036650f},
    {0.406736643075800f, 0.913545457642601f},
    {0.390731128489274f, 0.920504853452440f},
    {0.374606593415912f, 0.927183854566787f},
    {0.358367949545300f, 0.933580426497202f},
    {0.342020143325669f, 0.939692620785908f},
    {0.325568154457157f, 0.945518575599317f},
    {0.309016994374947f, 0.951056516295154f},
    {0.292371704722737f, 0.956304755963035f},
    {0.275637355816999f, 0.961261695938319f},
    {0.258819045102521f, 0.965925826289068f},
    {0.241921895599668f, 0.970295726275996f},
    {0.224951054343865f, 0.974370064785235f},
    {0.207911690817759f, 0.978147600733806f},
    {0.190808995376545f, 0.981627183447664f},
    {0.173648177666930f, 0.984807753012208f},
    {0.156434465040231f, 0.987688340595138f},
    {0.139173100960066f, 0.990268068741570f},
    {0.121869343405147f, 0.992546151641322f},
    {0.104528463267653f, 0.994521895368273f},
    {0.087155742747658f, 0.996194698091746f},
    {0.069756473744125f, 0.997564050259824f},
    {0.052335956242944f, 0.998629534754574f},
    {0.034899496702501f, 0.999390827019096f},
    {0.017452406437283f, 0.999847695156391f},
};
// This is the callback signature for Graphics_SetPixelsHelper(). This callback will be called once for each pixel in
// the area specified. The parameters are as follow: 1) int -- the x coordinates of the current pixel 2) int -- the y
// coordinates of the current pixel 3) CLR_UINT32 -- flags that communicate the status of the SetPixels operation, more
// specifically --
//      c_SetPixels_First           - This is the first time the callback is called, can be used to
//                                    conditionally perform any initialization work
//      c_SetPixels_NewRow          - This is the first pixel of a new row (i.e. new y value)
//      c_SetPixels_PixelHidden     - This pixel is outside of the clipping area. Note that if
//                                    c_SetPixelsConfig_NoClipChecks is set, this bit will never be
//                                    set.
// 4) CLR_UINT16& -- the return value for opacity. Note that you can set this to c_OpacityTransparent if
//               you want to leave the pixel unchanged.
// 5) void* -- this is the custom parameter that was passed into Graphics_SetPixelsHelper(), and it is
//             persisted between each callback call
// Return Value -- The color of the pixel
typedef CLR_UINT32 (*GFX_SetPixelsCallback)(int, int, CLR_UINT32, CLR_UINT16 &, void *);
// Graphics_SetPixelsHelper is designed to allow the caller to fill an area of a bitmap without incurring unnecessary
// cost of calculating the x, y position for each pixel. The parameters are as follow: 1) bitmap -- the target bitmap 2)
// rect -- the area on the bitmap to iterate through 3) config -- configuration for how the callbacks will be made, more
// specifically:
//      c_SetPixelsConfig_NoClip        - No clipping will be done, each pixel in the rect area specified will received
//                                        a callback once in order.
//      c_SetPixelsConfig_Clip          - Clipping will be done against bitmap.clipping, and each pixel in the visible
//      area
//                                        will receive a callback once in order.
//      c_SetPixelsConfig_NoClipChecks  - By default, when c_SetPixelsConfig_NoClip is set, each pixel is passed through
//                                        a series of test to determine if the pixel is visible prior to the callback,
//                                        and the result is reflected in the c_SetPixels_PixelHidden flag. If those
//                                        check are unneccessary, this configuration option will save some time.
//      c_SetPixelsConfig_ReverseY      - The default order of the callback is increasing x, increasing y (top to
//      bottom,
//                                        and within each row, left to right). Setting c_SetPixelsConfig_ReverseY will
//                                        change the order to be increasing x, decreasing y (bottom to top, and within
//                                        each row, left to right).
// 4) param -- a custom pointer that's passed into each callback.
struct PAL_GFX_Bitmap
{
    int width;
    int height;
    CLR_UINT32 *data;
    GFX_Rect clipping;
    CLR_UINT32 transparentColor;

    static const CLR_UINT32 c_InvalidColor = 0xFF000000;
    static const CLR_UINT16 c_OpacityTransparent = 0;
    static const CLR_UINT16 c_OpacityOpaque = 256;
    static const CLR_UINT32 c_SetPixels_None = 0x00000000;
    static const CLR_UINT32 c_SetPixels_First = 0x00000001;
    static const CLR_UINT32 c_SetPixels_NewRow = 0x00000002;
    static const CLR_UINT32 c_SetPixels_PixelHidden = 0x00000004;
    static const CLR_UINT32 c_SetPixelsConfig_NoClip = 0x00000000;
    static const CLR_UINT32 c_SetPixelsConfig_Clip = 0x00000001;
    static const CLR_UINT32 c_SetPixelsConfig_NoClipChecks = 0x00000002;
    static const CLR_UINT32 c_SetPixelsConfig_ReverseY = 0x00000004;

    static void ResetClipping(PAL_GFX_Bitmap &bitmap)
    {
        bitmap.clipping.left = 0;
        bitmap.clipping.top = 0;
        bitmap.clipping.right = bitmap.width;
        bitmap.clipping.bottom = bitmap.height;
    }
};
struct CLR_GFX_Bitmap
{
    static const int FIELD__m_bitmap = 1;

    // These have to be kept in sync with the Microsft.SPOT.Bitmap.DT_ flags
    static const CLR_UINT32 c_DrawText_WordWrap = 0x00000001;
    // Note - 0x00000002 is used for Alignment (look below)
    static const CLR_UINT32 c_DrawText_TruncateAtBottom = 0x00000004;
    // Note - 0x00000008 is used for Trimming (look below)
    static const CLR_UINT32 c_DrawText_IgnoreHeight = 0x00000010;

    static const CLR_UINT32 c_DrawText_AlignmentLeft = 0x00000000;
    static const CLR_UINT32 c_DrawText_AlignmentCenter = 0x00000002;
    static const CLR_UINT32 c_DrawText_AlignmentRight = 0x00000020;
    static const CLR_UINT32 c_DrawText_AlignmentUnused = 0x00000022;
    static const CLR_UINT32 c_DrawText_AlignmentMask = 0x00000022;

    static const CLR_UINT32 c_DrawText_TrimmingNone = 0x00000000;
    static const CLR_UINT32 c_DrawText_TrimmingWordEllipsis = 0x00000008;
    static const CLR_UINT32 c_DrawText_TrimmingCharacterEllipsis = 0x00000040;
    static const CLR_UINT32 c_DrawText_TrimmingUnused = 0x00000048;
    static const CLR_UINT32 c_DrawText_TrimmingMask = 0x00000048;

    CLR_GFX_BitmapDescription m_bm; // Initialized by the caller!
    PAL_GFX_Bitmap m_palBitmap;

    static HRESULT CreateInstance(CLR_RT_HeapBlock &ref, const CLR_GFX_BitmapDescription &bm);
    static HRESULT CreateInstance(CLR_RT_HeapBlock &ref, const CLR_UINT8 *data, CLR_UINT32 size, CLR_RT_Assembly *assm);
    static HRESULT CreateInstance(
        CLR_RT_HeapBlock &ref,
        const CLR_UINT8 *data,
        const CLR_UINT32 size,
        const CLR_UINT8 type);

    static HRESULT CreateInstanceJpeg(CLR_RT_HeapBlock &ref, const CLR_UINT8 *data, const CLR_UINT32 size);
    static HRESULT CreateInstanceGif(CLR_RT_HeapBlock &ref, const CLR_UINT8 *data, const CLR_UINT32 size);
    static HRESULT CreateInstanceBmp(CLR_RT_HeapBlock &ref, const CLR_UINT8 *data, const CLR_UINT32 size);

    static HRESULT GetInstanceFromGraphicsHeapBlock(const CLR_RT_HeapBlock &ref, CLR_GFX_Bitmap *&bitmap);
    static HRESULT DeleteInstance(CLR_RT_HeapBlock &ref);

    static CLR_UINT32 CreateInstanceJpegHelper(int x, int y, CLR_UINT32 flags, CLR_UINT16 &opacity, void *param);
    static CLR_UINT32 ConvertToNative1BppHelper(CLR_UINT32 flags, CLR_UINT16 &opacity, void *param);
    static CLR_UINT32 ConvertToNative16BppHelper(CLR_UINT32 flags, CLR_UINT16 &opacity, void *param);

    void Bitmap_Initialize();
    void Clear();
    void SetClipping(GFX_Rect &rc);

    CLR_UINT32 GetPixel(int xPos, int yPos) const;
    void SetPixel(int xPos, int yPos, CLR_UINT32 color);

    void DrawLine(const GFX_Pen &pen, int x0, int y0, int x1, int y1);
    void DrawRectangle(const GFX_Pen &pen, const GFX_Brush &brush, const GFX_Rect &rectangle);
    void DrawRoundedRectangle(
        const GFX_Pen &pen,
        const GFX_Brush &brush,
        const GFX_Rect &rectangle,
        int radiusX,
        int radiusY);
    void DrawEllipse(const GFX_Pen &pen, const GFX_Brush &brush, int x, int y, int radiusX, int radiusY);

    void DrawImage(const GFX_Rect &dst, const CLR_GFX_Bitmap &bitmapSrc, const GFX_Rect &src, CLR_UINT16 opacity);
    void RotateImage(
        int angle,
        const GFX_Rect &dst,
        const CLR_GFX_Bitmap &bitmapSrc,
        const GFX_Rect &src,
        CLR_UINT16 opacity);

    void DrawText(LPCSTR str, CLR_GFX_Font &font, CLR_UINT32 color, int x, int y);
    static HRESULT DrawTextInRect(
        LPCSTR &szText,
        int &xRelStart,
        int &yRelStart,
        int &renderWidth,
        int &renderHeight,
        CLR_GFX_Bitmap *bm,
        int x,
        int y,
        int width,
        int height,
        CLR_UINT32 dtFlags,
        CLR_UINT32 color,
        CLR_GFX_Font *font);
    void SetPixelsHelper(const GFX_Rect &rect, CLR_UINT32 config, GFX_SetPixelsCallback callback, void *param);

    // Helper routine
    HRESULT GetBitmap(CLR_RT_HeapBlock *pThis, bool fForWrite, CLR_GFX_Bitmap *&bitmap);
    HRESULT GetBitmap(CLR_RT_StackFrame &stack, bool fForWrite, CLR_GFX_Bitmap *&bitmap);

  private:
    void Decompress(const CLR_UINT8 *data, CLR_UINT32 size);
    void ConvertToNative(const CLR_GFX_BitmapDescription *bmSrc, CLR_UINT32 *dataSrc);

    PROHIBIT_ALL_CONSTRUCTORS(CLR_GFX_Bitmap);
};
struct CLR_GFX_Font
{
    static const int FIELD__m_font = 1;
    static const CLR_INT32 c_DefaultKerning = 1024; // Must keep in sync with Microsoft.SPOT.Font.DefaultKerning
    static const CLR_UINT16 c_UnicodeReplacementCharacter = 0xFFFD;

    CLR_GFX_FontDescription m_font;
    CLR_GFX_FontCharacterRange *m_ranges;
    CLR_GFX_FontCharacter *m_chars;
    CLR_GFX_Bitmap m_bitmap;
    CLR_GFX_FontCharacterInfo m_defaultChar;
    CLR_GFX_FontDescriptionEx m_fontEx;
    CLR_GFX_FontCharacterRangeEx *m_rangesEx;
    CLR_GFX_FontCharacterEx *m_charsEx;
    CLR_UINT8 *m_antiAliasingData;

    static HRESULT CreateInstance(CLR_RT_HeapBlock &ref, const CLR_UINT8 *data, CLR_RT_Assembly *assm);
    void Font_Initialize();
    int StringOut(
        LPCSTR str,
        int maxChars,
        CLR_INT32 kerning,
        CLR_GFX_Bitmap *bm,
        int xPos,
        int yPos,
        CLR_UINT32 color);
    void CountCharactersInWidth(
        LPCSTR str,
        int maxChars,
        int width,
        int &totWidth,
        bool fWordWrap,
        LPCSTR &strNext,
        int &numChars);
    void DrawChar(CLR_GFX_Bitmap *bitmap, CLR_GFX_FontCharacterInfo &chr, int xDst, int yDst, CLR_UINT32 color);
    static CLR_UINT32 DrawCharHelper(int x, int y, CLR_UINT32 flags, CLR_UINT16 &opacity, void *param);
    void GetCharInfo(CLR_UINT16 c, CLR_GFX_FontCharacterInfo &chrEx);
    void Relocate();
    static void RelocationHandler(CLR_RT_HeapBlock_BinaryBlob *ptr);

    PROHIBIT_ALL_CONSTRUCTORS(CLR_GFX_Font);
};
struct DrawCharHelperParam
{
    int originalDstX;
    int originalDstY;
    int srcX;
    int srcY;
    CLR_UINT32 *srcFirstWord;
    CLR_UINT32 srcFirstPixelMask;
    CLR_UINT32 srcCurPixelMask;
    CLR_UINT32 *srcCurWord;
    CLR_UINT32 srcWidthInWords;
    CLR_UINT32 color;
    CLR_UINT8 *antiAlias;
    CLR_UINT8 iAntiAlias;
    CLR_UINT8 antiAliasStep;
    CLR_UINT8 antiAliasShift;
    CLR_UINT8 antiAliasShiftFirstPixel;
    CLR_UINT32 antiAliasMask;
    CLR_UINT32 antiAliasMaskFirstPixel;
};
int GetCharInfoCmp(const void *c, const void *r);
struct GraphicsDriver
{
  public:
    static int GetSize(CLR_INT32 width, CLR_INT32 height);
    static int GetWidthInWords(CLR_INT32 width);
    static int GetLongerSide();
    static int GetShorterSide();
    static int GetWidth();
    static int GetHeight();
    static int GetBitsPerPixel();
    static DisplayOrientation GetOrientation();
    static void ChangeOrientation(DisplayOrientation newOrientation);

    static void Clear(const PAL_GFX_Bitmap &bitmap);

    static CLR_UINT32 GetPixel(const PAL_GFX_Bitmap &bitmap, int x, int y);
    static void SetPixel(const PAL_GFX_Bitmap &bitmap, int x, int y, CLR_UINT32 color);
    static void DrawLine(const PAL_GFX_Bitmap &bitmap, const GFX_Pen &pen, int x0, int y0, int x1, int y1);
    static void DrawLineRaw(const PAL_GFX_Bitmap &bitmap, const GFX_Pen &pen, int x0, int y0, int x1, int y1);
    static void DrawRectangle(
        const PAL_GFX_Bitmap &bitmap,
        const GFX_Pen &pen,
        const GFX_Brush &brush,
        const GFX_Rect &rectangle);
    static void DrawRoundedRectangle(
        const PAL_GFX_Bitmap &bitmap,
        const GFX_Pen &pen,
        const GFX_Brush &brush,
        const GFX_Rect &rectangle,
        int radiusX,
        int radiusY);
    static void DrawEllipse(
        const PAL_GFX_Bitmap &bitmap,
        const GFX_Pen &pen,
        const GFX_Brush &brush,
        int x,
        int y,
        int radiusX,
        int radiusY);
    static void DrawImage(
        const PAL_GFX_Bitmap &bitmapDst,
        const GFX_Rect &dst,
        const PAL_GFX_Bitmap &bitmapSrc,
        const GFX_Rect &src,
        CLR_UINT16 opacity);
    static void SetPixelsHelper(
        const PAL_GFX_Bitmap &bitmap,
        const GFX_Rect &rect,
        CLR_UINT32 config,
        GFX_SetPixelsCallback callback,
        void *param);
    static void RotateImage(
        CLR_INT16 degree,
        const PAL_GFX_Bitmap &dst,
        const GFX_Rect &dstRect,
        const PAL_GFX_Bitmap &src,
        const GFX_Rect &srcRect,
        CLR_UINT16 opacity);

    static void Screen_Flush(CLR_GFX_Bitmap &bitmap, CLR_UINT16 x, CLR_UINT16 y, CLR_UINT16 width, CLR_UINT16 height);

  private:
    static const CLR_UINT32 c_MaxSize = 2097152; // 2MB
    static CLR_UINT32 GetPixelNative(const PAL_GFX_Bitmap &bitmap, int x, int y);
    static void SetPixelNative(const PAL_GFX_Bitmap &bitmap, int x, int y, CLR_UINT32 color);
    static void SetThickPixel(const PAL_GFX_Bitmap &bitmap, int x, int y, GFX_Pen &pen);

    static void DrawLineNative(const PAL_GFX_Bitmap &bitmap, GFX_Pen &pen, int x0, int y0, int x1, int y1);
    static void DrawRectangleNative(
        const PAL_GFX_Bitmap &bitmap,
        GFX_Pen &pen,
        GFX_Brush &brush,
        const GFX_Rect &rectangle);
    static void DrawRoundedRectangleNative(
        const PAL_GFX_Bitmap &bitmap,
        GFX_Pen &pen,
        GFX_Brush &brush,
        const GFX_Rect &rectangle,
        int radiusX,
        int radiusY);
    static void DrawRoundedRectangleNative(
        const PAL_GFX_Bitmap &bitmap,
        GFX_Pen &pen,
        const GFX_Rect &rectangle,
        int radiusX,
        int radiusY);
    static void DrawEllipseNative(
        const PAL_GFX_Bitmap &bitmap,
        GFX_Pen &pen,
        GFX_Brush &brush,
        int x,
        int y,
        int radiusX,
        int radiusY);

    static CLR_UINT32 *ComputePosition(
        const PAL_GFX_Bitmap &bitmap,
        int xPos,
        int yPos,
        CLR_UINT32 &mask,
        CLR_UINT32 &shift);

    static bool IsPixelVisible(const GFX_Rect &clipping, int x, int y);
    static bool ClipToVisible(
        const PAL_GFX_Bitmap &target,
        int &x,
        int &y,
        int &width,
        int &height,
        const PAL_GFX_Bitmap *pSrc,
        int &xSrc,
        int &ySrc);

    static void DrawBresLineNative(const PAL_GFX_Bitmap &bitmap, int x0, int y0, int x1, int y1, GFX_Pen &pen);

    static CLR_UINT32 NativeColorInterpolate(CLR_UINT32 colorTo, CLR_UINT32 colorFrom, CLR_UINT16 scalar);

    __inline static CLR_UINT8 NativeColorRValue(CLR_UINT32 color)
    {
        return (color & 0xF800) >> 11;
    }
    __inline static CLR_UINT8 NativeColorGValue(CLR_UINT32 color)
    {
        return (color & 0x07E0) >> 5;
    }
    __inline static CLR_UINT8 NativeColorBValue(CLR_UINT32 color)
    {
        return color & 0x1F;
    }
    __inline static CLR_UINT32 NativeColorFromRGB(CLR_UINT8 r, CLR_UINT8 g, CLR_UINT8 b)
    {
        ASSERT((b <= 0x1F) && (g <= 0x3F) && (r <= 0x1F));
        return (r << 11) | (g << 5) | b;
    }

    __inline static CLR_UINT8 ColorRValue(CLR_UINT32 color)
    {
        return color & 0x0000FF;
    }
    __inline static CLR_UINT8 ColorGValue(CLR_UINT32 color)
    {
        return (color & 0x00FF00) >> 8;
    }
    __inline static CLR_UINT8 ColorBValue(CLR_UINT32 color)
    {
        return (color & 0xFF0000) >> 16;
    }
    __inline static CLR_UINT32 ColorFromRGB(CLR_UINT8 r, CLR_UINT8 g, CLR_UINT8 b)
    {
        return (b << 16) | (g << 8) | r;
    }

    __inline static CLR_UINT32 ConvertNativeToColor(CLR_UINT32 nativeColor)
    {
        int r = NativeColorRValue(nativeColor) << 3;
        if ((r & 0x8) != 0)
            r |= 0x7; // Copy LSB
        int g = NativeColorGValue(nativeColor) << 2;
        if ((g & 0x4) != 0)
            g |= 0x3; // Copy LSB
        int b = NativeColorBValue(nativeColor) << 3;
        if ((b & 0x8) != 0)
            b |= 0x7; // Copy LSB
        return ColorFromRGB(r, g, b);
    }
    __inline static CLR_UINT32 ConvertColorToNative(CLR_UINT32 color)
    {
        return NativeColorFromRGB(ColorRValue(color) >> 3, ColorGValue(color) >> 2, ColorBValue(color) >> 3);
    }

    __inline static GFX_Pen ConvertPenToNative(const GFX_Pen &pen)
    {
        GFX_Pen nativePen;
        nativePen.thickness = pen.thickness;
        nativePen.color = ConvertColorToNative(pen.color);
        return nativePen;
    }

    __inline static GFX_Brush ConvertBrushToNative(const GFX_Brush &brush)
    {
        GFX_Brush nativeBrush;
        nativeBrush.gradientStartX = brush.gradientStartX;
        nativeBrush.gradientStartY = brush.gradientStartY;
        nativeBrush.gradientStartColor = ConvertColorToNative(brush.gradientStartColor);
        nativeBrush.gradientEndX = brush.gradientEndX;
        nativeBrush.gradientEndY = brush.gradientEndY;
        nativeBrush.gradientEndColor = ConvertColorToNative(brush.gradientEndColor);
        nativeBrush.opacity = brush.opacity;
        return nativeBrush;
    }

    typedef void (*EllipseCallback)(const PAL_GFX_Bitmap &, int, int, void *);

    static void EllipseAlgorithm(
        const PAL_GFX_Bitmap &bitmap,
        int radiusX,
        int radiusY,
        void *params,
        EllipseCallback ellipseCallback);

    static void Draw4PointsEllipse(const PAL_GFX_Bitmap &bitmap, int offsetX, int offsetY, void *params);
    static void Draw4PointsRoundedRect(const PAL_GFX_Bitmap &bitmap, int offsetX, int offsetY, void *params);
};

// The PAL Graphics API uses the 24bit BGR color space, the one that's used for the
// CLR. It is the responsibility of whoever is implementing the PAL to deal with color conversion
// as neither CLR or TinyCore understands any color space other than this default one.
// For opacity, the valid value are from 0 (c_OpacityTransparent) to 256 (c_OpacityOpaque).

//_____________________________________
//   Gesture support
//_____________________________________

struct GestureDriver
{
    static const int c_IgnoreCount = 2;

  private:
    static bool s_initialized;

    static PalEventListener m_gestureListener;
    static HAL_COMPLETION m_gestureCompletion;
    static CLR_UINT32 m_index;
    static CLR_UINT32 m_currentState;
    static CLR_UINT16 m_lastx;
    static CLR_UINT16 m_lasty;
    static CLR_UINT16 m_startx;
    static CLR_UINT16 m_starty;

    static CLR_UINT32 m_stateIgnoreIndex;
    static CLR_UINT32 m_stateIgnoreHead;
    static CLR_UINT32 m_stateIgnoreTail;
    static CLR_UINT32 m_stateIgnoreBuffer[c_IgnoreCount];

  public:
    static HRESULT Initialize();
    static HRESULT Uninitialize();
    static bool ProcessPoint(CLR_UINT32 flags, CLR_UINT16 source, CLR_UINT16 x, CLR_UINT16 y, CLR_INT64 time);
    static void ResetRecognition();
    static void EventListener(uint32_t e, uint32_t param);
    static void GestureContinuationRoutine(void *arg);
};
// Bitmap decoder
typedef CLR_UINT32 COLORREF;
enum BmpEncodingType
{
    BmpUnknown = 0,
    Bmp16Bit_565 = 1,
    Bmp24Bit_RGB = 2,
    Bmp8Bit_Indexed = 3,
    Bmp32Bit_ARGB = 4,
};
struct BmpDecoder
{
    int width;
    int height;
    BmpEncodingType encodingType;
    HRESULT BmpInitOutput(const CLR_UINT8 *src, CLR_UINT32 srcSize);
    HRESULT BmpStartOutput(CLR_GFX_Bitmap *bitmap);
    static CLR_UINT32 BmpOutputHelper(CLR_UINT32 flags, CLR_UINT16 &opacity, void *param);

  private:
    CLR_RT_ByteArrayReader source;
    const CLR_UINT8 *palette;
    CLR_UINT8 paletteDepth;
    bool isTopDown;
};
struct BITMAPFILEHEADER
{
    CLR_UINT16 bfType;
    CLR_UINT32 bfSize;
    CLR_UINT16 bfReserved1;
    CLR_UINT16 bfReserved2;
    CLR_UINT32 bfOffBits;
} __attribute__((packed));
struct BITMAPINFOHEADER
{
    CLR_UINT32 biSize;
    CLR_INT32 biWidth;
    CLR_INT32 biHeight;
    CLR_UINT16 biPlanes;
    CLR_UINT16 biBitCount;
    CLR_UINT32 biCompression;
    CLR_UINT32 biSizeImage;
    CLR_INT32 biXPelsPerMeter;
    CLR_INT32 biYPelsPerMeter;
    CLR_UINT32 biClrUsed;
    CLR_UINT32 biClrImportant;
} __attribute__((packed));
struct BmpOutputHelperParam
{
    CLR_RT_ByteArrayReader *source;
    BmpEncodingType encodingType;
    CLR_UINT32 srcWidthInBytes;
    CLR_UINT8 *srcRGB;
    const CLR_UINT8 *palette;
    CLR_UINT8 paletteDepth;
};
#endif // _GRAPHICS_H_
