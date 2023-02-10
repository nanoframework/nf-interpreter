//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "nanoFramework_hardware_esp32_native.h"
#include <nanoCLR_Hardware.h>
#include <targetPAL.h>

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

static int numberInitialzed = 0;
static bool isTouchInitialized = false;
static bool isTouchPadUsed[TOUCH_PAD_MAX];
static bool isFilterOn = false;
static TaskHandle_t xHandle = NULL;
static bool touchValues[TOUCH_PAD_MAX];
static uint32_t thresholds[TOUCH_PAD_MAX];
static bool isTimeModeOn = false;
static uint32_t lastTouchValues[TOUCH_PAD_MAX];
static touch_fsm_mode_t measurementMode;

/*
Function used for all the interruption in the touch pad.
*/
static void IsrCallBack(void *arg)
{
    bool val;
    uint32_t padIntr = touch_pad_get_status();
#if defined(CONFIG_IDF_TARGET_ESP32)
    touch_pad_intr_clear();
#else
    touch_pad_intr_clear(TOUCH_PAD_INTR_MASK_ACTIVE | TOUCH_PAD_INTR_MASK_INACTIVE | TOUCH_PAD_INTR_MASK_TIMEOUT);
#endif
    for (int i = 0; i < TOUCH_PAD_MAX; i++)
    {
        val = (padIntr >> i) & 0x01;
        // Check if we have a change and raise an even if yes
        if (val != touchValues[i])
        {
            PostManagedEvent(EVENT_TOUCH, 0, i, val);
        }

        touchValues[i] = val;
    }
}

/*
Resources need to be cleaned and the driver uninstalled in case of a soft reboot.
*/
void HAL_AddSoftRebootHandler()
{
    // top the task
    isTimeModeOn = false;

    // Clean the isr registration
#if defined(CONFIG_IDF_TARGET_ESP32)
    touch_pad_intr_disable();
#else
    touch_pad_intr_disable(TOUCH_PAD_INTR_MASK_ACTIVE | TOUCH_PAD_INTR_MASK_INACTIVE | TOUCH_PAD_INTR_MASK_TIMEOUT);
#endif
    touch_pad_isr_deregister(IsrCallBack, NULL);
    // Clean filter and uninstall the driver
#if defined(CONFIG_IDF_TARGET_ESP32)
    touch_pad_filter_stop();
    touch_pad_filter_delete();
#else
    touch_pad_filter_disable();
#endif

    // Deinitialize and clean the touch pad driver
    touch_pad_deinit();

    isFilterOn = false;
    isTouchInitialized = false;
    // Make sure all pins are not reserved
    memset(isTouchPadUsed, 0, sizeof(isTouchPadUsed));
    // Clear the pin values
    memset(touchValues, 0, sizeof(touchValues));
    memset(thresholds, 0, sizeof(thresholds));
    memset(lastTouchValues, 0, sizeof(lastTouchValues));
}

/*
This function ensure that the driver is installed for static functions.
It does initialize as well the pins table and register for the soft reboot call back.
*/
void MakeSureTouchIsInitialized()
{
    if (!isTouchInitialized)
    {
        isTouchInitialized = true;
        touch_pad_init();

        // We setup software trigger mode because the FSM one
        // is not properly working on ESP32 and we will manage a read loop
        // in a task.
        touch_pad_set_fsm_mode(TOUCH_FSM_MODE_SW);

        // Make sure all pins are not reserved
        memset(isTouchPadUsed, 0, sizeof(isTouchPadUsed));

        HAL_AddSoftRebootHandler(HAL_AddSoftRebootHandler);
#if defined(CONFIG_IDF_TARGET_ESP32)
        touch_pad_intr_enable();
        touch_pad_isr_register(IsrCallBack, NULL);
#else
        touch_pad_intr_enable(TOUCH_PAD_INTR_MASK_ACTIVE | TOUCH_PAD_INTR_MASK_INACTIVE | TOUCH_PAD_INTR_MASK_TIMEOUT);
        touch_pad_isr_register(IsrCallBack, NULL, TOUCH_PAD_INTR_MASK_ALL);
#endif
    }
}

