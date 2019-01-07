////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Touch.h"
#include "TouchPanel_decl.h"
#include "CPU_GPIO_decl.h"

bool HAL_TOUCH_PANEL_Enable( GPIO_INTERRUPT_SERVICE_ROUTINE touchIsrProc )
{ 
    return Touch_Driver::Enable(touchIsrProc);
}

bool HAL_TOUCH_PANEL_Disable()
{
    return Touch_Driver::Disable();
}

void HAL_TOUCH_PANEL_GetPoint( TOUCH_PANEL_SAMPLE_FLAGS* pTipState, int* pSource, int* pUnCalX, int* pUnCalY )
{
    Touch_Driver::GetPoint(pTipState, pSource, pUnCalX, pUnCalY);
}

HRESULT HAL_TOUCH_PANEL_GetDeviceCaps(unsigned int iIndex, void* lpOutput)
{
    return Touch_Driver::GetDeviceCaps(iIndex, lpOutput);
}

