// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.

#include "nano_esp32_cam.h"

#include <CPU_GPIO_decl.h>
#include <esp_camera.h>
#include <targetPAL_Ledc.h>
#include <cstring>

typedef Library_nano_esp32_cam_nanoFramework_Esp32_Camera_Esp32Camera Camera;
typedef Library_nano_esp32_cam_nanoFramework_Esp32_Camera_CameraConnectionSettings Settings;

constexpr ledc_timer_t CameraLedcTimer = LEDC_TIMER_3;
constexpr ledc_channel_t CameraLedcChannel = LEDC_CHANNEL_7;
constexpr size_t CameraPinCount = 16;

static bool s_cameraInitialized = false;
static bool s_cameraLedcReserved = false;
static int s_reservedPins[CameraPinCount];
static size_t s_reservedPinCount = 0;

HRESULT MapEspError(esp_err_t error)
{
    switch (error)
    {
        case ESP_OK:
            return S_OK;
        case ESP_ERR_NO_MEM:
            return CLR_E_OUT_OF_MEMORY;
        case ESP_ERR_TIMEOUT:
            return CLR_E_TIMEOUT;
        case ESP_ERR_INVALID_ARG:
            return CLR_E_INVALID_PARAMETER;
        case ESP_ERR_INVALID_STATE:
            return CLR_E_INVALID_OPERATION;
        default:
            return CLR_E_FAIL;
    }
}

void ReleasePins()
{
    while (s_reservedPinCount > 0)
    {
        CPU_GPIO_ReservePin(s_reservedPins[--s_reservedPinCount], false);
    }
}

static void ReleaseCameraClock()
{
    if (s_cameraLedcReserved)
    {
        Esp32_Ledc_Release(LEDC_LOW_SPEED_MODE, CameraLedcChannel, Esp32LedcOwner::Camera);
        s_cameraLedcReserved = false;
    }
}

static esp_err_t CameraUninitialize()
{
    if (!s_cameraInitialized)
    {
        return ESP_OK;
    }

    esp_err_t result = esp_camera_deinit();
    s_cameraInitialized = false;
    ReleasePins();
    ReleaseCameraClock();
    return result;
}

static void CameraSoftRebootCleanup()
{
    esp_err_t result = CameraUninitialize();
    if (result != ESP_OK && result != ESP_ERR_INVALID_STATE)
    {
        ESP_LOGE("Camera", "Failed to deinitialize during cleanup: %s", esp_err_to_name(result));
    }
}

HRESULT ReservePin(int pin)
{
    if (pin < 0)
    {
        return S_OK;
    }

    if (!CPU_GPIO_ReservePin(pin, true))
    {
        return CLR_E_PIN_UNAVAILABLE;
    }

    s_reservedPins[s_reservedPinCount++] = pin;
    return S_OK;
}

HRESULT ReserveCameraPins(const camera_config_t &config)
{
    const int pins[CameraPinCount] = {
        config.pin_pwdn,
        config.pin_reset,
        config.pin_xclk,
        config.pin_sccb_sda,
        config.pin_sccb_scl,
        config.pin_d7,
        config.pin_d6,
        config.pin_d5,
        config.pin_d4,
        config.pin_d3,
        config.pin_d2,
        config.pin_d1,
        config.pin_d0,
        config.pin_vsync,
        config.pin_href,
        config.pin_pclk};

    s_reservedPinCount = 0;
    for (size_t index = 0; index < CameraPinCount; index++)
    {
        HRESULT result = ReservePin(pins[index]);
        if (FAILED(result))
        {
            ReleasePins();
            return result;
        }
    }

    return S_OK;
}

HRESULT GetSensor(sensor_t *&sensor)
{
    if (!s_cameraInitialized)
    {
        return CLR_E_INVALID_OPERATION;
    }

    sensor = esp_camera_sensor_get();
    return sensor == nullptr ? CLR_E_INVALID_OPERATION : S_OK;
}

HRESULT SetSensorResult(int result)
{
    return result == 0 ? S_OK : CLR_E_INVALID_PARAMETER;
}

