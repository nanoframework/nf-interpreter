//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _GESTURES_H_
#define _GESTURES_H_ 1


// Gesture Support
#define GESTURE_STATE_COUNT 10
#define GESTURE_COMPLETION_TIME_USEC 50000 
#define TOUCH_PANEL_MINIMUM_GESTURE_DISTANCE      10

enum TouchGestures
{
    TouchGesture_NoGesture = 0,          //Can be used to represent an error gesture or unknown gesture

    //Standard Win7 Gestures
    TouchGesture_Begin = 1,       //Used to identify the beginning of a Gesture Sequence; App can use this to highlight UIElement or some other sort of notification.
    TouchGesture_End = 2,       //Used to identify the end of a gesture sequence; Fired when last finger involved in a gesture is removed.

    // Standard stylus (single touch) gestues
    TouchGesture_Right = 3,
    TouchGesture_UpRight = 4,
    TouchGesture_Up = 5,
    TouchGesture_UpLeft = 6,
    TouchGesture_Left = 7,
    TouchGesture_DownLeft = 8,
    TouchGesture_Down = 9,
    TouchGesture_DownRight = 10,
    TouchGesture_Tap = 11,
    TouchGesture_DoubleTap = 12,

    // Multi-touch gestures
    TouchGesture_Zoom = 114,      //Equivalent to your "Pinch" gesture
    TouchGesture_Pan = 115,      //Equivalent to your "Scroll" gesture
    TouchGesture_Rotate = 116,
    TouchGesture_TwoFingerTap = 117,
    TouchGesture_Rollover = 118,      // Press and tap               

    //Additional NetMF gestures
    TouchGesture_UserDefined = 200
};

#endif  //_GESTURES_H_
