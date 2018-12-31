////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
// 
// This file is part of the Microsoft .NET Micro Framework Porting Kit Code Samples and is unsupported. 
// Copyright (C) Microsoft Corporation. All rights reserved. Use of this sample source code is subject to 
// the terms of the Microsoft license agreement under which you licensed this sample source code. 
// 
// THIS SAMPLE CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, 
// INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
// 
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "nanohal.h"
#include "TouchPanelDriver.h"

//--//

HRESULT TOUCH_PANEL_Initialize()
{
    return TouchPanel_Driver::Initialize();
}

HRESULT TOUCH_PANEL_Uninitialize()
{
    return TouchPanel_Driver::Uninitialize();
}

HRESULT TOUCH_PANEL_GetDeviceCaps(unsigned int iIndex, void* lpOutput)
{
    return TouchPanel_Driver::GetDeviceCaps(iIndex, lpOutput);
}

HRESULT TOUCH_PANEL_ResetCalibration()
{
    return TouchPanel_Driver::ResetCalibration();
}

HRESULT TOUCH_PANEL_SetCalibration(int pointCount, short* sx, short* sy, short* ux, short* uy)
{
    return TouchPanel_Driver::SetCalibration(pointCount, sx, sy, ux, uy);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
HRESULT TOUCH_PANEL_EnableTouchCollection(int flags, int x1, int x2, int y1, int y2, PAL_GFX_Bitmap* bmp)
{
    // return TouchPanel_Driver::EnableTouchCollection(flags, x1, x2, y1, y2, bmp);
    return S_OK;
}
#pragma GCC diagnostic pop


HRESULT TOUCH_PANEL_GetTouchPoints(int* pointCount, short* sx, short* sy)
{
    return TouchPanel_Driver::GetTouchPoints(pointCount, sx, sy);
}

HRESULT TOUCH_PANEL_GetSetTouchInfo(CLR_UINT32 flags, CLR_INT32* param1, CLR_INT32* param2, CLR_INT32* param3)
{
    return TouchPanel_Driver::GetSetTouchInfo(flags, param1, param2, param3);
}

HRESULT TOUCH_PANEL_GetTouchPoint(CLR_UINT32* flags, CLR_UINT16* source, CLR_UINT16* x, CLR_UINT16* y, CLR_INT64* time)
{
    return TouchPanel_Driver::GetTouchPoint(flags, source, x, y, time);
}


