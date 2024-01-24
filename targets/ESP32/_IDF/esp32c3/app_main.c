//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <targetHAL.h>
#include <nanoCLR_Application.h>
#include <target_os.h>
#include <WireProtocol_ReceiverThread.h>
#include <LaunchCLR.h>
#include <string.h>

extern void CLRStartupThread(void const *argument);
TaskHandle_t ReceiverTask;

void receiver_task(void *pvParameter)
{
    (void)pvParameter;

    ReceiverThread(0);

    vTaskDelete(NULL);
}

// Main task start point
void main_task(void *pvParameter)
{
    (void)pvParameter;

    // CLR settings to launch CLR thread
    CLR_SETTINGS clrSettings;
    (void)memset(&clrSettings, 0, sizeof(CLR_SETTINGS));

    clrSettings.MaxContextSwitches = 50;
    clrSettings.WaitForDebugger = false;
    clrSettings.EnterDebuggerLoopAfterExit = true;

    CLRStartupThread(&clrSettings);

    vTaskDelete(NULL);
}

// App_main
// Called from Esp32 IDF start up code before scheduler starts
void app_main()
{
    UBaseType_t taskPriority = 5;

    // Switch off logging so as not to interfere with WireProtocol over Uart0
    esp_log_level_set("*", ESP_LOG_NONE);

    ESP_ERROR_CHECK(nvs_flash_init());

    vTaskPrioritySet(NULL, taskPriority);

    // start receiver task
    xTaskCreate(&receiver_task, "ReceiverThread", 3072, NULL, taskPriority, &ReceiverTask);

    // start the CLR main task
    xTaskCreate(&main_task, "main_task", 15000, NULL, taskPriority, NULL);
}
