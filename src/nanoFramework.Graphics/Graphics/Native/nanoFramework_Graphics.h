//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef NANOFRAMEWORK_GRAPHICS_H
#define NANOFRAMEWORK_GRAPHICS_H

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>
#include <corlib_native.h>

// This is added as well from the copy/paste of the stub
#include "Graphics.h"

typedef enum __nfpack Dock
{
    Dock_Left = 0,
    Dock_Top = 1,
    Dock_Right = 2,
    Dock_Bottom = 3,
} Dock;

typedef enum __nfpack Orientation
{
    Orientation_Horizontal = 0,
    Orientation_Vertical = 1,
} Orientation;

typedef enum __nfpack ScrollingStyle
{
    ScrollingStyle_First = 0,
    ScrollingStyle_LineByLine = 0,
    ScrollingStyle_PageByPage = 1,
    ScrollingStyle_Last = 1,
} ScrollingStyle;

typedef enum __nfpack HorizontalAlignment
{
    HorizontalAlignment_Left = 0,
    HorizontalAlignment_Center = 1,
    HorizontalAlignment_Right = 2,
    HorizontalAlignment_Stretch = 3,
} HorizontalAlignment;

typedef enum __nfpack BrushMappingMode
{
    BrushMappingMode_Absolute = 0,
    BrushMappingMode_RelativeToBoundingBox = 1,
} BrushMappingMode;

typedef enum __nfpack Color
{
    Color_AliceBlue = 16775408,
    Color_AntiqueWhite = 14150650,
    Color_Aqua = 16776960,
    Color_Aquamarine = 13959039,
    Color_Azure = 16777200,
    Color_Beige = 14480885,
    Color_Bisque = 12903679,
    Color_Black = 0,
    Color_BlanchedAlmond = 13495295,
    Color_Blue = 16711680,
    Color_BlueViolet = 14822282,
    Color_Brown = 2763429,
    Color_BurlyWood = 8894686,
    Color_CadetBlue = 10526303,
    Color_Chartreuse = 65407,
    Color_Chocolate = 1993170,
    Color_Coral = 5275647,
    Color_CornflowerBlue = 15570276,
    Color_Cornsilk = 14481663,
    Color_Crimson = 3937500,
    Color_Cyan = 16776960,
    Color_DarkBlue = 9109504,
    Color_DarkCyan = 9145088,
    Color_DarkGoldenrod = 755384,
    Color_DarkGray = 11119017,
    Color_DarkGreen = 25600,
    Color_DarkKhaki = 7059389,
    Color_DarkMagenta = 9109643,
    Color_DarkOliveGreen = 3107669,
    Color_DarkOrange = 36095,
    Color_DarkOrchid = 13382297,
    Color_DarkRed = 139,
    Color_DarkSalmon = 8034025,
    Color_DarkSeaGreen = 9419919,
    Color_DarkSlateBlue = 9125192,
    Color_DarkSlateGray = 5197615,
    Color_DarkTurquoise = 13749760,
    Color_DarkViolet = 13828244,
    Color_DeepPink = 9639167,
    Color_DeepSkyBlue = 16760576,
    Color_DimGray = 6908265,
    Color_DodgerBlue = 16748574,
    Color_Firebrick = 2237106,
    Color_FloralWhite = 15792895,
    Color_ForestGreen = 2263842,
    Color_Gainsboro = 14474460,
    Color_GhostWhite = 16775416,
    Color_Gold = 55295,
    Color_Goldenrod = 2139610,
    Color_Gray = 8421504,
    Color_Green = 32768,
    Color_GreenYellow = 3145645,
    Color_Honeydew = 15794160,
    Color_HotPink = 11823615,
    Color_IndianRed = 6053069,
    Color_Indigo = 8519755,
    Color_Ivory = 15794175,
    Color_Khaki = 9234160,
    Color_Lavender = 16443110,
    Color_LavenderBlush = 16118015,
    Color_LawnGreen = 64636,
    Color_LemonChiffon = 13499135,
    Color_LightBlue = 15128749,
    Color_LightCoral = 8421616,
    Color_LightCyan = 16777184,
    Color_LightGoldenrodYellow = 13826810,
    Color_LightGray = 13882323,
    Color_LightGreen = 9498256,
    Color_LightPink = 12695295,
    Color_LightSalmon = 8036607,
    Color_LightSeaGreen = 11186720,
    Color_LightSkyBlue = 16436871,
    Color_LightSlateGray = 10061943,
    Color_LightSteelBlue = 14599344,
    Color_LightYellow = 14745599,
    Color_Lime = 65280,
    Color_LimeGreen = 3329330,
    Color_Linen = 15134970,
    Color_Magenta = 16711935,
    Color_Maroon = 128,
    Color_MediumAquamarine = 11193702,
    Color_MediumBlue = 13434880,
    Color_MediumOrchid = 13850042,
    Color_MediumPurple = 14381203,
    Color_MediumSeaGreen = 7451452,
    Color_MediumSlateBlue = 15624315,
    Color_MediumSpringGreen = 10156544,
    Color_MediumTurquoise = 13422920,
    Color_MediumVioletRed = 8721863,
    Color_MidnightBlue = 7346457,
    Color_MintCream = 16449525,
    Color_MistyRose = 14804223,
    Color_Moccasin = 11920639,
    Color_NavajoWhite = 11394815,
    Color_Navy = 8388608,
    Color_OldLace = 15136253,
    Color_Olive = 32896,
    Color_OliveDrab = 2330219,
    Color_Orange = 42495,
    Color_OrangeRed = 17919,
    Color_Orchid = 14053594,
    Color_PaleGoldenrod = 11200750,
    Color_PaleGreen = 10025880,
    Color_PaleTurquoise = 15658671,
    Color_PaleVioletRed = 9662683,
    Color_PapayaWhip = 14020607,
    Color_PeachPuff = 12180223,
    Color_Peru = 4163021,
    Color_Pink = 13353215,
    Color_Plum = 14524637,
    Color_PowderBlue = 15130800,
    Color_Purple = 8388736,
    Color_Red = 255,
    Color_RosyBrown = 9408444,
    Color_RoyalBlue = 14772545,
    Color_SaddleBrown = 1262987,
    Color_Salmon = 7504122,
    Color_SandyBrown = 6333684,
    Color_SeaGreen = 5737262,
    Color_SeaShell = 15660543,
    Color_Sienna = 2970272,
    Color_Silver = 12632256,
    Color_SkyBlue = 15453831,
    Color_SlateBlue = 13458026,
    Color_SlateGray = 9470064,
    Color_Snow = 16448255,
    Color_SpringGreen = 8388352,
    Color_SteelBlue = 11829830,
    Color_Tan = 9221330,
    Color_Teal = 8421376,
    Color_Thistle = 14204888,
    Color_Tomato = 4678655,
    Color_Turquoise = 13688896,
    Color_Violet = 15631086,
    Color_Wheat = 11788021,
    Color_White = 16777215,
    Color_WhiteSmoke = 16119285,
    Color_Yellow = 65535,
    Color_YellowGreen = 3329434,
} Color;

