// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.

#include "nano_esp32_cam.h"

#include <CPU_GPIO_decl.h>
#include <driver/gpio.h>
#include <esp_camera.h>
#include <esp_chip_info.h>
#include <esp_heap_caps.h>
#include <esp_log.h>
#include <targetPAL_Ledc.h>
#include <freertos/semphr.h>
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
static TaskHandle_t s_captureTask = nullptr;
static SemaphoreHandle_t s_captureRequest = nullptr;
static SemaphoreHandle_t s_captureExited = nullptr;
static portMUX_TYPE s_captureLock = portMUX_INITIALIZER_UNLOCKED;
static camera_fb_t *s_captureFrame = nullptr;
static bool s_captureComplete = false;
static bool s_captureStop = false;
static bool s_cameraStopping = false;
static CLR_UINT32 s_cameraGeneration = 0;
static CLR_RT_StackFrame *s_captureOwner = nullptr;
static int s_captureOwnerPid = 0;

// ESP-IDF task stack sizes are in bytes. Hardware testing must verify headroom on all camera targets.
constexpr uint32_t CameraWorkerStackSize = 2048;

static void CameraCaptureWorker(void *)
{
    for (;;)
    {
        xSemaphoreTake(s_captureRequest, portMAX_DELAY);

        portENTER_CRITICAL(&s_captureLock);
        bool stop = s_captureStop;
        portEXIT_CRITICAL(&s_captureLock);
        if (stop)
        {
            break;
        }

        camera_fb_t *frame = esp_camera_fb_get();

        portENTER_CRITICAL(&s_captureLock);
        s_captureFrame = frame;
        s_captureComplete = true;
        stop = s_captureStop;
        portEXIT_CRITICAL(&s_captureLock);
        Events_Set(SYSTEM_EVENT_FLAG_CAMERA);

        if (stop)
        {
            break;
        }
    }

    xSemaphoreGive(s_captureExited);
    Events_Set(SYSTEM_EVENT_FLAG_CAMERA);
    vTaskDelete(nullptr);
}

static bool CaptureComplete()
{
    portENTER_CRITICAL(&s_captureLock);
    bool complete = s_captureComplete;
    portEXIT_CRITICAL(&s_captureLock);
    return complete;
}

static void ReleaseCapture()
{
    portENTER_CRITICAL(&s_captureLock);
    camera_fb_t *frame = s_captureFrame;
    s_captureFrame = nullptr;
    s_captureComplete = false;
    portEXIT_CRITICAL(&s_captureLock);

    if (frame != nullptr)
    {
        esp_camera_fb_return(frame);
    }
    s_captureOwner = nullptr;
    Events_Set(SYSTEM_EVENT_FLAG_CAMERA);
}

static bool CaptureOwnerIsAlive(CLR_RT_DblLinkedList &threads)
{
    NANOCLR_FOREACH_NODE(CLR_RT_Thread, thread, threads)
    {
        if (thread->m_pid == s_captureOwnerPid && !(thread->m_flags & CLR_RT_Thread::TH_F_Aborted) &&
            thread->CurrentFrame() == s_captureOwner)
        {
            return true;
        }
    }
    NANOCLR_FOREACH_NODE_END();
    return false;
}

static HRESULT AcquireCameraFrame(CLR_RT_StackFrame &stack, camera_fb_t *&frame)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock hbTimeout;
    CLR_INT64 *timeout;
    bool eventResult = true;

    if (!s_cameraInitialized || s_cameraStopping)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    if (stack.m_customState == 0)
    {
        // The driver owns the capture timeout; only suspend the CLR caller while it runs.
        hbTimeout.SetInteger((CLR_INT64)-1);
        NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeout));
        stack.PushValueAndClear().SetInteger(s_cameraGeneration);
    }
    else
    {
        NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeout));
    }

    if (stack.m_evalStack[1].NumericByRef().u4 != s_cameraGeneration)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    for (;;)
    {
        if (stack.m_customState == 1)
        {
            // Reclaim a completed request if its managed caller was aborted while waiting.
            if (s_captureOwner != nullptr && CaptureComplete() &&
                !CaptureOwnerIsAlive(g_CLR_RT_ExecutionEngine.m_threadsReady) &&
                !CaptureOwnerIsAlive(g_CLR_RT_ExecutionEngine.m_threadsWaiting))
            {
                ReleaseCapture();
            }

            if (s_captureOwner == nullptr)
            {
                s_captureOwner = &stack;
                s_captureOwnerPid = stack.m_owningThread->m_pid;
                stack.m_customState = 2;
                xSemaphoreGive(s_captureRequest);
            }
        }

        if (stack.m_customState == 2 && CaptureComplete())
        {
            portENTER_CRITICAL(&s_captureLock);
            frame = s_captureFrame;
            portEXIT_CRITICAL(&s_captureLock);
            if (frame == nullptr)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
            }
            break;
        }

        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeout, Event_Camera, eventResult));
        if (!eventResult)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
        }
    }

    stack.PopValue();
    stack.PopValue();
    stack.m_customState = 3;

    NANOCLR_NOCLEANUP();
}

