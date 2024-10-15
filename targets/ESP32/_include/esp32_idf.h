//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef ESP32_IDF_H
#define ESP32_IDF_H

#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

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

#include <esp_log.h>

#include <spi_master.h>
#include <gpio.h>
#include <i2c.h>
#include <uart.h>
#include <ledc.h>
#include <adc.h>
#include <dac.h>
#include <timer.h>
#include <esp_spiffs.h>
#include <pcnt.h>

#include <soc/uart_channel.h>
#include <spiffs_config.h>
#include <esp_spiffs.h>
#include <esp_rom_crc.h>
#include <esp_rom_caps.h>

// includes specific for TinyUSB and CDC
#if CONFIG_USB_CDC_ENABLED
#include <tinyusb.h>
#include <tusb_cdc_acm.h>
#endif

// Uncomment to support Ethernet
//#define ESP32_ETHERNET_SUPPORT

#endif // ESP32_IDF_H