typedef enum __nfpack Stretch
{
    Stretch_None = 0,
    Stretch_Fill = 1,
} Stretch;

typedef enum __nfpack TextAlignment
{
    TextAlignment_Left = 0,
    TextAlignment_Center = 1,
    TextAlignment_Right = 2,
} TextAlignment;

typedef enum __nfpack TextTrimming
{
    TextTrimming_CharacterEllipsis = 0,
    TextTrimming_None = 1,
    TextTrimming_WordEllipsis = 2,
} TextTrimming;

typedef enum __nfpack Direction
{
    Direction_TopToBottom = 0,
    Direction_BottomToTop = 1,
} Direction;

typedef enum __nfpack SizeToContent
{
    SizeToContent_Manual = 0,
    SizeToContent_Width = 1,
    SizeToContent_Height = 2,
    SizeToContent_WidthAndHeight = 3,
} SizeToContent;

typedef enum __nfpack UIElement_Flags
{
    UIElement_Flags_None = 0,
    UIElement_Flags_IsSubtreeDirtyForRender = 2,
    UIElement_Flags_IsDirtyForRender = 4,
    UIElement_Flags_Enabled = 32,
    UIElement_Flags_InvalidMeasure = 64,
    UIElement_Flags_InvalidArrange = 128,
    UIElement_Flags_MeasureInProgress = 256,
    UIElement_Flags_ArrangeInProgress = 512,
    UIElement_Flags_MeasureDuringArrange = 1024,
    UIElement_Flags_NeverMeasured = 2048,
    UIElement_Flags_NeverArranged = 4096,
    UIElement_Flags_ShouldPostRender = 8192,
    UIElement_Flags_IsLayoutSuspended = 16384,
    UIElement_Flags_IsVisibleCache = 32768,
} UIElement_Flags;

typedef enum __nfpack VerticalAlignment
{
    VerticalAlignment_Top = 0,
    VerticalAlignment_Center = 1,
    VerticalAlignment_Bottom = 2,
    VerticalAlignment_Stretch = 3,
} VerticalAlignment;

typedef enum __nfpack Visibility
{
    Visibility_Visible = 0,
    Visibility_Hidden = 1,
    Visibility_Collapsed = 2,
} Visibility;

typedef enum __nfpack Bitmap_BitmapImageType
{
    Bitmap_BitmapImageType_NanoCLRBitmap = 0,
    Bitmap_BitmapImageType_Gif = 1,
    Bitmap_BitmapImageType_Jpeg = 2,
    Bitmap_BitmapImageType_Bmp = 3,
} Bitmap_BitmapImageType;

typedef enum __nfpack CollectionMethod
{
    CollectionMethod_Managed = 0,
    CollectionMethod_Native = 1,
} CollectionMethod;

typedef enum __nfpack CollectionMode
{
    CollectionMode_InkOnly = 2,
    CollectionMode_GestureOnly = 4,
    CollectionMode_InkAndGesture = 6,
} CollectionMode;

