//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "Graphics.h"
#include "Display.h"

struct GraphicsDriver g_GraphicsDriver;
extern DisplayDriver g_DisplayDriver;

int GraphicsDriver::GetSize(CLR_INT32 width, CLR_INT32 height)
{
    if (width < 32768 && height < 32768)
    {
        // if width and height are both less than 32768 (0x8000), the maximum size would be less than
        // 0x7FFF * 0x7FFF * 4 = 0xFFFC0004, which would fit in UINT32. So no need to do the 64-bit
        // math.
        CLR_UINT32 size = GetWidthInWords(width) * height * sizeof(CLR_UINT32);

        if (size > c_MaxSize)
            return -1;

        return size;
    }
    else
    {
        CLR_UINT64 size = (CLR_UINT64)GetWidthInWords(width) * height * sizeof(CLR_UINT32);

        if (size > (CLR_UINT64)c_MaxSize)
            return -1;

        return (int)size;
    }
}

int GraphicsDriver::GetWidthInWords(CLR_INT32 width)
{
    return (width * 16 + 31) / 32;
}

int GraphicsDriver::GetLongerSide()
{
    return g_DisplayDriver.Attributes.LongerSide;
}

int GraphicsDriver::GetShorterSide()
{
    return g_DisplayDriver.Attributes.ShorterSide;
}

int GraphicsDriver::GetWidth()
{
    return g_DisplayDriver.Attributes.Width;
}

int GraphicsDriver::GetHeight()
{
    return g_DisplayDriver.Attributes.Height;
}

int GraphicsDriver::GetBitsPerPixel()
{
    return g_DisplayDriver.Attributes.BitsPerPixel;
}

DisplayOrientation GraphicsDriver::GetOrientation()
{
    return g_DisplayDriver.Attributes.Orientation;
}

void GraphicsDriver::ChangeOrientation(DisplayOrientation newOrientation)
{
    // Not implemented ( Maybe in the future?)
    UNUSED(newOrientation);
    return;
}

CLR_UINT32 GraphicsDriver::GetPixel(const PAL_GFX_Bitmap &bitmap, int x, int y)
{
    return ConvertNativeToColor(GetPixelNative(bitmap, x, y));
}

void GraphicsDriver::SetPixel(const PAL_GFX_Bitmap &bitmap, int x, int y, CLR_UINT32 color)
{
    SetPixelNative(bitmap, x, y, ConvertColorToNative(color));
}

void GraphicsDriver::DrawLine(const PAL_GFX_Bitmap &bitmap, const GFX_Pen &pen, int x0, int y0, int x1, int y1)
{
    GFX_Pen nativePen = ConvertPenToNative(pen);
    DrawLineNative(bitmap, nativePen, x0, y0, x1, y1);
}

void GraphicsDriver::DrawRectangle(
    const PAL_GFX_Bitmap &bitmap,
    const GFX_Pen &pen,
    const GFX_Brush &brush,
    const GFX_Rect &rectangle)
{
    GFX_Pen nativePen = ConvertPenToNative(pen);
    GFX_Brush nativeBrush = ConvertBrushToNative(brush);
    DrawRectangleNative(bitmap, nativePen, nativeBrush, rectangle);
}

void GraphicsDriver::DrawRoundedRectangle(
    const PAL_GFX_Bitmap &bitmap,
    const GFX_Pen &pen,
    const GFX_Brush &brush,
    const GFX_Rect &rectangle,
    int radiusX,
    int radiusY)
{
    GFX_Pen nativePen = ConvertPenToNative(pen);
    GFX_Brush nativeBrush = ConvertBrushToNative(brush);
    DrawRoundedRectangleNative(bitmap, nativePen, nativeBrush, rectangle, radiusX, radiusY);
}

void GraphicsDriver::DrawEllipse(
    const PAL_GFX_Bitmap &bitmap,
    const GFX_Pen &pen,
    const GFX_Brush &brush,
    int x,
    int y,
    int radiusX,
    int radiusY)
{
    GFX_Pen nativePen = ConvertPenToNative(pen);
    GFX_Brush nativeBrush = ConvertBrushToNative(brush);
    DrawEllipseNative(bitmap, nativePen, nativeBrush, x, y, radiusX, radiusY);
}

void GraphicsDriver::Clear(const PAL_GFX_Bitmap &bitmap)
{
    memset(bitmap.data, 0, GetSize(bitmap.width, bitmap.height));
}

CLR_UINT32 GraphicsDriver::GetPixelNative(const PAL_GFX_Bitmap &bitmap, int x, int y)
{
    if (IsPixelVisible(bitmap.clipping, x, y))
    {
        CLR_UINT32 mask, shift;
        CLR_UINT32 *data = ComputePosition(bitmap, x, y, mask, shift);

        return (*data & mask) >> shift;
    }
    else
    {
        return 0;
    }
}

void GraphicsDriver::SetPixelNative(const PAL_GFX_Bitmap &bitmap, int x, int y, CLR_UINT32 color)
{
    if (IsPixelVisible(bitmap.clipping, x, y))
    {
        CLR_UINT32 mask, shift;
        CLR_UINT32 *data = ComputePosition(bitmap, x, y, mask, shift);

        *data &= ~mask;
        *data |= color << shift;
    }
}

void GraphicsDriver::SetThickPixel(const PAL_GFX_Bitmap &bitmap, int x, int y, GFX_Pen &pen)
{
    int thickness = pen.thickness;
    CLR_UINT32 color = pen.color;

    SetPixelNative(bitmap, x, y, color);

    if (thickness > 1)
    {
        int dx = 0;
        int dy = thickness;

        int rr = thickness * thickness + (thickness / 2);

        while (dx <= dy)
        {
            for (int x2 = x - dx; x2 < x + dx; x2++)
            {
                SetPixelNative(bitmap, x2, y + dy, color);
                SetPixelNative(bitmap, x2, y - dy, color);
            }

            for (int x2 = x - dy; x2 < x + dy; x2++)
            {
                SetPixelNative(bitmap, x2, y + dx, color);
                SetPixelNative(bitmap, x2, y - dx, color);
            }

            if (dx * dx + dy * dy > rr)
            {
                dy--;
            }

            dx++;
        }
    }
}

