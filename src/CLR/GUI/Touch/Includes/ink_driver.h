////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <nanohal.h>
#include <ink_decl.h>


class InkDriver
{
public:
    bool             m_initialized;
    HAL_COMPLETION   m_InkCompletion;
    CLR_UINT32           m_index;
    InkRegionInfo    m_InkRegionInfo;
    bool             m_InkingActive;
    CLR_UINT16           m_lastx;
    CLR_UINT16           m_lasty;
    PAL_GFX_Bitmap   m_ScreenBmp;

    InkDriver()
    {
        m_initialized = false;
    }
    
    void DrawInk(void *arg);    
    HRESULT Initialize();
    HRESULT Uninitialize();
    HRESULT SetRegion(InkRegionInfo* inkRegionInfo);
    HRESULT ResetRegion();

    static void InkContinuationRoutine(void *arg);

};

extern InkDriver g_InkDriver;


