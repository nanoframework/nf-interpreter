//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <esp_log.h>
#include <target_os.h>

static const char *TAG = "boot";

// wrapper for IDF bootloader_print_banner to tweak the banner
void __wrap_bootloader_print_banner(void)
{
    ESP_LOGI(TAG, ".NET nanoFramework 2nd stage bootloader ESP-IDF %s", IDF_VER_FIXED);
    ESP_LOGI(TAG, "build " __DATE__ " " __TIME__);
}