#endif

HRESULT GetAndCheckTouchPadNumber(CLR_RT_StackFrame &stack, touch_pad_t *padNumber)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

    // Get the stack
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);
    *padNumber =
        (touch_pad_t)(pThis[Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::
                                FIELD___touchPadNumber]
                          .NumericByRef()
                          .s4);

    // Let's make sure it's a proper pin
    if ((*padNumber < 0) || (*padNumber >= TOUCH_PAD_MAX))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::NativeInit___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

    touch_pad_t padNumber;
    NANOCLR_CHECK_HRESULT(GetAndCheckTouchPadNumber(stack, &padNumber));

    // We make sure the pin is not yet open
    if (isTouchPadUsed[padNumber])
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // We initalize the pin
    MakeSureTouchIsInitialized();
    if (touch_pad_io_init(padNumber) != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

#if defined(CONFIG_IDF_TARGET_ESP32)
    if (touch_pad_config(padNumber, 0) != ESP_OK)
#else
    if (touch_pad_config(padNumber) != ESP_OK)
#endif
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    // And we increase the number and mark the pin as open
    numberInitialzed++;
    isTouchPadUsed[padNumber] = true;

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::NativeDeinit___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

    if (numberInitialzed == 1)
    {
        touch_pad_deinit();
        isTouchInitialized = false;
    }

    touch_pad_t padNumber;
    NANOCLR_CHECK_HRESULT(GetAndCheckTouchPadNumber(stack, &padNumber));

    // Decrease the counter and make sure we mark this one as not used anymore
    numberInitialzed--;
    isTouchPadUsed[padNumber] = false;

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif

    NANOCLR_NOCLEANUP();
}

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

uint32_t TouchPadRead(touch_pad_t padNumber)
{
#if defined(CONFIG_IDF_TARGET_ESP32)
    uint16_t touchValue;
#else
    uint32_t touchValue;
#endif
    // Start a manual measurement if software mode
    touch_pad_sw_start();

    // while (!touch_pad_meas_is_done())
    {
        ;
    }

    // If we are filtering, the function to call to read the data is different
    if (isFilterOn)
    {
#if defined(CONFIG_IDF_TARGET_ESP32)
        touch_pad_read_filtered(padNumber, &touchValue);
#else
        touch_pad_filter_read_smooth(padNumber, &touchValue);
#endif
    }
    else
    {
#if defined(CONFIG_IDF_TARGET_ESP32)
        touch_pad_read(padNumber, &touchValue);
#else
        touch_pad_read_raw_data(padNumber, &touchValue);
#endif
    }

    // Do we have to reset the event
    if (touchValue > thresholds[padNumber] && touchValues[padNumber])
    {
        PostManagedEvent(EVENT_TOUCH, 0, padNumber, 0);
        touchValues[padNumber] = false;
    }

    lastTouchValues[padNumber] = touchValue;

    return touchValue;
}

