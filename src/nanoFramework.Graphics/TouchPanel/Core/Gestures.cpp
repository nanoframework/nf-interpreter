//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "Gestures.h"
#include "Graphics.h"
#include "TouchPanel.h"


CLR_INT32 GestureCurrentStateX;
CLR_INT32 GestureCurrentStateY;
TOUCH_PANEL_Point GestureLastPoint;


// It seems that events are defined in  nanoHAL_V2.h and are required to be matched with managed code events.
// This is a fudge to get it to compile
bool GestureDriver::s_initialized = FALSE;
// 50ms - allow for some touch points to accumulate before we run the gesture engine
HRESULT GestureDriver::Initialize()
{
    if (!GestureDriver::s_initialized)
    {
        GestureDriver::m_gestureCompletion.InitializeForUserMode(GestureContinuationRoutine);
        m_gestureListener.m_palEventListener = EventListener;
        m_gestureListener.m_eventMask = PAL_EVENT_TOUCH | PAL_EVENT_MOUSE;
        PalEvent_Enlist(&m_gestureListener);
        GestureDriver::s_initialized = TRUE;
    }
    return S_OK;
}

HRESULT GestureDriver::Uninitialize()
{
    if (GestureDriver::s_initialized)
    {
        GestureDriver::s_initialized = FALSE;
        if (m_gestureCompletion.IsLinked()) m_gestureCompletion.Abort();
        ResetRecognition();
    }
    return S_OK;
}

void GestureDriver::EventListener(uint32_t e, uint32_t param)
{
    if ((e & PAL_EVENT_TOUCH) && (param & TouchPanelStylusDown))
    {
        CLR_UINT32 flags = GetTouchPointFlags_LatestPoint | GetTouchPointFlags_UseTime | GetTouchPointFlags_UseSource;
        CLR_UINT16 source = 0;
        CLR_UINT16 x = 0;
        CLR_UINT16 y = 0;
        CLR_INT64 time = 0;
        if (TouchPanelDriver::GetTouchPoint(&flags, &source, &x, &y, &time) == S_OK)
        {
            ResetRecognition();
            m_index = (flags >> 16);
            if (!m_gestureCompletion.IsLinked()) m_gestureCompletion.Enqueue();
        }
    }
}
struct GestureState
{
    unsigned char NextState[8];
};
static GestureState GestureStates[GESTURE_STATE_COUNT]
// We have 8 directions (E, NE, N, NW, W, SW, S, SE) in this order. 
// States changed based on identified direction.
{
    //    E  NE N  NW W  SW S  SE
    //    0  1  2  3  4  5  6  7
       {{2, 3, 4, 5, 6, 7, 8, 9}},
       {{1, 1, 1, 1, 1, 1, 1, 1}},
       {{2, 1, 1, 1, 1, 1, 1, 1}},
       {{1, 3, 1, 1, 1, 1, 1, 1}},
       {{1, 1, 4, 1, 1, 1, 1, 1}},
       {{1, 1, 1, 5, 1, 1, 1, 1}},
       {{1, 1, 1, 1, 6, 1, 1, 1}},
       {{1, 1, 1, 1, 1, 7, 1, 1}},
       {{1, 1, 1, 1, 1, 1, 8, 1}},
       {{1, 1, 1, 1, 1, 1, 1, 9}},
};
static TouchGestures RecognizedGesture[GESTURE_STATE_COUNT] =
{
   TouchGesture_NoGesture,
   TouchGesture_NoGesture,
   TouchGesture_Right,
   TouchGesture_UpRight,
   TouchGesture_Up,
   TouchGesture_UpLeft,
   TouchGesture_Left,
   TouchGesture_DownLeft,
   TouchGesture_Down,
   TouchGesture_DownRight,
};