HRESULT Camera::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *pThis = nullptr;
    CLR_RT_HeapBlock *settings = nullptr;
    camera_config_t config = {};

    pThis = stack.This();
    FAULT_ON_NULL(pThis);

    if (s_cameraInitialized)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    settings = pThis[FIELD___cameraConnectionSettings].Dereference();
    FAULT_ON_NULL(settings);

    config.pin_pwdn = settings[Settings::FIELD___pinPowerDown].NumericByRef().s4;
    config.pin_reset = settings[Settings::FIELD___pinReset].NumericByRef().s4;
    config.pin_xclk = settings[Settings::FIELD___pinXclk].NumericByRef().s4;
    config.pin_sccb_sda = settings[Settings::FIELD___pinSccbSda].NumericByRef().s4;
    config.pin_sccb_scl = settings[Settings::FIELD___pinSccbScl].NumericByRef().s4;
    config.pin_d7 = settings[Settings::FIELD___pinD7].NumericByRef().s4;
    config.pin_d6 = settings[Settings::FIELD___pinD6].NumericByRef().s4;
    config.pin_d5 = settings[Settings::FIELD___pinD5].NumericByRef().s4;
    config.pin_d4 = settings[Settings::FIELD___pinD4].NumericByRef().s4;
    config.pin_d3 = settings[Settings::FIELD___pinD3].NumericByRef().s4;
    config.pin_d2 = settings[Settings::FIELD___pinD2].NumericByRef().s4;
    config.pin_d1 = settings[Settings::FIELD___pinD1].NumericByRef().s4;
    config.pin_d0 = settings[Settings::FIELD___pinD0].NumericByRef().s4;
    config.pin_vsync = settings[Settings::FIELD___pinVsync].NumericByRef().s4;
    config.pin_href = settings[Settings::FIELD___pinHref].NumericByRef().s4;
    config.pin_pclk = settings[Settings::FIELD___pinPclk].NumericByRef().s4;
    config.xclk_freq_hz = settings[Settings::FIELD___xclkFrequencyHz].NumericByRef().s4;
    config.ledc_timer = CameraLedcTimer;
    config.ledc_channel = CameraLedcChannel;
    config.pixel_format = PIXFORMAT_JPEG;
    config.frame_size = static_cast<framesize_t>(settings[Settings::FIELD___maximumFrameSize].NumericByRef().s4);
    config.jpeg_quality = pThis[FIELD___jpegQuality].NumericByRef().s4;
    config.fb_count = settings[Settings::FIELD___frameBufferCount].NumericByRef().s4;
    config.fb_location = settings[Settings::FIELD___frameBufferLocation].NumericByRef().s4 == FrameBufferLocation_Psram
                             ? CAMERA_FB_IN_PSRAM
                             : CAMERA_FB_IN_DRAM;
    config.grab_mode = settings[Settings::FIELD___grabMode].NumericByRef().s4 == GrabMode_Latest
                           ? CAMERA_GRAB_LATEST
                           : CAMERA_GRAB_WHEN_EMPTY;
    config.sccb_i2c_port = settings[Settings::FIELD___sccbI2cPort].NumericByRef().s4;
    config.jpeg_buffer_size = 0;

    if (config.frame_size < FRAMESIZE_96X96 || config.frame_size >= FRAMESIZE_INVALID || config.fb_count < 1 ||
        config.jpeg_quality < 0 || config.jpeg_quality > 63)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    NANOCLR_CHECK_HRESULT(ReserveCameraPins(config));

#if !defined(CONFIG_IDF_TARGET_ESP32S3)
    // ESP32-S3 generates XCLK with LCD_CAM instead of LEDC.
    if (config.pin_xclk >= 0)
    {
        if (!Esp32_Ledc_Reserve(LEDC_LOW_SPEED_MODE, CameraLedcTimer, CameraLedcChannel, Esp32LedcOwner::Camera))
        {
            ReleasePins();
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
        }
        s_cameraLedcReserved = true;
    }
#endif

    {
        esp_err_t result = esp_camera_init(&config);
        if (result != ESP_OK)
        {
            ReleasePins();
            ReleaseCameraClock();
            NANOCLR_SET_AND_LEAVE(MapEspError(result));
        }
    }

    s_cameraInitialized = true;
    HAL_AddSoftRebootHandler(CameraSoftRebootCleanup);

    NANOCLR_NOCLEANUP();
}

