//
//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "TouchPanel.h"
#include "TouchDevice.h"
#include "TouchInterface.h"
#include <nanoPAL_Events.h>

#define ONE_MHZ  1000000
#define abs(a) (((a) < 0) ? -(a) : (a))
#define sign(a) (((a) < 0) ? -1 : 1)

#define TOUCH_POINT_BUFFER_SIZE 200
#define TOUCH_POINT_RUNNINGAVG_BUFFER_SIZE 4

TOUCH_PANEL_CalibrationData g_TouchPanel_Calibration_Config = { 1, 1, 0, 0, 1, 1 };
TOUCH_PANEL_SamplingSettings g_TouchPanel_Sampling_Settings = { 0, 1, 500 * 500, FALSE, {50, 200, 100, TOUCH_PANEL_SAMPLE_MS_TO_TICKS(50)} }; // the emulator is "perfect" hardware, so we don't need to ignore readings

TouchPoint g_PAL_TouchPointBuffer[TOUCH_POINT_BUFFER_SIZE];
CLR_UINT32 g_PAL_TouchPointBufferSize = TOUCH_POINT_BUFFER_SIZE;
TOUCH_PANEL_Point g_PAL_RunningAvg_Buffer[TOUCH_POINT_RUNNINGAVG_BUFFER_SIZE];
CLR_UINT32 g_PAL_RunningAvg_Buffer_Size = TOUCH_POINT_RUNNINGAVG_BUFFER_SIZE;

TOUCH_PANEL_CalibrationData g_TouchPanel_DefaultCalibration_Config = { 1, 1, 0, 0, 1, 1 };

TouchPanelDriver g_TouchPanelDriver;


extern TouchDevice g_TouchDevice;
TouchInterface g_TouchInterface;


/// Divide a by b, and return rounded integer.
CLR_INT32 RoundDiv(CLR_INT32 a, CLR_INT32 b)
{
    if (b == 0)
        return 0xFFFF;

    CLR_INT32 d = a / b;
    CLR_INT32 r = abs(a) % abs(b);

    if ((r * 2) > abs(a))
    {
        d = (abs(d) + 1) * sign(b) * sign(a);
    }

    return d;
}

HRESULT TouchPanelDriver::Initialize()
{
    g_TouchPanelDriver.m_samplingTimespan = ONE_MHZ / g_TouchPanel_Sampling_Settings.SampleRate.CurrentSampleRateSetting;

    g_TouchPanelDriver.m_calibrationData = g_TouchPanel_Calibration_Config;
    g_TouchPanelDriver.m_InternalFlags = 0;

    g_TouchPanelDriver.m_head = 0;
    g_TouchPanelDriver.m_tail = 0;

    g_TouchPanelDriver.m_runavgTotalX = 0;
    g_TouchPanelDriver.m_runavgTotalY = 0;
    g_TouchPanelDriver.m_runavgCount = 0;
    g_TouchPanelDriver.m_runavgIndex = 0;

    g_TouchPanelDriver.m_startMovePtr = NULL;

    g_TouchPanelDriver.m_touchMoveIndex = 0;

    /// Following four should be done at HAL_Initialize(), currently an issue blocking the move.
    //PalEvent_Initialize();
    //Gesture_Initialize();
    //Ink_Initialize();

    /// Enable the touch hardware.
    if (!g_TouchDevice.Enable(TouchIsrProc))
    {
        return CLR_E_FAIL;
    }

    g_TouchPanelDriver.m_touchCompletion.InitializeForISR(TouchPanelDriver::TouchCompletion, NULL);
    /// At this point we should be ready to recieve touch inputs.    

    return S_OK;
}

HRESULT TouchPanelDriver::Uninitialize()
{
    if (g_TouchPanelDriver.m_touchCompletion.IsLinked())
    {
        g_TouchPanelDriver.m_touchCompletion.Abort();
    }

    g_TouchDevice.Disable();

    return S_OK;
}