// Moved to Display.h for convenience
/*typedef enum __nfpack DisplayOrientation
{
    DisplayOrientation_Portrait = 0,
    DisplayOrientation_Portrait180 = 1,
    DisplayOrientation_Landscape = 2,
    DisplayOrientation_Landscape180 = 3,
} DisplayOrientation;

typedef enum __nfpack GraphicDriverCommandType
{
    GraphicDriverCommandType_Sleep = 0,
    GraphicDriverCommandType_Command = 1,
} GraphicDriverCommandType;
*/
typedef enum __nfpack Button
{
    Button_None = 0,
    Button_VK_LBUTTON = 1,
    Button_VK_RBUTTON = 2,
    Button_VK_CANCEL = 3,
    Button_VK_MBUTTON = 4,
    Button_VK_BACK = 8,
    Button_VK_TAB = 9,
    Button_VK_CLEAR = 12,
    Button_VK_RETURN = 13,
    Button_VK_SHIFT = 16,
    Button_VK_CONTROL = 17,
    Button_VK_MENU = 18,
    Button_VK_PAUSE = 19,
    Button_VK_CAPITAL = 20,
    Button_VK_KANA = 21,
    Button_VK_HANGEUL = 21,
    Button_VK_HANGUL = 21,
    Button_VK_JUNJA = 23,
    Button_VK_FINAL = 24,
    Button_VK_HANJA = 25,
    Button_VK_KANJI = 25,
    Button_VK_ESCAPE = 27,
    Button_VK_CONVERT = 28,
    Button_VK_NOCONVERT = 29,
    Button_VK_SPACE = 32,
    Button_VK_PRIOR = 33,
    Button_VK_NEXT = 34,
    Button_VK_END = 35,
    Button_VK_HOME = 36,
    Button_VK_LEFT = 37,
    Button_VK_UP = 38,
    Button_VK_RIGHT = 39,
    Button_VK_DOWN = 40,
    Button_VK_SELECT = 41,
    Button_VK_PRINT = 42,
    Button_VK_EXECUTE = 43,
    Button_VK_SNAPSHOT = 44,
    Button_VK_INSERT = 45,
    Button_VK_DELETE = 46,
    Button_VK_HELP = 47,
    Button_VK_0 = 48,
    Button_VK_1 = 49,
    Button_VK_2 = 50,
    Button_VK_3 = 51,
    Button_VK_4 = 52,
    Button_VK_5 = 53,
    Button_VK_6 = 54,
    Button_VK_7 = 55,
    Button_VK_8 = 56,
    Button_VK_9 = 57,
    Button_VK_A = 65,
    Button_VK_B = 66,
    Button_VK_C = 67,
    Button_VK_D = 68,
    Button_VK_E = 69,
    Button_VK_F = 70,
    Button_VK_G = 71,
    Button_VK_H = 72,
    Button_VK_I = 73,
    Button_VK_J = 74,
    Button_VK_K = 75,
    Button_VK_L = 76,
    Button_VK_M = 77,
    Button_VK_N = 78,
    Button_VK_O = 79,
    Button_VK_P = 80,
    Button_VK_Q = 81,
    Button_VK_R = 82,
    Button_VK_S = 83,
    Button_VK_T = 84,
    Button_VK_U = 85,
    Button_VK_V = 86,
    Button_VK_W = 87,
    Button_VK_X = 88,
    Button_VK_Y = 89,
    Button_VK_Z = 90,
    Button_VK_LWIN = 91,
    Button_VK_RWIN = 92,
    Button_VK_APPS = 93,
    Button_VK_SLEEP = 95,
    Button_VK_NUMPAD0 = 96,
    Button_VK_NUMPAD1 = 97,
    Button_VK_NUMPAD2 = 98,
    Button_VK_NUMPAD3 = 99,
    Button_VK_NUMPAD4 = 100,
    Button_VK_NUMPAD5 = 101,
    Button_VK_NUMPAD6 = 102,
    Button_VK_NUMPAD7 = 103,
    Button_VK_NUMPAD8 = 104,
    Button_VK_NUMPAD9 = 105,
    Button_VK_MULTIPLY = 106,
    Button_VK_ADD = 107,
    Button_VK_SEPARATOR = 108,
    Button_VK_SUBTRACT = 109,
    Button_VK_DECIMAL = 110,
    Button_VK_DIVIDE = 111,
    Button_VK_F1 = 112,
    Button_VK_F2 = 113,
    Button_VK_F3 = 114,
    Button_VK_F4 = 115,
    Button_VK_F5 = 116,
    Button_VK_F6 = 117,
    Button_VK_F7 = 118,
    Button_VK_F8 = 119,
    Button_VK_F9 = 120,
    Button_VK_F10 = 121,
    Button_VK_F11 = 122,
    Button_VK_F12 = 123,
    Button_VK_F13 = 124,
    Button_VK_F14 = 125,
    Button_VK_F15 = 126,
    Button_VK_F16 = 127,
    Button_VK_F17 = 128,
    Button_VK_F18 = 129,
    Button_VK_F19 = 130,
    Button_VK_F20 = 131,
    Button_VK_F21 = 132,
    Button_VK_F22 = 133,
    Button_VK_F23 = 134,
    Button_VK_F24 = 135,
    Button_VK_NUMLOCK = 144,
    Button_VK_SCROLL = 145,
    Button_VK_LSHIFT = 160,
    Button_VK_RSHIFT = 161,
    Button_VK_LCONTROL = 162,
    Button_VK_RCONTROL = 163,
    Button_VK_LMENU = 164,
    Button_VK_RMENU = 165,
    Button_VK_EXTEND_BSLASH = 226,
    Button_VK_OEM_102 = 226,
    Button_VK_PROCESSKEY = 229,
    Button_VK_ATTN = 246,
    Button_VK_CRSEL = 247,
    Button_VK_EXSEL = 248,
    Button_VK_EREOF = 249,
    Button_VK_PLAY = 250,
    Button_VK_ZOOM = 251,
    Button_VK_NONAME = 252,
    Button_VK_PA1 = 253,
    Button_VK_OEM_CLEAR = 254,
    Button_VK_SEMICOLON = 186,
    Button_VK_EQUAL = 187,
    Button_VK_COMMA = 188,
    Button_VK_HYPHEN = 189,
    Button_VK_PERIOD = 190,
    Button_VK_SLASH = 191,
    Button_VK_BACKQUOTE = 192,
    Button_VK_BROWSER_BACK = 166,
    Button_VK_BROWSER_FORWARD = 167,
    Button_VK_BROWSER_REFRESH = 168,
    Button_VK_BROWSER_STOP = 169,
    Button_VK_BROWSER_SEARCH = 170,
    Button_VK_BROWSER_FAVORITES = 171,
    Button_VK_BROWSER_HOME = 172,
    Button_VK_VOLUME_MUTE = 173,
    Button_VK_VOLUME_DOWN = 174,
    Button_VK_VOLUME_UP = 175,
    Button_VK_MEDIA_NEXT_TRACK = 176,
    Button_VK_MEDIA_PREV_TRACK = 177,
    Button_VK_MEDIA_STOP = 178,
    Button_VK_MEDIA_PLAY_PAUSE = 179,
    Button_VK_LAUNCH_MAIL = 180,
    Button_VK_LAUNCH_MEDIA_SELECT = 181,
    Button_VK_LAUNCH_APP1 = 182,
    Button_VK_LAUNCH_APP2 = 183,
    Button_VK_LBRACKET = 219,
    Button_VK_BACKSLASH = 220,
    Button_VK_RBRACKET = 221,
    Button_VK_APOSTROPHE = 222,
    Button_VK_OFF = 223,
    Button_VK_DBE_ALPHANUMERIC = 240,
    Button_VK_DBE_KATAKANA = 241,
    Button_VK_DBE_HIRAGANA = 242,
    Button_VK_DBE_SBCSCHAR = 243,
    Button_VK_DBE_DBCSCHAR = 244,
    Button_VK_DBE_ROMAN = 245,
    Button_VK_DBE_NOROMAN = 246,
    Button_VK_DBE_ENTERWORDREGISTERMODE = 247,
    Button_VK_DBE_ENTERIMECONFIGMODE = 248,
    Button_VK_DBE_FLUSHSTRING = 249,
    Button_VK_DBE_CODEINPUT = 250,
    Button_VK_DBE_NOCODEINPUT = 251,
    Button_VK_DBE_DETERMINESTRING = 252,
    Button_VK_DBE_ENTERDLGCONVERSIONMODE = 253,
    Button_LastSystemDefinedButton = 272,
    Button_AppDefined1 = 273,
    Button_AppDefined2 = 274,
    Button_AppDefined3 = 275,
    Button_AppDefined4 = 276,
    Button_AppDefined5 = 277,
    Button_AppDefined6 = 278,
    Button_AppDefined7 = 279,
    Button_AppDefined8 = 280,
} Button;

typedef enum __nfpack ButtonState
{
    ButtonState_None = 0,
    ButtonState_Down = 1,
    ButtonState_Held = 2,
} ButtonState;

typedef enum __nfpack CaptureMode
{
    CaptureMode_None = 0,
    CaptureMode_Element = 1,
    CaptureMode_SubTree = 2,
} CaptureMode;

typedef enum __nfpack InputManager_InputDeviceType
{
    InputManager_InputDeviceType_Button = 0,
    InputManager_InputDeviceType_Touch = 1,
    InputManager_InputDeviceType_Generic = 2,
    InputManager_InputDeviceType_Last = 3,
} InputManager_InputDeviceType;

typedef enum __nfpack RawButtonActions
{
    RawButtonActions_ButtonDown = 1,
    RawButtonActions_ButtonUp = 2,
    RawButtonActions_Activate = 4,
    RawButtonActions_Deactivate = 8,
} RawButtonActions;

typedef enum __nfpack RawTouchActions
{
    RawTouchActions_TouchDown = 1,
    RawTouchActions_TouchUp = 2,
    RawTouchActions_Activate = 4,
    RawTouchActions_Deactivate = 8,
    RawTouchActions_TouchMove = 16,
} RawTouchActions;

typedef enum __nfpack ReasonSessionEnding
{
    ReasonSessionEnding_Logoff = 0,
    ReasonSessionEnding_Shutdown = 1,
} ReasonSessionEnding;

typedef enum __nfpack RoutedEventArgs_Flags
{
    RoutedEventArgs_Flags_Handled = 1,
    RoutedEventArgs_Flags_InvokingHandler = 2,
} RoutedEventArgs_Flags;

