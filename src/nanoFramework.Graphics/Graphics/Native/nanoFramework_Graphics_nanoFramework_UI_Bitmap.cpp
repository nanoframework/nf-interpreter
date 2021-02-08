//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
//---------------------------------------------------------------------------

#include "Graphics.h"
#include "nanoFramework_Graphics.h"

extern GraphicsDriver g_GraphicsDriver;

// Helper routine 
static HRESULT GetBitmap(CLR_RT_HeapBlock* pThis, bool fForWrite, CLR_GFX_Bitmap*& bitmap);
static HRESULT GetBitmap(CLR_RT_StackFrame& stack, bool fForWrite, CLR_GFX_Bitmap*& bitmap);

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_Bitmap::_ctor___VOID__I4__I4(CLR_RT_StackFrame& stack)
{
    NANOCLR_HEADER();

    CLR_GFX_BitmapDescription bm;
    CLR_RT_HeapBlock* pThis = stack.This();
    CLR_RT_HeapBlock* pArgs = &(stack.Arg1());
    CLR_RT_HeapBlock* blob;

    //
    // Set up for restart on out of memory.
    //
    if (stack.m_customState == 0)
    {
        stack.m_customState = 1;
        stack.m_flags |= CLR_RT_StackFrame::c_CompactAndRestartOnOutOfMemory;
    }

    int width = pArgs[0].NumericByRef().s4;
    int height = pArgs[1].NumericByRef().s4;

    if (bm.BitmapDescription_Initialize(width, height, CLR_GFX_BitmapDescription::c_NativeBpp) == false)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    NANOCLR_CHECK_HRESULT(CLR_GFX_Bitmap::CreateInstance(pThis[CLR_GFX_Bitmap::FIELD__m_bitmap], bm));

    blob = pThis[CLR_GFX_Bitmap::FIELD__m_bitmap].Dereference();
    if (blob->DataType() == DATATYPE_BINARY_BLOB_HEAD)
    {
        //The bitmap data is stored on the managed heap, so there is no need for the finalizer to run.
        //This allows bitmaps on the managed heap to be reclaimed by GC when ExtractHeapBlocks cannot find memory
        //rather than waiting for later when the finalizers run.
        CLR_RT_HeapBlock_Finalizer::SuppressFinalize(pThis);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_Bitmap::_ctor___VOID__SZARRAY_U1__nanoFrameworkUIBitmapBitmapImageType(CLR_RT_StackFrame& stack)
{
    NANOCLR_HEADER();

    CLR_UINT8* imageData;
    CLR_UINT32 imageDataSize;
    CLR_UINT8  imageType;

    CLR_RT_HeapBlock* pThis = stack.This();
    CLR_RT_HeapBlock* pArgs = &(stack.Arg1());
    CLR_RT_HeapBlock* blob;

    //
    // Set up for restart on out of memory.
    //
    if (stack.m_customState == 0)
    {
        stack.m_customState = 1;
        stack.m_flags |= CLR_RT_StackFrame::c_CompactAndRestartOnOutOfMemory;
    }

    CLR_RT_HeapBlock_Array* imageDataHB = pArgs[0].DereferenceArray();  FAULT_ON_NULL(imageDataHB);

    imageType = pArgs[1].NumericByRef().u1;

    imageDataHB->Pin();

    ASSERT(imageDataHB->DataType() == DATATYPE_SZARRAY);
    ASSERT(imageDataHB->m_typeOfElement == DATATYPE_U1);
    ASSERT(imageDataHB->m_sizeOfElement == 1);

    imageData = (CLR_UINT8*)imageDataHB->GetFirstElement();
    imageDataSize = imageDataHB->m_numOfElements;

    NANOCLR_CHECK_HRESULT(CLR_GFX_Bitmap::CreateInstance(pThis[CLR_GFX_Bitmap::FIELD__m_bitmap], imageData, imageDataSize, imageType));

    blob = pThis[CLR_GFX_Bitmap::FIELD__m_bitmap].Dereference();
    if (blob->DataType() == DATATYPE_BINARY_BLOB_HEAD)
    {
        //The bitmap data is stored on the managed heap, so there is no need for the finalizer to run.
        //This allows bitmaps on the managed heap to be reclaimed by GC when ExtractHeapBlocks cannot find memory
        //rather than waiting for later when the finalizers run.
        CLR_RT_HeapBlock_Finalizer::SuppressFinalize(pThis);
    }

    NANOCLR_CLEANUP();

    imageDataHB->Unpin();

    NANOCLR_CLEANUP_END();
}

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_Bitmap::Flush___VOID(CLR_RT_StackFrame& stack)
{
    NANOCLR_HEADER();

    CLR_GFX_Bitmap* bitmap;

    NANOCLR_CHECK_HRESULT(GetBitmap(stack, false, bitmap));

    g_GraphicsDriver.Screen_Flush(*bitmap, 0, 0, bitmap->m_bm.m_width, bitmap->m_bm.m_height);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_Bitmap::Flush___VOID__I4__I4__I4__I4(CLR_RT_StackFrame& stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock* pArgs = &(stack.Arg1());

    CLR_INT32 x = pArgs[0].NumericByRef().s4;
    CLR_INT32 y = pArgs[1].NumericByRef().s4;
    CLR_INT32 width = pArgs[2].NumericByRef().s4;
    CLR_INT32 height = pArgs[3].NumericByRef().s4;

    CLR_GFX_Bitmap* bitmap;

    NANOCLR_CHECK_HRESULT(GetBitmap(stack, false, bitmap));

    g_GraphicsDriver.Screen_Flush(*bitmap, (CLR_UINT16)x, (CLR_UINT16)y, (CLR_UINT16)width, (CLR_UINT16)height);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_Bitmap::Clear___VOID(CLR_RT_StackFrame& stack)
{
    NANOCLR_HEADER();

    CLR_GFX_Bitmap* bitmap;

    NANOCLR_CHECK_HRESULT(GetBitmap(stack, true, bitmap));

    bitmap->Clear();

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_Bitmap::DrawTextInRect___BOOLEAN__BYREF_STRING__BYREF_I4__BYREF_I4__I4__I4__I4__I4__U4__nanoFrameworkPresentationMediaColor__nanoFrameworkUIFont(CLR_RT_StackFrame& stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock* pArgs = &(stack.Arg1());
    CLR_GFX_Bitmap* bitmap;
    CLR_GFX_Font* font;
    CLR_RT_HeapBlock& hbText = stack.PushValueAndClear();
    CLR_RT_HeapBlock  hbXRelStart;
    CLR_RT_HeapBlock  hbYRelStart;
    LPCSTR            szText;
    int               xRelStart;
    int               yRelStart;
    int               renderWidth;
    int               renderHeight;


    NANOCLR_CHECK_HRESULT(GetBitmap(stack, true, bitmap));
    NANOCLR_CHECK_HRESULT(Library_nanoFramework_Graphics_nanoFramework_UI_Font::GetFont(&pArgs[9], font));

    NANOCLR_CHECK_HRESULT(hbText.LoadFromReference(pArgs[0]));
    NANOCLR_CHECK_HRESULT(hbXRelStart.LoadFromReference(pArgs[1]));
    NANOCLR_CHECK_HRESULT(hbYRelStart.LoadFromReference(pArgs[2]));

    szText = hbText.RecoverString(); if (!szText) szText = "";
    xRelStart = hbXRelStart.NumericByRef().s4;
    yRelStart = hbYRelStart.NumericByRef().s4;

    NANOCLR_CHECK_HRESULT(CLR_GFX_Bitmap::DrawTextInRect(szText,
        xRelStart,
        yRelStart,
        renderWidth,
        renderHeight,
        bitmap,
        pArgs[3].NumericByRef().s4, /* x      */
        pArgs[4].NumericByRef().s4, /* y      */
        pArgs[5].NumericByRef().s4, /* width  */
        pArgs[6].NumericByRef().s4, /* height */
        pArgs[7].NumericByRef().u4, /* flags  */
        pArgs[8].NumericByRef().u4, /* color  */
        font
    ));

    if (szText[0])
    {
        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(hbText, szText));
    }
    else
    {
        hbText.SetObjectReference(NULL);
    }

    hbXRelStart.SetInteger((CLR_INT32)xRelStart);
    hbYRelStart.SetInteger((CLR_INT32)yRelStart);

    NANOCLR_CHECK_HRESULT(hbText.StoreToReference(pArgs[0], 0));
    NANOCLR_CHECK_HRESULT(hbXRelStart.StoreToReference(pArgs[1], 0));
    NANOCLR_CHECK_HRESULT(hbYRelStart.StoreToReference(pArgs[2], 0));

    //
    // Remove temporaries from the stack.
    //
    stack.PopValue();

    stack.SetResult_Boolean(szText[0] == 0);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_Bitmap::SetClippingRectangle___VOID__I4__I4__I4__I4(CLR_RT_StackFrame& stack)
{
    NANOCLR_HEADER();

    CLR_GFX_Bitmap* bitmap;
    CLR_RT_HeapBlock* pArgs;
    GFX_Rect      rc;

    NANOCLR_CHECK_HRESULT(GetBitmap(stack, false, bitmap));

    pArgs = &(stack.Arg1());

    rc.left = pArgs[0].NumericByRef().s4;
    rc.top = pArgs[1].NumericByRef().s4;
    rc.right = rc.left + pArgs[2].NumericByRef().s4;
    rc.bottom = rc.top + pArgs[3].NumericByRef().s4;

    bitmap->SetClipping(rc);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_Bitmap::get_Width___I4(CLR_RT_StackFrame& stack)
{
    NANOCLR_HEADER();
    CLR_GFX_Bitmap* bitmap;

    NANOCLR_CHECK_HRESULT(GetBitmap(stack, false, bitmap));

    stack.SetResult_I4(bitmap->m_bm.m_width);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_Bitmap::get_Height___I4(CLR_RT_StackFrame& stack)
{
    NANOCLR_HEADER();
    CLR_GFX_Bitmap* bitmap;

    NANOCLR_CHECK_HRESULT(GetBitmap(stack, false, bitmap));

    stack.SetResult_I4(bitmap->m_bm.m_height);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_Bitmap::DrawEllipse___VOID__nanoFrameworkPresentationMediaColor__I4__I4__I4__I4__I4__nanoFrameworkPresentationMediaColor__I4__I4__nanoFrameworkPresentationMediaColor__I4__I4__U2(CLR_RT_StackFrame& stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock* pArgs;
    CLR_GFX_Bitmap* bitmap;

    NANOCLR_CHECK_HRESULT(GetBitmap(stack, true, bitmap));
    pArgs = &(stack.Arg1());

    GFX_Pen pen;
    pen.color = pArgs[0].NumericByRef().u4;
    pen.thickness = pArgs[1].NumericByRef().s4;

    GFX_Brush brush;
    brush.gradientStartColor = pArgs[6].NumericByRef().u4;
    brush.gradientStartX = pArgs[7].NumericByRef().s4;
    brush.gradientStartY = pArgs[8].NumericByRef().s4;
    brush.gradientEndColor = pArgs[9].NumericByRef().u4;
    brush.gradientEndX = pArgs[10].NumericByRef().s4;
    brush.gradientEndY = pArgs[11].NumericByRef().s4;
    brush.opacity = pArgs[12].NumericByRef().u2;

    bitmap->DrawEllipse(pen, brush,
        pArgs[2].NumericByRef().s4,
        pArgs[3].NumericByRef().s4,
        pArgs[4].NumericByRef().s4,
        pArgs[5].NumericByRef().s4
    );
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_Bitmap::DrawImage___VOID__I4__I4__nanoFrameworkUIBitmap__I4__I4__I4__I4__U2(CLR_RT_StackFrame& stack)
{
    NANOCLR_HEADER();
    CLR_RT_HeapBlock* pArgs;
    CLR_GFX_Bitmap* bitmap;
    CLR_GFX_Bitmap* bitmapSrc;
    int width, height;

    pArgs = &(stack.Arg1());
    NANOCLR_CHECK_HRESULT(GetBitmap(stack, true, bitmap));
    NANOCLR_CHECK_HRESULT(GetBitmap(&pArgs[2], false, bitmapSrc));

    if (bitmap != bitmapSrc)
    {
        GFX_Rect dst;

        width = pArgs[5].NumericByRef().s4;
        height = pArgs[6].NumericByRef().s4;

        if (width < 0 || height < 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        dst.left = pArgs[0].NumericByRef().s4;
        dst.top = pArgs[1].NumericByRef().s4;
        dst.right = dst.left + width - 1;
        dst.bottom = dst.top + height - 1;

        GFX_Rect src;
        src.left = pArgs[3].NumericByRef().s4;
        src.top = pArgs[4].NumericByRef().s4;
        src.right = src.left + dst.Width() - 1;
        src.bottom = src.top + dst.Height() - 1;

        bitmap->DrawImage(dst, *bitmapSrc, src, pArgs[7].NumericByRef().u2);
    }
    else
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_Bitmap::RotateImage___VOID__I4__I4__I4__nanoFrameworkUIBitmap__I4__I4__I4__I4__U2(CLR_RT_StackFrame& stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock* pArgs;
    CLR_GFX_Bitmap* bitmap;
    CLR_GFX_Bitmap* bitmapSrc;

    pArgs = &(stack.Arg1());
    NANOCLR_CHECK_HRESULT(GetBitmap(stack, true, bitmap));
    NANOCLR_CHECK_HRESULT(GetBitmap(&pArgs[3], false, bitmapSrc));

    if (bitmap != bitmapSrc)
    {
        int angle = pArgs[0].NumericByRef().s4;

        GFX_Rect dst;
        dst.left = pArgs[1].NumericByRef().s4;
        dst.top = pArgs[2].NumericByRef().s4;
        dst.right = dst.left + pArgs[6].NumericByRef().s4 - 1;
        dst.bottom = dst.top + pArgs[7].NumericByRef().s4 - 1;

        GFX_Rect src;
        src.left = pArgs[4].NumericByRef().s4;
        src.top = pArgs[5].NumericByRef().s4;
        src.right = src.left + dst.Width() - 1;
        src.bottom = src.top + dst.Height() - 1;

        bitmap->RotateImage(angle, dst, *bitmapSrc, src, pArgs[7].NumericByRef().u2);
    }
    else
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_Bitmap::MakeTransparent___VOID__nanoFrameworkPresentationMediaColor(CLR_RT_StackFrame& stack)
{
    NANOCLR_HEADER();

    CLR_GFX_Bitmap* bitmap;
    CLR_UINT32 color;

    NANOCLR_CHECK_HRESULT(GetBitmap(stack, true, bitmap));

    color = stack.Arg1().NumericByRef().u4;

    bitmap->m_palBitmap.transparentColor = (color & 0xFF000000) ? PAL_GFX_Bitmap::c_InvalidColor : color;

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_Bitmap::StretchImage___VOID__I4__I4__nanoFrameworkUIBitmap__I4__I4__U2(CLR_RT_StackFrame& stack)
{
    NANOCLR_HEADER();

    CLR_GFX_Bitmap* bitmap;
    CLR_GFX_Bitmap* bitmapSrc;
    CLR_RT_HeapBlock* pArgs;

    pArgs = &(stack.Arg1());
    NANOCLR_CHECK_HRESULT(GetBitmap(stack, true, bitmap));
    NANOCLR_CHECK_HRESULT(GetBitmap(&pArgs[2], false, bitmapSrc));

    if (bitmap != bitmapSrc)
    {
        GFX_Rect dst;
        dst.left = pArgs[0].NumericByRef().s4;
        dst.top = pArgs[1].NumericByRef().s4;
        dst.right = dst.left + pArgs[3].NumericByRef().s4 - 1;
        dst.bottom = dst.top + pArgs[4].NumericByRef().s4 - 1;

        GFX_Rect src;
        src.left = 0;
        src.top = 0;
        src.right = bitmapSrc->m_bm.m_width - 1;
        src.bottom = bitmapSrc->m_bm.m_height - 1;

        bitmap->DrawImage(dst, *bitmapSrc, src, pArgs[5].NumericByRef().u2);
    }
    else
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_Bitmap::DrawLine___VOID__nanoFrameworkPresentationMediaColor__I4__I4__I4__I4__I4(CLR_RT_StackFrame& stack)
{
    NANOCLR_HEADER();
    CLR_GFX_Bitmap* bitmap;
    CLR_RT_HeapBlock* pArgs;

    NANOCLR_CHECK_HRESULT(GetBitmap(stack, true, bitmap));
    pArgs = &(stack.Arg1());

    GFX_Pen pen;
    pen.color = pArgs[0].NumericByRef().u4;
    pen.thickness = pArgs[1].NumericByRef().s4;

    bitmap->DrawLine(pen,
        pArgs[2].NumericByRef().s4,
        pArgs[3].NumericByRef().s4,
        pArgs[4].NumericByRef().s4,
        pArgs[5].NumericByRef().s4);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_Bitmap::DrawRectangle___VOID__nanoFrameworkPresentationMediaColor__I4__I4__I4__I4__I4__I4__I4__nanoFrameworkPresentationMediaColor__I4__I4__nanoFrameworkPresentationMediaColor__I4__I4__U2(CLR_RT_StackFrame& stack)
{
    NANOCLR_HEADER();
    CLR_RT_HeapBlock* pArgs;
    CLR_GFX_Bitmap* bitmap;
    GFX_Pen           pen;
    GFX_Brush         brush;
    GFX_Rect          rectangle;
    int               radiusX;
    int               radiusY;

    NANOCLR_CHECK_HRESULT(GetBitmap(stack, true, bitmap));
    pArgs = &(stack.Arg1());


    pen.color = pArgs[0].NumericByRef().u4;
    pen.thickness = pArgs[1].NumericByRef().s4;


    brush.gradientStartColor = pArgs[8].NumericByRef().u4;
    brush.gradientStartX = pArgs[9].NumericByRef().s4;
    brush.gradientStartY = pArgs[10].NumericByRef().s4;
    brush.gradientEndColor = pArgs[11].NumericByRef().u4;
    brush.gradientEndX = pArgs[12].NumericByRef().s4;
    brush.gradientEndY = pArgs[13].NumericByRef().s4;
    brush.opacity = pArgs[14].NumericByRef().u2;


    rectangle.left = pArgs[2].NumericByRef().s4;
    rectangle.top = pArgs[3].NumericByRef().s4;
    rectangle.right = rectangle.left + pArgs[4].NumericByRef().s4 - 1;
    rectangle.bottom = rectangle.top + pArgs[5].NumericByRef().s4 - 1;

    radiusX = pArgs[6].NumericByRef().s4;
    radiusY = pArgs[7].NumericByRef().s4;

    if (radiusX > 0 || radiusY > 0) //it must be an ellipse or rounded rectangle
    {
        if (radiusX < 0 || radiusY < 0) // if one of the params is less than zero then signal error
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        bitmap->DrawRoundedRectangle(pen, brush, rectangle, radiusX, radiusY);
    }
    else if (rectangle.Width() >= 0 && rectangle.Height() >= 0)
    {
        bitmap->DrawRectangle(pen, brush, rectangle);
    }
    else
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }


    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_Bitmap::DrawText___VOID__STRING__nanoFrameworkUIFont__nanoFrameworkPresentationMediaColor__I4__I4(CLR_RT_StackFrame& stack)
{
    NANOCLR_HEADER();
    CLR_RT_HeapBlock* pArgs;
    CLR_GFX_Bitmap* bitmap;
    CLR_GFX_Font* font;
    LPCSTR            szText;

    pArgs = &(stack.Arg1());

    NANOCLR_CHECK_HRESULT(GetBitmap(stack, true, bitmap));
    NANOCLR_CHECK_HRESULT(Library_nanoFramework_Graphics_nanoFramework_UI_Font::GetFont(&pArgs[1], font));

    szText = pArgs[0].RecoverString(); FAULT_ON_NULL(szText);

    bitmap->DrawText(szText, *font, pArgs[2].NumericByRef().u4, pArgs[3].NumericByRef().s4, pArgs[4].NumericByRef().s4);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_Bitmap::SetPixel___VOID__I4__I4__nanoFrameworkPresentationMediaColor(CLR_RT_StackFrame& stack)
{
    NANOCLR_HEADER();
    CLR_GFX_Bitmap* bitmap;

    NANOCLR_CHECK_HRESULT(GetBitmap(stack, true, bitmap));

    bitmap->SetPixel(stack.Arg1().NumericByRef().s4, stack.Arg2().NumericByRef().s4, stack.Arg3().NumericByRef().u4);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_Bitmap::GetPixel___nanoFrameworkPresentationMediaColor__I4__I4(CLR_RT_StackFrame& stack)
{
    NANOCLR_HEADER();
    CLR_GFX_Bitmap* bitmap;

    NANOCLR_CHECK_HRESULT(GetBitmap(stack, true, bitmap));

    stack.SetResult_U4(bitmap->GetPixel(stack.Arg1().NumericByRef().s4, stack.Arg2().NumericByRef().s4));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_Bitmap::GetBitmap___SZARRAY_U1(CLR_RT_StackFrame& stack)
{
    NANOCLR_HEADER();
    CLR_RT_HeapBlock_Array* imageDataHB = NULL;
    CLR_GFX_Bitmap* bitmap = NULL;
    CLR_UINT32* imageData = NULL;
    CLR_UINT32* row = NULL;
    CLR_UINT32* pixel = NULL;
    int                     stride = 0;

    CLR_GFX_BitmapDescription bm;

    CLR_RT_HeapBlock& array = stack.PushValue();

    //
    // Set up for restart on out of memory.
    //
    if (stack.m_customState == 0)
    {
        stack.m_customState = 1;
        stack.m_flags |= CLR_RT_StackFrame::c_CompactAndRestartOnOutOfMemory;
    }

    NANOCLR_CHECK_HRESULT(GetBitmap(stack, false, bitmap));

    if (bm.BitmapDescription_Initialize(bitmap->m_bm.m_width, bitmap->m_bm.m_height, 32) == false)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(array, bm.GetTotalSize(), g_CLR_RT_WellKnownTypes.m_UInt8));

    imageDataHB = array.DereferenceArray();  FAULT_ON_NULL(imageDataHB);
    imageDataHB->Pin();

    stride = bm.GetWidthInWords();
    imageData = (CLR_UINT32*)imageDataHB->GetFirstElement();
    row = imageData;
    pixel = row;

    for (CLR_UINT32 y = 0; y < bitmap->m_bm.m_height; y++, row += stride, pixel = row)
    {
        for (CLR_UINT32 x = 0; x < bitmap->m_bm.m_width; x++, pixel++)
        {
            *pixel = g_GraphicsDriver.GetPixel(bitmap->m_palBitmap, x, y);
        }
    }

    NANOCLR_CLEANUP();

    imageDataHB->Unpin();

    NANOCLR_CLEANUP_END();
}

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_Bitmap::StretchImage___VOID__I4__I4__I4__I4__nanoFrameworkUIBitmap__I4__I4__I4__I4__U2(CLR_RT_StackFrame& stack)
{
    NANOCLR_HEADER();

    CLR_GFX_Bitmap* bitmap;
    CLR_GFX_Bitmap* bitmapSrc;
    CLR_RT_HeapBlock* pArgs;

    pArgs = &(stack.Arg1());
    NANOCLR_CHECK_HRESULT(GetBitmap(stack, true, bitmap));
    NANOCLR_CHECK_HRESULT(GetBitmap(&pArgs[4], false, bitmapSrc));

    if (bitmap != bitmapSrc)
    {
        GFX_Rect dst;
        dst.left = pArgs[0].NumericByRef().s4;
        dst.top = pArgs[1].NumericByRef().s4;
        dst.right = dst.left + pArgs[2].NumericByRef().s4 - 1;
        dst.bottom = dst.top + pArgs[3].NumericByRef().s4 - 1;

        GFX_Rect src;
        src.left = pArgs[5].NumericByRef().s4;
        src.top = pArgs[6].NumericByRef().s4;
        src.right = src.left + pArgs[7].NumericByRef().s4 - 1;
        src.bottom = src.top + pArgs[8].NumericByRef().s4 - 1;

        bitmap->DrawImage(dst, *bitmapSrc, src, pArgs[9].NumericByRef().u2);
    }
    else
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_Bitmap::TileImage___VOID__I4__I4__nanoFrameworkUIBitmap__I4__I4__U2(CLR_RT_StackFrame& stack)
{
    NANOCLR_HEADER();
    CLR_GFX_Bitmap* bitmap;
    CLR_GFX_Bitmap* bitmapSrc;
    CLR_RT_HeapBlock* pArgs;

    pArgs = &(stack.Arg1());
    NANOCLR_CHECK_HRESULT(GetBitmap(stack, true, bitmap));
    NANOCLR_CHECK_HRESULT(GetBitmap(&pArgs[2], false, bitmapSrc));

    if (bitmap != bitmapSrc)
    {
        int xDst = pArgs[0].NumericByRef().s4;
        int yDst = pArgs[1].NumericByRef().s4;
        int widthDst = pArgs[3].NumericByRef().s4;
        int heightDst = pArgs[4].NumericByRef().s4;
        unsigned short opacity = pArgs[5].NumericByRef().u2;

        int widthSrc = bitmapSrc->m_bm.m_width;
        int heightSrc = bitmapSrc->m_bm.m_height;
        int x, y, w, h;

        GFX_Rect src;
        src.left = 0;
        src.top = 0;

        w = widthSrc;
        for (x = 0; x < widthDst; x += widthSrc)
        {
            if (widthDst - x < widthSrc)
                w = widthDst - x;
            h = heightSrc;
            for (y = 0; y < heightDst; y += heightSrc)
            {
                if (heightDst - y < heightSrc)
                    h = heightDst - y;

                src.right = w - 1;
                src.bottom = h - 1;

                GFX_Rect dst;
                dst.left = xDst + x;
                dst.top = yDst + y;
                dst.right = dst.left + w - 1;
                dst.bottom = dst.top + h - 1;

                bitmap->DrawImage(dst, *bitmapSrc, src, opacity);
            }
        }
    }
    else
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_Bitmap::Scale9Image___VOID__I4__I4__I4__I4__nanoFrameworkUIBitmap__I4__I4__I4__I4__U2(CLR_RT_StackFrame& stack)
{
    NANOCLR_HEADER();
    CLR_GFX_Bitmap* bitmap;
    CLR_GFX_Bitmap* bitmapSrc;
    CLR_RT_HeapBlock* pArgs;

    pArgs = &(stack.Arg1());
    NANOCLR_CHECK_HRESULT(GetBitmap(stack, true, bitmap));
    NANOCLR_CHECK_HRESULT(GetBitmap(&pArgs[4], false, bitmapSrc));

    if (bitmap != bitmapSrc)
    {
        int xDst = pArgs[0].NumericByRef().s4;
        int yDst = pArgs[1].NumericByRef().s4;
        int widthDst = pArgs[2].NumericByRef().s4;

        int heightDst = pArgs[3].NumericByRef().s4;
        int leftBorder = pArgs[5].NumericByRef().s4;
        int topBorder = pArgs[6].NumericByRef().s4;
        int rightBorder = pArgs[7].NumericByRef().s4;
        int bottomBorder = pArgs[8].NumericByRef().s4;
        unsigned short opacity = pArgs[9].NumericByRef().u2;

        int widthSrc = bitmapSrc->m_bm.m_width;
        int heightSrc = bitmapSrc->m_bm.m_height;

        if (widthDst >= leftBorder && heightDst >= topBorder)
        {
            int centerWidthSrc = widthSrc - (leftBorder + rightBorder);
            int centerHeightSrc = heightSrc - (topBorder + bottomBorder);
            int centerWidthDst = widthDst - (leftBorder + rightBorder);
            int centerHeightDst = heightDst - (topBorder + bottomBorder);
            GFX_Rect src;
            GFX_Rect dst;

            //top-left
            //if(widthDst >= leftBorder && heightDst >= topBorder)
            dst.left = xDst;
            dst.top = yDst;
            dst.right = dst.left + leftBorder - 1;
            dst.bottom = dst.top + topBorder - 1;
            src.left = 0;
            src.top = 0;
            src.right = src.left + leftBorder - 1;
            src.bottom = src.top + topBorder - 1;
            bitmap->DrawImage(dst, *bitmapSrc, src, opacity);
            //bmpDest.StretchImage(xDst, yDst, leftBorder, topBorder, bitmap, 0, 0, leftBorder, topBorder, opacity);

      //top-right
            if (widthDst > leftBorder /*&& heightDst >= topBorder*/)
            {
                dst.left = xDst + widthDst - rightBorder;
                dst.top = yDst;
                dst.right = dst.left + rightBorder - 1;
                dst.bottom = dst.top + topBorder - 1;
                src.left = widthSrc - rightBorder;
                src.top = 0;
                src.right = src.left + rightBorder - 1;
                src.bottom = src.top + topBorder - 1;
                bitmap->DrawImage(dst, *bitmapSrc, src, opacity);
                //bmpDest.StretchImage(xDst + widthDst - rightBorder, yDst, rightBorder, topBorder, bitmap,
                //                     widthSrc - rightBorder, 0, rightBorder, topBorder, opacity);
            }
            //bottom-left
            if (/*widthDst >= leftBorder && */heightDst > topBorder)
            {
                dst.left = xDst;
                dst.top = yDst + heightDst - bottomBorder;
                dst.right = dst.left + leftBorder - 1;
                dst.bottom = dst.top + bottomBorder - 1;
                src.left = 0;
                src.top = heightSrc - bottomBorder;
                src.right = src.left + leftBorder - 1;
                src.bottom = src.top + bottomBorder - 1;
                bitmap->DrawImage(dst, *bitmapSrc, src, opacity);
                //bmpDest.StretchImage(xDst, yDst + heightDst - bottomBorder, leftBorder, bottomBorder, bitmap,
                //                     0, heightSrc - bottomBorder, leftBorder, bottomBorder, opacity);
            }
            //bottom-right
            if (widthDst > leftBorder&& heightDst > topBorder)
            {
                dst.left = xDst + widthDst - rightBorder;
                dst.top = yDst + heightDst - bottomBorder;
                dst.right = dst.left + rightBorder - 1;
                dst.bottom = dst.top + bottomBorder - 1;
                src.left = widthSrc - rightBorder;
                src.top = heightSrc - bottomBorder;
                src.right = src.left + rightBorder - 1;
                src.bottom = src.top + bottomBorder - 1;
                bitmap->DrawImage(dst, *bitmapSrc, src, opacity);
                //bmpDest.StretchImage(xDst + widthDst - rightBorder, yDst + heightDst - bottomBorder, rightBorder, bottomBorder, bitmap,
                //                     widthSrc - rightBorder, heightSrc - bottomBorder, rightBorder, bottomBorder, opacity);
            }
            //left
            if (/*widthDst >= leftBorder &&*/ centerHeightDst > 0)
            {
                dst.left = xDst;
                dst.top = yDst + topBorder;
                dst.right = dst.left + leftBorder - 1;
                dst.bottom = dst.top + centerHeightDst - 1;
                src.left = 0;
                src.top = topBorder;
                src.right = src.left + leftBorder - 1;
                src.bottom = src.top + centerHeightSrc - 1;
                bitmap->DrawImage(dst, *bitmapSrc, src, opacity);
                //bmpDest.StretchImage(xDst, yDst + topBorder, leftBorder, centerHeightDst, bitmap,
                //                     0, topBorder, leftBorder, centerHeightSrc, opacity);
            }
            //top
            if (centerWidthDst > 0 /*&& heightDst >= topBorder*/)
            {
                dst.left = xDst + leftBorder;
                dst.top = yDst;
                dst.right = dst.left + centerWidthDst - 1;
                dst.bottom = dst.top + topBorder - 1;
                src.left = leftBorder;
                src.top = 0;
                src.right = src.left + centerWidthSrc - 1;
                src.bottom = src.top + topBorder - 1;
                bitmap->DrawImage(dst, *bitmapSrc, src, opacity);
                //bmpDest.StretchImage(xDst + leftBorder, yDst, centerWidthDst, topBorder, bitmap,
                //                     leftBorder, 0, centerWidthSrc, topBorder, opacity);
            }


            //right
            if (widthDst > leftBorder&& centerHeightDst > 0)
            {
                dst.left = xDst + widthDst - rightBorder;
                dst.top = yDst + topBorder;
                dst.right = dst.left + rightBorder - 1;
                dst.bottom = dst.top + centerHeightDst - 1;
                src.left = widthSrc - rightBorder;
                src.top = topBorder;
                src.right = src.left + rightBorder - 1;
                src.bottom = src.top + centerHeightSrc - 1;
                bitmap->DrawImage(dst, *bitmapSrc, src, opacity);
                //bmpDest.StretchImage(xDst + widthDst - rightBorder, yDst + topBorder, rightBorder, centerHeightDst, bitmap,
                //                     widthSrc - rightBorder, topBorder, rightBorder,  centerHeightSrc, opacity);
            }
            //bottom
            if (centerWidthDst > 0 && heightDst > topBorder)
            {
                dst.left = xDst + leftBorder;
                dst.top = yDst + heightDst - bottomBorder;
                dst.right = dst.left + centerWidthDst - 1;
                dst.bottom = dst.top + bottomBorder - 1;
                src.left = leftBorder;
                src.top = heightSrc - bottomBorder;
                src.right = src.left + centerWidthSrc - 1;
                src.bottom = src.top + bottomBorder - 1;
                bitmap->DrawImage(dst, *bitmapSrc, src, opacity);
                //bmpDest.StretchImage(xDst + leftBorder, yDst + heightDst - bottomBorder, centerWidthDst, bottomBorder, bitmap,
                //                 leftBorder, heightSrc - bottomBorder, centerWidthSrc, bottomBorder, opacity);
            }
            //center
            if (centerWidthDst > 0 && centerHeightDst > 0)
            {
                dst.left = xDst + leftBorder;
                dst.top = yDst + topBorder;
                dst.right = dst.left + centerWidthDst - 1;
                dst.bottom = dst.top + centerHeightDst - 1;
                src.left = leftBorder;
                src.top = topBorder;
                src.right = src.left + centerWidthSrc - 1;
                src.bottom = src.top + centerHeightSrc - 1;
                bitmap->DrawImage(dst, *bitmapSrc, src, opacity);
                //bmpDest.StretchImage(xDst + leftBorder, yDst + topBorder, centerWidthDst, centerHeightDst, bitmap,
                //                     leftBorder, topBorder, centerWidthSrc, centerHeightSrc, opacity);
            }
        }
    }
    else
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_Bitmap::Dispose___VOID__BOOLEAN(CLR_RT_StackFrame& stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock* pThis = stack.This();

    if (pThis[CLR_GFX_Bitmap::FIELD__m_bitmap].Dereference() == NULL)
    {
        NANOCLR_SET_AND_LEAVE(S_OK);
    }

    NANOCLR_CHECK_HRESULT(CLR_GFX_Bitmap::DeleteInstance(pThis[FIELD__m_bitmap]));
    pThis[FIELD__m_bitmap].SetObjectReference(NULL);


    //   Code from another version ?
    //   NANOCLR_CHECK_HRESULT(CLR_GFX_Bitmap::DeleteInstance(pThis[CLR_GFX_Bitmap::FIELD__m_bitmap]));
    //   pThis[CLR_GFX_Bitmap::FIELD__m_bitmap].SetObjectReference(NULL);

    NANOCLR_NOCLEANUP();
}

// Helper routines, not automatically generated as per above 
//

HRESULT GetBitmap(CLR_RT_HeapBlock* pThis, bool fForWrite, CLR_GFX_Bitmap*& bitmap)
{
    NANOCLR_HEADER();

    if (pThis) pThis = pThis->Dereference(); FAULT_ON_NULL(pThis);

    NANOCLR_CHECK_HRESULT(CLR_GFX_Bitmap::GetInstanceFromGraphicsHeapBlock(pThis[CLR_GFX_Bitmap::FIELD__m_bitmap], bitmap));

    if ((bitmap->m_bm.m_flags & CLR_GFX_BitmapDescription::c_ReadOnly) && fForWrite) NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    if ((bitmap->m_bm.m_flags & CLR_GFX_BitmapDescription::c_Compressed)) NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);

    NANOCLR_NOCLEANUP();
}

HRESULT GetBitmap(CLR_RT_StackFrame& stack, bool fForWrite, CLR_GFX_Bitmap*& bitmap)
{
    return GetBitmap(&stack.Arg0(), fForWrite, bitmap);
}