static void RequestCameraStop()
{
    s_cameraStopping = true;
    portENTER_CRITICAL(&s_captureLock);
    s_captureStop = true;
    portEXIT_CRITICAL(&s_captureLock);
    xSemaphoreGive(s_captureRequest);
}

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
        case ESP_ERR_NOT_FOUND:
        case ESP_ERR_CAMERA_NOT_DETECTED:
            return CLR_E_NOT_FOUND;
        case ESP_ERR_NOT_SUPPORTED:
            return CLR_E_NOT_SUPPORTED;
        case ESP_ERR_CAMERA_FAILED_TO_SET_FRAME_SIZE:
            return CLR_E_INVALID_PARAMETER;
        default:
            return CLR_E_IO;
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

static esp_err_t CameraUninitialize(bool workerExited = false)
{
    if (!s_cameraInitialized)
    {
        return ESP_OK;
    }

    RequestCameraStop();
    // Soft reboot has no managed callers left to schedule. Wait for the worker to leave the driver.
    if (!workerExited)
    {
        xSemaphoreTake(s_captureExited, portMAX_DELAY);
    }
    // The worker self-deletes after acknowledging exit; never delete a task running on another core.
    s_captureTask = nullptr;
    vSemaphoreDelete(s_captureRequest);
    s_captureRequest = nullptr;
    vSemaphoreDelete(s_captureExited);
    s_captureExited = nullptr;
    ReleaseCapture();

    esp_err_t result = esp_camera_deinit();
    s_cameraInitialized = false;
    s_cameraStopping = false;
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
    const char *roles[CameraPinCount] = {
        "PWDN",
        "RESET",
        "XCLK",
        "SCCB SDA",
        "SCCB SCL",
        "D7",
        "D6",
        "D5",
        "D4",
        "D3",
        "D2",
        "D1",
        "D0",
        "VSYNC",
        "HREF",
        "PCLK"};

    s_reservedPinCount = 0;
    for (size_t index = 0; index < CameraPinCount; index++)
    {
        if ((pins[index] != -1 || index >= 5) &&
            (!GPIO_IS_VALID_GPIO(pins[index]) || (index < 5 && !GPIO_IS_VALID_OUTPUT_GPIO(pins[index]))))
        {
            ESP_LOGE("Camera", "GPIO configuration failed: %s GPIO=%d", roles[index], pins[index]);
            ReleasePins();
            return CLR_E_INVALID_PARAMETER;
        }
        HRESULT result = ReservePin(pins[index]);
        if (FAILED(result))
        {
            ESP_LOGE("Camera", "GPIO reservation failed: %s GPIO=%d", roles[index], pins[index]);
            ReleasePins();
            return result;
        }
    }

    return S_OK;
}