typedef enum __nfpack RoutingStrategy
{
    RoutingStrategy_Tunnel = 0,
    RoutingStrategy_Bubble = 1,
    RoutingStrategy_Direct = 2,
} RoutingStrategy;

typedef enum __nfpack ShutdownMode
{
    ShutdownMode_OnLastWindowClose = 0,
    ShutdownMode_OnMainWindowClose = 1,
    ShutdownMode_OnExplicitShutdown = 2,
} ShutdownMode;

typedef enum __nfpack Temporary_EventCategoryEx
{
    Temporary_EventCategoryEx_Touch = 80,
    Temporary_EventCategoryEx_Gesture = 90,
} Temporary_EventCategoryEx;

typedef enum __nfpack DispatcherOperationStatus
{
    DispatcherOperationStatus_Pending = 0,
    DispatcherOperationStatus_Aborted = 1,
    DispatcherOperationStatus_Completed = 2,
    DispatcherOperationStatus_Executing = 3,
} DispatcherOperationStatus;

typedef enum __nfpack TouchCollectorConfiguration_TouchInput
{
    TouchCollectorConfiguration_TouchInput_LastTouchPoint = 2,
    TouchCollectorConfiguration_TouchInput_SamplingDistance = 4,
    TouchCollectorConfiguration_TouchInput_TouchMoveFrequency = 8,
} TouchCollectorConfiguration_TouchInput;

typedef enum __nfpack TouchGesture
{
    TouchGesture_NoGesture = 0,
    TouchGesture_Begin = 1,
    TouchGesture_End = 2,
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
    TouchGesture_Zoom = 114,
    TouchGesture_Pan = 115,
    TouchGesture_Rotate = 116,
    TouchGesture_TwoFingerTap = 117,
    TouchGesture_Rollover = 118,
    TouchGesture_UserDefined = 200,
} TouchGesture;

typedef enum __nfpack TouchInputFlags
{
    TouchInputFlags_None = 0,
    TouchInputFlags_Primary = 16,
    TouchInputFlags_Pen = 64,
    TouchInputFlags_Palm = 128,
} TouchInputFlags;

typedef enum __nfpack TouchMessages
{
    TouchMessages_Down = 1,
    TouchMessages_Up = 2,
    TouchMessages_Move = 3,
} TouchMessages;

struct Library_nanoFramework_Graphics_nanoFramework_UI_RoutedEvent
{
    static const int FIELD_STATIC___eventCount = 0;

