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
typedef Library_nanoFramework_Graphics_nanoFramework_UI_SpiConfiguration SpiConfiguration;
typedef Library_nanoFramework_Graphics_nanoFramework_UI_ScreenConfiguration ScreenConfiguration;
typedef Library_nanoFramework_Graphics_nanoFramework_UI_GraphicDriver GraphicDriver;

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
    stack.SetResult_Boolean(g_GraphicsDriver.ChangeOrientation((DisplayOrientation)orientation));
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
        g_DisplayDriver.BitBlt(
            0,                              // srcX
            0,                              // srcY
            stack.Arg2().NumericByRef().u2, // width
            stack.Arg3().NumericByRef().u2, // height
            stack.Arg2().NumericByRef().u2, // stride
            stack.Arg0().NumericByRef().u2, // screenX
            stack.Arg1().NumericByRef().u2, // screenY
            writeData);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_DisplayControl::
    NativeInitSpi___STATIC__U4__nanoFrameworkUISpiConfiguration__nanoFrameworkUIScreenConfiguration__U4(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *spiconfig;
    CLR_RT_HeapBlock *screenconfig;
    CLR_RT_HeapBlock *graphicDriver;
    CLR_INT32 desired;
    // Initialise Graphics after devices initialised
    DisplayInterfaceConfig displayConfig;

    desired = stack.Arg2().NumericByRef().u4;
    g_GraphicsMemoryHeap.Initialize(desired);

    spiconfig = stack.Arg0().Dereference();
    screenconfig = stack.Arg1().Dereference();

    // Define SPI display configuration for the display
    // internally SPI bus ID is zero based, so better take care of that here
    displayConfig.Spi.spiBus = spiconfig[SpiConfiguration::FIELD___spiBus].NumericByRef().u1 - 1;

    displayConfig.Spi.chipSelect = spiconfig[SpiConfiguration::FIELD___chipSelect].NumericByRef().s4;
    displayConfig.Spi.dataCommand = spiconfig[SpiConfiguration::FIELD___dataCommand].NumericByRef().s4;
    displayConfig.Spi.reset = spiconfig[SpiConfiguration::FIELD___reset].NumericByRef().s4;
    displayConfig.Spi.backLight = spiconfig[SpiConfiguration::FIELD___backLight].NumericByRef().s4;
    displayConfig.Screen.x = screenconfig[ScreenConfiguration::FIELD___x].NumericByRef().u2;
    displayConfig.Screen.y = screenconfig[ScreenConfiguration::FIELD___y].NumericByRef().u2;
    displayConfig.Screen.width = screenconfig[ScreenConfiguration::FIELD___width].NumericByRef().u2;
    displayConfig.Screen.height = screenconfig[ScreenConfiguration::FIELD___height].NumericByRef().u2;
    graphicDriver = screenconfig[ScreenConfiguration::FIELD___graphicDriver].Dereference();

    if (graphicDriver != nullptr)
    {
        displayConfig.GenericDriverCommands.Width = graphicDriver[GraphicDriver::FIELD___width].NumericByRef().u4;
        displayConfig.GenericDriverCommands.Height = graphicDriver[GraphicDriver::FIELD___height].NumericByRef().u4;
        displayConfig.GenericDriverCommands.BitsPerPixel =
            graphicDriver[GraphicDriver::FIELD___initializationSequence].NumericByRef().u1;
        displayConfig.GenericDriverCommands.InitializationSequence =
            graphicDriver[GraphicDriver::FIELD___initializationSequence].DereferenceArray();
        displayConfig.GenericDriverCommands.MemoryWrite =
            graphicDriver[GraphicDriver::FIELD___memoryWrite].NumericByRef().u1;
        displayConfig.GenericDriverCommands.SetColumnAddress =
            graphicDriver[GraphicDriver::FIELD___setColumnAddress].NumericByRef().u1;
        displayConfig.GenericDriverCommands.SetRowAddress =
            graphicDriver[GraphicDriver::FIELD___setRowAddress].NumericByRef().u1;
        displayConfig.GenericDriverCommands.PowerModeNormal =
            graphicDriver[GraphicDriver::FIELD___powerModeNormal].DereferenceArray();
        displayConfig.GenericDriverCommands.PowerModeSleep =
            graphicDriver[GraphicDriver::FIELD___powerModeSleep].DereferenceArray();
        displayConfig.GenericDriverCommands.OrientationPortrait =
            graphicDriver[GraphicDriver::FIELD___orientationPortrait].DereferenceArray();
        displayConfig.GenericDriverCommands.OrientationPortrait180 =
            graphicDriver[GraphicDriver::FIELD___orientationPortrait180].DereferenceArray();
        displayConfig.GenericDriverCommands.OrientationLandscape =
            graphicDriver[GraphicDriver::FIELD___orientationLandscape].DereferenceArray();
        displayConfig.GenericDriverCommands.OrientationLandscape180 =
            graphicDriver[GraphicDriver::FIELD___orientationLandscape180].DereferenceArray();
        displayConfig.GenericDriverCommands.Clear = graphicDriver[GraphicDriver::FIELD___clear].DereferenceArray();
        displayConfig.GenericDriverCommands.Brightness =
            graphicDriver[GraphicDriver::FIELD___brightness].NumericByRef().u1;
        displayConfig.GenericDriverCommands.DefaultOrientation =
            (CLR_UINT8)graphicDriver[GraphicDriver::FIELD___defaultOrientation].NumericByRef().s4;
        displayConfig.GenericDriverCommands.SetWindowType =
            (CLR_UINT8)graphicDriver[GraphicDriver::FIELD___setWindowType].NumericByRef().s4;
    }

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
    NativeInitI2c___STATIC__U4__nanoFrameworkUII2cConfiguration__nanoFrameworkUIScreenConfiguration__U4(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_DisplayControl::
    Write___STATIC__VOID__STRING__U2__U2__U2__U2__nanoFrameworkUIFont__U4__U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        LPCSTR szText;
        uint16_t x;
        uint16_t y;
        uint16_t width;
        uint16_t height;
        CLR_GFX_Font *font;
        uint32_t foregroundColor;
        uint32_t backgroundColor;
        CLR_RT_HeapBlock_BinaryBlob *blob;
        CLR_GFX_FontCharacterInfo chr;
        CLR_UINT32 widthChar = 0;
        CLR_UINT32 heightChar = 0;

        // Get the text and the other parameters
        szText = stack.Arg0().RecoverString();
        x = stack.Arg1().NumericByRef().u2;
        y = stack.Arg2().NumericByRef().u2;
        width = stack.Arg3().NumericByRef().u2;
        height = stack.Arg4().NumericByRef().u2;
        foregroundColor = stack.Arg6().NumericByRef().u4;
        backgroundColor = stack.Arg7().NumericByRef().u4;

        // Prepare all the elements for the colors
        GFX_Brush brush;
        GFX_Rect rectangle;
        brush.gradientStartColor = backgroundColor;
        brush.gradientEndColor = backgroundColor;
        rectangle.left = 0;
        rectangle.top = 0;

        // Get the font
        CLR_RT_HeapBlock *pThis = stack.Arg5().Dereference();
        blob = pThis[CLR_GFX_Font::FIELD__m_font].DereferenceBinaryBlob();
        if (!blob || blob->DataType() != DATATYPE_BINARY_BLOB_HEAD)
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        font = (CLR_GFX_Font *)blob->GetData();

        // Create a bitmap
        CLR_GFX_Bitmap *bitmap;
        CLR_GFX_BitmapDescription bm;
        int size;

        // The bitmap need to be 1 more pixel width than the font
        if (bm.BitmapDescription_Initialize(
                font->m_font.m_metrics.m_maxCharWidth + 1,
                font->m_font.m_metrics.m_height,
                CLR_GFX_BitmapDescription::c_NativeBpp) == false)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }

        // Create the bitmap with the propersize
        size = sizeof(CLR_GFX_Bitmap) + bm.GetTotalSize();
        bitmap = (CLR_GFX_Bitmap *)platform_malloc(size);
        if (!bitmap)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
        }

        // Setup the bitmap properly
        bitmap->m_bm = bm;
        bitmap->Bitmap_Initialize();

        // Now fill the text
        CLR_RT_UnicodeHelper uh;
        CLR_UINT16 buf[3];

        // Get a proper character from the UTF8 encoded string
        uh.SetInputUTF8(szText);
        int posX = x;
        int posY = y;
        int prevCharWidth = 0;
        int textLength = uh.CountNumberOfCharacters();
        CLR_UINT16 c;

        // Loop for each characters
        heightChar = bm.m_height;
        for (int i = 0; i < textLength; i++)
        {
            // check if exceeded the height
            if (posY <= height)
            {
                uh.m_outputUTF16 = buf;
                uh.m_outputUTF16_size = MAXSTRLEN(buf);

                // If not a valid character, go to the next one
                if (uh.ConvertFromUTF8(1, false) == false)
                {
                    break;
                }

                c = buf[0];
                font->GetCharInfo(c, chr);
                if (chr.isValid)
                {
                    widthChar = chr.width;
                    // calculate character max height for current line
                    // seems better to use font height

                    // set X position using previous char's width
                    posX += prevCharWidth;
                    if (posX + bm.m_width > width)
                    {
                        // go to next line
                        posX = x;
                        // add max character height of current line
                        posY += heightChar;
                        // reset character height (will get new max height for next line)
                        // heightChar=0;
                    }

                    // If there is a background, will fill the bitmap with it
                    if (backgroundColor != 0)
                    {
                        rectangle.right = bm.m_width;
                        rectangle.bottom = bm.m_height;
                        // change bitmap->DrawRectangle(pen, brush, rectangle);
                        // to FillRectangle (this function little bit faster than DrawRectangle)
                        // In the future I plan to improve it too
                        bitmap->FillRectangle(brush, rectangle);
                    }
                    else
                    {
                        // If no background, just clear it (faster)
                        bitmap->Clear();
                    }

                    // Draw the char in the bitmap
                    font->DrawChar(bitmap, chr, 0, 0, foregroundColor);

                    // Flush the character on the screen incrememting accordingly the position
                    // to fit into the rectangle.
                    if (posY <= height)
                    {
                        g_GraphicsDriver.Screen_Flush(*bitmap, 0, 0, bm.m_width, bm.m_height, posX, posY);
                    }

                    prevCharWidth = widthChar;
                }
            }
        }

        // Free the bitmap, the bitmpa exists for sure.
        // And there is no exist before, so memory will be properly released
        platform_free(bitmap);
    }
    NANOCLR_NOCLEANUP();
}
