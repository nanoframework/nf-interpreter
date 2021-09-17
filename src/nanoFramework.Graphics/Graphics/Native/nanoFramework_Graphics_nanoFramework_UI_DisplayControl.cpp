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