    static const int FIELD___name = 1;
    static const int FIELD___routingStrategy = 2;
    static const int FIELD___handlerType = 3;
    static const int FIELD___globalIndex = 4;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_RoutedEventArgs
{
    static const int FIELD___routedEvent = 1;
    static const int FIELD___source = 2;
    static const int FIELD___originalSource = 3;
    static const int FIELD___flags = 4;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_Input_ButtonEventArgs
{
    static const int FIELD__Button = 7;
    static const int FIELD__InputSource = 8;
    static const int FIELD___isRepeat = 9;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_Input_FocusChangedEventArgs
{
    static const int FIELD__OldFocus = 7;
    static const int FIELD__NewFocus = 8;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_Input_GenericEventArgs
{
    static const int FIELD__InternalEvent = 7;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_TouchGestureEventArgs
{
    static const int FIELD__Timestamp = 1;
    static const int FIELD__Gesture = 2;
    static const int FIELD__X = 3;
    static const int FIELD__Y = 4;
    static const int FIELD__Arguments = 5;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_Input_TouchEventArgs
{
    static const int FIELD__Touches = 7;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_Threading_Dispatcher
{
    static const int FIELD_STATIC___dispatchers = 1;
    static const int FIELD_STATIC___possibleDispatcher = 2;

    static const int FIELD__ShutdownStarted = 1;
    static const int FIELD__ShutdownFinished = 2;
    static const int FIELD___currentFrame = 3;
    static const int FIELD___frameDepth = 4;
    static const int FIELD___hasShutdownStarted = 5;
    static const int FIELD___hasShutdownFinished = 6;
    static const int FIELD___queue = 7;
    static const int FIELD___event = 8;
    static const int FIELD___instanceLock = 9;
    static const int FIELD___thread = 10;
    static const int FIELD___finalExceptionHandler = 11;
    static const int FIELD___layoutManager = 12;
    static const int FIELD___inputManager = 13;
    static const int FIELD___mediaContext = 14;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_Threading_DispatcherFrame
{
    static const int FIELD___exitWhenRequested = 1;
    static const int FIELD___continue = 2;
    static const int FIELD___dispatcher = 3;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_Threading_DispatcherOperation
{
    static const int FIELD__Aborted = 1;
    static const int FIELD__Completed = 2;
    static const int FIELD___dispatcher = 3;
    static const int FIELD___method = 4;
    static const int FIELD___args = 5;
    static const int FIELD___result = 6;
    static const int FIELD___status = 7;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_Font
{
    static const int FIELD__m_font = 1;

    NANOCLR_NATIVE_DECLARE(CharWidth___I4__CHAR);
    NANOCLR_NATIVE_DECLARE(get_Height___I4);
    NANOCLR_NATIVE_DECLARE(get_AverageWidth___I4);
    NANOCLR_NATIVE_DECLARE(get_MaxWidth___I4);
    NANOCLR_NATIVE_DECLARE(get_Ascent___I4);
    NANOCLR_NATIVE_DECLARE(get_Descent___I4);
    NANOCLR_NATIVE_DECLARE(get_InternalLeading___I4);
    NANOCLR_NATIVE_DECLARE(get_ExternalLeading___I4);
    NANOCLR_NATIVE_DECLARE(ComputeExtent___VOID__STRING__BYREF_I4__BYREF_I4__I4);
    NANOCLR_NATIVE_DECLARE(ComputeTextInRect___VOID__STRING__BYREF_I4__BYREF_I4__I4__I4__I4__I4__U4);

    //--//
    // The Following was not generated //
    // Update me every time you copy/paste this file //
    static HRESULT GetFont(CLR_RT_HeapBlock *pThis, CLR_GFX_Font *&font);
    static HRESULT GetFont(CLR_RT_StackFrame &stack, CLR_GFX_Font *&font);
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_Bitmap
{
    static const int FIELD_STATIC__MaxWidth = 3;
    static const int FIELD_STATIC__MaxHeight = 4;
    static const int FIELD_STATIC__CenterX = 5;
    static const int FIELD_STATIC__CenterY = 6;

    static const int FIELD__m_bitmap = 1;

    NANOCLR_NATIVE_DECLARE(_ctor___VOID__I4__I4);
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__SZARRAY_U1__nanoFrameworkUIBitmapBitmapImageType);
    NANOCLR_NATIVE_DECLARE(Flush___VOID);
    NANOCLR_NATIVE_DECLARE(Flush___VOID__I4__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(Flush___VOID__I4__I4__I4__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(Clear___VOID);
    NANOCLR_NATIVE_DECLARE(
        DrawTextInRect___BOOLEAN__BYREF_STRING__BYREF_I4__BYREF_I4__I4__I4__I4__I4__U4__nanoFrameworkPresentationMediaColor__nanoFrameworkUIFont);
    NANOCLR_NATIVE_DECLARE(DrawChar___VOID__U2__I4__I4__nanoFrameworkPresentationMediaColor__nanoFrameworkUIFont);
    NANOCLR_NATIVE_DECLARE(SetClippingRectangle___VOID__I4__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(get_Width___I4);
    NANOCLR_NATIVE_DECLARE(get_Height___I4);
    NANOCLR_NATIVE_DECLARE(
        DrawEllipse___VOID__nanoFrameworkPresentationMediaColor__I4__I4__I4__I4__I4__nanoFrameworkPresentationMediaColor__I4__I4__nanoFrameworkPresentationMediaColor__I4__I4__U2);
    NANOCLR_NATIVE_DECLARE(DrawImage___VOID__I4__I4__nanoFrameworkUIBitmap__I4__I4__I4__I4__U2);
    NANOCLR_NATIVE_DECLARE(RotateImage___VOID__I4__I4__I4__nanoFrameworkUIBitmap__I4__I4__I4__I4__U2);
    NANOCLR_NATIVE_DECLARE(MakeTransparent___VOID__nanoFrameworkPresentationMediaColor);
    NANOCLR_NATIVE_DECLARE(StretchImage___VOID__I4__I4__nanoFrameworkUIBitmap__I4__I4__U2);
    NANOCLR_NATIVE_DECLARE(DrawLine___VOID__nanoFrameworkPresentationMediaColor__I4__I4__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(DrawRectangle___VOID__I4__I4__I4__I4__I4__nanoFrameworkPresentationMediaColor);
    NANOCLR_NATIVE_DECLARE(
        DrawRectangle___VOID__nanoFrameworkPresentationMediaColor__I4__I4__I4__I4__I4__I4__I4__nanoFrameworkPresentationMediaColor__I4__I4__nanoFrameworkPresentationMediaColor__I4__I4__U2);
    NANOCLR_NATIVE_DECLARE(DrawRoundRectangle___VOID__I4__I4__I4__I4__I4__I4__I4__nanoFrameworkPresentationMediaColor);
    NANOCLR_NATIVE_DECLARE(FillRectangle___VOID__I4__I4__I4__I4__nanoFrameworkPresentationMediaColor__U2);
    NANOCLR_NATIVE_DECLARE(FillRoundRectangle___VOID__I4__I4__I4__I4__I4__I4__nanoFrameworkPresentationMediaColor__U2);
    NANOCLR_NATIVE_DECLARE(
        FillGradientRectangle___VOID__I4__I4__I4__I4__nanoFrameworkPresentationMediaColor__I4__I4__nanoFrameworkPresentationMediaColor__I4__I4__U2);
    NANOCLR_NATIVE_DECLARE(DrawText___VOID__STRING__nanoFrameworkUIFont__nanoFrameworkPresentationMediaColor__I4__I4);
    NANOCLR_NATIVE_DECLARE(SetPixel___VOID__I4__I4__nanoFrameworkPresentationMediaColor);
    NANOCLR_NATIVE_DECLARE(GetPixel___nanoFrameworkPresentationMediaColor__I4__I4);
    NANOCLR_NATIVE_DECLARE(GetBitmap___SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(StretchImage___VOID__I4__I4__I4__I4__nanoFrameworkUIBitmap__I4__I4__I4__I4__U2);
    NANOCLR_NATIVE_DECLARE(TileImage___VOID__I4__I4__nanoFrameworkUIBitmap__I4__I4__U2);
    NANOCLR_NATIVE_DECLARE(Scale9Image___VOID__I4__I4__I4__I4__nanoFrameworkUIBitmap__I4__I4__I4__I4__U2);
    NANOCLR_NATIVE_DECLARE(Dispose___VOID__BOOLEAN);

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_Media_Pen
{
    static const int FIELD__Color = 1;
    static const int FIELD__Thickness = 2;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_Media_DrawingContext
{
    static const int FIELD__EmptyClipRect = 2;
    static const int FIELD___bitmap = 3;
    static const int FIELD___x = 4;
    static const int FIELD___y = 5;
    static const int FIELD___clippingRectangles = 6;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_Media_MediaContext
{
    static const int FIELD___screenW = 2;
    static const int FIELD___screenH = 3;
    static const int FIELD___dirtyX0 = 4;
    static const int FIELD___dirtyY0 = 5;
    static const int FIELD___dirtyX1 = 6;
    static const int FIELD___dirtyY1 = 7;
    static const int FIELD___currentRenderOp = 8;
    static const int FIELD___renderMessage = 9;
    static const int FIELD___isRendering = 10;
    static const int FIELD___invokeOnRenderCallbacks = 11;
    static const int FIELD___target = 12;
    static const int FIELD___screen = 13;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_LayoutManager__LayoutQueue
{
    static const int FIELD___layoutManager = 1;
    static const int FIELD___elements = 2;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_LayoutManager
{
    static const int FIELD__ts = 2;
    static const int FIELD___isUpdating = 3;
    static const int FIELD___gotException = 4;
    static const int FIELD___layoutRequestPosted = 5;
    static const int FIELD___forceLayoutElement = 6;
    static const int FIELD___arrangeQueue = 7;
    static const int FIELD___measureQueue = 8;
    static const int FIELD___updateLayoutBackground = 9;
    static const int FIELD___updateCallback = 10;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_PropertyChangedEventArgs
{
    static const int FIELD__Property = 1;
    static const int FIELD__OldValue = 2;
    static const int FIELD__NewValue = 3;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_RoutedEventHandlerInfo
{
    static const int FIELD___handler = 1;
    static const int FIELD___handledEventsToo = 2;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_RouteItem
{
    static const int FIELD___target = 1;
    static const int FIELD___routedEventHandlerInfo = 2;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_EventRoute
{
    static const int FIELD__RoutedEvent = 1;
    static const int FIELD___routeItemList = 2;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_UIElement
{
    static const int FIELD_STATIC___classEventHandlersStore = 7;

    static const int FIELD__TouchDown = 2;
    static const int FIELD__TouchUp = 3;
    static const int FIELD__TouchMove = 4;
    static const int FIELD__TouchGestureStart = 5;
    static const int FIELD__TouchGestureChanged = 6;
    static const int FIELD__TouchGestureEnd = 7;
    static const int FIELD___parent = 8;
    static const int FIELD___logicalChildren = 9;
    static const int FIELD___flags = 10;
    static const int FIELD___visibility = 11;
    static const int FIELD___requestedSize = 12;
    static const int FIELD___anchorInfo = 13;
    static const int FIELD___marginLeft = 14;
    static const int FIELD___marginTop = 15;
    static const int FIELD___marginRight = 16;
    static const int FIELD___marginBottom = 17;
    static const int FIELD___horizontalAlignment = 18;
    static const int FIELD___verticalAlignment = 19;
    static const int FIELD___finalX = 20;
    static const int FIELD___finalY = 21;
    static const int FIELD___finalWidth = 22;
    static const int FIELD___finalHeight = 23;
    static const int FIELD___offsetX = 24;
    static const int FIELD___offsetY = 25;
    static const int FIELD___renderWidth = 26;
    static const int FIELD___renderHeight = 27;
    static const int FIELD___previousAvailableWidth = 28;
    static const int FIELD___previousAvailableHeight = 29;
    static const int FIELD___desiredWidth = 30;
    static const int FIELD___desiredHeight = 31;
    static const int FIELD___unclippedWidth = 32;
    static const int FIELD___unclippedHeight = 33;
    static const int FIELD___instanceEventHandlersStore = 34;
    static const int FIELD___isEnabledChanged = 35;
    static const int FIELD___isVisibleChanged = 36;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_Media_Brush
{
    static const int FIELD___opacity = 1;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_Controls_Border
{
    static const int FIELD___borderBrush = 40;
    static const int FIELD___borderLeft = 41;
    static const int FIELD___borderTop = 42;
    static const int FIELD___borderRight = 43;
    static const int FIELD___borderBottom = 44;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_UIElementCollection
{
    static const int FIELD___items = 1;
    static const int FIELD___size = 2;
    static const int FIELD___version = 3;
    static const int FIELD___owner = 4;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_Controls_Control
{
    static const int FIELD___background = 37;
    static const int FIELD___foreground = 38;
    static const int FIELD___font = 39;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_Controls_DockPanel
{
    static const int FIELD_STATIC__DockPropertiesKeys = 8;
    static const int FIELD_STATIC__DockPropertiesValues = 9;

    static const int FIELD___lastChildFill = 37;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_Controls_DrawingAttributes
{
    static const int FIELD__Color = 1;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_Controls_Image
{
    static const int FIELD___bitmap = 37;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_Controls_InkCanvas
{
    static const int FIELD___defaultDrawingAttributes = 37;
    static const int FIELD___bitmap = 38;
    static const int FIELD___borderWidth = 39;
    static const int FIELD___width = 40;
    static const int FIELD___height = 41;
    static const int FIELD___top = 42;
    static const int FIELD___left = 43;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_Controls_ListBoxItem
{
    static const int FIELD___isSelectable = 40;
    static const int FIELD___listBox = 41;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_Controls_SelectionChangedEventArgs
{
    static const int FIELD__PreviousSelectedIndex = 1;
    static const int FIELD__SelectedIndex = 2;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_Controls_ListBoxItemCollection
{
    static const int FIELD___items = 1;
    static const int FIELD___listBox = 2;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_Controls_ListBox
{
    static const int FIELD___scrollViewer = 40;
    static const int FIELD___panel = 41;
    static const int FIELD___selectedIndex = 42;
    static const int FIELD___selectionChanged = 43;
    static const int FIELD___items = 44;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_Controls_ScrollChangedEventArgs
{
    static const int FIELD__HorizontalChange = 1;
    static const int FIELD__HorizontalOffset = 2;
    static const int FIELD__VerticalChange = 3;
    static const int FIELD__VerticalOffset = 4;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_Controls_ScrollViewer
{
    static const int FIELD___previousHorizontalOffset = 40;
    static const int FIELD___previousVerticalOffset = 41;
    static const int FIELD___horizontalOffset = 42;
    static const int FIELD___verticalOffset = 43;
    static const int FIELD___extentWidth = 44;
    static const int FIELD___extentHeight = 45;
    static const int FIELD___scrollableWidth = 46;
    static const int FIELD___scrollableHeight = 47;
    static const int FIELD___lineHeight = 48;
    static const int FIELD___lineWidth = 49;
    static const int FIELD___scrollingStyle = 50;
    static const int FIELD___scrollChanged = 51;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_Controls_StackPanel
{
    static const int FIELD___orientation = 37;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_Controls_Text
{
    static const int FIELD___font = 37;
    static const int FIELD___foreColor = 38;
    static const int FIELD___text = 39;
    static const int FIELD___textWrap = 40;
    static const int FIELD___trimming = 41;
    static const int FIELD___alignment = 42;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_Controls_TextRun
{
    static const int FIELD__Text = 1;
    static const int FIELD__Font = 2;
    static const int FIELD__ForeColor = 3;
    static const int FIELD__IsEndOfLine = 4;
    static const int FIELD___width = 5;
    static const int FIELD___height = 6;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_Controls_TextFlow
{
    static const int FIELD__TextRuns = 37;
    static const int FIELD___lineCache = 38;
    static const int FIELD___alignment = 39;
    static const int FIELD___currentLine = 40;
    static const int FIELD___scrollingStyle = 41;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_Controls_TextFlow__TextLine
{
    static const int FIELD__Runs = 1;
    static const int FIELD__Baseline = 2;
    static const int FIELD__Height = 3;
    static const int FIELD___width = 4;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_Controls_TextRunCollection
{
    static const int FIELD___textFlow = 1;
    static const int FIELD___textRuns = 2;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_Controls_WrapPanel
{
    static const int FIELD___itemWidth = 37;
    static const int FIELD___itemHeight = 38;
    static const int FIELD___orientation = 39;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_Controls_WrapPanel__UVSize
{
    static const int FIELD__U = 1;
    static const int FIELD__V = 2;
    static const int FIELD___orientation = 3;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_Media_DrawingContext__ClipRectangle
{
    static const int FIELD__X = 1;
    static const int FIELD__Y = 2;
    static const int FIELD__Width = 3;
    static const int FIELD__Height = 4;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_Media_ImageBrush
{
    static const int FIELD__BitmapSource = 2;
    static const int FIELD__Stretch = 3;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_Media_LinearGradientBrush
{
    static const int FIELD__StartColor = 2;
    static const int FIELD__EndColor = 3;
    static const int FIELD__MappingMode = 4;
    static const int FIELD__StartX = 5;
    static const int FIELD__StartY = 6;
    static const int FIELD__EndX = 7;
    static const int FIELD__EndY = 8;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_Media_MediaContext__InvokeOnRenderCallback
{
    static const int FIELD___callback = 1;
    static const int FIELD___arg = 2;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_Media_SolidColorBrush
{
    static const int FIELD__Color = 2;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_Media_SolidColorBrush__LineSegment
{
    static const int FIELD__x1 = 1;
    static const int FIELD__y1 = 2;
    static const int FIELD__x2 = 3;
    static const int FIELD__y2 = 4;
    static const int FIELD__dx = 5;
    static const int FIELD__dy = 6;
    static const int FIELD__cx = 7;
    static const int FIELD__e = 8;
    static const int FIELD__highSlope = 9;
    static const int FIELD__ix = 10;
    static const int FIELD__processedPts = 11;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_PresentationSource
{
    static const int FIELD___rootUIElement = 2;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_Shapes_Line
{
    static const int FIELD___direction = 39;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_Shapes_Polygon
{
    static const int FIELD___pts = 39;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_Shapes_Shape
{
    static const int FIELD___fill = 37;
    static const int FIELD___stroke = 38;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_UIElement__Pair
{
    static const int FIELD___first = 1;
    static const int FIELD___second = 2;
    static const int FIELD___status = 3;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_UIElementCollection__Enumerator
{
    static const int FIELD___collection = 1;
    static const int FIELD___index = 2;
    static const int FIELD___version = 3;
    static const int FIELD___currentElement = 4;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_WindowManager
{
    static const int FIELD_STATIC__Instance = 10;

    static const int FIELD___postRenderHandler = 37;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_WindowCollection
{
    static const int FIELD___list = 1;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_Input_InputEventArgs
{
    static const int FIELD__Timestamp = 5;
    static const int FIELD___inputDevice = 6;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_Input_StagingAreaInputItem
{
    static const int FIELD__Input = 1;
    static const int FIELD___table = 2;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_Input_PreProcessInputEventArgs
{
    static const int FIELD___canceled = 2;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_Input_NotifyInputEventArgs
{
    static const int FIELD__StagingItem = 1;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_Input_InputReport
{
    static const int FIELD__InputSource = 1;
    static const int FIELD__Timestamp = 2;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_Input_InputManager
{
    static const int FIELD_STATIC__PreviewInputReportEvent = 11;
    static const int FIELD_STATIC__InputReportEvent = 12;

    static const int FIELD___continueProcessingStagingAreaCallback = 2;
    static const int FIELD___frameStagingArea = 3;
    static const int FIELD__InputDeviceEvents = 4;
    static const int FIELD___inputProviders = 5;
    static const int FIELD___inputProviderSites = 6;
    static const int FIELD___currentStagingStack = 7;
    static const int FIELD___stagingArea = 8;
    static const int FIELD___mostRecentInputDevice = 9;
    static const int FIELD___buttonDevice = 10;
    static const int FIELD___touchDevice = 11;
    static const int FIELD___genericDevice = 12;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_Input_InputProviderSite
{
    static const int FIELD___isDisposed = 1;
    static const int FIELD___inputManager = 2;
    static const int FIELD___inputProvider = 3;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_TouchInput
{
    static const int FIELD__X = 1;
    static const int FIELD__Y = 2;
    static const int FIELD__SourceID = 3;
    static const int FIELD__Flags = 4;
    static const int FIELD__ContactWidth = 5;
    static const int FIELD__ContactHeight = 6;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_Input_TouchDevice
{
    static const int FIELD___inputManager = 2;
    static const int FIELD___focus = 3;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Runtime_Events_GenericEventEx
{
    static const int FIELD__X = 6;
    static const int FIELD__Y = 7;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_Input_GenericDevice
{
    static const int FIELD___focus = 2;
    static const int FIELD___inputManager = 3;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_Application
{
    static const int FIELD_STATIC___isShuttingDown = 13;
    static const int FIELD_STATIC___appCreatedInThisAppDomain = 14;
    static const int FIELD_STATIC___appInstance = 15;
    static const int FIELD_STATIC___reportInputMethod = 16;
    static const int FIELD_STATIC___inputManager = 17;
    static const int FIELD_STATIC___stylusMaxX = 18;
    static const int FIELD_STATIC___stylusMaxY = 19;

    static const int FIELD___appWindowList = 2;
    static const int FIELD___nonAppWindowList = 3;
    static const int FIELD___mainWindow = 4;
    static const int FIELD___ownDispatcherStarted = 5;
    static const int FIELD___appIsShutdown = 6;
    static const int FIELD___shutdownMode = 7;
    static const int FIELD___startupEventHandler = 8;
    static const int FIELD___exitEventHandler = 9;
    static const int FIELD___inputProviderSite = 10;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_Window
{
    static const int FIELD___sizeToContent = 40;
    static const int FIELD___windowManager = 41;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_CancelEventArgs
{
    static const int FIELD__Cancel = 1;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_SpiConfiguration
{
    static const int FIELD___spiBus = 1;
    static const int FIELD___chipSelect = 2;
    static const int FIELD___dataCommand = 3;
    static const int FIELD___reset = 4;
    static const int FIELD___backLight = 5;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_ScreenConfiguration
{
    static const int FIELD___x = 1;
    static const int FIELD___y = 2;
    static const int FIELD___width = 3;
    static const int FIELD___height = 4;
    static const int FIELD___graphicDriver = 5;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_I2cConfiguration
{
    static const int FIELD___i2cBus = 1;
    static const int FIELD___address = 2;
    static const int FIELD___fastMode = 3;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_DisplayControl
{
    static const int FIELD_STATIC___maximumBufferSize = 20;
    static const int FIELD_STATIC___fullScreen = 21;
    static const int FIELD_STATIC___point = 22;

    NANOCLR_NATIVE_DECLARE(get_LongerSide___STATIC__I4);
    NANOCLR_NATIVE_DECLARE(get_ShorterSide___STATIC__I4);
    NANOCLR_NATIVE_DECLARE(get_ScreenWidth___STATIC__I4);
    NANOCLR_NATIVE_DECLARE(get_ScreenHeight___STATIC__I4);
    NANOCLR_NATIVE_DECLARE(get_BitsPerPixel___STATIC__I4);
    NANOCLR_NATIVE_DECLARE(get_Orientation___STATIC__nanoFrameworkUIDisplayOrientation);
    NANOCLR_NATIVE_DECLARE(Clear___STATIC__VOID);
    NANOCLR_NATIVE_DECLARE(Write___STATIC__VOID__U2__U2__U2__U2__SZARRAY_U2);
    NANOCLR_NATIVE_DECLARE(
        Write___STATIC__VOID__STRING__U2__U2__U2__U2__nanoFrameworkUIFont__nanoFrameworkPresentationMediaColor__nanoFrameworkPresentationMediaColor);
    NANOCLR_NATIVE_DECLARE(NativeChangeOrientation___STATIC__BOOLEAN__nanoFrameworkUIDisplayOrientation);
    NANOCLR_NATIVE_DECLARE(
        NativeInitSpi___STATIC__U4__nanoFrameworkUISpiConfiguration__nanoFrameworkUIScreenConfiguration__U4);
    NANOCLR_NATIVE_DECLARE(
        NativeInitI2c___STATIC__U4__nanoFrameworkUII2cConfiguration__nanoFrameworkUIScreenConfiguration__U4);

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_GraphicDriver
{
    static const int FIELD___width = 1;
    static const int FIELD___height = 2;
    static const int FIELD___bitsPerPixel = 3;
    static const int FIELD___initializationSequence = 4;
    static const int FIELD___memoryWrite = 5;
    static const int FIELD___setColumnAddress = 6;
    static const int FIELD___setRowAddress = 7;
    static const int FIELD___powerModeNormal = 8;
    static const int FIELD___powerModeSleep = 9;
    static const int FIELD___orientationPortrait = 10;
    static const int FIELD___orientationPortrait180 = 11;
    static const int FIELD___orientationLandscape = 12;
    static const int FIELD___orientationLandscape180 = 13;
    static const int FIELD___clear = 14;
    static const int FIELD___brightness = 15;
    static const int FIELD___defaultOrientation = 16;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_Ink
{
    NANOCLR_NATIVE_DECLARE(SetInkRegion___STATIC__VOID__U4__I4__I4__I4__I4__I4__I4__I4__nanoFrameworkUIBitmap);
    NANOCLR_NATIVE_DECLARE(ResetInkRegion___STATIC__VOID);

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_Input_RawButtonInputReport
{
    static const int FIELD__Button = 3;
    static const int FIELD__Actions = 4;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_Input_ButtonDevice
{
    static const int FIELD___inputManager = 2;
    static const int FIELD___isActive = 3;
    static const int FIELD___focus = 4;
    static const int FIELD___focusRootUIElement = 5;
    static const int FIELD___previousButton = 6;
    static const int FIELD___isEnabledOrVisibleChangedEventHandler = 7;
    static const int FIELD___buttonState = 8;
    static const int FIELD___tagNonRedundantActions = 9;
    static const int FIELD___tagButton = 10;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_Input_Buttons
{
    static const int FIELD_STATIC__PreviewButtonDownEvent = 23;
    static const int FIELD_STATIC__PreviewButtonUpEvent = 24;
    static const int FIELD_STATIC__ButtonDownEvent = 25;
    static const int FIELD_STATIC__ButtonUpEvent = 26;
    static const int FIELD_STATIC__GotFocusEvent = 27;
    static const int FIELD_STATIC__LostFocusEvent = 28;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_Input_GenericEvents
{
    static const int FIELD_STATIC__GenericStandardEvent = 29;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_Input_InputManager__DeviceEvents
{
    static const int FIELD___preProcessInput = 2;
    static const int FIELD___preNotifyInput = 3;
    static const int FIELD___postNotifyInput = 4;
    static const int FIELD___postProcessInput = 5;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_Input_InputReportArgs
{
    static const int FIELD__Device = 1;
    static const int FIELD__Report = 2;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_Input_InputReportEventArgs
{
    static const int FIELD__Report = 7;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_Input_RawGenericInputReport
{
    static const int FIELD__Target = 3;
    static const int FIELD__InternalEvent = 4;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_Input_RawTouchInputReport
{
    static const int FIELD__Target = 3;
    static const int FIELD__EventMessage = 4;
    static const int FIELD__Touches = 5;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_Input_TouchCapture
{
    static const int FIELD_STATIC___captureElement = 30;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_Input_TouchEvents
{
    static const int FIELD_STATIC__TouchDownEvent = 31;
    static const int FIELD_STATIC__TouchMoveEvent = 32;
    static const int FIELD_STATIC__TouchUpEvent = 33;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_Threading_DispatcherObject
{
    static const int FIELD__Dispatcher = 1;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_Threading_DispatcherOperation__DispatcherOperationEvent
{
    static const int FIELD___operation = 1;
    static const int FIELD___timeout = 2;
    static const int FIELD___event = 3;
    static const int FIELD___waitTimer = 4;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_Threading_DispatcherOperation__DispatcherOperationFrame
{
    static const int FIELD___operation = 4;
    static const int FIELD___waitTimer = 5;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_Threading_DispatcherTimer
{
    static const int FIELD__Tick = 1;
    static const int FIELD___instanceLock = 2;
    static const int FIELD___dispatcher = 3;
    static const int FIELD___interval = 4;
    static const int FIELD___tag = 5;
    static const int FIELD___isEnabled = 6;
    static const int FIELD___timer = 7;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_Touch
{
    static const int FIELD_STATIC___initialized = 34;
    static const int FIELD_STATIC___activeTouchPanel = 35;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_TouchCollector
{
    static const int FIELD__lastTime = 1;
    static const int FIELD___nativeBufferSize = 2;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_TouchCollectorConfiguration
{
    static const int FIELD_STATIC___collectionMode = 36;
    static const int FIELD_STATIC___collectionMethod = 37;
    static const int FIELD_STATIC___touchCollector = 38;
    static const int FIELD_STATIC___collectionBufferSize = 39;

    NANOCLR_NATIVE_DECLARE(GetTouchPoints___STATIC__VOID__BYREF_I4__SZARRAY_I2__SZARRAY_I2);
    NANOCLR_NATIVE_DECLARE(
        GetTouchInput___STATIC__VOID__nanoFrameworkUITouchCollectorConfigurationTouchInput__BYREF_I4__BYREF_I4__BYREF_I4);
    NANOCLR_NATIVE_DECLARE(
        SetTouchInput___STATIC__VOID__nanoFrameworkUITouchCollectorConfigurationTouchInput__I4__I4__I4);

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_TouchEvent
{
    static const int FIELD__Time = 3;
    static const int FIELD__Touches = 4;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_TouchEventProcessor
{
    NANOCLR_NATIVE_DECLARE(ProcessEvent___nanoFrameworkRuntimeEventsBaseEvent__U4__U4__SystemDateTime);

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_TouchPanel
{
    NANOCLR_NATIVE_DECLARE(SetCalibration___VOID__I4__SZARRAY_I2__SZARRAY_I2__SZARRAY_I2__SZARRAY_I2);
    NANOCLR_NATIVE_DECLARE(GetCalibrationPointCount___VOID__BYREF_I4);
    NANOCLR_NATIVE_DECLARE(StartCalibration___VOID);
    NANOCLR_NATIVE_DECLARE(GetCalibrationPoint___VOID__I4__BYREF_I4__BYREF_I4);

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_TouchScreen__ActiveRectangle
{
    static const int FIELD__X = 1;
    static const int FIELD__Y = 2;
    static const int FIELD__Width = 3;
    static const int FIELD__Height = 4;
    static const int FIELD__Target = 5;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_TouchScreenEventArgs
{
    static const int FIELD__Touches = 1;
    static const int FIELD__TimeStamp = 2;
    static const int FIELD__Target = 3;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_TouchScreen
{
    static const int FIELD___activeRegions = 1;
    static const int FIELD___maxWidth = 2;
    static const int FIELD___maxHeight = 3;
    static const int FIELD__OnTouchDown = 4;
    static const int FIELD__OnTouchMove = 5;
    static const int FIELD__OnTouchUp = 6;
    static const int FIELD__OnGestureStarted = 7;
    static const int FIELD__OnGestureChanged = 8;
    static const int FIELD__OnGestureEnded = 9;

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Graphics;

#endif // NANOFRAMEWORK_GRAPHICS_H