bool GestureDriver::ProcessPoint(CLR_UINT32 flags, CLR_UINT16 source, CLR_UINT16 x, CLR_UINT16 y, CLR_INT64 time)
{
    if (source == 0) {} // Avoid unused paramter , not used as ported, maybe in future?
    if (time == 0) {} // Avoid unused paramter , not used as ported, maybe in future?

    if (!GestureDriver::s_initialized) return FALSE;

    m_index = (flags >> 16);

    if (x == TouchPointLocationFlags_ContactUp)
    {
        CLR_UINT8 gesture = (CLR_UINT8)RecognizedGesture[m_currentState];
        if (gesture != (CLR_UINT8)TouchGesture_NoGesture)
        {
            PostManagedEvent(EVENT_GESTURE, gesture, 0, ((CLR_UINT32)m_startx << 16) | m_starty);
        }
        return FALSE;
    }

    if (x == TouchPointLocationFlags_ContactDown) return true;

    if (m_lastx == 0xFFFF)
    {
        m_lastx = x;
        m_lasty = y;

        m_startx = x;
        m_starty = y;

        return TRUE;
    }

    CLR_INT16 dx = (CLR_INT16)x - (CLR_INT16)m_lastx;
    CLR_INT16 dy = (CLR_INT16)y - (CLR_INT16)m_lasty;
    CLR_INT16 adx = abs(dx);
    CLR_INT16 ady = abs(dy);

    if ((adx + ady) >= TOUCH_PANEL_MINIMUM_GESTURE_DISTANCE)
    {
        {
            int dir = 0;
            bool diagonal = false;

            ///
            /// Diagonal line is defined as a line with angle between 22.5 degrees and 67.5 (and equivalent translations in
            /// each quadrant).  which means the abs(dx) - abs(dy) <= abs(dx) if dx > dy or abs(dy) -abs(dx) <= abs(dy) if dy > dx
            ///
            if (adx > ady)
            {
                diagonal = (adx - ady) <= (adx / 2);
            }
            else
            {
                diagonal = (ady - adx) <= (ady / 2);
            }

            if (diagonal)
            {
                if (dx > 0)
                {
                    if (dy > 0)
                        dir = 7; /// SE.
                    else
                        dir = 1; /// NE.
                }
                else
                {
                    if (dy > 0)
                        dir = 5; /// SW
                    else
                        dir = 3; /// NW.
                }
            }
            else if (adx > ady)
            {
                if (dx > 0)
                    dir = 0; /// E.
                else
                    dir = 4; /// W.
            }
            else
            {
                if (dy > 0)
                    dir = 6; /// S.
                else
                    dir = 2; /// N.
            }

            /// 
            /// The first and last points are sometimes erratic, so lets ignore the first c_IgnoreCount points
            /// and the last c_IgnoreCount points.  We do this by creating a buffer to track the last c_IgnoreCount
            /// points and only update the current state when the buffer is full.
            ///
            if (m_stateIgnoreIndex >= 2 * c_IgnoreCount)
            {
                m_currentState = m_stateIgnoreBuffer[m_stateIgnoreHead];
                m_stateIgnoreBuffer[m_stateIgnoreHead] = GestureStates[m_stateIgnoreBuffer[m_stateIgnoreTail]].NextState[dir];
                m_stateIgnoreHead++;
                if (m_stateIgnoreHead >= c_IgnoreCount) m_stateIgnoreHead = 0;
                m_stateIgnoreTail++;
                if (m_stateIgnoreTail >= c_IgnoreCount) m_stateIgnoreTail = 0;
            }
            else if (m_stateIgnoreIndex >= c_IgnoreCount)
            {
                m_stateIgnoreBuffer[m_stateIgnoreTail] =
                    GestureStates[m_stateIgnoreTail == 0 ? 0 :
                    m_stateIgnoreBuffer[m_stateIgnoreTail - 1]].NextState[dir];
                m_stateIgnoreIndex++;

                if ((m_stateIgnoreTail + 1) < c_IgnoreCount)
                {
                    m_stateIgnoreTail++;
                }
            }
            else
            {
                m_stateIgnoreIndex++;
            }

            m_lastx = x;
            m_lasty = y;

        }
    }

    return true;
}

void GestureDriver::ResetRecognition()
{
    m_currentState = 0;
    m_lastx = 0xFFFF;
    m_lasty = 0xFFFF;
    m_stateIgnoreIndex = 0;
    m_stateIgnoreHead = 0;
    m_stateIgnoreTail = 0;
    m_stateIgnoreBuffer[0] = 0;
}

void GestureDriver::GestureContinuationRoutine(void* arg)
{
    if (arg == NULL) {}; // avoid unused parameter, maybe used in the future
    HRESULT hr = S_OK;
    CLR_UINT32 flags = (m_index << 16) | GetTouchPointFlags_NextPoint | GetTouchPointFlags_UseTime | GetTouchPointFlags_UseSource;
    CLR_UINT16 source = 0;
    CLR_UINT16 x = 0;
    CLR_UINT16 y = 0;
    CLR_INT64 time = 0;

    while ((hr = TouchPanelDriver::GetTouchPoint(&flags, &source, &x, &y, &time)) == S_OK)
    {
        if (!ProcessPoint(flags, source, x, y, time)) break;
    }

    if (x == TouchPointLocationFlags_ContactUp)
    {
        if (m_gestureCompletion.IsLinked()) m_gestureCompletion.Abort();
    }
    else
    {
        if (!m_gestureCompletion.IsLinked()) m_gestureCompletion.EnqueueDelta(GESTURE_COMPLETION_TIME_USEC);
    }
}
