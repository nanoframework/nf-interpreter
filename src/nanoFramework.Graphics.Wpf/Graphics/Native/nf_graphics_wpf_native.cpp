#include <Graphics.h>
#include "nf_graphics_wpf_native.h"


static const CLR_RT_MethodHandler method_lookup[] =
{
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_nf_graphics_wpf_native_nanoFramework_UI_Bitmap::_ctor___VOID__I4__I4,
    Library_nf_graphics_wpf_native_nanoFramework_UI_Bitmap::_ctor___VOID__SZARRAY_U1__nanoFrameworkUIBitmapBitmapImageType,
    Library_nf_graphics_wpf_native_nanoFramework_UI_Bitmap::Flush___VOID,
    Library_nf_graphics_wpf_native_nanoFramework_UI_Bitmap::Flush___VOID__I4__I4__I4__I4,
    Library_nf_graphics_wpf_native_nanoFramework_UI_Bitmap::Clear___VOID,
    Library_nf_graphics_wpf_native_nanoFramework_UI_Bitmap::DrawTextInRect___BOOLEAN__BYREF_STRING__BYREF_I4__BYREF_I4__I4__I4__I4__I4__U4__nanoFrameworkPresentationMediaColor__nanoFrameworkUIFont,
    NULL,
    Library_nf_graphics_wpf_native_nanoFramework_UI_Bitmap::SetClippingRectangle___VOID__I4__I4__I4__I4,
    Library_nf_graphics_wpf_native_nanoFramework_UI_Bitmap::get_Width___I4,
    Library_nf_graphics_wpf_native_nanoFramework_UI_Bitmap::get_Height___I4,
    Library_nf_graphics_wpf_native_nanoFramework_UI_Bitmap::DrawEllipse___VOID__nanoFrameworkPresentationMediaColor__I4__I4__I4__I4__I4__nanoFrameworkPresentationMediaColor__I4__I4__nanoFrameworkPresentationMediaColor__I4__I4__U2,
    NULL,
    NULL,
    Library_nf_graphics_wpf_native_nanoFramework_UI_Bitmap::DrawImage___VOID__I4__I4__nanoFrameworkUIBitmap__I4__I4__I4__I4__U2,
    Library_nf_graphics_wpf_native_nanoFramework_UI_Bitmap::RotateImage___VOID__I4__I4__I4__nanoFrameworkUIBitmap__I4__I4__I4__I4__U2,
    Library_nf_graphics_wpf_native_nanoFramework_UI_Bitmap::MakeTransparent___VOID__nanoFrameworkPresentationMediaColor,
    Library_nf_graphics_wpf_native_nanoFramework_UI_Bitmap::StretchImage___VOID__I4__I4__nanoFrameworkUIBitmap__I4__I4__U2,
    Library_nf_graphics_wpf_native_nanoFramework_UI_Bitmap::DrawLine___VOID__nanoFrameworkPresentationMediaColor__I4__I4__I4__I4__I4,
    Library_nf_graphics_wpf_native_nanoFramework_UI_Bitmap::DrawRectangle___VOID__nanoFrameworkPresentationMediaColor__I4__I4__I4__I4__I4__I4__I4__nanoFrameworkPresentationMediaColor__I4__I4__nanoFrameworkPresentationMediaColor__I4__I4__U2,
    Library_nf_graphics_wpf_native_nanoFramework_UI_Bitmap::DrawText___VOID__STRING__nanoFrameworkUIFont__nanoFrameworkPresentationMediaColor__I4__I4,
    Library_nf_graphics_wpf_native_nanoFramework_UI_Bitmap::SetPixel___VOID__I4__I4__nanoFrameworkPresentationMediaColor,
    Library_nf_graphics_wpf_native_nanoFramework_UI_Bitmap::GetPixel___nanoFrameworkPresentationMediaColor__I4__I4,
    Library_nf_graphics_wpf_native_nanoFramework_UI_Bitmap::GetBitmap___SZARRAY_U1,
    Library_nf_graphics_wpf_native_nanoFramework_UI_Bitmap::StretchImage___VOID__I4__I4__I4__I4__nanoFrameworkUIBitmap__I4__I4__I4__I4__U2,
    Library_nf_graphics_wpf_native_nanoFramework_UI_Bitmap::TileImage___VOID__I4__I4__nanoFrameworkUIBitmap__I4__I4__U2,
    Library_nf_graphics_wpf_native_nanoFramework_UI_Bitmap::Scale9Image___VOID__I4__I4__I4__I4__nanoFrameworkUIBitmap__I4__I4__I4__I4__U2,
    Library_nf_graphics_wpf_native_nanoFramework_UI_Bitmap::Dispose___VOID__BOOLEAN,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_nf_graphics_wpf_native_nanoFramework_Touch_TouchCollectorConfiguration::EnableTouchCollection___STATIC__VOID__I4__I4__I4__I4__I4__nanoFrameworkUIBitmap,
    Library_nf_graphics_wpf_native_nanoFramework_Touch_TouchCollectorConfiguration::GetTouchPoints___STATIC__VOID__BYREF_I4__SZARRAY_I2__SZARRAY_I2,
    Library_nf_graphics_wpf_native_nanoFramework_Touch_TouchCollectorConfiguration::GetTouchInput___STATIC__VOID__nanoFrameworkTouchTouchCollectorConfigurationTouchInput__BYREF_I4__BYREF_I4__BYREF_I4,
    Library_nf_graphics_wpf_native_nanoFramework_Touch_TouchCollectorConfiguration::SetTouchInput___STATIC__VOID__nanoFrameworkTouchTouchCollectorConfigurationTouchInput__I4__I4__I4,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_nf_graphics_wpf_native_nanoFramework_Touch_Ink::SetInkRegion___STATIC__VOID__U4__I4__I4__I4__I4__I4__I4__I4__nanoFrameworkUIBitmap,
    Library_nf_graphics_wpf_native_nanoFramework_Touch_Ink::ResetInkRegion___STATIC__VOID,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_nf_graphics_wpf_native_nanoFramework_Touch_TouchEventProcessor::ProcessEvent___nanoFrameworkRuntimeEventsnanoFrameworkRuntimeEventsBaseEvent__U4__U4__mscorlibSystemDateTime,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_nf_graphics_wpf_native_nanoFramework_Touch_TouchPanel::SetCalibration___VOID__I4__SZARRAY_I2__SZARRAY_I2__SZARRAY_I2__SZARRAY_I2,
    Library_nf_graphics_wpf_native_nanoFramework_Touch_TouchPanel::GetCalibrationPointCount___VOID__BYREF_I4,
    Library_nf_graphics_wpf_native_nanoFramework_Touch_TouchPanel::StartCalibration___VOID,
    Library_nf_graphics_wpf_native_nanoFramework_Touch_TouchPanel::GetCalibrationPoint___VOID__I4__BYREF_I4__BYREF_I4,
    Library_nf_graphics_wpf_native_nanoFramework_Touch_TouchPanel::EnableInternal___VOID__BOOLEAN,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_nf_graphics_wpf_native_nanoFramework_UI_DisplayControl::get_LongerSide___I4,
    Library_nf_graphics_wpf_native_nanoFramework_UI_DisplayControl::get_ShorterSide___I4,
    Library_nf_graphics_wpf_native_nanoFramework_UI_DisplayControl::get_BitsPerPixel___I4,
    Library_nf_graphics_wpf_native_nanoFramework_UI_DisplayControl::get_Orientation___I4,
    Library_nf_graphics_wpf_native_nanoFramework_UI_DisplayControl::ChangeOrientation___BOOLEAN__BYREF_I4,
    Library_nf_graphics_wpf_native_nanoFramework_UI_DisplayControl::Dispose___VOID__BOOLEAN,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_nf_graphics_wpf_native_nanoFramework_UI_Font::CharWidth___I4__CHAR,
    Library_nf_graphics_wpf_native_nanoFramework_UI_Font::get_Height___I4,
    Library_nf_graphics_wpf_native_nanoFramework_UI_Font::get_AverageWidth___I4,
    Library_nf_graphics_wpf_native_nanoFramework_UI_Font::get_MaxWidth___I4,
    Library_nf_graphics_wpf_native_nanoFramework_UI_Font::get_Ascent___I4,
    Library_nf_graphics_wpf_native_nanoFramework_UI_Font::get_Descent___I4,
    Library_nf_graphics_wpf_native_nanoFramework_UI_Font::get_InternalLeading___I4,
    Library_nf_graphics_wpf_native_nanoFramework_UI_Font::get_ExternalLeading___I4,
    NULL,
    Library_nf_graphics_wpf_native_nanoFramework_UI_Font::ComputeExtent___VOID__STRING__BYREF_I4__BYREF_I4__I4,
    NULL,
    NULL,
    Library_nf_graphics_wpf_native_nanoFramework_UI_Font::ComputeTextInRect___VOID__STRING__BYREF_I4__BYREF_I4__I4__I4__I4__I4__U4,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Graphics_Wpf =
{
    "nanoFramework.Graphics.Wpf", 
	0xFB09D903,
    method_lookup,
    ////////////////////////////////////////////////////////////////////////////////////
    // check if the version bellow matches the one in AssemblyNativeVersion attribute //
    ////////////////////////////////////////////////////////////////////////////////////
    { 1, 0, 0, 0 }
};
