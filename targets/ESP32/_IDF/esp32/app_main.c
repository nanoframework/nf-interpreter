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

// Dummy defauly log method to stop output from ESP32 IDF
int dummyLog(const char *format, va_list arg)
{
    (void)format;
    (void)arg;
    return 1;
}

// App_main
// Called from Esp32 IDF start up code before scheduler starts
void app_main()
{
    // Switch off logging so as not to interfere with WireProtocol over Uart0
    esp_log_level_set("*", ESP_LOG_NONE);

    // Stop any logging being directed to VS connection, was an issue with Nimble, outputting on Uart0
    // TODO : redirect these to debugger controlled from nanoframework.Hardware.Esp32
    esp_log_set_vprintf(dummyLog);

    ESP_ERROR_CHECK(nvs_flash_init());

    // start receiver task pinned to core 1
    xTaskCreatePinnedToCore(&receiver_task, "ReceiverThread", 3072, NULL, 5, NULL, 1);

    // start the CLR main task pinned to core 0
    xTaskCreatePinnedToCore(&main_task, "main_task", 15000, NULL, 5, NULL, 0);
}