void GraphicsDriver::DrawLineRaw(const PAL_GFX_Bitmap &bitmap, const GFX_Pen &pen, int x0, int y0, int x1, int y1)
{
    GFX_Pen nativePen = ConvertPenToNative(pen);
    //  nativePen.color = g_DisplayDriver.ConvertColor(nativePen.color);

    // TODO: thickness
    // thickness value is ignore -- we always assume it's 1 pixel, for now
    DrawBresLineNative(bitmap, x0, y0, x1, y1, nativePen);
}

void GraphicsDriver::DrawLineNative(const PAL_GFX_Bitmap &bitmap, GFX_Pen &pen, int x0, int y0, int x1, int y1)
{
    // TODO: thickness
    // thickness value is ignore -- we always assume it's 1 pixel, for now
    DrawBresLineNative(bitmap, x0, y0, x1, y1, pen);
}

void GraphicsDriver::DrawRectangleNative(
    const PAL_GFX_Bitmap &bitmap,
    GFX_Pen &pen,
    GFX_Brush &brush,
    const GFX_Rect &rectangle)
{
    if (rectangle.Width() <= 0 || rectangle.Height() <= 0)
        return;

    int x, y;

    x = rectangle.left;
    y = rectangle.top;

    int outset = pen.thickness / 2;
    int inset = pen.thickness - outset;

    int outsetX = x - outset;
    int outsetY = y - outset;
    int outsetWidth = rectangle.Width() + 2 * outset;
    int outsetHeight = rectangle.Height() + 2 * outset;

    int insetX = x + inset;
    int insetY = y + inset;
    int insetWidth = rectangle.Width() - 2 * inset;
    int insetHeight = rectangle.Height() - 2 * inset;

    int stride = GetWidthInWords(bitmap.width) * 2;

    const CLR_UINT16 opacity = brush.opacity;

    int xSrc = 0, ySrc = 0;
    // If the outset rect is completely outside of the drawing region, we can safely return (inset rect is always inside
    // the outset rect)
    if (ClipToVisible(bitmap, outsetX, outsetY, outsetWidth, outsetHeight, NULL, xSrc, ySrc) == false)
        return;

    // If the inset rectangle is completely outside of the drawing region, the insetWidth and insetHeight would be set
    // to 0 (We have to keep going because the outset rect is at least partly visible)
    ClipToVisible(bitmap, insetX, insetY, insetWidth, insetHeight, NULL, xSrc, ySrc);

    // Outline
    if (pen.thickness > 0)
    {
        CLR_UINT16 *curRow = ((CLR_UINT16 *)bitmap.data) + outsetY * stride + outsetX;
        CLR_UINT16 *curPixel;
        for (int row = outsetY; row < outsetY + outsetHeight; row++, curRow += stride)
        {
            curPixel = curRow;
            if (row >= insetY && row < insetY + insetHeight)
            {
                for (int col = outsetX; col < insetX; col++, curPixel++)
                {
                    *curPixel = pen.color;
                }

                curPixel += insetWidth;
                for (int col = insetX + insetWidth; col < outsetX + outsetWidth; col++, curPixel++)
                {
                    *curPixel = pen.color;
                }
            }
            else
            {
                for (int col = outsetX; col < outsetX + outsetWidth; col++, curPixel++)
                {
                    *curPixel = pen.color;
                }
            }
        }
    }

    // Fills (Gradient / Translucent / Solid)
    if (opacity != 0)
    {
        int gradientDeltaY = brush.gradientEndY - brush.gradientStartY;
        int gradientDeltaX = brush.gradientEndX - brush.gradientStartX;

        if (brush.gradientStartColor == brush.gradientEndColor || (gradientDeltaX == 0 && gradientDeltaY == 0))
        {
            // Solid fill (including Translucent fill)
            CLR_UINT32 fillColor = brush.gradientStartColor;

            if (insetWidth > 0 && insetHeight > 0)
            {
                CLR_UINT16 *curRow = ((CLR_UINT16 *)bitmap.data) + insetY * stride + insetX;
                CLR_UINT16 *curPixel = curRow;

                if (opacity == PAL_GFX_Bitmap::c_OpacityOpaque)
                {
                    for (int row = 0; row < insetHeight; row++, curRow += stride)
                    {
                        curPixel = curRow;
                        for (int col = 0; col < insetWidth; col++, curPixel++)
                        {
                            *curPixel = fillColor;
                        }
                    }
                }
                else // if (opacity != PAL_GFX_Bitmap::c_OpacityOpaque)
                {
                    CLR_UINT16 lastPixel = *curPixel;
                    CLR_UINT16 interpolated = g_GraphicsDriver.NativeColorInterpolate(fillColor, lastPixel, opacity);
                    for (int row = 0; row < insetHeight; row++, curRow += stride)
                    {
                        curPixel = curRow;
                        for (int col = 0; col < insetWidth; col++, curPixel++)
                        {
                            if (*curPixel != lastPixel)
                            {
                                lastPixel = *curPixel;
                                interpolated = g_GraphicsDriver.NativeColorInterpolate(fillColor, lastPixel, opacity);
                            }
                            *curPixel = interpolated;
                        }
                    }
                }
            }
        }
        else
        {
            // Gradient Fill
            if (gradientDeltaX < 0 || ((gradientDeltaX == 0) && gradientDeltaY < 0))
            {
                // Make sure the gradient always go from left to right (x is always increasing)
                // And if it's vertical (no change in x), from top to bottom.
                CLR_UINT32 temp;

                temp = brush.gradientEndX;
                brush.gradientEndX = brush.gradientStartX;
                brush.gradientStartX = temp;

                temp = brush.gradientEndY;
                brush.gradientEndY = brush.gradientStartY;
                brush.gradientStartY = temp;

                temp = brush.gradientEndColor;
                brush.gradientEndColor = brush.gradientStartColor;
                brush.gradientStartColor = temp;

                gradientDeltaX *= -1; // so gradientDeltaX is always >= 0
            }

            if (gradientDeltaY < 0)
                gradientDeltaY *= -1;

            const int LIMIT = 1 << 12;

            CLR_UINT16 *curRow = ((CLR_UINT16 *)bitmap.data) + insetY * stride + insetX;

            CLR_UINT32 scalar = (gradientDeltaY * gradientDeltaY * LIMIT) /
                                (gradientDeltaY * gradientDeltaY + gradientDeltaX * gradientDeltaX);

            CLR_UINT16 colorCache[257];
            bool colorCacheValid[257];

            for (int i = 0; i < 257; i++)
            {
                colorCacheValid[i] = false;
            }

            int scaleGradientTopLeft;
            int scaleGradientTopRight;
            int scaleGradientBottomLeft;
            int gradientTopLeftX;
            int gradientTopLeftY;

            if (brush.gradientEndY > brush.gradientStartY)
            {
                scaleGradientTopLeft = 0;
                scaleGradientTopRight = LIMIT - scalar;
                scaleGradientBottomLeft = scalar;
                gradientTopLeftX = brush.gradientStartX - insetX;
                gradientTopLeftY = brush.gradientStartY - insetY;
            }
            else
            {
                scaleGradientTopLeft = scalar;
                scaleGradientTopRight = LIMIT;
                scaleGradientBottomLeft = 0;
                gradientTopLeftX = brush.gradientStartX - insetX;
                gradientTopLeftY = brush.gradientEndY - insetY;
            }

            int diffX = 0;
            int diffY = 0;

            if (gradientDeltaX != 0)
                diffX = (scaleGradientTopRight - scaleGradientTopLeft) * gradientTopLeftX / gradientDeltaX;
            if (gradientDeltaY != 0)
                diffY = (scaleGradientBottomLeft - scaleGradientTopLeft) * gradientTopLeftY / gradientDeltaY;

            int scaleLeft = scaleGradientTopLeft - diffX - diffY;

            DivHelper widthDivHelper(scaleGradientTopRight - scaleGradientTopLeft, gradientDeltaX, 0);
            DivHelper heightDivHelper(scaleGradientBottomLeft - scaleGradientTopLeft, gradientDeltaY, scaleLeft);

            for (int j = 0; j < insetHeight; j++, curRow += stride)
            {
                widthDivHelper.Reset(heightDivHelper.Next());

                CLR_UINT16 *curPixel = curRow;
                for (int i = 0; i < insetWidth; i++, curPixel++)
                {
                    int scale = widthDivHelper.Next();

                    if (scale < 0)
                    {
                        *curPixel = (opacity != PAL_GFX_Bitmap::c_OpacityOpaque)
                                        ? NativeColorInterpolate(brush.gradientStartColor, *curPixel, opacity)
                                        : brush.gradientStartColor;
                    }
                    else if (scale > LIMIT)
                    {
                        *curPixel = (opacity != PAL_GFX_Bitmap::c_OpacityOpaque)
                                        ? NativeColorInterpolate(brush.gradientEndColor, *curPixel, opacity)
                                        : brush.gradientEndColor;
                    }
                    else
                    {
                        int scale256 = scale * 256 / LIMIT;

                        if (colorCacheValid[scale256] == false)
                        {
                            colorCache[scale256] =
                                NativeColorInterpolate(brush.gradientEndColor, brush.gradientStartColor, scale256);
                            colorCacheValid[scale256] = true;
                        }

                        if (opacity != PAL_GFX_Bitmap::c_OpacityOpaque)
                        {
                            *curPixel = NativeColorInterpolate(colorCache[scale256], *curPixel, opacity);
                        }
                        else
                        {
                            *curPixel = colorCache[scale256];
                        }
                    }
                }
            }
        }
    } //(opacity != 0)
}

