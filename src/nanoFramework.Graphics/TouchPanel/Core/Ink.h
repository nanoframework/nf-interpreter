//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _GESTURES_H_
#define _GESTURES_H_ 1

#include "graphics.h"

struct InkRegionInfo
{
    CLR_UINT16          X1, X2, Y1, Y2; /// Inking region in screen co-ordinates.
    CLR_UINT16          BorderWidth; /// border width for inking region
    PAL_GFX_Bitmap* Bmp; /// This field may be NULL, if not NULL it must be valid pinned memory.
                    /// Other criterion is this bitmap must have size (X2-X1) x (Y2-Y1).
    GFX_Pen         Pen;
};

class InkDriver
{
public:
    bool             m_initialized;
    HAL_COMPLETION   m_InkCompletion;
    CLR_UINT32       m_index;
    InkRegionInfo    m_InkRegionInfo;
    bool             m_InkingActive;
    CLR_UINT16       m_lastx;
    CLR_UINT16       m_lasty;
    PAL_GFX_Bitmap   m_ScreenBmp;

    InkDriver()
    {
        m_initialized = false;
    }
    void DrawInk(void* arg);
    HRESULT Initialize();
    HRESULT Uninitialize();
    HRESULT SetRegion(InkRegionInfo* inkRegionInfo);
    HRESULT ResetRegion();

    static void InkContinuationRoutine(void* arg);
};


#endif  //_GESTURES_H_