/// Calibrate an uncalibrated point.
void TouchPanelDriver::TouchPanelCalibratePoint(CLR_INT32 UncalX, CLR_INT32 UncalY, CLR_INT32* pCalX, CLR_INT32* pCalY)
{
    /// Doing simple linear calibration for now.
    /// ASSUMPTION: uncalibrated x, y touch co-ordinates are independent. In reality this is not correct.
    /// We should consider doing 2D calibration that takes dependeng_Cy issue into account.

    *pCalX = RoundDiv((g_TouchPanelDriver.m_calibrationData.Mx * UncalX + g_TouchPanelDriver.m_calibrationData.Cx), g_TouchPanelDriver.m_calibrationData.Dx);
    *pCalY = RoundDiv((g_TouchPanelDriver.m_calibrationData.My * UncalY + g_TouchPanelDriver.m_calibrationData.Cy), g_TouchPanelDriver.m_calibrationData.Dy);

    /// FUTURE - 07/10/2008-munirula- Negative co-ords are meaningful or meaningless. I am leaning
    /// towards meaningless for now, but this needs further thought which I agree.
    if (*pCalX < 0)
        *pCalX = 0;

    if (*pCalY < 0)
        *pCalY = 0;
}

HRESULT TouchPanelDriver::GetDeviceCaps(unsigned int iIndex, void* lpOutput)
{
    if (lpOutput == NULL)
    {
        return FALSE;
    }

    switch (iIndex)
    {
    case TOUCH_PANEL_SAMPLE_RATE_ID:
    {
        TOUCH_PANEL_SAMPLE_RATE* pTSR = (TOUCH_PANEL_SAMPLE_RATE*)lpOutput;

        pTSR->SamplesPerSecondLow = g_TouchPanel_Sampling_Settings.SampleRate.SamplesPerSecondLow;
        pTSR->SamplesPerSecondHigh = g_TouchPanelDriver.SampleRate.SamplesPerSecondHigh;
        pTSR->CurrentSampleRateSetting = g_TouchPanelDriver.SampleRate.CurrentSampleRateSetting;
        pTSR->MaxTimeForMoveEvent_ticks = g_TouchPanelDriver.SampleRate.MaxTimeForMoveEvent_ticks;
    }
    break;

    case TOUCH_PANEL_CALIBRATION_POINT_COUNT_ID:
    {
        TOUCH_PANEL_CALIBRATION_POINT_COUNT* pTCPC = (TOUCH_PANEL_CALIBRATION_POINT_COUNT*)lpOutput;

        pTCPC->flags = 0;
        pTCPC->cCalibrationPoints = 5;
    }
    break;

    case TOUCH_PANEL_CALIBRATION_POINT_ID:
        return(g_TouchPanelDriver.CalibrationPointGet((TOUCH_PANEL_CALIBRATION_POINT*)lpOutput));

    default:
        return FALSE;
    }

    return TRUE;
}

HRESULT TouchPanelDriver::ResetCalibration()
{
    g_TouchPanelDriver.m_calibrationData = g_TouchPanel_DefaultCalibration_Config;

    return S_OK;
}

HRESULT TouchPanelDriver::SetCalibration(int pointCount, CLR_INT16* sx, CLR_INT16* sy, CLR_INT16* ux, CLR_INT16* uy)
{
    if (pointCount != 5)
        return CLR_E_FAIL;

    /// Calculate simple 1D calibration parameters.
    g_TouchPanelDriver.m_calibrationData.Mx = sx[2] - sx[3];
    g_TouchPanelDriver.m_calibrationData.Cx = ux[2] * sx[3] - ux[3] * sx[2];
    g_TouchPanelDriver.m_calibrationData.Dx = ux[2] - ux[3];

    g_TouchPanelDriver.m_calibrationData.My = sy[1] - sy[2];
    g_TouchPanelDriver.m_calibrationData.Cy = uy[1] * sy[2] - uy[2] * sy[1];
    g_TouchPanelDriver.m_calibrationData.Dy = uy[1] - uy[2];

    return S_OK;
}

void TouchPanelDriver::TouchCompletion(void* arg)
{
    if (arg == NULL) {};  // Avoid the unused parameter, and maybe in future is it needed? 
    /// Completion routine, called in every 10ms or so, when we are actively sampling stylus.
    g_TouchPanelDriver.PollTouchPoint();
}

