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

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_DisplayControl::
    NativeInitSpi___STATIC__U4__U1__I4__I4__I4__I4__U2__U2__U2__U2__U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();    

    g_GraphicsMemoryHeap.Initialize();
    // Initialise Graphics after devices initialised
    DisplayInterfaceConfig displayConfig;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // Define SPI display configuration for the display
    displayConfig.Spi.spiBus = stack.Arg0().NumericByRef().u1;
    displayConfig.Spi.chipSelect = stack.Arg1().NumericByRef().s4;
    displayConfig.Spi.dataCommand = stack.Arg2().NumericByRef().s4;
    displayConfig.Spi.reset = stack.Arg3().NumericByRef().s4;
    displayConfig.Spi.backLight = stack.Arg4().NumericByRef().s4;
    displayConfig.Screen.x = stack.Arg5().NumericByRef().u2;
    displayConfig.Screen.y = stack.Arg6().NumericByRef().u2;
    displayConfig.Screen.width = stack.Arg7().NumericByRef().u2;
    displayConfig.Screen.height = stack.ArgN(8).NumericByRef().u2;
    CLR_Debug::Printf("spibus=%d,cs=%d,dc=%d,rst=%d,bl=%d\n", displayConfig.Spi.spiBus, displayConfig.Spi.chipSelect, displayConfig.Spi.dataCommand, displayConfig.Spi.reset, displayConfig.Spi.backLight);
    CLR_Debug::Printf("x=%d,y=%d,w=%d,h=%d\n", displayConfig.Screen.x,displayConfig.Screen.y, displayConfig.Screen.width, displayConfig.Screen.height);

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

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_DisplayControl::
    NativeInitI2c___STATIC__U4__U1__U1__BOOLEAN__U2__U2__U2__U2__U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

