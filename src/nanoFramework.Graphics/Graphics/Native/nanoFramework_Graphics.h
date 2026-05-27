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

typedef enum __nfpack DrawTextOptions
{
    DrawTextOptions_None = 0,
    DrawTextOptions_WordWrap = 1,
    DrawTextOptions_TruncateAtBottom = 4,
    DrawTextOptions_Ellipsis = 8,
    DrawTextOptions_IgnoreHeight = 16,
    DrawTextOptions_AlignmentLeft = 0,
    DrawTextOptions_AlignmentCenter = 2,
    DrawTextOptions_AlignmentRight = 32,
    DrawTextOptions_AlignmentMask = 34,
    DrawTextOptions_TrimmingNone = 0,
    DrawTextOptions_TrimmingWordEllipsis = 8,
    DrawTextOptions_TrimmingCharacterEllipsis = 64,
    DrawTextOptions_TrimmingMask = 72,
} DrawTextOptions;

/* moved to Display.h for convenience
typedef enum __nfpack GraphicDriverCommandType
{
    GraphicDriverCommandType_Sleep = 0,
    GraphicDriverCommandType_Command = 1,
} GraphicDriverCommandType;
*/

typedef enum __nfpack TouchCaptureMode
{
    TouchCaptureMode_None = 0,
    TouchCaptureMode_Element = 1,
    TouchCaptureMode_SubTree = 2,
} TouchCaptureMode;

/*
typedef enum __nfpack RoutedEventArgs_Flags
{
    RoutedEventArgs_Flags_Handled = 1,
    RoutedEventArgs_Flags_InvokingHandler = 2,
} RoutedEventArgs_Flags;

typedef enum __nfpack SetWindowType
{
    SetWindowType_NoWindowing = 0,
    SetWindowType_X8bitsY1Bit = 1,
    SetWindowType_X8bitsY8Bits = 2,
    SetWindowType_X16bitsY16Bit = 3,
} SetWindowType;
*/

typedef enum __nfpack TouchCollectorConfiguration_TouchInput
{
    TouchCollectorConfiguration_TouchInput_LastTouchPoint = 2,
    TouchCollectorConfiguration_TouchInput_SamplingDistance = 4,
    TouchCollectorConfiguration_TouchInput_TouchMoveFrequency = 8,
} TouchCollectorConfiguration_TouchInput;

typedef enum __nfpack TouchInputFlags
{
    TouchInputFlags_None = 0,
    TouchInputFlags_Primary = 16,
    TouchInputFlags_Pen = 64,
    TouchInputFlags_Palm = 128,
} TouchInputFlags;

struct Library_nanoFramework_Graphics_nanoFramework_UI_RoutedEvent
{
    static const int FIELD_STATIC___eventCount = 0;

    static const int FIELD___name = 1;
    static const int FIELD___routingStrategy = 2;
    static const int FIELD___handlerType = 3;
    static const int FIELD___globalIndex = 4;

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