void TouchPanelDriver::PollTouchPoint()
{
    TOUCH_PANEL_SAMPLE_FLAGS flags;
    CLR_INT32 x = 0;
    CLR_INT32 y = 0;
    CLR_INT32 ux = 0; /// Uncalibrated x.
    CLR_INT32 uy = 0; /// Uncalibrated y.
    CLR_INT32 source = 0;
    bool fProcessUp = false;

    bool ignoreDuplicates = true;
    CLR_INT64 time = ::HAL_Time_CurrentTime();


/// To be revisited::    GLOBAL_LOCK(isr);

    /// Get the point information from driver.
    GetPoint(&flags, &source, &ux, &uy);

    if ((flags & TouchSampleValidFlag) == TouchSampleValidFlag)
    {
        /// Calibrate a point;
        /// Driver should be doing all filter/averaging of the points.
        TouchPanelCalibratePoint(ux, uy, &x, &y);

        TouchPoint* point = NULL;

        if ((g_TouchPanelDriver.m_InternalFlags & Contact_Down) && (!(g_TouchPanelDriver.m_InternalFlags & Contact_WasDown)))
        {
            // NOTE: Added ignore duplicate, not present in original ported code?
            AddTouchPoint(0, TouchPointLocationFlags_ContactDown, TouchPointLocationFlags_ContactDown, time, ignoreDuplicates);
            /// Stylus down.
            PalEventDriver::PostEvent(PAL_EVENT_TOUCH, TouchPanelStylusDown);

            if (NULL != (point = AddTouchPoint(0, x, y, time, ignoreDuplicates)))
            {
                PostManagedEvent(EVENT_TOUCH, TouchPanelStylusDown, 1, (CLR_UINT32)point);
            }

            g_TouchPanelDriver.m_InternalFlags |= Contact_WasDown;
        }
        else if ((!(g_TouchPanelDriver.m_InternalFlags & Contact_Down)) && (g_TouchPanelDriver.m_InternalFlags & Contact_WasDown))
        {
            fProcessUp = true;

            point = AddTouchPoint(0, x, y, time, ignoreDuplicates);
        }
        else if ((g_TouchPanelDriver.m_InternalFlags & Contact_Down) && (g_TouchPanelDriver.m_InternalFlags & Contact_WasDown))
        {
            /// Stylus Move.
            if (NULL != (point = AddTouchPoint(0, x, y, time, ignoreDuplicates)))
            {
                if (m_startMovePtr == NULL)
                {
                    m_startMovePtr = point;
                    m_touchMoveIndex = 1;
                }
                else
                {
                    if (((CLR_UINT32)m_startMovePtr > (CLR_UINT32)point) ||
                        (time - m_startMovePtr->time) > (g_TouchPanel_Sampling_Settings.SampleRate.MaxTimeForMoveEvent_ticks))
                    {
                        PostManagedEvent(EVENT_TOUCH, TouchPanelStylusMove, m_touchMoveIndex, (CLR_UINT32)m_startMovePtr);

                        m_startMovePtr = NULL;
                        m_touchMoveIndex = 0;
                    }
                    else if (m_startMovePtr != point)
                    {
                        m_touchMoveIndex++;
                    }
                }
            }
            // we will get here if the stylus is in a TouchDown state but hasn't moved, so if we don't have a current move ptr, then
            // set the start move ptr to the latest point
            else if (m_startMovePtr == NULL)
            {
                CLR_UINT32 touchflags = GetTouchPointFlags_LatestPoint | GetTouchPointFlags_UseTime | GetTouchPointFlags_UseSource;

                if (SUCCEEDED(GetTouchPoint(&touchflags, &point)))
                {
                    m_startMovePtr = point;
                    m_touchMoveIndex = 1;
                }
            }
            // We should always send a move event if we cross the max time boundary for a move event, even if it is just one point
            else if ((time - m_startMovePtr->time) > (g_TouchPanel_Sampling_Settings.SampleRate.MaxTimeForMoveEvent_ticks))
            {
                PostManagedEvent(EVENT_TOUCH, TouchPanelStylusMove, m_touchMoveIndex, (CLR_UINT32)m_startMovePtr);

                if (m_touchMoveIndex > 1)
                {
                    m_startMovePtr = &m_startMovePtr[m_touchMoveIndex - 1];
                    m_touchMoveIndex = 1;
                }

                m_startMovePtr->time = time;
            }
        }
    }

    if ((!(g_TouchPanelDriver.m_InternalFlags & Contact_Down)) && (g_TouchPanelDriver.m_InternalFlags & Contact_WasDown))
    {
        fProcessUp = true;
    }

    if (fProcessUp)
    {
        CLR_UINT32 touchflags = GetTouchPointFlags_LatestPoint | GetTouchPointFlags_UseTime | GetTouchPointFlags_UseSource;

        TouchPoint* point = NULL;

        /// Only send a move event if we have substantial data (more than two items) otherwise, the
        /// TouchUp event should suffice
        if (m_touchMoveIndex > 2 && m_startMovePtr != NULL)
        {
            PostManagedEvent(EVENT_TOUCH, TouchPanelStylusMove, m_touchMoveIndex, (CLR_UINT32)m_startMovePtr);
        }

        m_startMovePtr = NULL;
        m_touchMoveIndex = 0;

        if (FAILED(GetTouchPoint(&touchflags, &point)))
        {
            point = NULL;
        }

        /// Now add special contactup delimeter.
        AddTouchPoint(0, TouchPointLocationFlags_ContactUp, TouchPointLocationFlags_ContactUp, time, ignoreDuplicates);

        /// Stylus up.
//        PalEvent_Post((unsigned int)PAL_EVENT_TOUCH, (unsigned int)TouchPanelStylusUp);

        g_palEventDriver.PostEvent(PAL_EVENT_TOUCH,TouchPanelStylusUp);



        //Make a copy of the point for the Microsoft.SPOT.Touch handler.
        if (point == NULL)
        {
            m_tmpUpTouch.contact = 0;
            m_tmpUpTouch.time = time;
            m_tmpUpTouch.location = 0;

            point = &m_tmpUpTouch;
        }

        PostManagedEvent(EVENT_TOUCH, TouchPanelStylusUp, 1, (CLR_UINT32)point);

        g_TouchPanelDriver.m_InternalFlags &= ~Contact_WasDown;
    }

    /// Schedule or unschedule completion.
    if (!g_TouchPanelDriver.m_touchCompletion.IsLinked())
    {
        if (g_TouchPanelDriver.m_InternalFlags & Contact_Down)
        {
            g_TouchPanelDriver.m_touchCompletion.EnqueueDelta(g_TouchPanelDriver.m_samplingTimespan);
        }
    }
    else
    {
        if (!(g_TouchPanelDriver.m_InternalFlags & Contact_Down))
        {
            g_TouchPanelDriver.m_touchCompletion.Abort();
        }
    }
}

