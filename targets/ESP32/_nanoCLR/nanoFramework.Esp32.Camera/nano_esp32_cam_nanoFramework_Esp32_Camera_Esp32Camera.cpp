// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.

#include "nano_esp32_cam.h"

#include <CPU_GPIO_decl.h>
#include <driver/gpio.h>
#include <esp_camera.h>
#include <esp_heap_caps.h>
#include <targetPAL_Ledc.h>
#include <freertos/semphr.h>

#if defined(DEBUG)
#include <esp_log.h>
#endif

// LEDC timer and channel used to generate XCLK
#define CAMERA_LEDC_TIMER   LEDC_TIMER_3
#define CAMERA_LEDC_CHANNEL LEDC_CHANNEL_7

// number of pins in camera_config_t, from pin_pwdn to pin_pclk
#define CAMERA_PIN_COUNT 16

// stack size in bytes for the capture worker task
#define CAMERA_CAPTURE_WORKER_TASK_STACK_SIZE 2048

// priority of the capture worker task, above the CLR task so the frame hand-off isn't delayed
// (same as the serial port, I2C slave and 1-Wire worker tasks)
#define CAMERA_CAPTURE_WORKER_TASK_PRIORITY 12

typedef Library_nano_esp32_cam_nanoFramework_Esp32_Camera_CameraConnectionSettings Settings;

// capture worker task
static TaskHandle_t CaptureWorkerTaskHandle = NULL;
// signaled to request a capture (or the worker task exit)
static SemaphoreHandle_t CaptureRequest = NULL;
// task waiting for the worker task to exit
static TaskHandle_t DisposingTask = NULL;
static volatile bool CaptureWorkerExit = false;
// frame captured by the worker task, NULL if the driver didn't get a frame
static camera_fb_t *volatile CapturedFrame = NULL;

static int ReservedPins[CAMERA_PIN_COUNT];
static int ReservedPinCount = 0;
static bool LedcReserved = false;

static void CameraCaptureWorkerTask(void *pvParameters)
{
    (void)pvParameters;

    while (true)
    {
        // wait for a capture request
        xSemaphoreTake(CaptureRequest, portMAX_DELAY);

        if (CaptureWorkerExit)
        {
            break;
        }

        // waits for a frame to be available...
        // ... or the driver timeout expires in which case it returns NULL
        CapturedFrame = esp_camera_fb_get();

        // fire event for capture completed
        Events_Set(SYSTEM_EVENT_FLAG_CAMERA);
    }

    // done with the driver, let the disposing task proceed
    xTaskNotifyGive(DisposingTask);

    vTaskDelete(NULL);
}

void Library_nano_esp32_cam_nanoFramework_Esp32_Camera_Esp32Camera::Uninitialize()
{
    if (CaptureWorkerTaskHandle != NULL)
    {
        // request the worker task to exit and wait for it to leave the driver
        // this is immediate unless the worker task is still waiting for a frame from the driver
        DisposingTask = xTaskGetCurrentTaskHandle();
        CaptureWorkerExit = true;
        xSemaphoreGive(CaptureRequest);

        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        CaptureWorkerTaskHandle = NULL;
    }

    if (CaptureRequest != NULL)
    {
        vSemaphoreDelete(CaptureRequest);
        CaptureRequest = NULL;
    }

    // the driver is initialized when it has a sensor
    if (esp_camera_sensor_get() != NULL)
    {
        // a frame that wasn't returned is freed along with the frame buffers
        esp_camera_deinit();
    }

    CapturedFrame = NULL;
    CaptureWorkerExit = false;

    if (LedcReserved)
    {
        Esp32_Ledc_Release(LEDC_LOW_SPEED_MODE, CAMERA_LEDC_CHANNEL, Esp32LedcOwner::Camera);
        LedcReserved = false;
    }

    while (ReservedPinCount > 0)
    {
        CPU_GPIO_ReservePin(ReservedPins[--ReservedPinCount], false);
    }
}

