////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _ft6x06_DRIVER_H_
#define _ft6x06_DRIVER_H_ 1


#include <nanohal.h>
#include "CPU_GPIO_decl.h"
#include "Display_driver.h"
#include "TouchPanel_decl.h"


struct ft6x06_Driver
{
    static bool Enable(GPIO_INTERRUPT_SERVICE_ROUTINE touchIsrProc);
    static bool Disable();
    static void GetPoint(TOUCH_PANEL_SAMPLE_FLAGS* pTipState, int* pSource, int* pUnCalX, int* pUnCalY );
    static HRESULT GetDeviceCaps(unsigned int iIndex, void* lpOutput);

private:
    static bool CalibrationPointGet(TOUCH_PANEL_CALIBRATION_POINT *pTCP);

};

#endif
