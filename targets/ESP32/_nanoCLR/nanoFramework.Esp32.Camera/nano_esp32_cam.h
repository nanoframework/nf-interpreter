// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.

#ifndef NANO_ESP32_CAM_H
#define NANO_ESP32_CAM_H

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>
#include <corlib_native.h>

typedef enum __nfpack CameraSensor
{
    CameraSensor_Unknown = 0,
    CameraSensor_Ov2640 = 38,
    CameraSensor_Ov3660 = 13920,
    CameraSensor_Ov5640 = 22080,
    CameraSensor_Gc0308 = 155,
} CameraSensor;

typedef enum __nfpack FrameBufferLocation
{
    FrameBufferLocation_Psram = 0,
    FrameBufferLocation_Dram = 1,
} FrameBufferLocation;

typedef enum __nfpack FrameSize
{
    FrameSize_Size96X96 = 0,
    FrameSize_Qqvga = 1,
    FrameSize_Size128X128 = 2,
    FrameSize_Qcif = 3,
    FrameSize_Hqvga = 4,
    FrameSize_Size240X240 = 5,
    FrameSize_Qvga = 6,
    FrameSize_Size320X320 = 7,
    FrameSize_Cif = 8,
    FrameSize_Hvga = 9,
    FrameSize_Vga = 10,
    FrameSize_Svga = 11,
    FrameSize_Xga = 12,
    FrameSize_Hd = 13,
    FrameSize_Sxga = 14,
    FrameSize_Uxga = 15,
    FrameSize_Fhd = 16,
    FrameSize_PortraitHd = 17,
    FrameSize_Portrait3Mp = 18,
    FrameSize_Qxga = 19,
    FrameSize_Qhd = 20,
    FrameSize_Wqxga = 21,
    FrameSize_PortraitFhd = 22,
    FrameSize_Qsxga = 23,
    FrameSize_Size5Mp = 24,
} FrameSize;

typedef enum __nfpack GrabMode
{
    GrabMode_WhenEmpty = 0,
    GrabMode_Latest = 1,
} GrabMode;

struct Library_nano_esp32_cam_nanoFramework_Esp32_Camera_CameraConnectionSettings
{
    static const int FIELD___pinPowerDown = 1;
    static const int FIELD___pinReset = 2;
    static const int FIELD___pinXclk = 3;
    static const int FIELD___pinSccbSda = 4;
    static const int FIELD___pinSccbScl = 5;
    static const int FIELD___pinD7 = 6;
    static const int FIELD___pinD6 = 7;
    static const int FIELD___pinD5 = 8;
    static const int FIELD___pinD4 = 9;
    static const int FIELD___pinD3 = 10;
    static const int FIELD___pinD2 = 11;
    static const int FIELD___pinD1 = 12;
    static const int FIELD___pinD0 = 13;
    static const int FIELD___pinVsync = 14;
    static const int FIELD___pinHref = 15;
    static const int FIELD___pinPclk = 16;
    static const int FIELD___xclkFrequencyHz = 17;
    static const int FIELD___frameBufferCount = 18;
    static const int FIELD___maximumFrameSize = 19;
    static const int FIELD___frameBufferLocation = 20;
    static const int FIELD___grabMode = 21;
    static const int FIELD___sccbI2cPort = 22;

    //--//
};

struct Library_nano_esp32_cam_nanoFramework_Esp32_Camera_Esp32Camera
{
    static const int FIELD___syncLock = 1;
    static const int FIELD___cameraConnectionSettings = 2;
    static const int FIELD___disposed = 3;
    static const int FIELD___initialized = 4;
    static const int FIELD___frameSize = 5;
    static const int FIELD___jpegQuality = 6;
    static const int FIELD___verticalFlip = 7;
    static const int FIELD___horizontalMirror = 8;
    static const int FIELD___brightness = 9;

    NANOCLR_NATIVE_DECLARE(NativeInit___VOID);
    NANOCLR_NATIVE_DECLARE(NativeCapture___SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(NativeCaptureToBuffer___I4__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(NativeGetSensorId___I4);
    NANOCLR_NATIVE_DECLARE(NativeSetFrameSize___VOID__I4);
    NANOCLR_NATIVE_DECLARE(NativeSetJpegQuality___VOID__I4);
    NANOCLR_NATIVE_DECLARE(NativeSetVerticalFlip___VOID__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeSetHorizontalMirror___VOID__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeSetBrightness___VOID__I4);
    NANOCLR_NATIVE_DECLARE(NativeDispose___VOID);

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Esp32_Camera;

#endif // NANO_ESP32_CAM_H