TouchPoint* TouchPanelDriver::AddTouchPoint(CLR_UINT16 source, CLR_UINT16 x, CLR_UINT16 y, CLR_INT64 time, bool fIgnoreDuplicate)
{
    static CLR_UINT16 lastAddedX = 0xFFFF;
    static CLR_UINT16 lastAddedY = 0xFFFF;

/// To be revisited::    GLOBAL_LOCK(isr);

    if ((x == TouchPointLocationFlags_ContactUp) || (x == TouchPointLocationFlags_ContactDown))
    {
        /// Reset the points.
        lastAddedX = 0xFFFF;
        lastAddedY = 0xFFFF;

        m_runavgIndex = 0;
        m_runavgCount = 0;
        m_runavgTotalX = 0;
        m_runavgTotalY = 0;
    }
    else
    {
        if (lastAddedX != 0xFFFF)
        {
            /// dist2 is distance squared.
            CLR_INT32 dx = abs(x - lastAddedX);
            CLR_INT32 dy = abs(y - lastAddedY);
            CLR_INT32 dist2 = dx * dx + dy * dy;

            /// Ignore this point if it is too far from last point.
            if (dist2 > g_TouchPanel_Sampling_Settings.MaxFilterDistance) return NULL;
        }

        if (g_PAL_RunningAvg_Buffer_Size > 1)
        {
            if (m_runavgIndex >= (CLR_INT32)g_PAL_RunningAvg_Buffer_Size)
                m_runavgIndex = 0;

            if (m_runavgCount >= (CLR_INT32)g_PAL_RunningAvg_Buffer_Size)
            {
                m_runavgTotalX -= g_PAL_RunningAvg_Buffer[m_runavgIndex].sx;
                m_runavgTotalY -= g_PAL_RunningAvg_Buffer[m_runavgIndex].sy;
            }
            else
            {
                m_runavgCount++;
            }

            m_runavgTotalX += x;
            m_runavgTotalY += y;

            g_PAL_RunningAvg_Buffer[m_runavgIndex].sx = x;
            g_PAL_RunningAvg_Buffer[m_runavgIndex].sy = y;

            m_runavgIndex++;

            x = m_runavgTotalX / m_runavgCount;
            y = m_runavgTotalY / m_runavgCount;
        }

        ///
        /// This is mainly intended for TouchMove events.  We don't want to add duplicate points
        /// if the touch down is not moving
        ///
        if (fIgnoreDuplicate)
        {
            CLR_UINT32 dx = abs(x - lastAddedX);
            CLR_UINT32 dy = abs(y - lastAddedY);

            if (dx <= 2 && dy <= 2) return NULL;
        }

        lastAddedX = x;
        lastAddedY = y;
    }

    CLR_UINT32 location = (x & 0x3FFF) | ((y & 0x3FFF) << 14) | (source << 28);
    CLR_UINT32 contact = TouchPointContactFlags_Primary | TouchPointContactFlags_Pen;
    TouchPoint& point = g_PAL_TouchPointBuffer[m_tail];
    point.time = time;
    point.location = location;
    point.contact = contact;

    if (g_PAL_TouchPointBufferSize > 1)
    {
        m_tail++;

        if (m_tail >= (CLR_INT32)g_PAL_TouchPointBufferSize) m_tail = 0;

        if (m_tail == m_head)
        {
            m_head++;

            if (m_head >= (CLR_INT32)g_PAL_TouchPointBufferSize) m_head = 0;
        }
    }
    return &point;
}

