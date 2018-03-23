//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _ESP32_OS_H_
#define _ESP32_OS_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "string.h"
#include "esp_system.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "task.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_wpa2.h"
#include "esp_eth.h"
#include "esp_event_loop.h"

#include "spi_master.h"
#include "gpio.h"
#include "i2c.h"
#include "uart.h"   
#include "ledc.h"
#include "adc.h"


#endif // _ESP32_OS_H_