HRESULT Camera::NativeCapture___SZARRAY_U1(CLR_RT_StackFrame &stack)
{
    camera_fb_t *frame = nullptr;
    NANOCLR_HEADER();

    if (!s_cameraInitialized)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    frame = esp_camera_fb_get();
    if (frame == nullptr)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
    }

    {
        CLR_RT_HeapBlock &top = stack.PushValueAndClear();
        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(top, frame->len, g_CLR_RT_WellKnownTypes.m_UInt8));
        memcpy(top.DereferenceArray()->GetFirstElement(), frame->buf, frame->len);
    }

    NANOCLR_CLEANUP();

    if (frame != nullptr)
    {
        esp_camera_fb_return(frame);
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Camera::NativeCaptureToBuffer___I4__SZARRAY_U1(CLR_RT_StackFrame &stack)
{
    camera_fb_t *frame = nullptr;
    CLR_RT_HeapBlock_Array *destination = nullptr;
    NANOCLR_HEADER();

    if (!s_cameraInitialized)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    destination = stack.Arg1().DereferenceArray();
    FAULT_ON_NULL(destination);

    frame = esp_camera_fb_get();
    if (frame == nullptr)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
    }

    if (destination->m_numOfElements < frame->len)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_BUFFER_TOO_SMALL);
    }

    memcpy(destination->GetFirstElement(), frame->buf, frame->len);
    stack.SetResult_I4(static_cast<CLR_INT32>(frame->len));

    NANOCLR_CLEANUP();

    if (frame != nullptr)
    {
        esp_camera_fb_return(frame);
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Camera::NativeGetSensorId___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    sensor_t *sensor = nullptr;
    NANOCLR_CHECK_HRESULT(GetSensor(sensor));
    stack.SetResult_I4(static_cast<CLR_INT32>(sensor->id.PID));

    NANOCLR_NOCLEANUP();
}

HRESULT Camera::NativeSetFrameSize___VOID__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_INT32 value = stack.Arg1().NumericByRef().s4;
    sensor_t *sensor = nullptr;
    if (value < FRAMESIZE_96X96 || value >= FRAMESIZE_INVALID)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    NANOCLR_CHECK_HRESULT(GetSensor(sensor));
    NANOCLR_CHECK_HRESULT(SetSensorResult(sensor->set_framesize(sensor, static_cast<framesize_t>(value))));

    NANOCLR_NOCLEANUP();
}

HRESULT Camera::NativeSetJpegQuality___VOID__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_INT32 value = stack.Arg1().NumericByRef().s4;
    sensor_t *sensor = nullptr;
    if (value < 0 || value > 63)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    NANOCLR_CHECK_HRESULT(GetSensor(sensor));
    NANOCLR_CHECK_HRESULT(SetSensorResult(sensor->set_quality(sensor, value)));

    NANOCLR_NOCLEANUP();
}

HRESULT Camera::NativeSetVerticalFlip___VOID__BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    sensor_t *sensor = nullptr;
    NANOCLR_CHECK_HRESULT(GetSensor(sensor));
    NANOCLR_CHECK_HRESULT(SetSensorResult(sensor->set_vflip(sensor, stack.Arg1().NumericByRef().u1 != 0)));

    NANOCLR_NOCLEANUP();
}

HRESULT Camera::NativeSetHorizontalMirror___VOID__BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    sensor_t *sensor = nullptr;
    NANOCLR_CHECK_HRESULT(GetSensor(sensor));
    NANOCLR_CHECK_HRESULT(SetSensorResult(sensor->set_hmirror(sensor, stack.Arg1().NumericByRef().u1 != 0)));

    NANOCLR_NOCLEANUP();
}

HRESULT Camera::NativeSetBrightness___VOID__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_INT32 value = stack.Arg1().NumericByRef().s4;
    sensor_t *sensor = nullptr;
    if (value < -2 || value > 2)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    NANOCLR_CHECK_HRESULT(GetSensor(sensor));
    NANOCLR_CHECK_HRESULT(SetSensorResult(sensor->set_brightness(sensor, value)));

    NANOCLR_NOCLEANUP();
}

HRESULT Camera::NativeDispose___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    (void)stack;

    if (s_cameraInitialized)
    {
        esp_err_t result = CameraUninitialize();

        if (result != ESP_OK && result != ESP_ERR_INVALID_STATE)
        {
            NANOCLR_SET_AND_LEAVE(MapEspError(result));
        }
    }

    NANOCLR_NOCLEANUP();
}
