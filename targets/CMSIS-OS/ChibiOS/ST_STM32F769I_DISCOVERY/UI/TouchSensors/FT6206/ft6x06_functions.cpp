////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ft6x06_driver.h"
#include "CPU_GPIO_decl.h"
#include "TouchPanel_decl.h"

bool HAL_TOUCH_PANEL_Enable( GPIO_INTERRUPT_SERVICE_ROUTINE touchIsrProc )
{ 
    return ft6x06_Driver::Enable(touchIsrProc);
}

bool HAL_TOUCH_PANEL_Disable()
{
    return ft6x06_Driver::Disable();
}

void HAL_TOUCH_PANEL_GetPoint( TOUCH_PANEL_SAMPLE_FLAGS* pTipState, int* pSource, int* pUnCalX, int* pUnCalY )
{
    ft6x06_Driver::GetPoint(pTipState, pSource, pUnCalX, pUnCalY);
}

HRESULT HAL_TOUCH_PANEL_GetDeviceCaps(unsigned int iIndex, void* lpOutput)
{
    return ft6x06_Driver::GetDeviceCaps(iIndex, lpOutput);
}