void TouchPanelDriver::TouchIsrProc(GPIO_PIN pin, bool pinState)
{
    (void)pin;

    // Question does this method work?
    if (pinState == g_TouchPanel_Sampling_Settings.ActivePinStateForTouchDown)
    {
        g_TouchPanelDriver.m_InternalFlags |= Contact_Down; // Toggle contact flag.
        g_TouchPanelDriver.m_readCount = 0;
    }
    else
    {
        g_TouchPanelDriver.m_InternalFlags &= ~Contact_Down; // Toggle contact flag.        
    }

    if (g_TouchPanelDriver.m_touchCompletion.IsLinked())
    {
        g_TouchPanelDriver.m_touchCompletion.Abort();
    }
    g_TouchPanelDriver.m_touchCompletion.EnqueueDelta(0);
}

HRESULT TouchPanelDriver::GetTouchPoints(int* pointCount, CLR_INT16* sx, CLR_INT16* sy)
{
    if (pointCount == NULL) {}; // Avoid unused parameter, maybe used in the future?
    if (sx == NULL) {}; // Avoid unused parameter, maybe used in the future?
    if (sy == NULL) {}; // Avoid unused parameter, maybe used in the future?

// To be revisited::    GLOBAL_LOCK(isr);
    return S_OK;
}

