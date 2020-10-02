//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "ink.h"
#include "TouchPanel.h"

#define INK_COMPLETION_TIME_USEC 10000 // 10ms - same as default touch completion


extern TouchPanelDriver g_TouchPanelDriver;
struct InkDriver g_InkDriver;
extern DisplayDriver g_DisplayDriver;
extern GraphicsDriver g_GraphicsDriver;

TOUCH_PANEL_Point InkLastPoint;

HRESULT InkDriver::Initialize()
{
    if (!m_initialized)
    {
        g_InkDriver.m_InkCompletion.InitializeForUserMode(InkContinuationRoutine);
        m_initialized = true;
        m_InkingActive = false;
        m_InkRegionInfo.Bmp = NULL;
        m_ScreenBmp.width = g_DisplayDriver.Attributes.Width;
        m_ScreenBmp.height = g_DisplayDriver.Attributes.Height;
        //   m_ScreenBmp.data = Display::GetFrameBuffer();   
        m_ScreenBmp.transparentColor = PAL_GFX_Bitmap::c_InvalidColor;
    }

    return S_OK;
}

HRESULT InkDriver::Uninitialize()
{
    if (m_initialized)
    {
        m_initialized = false;
        ResetRegion();
    }

    return S_OK;
}

HRESULT InkDriver::SetRegion(InkRegionInfo* inkRegionInfo)
{
    m_InkRegionInfo = *inkRegionInfo;

    /// Possibly validate m_InkRegionInfo data.
    m_InkingActive = true;
    m_lastx = 0xFFFF;
    m_lasty = 0xFFFF;
    m_ScreenBmp.clipping.left = m_InkRegionInfo.X1 + m_InkRegionInfo.BorderWidth;
    m_ScreenBmp.clipping.right = m_InkRegionInfo.X2 - m_InkRegionInfo.BorderWidth;
    m_ScreenBmp.clipping.top = m_InkRegionInfo.Y1 + m_InkRegionInfo.BorderWidth;
    m_ScreenBmp.clipping.bottom = m_InkRegionInfo.Y2 - m_InkRegionInfo.BorderWidth;

    if (m_InkRegionInfo.Bmp == NULL)
    {
        m_InkRegionInfo.Bmp = &m_ScreenBmp;
    }


    CLR_UINT32 flags = GetTouchPointFlags_LatestPoint | GetTouchPointFlags_UseTime | GetTouchPointFlags_UseSource;
    CLR_UINT16 source = 0;
    CLR_UINT16 x = 0;
    CLR_UINT16 y = 0;
    CLR_INT64 time = 0;

    if (g_TouchPanelDriver.GetTouchPoint(&flags, &source, &x, &y, &time) == S_OK)
    {

        if (x == TouchPointLocationFlags_ContactUp) return S_OK;

        if (x == TouchPointLocationFlags_ContactDown)
        {
            g_TouchPanelDriver.GetTouchPoint(&flags, &source, &x, &y, &time);
        }

        m_index = (flags >> 16);
        m_lastx = x;
        m_lasty = y;

        if (!m_InkCompletion.IsLinked()) m_InkCompletion.EnqueueDelta(INK_COMPLETION_TIME_USEC);
    }
    else return CLR_E_FAIL;

    return S_OK;
}

HRESULT InkDriver::ResetRegion()
{
    if (m_InkCompletion.IsLinked()) m_InkCompletion.Abort();

    m_InkingActive = false;
    m_InkRegionInfo.Bmp = NULL;

    return S_OK;
}

void InkDriver::InkContinuationRoutine(void* arg)
{
    g_InkDriver.DrawInk(arg);
}

void InkDriver::DrawInk(void* arg)
{
    if (arg == NULL) {}; // Avoid unused parameter, maybe we need it in the future?
    HRESULT hr = S_OK;
    CLR_UINT32 flags = (m_index << 16) | GetTouchPointFlags_NextPoint | GetTouchPointFlags_UseTime | GetTouchPointFlags_UseSource;
    CLR_UINT16 source = 0;
    CLR_UINT16 x = 0;
    CLR_UINT16 y = 0;
    CLR_INT64 time = 0;
    CLR_INT16 dx = m_InkRegionInfo.X1;
    CLR_INT16 dy = m_InkRegionInfo.Y1;

    while ((hr = g_TouchPanelDriver.GetTouchPoint(&flags, &source, &x, &y, &time)) == S_OK)
    {
        if (x == TouchPointLocationFlags_ContactUp) break;
        if (x == TouchPointLocationFlags_ContactDown) continue;

        if (m_lastx != 0xFFFF)
        {
            g_GraphicsDriver.DrawLineRaw(m_ScreenBmp, m_InkRegionInfo.Pen, m_lastx, m_lasty, x, y);
            if (m_InkRegionInfo.Bmp != NULL)
            {
                g_GraphicsDriver.DrawLine(*(m_InkRegionInfo.Bmp), m_InkRegionInfo.Pen, m_lastx - dx, m_lasty - dy, x - dx, y - dy);
            }
        }

        m_lastx = x;
        m_lasty = y;

        m_index = (flags >> 16);
    }

    if (x != TouchPointLocationFlags_ContactUp)
    {
        if (!m_InkCompletion.IsLinked()) m_InkCompletion.EnqueueDelta(INK_COMPLETION_TIME_USEC);
    }
}