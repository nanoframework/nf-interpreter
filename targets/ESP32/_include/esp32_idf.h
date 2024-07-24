//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef ESP32_IDF_H
#define ESP32_IDF_H

#include <nanoCLR_Headers.h>
#include <target_platform.h>

#include <sys/time.h>
#include <time.h>

#if defined(CONFIG_IDF_TARGET_ESP32C3)
#include <stddef.h>
#endif

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>
#include <freertos/event_groups.h>
#include <esp_system.h>
#include <esp_attr.h>
#include <nvs_flash.h>
#include <sdkconfig.h>

// network
#include <esp_wifi.h>
#include <esp_eth.h>
#include <esp_event.h>
#include <esp_netif_lwip_internal.h>

#include <esp_timer.h>
#include <esp_sleep.h>

// need this hack here, otherwise can't call ESP_LOGxx from cpp code
#ifndef CONFIG_LOG_TIMESTAMP_SOURCE_RTOS
#define CONFIG_LOG_TIMESTAMP_SOURCE_RTOS 1
#endif

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"
#endif

#include <esp_log.h>

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"
#endif

#include <driver/spi_master.h>
#include <driver/gpio.h>
#include <driver/i2c.h>

#include <driver/i2s.h>
#include <soc/i2s_reg.h>
// #include <driver/i2s_std.h>
// #include <driver/i2s_pdm.h>

#include <driver/uart.h>
#include <driver/ledc.h>

#include <driver/adc.h>
// #include <esp_adc/adc_oneshot.h>

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2)
#include <driver/dac_oneshot.h>
#endif

#include <driver/gptimer.h>
#include <driver/pulse_cnt.h>

#include <soc/uart_channel.h>
#include <esp_rom_crc.h>
#include <esp_rom_caps.h>
#include <esp_pm.h>

// Touch pad supported only on those platforms
#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)
#include <driver/touch_sensor.h>
#include <driver/touch_pad.h>
#endif

#if HAL_USE_THREAD == TRUE
#include "esp_openthread.h"
#include "esp_openthread_netif_glue.h"
#include "esp_vfs_eventfd.h"
#endif

#if CONFIG_MBEDTLS_DEBUG
#include <mbedtls/esp_debug.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    int ets_printf(const char *fmt, ...);

#ifdef __cplusplus
}

#endif

// Uncomment to support Ethernet
// #define ESP32_ETHERNET_SUPPORT

#endif // ESP32_IDF_H