    static const int FIELD___currentFrame = 1;
    static const int FIELD___frameDepth = 2;
    static const int FIELD___hasShutdownStarted = 3;
    static const int FIELD___hasShutdownFinished = 4;
    static const int FIELD___queue = 5;
    static const int FIELD___event = 6;
    static const int FIELD___instanceLock = 7;
    static const int FIELD___thread = 8;
    static const int FIELD___finalExceptionHandler = 9;
    static const int FIELD___layoutManager = 10;
    static const int FIELD___inputManager = 11;
    static const int FIELD___mediaContext = 12;
    static const int FIELD__ShutdownStarted = 13;
    static const int FIELD__ShutdownFinished = 14;

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
    static const int FIELD___dispatcher = 1;
    static const int FIELD___method = 2;
    static const int FIELD___args = 3;
    static const int FIELD___result = 4;
    static const int FIELD___status = 5;
    static const int FIELD__Aborted = 6;
    static const int FIELD__Completed = 7;

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
    NANOCLR_NATIVE_DECLARE(SetClippingRectangle___VOID__I4__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(get_Width___I4);
    NANOCLR_NATIVE_DECLARE(get_Height___I4);
    NANOCLR_NATIVE_DECLARE(DrawImage___VOID__I4__I4__nanoFrameworkUIBitmap__I4__I4__I4__I4__U2);
    NANOCLR_NATIVE_DECLARE(RotateImage___VOID__I4__I4__I4__nanoFrameworkUIBitmap__I4__I4__I4__I4__U2);
    NANOCLR_NATIVE_DECLARE(StretchImage___VOID__I4__I4__nanoFrameworkUIBitmap__I4__I4__U2);
    NANOCLR_NATIVE_DECLARE(GetBitmap___SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(StretchImage___VOID__I4__I4__I4__I4__nanoFrameworkUIBitmap__I4__I4__I4__I4__U2);
    NANOCLR_NATIVE_DECLARE(TileImage___VOID__I4__I4__nanoFrameworkUIBitmap__I4__I4__U2);
    NANOCLR_NATIVE_DECLARE(Scale9Image___VOID__I4__I4__I4__I4__nanoFrameworkUIBitmap__I4__I4__I4__I4__U2);
    NANOCLR_NATIVE_DECLARE(Dispose___VOID__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(DrawChar___VOID__U2__I4__I4__U4__nanoFrameworkUIFont);
    NANOCLR_NATIVE_DECLARE(DrawEllipse___VOID__U4__I4__I4__I4__I4__I4__U4__I4__I4__U4__I4__I4__U2);
    NANOCLR_NATIVE_DECLARE(MakeTransparent___VOID__U4);
    NANOCLR_NATIVE_DECLARE(DrawLine___VOID__U4__I4__I4__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(DrawRectangle___VOID__U4__I4__I4__I4__I4__I4__I4__I4__U4__I4__I4__U4__I4__I4__U2);
    NANOCLR_NATIVE_DECLARE(DrawRectangle___VOID__I4__I4__I4__I4__I4__U4);
    NANOCLR_NATIVE_DECLARE(FillGradientRectangle___VOID__I4__I4__I4__I4__U4__I4__I4__U4__I4__I4__U2);
    NANOCLR_NATIVE_DECLARE(FillRectangle___VOID__I4__I4__I4__I4__U4__U2);
    NANOCLR_NATIVE_DECLARE(DrawRoundRectangle___VOID__I4__I4__I4__I4__I4__I4__I4__U4);
    NANOCLR_NATIVE_DECLARE(FillRoundRectangle___VOID__I4__I4__I4__I4__I4__I4__U4__U2);
    NANOCLR_NATIVE_DECLARE(DrawText___VOID__STRING__nanoFrameworkUIFont__U4__I4__I4);
    NANOCLR_NATIVE_DECLARE(SetPixel___VOID__I4__I4__U4);
    NANOCLR_NATIVE_DECLARE(GetPixelInt___U4__I4__I4);
    NANOCLR_NATIVE_DECLARE(
        DrawTextInRect___BOOLEAN__BYREF_STRING__BYREF_I4__BYREF_I4__I4__I4__I4__I4__U4__U4__nanoFrameworkUIFont);

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
    static const int FIELD___bitmap = 2;
    static const int FIELD___clippingRectangles = 3;
    static const int FIELD__EmptyClipRect = 4;
    static const int FIELD___x = 5;
    static const int FIELD___y = 6;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_Presentation_Media_MediaContext
{
    static const int FIELD___currentRenderOp = 2;
    static const int FIELD___renderMessage = 3;
    static const int FIELD___isRendering = 4;
    static const int FIELD___invokeOnRenderCallbacks = 5;
    static const int FIELD___target = 6;
    static const int FIELD___screen = 7;
    static const int FIELD___screenW = 8;
    static const int FIELD___screenH = 9;
    static const int FIELD___dirtyX0 = 10;
    static const int FIELD___dirtyY0 = 11;
    static const int FIELD___dirtyX1 = 12;
    static const int FIELD___dirtyY1 = 13;

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
    static const int FIELD___routeItemList = 1;
    static const int FIELD__RoutedEvent = 2;

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
    static const int FIELD_STATIC__DockPropertiesKeys = 7;
    static const int FIELD_STATIC__DockPropertiesValues = 8;

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
    static const int FIELD_STATIC__Instance = 9;

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
    static const int FIELD_STATIC__PreviewInputReportEvent = 10;
    static const int FIELD_STATIC__InputReportEvent = 11;

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
    static const int FIELD_STATIC___isShuttingDown = 12;
    static const int FIELD_STATIC___appCreatedInThisAppDomain = 13;
    static const int FIELD_STATIC___appInstance = 14;
    static const int FIELD_STATIC___reportInputMethod = 15;
    static const int FIELD_STATIC___inputManager = 16;
    static const int FIELD_STATIC___stylusMaxX = 17;
    static const int FIELD_STATIC___stylusMaxY = 18;

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
    static const int FIELD_STATIC___maximumBufferSize = 19;
    static const int FIELD_STATIC___fullScreen = 20;
    static const int FIELD_STATIC___point = 21;

    NANOCLR_NATIVE_DECLARE(get_LongerSide___STATIC__I4);
    NANOCLR_NATIVE_DECLARE(get_ShorterSide___STATIC__I4);
    NANOCLR_NATIVE_DECLARE(get_ScreenWidth___STATIC__I4);
    NANOCLR_NATIVE_DECLARE(get_ScreenHeight___STATIC__I4);
    NANOCLR_NATIVE_DECLARE(get_BitsPerPixel___STATIC__I4);
    NANOCLR_NATIVE_DECLARE(get_Orientation___STATIC__nanoFrameworkUIDisplayOrientation);
    NANOCLR_NATIVE_DECLARE(Clear___STATIC__VOID);
    NANOCLR_NATIVE_DECLARE(Write___STATIC__VOID__U2__U2__U2__U2__SZARRAY_U2);
    NANOCLR_NATIVE_DECLARE(Write___STATIC__VOID__STRING__U2__U2__U2__U2__nanoFrameworkUIFont__U4__U4);
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
    static const int FIELD___setWindowType = 17;

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
    static const int FIELD_STATIC__PreviewButtonDownEvent = 22;
    static const int FIELD_STATIC__PreviewButtonUpEvent = 23;
    static const int FIELD_STATIC__ButtonDownEvent = 24;
    static const int FIELD_STATIC__ButtonUpEvent = 25;
    static const int FIELD_STATIC__GotFocusEvent = 26;
    static const int FIELD_STATIC__LostFocusEvent = 27;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_Input_GenericEvents
{
    static const int FIELD_STATIC__GenericStandardEvent = 28;

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
    static const int FIELD_STATIC___captureElement = 29;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_Input_TouchEvents
{
    static const int FIELD_STATIC__TouchDownEvent = 30;
    static const int FIELD_STATIC__TouchMoveEvent = 31;
    static const int FIELD_STATIC__TouchUpEvent = 32;

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
    static const int FIELD___instanceLock = 1;
    static const int FIELD___dispatcher = 2;
    static const int FIELD___interval = 3;
    static const int FIELD___tag = 4;
    static const int FIELD___isEnabled = 5;
    static const int FIELD___timer = 6;
    static const int FIELD__Tick = 7;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_Touch
{
    static const int FIELD_STATIC___initialized = 33;
    static const int FIELD_STATIC___activeTouchPanel = 34;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_TouchCollector
{
    static const int FIELD___nativeBufferSize = 1;
    static const int FIELD__lastTime = 2;

    //--//
};

struct Library_nanoFramework_Graphics_nanoFramework_UI_TouchCollectorConfiguration
{
    static const int FIELD_STATIC___collectionMode = 35;
    static const int FIELD_STATIC___collectionMethod = 36;
    static const int FIELD_STATIC___touchCollector = 37;
    static const int FIELD_STATIC___collectionBufferSize = 38;

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
