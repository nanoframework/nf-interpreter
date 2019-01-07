////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _TOUCH_PANEL_DRIVER_H_
#define _TOUCH_PANEL_DRIVER_H_ 1


#include "TouchPanel_decl.h"
#include "nanohal.h"
#include "nanoCLR_Types.h"

//--//

class TouchPanel_Driver
{
public:
    static HRESULT Initialize();
    static HRESULT Uninitialize();

    static HRESULT GetDeviceCaps(unsigned int iIndex, void* lpOutput);
    static HRESULT ResetCalibration();
    static HRESULT SetCalibration(int pointCount, short* sx, short* sy, short* ux, short* uy);
    static HRESULT SetNativeBufferSize(int transientBufferSize, int strokeBufferSize);
    static HRESULT GetTouchPoints(int* pointCount, short* sx, short* sy);
    static HRESULT GetSetTouchInfo(CLR_UINT32 flags, CLR_INT32* param1, CLR_INT32* param2, CLR_INT32* param3);

    static HRESULT GetTouchPoint(CLR_UINT32* flags, CLR_UINT16* source, CLR_UINT16* x, CLR_UINT16* y, CLR_INT64* time);
    static HRESULT GetTouchPoint(CLR_UINT32* flags, CLR_UINT32* location, CLR_INT64* time);

private:

    static HRESULT GetTouchPoint(CLR_UINT32* flags, TouchPoint **point);

    static void    TouchIsrProc(GPIO_PIN pin, bool pinState, void* context);
    static void    TouchCompletion(void* arg);

    //--//

    void           TouchPanelCalibratePoint(int UncalX, int UncalY, int *pCalX, int   *pCalY);
    CLR_UINT16         GetTouchStylusFlags(unsigned int sampleFlags);
    void           SetDriverDefaultCalibrationData();

    void           PollTouchPoint(void* arg);
    TouchPoint*    AddTouchPoint(CLR_UINT16 source, CLR_UINT16 x, CLR_UINT16 y, CLR_INT64 time, bool fIgnoreDuplicate = FALSE);

    //

    int                             m_touchMoveIndex;
    TouchPoint*                     m_startMovePtr;
    TouchPoint                      m_tmpUpTouch;

    HAL_COMPLETION                  m_touchCompletion;
    TOUCH_PANEL_CalibrationData     m_calibrationData;
    CLR_INT32                           m_samplingTimespan;
    CLR_INT32                           m_InternalFlags;
    CLR_INT32                           m_readCount;
    CLR_INT32                           m_runavgTotalX;
    CLR_INT32                           m_runavgTotalY;
    CLR_INT32                           m_runavgCount;
    CLR_INT32                           m_runavgIndex;

    CLR_INT32                           m_head;
    CLR_INT32                           m_tail;


    enum InternalFlags
    {
        Contact_Down = 0x1,
        Contact_WasDown = 0x2,
    };
};

#endif