void GraphicsDriver::DrawRoundedRectangleNative(
    const PAL_GFX_Bitmap &bitmap,
    GFX_Pen &pen,
    GFX_Brush &brush,
    const GFX_Rect &rectangle,
    int radiusX,
    int radiusY)
{
    if (brush.opacity == 0)
    {
    }; // eliminate unused variable, maybe used in future?
    if (rectangle.Width() <= 0 || rectangle.Height() <= 0 || radiusX < 0 || radiusY < 0)
        return;

    int x, y, x2, y2;

    x = rectangle.left;
    y = rectangle.top;
    x2 = rectangle.right;
    y2 = rectangle.bottom;

    if (radiusX > 0 && radiusY > 0)
    {
        // Set the coordinates to reflect the centers of 4 quarter circles
        Draw4PointsRoundedRectParams params;

        params.x1 = x + radiusX;
        params.y1 = y + radiusY;
        params.x2 = x2 - radiusX;
        params.y2 = y2 - radiusY;

        // if the corners ended up overlapping, then don't do anything
        if (params.x2 < params.x1 || params.y2 < params.y1)
            return;

        params.pen = &pen;
        params.brush = NULL;

        EllipseAlgorithm(bitmap, radiusX, radiusY, &params, &Draw4PointsRoundedRect);

        DrawBresLineNative(bitmap, params.x1, y, params.x2, y, pen);
        DrawBresLineNative(bitmap, x, params.y1, x, params.y2, pen);
        DrawBresLineNative(bitmap, x2, params.y1, x2, params.y2, pen);
        DrawBresLineNative(bitmap, params.x1, y2, params.x2, y2, pen);

        // TODO - fill rounded rectangle
    }
}