HRESULT TouchPanelDriver::GetSetTouchInfo(CLR_UINT32 flags, CLR_INT32* param1, CLR_INT32* param2, CLR_INT32* param3)
{
    if (flags & TouchInfo_Set) /// SET.
    {
        if (flags & TouchInfo_SamplingDistance)
        {
            CLR_INT32 samplesPerSecond = ONE_MHZ / *param1;

            /// Minimum of 500us otherwise the system will be overrun
            if (samplesPerSecond >= g_TouchPanel_Sampling_Settings.SampleRate.SamplesPerSecondLow &&
                samplesPerSecond <= g_TouchPanel_Sampling_Settings.SampleRate.SamplesPerSecondHigh)
            {
                g_TouchPanelDriver.m_samplingTimespan = *param1;

                g_TouchPanel_Sampling_Settings.SampleRate.CurrentSampleRateSetting = samplesPerSecond;
            }
            else
            {
                return CLR_E_OUT_OF_RANGE;
            }
        }
        else if (flags & TouchInfo_StylusMoveFrequency)
        {
            CLR_INT32 ms_per_touchmove_event = *param1; // *param1 is in milliseconds
            CLR_INT32 min_ms_per_touchsample = 1000 / g_TouchPanel_Sampling_Settings.SampleRate.SamplesPerSecondLow;
            CLR_INT32 ticks;

            // zero value indicates turning move notifications based on time off
            if (ms_per_touchmove_event == 0)         ticks = 0x7FFFFFFF;
            // min_ms_per_touchsample is the sample frequency for the touch screen driver
            // Touch Move events are queued up and sent at the given time frequency (StylusMoveFrequency)
            // We should not set the move frequency to be less than the sample frequency, otherwise there
            // would be no data available occassionally.
            else
            {
                ticks = TOUCH_PANEL_SAMPLE_MS_TO_TICKS(ms_per_touchmove_event);
                if (ticks < min_ms_per_touchsample) return CLR_E_OUT_OF_RANGE;
            }

            g_TouchPanel_Sampling_Settings.SampleRate.MaxTimeForMoveEvent_ticks = ticks;
        }
        else if (flags & TouchInfo_SamplingReadsToIgnore)
        {
            g_TouchDevice.ReadsToIgnore = *param1;
        }
        else if (flags & TouchInfo_SamplingReadsPerSample)
        {
            g_TouchDevice.ReadsPerSample = *param1;
        }
        else if (flags & TouchInfo_SamplingFilterDistance)
        {
            g_TouchDevice.MaxFilterDistance = *param1;
        }
        else
        {
            return CLR_E_INVALID_PARAMETER;
        }
    }
    else /// GET
    {
        *param1 = 0;
        *param2 = 0;
        *param3 = 0;

        if (flags & TouchInfo_LastTouchPoint)
        {
            CLR_UINT16 x = 0;
            CLR_UINT16 y = 0;
            CLR_UINT32 flags = GetTouchPointFlags_LatestPoint | GetTouchPointFlags_UseTime | GetTouchPointFlags_UseSource;
            CLR_UINT16 source = 0;
            CLR_INT64 time = 0;
            GetTouchPoint(&flags, &source, &x, &y, &time);
            *param1 = x;
            *param2 = y;
        }
        else if (flags & TouchInfo_SamplingDistance)
        {
            *param1 = g_TouchPanelDriver.m_samplingTimespan;
        }
        else if (flags & TouchInfo_StylusMoveFrequency)
        {
            int ticks = g_TouchPanel_Sampling_Settings.SampleRate.MaxTimeForMoveEvent_ticks;

            if (ticks == 0x7FFFFFFF)*param1 = 0;
            else                    *param1 = ticks / TIME_CONVERSION__TO_MILLISECONDS;
        }
        else if (flags & TouchInfo_SamplingReadsToIgnore)
        {
            *param1 = g_TouchDevice.ReadsToIgnore;
        }
        else if (flags & TouchInfo_SamplingReadsPerSample)
        {
            *param1 = g_TouchDevice.ReadsPerSample;
        }
        else if (flags & TouchInfo_SamplingFilterDistance)
        {
            *param1 = g_TouchDevice.MaxFilterDistance;
        }
        else
        {
            return CLR_E_INVALID_PARAMETER;
        }
    }

    return S_OK;
}

HRESULT TouchPanelDriver::GetTouchPoint(CLR_UINT32* flags, TouchPoint** point)
{
    CLR_UINT8 searchFlag = *flags & 0xF;
    CLR_UINT8 conditionalFlag = *flags & 0xF0;
    CLR_INT32 index = 0;


// To be revisited::    GLOBAL_LOCK(isr);

    if ((g_TouchPanelDriver.m_head == g_TouchPanelDriver.m_tail) && g_PAL_TouchPointBufferSize > 1)
        return CLR_E_FAIL;

    if (searchFlag == GetTouchPointFlags_LatestPoint)
    {
        index = g_TouchPanelDriver.m_tail > 0 ? (g_TouchPanelDriver.m_tail - 1) : (g_PAL_TouchPointBufferSize - 1);
    }
    else if (searchFlag == GetTouchPointFlags_EarliestPoint)
    {
        index = g_TouchPanelDriver.m_head;
    }
    else if (searchFlag == GetTouchPointFlags_NextPoint)
    {
        if (conditionalFlag & GetTouchPointFlags_UseTime)
        {
            index = (*flags >> 16);
            index = (index + 1) % g_PAL_TouchPointBufferSize;
            if ((index == g_TouchPanelDriver.m_tail) && g_PAL_TouchPointBufferSize > 1)
                return CLR_E_FAIL;
        }
        else return CLR_E_NOT_SUPPORTED;
    }

    *point = &g_PAL_TouchPointBuffer[index];

    *flags &= 0xFFFF; /// Clear high 16 bit.
    *flags |= (index << 16);

    return S_OK;
}

HRESULT TouchPanelDriver::GetTouchPoint(CLR_UINT32* flags, CLR_UINT32* location, CLR_INT64* time)
{
    TouchPoint* point;

    HRESULT hr = GetTouchPoint(flags, &point);
    if (hr != S_OK)
    {
        return hr;
    }

    *location = point->location;
    *time = point->time;

    return S_OK;
}