HRESULT GetSensor(sensor_t *&sensor)
{
    if (!s_cameraInitialized || s_cameraStopping)
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
    esp_chip_info_t chip = {};

    esp_chip_info(&chip);
    ESP_LOGI(
        "Camera",
        "Production NativeInit: target=%s model=%d revision=%d cores=%d",
        CONFIG_IDF_TARGET,
        static_cast<int>(chip.model),
        chip.revision,
        chip.cores);
    pThis = stack.This();
    if (pThis == nullptr)
    {
        ESP_LOGE("Camera", "Configuration failed: null camera instance");
    }
    FAULT_ON_NULL(pThis);

    if (s_cameraInitialized)
    {
        ESP_LOGE("Camera", "Instance check failed: a camera is already initialized");
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    settings = pThis[FIELD___cameraConnectionSettings].Dereference();
    if (settings == nullptr)
    {
        ESP_LOGE("Camera", "Configuration failed: null CameraConnectionSettings");
    }
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

    ESP_LOGI(
        "Camera",
        "Config: XCLK=%dHz frame=%d JPEG=%d buffers=%u location=%s grab=%d SCCB=%d/%d port=%d",
        config.xclk_freq_hz,
        static_cast<int>(config.frame_size),
        config.jpeg_quality,
        static_cast<unsigned>(config.fb_count),
        config.fb_location == CAMERA_FB_IN_PSRAM ? "PSRAM" : "DRAM",
        static_cast<int>(config.grab_mode),
        config.pin_sccb_sda,
        config.pin_sccb_scl,
        config.sccb_i2c_port);
    ESP_LOGI(
        "Camera",
        "Heap: PSRAM available=%d internal free=%u largest=%u PSRAM free=%u",
        heap_caps_get_total_size(MALLOC_CAP_SPIRAM) != 0,
        static_cast<unsigned>(heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)),
        static_cast<unsigned>(heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)),
        static_cast<unsigned>(heap_caps_get_free_size(MALLOC_CAP_SPIRAM)));

    if (config.frame_size < FRAMESIZE_96X96 || config.frame_size >= FRAMESIZE_INVALID ||
        settings[Settings::FIELD___frameBufferCount].NumericByRef().s4 < 1 || config.xclk_freq_hz <= 0 ||
        config.jpeg_quality < 0 || config.jpeg_quality > 63 ||
        (settings[Settings::FIELD___frameBufferLocation].NumericByRef().s4 != FrameBufferLocation_Psram &&
         settings[Settings::FIELD___frameBufferLocation].NumericByRef().s4 != FrameBufferLocation_Dram) ||
        (settings[Settings::FIELD___grabMode].NumericByRef().s4 != GrabMode_WhenEmpty &&
         settings[Settings::FIELD___grabMode].NumericByRef().s4 != GrabMode_Latest))
    {
        ESP_LOGE(
            "Camera",
            "Configuration failed: invalid clock, frame size, buffer count/location, grab mode or quality");
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    if ((config.pin_sccb_sda == -1) != (config.pin_sccb_scl == -1) ||
        (config.pin_sccb_sda == -1 && (config.sccb_i2c_port < 0 || config.sccb_i2c_port >= SOC_I2C_NUM)))
    {
        ESP_LOGE("Camera", "SCCB configuration failed: specify both pins or an existing valid I2C port");
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    if (config.fb_location == CAMERA_FB_IN_PSRAM && heap_caps_get_total_size(MALLOC_CAP_SPIRAM) == 0)
    {
        ESP_LOGE("Camera", "Framebuffer configuration failed: PSRAM requested but unavailable");
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    NANOCLR_CHECK_HRESULT(ReserveCameraPins(config));

#if !defined(CONFIG_IDF_TARGET_ESP32S3)
    // ESP32-S3 generates XCLK with LCD_CAM instead of LEDC.
    if (config.pin_xclk >= 0)
    {
        if (!Esp32_Ledc_Reserve(LEDC_LOW_SPEED_MODE, CameraLedcTimer, CameraLedcChannel, Esp32LedcOwner::Camera))
        {
            ESP_LOGE(
                "Camera",
                "LEDC reservation failed: mode=%d timer=%d channel=%d",
                LEDC_LOW_SPEED_MODE,
                CameraLedcTimer,
                CameraLedcChannel);
            ReleasePins();
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
        }
        s_cameraLedcReserved = true;
    }
#endif

    {
        ESP_LOGI("Camera", "Calling esp_camera_init");
        esp_err_t result = esp_camera_init(&config);
        ESP_LOGI(
            "Camera",
            "esp_camera_init returned 0x%x (%s)",
            static_cast<unsigned>(result),
            esp_err_to_name(result));
        if (result != ESP_OK)
        {
            ESP_LOGE("Camera", "Driver initialization failed; see camera/SCCB/LEDC driver diagnostics above");
            ReleasePins();
            ReleaseCameraClock();
            NANOCLR_SET_AND_LEAVE(MapEspError(result));
        }
    }

    s_cameraInitialized = true;
    s_cameraGeneration++;
    s_captureStop = false;
    s_captureRequest = xSemaphoreCreateBinary();
    s_captureExited = xSemaphoreCreateBinary();
    if (s_captureRequest == nullptr || s_captureExited == nullptr ||
        xTaskCreate(
            CameraCaptureWorker,
            "CameraCapture",
            CameraWorkerStackSize,
            nullptr,
            uxTaskPriorityGet(nullptr),
            &s_captureTask) != pdPASS)
    {
        ESP_LOGE("Camera", "Capture worker allocation failed");
        if (s_captureRequest != nullptr)
        {
            vSemaphoreDelete(s_captureRequest);
            s_captureRequest = nullptr;
        }
        if (s_captureExited != nullptr)
        {
            vSemaphoreDelete(s_captureExited);
            s_captureExited = nullptr;
        }
        esp_err_t result = esp_camera_deinit();
        if (result != ESP_OK && result != ESP_ERR_INVALID_STATE)
        {
            ESP_LOGE("Camera", "Failed to deinitialize after worker allocation failure: %s", esp_err_to_name(result));
        }
        s_cameraInitialized = false;
        ReleasePins();
        ReleaseCameraClock();
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }
    HAL_AddSoftRebootHandler(CameraSoftRebootCleanup);
    ESP_LOGI("Camera", "Initialization complete");

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

    NANOCLR_CHECK_HRESULT(AcquireCameraFrame(stack, frame));

    {
        CLR_RT_HeapBlock &top = stack.PushValueAndClear();
        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(top, frame->len, g_CLR_RT_WellKnownTypes.m_UInt8));
        memcpy(top.DereferenceArray()->GetFirstElement(), frame->buf, frame->len);
    }

    NANOCLR_CLEANUP();

    if (hr != CLR_E_THREAD_WAITING && s_captureOwner == &stack)
    {
        ReleaseCapture();
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

    if (stack.Arg1().DereferenceArray() == nullptr)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_ARGUMENT_NULL);
    }

    NANOCLR_CHECK_HRESULT(AcquireCameraFrame(stack, frame));

    // The GC may relocate the array while this caller is suspended.
    destination = stack.Arg1().DereferenceArray();
    FAULT_ON_NULL(destination);

    if (destination->m_numOfElements < frame->len)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_BUFFER_TOO_SMALL);
    }

    memcpy(destination->GetFirstElement(), frame->buf, frame->len);
    stack.SetResult_I4(static_cast<CLR_INT32>(frame->len));

    NANOCLR_CLEANUP();

    if (hr != CLR_E_THREAD_WAITING && s_captureOwner == &stack)
    {
        ReleaseCapture();
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

    CLR_RT_HeapBlock hbTimeout;
    CLR_INT64 *timeout;
    bool eventResult = true;

    if (stack.m_customState == 0)
    {
        if (!s_cameraInitialized)
        {
            NANOCLR_SET_AND_LEAVE(S_OK);
        }

        hbTimeout.SetInteger((CLR_INT64)-1);
        NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeout));
        stack.PushValueAndClear().SetInteger(s_cameraGeneration);
        RequestCameraStop();
    }
    else
    {
        NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeout));
    }

    while (s_cameraInitialized && stack.m_evalStack[1].NumericByRef().u4 == s_cameraGeneration)
    {
        if (xSemaphoreTake(s_captureExited, 0) == pdTRUE)
        {
            esp_err_t result = CameraUninitialize(true);
            stack.PopValue();
            stack.PopValue();
            if (result != ESP_OK && result != ESP_ERR_INVALID_STATE)
            {
                NANOCLR_SET_AND_LEAVE(MapEspError(result));
            }
            NANOCLR_SET_AND_LEAVE(S_OK);
        }

        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeout, Event_Camera, eventResult));
        if (!eventResult)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
        }
    }

    stack.PopValue();
    stack.PopValue();

    NANOCLR_NOCLEANUP();
}
