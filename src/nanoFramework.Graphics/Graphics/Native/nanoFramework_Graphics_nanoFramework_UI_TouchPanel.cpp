//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "Graphics.h"
#include "TouchDevice.h"
#include "Display.h"
#include "nanoFramework_Graphics.h"

extern DisplayDriver g_DisplayDriver;
extern TouchPanelDriver g_TouchPanelDriver;

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_TouchPanel::SetCalibration___VOID__I4__SZARRAY_I2__SZARRAY_I2__SZARRAY_I2__SZARRAY_I2(CLR_RT_StackFrame& stack)
{
      NANOCLR_HEADER();

   CLR_RT_HeapBlock_Array* screenX;
   CLR_RT_HeapBlock_Array* screenY;
   CLR_RT_HeapBlock_Array* uncalX;
   CLR_RT_HeapBlock_Array* uncalY;

   CLR_RT_HeapBlock* pArgs = &stack.Arg2();

   screenX = pArgs[0].DereferenceArray();  FAULT_ON_NULL(screenX);
   screenY = pArgs[1].DereferenceArray();  FAULT_ON_NULL(screenY);
   uncalX = pArgs[2].DereferenceArray();  FAULT_ON_NULL(uncalX);
   uncalY = pArgs[3].DereferenceArray();  FAULT_ON_NULL(uncalY);

   NANOCLR_SET_AND_LEAVE(g_TouchPanelDriver.SetCalibration(
      stack.Arg1().NumericByRef().s4,
      (CLR_INT16*)screenX->GetFirstElement(),
      (CLR_INT16*)screenY->GetFirstElement(),
      (CLR_INT16*)uncalX->GetFirstElement(),
      (CLR_INT16*)uncalY->GetFirstElement()
   ));

   NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_TouchPanel::GetCalibrationPointCount___VOID__BYREF_I4(CLR_RT_StackFrame& stack)
{
     NANOCLR_HEADER();

   TOUCH_PANEL_CALIBRATION_POINT tcp;
   CLR_RT_HeapBlock              hbX;
   CLR_RT_HeapBlock              hbY;

   tcp.PointNumber = stack.Arg1().NumericByRef().s4;
   tcp.cDisplayWidth = g_DisplayDriver.Attributes.Width;
   tcp.cDisplayHeight = g_DisplayDriver.Attributes.Height;
   tcp.CalibrationX = 0;
   tcp.CalibrationY = 0;

   NANOCLR_CHECK_HRESULT(g_TouchPanelDriver.GetDeviceCaps(TOUCH_PANEL_CALIBRATION_POINT_ID, &tcp));

   hbX.SetInteger(tcp.CalibrationX);  NANOCLR_CHECK_HRESULT(hbX.StoreToReference(stack.Arg2(), 0));
   hbY.SetInteger(tcp.CalibrationY);  NANOCLR_CHECK_HRESULT(hbY.StoreToReference(stack.Arg3(), 0));

   NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_TouchPanel::StartCalibration___VOID(CLR_RT_StackFrame& stack)
{
      NANOCLR_HEADER();

   if (stack.m_customState == 0) {}; //  eliminate unused parameter error

   NANOCLR_SET_AND_LEAVE(g_TouchPanelDriver.ResetCalibration());

   NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_Graphics_nanoFramework_UI_TouchPanel::GetCalibrationPoint___VOID__I4__BYREF_I4__BYREF_I4(CLR_RT_StackFrame& stack)
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

