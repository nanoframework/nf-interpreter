// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.

#ifndef NANO_ESP32_CAM_H
#define NANO_ESP32_CAM_H

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>
#include <corlib_native.h>
#include <esp_camera.h>

typedef enum __nfpack CameraInitResult
{
    CameraInitResult_Success = 0,
    CameraInitResult_AlreadyInitialized = 1,
    CameraInitResult_InvalidPin = 2,
    CameraInitResult_PinUnavailable = 3,
    CameraInitResult_InvalidSccbConfiguration = 4,
    CameraInitResult_InvalidXclkFrequency = 5,
    CameraInitResult_InvalidFrameSize = 6,
    CameraInitResult_InvalidJpegQuality = 7,
    CameraInitResult_InvalidFrameBufferCount = 8,
    CameraInitResult_InvalidFrameBufferLocation = 9,
    CameraInitResult_InvalidGrabMode = 10,
    CameraInitResult_PsramUnavailable = 11,
    CameraInitResult_ClockUnavailable = 12,
    CameraInitResult_SensorNotDetected = 13,
    CameraInitResult_SensorNotSupported = 14,
    CameraInitResult_FailedToSetFrameSize = 15,
    CameraInitResult_FailedToSetPixelFormat = 16,
    CameraInitResult_OutOfMemory = 17,
    CameraInitResult_DriverError = 18,
} CameraInitResult;

typedef enum __nfpack CameraSensor
{
    CameraSensor_Unknown = 0,
    CameraSensor_Ov9650 = 150,
    CameraSensor_Ov7725 = 119,
    CameraSensor_Ov2640 = 38,
    CameraSensor_Ov3660 = 13920,
    CameraSensor_Ov3640 = 13888,
    CameraSensor_Ov5640 = 22080,
    CameraSensor_Ov7670 = 118,
    CameraSensor_Nt99141 = 5136,
    CameraSensor_Gc2145 = 8517,
    CameraSensor_Gc032a = 9002,
    CameraSensor_Gc0308 = 155,
    CameraSensor_Bf3005 = 48,
    CameraSensor_Bf20a6 = 8358,
    CameraSensor_Sc101iot = 55882,
    CameraSensor_Sc030iot = 39494,
    CameraSensor_Sc031gs = 49,
    CameraSensor_MegaCcm = 926,
    CameraSensor_Hm1055 = 2389,
    CameraSensor_Hm0360 = 864,
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

typedef enum __nfpack SensorSetting
{
    SensorSetting_FrameSize = 0,
    SensorSetting_Quality = 1,
    SensorSetting_Brightness = 2,
    SensorSetting_VFlip = 3,
    SensorSetting_HMirror = 4,
} SensorSetting;

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
    static const int FIELD___xclkFreqHz = 17;
    static const int FIELD___frameSize = 18;
    static const int FIELD___jpegQuality = 19;
    static const int FIELD___frameBufferCount = 20;
    static const int FIELD___frameBufferLocation = 21;
    static const int FIELD___grabMode = 22;
    static const int FIELD___sccbI2cPort = 23;

    //--//
};

struct Library_nano_esp32_cam_nanoFramework_Esp32_Camera_Esp32Camera
{
    static const int FIELD___syncLock = 1;
    static const int FIELD___connectionSettings = 2;
    static const int FIELD___disposed = 3;
    static const int FIELD___initialized = 4;

    NANOCLR_NATIVE_DECLARE(NativeInit___nanoFrameworkEsp32CameraCameraInitResult);
    NANOCLR_NATIVE_DECLARE(NativeCapture___SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(NativeCaptureToBuffer___I4__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(NativeGetSensorId___I4);
    NANOCLR_NATIVE_DECLARE(NativeGetSensorSetting___I4__nanoFrameworkEsp32CameraSensorSetting);
    NANOCLR_NATIVE_DECLARE(NativeSetSensorSetting___VOID__nanoFrameworkEsp32CameraSensorSetting__I4);
    NANOCLR_NATIVE_DECLARE(NativeDispose___VOID);

    //--//

    static void Uninitialize();
    static HRESULT CaptureExecution(CLR_RT_StackFrame &stack, camera_fb_t *&frame);
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Esp32_Camera;

#endif // NANO_ESP32_CAM_H
