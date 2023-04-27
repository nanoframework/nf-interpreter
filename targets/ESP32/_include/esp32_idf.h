//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef ESP32_IDF_H
#define ESP32_IDF_H

#include <nanoCLR_Headers.h>

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

#include <spi_master.h>
#include <gpio.h>
#include <i2c.h>
#include <i2s.h>
#include <uart.h>
#include <ledc.h>
#include <adc.h>
#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2)
#include <dac.h>
#endif
#include <timer.h>
#include <esp_spiffs.h>
#include <pcnt.h>

#include <soc/uart_channel.h>
#include <spiffs_config.h>
#include <esp_spiffs.h>
#include <esp_rom_crc.h>
#include <esp_rom_caps.h>

// Touch pad supported only on those platforms
#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)
#include <touch_sensor.h>
#include <touch_pad.h>
#endif

// includes specific for TinyUSB and CDC
#if CONFIG_USB_CDC_ENABLED
#include <tinyusb.h>
#include <tusb_cdc_acm.h>
#endif

#if CONFIG_MBEDTLS_DEBUG
#include <mbedtls/esp_debug.h>
#endif

// Uncomment to support Ethernet
// #define ESP32_ETHERNET_SUPPORT

#endif // ESP32_IDF_H