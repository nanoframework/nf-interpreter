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
    NativeInitSpi___STATIC__VOID__nanoFrameworkUISpiConfiguration__I4__I4__I4( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();    
    //uint i = 0;
    //NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());
    // NANOCLR_CHECK_HRESULT(InitScreen(stack));
    
    CLR_RT_HeapBlock *spiconfig;
    CLR_Debug::Printf("Spi init native\n");   
    CLR_Debug::Printf("before memory initialize\n");
    g_GraphicsMemoryHeap.Initialize();
    CLR_Debug::Printf("memory allocated\n");
    // Initialise Graphics after devices initialised
    DisplayInterfaceConfig displayConfig;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    CLR_Debug::Printf("before spi config\n");
    spiconfig = stack.Arg1().Dereference();
    FAULT_ON_NULL(spiconfig);

    // Define SPI display configuration for Wrover
    //displayConfig.Spi.spiBus = spiconfig[Library_nanoFramework_Graphics_nanoFramework_UI_SpiConfiguration::FIELD___spiBus].NumericByRef().u1;                // Spi Bus
    //displayConfig.Spi.chipSelect = spiconfig[Library_nanoFramework_Graphics_nanoFramework_UI_SpiConfiguration::FIELD___chipSelect].NumericByRef().s4;  // CS_1     GPIO22   CS
    //displayConfig.Spi.dataCommand = spiconfig[Library_nanoFramework_Graphics_nanoFramework_UI_SpiConfiguration::FIELD___dataCommand].NumericByRef().s4; // D/CX_1   GPIO21   D/C
    //displayConfig.Spi.reset = spiconfig[Library_nanoFramework_Graphics_nanoFramework_UI_SpiConfiguration::FIELD___reset].NumericByRef().s4;       // RST_1    GPIO18   RESET
    //displayConfig.Spi.backLight = spiconfig[Library_nanoFramework_Graphics_nanoFramework_UI_SpiConfiguration::FIELD___backLight].NumericByRef().s4;    // GPIO5    Backlight
    displayConfig.Spi.spiBus = 1;                // Spi Bus
    // displayConfig.Spi.chipSelect = 14;  // CS_1     GPIO22   CS
    // displayConfig.Spi.dataCommand = 27; // D/CX_1   GPIO21   D/C
    // displayConfig.Spi.reset = 33;       // RST_1    GPIO18   RESET
    // displayConfig.Spi.backLight = 32;    // GPIO5    Backlight
    displayConfig.Spi.chipSelect = 22;  // CS_1     GPIO22   CS
    displayConfig.Spi.dataCommand = 21; // D/CX_1   GPIO21   D/C
    displayConfig.Spi.reset = 18;       // RST_1    GPIO18   RESET
    displayConfig.Spi.backLight = 5;    // GPIO5    Backlight

    CLR_Debug::Printf("bus=%d, cs=%d, dc=%d, rst=%d, bl=%d\n", displayConfig.Spi.spiBus, displayConfig.Spi.chipSelect, displayConfig.Spi.dataCommand, displayConfig.Spi.reset, displayConfig.Spi.backLight);
    CLR_Debug::Printf("before display initialize\n");
        
    
    g_DisplayInterface.Initialize(displayConfig);
    g_DisplayDriver.Initialize();
    CLR_Debug::Printf("driver and display initialized\n");

    // g_TouchInterface.Initialize();
    // g_TouchDevice.Initialize();

    PalEvent_Initialize();
    // Gesture_Initialize();
    // Ink_Initialize();
    g_DisplayDriver.SetDefaultOrientation();
    g_DisplayDriver.Clear();

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_DisplayControl::
    NativeInitI2c___STATIC__VOID__nanoFrameworkUII2cConfiguration__I4__I4__I4( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

