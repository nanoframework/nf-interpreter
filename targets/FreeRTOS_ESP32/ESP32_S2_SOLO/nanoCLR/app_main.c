//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <esp32_os.h>

#include <targetHAL.h>
#include <nanoCLR_Application.h>
#include <target_os.h>
#include <WireProtocol_ReceiverThread.h>
#include <LaunchCLR.h>
#include <string.h>

// extern void CLRStartupThread(void const *argument);

// // Mutex for GLOBAL_LOCK / GLOBAL_UNLOCK
// portMUX_TYPE globalLockMutex = portMUX_INITIALIZER_UNLOCKED;

// void receiver_task(void *pvParameter)
// {
//     (void)pvParameter;

//     ReceiverThread(0);

//     vTaskDelete(NULL);
// }

// // Main task start point
// void main_task(void *pvParameter)
// {
//     (void)pvParameter;

//     // CLR settings to launch CLR thread
//     CLR_SETTINGS clrSettings;
//     (void)memset(&clrSettings, 0, sizeof(CLR_SETTINGS));

//     clrSettings.MaxContextSwitches = 50;
//     clrSettings.WaitForDebugger = false;
//     clrSettings.EnterDebuggerLoopAfterExit = true;

//     CLRStartupThread(&clrSettings);

//     vTaskDelete(NULL);
// }

// // Dummy defauly log method to stop output from ESP32 IDF
// int dummyLog ( const char * format, va_list arg )
// {
//     (void)format;
//     (void)arg;
//     return 1;
// }

// // App_main
// // Called from Esp32 IDF start up code before scheduler starts
// //
// void app_main()
// {
//     // Switch off logging so as not to interfere with WireProtocol over Uart0
//     esp_log_level_set("*", ESP_LOG_NONE);

//     // Stop any logging being directed to VS connection, was an issue with Nimble, outputting on Uart0
//     // TODO : redirect these to debugger controlled from nanoframework.Hardware.Esp32
//     esp_log_set_vprintf(dummyLog); 

//     ESP_ERROR_CHECK(nvs_flash_init());

//     xTaskCreatePinnedToCore(&receiver_task, "ReceiverThread", 2048, NULL, 5, NULL, 1);

//     // Start the main task pinned to 2nd core
//     xTaskCreatePinnedToCore(&main_task, "main_task", 15000, NULL, 5, NULL, 1);
// }

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

void app_main(void)
{
    printf("Hello world!\n");

    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    for (int i = 10; i >= 0; i--) {
        printf("Restarting in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}