void GraphicsDriver::Draw4PointsRoundedRect(const PAL_GFX_Bitmap &bitmap, int offsetX, int offsetY, void *params)
{
    Draw4PointsRoundedRectParams *p = (Draw4PointsRoundedRectParams *)params;

    if (p->pen != NULL)
    {
        if (p->pen->thickness < 2)
        {
            /// For thickness == 1 use the faster path.
            CLR_UINT32 color = p->pen->color;

            SetPixelNative(bitmap, p->x2 + offsetX, p->y2 + offsetY, color);
            SetPixelNative(bitmap, p->x1 - offsetX, p->y2 + offsetY, color);
            SetPixelNative(bitmap, p->x2 + offsetX, p->y1 - offsetY, color);
            SetPixelNative(bitmap, p->x1 - offsetX, p->y1 - offsetY, color);
        }
        else
        {
            SetThickPixel(bitmap, p->x2 + offsetX, p->y2 + offsetY, *p->pen);
            SetThickPixel(bitmap, p->x1 - offsetX, p->y2 + offsetY, *p->pen);
            SetThickPixel(bitmap, p->x2 + offsetX, p->y1 - offsetY, *p->pen);
            SetThickPixel(bitmap, p->x1 - offsetX, p->y1 - offsetY, *p->pen);
        }
    }
}

void GraphicsDriver::DrawEllipseNative(
    const PAL_GFX_Bitmap &bitmap,
    GFX_Pen &pen,
    GFX_Brush &brush,
    int x,
    int y,
    int radiusX,
    int radiusY)
{
    Draw4PointsEllipseParams params;

    params.centerX = x;
    params.centerY = y;

    /// If Fill is expected, then do fill part first before drawing outline.
    if (brush.opacity != PAL_GFX_Bitmap::c_OpacityTransparent)
    {
        params.pen = NULL;
        params.brush = &brush;
        params.lastFillOffsetY = -1;
        EllipseAlgorithm(bitmap, radiusX, radiusY, &params, &Draw4PointsEllipse);
    }

    params.pen = &pen;
    params.brush = NULL;

    EllipseAlgorithm(bitmap, radiusX, radiusY, &params, &Draw4PointsEllipse);
}

void GraphicsDriver::Draw4PointsEllipse(const PAL_GFX_Bitmap &bitmap, int offsetX, int offsetY, void *params)
{
    Draw4PointsEllipseParams *p = (Draw4PointsEllipseParams *)params;
    int centerX = p->centerX;
    int centerY = p->centerY;

    if (p->pen != NULL)
    {
        if (p->pen->thickness < 2)
        {
            /// For thickness == 1 use the faster path.
            CLR_UINT32 color = p->pen->color;

            SetPixelNative(bitmap, centerX + offsetX, centerY + offsetY, color);
            SetPixelNative(bitmap, centerX - offsetX, centerY + offsetY, color);
            SetPixelNative(bitmap, centerX + offsetX, centerY - offsetY, color);
            SetPixelNative(bitmap, centerX - offsetX, centerY - offsetY, color);
        }
        else
        {
            SetThickPixel(bitmap, centerX + offsetX, centerY + offsetY, *p->pen);
            SetThickPixel(bitmap, centerX - offsetX, centerY + offsetY, *p->pen);
            SetThickPixel(bitmap, centerX + offsetX, centerY - offsetY, *p->pen);
            SetThickPixel(bitmap, centerX - offsetX, centerY - offsetY, *p->pen);
        }
    }

    if (p->brush != NULL)
    {
        int opacity = p->brush->opacity;

        /// Fill only if this is not a NULL_BRUSH and we have not painted this scan line already.
        if ((opacity != PAL_GFX_Bitmap::c_OpacityTransparent) && (p->lastFillOffsetY != offsetY))
        {
            int gradientDeltaY = p->brush->gradientEndY - p->brush->gradientStartY;
            int gradientDeltaX = p->brush->gradientEndX - p->brush->gradientStartX;

            int startX = centerX - offsetX + 1;
            int endX = centerX + offsetX;
            int startY[2] = {centerY - offsetY, centerY + offsetY};
            // int stride = GetWidthInWords(bitmap.width) * 2;

            p->lastFillOffsetY = offsetY;

            if (p->brush->gradientStartColor == p->brush->gradientEndColor ||
                (gradientDeltaX == 0 && gradientDeltaY == 0))
            {
                // Solid fill (including Translucent fill)
                CLR_UINT32 fillColor = p->brush->gradientStartColor;

                for (int r = 0; r < 2; r++)
                {
                    if (opacity == PAL_GFX_Bitmap::c_OpacityOpaque)
                    {
                        for (int col = startX; col < endX; col++)
                        {
                            SetPixelNative(bitmap, col, startY[r], fillColor);
                        }
                    }
                    else // if (opacity != PAL_GFX_Bitmap::c_OpacityOpaque)
                    {
                        CLR_UINT16 lastPixel = 0;
                        CLR_UINT16 interpolated = 0;
                        for (int col = startX; col < endX; col++)
                        {
                            lastPixel = g_GraphicsDriver.GetPixelNative(bitmap, col, startY[r]);
                            interpolated = NativeColorInterpolate(fillColor, lastPixel, opacity);
                            SetPixelNative(bitmap, col, startY[r], interpolated);
                        }
                    }

                    /// Do only once when y offset shift is zero.
                    if (offsetY == 0)
                        break;
                }
            }
        }
    }
}