static void ReadTask(void *pvParameter)
{
    while (isTimeModeOn)
    {
        for (int i = 0; i < TOUCH_PAD_MAX; i++)
        {
            if (isTouchPadUsed[i])
            {
                TouchPadRead((touch_pad_t)i);
            }
        }

        // TODO: find the best value for this task. Different samples
        // shows different values from 10 to 200.
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

#endif

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::NativeRead___U4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

    touch_pad_t padNumber;
    uint32_t touchValue;
    NANOCLR_CHECK_HRESULT(GetAndCheckTouchPadNumber(stack, &padNumber));

    // If we already have the background task running
    if (isTimeModeOn)
    {
        touchValue = lastTouchValues[padNumber];
    }
    else
    {
        touchValue = TouchPadRead(padNumber);
    }

    stack.SetResult_U4(touchValue);

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::
    NativeGetThreshold___U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

    touch_pad_t padNumber;
    NANOCLR_CHECK_HRESULT(GetAndCheckTouchPadNumber(stack, &padNumber));

#if defined(CONFIG_IDF_TARGET_ESP32)
    uint16_t threshold;
#else
    uint32_t threshold;
#endif
    if (touch_pad_get_thresh(padNumber, &threshold) != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    // Make sure we store it for later usage
    thresholds[padNumber] = threshold;
    stack.SetResult_U4(threshold);

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::
    NativeSetThreshold___VOID__U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

    touch_pad_t padNumber;
    NANOCLR_CHECK_HRESULT(GetAndCheckTouchPadNumber(stack, &padNumber));

#if defined(CONFIG_IDF_TARGET_ESP32)
    uint16_t threshold;
    // Non static function, Arg1 is the first argument
    threshold = (uint16_t)stack.Arg1().NumericByRef().u4;
#else
    uint32_t threshold;
    // Non static function, Arg1 is the first argument
    threshold = (uint32_t)stack.Arg1().NumericByRef().u4;

#endif
    if (touch_pad_set_thresh(padNumber, threshold) != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    // Make sure we store it for later usage
    thresholds[padNumber] = threshold;

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::
    NativeGetTouchSpeed___VOID__BYREF_I4__BYREF_I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

    touch_pad_t padNumber;
    touch_cnt_slope_t slope;
    touch_tie_opt_t opt;

    NANOCLR_CHECK_HRESULT(GetAndCheckTouchPadNumber(stack, &padNumber));
    if (touch_pad_get_cnt_mode(padNumber, &slope, &opt) != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    // Arg1 as we are in a non static function.
    CLR_RT_HeapBlock bhSlope;
    CLR_RT_HeapBlock bhOpt;
    bhSlope.SetInteger(slope);
    NANOCLR_CHECK_HRESULT(bhSlope.StoreToReference(stack.Arg1(), 0));
    bhOpt.SetInteger(opt);
    NANOCLR_CHECK_HRESULT(bhOpt.StoreToReference(stack.Arg2(), 0));

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::
    NativeSetChargeSpeed___VOID__I4__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

    touch_pad_t padNumber;
    touch_cnt_slope_t slope;
    touch_tie_opt_t opt;

    NANOCLR_CHECK_HRESULT(GetAndCheckTouchPadNumber(stack, &padNumber));
    // Non static function, Arg1 is the first argument
    slope = (touch_cnt_slope_t)stack.Arg1().NumericByRef().u4;
    opt = (touch_tie_opt_t)stack.Arg2().NumericByRef().u4;
    if (touch_pad_set_cnt_mode(padNumber, slope, opt) != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::
    NativeCalibrate___I4__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

    touch_pad_t padNumber;
    int count;
    double average = 0;

    NANOCLR_CHECK_HRESULT(GetAndCheckTouchPadNumber(stack, &padNumber));
    // Non static function, Arg1 is the first argument
    count = (int)stack.Arg1().NumericByRef().s4;

    // We will read the values and make an average
    for (int i = 0; i < count; i++)
    {
        average = (average * i + TouchPadRead(padNumber)) / (i + 1);
    }

    // And we return the average
    stack.SetResult_I4((int)average);

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::
    NativeGetGpioNumberFromTouchNumber___STATIC__I4__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

    int padNumber = (int)(stack.Arg0().NumericByRef().u4);
    int gpioNumber = -1;

    // Let's make sure it's a proper pin
    if ((padNumber < 0) || (padNumber >= TOUCH_PAD_MAX))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    /*!< Touch pad channel 0 is GPIO4(ESP32) */
    /*!< Touch pad channel 1 is GPIO0(ESP32) / GPIO1(ESP32-S2) */
    /*!< Touch pad channel 2 is GPIO2(ESP32) / GPIO2(ESP32-S2) */
    /*!< Touch pad channel 3 is GPIO15(ESP32) / GPIO3(ESP32-S2) */
    /*!< Touch pad channel 4 is GPIO13(ESP32) / GPIO4(ESP32-S2) */
    /*!< Touch pad channel 5 is GPIO12(ESP32) / GPIO5(ESP32-S2) */
    /*!< Touch pad channel 6 is GPIO14(ESP32) / GPIO6(ESP32-S2) */
    /*!< Touch pad channel 7 is GPIO27(ESP32) / GPIO7(ESP32-S2) */
    /*!< Touch pad channel 8 is GPIO33(ESP32) / GPIO8(ESP32-S2) */
    /*!< Touch pad channel 9 is GPIO32(ESP32) / GPIO9(ESP32-S2) */
    /*!< Touch channel 10 is GPIO10(ESP32-S2) */
    /*!< Touch channel 11 is GPIO11(ESP32-S2) */
    /*!< Touch channel 12 is GPIO12(ESP32-S2) */
    /*!< Touch channel 13 is GPIO13(ESP32-S2) */
    /*!< Touch channel 14 is GPIO14(ESP32-S2) */

    // This is the ESP32-S2 series
    if (SOC_TOUCH_SENSOR_NUM > 10)
    {
        // In this case it's simple
        if (padNumber > 0)
        {
            gpioNumber = padNumber;
        }
    }
    else
    {
        // This is traditional ESP32
        switch (padNumber)
        {
            case TOUCH_PAD_NUM0:
                gpioNumber = 4;
                break;

            case TOUCH_PAD_NUM1:
                gpioNumber = 0;
                break;

            case TOUCH_PAD_NUM2:
                gpioNumber = 2;
                break;

            case TOUCH_PAD_NUM3:
                gpioNumber = 15;
                break;

            case TOUCH_PAD_NUM4:
                gpioNumber = 13;
                break;

            case TOUCH_PAD_NUM5:
                gpioNumber = 12;
                break;

            case TOUCH_PAD_NUM6:
                gpioNumber = 14;
                break;

            case TOUCH_PAD_NUM7:
                gpioNumber = 27;
                break;

            case TOUCH_PAD_NUM8:
                gpioNumber = 33;
                break;

            case TOUCH_PAD_NUM9:
                gpioNumber = 32;
                break;
        }
    }

    stack.SetResult_I4(gpioNumber);

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::
    NativeGetTriggerMode___STATIC__nanoFrameworkHardwareEsp32TouchTouchTriggerMode(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) // Not implemented in this IDF version on the S2/S3 ||
                                     // defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

    // For static functions, we need to make sure that we have the touch pad initialized
    MakeSureTouchIsInitialized();

    touch_trigger_mode_t triggerMode;
    if (touch_pad_get_trigger_mode(&triggerMode) != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    stack.SetResult_I4(triggerMode);

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::
    NativeSetTriggerMode___STATIC__VOID__nanoFrameworkHardwareEsp32TouchTouchTriggerMode(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) // Not implemented in this IDF version on the S2/S3 ||
                                     // defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

    // For static functions, we need to make sure that we have the touch pad initialized
    MakeSureTouchIsInitialized();
    touch_trigger_mode_t triggerMode = (touch_trigger_mode_t)stack.Arg0().NumericByRef().s4;
    if (touch_pad_set_trigger_mode(triggerMode) != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::
    NativeGetTriggerSource___STATIC__nanoFrameworkHardwareEsp32TouchWakeUpSource(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) // Not implemented in this IDF version on the S2/S3 ||
                                     // defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

    // For static functions, we need to make sure that we have the touch pad initialized
    MakeSureTouchIsInitialized();
    touch_trigger_src_t triggerSource;

    if (touch_pad_get_trigger_source(&triggerSource) != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    stack.SetResult_I4(triggerSource);

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::
    NativeSetTriggerSource___STATIC__VOID__nanoFrameworkHardwareEsp32TouchWakeUpSource(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) // Not implemented in this IDF version on the S2/S3 ||
                                     // defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

    // For static functions, we need to make sure that we have the touch pad initialized
    MakeSureTouchIsInitialized();
    touch_trigger_src_t triggerSource = (touch_trigger_src_t)stack.Arg0().NumericByRef().s4;
    if (touch_pad_set_trigger_source(triggerSource) != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::
    NativeGetMeasurementMode___STATIC__nanoFrameworkHardwareEsp32TouchMeasurementMode(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

    // For static functions, we need to make sure that we have the touch pad initialized
    MakeSureTouchIsInitialized();

    stack.SetResult_I4(measurementMode);

    NANOCLR_NOCLEANUP_NOLABEL();

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
    NANOCLR_NOCLEANUP();
#endif
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::
    NativeSetMeasurementMode___STATIC__VOID__nanoFrameworkHardwareEsp32TouchMeasurementMode(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

    // For static functions, we need to make sure that we have the touch pad initialized
    MakeSureTouchIsInitialized();
    measurementMode = (touch_fsm_mode_t)stack.Arg0().NumericByRef().s4;

    // TODO: once we'll use IDF 5.0 or a version where the FSM start is present
    // This section will have to be adjusted.

    // Do we need to stop the task?
    if (measurementMode == TOUCH_FSM_MODE_SW)
    {
        isTimeModeOn = false;
        // If we just switch mode, we have to wait for the task to exit
        // So killing it
        if (xHandle != NULL)
        {
            vTaskDelete(xHandle);
        }
    }

    // As the ESP32 in this current version of IDF does not have a task to start the measurement,
    // we do it in a task and don't change the mode.
    if (measurementMode == TOUCH_FSM_MODE_TIMER)
    {
        if (!isTimeModeOn)
        {
            isTimeModeOn = true;
            // Start a task to show what pads have been touched
            xTaskCreate(&ReadTask, "ReadTask", 4096, NULL, 5, &xHandle);
        }
    }

    NANOCLR_NOCLEANUP_NOLABEL();

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
    NANOCLR_NOCLEANUP();
#endif
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::
    NativeSetVoltage___STATIC__VOID__nanoFrameworkHardwareEsp32TouchTouchHighVoltage__nanoFrameworkHardwareEsp32TouchTouchLowVoltage__nanoFrameworkHardwareEsp32TouchTouchHighVoltageAttenuation(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

    // For static functions, we need to make sure that we have the touch pad initialized
    MakeSureTouchIsInitialized();
    touch_high_volt_t refh = (touch_high_volt_t)stack.Arg0().NumericByRef().s4;
    touch_low_volt_t refl = (touch_low_volt_t)stack.Arg1().NumericByRef().s4;
    touch_volt_atten_t atten = (touch_volt_atten_t)stack.Arg2().NumericByRef().s4;

    if (touch_pad_set_voltage(refh, refl, atten) != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::
    NativeGetVoltage___STATIC__VOID__BYREF_nanoFrameworkHardwareEsp32TouchTouchHighVoltage__BYREF_nanoFrameworkHardwareEsp32TouchTouchLowVoltage__BYREF_nanoFrameworkHardwareEsp32TouchTouchHighVoltageAttenuation(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

    touch_high_volt_t refh;
    touch_low_volt_t refl;
    touch_volt_atten_t atten;

    // Get the voltage
    if (touch_pad_get_voltage(&refh, &refl, &atten) != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    CLR_RT_HeapBlock bhRefh;
    CLR_RT_HeapBlock bhRefl;
    CLR_RT_HeapBlock bhAtten;
    bhRefh.SetInteger(refh);
    NANOCLR_CHECK_HRESULT(bhRefh.StoreToReference(stack.Arg0(), 0));
    bhRefl.SetInteger(refl);
    NANOCLR_CHECK_HRESULT(bhRefl.StoreToReference(stack.Arg1(), 0));
    bhAtten.SetInteger(atten);
    NANOCLR_CHECK_HRESULT(bhAtten.StoreToReference(stack.Arg2(), 0));

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::
    NativeStartFilter___STATIC__VOID__nanoFrameworkHardwareEsp32TouchIFilterSetting(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32)

    // Static function, argument 0 is the first argument
    CLR_RT_HeapBlock *bhPeriodeSetting;
    uint32_t period;
    int type;
    esp_err_t err;

    bhPeriodeSetting = stack.Arg0().Dereference();
    // sanity check for the type of arguments
    type = (int)bhPeriodeSetting
               [Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_Esp32FilterSetting::
                    FIELD___type]
                   .NumericByRef()
                   .u4;
    if (type != (int)IFilterSetting_FilterType_Esp32)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    period = (uint32_t)bhPeriodeSetting
                 [Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_Esp32FilterSetting::
                      FIELD___period]
                     .NumericByRef()
                     .u4;

    err = touch_pad_filter_start(period);
    if (err != ESP_OK)
    {
        if (err == ESP_ERR_INVALID_ARG)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
        }
    }

    isFilterOn = true;

#elif defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)
    CLR_RT_HeapBlock *bhPeriodeSetting;
    touch_filter_config_t filterConfig;
    int type;
    esp_err_t err;

    bhPeriodeSetting = stack.Arg0().Dereference();
    // sanity check for the type of arguments
    type = (int)bhPeriodeSetting
               [Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_S2S3FilterSetting::
                    FIELD___type]
                   .NumericByRef()
                   .u4;
    if (type != (int)IFilterSetting_FilterType_S2_S3)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    filterConfig.mode =
        (touch_filter_mode_t)bhPeriodeSetting
            [Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_S2S3FilterSetting::
                 FIELD___periodeSettingMode]
                .NumericByRef()
                .s4;
    filterConfig.debounce_cnt =
        (uint32_t)bhPeriodeSetting
            [Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_S2S3FilterSetting::
                 FIELD___filterSettingDebounce]
                .NumericByRef()
                .s4;
    filterConfig.noise_thr =
        (uint32_t)bhPeriodeSetting
            [Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_S2S3FilterSetting::
                 FIELD___filterSettingNoiseThreshold]
                .NumericByRef()
                .s4;
    filterConfig.jitter_step =
        (uint32_t)bhPeriodeSetting
            [Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_S2S3FilterSetting::
                 FIELD___jitterSize]
                .NumericByRef()
                .u1;
    filterConfig.smh_lvl =
        (touch_smooth_mode_t)bhPeriodeSetting
            [Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_S2S3FilterSetting::
                 FIELD___filterSettingSmoothMode]
                .NumericByRef()
                .s4;

    err = touch_pad_filter_set_config(filterConfig);
    if (err != ESP_OK)
    {
        if (err == ESP_ERR_INVALID_ARG)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
        }
    }

    if (touch_pad_filter_enable() != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    isFilterOn = true;
#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::
    NativeStopFilter___STATIC__VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

#if defined(CONFIG_IDF_TARGET_ESP32)
    touch_pad_filter_stop();
#else
    touch_pad_filter_disable();
#endif

    isFilterOn = false;
    NANOCLR_NOCLEANUP_NOLABEL();

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
    NANOCLR_NOCLEANUP();
#endif
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::
    NativeSetMeasurementTime___STATIC__VOID__U2__U2(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

    // Static function, argument 0 is the first argument
    uint16_t sleep = (uint32_t)stack.Arg0().NumericByRef().u2;
    uint16_t meas = (uint32_t)stack.Arg1().NumericByRef().u2;

    if (touch_pad_set_meas_time(sleep, meas) != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_Touch_TouchPad::
    NativeGetMeasurementTime___STATIC__VOID__BYREF_U2__BYREF_U2(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

    // Static function, argument 0 is the first argument
    uint16_t sleep;
    uint16_t meas;

    if (touch_pad_get_meas_time(&sleep, &meas) != ESP_OK)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    // Arg0 as we are in a non static function.
    CLR_RT_HeapBlock bhSleep;
    CLR_RT_HeapBlock bhMeas;
    bhSleep.SetInteger(sleep);
    NANOCLR_CHECK_HRESULT(bhSleep.StoreToReference(stack.Arg0(), 0));
    bhMeas.SetInteger(meas);
    NANOCLR_CHECK_HRESULT(bhMeas.StoreToReference(stack.Arg1(), 0));

#else
    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
#endif

    NANOCLR_NOCLEANUP();
}