HRESULT Library_nano_esp32_cam_nanoFramework_Esp32_Camera_Esp32Camera::
    NativeInit___nanoFrameworkEsp32CameraCameraInitResult(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CameraInitResult result = CameraInitResult_Success;
    CLR_RT_HeapBlock *pThis;
    CLR_RT_HeapBlock *settings;
    camera_config_t config = {};
    int pins[CAMERA_PIN_COUNT];
    esp_err_t error;
    bool driverInitialized = false;
    int frameBufferLocation;
    int grabMode;

    // get a pointer to the managed object instance and check that it's not NULL
    pThis = stack.This();
    FAULT_ON_NULL(pThis);

    settings = pThis[FIELD___connectionSettings].Dereference();
    FAULT_ON_NULL(settings);

    // the esp32-camera driver supports a single camera
    if (CaptureWorkerTaskHandle != NULL)
    {
        result = CameraInitResult_AlreadyInitialized;
        NANOCLR_SET_AND_LEAVE(S_OK);
    }

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
    config.xclk_freq_hz = settings[Settings::FIELD___xclkFreqHz].NumericByRef().s4;
    config.ledc_timer = CAMERA_LEDC_TIMER;
    config.ledc_channel = CAMERA_LEDC_CHANNEL;
    config.pixel_format = PIXFORMAT_JPEG;
    config.frame_size = (framesize_t)settings[Settings::FIELD___frameSize].NumericByRef().s4;
    config.jpeg_quality = settings[Settings::FIELD___jpegQuality].NumericByRef().s4;
    config.fb_count = settings[Settings::FIELD___frameBufferCount].NumericByRef().s4;
    config.sccb_i2c_port = settings[Settings::FIELD___sccbI2cPort].NumericByRef().s4;

    frameBufferLocation = settings[Settings::FIELD___frameBufferLocation].NumericByRef().s4;
    grabMode = settings[Settings::FIELD___grabMode].NumericByRef().s4;
    config.fb_location = (camera_fb_location_t)frameBufferLocation;
    config.grab_mode = (camera_grab_mode_t)grabMode;

    // validate configuration
    if (config.xclk_freq_hz <= 0)
    {
        result = CameraInitResult_InvalidXclkFrequency;
    }
    else if (config.frame_size < FRAMESIZE_96X96 || config.frame_size >= FRAMESIZE_INVALID)
    {
        result = CameraInitResult_InvalidFrameSize;
    }
    else if (config.jpeg_quality < 0 || config.jpeg_quality > 63)
    {
        result = CameraInitResult_InvalidJpegQuality;
    }
    else if (settings[Settings::FIELD___frameBufferCount].NumericByRef().s4 < 1)
    {
        result = CameraInitResult_InvalidFrameBufferCount;
    }
    else if (frameBufferLocation != FrameBufferLocation_Psram && frameBufferLocation != FrameBufferLocation_Dram)
    {
        result = CameraInitResult_InvalidFrameBufferLocation;
    }
    else if (grabMode != GrabMode_WhenEmpty && grabMode != GrabMode_Latest)
    {
        result = CameraInitResult_InvalidGrabMode;
    }
    else if (
        (config.pin_sccb_sda == -1) != (config.pin_sccb_scl == -1) ||
        (config.pin_sccb_sda == -1 && (config.sccb_i2c_port < 0 || config.sccb_i2c_port >= SOC_I2C_NUM)))
    {
        // either both SCCB pins are set, or both are -1 and an existing I2C port is used
        result = CameraInitResult_InvalidSccbConfiguration;
    }
    else if (config.fb_location == CAMERA_FB_IN_PSRAM && heap_caps_get_total_size(MALLOC_CAP_SPIRAM) == 0)
    {
        result = CameraInitResult_PsramUnavailable;
    }

    if (result != CameraInitResult_Success)
    {
        NANOCLR_SET_AND_LEAVE(S_OK);
    }

    // validate and reserve pins
    // PWDN, RESET, XCLK and SCCB pins are optional outputs, the remaining ones are required inputs
    pins[0] = config.pin_pwdn;
    pins[1] = config.pin_reset;
    pins[2] = config.pin_xclk;
    pins[3] = config.pin_sccb_sda;
    pins[4] = config.pin_sccb_scl;
    pins[5] = config.pin_d7;
    pins[6] = config.pin_d6;
    pins[7] = config.pin_d5;
    pins[8] = config.pin_d4;
    pins[9] = config.pin_d3;
    pins[10] = config.pin_d2;
    pins[11] = config.pin_d1;
    pins[12] = config.pin_d0;
    pins[13] = config.pin_vsync;
    pins[14] = config.pin_href;
    pins[15] = config.pin_pclk;

    for (int index = 0; index < CAMERA_PIN_COUNT; index++)
    {
        bool isOutput = index < 5;

        if (isOutput && pins[index] == -1)
        {
            continue;
        }

        if (!GPIO_IS_VALID_GPIO(pins[index]) || (isOutput && !GPIO_IS_VALID_OUTPUT_GPIO(pins[index])))
        {
            result = CameraInitResult_InvalidPin;
            NANOCLR_SET_AND_LEAVE(S_OK);
        }

        if (!CPU_GPIO_ReservePin(pins[index], true))
        {
            result = CameraInitResult_PinUnavailable;
            NANOCLR_SET_AND_LEAVE(S_OK);
        }

        ReservedPins[ReservedPinCount++] = pins[index];
    }

#if !defined(CONFIG_IDF_TARGET_ESP32S3)
    // ESP32-S3 generates XCLK with LCD_CAM instead of LEDC
    if (config.pin_xclk >= 0)
    {
        if (!Esp32_Ledc_Reserve(LEDC_LOW_SPEED_MODE, CAMERA_LEDC_TIMER, CAMERA_LEDC_CHANNEL, Esp32LedcOwner::Camera))
        {
            result = CameraInitResult_ClockUnavailable;
            NANOCLR_SET_AND_LEAVE(S_OK);
        }

        LedcReserved = true;
    }
#endif

    error = esp_camera_init(&config);

    if (error != ESP_OK)
    {
#if defined(DEBUG)
        ESP_LOGE("Camera", "esp_camera_init failed: %s", esp_err_to_name(error));
#endif

        switch (error)
        {
            case ESP_ERR_CAMERA_NOT_DETECTED:
                result = CameraInitResult_SensorNotDetected;
                break;

            case ESP_ERR_CAMERA_NOT_SUPPORTED:
            case ESP_ERR_NOT_SUPPORTED:
                result = CameraInitResult_SensorNotSupported;
                break;

            case ESP_ERR_CAMERA_FAILED_TO_SET_FRAME_SIZE:
                result = CameraInitResult_FailedToSetFrameSize;
                break;

            case ESP_ERR_CAMERA_FAILED_TO_SET_OUT_FORMAT:
                result = CameraInitResult_FailedToSetPixelFormat;
                break;

            case ESP_ERR_NO_MEM:
                result = CameraInitResult_OutOfMemory;
                break;

            default:
                result = CameraInitResult_DriverError;
                break;
        }

        // esp_camera_init releases whatever it allocated when it fails
        NANOCLR_SET_AND_LEAVE(S_OK);
    }

    driverInitialized = true;

    // create the capture worker task
    CaptureRequest = xSemaphoreCreateBinary();

    if (CaptureRequest == NULL)
    {
        result = CameraInitResult_OutOfMemory;
        NANOCLR_SET_AND_LEAVE(S_OK);
    }

    if (xTaskCreate(
            CameraCaptureWorkerTask,
            "CameraCapture",
            CAMERA_CAPTURE_WORKER_TASK_STACK_SIZE,
            NULL,
            CAMERA_CAPTURE_WORKER_TASK_PRIORITY,
            &CaptureWorkerTaskHandle) != pdPASS)
    {
        CaptureWorkerTaskHandle = NULL;
        result = CameraInitResult_OutOfMemory;
        NANOCLR_SET_AND_LEAVE(S_OK);
    }

    // driver has to be uninitialized on soft reboot
    HAL_AddSoftRebootHandler(Uninitialize);

    NANOCLR_CLEANUP();

    if (result != CameraInitResult_Success && result != CameraInitResult_AlreadyInitialized)
    {
        if (CaptureRequest != NULL)
        {
            vSemaphoreDelete(CaptureRequest);
            CaptureRequest = NULL;
        }

        if (driverInitialized)
        {
            esp_camera_deinit();
        }

        if (LedcReserved)
        {
            Esp32_Ledc_Release(LEDC_LOW_SPEED_MODE, CAMERA_LEDC_CHANNEL, Esp32LedcOwner::Camera);
            LedcReserved = false;
        }

        while (ReservedPinCount > 0)
        {
            CPU_GPIO_ReservePin(ReservedPins[--ReservedPinCount], false);
        }
    }

    if (SUCCEEDED(hr))
    {
        stack.SetResult_I4(result);
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Library_nano_esp32_cam_nanoFramework_Esp32_Camera_Esp32Camera::CaptureExecution(
    CLR_RT_StackFrame &stack,
    camera_fb_t *&frame)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock hbTimeout;
    CLR_INT64 *timeout;
    bool eventResult = true;

    if (CaptureWorkerTaskHandle == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    // setup timeout
    // always set CLR infinite timeout as the driver takes care of the timeout
    hbTimeout.SetInteger((CLR_INT64)-1);
    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeout));

    if (stack.m_customState == 1)
    {
        // request a frame from the worker task
        xSemaphoreGive(CaptureRequest);

        // bump custom state
        stack.m_customState = 2;
    }

    while (eventResult)
    {
        // non-blocking wait allowing other threads to run while we wait for the capture to complete
        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeout, Event_Camera, eventResult));

        if (eventResult)
        {
            // grab the frame, NULL if the driver didn't get one
            frame = CapturedFrame;
            CapturedFrame = NULL;

            break;
        }
    }

    // pop timeout heap block from stack
    stack.PopValue();

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nano_esp32_cam_nanoFramework_Esp32_Camera_Esp32Camera::NativeCapture___SZARRAY_U1(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    camera_fb_t *frame = NULL;

    NANOCLR_CHECK_HRESULT(CaptureExecution(stack, frame));

    if (frame == NULL)
    {
        // driver didn't get a frame, return null
        stack.SetResult_Object(NULL);
        NANOCLR_SET_AND_LEAVE(S_OK);
    }

    {
        // the frame buffer is owned by the driver, so allocating the array (and a possible GC run) doesn't affect it
        CLR_RT_HeapBlock &top = stack.PushValueAndClear();
        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(top, frame->len, g_CLR_RT_WellKnownTypes.m_UInt8));

        memcpy(top.DereferenceArray()->GetFirstElement(), frame->buf, frame->len);
    }

    NANOCLR_CLEANUP();

    if (frame != NULL)
    {
        // done with the frame, return it to the driver
        esp_camera_fb_return(frame);
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Library_nano_esp32_cam_nanoFramework_Esp32_Camera_Esp32Camera::NativeCaptureToBuffer___I4__SZARRAY_U1(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    camera_fb_t *frame = NULL;
    CLR_RT_HeapBlock_Array *buffer;

    NANOCLR_CHECK_HRESULT(CaptureExecution(stack, frame));

    if (frame == NULL)
    {
        // driver didn't get a frame, return 0 bytes written
        stack.SetResult_I4(0);
        NANOCLR_SET_AND_LEAVE(S_OK);
    }

    // get the buffer only now, as the GC may have relocated it while this thread was waiting
    buffer = stack.Arg1().DereferenceArray();
    FAULT_ON_NULL_ARG(buffer);

    if (buffer->m_numOfElements < frame->len)
    {
        // buffer too small for the image
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    memcpy(buffer->GetFirstElement(), frame->buf, frame->len);

    stack.SetResult_I4((CLR_INT32)frame->len);

    NANOCLR_CLEANUP();

    if (frame != NULL)
    {
        // done with the frame, return it to the driver
        esp_camera_fb_return(frame);
    }

    NANOCLR_CLEANUP_END();
}

HRESULT Library_nano_esp32_cam_nanoFramework_Esp32_Camera_Esp32Camera::NativeGetSensorId___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    sensor_t *sensor = esp_camera_sensor_get();

    if (sensor == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    stack.SetResult_I4(sensor->id.PID);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nano_esp32_cam_nanoFramework_Esp32_Camera_Esp32Camera::
    NativeGetSensorSetting___I4__nanoFrameworkEsp32CameraSensorSetting(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    sensor_t *sensor = esp_camera_sensor_get();
    CLR_INT32 value;

    if (sensor == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    switch ((SensorSetting)stack.Arg1().NumericByRef().s4)
    {
        case SensorSetting_FrameSize:
            value = sensor->status.framesize;
            break;

        case SensorSetting_Quality:
            value = sensor->status.quality;
            break;

        case SensorSetting_Brightness:
            value = sensor->status.brightness;
            break;

        case SensorSetting_VFlip:
            value = sensor->status.vflip;
            break;

        case SensorSetting_HMirror:
            value = sensor->status.hmirror;
            break;

        default:
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    stack.SetResult_I4(value);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nano_esp32_cam_nanoFramework_Esp32_Camera_Esp32Camera::
    NativeSetSensorSetting___VOID__nanoFrameworkEsp32CameraSensorSetting__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    sensor_t *sensor = esp_camera_sensor_get();
    CLR_INT32 value = stack.Arg2().NumericByRef().s4;
    int result;

    if (sensor == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    // values are validated in managed code
    switch ((SensorSetting)stack.Arg1().NumericByRef().s4)
    {
        case SensorSetting_FrameSize:
            result = sensor->set_framesize(sensor, (framesize_t)value);
            break;

        case SensorSetting_Quality:
            result = sensor->set_quality(sensor, value);
            break;

        case SensorSetting_Brightness:
            result = sensor->set_brightness(sensor, value);
            break;

        case SensorSetting_VFlip:
            result = sensor->set_vflip(sensor, value);
            break;

        case SensorSetting_HMirror:
            result = sensor->set_hmirror(sensor, value);
            break;

        default:
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    if (result != 0)
    {
        // sensor doesn't support the setting or failed to write it
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nano_esp32_cam_nanoFramework_Esp32_Camera_Esp32Camera::NativeDispose___VOID(CLR_RT_StackFrame &stack)
{
    (void)stack;

    NANOCLR_HEADER();

    Uninitialize();

    NANOCLR_NOCLEANUP_NOLABEL();
}