void GraphicsDriver::RotateImage(
    CLR_INT16 degree,
    const PAL_GFX_Bitmap &dst,
    const GFX_Rect &dstRect,
    const PAL_GFX_Bitmap &src,
    const GFX_Rect &srcRect,
    CLR_UINT16 opacity)
{
    float cos, sin;
    int yMin, yMax, xMin, xMax;
    int index;

    degree %= 360;
    if (degree < 0)
        degree = 360 + degree;

    // If it's just a translation, do the BitBlt instead
    if (degree == 0)
    {
        DrawImage(dst, dstRect, src, srcRect, opacity);
        return;
    }

    // Assumption 2 bytes per pixel
    CLR_UINT32 destinationMemoryBlockStart = (CLR_UINT32)&dst.data;
    CLR_UINT32 destinationMemoryBlockEnd = destinationMemoryBlockStart + dst.height * dst.width * GetBitsPerPixel() / 8;

    CLR_UINT32 sourceMemoryBlockStart = (CLR_UINT32)&src.data;
    CLR_UINT32 sourceMemoryBlockEnd = sourceMemoryBlockStart + src.height * src.width * GetBitsPerPixel() / 8;

    index = (degree % 90);

    cos = (radian[index].cos);
    sin = (radian[index].sin);

    if (degree >= 270)
    {
        float tmp = cos;
        cos = sin;
        sin = -tmp;
    }
    else if (degree >= 180)
    {
        cos = -cos;
        sin = -sin;
    }
    else if (degree >= 90)
    {
        float tmp = cos;
        cos = -sin;
        sin = tmp;
    }

    float xCenterSrc = (srcRect.right + srcRect.left) / 2.0f;
    float yCenterSrc = (srcRect.top + srcRect.bottom) / 2.0f;
    float xCenterDst = (dstRect.right + dstRect.left) / 2.0f;
    float yCenterDst = (dstRect.top + dstRect.bottom) / 2.0f;

    xMin = dstRect.left;
    xMax = dstRect.right;

    yMin = dstRect.top;
    yMax = dstRect.bottom;

    for (int y = yMin; y <= yMax; y++)
    {
        float dy = (y - yCenterDst);

        float cosY = cos * dy;
        float sinY = sin * dy;

        for (int x = xMin; x <= xMax; x++)
        {
            float dx = (x - xCenterDst);

            int xSrc = (int)(xCenterSrc + ((dx * cos) + sinY) + 0.5f);
            int ySrc = (int)(yCenterSrc + (cosY - (dx * sin)) + 0.5f);
            if (xSrc <= srcRect.right && srcRect.left <= xSrc && ySrc <= srcRect.bottom && srcRect.top <= ySrc &&
                ySrc > 0 && xSrc > 0)
            {
                CLR_UINT32 maskDst, shiftDst;
                CLR_UINT32 *pbyteDst = ComputePosition(dst, x, y, maskDst, shiftDst);

                if ((CLR_UINT32)pbyteDst >= destinationMemoryBlockStart &&
                    (CLR_UINT32)pbyteDst <= destinationMemoryBlockEnd)
                {
                    CLR_UINT32 mask, shift;
                    CLR_UINT32 *pbyteSrc = ComputePosition(src, xSrc, ySrc, mask, shift);
                    if ((CLR_UINT32)pbyteSrc >= sourceMemoryBlockStart && (CLR_UINT32)pbyteSrc <= sourceMemoryBlockEnd)
                    {
                        *pbyteDst &= ~maskDst;
                        *pbyteDst |= ((*pbyteSrc & mask) >> shift) << shiftDst;
                    }
                }
            }
        }
    }
}