HRESULT TouchPanelDriver::GetTouchPoint(CLR_UINT32* flags, CLR_UINT16* source, CLR_UINT16* x, CLR_UINT16* y, CLR_INT64* time)
{
    CLR_UINT32 location = 0;
    HRESULT hr = GetTouchPoint(flags, &location, time);
    if (hr != S_OK)
    {
        return hr;
    }

    *source = (location >> 28);
    *x = location & 0x3FFF;
    *y = (location >> 14) & 0x3FFF;

    return S_OK;
}

void TouchPanelDriver::GetPoint(TOUCH_PANEL_SAMPLE_FLAGS* pTipState, int* pSource, int* pUnCalX, int* pUnCalY)
{
    *pTipState = 0;
    *pUnCalX = 0;
    *pUnCalY = 0;
    *pSource = 0;

    static bool  stylusDown = false;

    /// Apparently there's a lot of noise from the touch hardware. We will take several
    /// independent measures to compensate for them:
    /// 1. Settle down time (instead of reading right away, wait few moments) --> ReadsToIgnore
    /// 2. Read multiple samples (read a number of them, and then take average) --> ReadsPerSample.
    ///

    CLR_UINT16 i = 0;
    CLR_UINT16 totalReads = g_TouchDevice.ReadsToIgnore + g_TouchDevice.ReadsPerSample;

    CLR_UINT32 x = -1;
    CLR_UINT32 y = -1;

    CLR_INT32 validReadCount = 0;

    CLR_UINT32 d1 = 0xFFFF;
    CLR_UINT32 d2 = 0;

    for (i = 0; i < totalReads; i++)
    {
        d2 = d1;
        TouchPointDevice point = g_TouchDevice.GetPoint();
        d1 = point.x;
        d1 <<= 8;
        d1 |= point.y;
        d1 >>= 3;

        if (d1 == d2)
            break;
    }

    y = d1;

    d1 = 0xFFFF;
    d2 = 0;
    for (i = 0; i < g_TouchDevice.ReadsPerSample; i++)
    {
        d2 = d1;
        TouchPointDevice point = g_TouchDevice.GetPoint();
        d1 = point.x;
        d1 <<= 8;
        d1 |= point.y;
        d1 >>= 3;

        if (d1 == d2)
            break;
    }

    x = d1;

    if (x >= 3700)
    {
        validReadCount = 0;
    }
    else
    {
        validReadCount = 1;
    }


    if (stylusDown)
    {
        *pTipState |= TouchSamplePreviousDownFlag;
    }

    if (validReadCount > 0)
    {
        *pTipState |= TouchSampleValidFlag;
        *pUnCalX = x;
        *pUnCalY = y;
        *pTipState |= TouchSampleDownFlag;
        stylusDown = true;
    }
    else
    {
        *pUnCalX = -1;
        *pUnCalY = -1;
        stylusDown = false;
    }
}

bool TouchPanelDriver::CalibrationPointGet(TOUCH_PANEL_CALIBRATION_POINT* pTCP)
{

    CLR_INT32   cDisplayWidth = pTCP->cDisplayWidth;
    CLR_INT32   cDisplayHeight = pTCP->cDisplayHeight;

    int CalibrationRadiusX = cDisplayWidth / 20;
    int CalibrationRadiusY = cDisplayHeight / 20;

    switch (pTCP->PointNumber)
    {
    case    0:
        pTCP->CalibrationX = cDisplayWidth / 2;
        pTCP->CalibrationY = cDisplayHeight / 2;
        break;

    case    1:
        pTCP->CalibrationX = CalibrationRadiusX * 2;
        pTCP->CalibrationY = CalibrationRadiusY * 2;
        break;

    case    2:
        pTCP->CalibrationX = CalibrationRadiusX * 2;
        pTCP->CalibrationY = cDisplayHeight - CalibrationRadiusY * 2;
        break;

    case    3:
        pTCP->CalibrationX = cDisplayWidth - CalibrationRadiusX * 2;
        pTCP->CalibrationY = cDisplayHeight - CalibrationRadiusY * 2;
        break;

    case    4:
        pTCP->CalibrationX = cDisplayWidth - CalibrationRadiusX * 2;
        pTCP->CalibrationY = CalibrationRadiusY * 2;
        break;

    default:
        pTCP->CalibrationX = cDisplayWidth / 2;
        pTCP->CalibrationY = cDisplayHeight / 2;

        return FALSE;
    }

    return TRUE;
}


