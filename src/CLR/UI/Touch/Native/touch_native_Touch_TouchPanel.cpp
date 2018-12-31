////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include "TouchPanel_decl.h"
#include "touch_native.h"
#include "Graphics.h"


HRESULT Library_touch_native_TouchPanel::EnableInternal___VOID__BOOLEAN( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    bool fInit = stack.Arg1().NumericByRef().s1 != 0;

    if(fInit)
    {
        NANOCLR_SET_AND_LEAVE(TOUCH_PANEL_Initialize());  
    }
    else
    {
        NANOCLR_SET_AND_LEAVE(TOUCH_PANEL_Uninitialize());         
    }
    
    NANOCLR_NOCLEANUP();
}

HRESULT Library_touch_native_TouchPanel::GetCalibrationPointCount___VOID__BYREF_I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); 

    CLR_RT_HeapBlock hbCalibration;
    TOUCH_PANEL_CALIBRATION_POINT_COUNT tcpc;
        
    NANOCLR_CHECK_HRESULT(TOUCH_PANEL_GetDeviceCaps( TOUCH_PANEL_CALIBRATION_POINT_COUNT_ID, (void *)&tcpc ));

    hbCalibration.SetInteger( tcpc.cCalibrationPoints ); NANOCLR_CHECK_HRESULT(hbCalibration.StoreToReference( stack.Arg1(), 0 ));
    
    NANOCLR_NOCLEANUP();
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
HRESULT Library_touch_native_TouchPanel::StartCalibration___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
        
    NANOCLR_SET_AND_LEAVE(TOUCH_PANEL_ResetCalibration());

    NANOCLR_NOCLEANUP();
}
#pragma GCC diagnostic pop

HRESULT Library_touch_native_TouchPanel::GetCalibrationPoint___VOID__I4__BYREF_I4__BYREF_I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); 
    
    TOUCH_PANEL_CALIBRATION_POINT tcp;
    CLR_RT_HeapBlock              hbX;
    CLR_RT_HeapBlock              hbY;

    tcp.PointNumber    = stack.Arg1().NumericByRef().s4;
    tcp.cDisplayWidth  = LCD_GetWidth();
    tcp.cDisplayHeight = LCD_GetHeight();
    tcp.CalibrationX   = 0;
    tcp.CalibrationY   = 0;

    NANOCLR_CHECK_HRESULT(TOUCH_PANEL_GetDeviceCaps( TOUCH_PANEL_CALIBRATION_POINT_ID, &tcp ));

    hbX.SetInteger( tcp.CalibrationX );  NANOCLR_CHECK_HRESULT(hbX.StoreToReference( stack.Arg2(), 0 ));
    hbY.SetInteger( tcp.CalibrationY );  NANOCLR_CHECK_HRESULT(hbY.StoreToReference( stack.Arg3(), 0 ));
    
    NANOCLR_NOCLEANUP();
}

HRESULT Library_touch_native_TouchPanel::SetCalibration___VOID__I4__SZARRAY_I2__SZARRAY_I2__SZARRAY_I2__SZARRAY_I2( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); 

    CLR_RT_HeapBlock_Array* screenX;
    CLR_RT_HeapBlock_Array* screenY;
    CLR_RT_HeapBlock_Array* uncalX;
    CLR_RT_HeapBlock_Array* uncalY;

    CLR_RT_HeapBlock* pArgs = &stack.Arg2();

    screenX = pArgs[ 0 ].DereferenceArray();  FAULT_ON_NULL(screenX);        
    screenY = pArgs[ 1 ].DereferenceArray();  FAULT_ON_NULL(screenY);        
    uncalX  = pArgs[ 2 ].DereferenceArray();  FAULT_ON_NULL(uncalX);        
    uncalY  = pArgs[ 3 ].DereferenceArray();  FAULT_ON_NULL(uncalY);        
        
    NANOCLR_SET_AND_LEAVE(TOUCH_PANEL_SetCalibration( 
                                                     stack.Arg1().NumericByRef().s4, 
                                                     (CLR_INT16*)screenX->GetFirstElement(), 
                                                     (CLR_INT16*)screenY->GetFirstElement(), 
                                                     (CLR_INT16*)uncalX ->GetFirstElement(), 
                                                     (CLR_INT16*)uncalY ->GetFirstElement()
                                                    ));

    NANOCLR_NOCLEANUP();
}