void GraphicsDriver::DrawImage(
    const PAL_GFX_Bitmap &bitmapDst,
    const GFX_Rect &dst,
    const PAL_GFX_Bitmap &bitmapSrc,
    const GFX_Rect &src,
    CLR_UINT16 opacity)
{
    if (opacity == PAL_GFX_Bitmap::c_OpacityTransparent)
        return; // No-op for opacity == 0

    if (dst.Width() == src.Width() && dst.Height() == src.Height())
    {
        int xDst = dst.left;
        int yDst = dst.top;
        int nWidth = dst.Width();
        int nHeight = dst.Height();
        int xSrc = src.left;
        int ySrc = src.top;

        if (g_GraphicsDriver.ClipToVisible(bitmapDst, xDst, yDst, nWidth, nHeight, &bitmapSrc, xSrc, ySrc) == false)
            return;

        int strideSrc = g_GraphicsDriver.GetWidthInWords(bitmapSrc.width) * 2;
        int strideDst = g_GraphicsDriver.GetWidthInWords(bitmapDst.width) * 2;

        CLR_UINT16 *ps = (CLR_UINT16 *)bitmapSrc.data + (strideSrc * ySrc) + xSrc;
        CLR_UINT16 *pd = (CLR_UINT16 *)bitmapDst.data + (strideDst * yDst) + xDst;

        static const int Transparent = 0x1;
        static const int Translucent = 0x2;

        static const int NotTranslucent_NotTransparent = 0x0;
        static const int NotTranslucent_Transparent = Transparent;
        static const int Translucent_NotTransparent = Translucent;
        static const int Translucent_Transparent = Translucent | Transparent;

        int type = 0x0;
        CLR_UINT32 nativeTransparentColor = 0;
        if (bitmapSrc.transparentColor != PAL_GFX_Bitmap::c_InvalidColor)
        {
            type |= Transparent;
            nativeTransparentColor = g_GraphicsDriver.ConvertColorToNative(bitmapSrc.transparentColor);
        }
        if (opacity != PAL_GFX_Bitmap::c_OpacityOpaque)
        {
            type |= Translucent;
        }

        if (type == NotTranslucent_NotTransparent)
        {
            while (nHeight-- > 0)
            {
                memcpy(pd, ps, (CLR_UINT16)(nWidth * 2));
                pd += strideDst;
                ps += strideSrc;
            }
        }
        else
        {
            while (nHeight-- > 0)
            {
                // We're translucent or transparent or both
                for (int col = 0; col < nWidth; col++, ps++, pd++)
                {
                    CLR_UINT16 sourceColor = *ps;

                    switch (type)
                    {
                        case NotTranslucent_Transparent:
                            if (sourceColor != nativeTransparentColor)
                            {
                                *pd = sourceColor;
                            }
                            break;

                        case Translucent_NotTransparent:
                            *pd = g_GraphicsDriver.NativeColorInterpolate(sourceColor, *pd, opacity);
                            break;

                        case Translucent_Transparent:
                            if (sourceColor != nativeTransparentColor)
                            {
                                *pd = g_GraphicsDriver.NativeColorInterpolate(sourceColor, *pd, opacity);
                            }
                            break;
                    }
                }
                pd += strideDst - nWidth;
                ps += strideSrc - nWidth;
            }
        }
    }
    else // StretchImage
    {
        int sourceWidth = src.Width();
        int sourceHeight = src.Height();
        int dstWidth = dst.Width();
        int dstHeight = dst.Height();
        int xDst = dst.left;
        int yDst = dst.top;

        int xSrc = src.left;
        int ySrc = src.top;

        if (sourceWidth <= 0 || sourceHeight <= 0)
            return;

        int strideSrc = g_GraphicsDriver.GetWidthInWords(bitmapSrc.width) * 2;
        int strideDst = g_GraphicsDriver.GetWidthInWords(bitmapDst.width) * 2;

        CLR_UINT16 *ps = (CLR_UINT16 *)bitmapSrc.data + (strideSrc * ySrc) + xSrc;
        CLR_UINT16 *rowstart = ps;

        // Clip left
        //
        int croppedWidth = dstWidth;
        int xOffset = xDst;
        int xSourceOffset = 0;
        int xNumeratorStart = 0;
        if (xOffset < bitmapDst.clipping.left)
        {
            int deltaX = bitmapDst.clipping.left - xOffset;
            croppedWidth -= deltaX;
            xNumeratorStart += sourceWidth * deltaX;
            xSourceOffset = xNumeratorStart / dstWidth;
            xNumeratorStart %= dstWidth;
            xOffset = bitmapDst.clipping.left;
        }

        // Clip right
        //
        if (xOffset + croppedWidth > bitmapDst.clipping.right)
        {
            croppedWidth = bitmapDst.clipping.right - xOffset;
        }

        // Clip top
        //
        int croppedHeight = dstHeight;
        int yOffset = yDst;
        int ySourceOffset = 0;
        int yNumerator = 0;
        if (yOffset < bitmapDst.clipping.top)
        {
            int deltaY = bitmapDst.clipping.top - yOffset;
            croppedHeight -= deltaY;
            yNumerator += sourceHeight * deltaY;
            ySourceOffset = yNumerator / dstHeight;
            yNumerator %= dstHeight;
            yOffset = bitmapDst.clipping.top;
            rowstart += strideSrc * ySourceOffset;
        }

        // Clip bottom
        //
        if (yOffset + croppedHeight > bitmapDst.clipping.bottom)
        {
            croppedHeight = bitmapDst.clipping.bottom - yOffset;
        }

        if (croppedWidth <= 0 || croppedHeight <= 0)
            return;

        CLR_UINT16 *pd = (CLR_UINT16 *)bitmapDst.data + (strideDst * yOffset) + xOffset;

        for (int y = 0; y < croppedHeight; y++, pd += strideDst)
        {
            int xNumerator = xNumeratorStart;

            ps = rowstart + xSourceOffset;

            CLR_UINT16 color = *ps;
            CLR_UINT32 nativeTransparentColor = g_GraphicsDriver.ConvertColorToNative(bitmapSrc.transparentColor);
            bool noTransparent = bitmapSrc.transparentColor == PAL_GFX_Bitmap::c_InvalidColor;
            bool transparent = (noTransparent == false) && (color == nativeTransparentColor);

            for (int x = 0; x < croppedWidth; x++, pd++)
            {
                if (!transparent)
                {
                    *pd = (opacity == PAL_GFX_Bitmap::c_OpacityOpaque)
                              ? color
                              : g_GraphicsDriver.NativeColorInterpolate(color, *pd, opacity);
                }

                xNumerator += sourceWidth;
                while (xNumerator >= dstWidth)
                {
                    xNumerator -= dstWidth;
                    color = *(++ps);
                    transparent = (noTransparent == false) && (color == nativeTransparentColor);
                }
            }
            pd -= croppedWidth;

            yNumerator += sourceHeight;
            while (yNumerator >= dstHeight)
            {
                yNumerator -= dstHeight;
                rowstart += strideSrc;
            }
        }
    }
}

CLR_UINT32 *GraphicsDriver::ComputePosition(
    const PAL_GFX_Bitmap &bitmap,
    int x,
    int y,
    CLR_UINT32 &mask,
    CLR_UINT32 &shift)
{
    shift = (x % 2) * 16;
    mask = 0x0000FFFF << shift;
    return &(bitmap.data[y * GetWidthInWords(bitmap.width) + (x / 2)]);
}

bool GraphicsDriver::IsPixelVisible(const GFX_Rect &clipping, int x, int y)
{
    return (x >= clipping.left && x < clipping.right) && (y >= clipping.top && y < clipping.bottom);
}

bool GraphicsDriver::ClipToVisible(
    const PAL_GFX_Bitmap &target,
    int &x,
    int &y,
    int &width,
    int &height,
    const PAL_GFX_Bitmap *pSrc,
    int &xSrc,
    int &ySrc)
{
    if (pSrc != NULL)
    {
        if (xSrc < 0 || ySrc < 0)
            return false;

        int widthSrc = pSrc->width - xSrc;
        int heightSrc = pSrc->height - ySrc;

        if (width == -1)
            width = pSrc->width;
        if (height == -1)
            height = pSrc->height;

        if (width > widthSrc)
            width = widthSrc;
        if (height > heightSrc)
            height = heightSrc;
    }

    int xEnd = x + width;
    int yEnd = y + height;

    //
    // Totally outside the clipping region?
    //
    if (xEnd < target.clipping.left || x >= target.clipping.right || yEnd < target.clipping.top ||
        y >= target.clipping.bottom)
    {
        width = 0;
        height = 0;
        return false;
    }

    //--//

    if (x < target.clipping.left)
    {
        int nDelta = target.clipping.left - x;

        x += nDelta;
        xSrc += nDelta;
        width -= nDelta;
    }

    if (xEnd > target.clipping.right)
    {
        width = target.clipping.right - x;
    }

    //--//

    if (y < target.clipping.top)
    {
        int nDelta = target.clipping.top - y;

        y += nDelta;
        ySrc += nDelta;
        height -= nDelta;
    }

    if (yEnd > target.clipping.bottom)
    {
        height = target.clipping.bottom - y;
    }

    if (width <= 0 || height <= 0)
        return false;

    return true;
}

