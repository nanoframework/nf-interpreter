//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "Graphics.h"
#include "nanoFramework_Graphics.h"
#include "GraphicsMemoryHeap.h"
#include "nanoHAL_Graphics.h"

extern GraphicsDriver g_GraphicsDriver;

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_DisplayControl::get_LongerSide___STATIC__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    stack.SetResult_I4(g_GraphicsDriver.GetLongerSide());
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_DisplayControl::get_ShorterSide___STATIC__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    stack.SetResult_I4(g_GraphicsDriver.GetShorterSide());
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_DisplayControl::get_ScreenWidth___STATIC__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    stack.SetResult_I4(g_GraphicsDriver.GetWidth());
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_DisplayControl::get_ScreenHeight___STATIC__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    stack.SetResult_I4(g_GraphicsDriver.GetHeight());
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_DisplayControl::get_BitsPerPixel___STATIC__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    stack.SetResult_I4(g_GraphicsDriver.GetBitsPerPixel());
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_DisplayControl::
    get_Orientation___STATIC__nanoFrameworkUIDisplayOrientation(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    stack.SetResult_I4((CLR_INT32)g_GraphicsDriver.GetOrientation());
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_DisplayControl::
    NativeChangeOrientation___STATIC__BOOLEAN__nanoFrameworkUIDisplayOrientation(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    CLR_INT32 orientation = stack.Arg0().NumericByRef().s4;
    g_GraphicsDriver.ChangeOrientation((DisplayOrientation)orientation);
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_DisplayControl::Clear___STATIC__VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();    
    {
        (void)stack;
        g_DisplayDriver.Clear();
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_DisplayControl::
    Write___STATIC__VOID__U2__U2__U2__U2__SZARRAY_U2(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock_Array *colors;
        CLR_UINT32 *writeData;
        colors = stack.Arg4().DereferenceArray();
        FAULT_ON_NULL(colors);

        writeData = (CLR_UINT32 *)colors->GetFirstElement();
        g_DisplayDriver.BitBlt(stack.Arg0().NumericByRef().u2, stack.Arg1().NumericByRef().u2, stack.Arg2().NumericByRef().u2, stack.Arg3().NumericByRef().u2, writeData);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_DisplayControl::
    NativeInitSpi___STATIC__U4__nanoFrameworkUISpiConfiguration__nanoFrameworkUIScreenConfiguration__U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();    

    CLR_RT_HeapBlock *spiconfig;
    CLR_RT_HeapBlock *screenconfig;
    CLR_INT32 desired;
    // Initialise Graphics after devices initialised
    DisplayInterfaceConfig displayConfig;
   
    desired = stack.Arg2().NumericByRef().u4;
    g_GraphicsMemoryHeap.Initialize(desired);

    spiconfig = stack.Arg0().Dereference();
    screenconfig = stack.Arg1().Dereference();
    // Define SPI display configuration for the display
    displayConfig.Spi.spiBus = spiconfig[Library_nanoFramework_Graphics_nanoFramework_UI_SpiConfiguration::FIELD___spiBus].NumericByRef().u1;
    displayConfig.Spi.chipSelect = spiconfig[Library_nanoFramework_Graphics_nanoFramework_UI_SpiConfiguration::FIELD___chipSelect].NumericByRef().s4;
    displayConfig.Spi.dataCommand = spiconfig[Library_nanoFramework_Graphics_nanoFramework_UI_SpiConfiguration::FIELD___dataCommand].NumericByRef().s4;
    displayConfig.Spi.reset = spiconfig[Library_nanoFramework_Graphics_nanoFramework_UI_SpiConfiguration::FIELD___reset].NumericByRef().s4;
    displayConfig.Spi.backLight = spiconfig[Library_nanoFramework_Graphics_nanoFramework_UI_SpiConfiguration::FIELD___backLight].NumericByRef().s4;
    displayConfig.Screen.x = screenconfig[Library_nanoFramework_Graphics_nanoFramework_UI_ScreenConfiguration::FIELD___x].NumericByRef().u2;
    displayConfig.Screen.y = screenconfig[Library_nanoFramework_Graphics_nanoFramework_UI_ScreenConfiguration::FIELD___y].NumericByRef().u2;
    displayConfig.Screen.width = screenconfig[Library_nanoFramework_Graphics_nanoFramework_UI_ScreenConfiguration::FIELD___width].NumericByRef().u2;
    displayConfig.Screen.height = screenconfig[Library_nanoFramework_Graphics_nanoFramework_UI_ScreenConfiguration::FIELD___height].NumericByRef().u2;
    g_DisplayInterface.Initialize(displayConfig);    
    g_DisplayDriver.Initialize();

    // TODO for touch
    // g_TouchInterface.Initialize();
    // g_TouchDevice.Initialize();

    PalEvent_Initialize();
    
    // TODO for touch
    // Gesture_Initialize();
    // Ink_Initialize();

    g_DisplayDriver.SetDefaultOrientation();
    g_DisplayDriver.Clear();
    
    stack.SetResult_U4(g_GraphicsMemoryHeap.GetMaxBuffer());

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_DisplayControl::
    NativeInitI2c___STATIC__U4__nanoFrameworkUII2cConfiguration__nanoFrameworkUIScreenConfiguration__U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_DisplayControl::Write___STATIC__VOID__STRING__U2__U2__U2__U2__nanoFrameworkUIFont__nanoFrameworkPresentationMediaColor__nanoFrameworkPresentationMediaColor( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();
    {
        LPCSTR szText;
        uint16_t x;
        uint16_t y;
        uint16_t width;
        uint16_t height;
        CLR_GFX_Font* font;
        uint32_t foregroundColor;
        uint32_t backgroundColor;    
        CLR_RT_HeapBlock_BinaryBlob* blob;
        CLR_GFX_FontCharacterInfo chr;
        CLR_UINT32 widthChar = 0;
        CLR_UINT32 heightChar = 0;

        szText = stack.Arg0().RecoverString();
        x = stack.Arg1().NumericByRef().u2;
        y = stack.Arg2().NumericByRef().u2;
        width = stack.Arg3().NumericByRef().u2;
        height = stack.Arg4().NumericByRef().u2;
        foregroundColor = stack.Arg6().NumericByRef().u4;
        backgroundColor = stack.Arg7().NumericByRef().u4;
        CLR_Debug::Printf("text=%s,x=%d,y=%d,w=%d,h=%d,fc=%d,bc=%d\n",szText, x, y, width, height, foregroundColor, backgroundColor);
        
        // Get the font
        CLR_RT_HeapBlock*  pThis = stack.Arg5().Dereference();

        blob = pThis[CLR_GFX_Font::FIELD__m_font].DereferenceBinaryBlob();
        if (!blob || blob->DataType() != DATATYPE_BINARY_BLOB_HEAD) NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        font = (CLR_GFX_Font*)blob->GetData();

        // Create a bitmap
        CLR_GFX_Bitmap *bitmap;
        CLR_GFX_BitmapDescription bm;
        int size;

        if (bm.BitmapDescription_Initialize(font->m_font.m_metrics.m_maxCharWidth + 1, font->m_font.m_metrics.m_height, CLR_GFX_BitmapDescription::c_NativeBpp) == false)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
        size = sizeof(CLR_GFX_Bitmap) + bm.GetTotalSize();
        bitmap = (CLR_GFX_Bitmap *)malloc(size);

        if (!bitmap)
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);

        memset(bitmap, 0, size); // Clear the memory to contain the bitmap structure
        bitmap->m_bm = bm;
        bitmap->Bitmap_Initialize();

        // Now fill the text
        //int incStr = 0;
        CLR_RT_UnicodeHelper uh;
        CLR_UINT16 buf[3];
        CLR_UINT32 nTotWidth = 0;
        
        uh.SetInputUTF8(szText);
        
        uh.m_outputUTF16 = buf;
        uh.m_outputUTF16_size = MAXSTRLEN(buf);

        //if (uh.ConvertFromUTF8(1, false) == false)
        //    break;

        CLR_UINT16 c = buf[0];

        font->GetCharInfo(c, chr);

        if (chr.isValid) 
        {
            widthChar = chr.width;
            heightChar = chr.height;
            CLR_Debug::Printf("w=%d,h=%d\n", widthChar, heightChar);

            // It doesn't look like we support kerning??            

            int nOffset = ((nTotWidth + (chr.width + 1) / 2)) - (chr.width + 1) / 2;

            nTotWidth = nOffset + chr.width;

            if (bitmap)
            {
                font->DrawChar(bitmap, chr, 0 + chr.marginLeft + nOffset, 0, foregroundColor);
            }
        }
        
        g_GraphicsDriver.Screen_Flush(*bitmap, x, y, font->m_font.m_metrics.m_maxCharWidth, font->m_font.m_metrics.m_height);

        
        free(bitmap);
    }
    NANOCLR_NOCLEANUP();
}