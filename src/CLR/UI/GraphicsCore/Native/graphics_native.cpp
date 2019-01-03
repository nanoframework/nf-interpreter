////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Graphics.h"


static const CLR_RT_MethodHandler method_lookup[] =
{
    NULL,
    NULL,
    Library_graphics_native_Bitmap::_ctor___VOID__I4__I4,
    Library_graphics_native_Bitmap::_ctor___VOID__SZARRAY_U1__BitmapBitmapImageType,
    Library_graphics_native_Bitmap::Flush___VOID,
    Library_graphics_native_Bitmap::Flush___VOID__I4__I4__I4__I4,
    Library_graphics_native_Bitmap::Clear___VOID,
    Library_graphics_native_Bitmap::DrawTextInRect___BOOLEAN__BYREF_STRING__BYREF_I4__BYREF_I4__I4__I4__I4__I4__U4__PresentationMediaColor__Font,
    NULL,
    Library_graphics_native_Bitmap::SetClippingRectangle___VOID__I4__I4__I4__I4,
    Library_graphics_native_Bitmap::get_Width___I4,
    Library_graphics_native_Bitmap::get_Height___I4,
    Library_graphics_native_Bitmap::DrawEllipse___VOID__PresentationMediaColor__I4__I4__I4__I4__I4__PresentationMediaColor__I4__I4__PresentationMediaColor__I4__I4__U2,
    NULL,
    NULL,
    Library_graphics_native_Bitmap::DrawImage___VOID__I4__I4__Bitmap__I4__I4__I4__I4__U2,
    Library_graphics_native_Bitmap::RotateImage___VOID__I4__I4__I4__Bitmap__I4__I4__I4__I4__U2,
    Library_graphics_native_Bitmap::MakeTransparent___VOID__PresentationMediaColor,
    Library_graphics_native_Bitmap::StretchImage___VOID__I4__I4__Bitmap__I4__I4__U2,
    Library_graphics_native_Bitmap::DrawLine___VOID__PresentationMediaColor__I4__I4__I4__I4__I4,
    Library_graphics_native_Bitmap::DrawRectangle___VOID__PresentationMediaColor__I4__I4__I4__I4__I4__I4__I4__PresentationMediaColor__I4__I4__PresentationMediaColor__I4__I4__U2,
    Library_graphics_native_Bitmap::DrawText___VOID__STRING__Font__PresentationMediaColor__I4__I4,
    Library_graphics_native_Bitmap::SetPixel___VOID__I4__I4__PresentationMediaColor,
    Library_graphics_native_Bitmap::GetPixel___PresentationMediaColor__I4__I4,
    Library_graphics_native_Bitmap::GetBitmap___SZARRAY_U1,
    Library_graphics_native_Bitmap::StretchImage___VOID__I4__I4__I4__I4__Bitmap__I4__I4__I4__I4__U2,
    Library_graphics_native_Bitmap::TileImage___VOID__I4__I4__Bitmap__I4__I4__U2,
    Library_graphics_native_Bitmap::Scale9Image___VOID__I4__I4__I4__I4__Bitmap__I4__I4__I4__I4__U2,
    Library_graphics_native_Bitmap::Dispose___VOID__BOOLEAN,
    NULL,
    NULL,
    Library_graphics_native_Font::CharWidth___I4__CHAR,
    Library_graphics_native_Font::get_Height___I4,
    Library_graphics_native_Font::get_AverageWidth___I4,
    Library_graphics_native_Font::get_MaxWidth___I4,
    Library_graphics_native_Font::get_Ascent___I4,
    Library_graphics_native_Font::get_Descent___I4,
    Library_graphics_native_Font::get_InternalLeading___I4,
    Library_graphics_native_Font::get_ExternalLeading___I4,
    NULL,
    Library_graphics_native_Font::ComputeExtent___VOID__STRING__BYREF_I4__BYREF_I4__I4,
    NULL,
    NULL,
    Library_graphics_native_Font::ComputeTextInRect___VOID__STRING__BYREF_I4__BYREF_I4__I4__I4__I4__I4__U4,
    NULL,
    NULL,
    NULL,
    NULL,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Graphics =
{
    "nano.Graphics", 
    0x46930647,
    method_lookup
};