CLR_UINT32 GraphicsDriver::NativeColorInterpolate(CLR_UINT32 colorTo, CLR_UINT32 colorFrom, CLR_UINT16 scalar)
{
    // truncate to maximum acceptable value to implement a tolerant behaviour
    if (scalar > PAL_GFX_Bitmap::c_OpacityOpaque)
        scalar = PAL_GFX_Bitmap::c_OpacityOpaque;

    if (scalar == PAL_GFX_Bitmap::c_OpacityOpaque)
        return colorTo;
    if (scalar == PAL_GFX_Bitmap::c_OpacityTransparent)
        return colorFrom;

    CLR_UINT32 rT = NativeColorRValue(colorTo) * scalar;
    CLR_UINT32 gT = NativeColorGValue(colorTo) * scalar;
    CLR_UINT32 bT = NativeColorBValue(colorTo) * scalar;

    CLR_UINT16 invertScalar = PAL_GFX_Bitmap::c_OpacityOpaque - scalar;

    CLR_UINT32 rF = NativeColorRValue(colorFrom) * invertScalar;
    CLR_UINT32 gF = NativeColorGValue(colorFrom) * invertScalar;
    CLR_UINT32 bF = NativeColorBValue(colorFrom) * invertScalar;

    ASSERT(((rT + rF) >> 8) <= 0x1F);
    ASSERT(((gT + gF) >> 8) <= 0x3F);
    ASSERT(((bT + bF) >> 8) <= 0x1F);

    return NativeColorFromRGB((rT + rF) >> 8, (gT + gF) >> 8, (bT + bF) >> 8);
}

void GraphicsDriver::DrawBresLineNative(const PAL_GFX_Bitmap &bitmap, int x0, int y0, int x1, int y1, GFX_Pen &pen)
{
    int dPr;
    int dPru;
    int P;
    int dx;
    int dy;
    int incrementX;
    int incrementY;
    int thickness = pen.thickness;
    CLR_UINT32 color = pen.color;

    // RWOLFF - this can be very slow if outside clipping bounds
    bool fValidX = x0 >= bitmap.clipping.left && x0 < bitmap.clipping.right;
    bool fValidY = y0 >= bitmap.clipping.top && y0 < bitmap.clipping.bottom;
    CLR_UINT32 mask, shift;
    CLR_UINT32 *data;

    dx = abs(x1 - x0); // store the change in X and Y of the line endpoints
    dy = abs(y1 - y0);

    incrementX = (x0 > x1) ? -1 : 1;
    incrementY = (y0 > y1) ? -1 : 1;

    /// Pixels are rectangular rather than circular lets adjust approximate
    /// width for lines that are not horizontal or vertical.
    if (thickness > 2)
    {
        int m1 = dx;
        int m2 = dy;
        int r = 0;
        int t = thickness;
        if (dy > dx)
        {
            m1 = dy;
            m2 = dx;
        }

        if (m2 > 0)
        {
            r = m1 / m2;

            if (r < 2)
            {
                thickness = (t * 2) / 3;
            }
            else if (r < 4)
            {
                thickness = (t * 4) / 5;
            }
        }
    }

    if (dx >= dy)
    {
        dPr = dy << 1;          // amount to increment decision if right is chosen (always)
        dPru = dPr - (dx << 1); // amount to increment decision if up is chosen
        P = dPr - dx;           // decision variable start value

        for (; dx >= 0; dx--)
        {
            if (fValidX && fValidY)
            {
                if (thickness > 1)
                {
                    SetThickPixel(bitmap, x0, y0, pen);
                }
                else
                {
                    data = ComputePosition(bitmap, x0, y0, mask, shift);

                    *data &= ~mask;
                    *data |= color << shift;
                }
            }

            if (P > 0)
            {
                x0 += incrementX;
                y0 += incrementY;
                P += dPru;

                fValidY = y0 >= bitmap.clipping.top && y0 < bitmap.clipping.bottom;
            }
            else
            {
                x0 += incrementX;
                P += dPr;
            }

            fValidX = x0 >= bitmap.clipping.left && x0 < bitmap.clipping.right;
        }
    }
    else
    {
        dPr = dx << 1;          // amount to increment decision if right is chosen (always)
        dPru = dPr - (dy << 1); // amount to increment decision if up is chosen
        P = dPr - dy;           // decision variable start value

        for (; dy >= 0; dy--)
        {
            if (fValidX && fValidY)
            {
                if (thickness > 1)
                {
                    SetThickPixel(bitmap, x0, y0, pen);
                }
                else
                {
                    data = ComputePosition(bitmap, x0, y0, mask, shift);

                    *data &= ~mask;
                    *data |= color << shift;
                }
            }

            if (P > 0)
            {
                x0 += incrementX;
                y0 += incrementY;
                P += dPru;

                fValidX = x0 >= bitmap.clipping.left && x0 < bitmap.clipping.right;
            }
            else
            {
                y0 += incrementY;
                P += dPr;
            }

            fValidY = y0 >= bitmap.clipping.top && y0 < bitmap.clipping.bottom;
        }
    }
}

