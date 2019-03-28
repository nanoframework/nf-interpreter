//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

//
//	Initialise SD card and mount on VFS
//  Supports  MMC and SPI SD cards
//  MMC supports 4bit or 1 bit

// == Boards == 
// OLimex EVB - 1 bit SD/MMC  ( DAT0 - GPIO2, CMD/DI - GPIO15 ), No Card detect, No Write protect
// Wrover V4 -  4 bit SD/MMC  ( Card detect )



#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"

static const char *TAG = "SDCard";


bool Storage_MountSDCard(char * vfsName, sdmmc_slot_config_t* slot_config, bool formatOnMount, int maxFiles )
{
	sdmmc_host_t host = SDMMC_HOST_DEFAULT();

	esp_vfs_fat_sdmmc_mount_config_t mount_config = {
		.format_if_mount_failed = formatOnMount,
		.max_files = maxFiles,
		.allocation_unit_size = 16 * 1024
	};

	sdmmc_card_t* card;
	esp_err_t ret = esp_vfs_fat_sdmmc_mount(vfsName, &host, &slot_config, &mount_config, &card);

	if (ret != ESP_OK) {
		if (ret == ESP_FAIL) {
			ESP_LOGE(TAG, "Failed to mount filesystem. ");
		}
		else {
			ESP_LOGE(TAG, "Failed to initialize the card (%s).  ", esp_err_to_name(ret));
		}
		return false;
	}

	return true;
}

//  Storage_InitSDCardMMC
//	Initial the SD card Slot 1 - 4/1 bit ( hs2_* signals )
//
//  vfsName - Name of disk for vfs - i.e "/SDSPI"
//  maxFiles- Maximum number of open files
//  bit1Mode- true to use 1 bit MMC interface
//
//  return true if OK
//
bool Storage_InitSDCardMMC(char * vfsName, int maxFiles, bool bit1Mode)
{
	ESP_LOGI(TAG, "Initializing MMC SD card");

	// This initializes the slot without card detect (CD) and write protect (WP) signals.
	// Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
	sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

	// GPIOs 15, 2, 4, 12, 13 should have external 10k pull-ups.
	// Internal pull-ups are not sufficient. However, enabling internal pull-ups
	// does make a difference some boards, so we do that here.
	gpio_set_pull_mode(15, GPIO_PULLUP_ONLY);       // CMD, needed in 4- and 1- line modes
	gpio_set_pull_mode(2, GPIO_PULLUP_ONLY);        // D0, needed in 4- and 1-line modes

	// Using 1 bit data ( OLimex EVB etc )
	if (bit1Mode)
	{
		slot_config.width = 1;
	}
	else
	{
		gpio_set_pull_mode(4, GPIO_PULLUP_ONLY);    // D1, needed in 4-line mode only
		gpio_set_pull_mode(12, GPIO_PULLUP_ONLY);   // D2, needed in 4-line mode only
		gpio_set_pull_mode(13, GPIO_PULLUP_ONLY);   // D3, needed in 4-line modes only
	}

	return Storage_MountSDCard(vfsName, &slot_config, false, maxFiles);
}

//  Storage_InitSDCardSPI
//	Initial the SD card connected by SPI)
//
//  vfsName - Name of disk for vfs - i.e "/SDSPI"
//  maxFiles- Maximum number of open files
//
//  return true if OK
//
bool Storage_InitSDCardSPI(char * vfsName, int maxFiles, int pin_Miso, int pin_Mosi, int pin_Clk, int pin_Cs)
{
	ESP_LOGI(TAG, "Initializing SPI SD card");

	sdmmc_host_t host = SDSPI_HOST_DEFAULT();
	sdspi_slot_config_t slot_config = SDSPI_SLOT_CONFIG_DEFAULT();
	slot_config.gpio_miso = pin_Miso;
	slot_config.gpio_mosi = pin_Mosi;
	slot_config.gpio_sck = pin_Clk;
	slot_config.gpio_cs = pin_Cs;

	return Storage_MountSDCard(vfsName, &slot_config, false, maxFiles);
}