void GraphicsDriver::EllipseAlgorithm(
    const PAL_GFX_Bitmap &bitmap,
    int radiusX,
    int radiusY,
    void *params,
    EllipseCallback ellipseCallback)
{
    // TODO: check for some upper bound of the radius so that the caclulation won't overflow the int.

    if (radiusX <= 0 || radiusY <= 0)
        return;

    if (radiusX == radiusY)
    {
        int x = 0;
        int y = radiusX;
        int p = 3 - (radiusX << 1);
        int yNext = y;

        while (x <= y)
        {
            if (p < 0)
            {
                p += (x << 2) + 6;
            }
            else
            {
                p += ((x - y) << 2) + 10;
                yNext--;
            }

            (*ellipseCallback)(bitmap, x, y, params);
            if (x < y)
                (*ellipseCallback)(bitmap, y, x, params);

            y = yNext;
            x++;
        }
    }
    else
    {
        int x = 0;
        int y = radiusY;

        const int aSqr = radiusX * radiusX;
        const int bSqr = radiusY * radiusY;
        const int fourASqr = 4 * aSqr;
        const int fourBSqr = 4 * bSqr;
        const int eightASqr = 8 * aSqr;
        const int eightBSqr = 8 * bSqr;

        int D = fourBSqr - fourASqr * radiusY + aSqr;
        int G = fourASqr * (2 * radiusY - 1);

        int deltaE = fourBSqr + eightBSqr;
        int deltaSE = fourBSqr + eightBSqr + eightASqr * (1 - radiusY);

        (*ellipseCallback)(bitmap, x, y, params);

        while (G > 0)
        {
            if (D < 0) // Select E
            {
                D += deltaE;

                deltaE += eightBSqr;
                deltaSE += eightBSqr;

                G -= eightBSqr;
            }
            else // Select SE
            {
                D += deltaSE;

                deltaE += eightBSqr;
                deltaSE += eightBSqr + eightASqr;

                G -= (eightASqr + eightBSqr);

                y--;
            }
            x++;

            (*ellipseCallback)(bitmap, x, y, params);
        }

        D += aSqr * (3 - 4 * y) - bSqr * (3 + 4 * x);

        int deltaS;

        deltaS = fourASqr * (-2 * y + 3);
        deltaSE = eightBSqr * (x + 1) + deltaS;

        while (y > 0)
        {
            if (D < 0) // Select SE
            {
                D += deltaSE;

                deltaS += eightASqr;
                deltaSE += eightASqr + eightBSqr;

                x++;
            }
            else // Select S
            {
                D += deltaS;

                deltaS += eightASqr;
                deltaSE += eightASqr;
            }
            y--;

            (*ellipseCallback)(bitmap, x, y, params);
        }
    }
}

void GraphicsDriver::SetPixelsHelper(
    const PAL_GFX_Bitmap &bitmap,
    const GFX_Rect &rect,
    CLR_UINT32 config,
    GFX_SetPixelsCallback callback,
    void *param)
{
    int x = rect.left;
    int y = rect.top;
    int width = rect.Width();
    int height = rect.Height();
    int xSrc = 0;
    int ySrc = 0;
    bool checkPixelVisible = (!(config & PAL_GFX_Bitmap::c_SetPixelsConfig_Clip)) &&
                             (!(config & PAL_GFX_Bitmap::c_SetPixelsConfig_NoClipChecks));

    if (config & PAL_GFX_Bitmap::c_SetPixelsConfig_Clip)
    {
        if (g_GraphicsDriver.ClipToVisible(bitmap, x, y, width, height, NULL, xSrc, ySrc) == FALSE)
            return;
    }

    int stride = g_GraphicsDriver.GetWidthInWords(bitmap.width) * 2;
    CLR_UINT16 *curRow = ((CLR_UINT16 *)bitmap.data) + y * stride + x;
    CLR_UINT16 *curPixel;

    int xStart = x;
    int xEnd = x + width;
    CLR_UINT32 color;
    CLR_UINT32 flag = PAL_GFX_Bitmap::c_SetPixels_First;
    CLR_UINT16 opacity;
    int yIncrement = 1;

    if (config & PAL_GFX_Bitmap::c_SetPixelsConfig_ReverseY)
    {
        yIncrement = -1;
        y = y + height - 1;
        curRow += (height - 1) * stride; // go to the last row
        stride *= -1;
    }

    curPixel = curRow;

    for (; height > 0; height--)
    {
        flag |= PAL_GFX_Bitmap::c_SetPixels_NewRow;

        for (x = xStart; x < xEnd; x++)
        {
            opacity = PAL_GFX_Bitmap::c_OpacityOpaque;

            if (checkPixelVisible && !g_GraphicsDriver.IsPixelVisible(bitmap.clipping, x, y))
            {
                flag |= PAL_GFX_Bitmap::c_SetPixels_PixelHidden;
                (*callback)(x, y, flag, opacity, param);
            }
            else
            {
                color = (*callback)(x, y, flag, opacity, param);

                if (opacity != PAL_GFX_Bitmap::c_OpacityTransparent)
                {
                    *curPixel = g_GraphicsDriver.NativeColorInterpolate(
                        g_GraphicsDriver.ConvertColorToNative(color),
                        *curPixel,
                        opacity);
                }
            }

            flag = PAL_GFX_Bitmap::c_SetPixels_None;
            curPixel++;
        }

        curRow += stride;
        curPixel = curRow;
        y += yIncrement;
    }
}

void GraphicsDriver::Screen_Flush(
    CLR_GFX_Bitmap &bitmap,
    CLR_UINT16 x,
    CLR_UINT16 y,
    CLR_UINT16 width,
    CLR_UINT16 height)
{
    NATIVE_PROFILE_CLR_HARDWARE();
    CLR_INT32 widthMax = g_DisplayDriver.Attributes.Width;
    CLR_INT32 heightMax = g_DisplayDriver.Attributes.Height;

    if ((CLR_UINT32)(x + width) > bitmap.m_bm.m_width)
        width = bitmap.m_bm.m_width - x;
    if ((CLR_UINT32)(y + height) > bitmap.m_bm.m_height)
        height = bitmap.m_bm.m_height - y;

    if (bitmap.m_bm.m_width != (CLR_UINT32)widthMax)
        return;
    if (bitmap.m_bm.m_height != (CLR_UINT32)heightMax)
        return;
    if (bitmap.m_bm.m_bitsPerPixel != CLR_GFX_BitmapDescription::c_NativeBpp)
        return;
    if (bitmap.m_palBitmap.transparentColor != PAL_GFX_Bitmap::c_InvalidColor)
        return;

    g_DisplayDriver.BitBlt(x, y, width, height, bitmap.m_palBitmap.data);
}
