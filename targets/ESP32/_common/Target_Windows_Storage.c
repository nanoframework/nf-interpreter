//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

//
//	Initialise SD card and mount on VFS
//  Supports  MMC and SPI SD cards
//  MMC supports 4bit or 1 bit

// == Boards ==
// OLimex EVB - 1 bit SD/MMC  ( DAT0 - GPIO2, CMD/DI - GPIO15 ), No Card detect, No Write protect
// Wrover V4 -  4 bit SD/MMC  ( Card detect )
//              D0 = 2, D1 = 4, D2 = 12, D3 = 13
//              CLK = 14, CMD = 15, DETECT = 21

//  SPI test
//  MISO = 2, MOSI = 15, CLK = 14, CS = 15

//   5    	VSPICS0 (cs)
//	18		VSPICLK (clockPin) *
//  19      VSPIQ   (miso) *
//  21		VSPIHD  (HD)
//	22		VSPIWP  (WP)
//	23		VSPID   (mosi) *

#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <esp32_idf.h>
#include <esp_vfs_fat.h>
#include <sdmmc_host.h>
#include <sdspi_host.h>
#include <include/sdmmc_cmd.h>

#include <target_platform.h>

#include <Target_Windows_Storage.h>
#include <nanoHAL_Windows_Storage.h>

static const char *TAG = "SDCard";

#if (HAL_USE_SDC == TRUE)

//
//  Unmount SD card ( MMC/SDIO or SPI)
//
bool Storage_UnMountSDCard()
{
    if (esp_vfs_fat_sdmmc_unmount() != ESP_OK)
    {
        return false;
    }
    return true;
}

bool LogMountResult(esp_err_t errCode)
{
    if (errCode != ESP_OK)
    {
        if (errCode == ESP_FAIL)
        {
            ESP_LOGE(TAG, "Failed to mount filesystem. ");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to initialize the card (%s).  ", esp_err_to_name(errCode));
        }
        return false;
    }
    return true;
}
//
// Mount SDcard on MMC/SDIO bus
//
//  bit1Mode- true to use 1 bit MMC interface
//  driveIndex =  0 = first drive
//
bool Storage_MountMMC(bool bit1Mode, int driveIndex)
{
    esp_err_t errCode;
    char mountPoint[] = INDEX0_DRIVE_LETTER;

    // Change fatfs drive letter to mount point  D: -> /D for ESP32 VFS
    mountPoint[1] = mountPoint[0] + driveIndex;
    mountPoint[0] = '/';

    ESP_LOGI(TAG, "Initializing MMC SD card");

    sdmmc_host_t host = SDMMC_HOST_DEFAULT();

    // Using 1 bit data ( OLimex EVB etc )
    if (bit1Mode)
    {
        host.flags = SDMMC_HOST_FLAG_1BIT;
        host.max_freq_khz = SDMMC_FREQ_PROBING;
    }

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

    // GPIOs 15, 2, 4, 12, 13 should have external 10k pull-ups.
    // Internal pull-ups are not sufficient. However, enabling internal pull-ups
    // does make a difference some boards, so we do that here.
    gpio_set_pull_mode((gpio_num_t)15, GPIO_PULLUP_ONLY); // CMD, needed in 4- and 1- line modes
    gpio_set_pull_mode((gpio_num_t)2, GPIO_PULLUP_ONLY);  // D0, needed in 4- and 1-line modes

    if (!bit1Mode)
    {
        gpio_set_pull_mode((gpio_num_t)4, GPIO_PULLUP_ONLY);  // D1, needed in 4-line mode only
        gpio_set_pull_mode((gpio_num_t)12, GPIO_PULLUP_ONLY); // D2, needed in 4-line mode only
        gpio_set_pull_mode((gpio_num_t)13, GPIO_PULLUP_ONLY); // D3, needed in 4-line modes only
    }

    //	Mount the SDCard device as a FAT device on the VFS
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = SDC_MAX_OPEN_FILES,
        .allocation_unit_size = 16 * 1024};

    sdmmc_card_t *card;
    errCode = esp_vfs_fat_sdmmc_mount(mountPoint, &host, &slot_config, &mount_config, &card);
    if (errCode == ESP_ERR_INVALID_STATE)
    {
        // Invalid state means its already mounted, this can happen if you are trying to debug mount from managed code
        // and the code has already run & mounted
        Storage_UnMountSDCard();
        errCode = esp_vfs_fat_sdmmc_mount(mountPoint, &host, &slot_config, &mount_config, &card);
    }

    return LogMountResult(errCode);
}

//
// Mount card on SPI bus
// Expects SPI bus to be already initialised
//
//  spiBus     -  SPI bus index (0 based)
//  csPin      - Chip select pin for SD card
//  driveIndex - 0 = first drive
//
//  return true if OK
//
bool Storage_MountSpi(int spiBus, uint32_t csPin, int driveIndex)
{
    esp_err_t errCode;
    char mountPoint[] = INDEX0_DRIVE_LETTER;

    // Change fatfs drive letter to mount point  D: -> /D for ESP32 VFS
    mountPoint[1] = mountPoint[0] + driveIndex;
    mountPoint[0] = '/';

    ESP_LOGI(TAG, "Initializing SPI SD card");

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.slot = spiBus + HSPI_HOST;

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024};

    sdmmc_card_t *card;

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = (gpio_num_t)csPin;
    slot_config.host_id = (spi_host_device_t)host.slot;

    errCode = esp_vfs_fat_sdspi_mount(mountPoint, &host, &slot_config, &mount_config, &card);
    if (errCode == ESP_ERR_INVALID_STATE)
    {
        // Invalid state means its already mounted, this can happen if you are trying to debug mount from managed code
        // and the code has already run & mounted
        Storage_UnMountSDCard();
        errCode = esp_vfs_fat_sdspi_mount(mountPoint, &host, &slot_config, &mount_config, &card);
    }

    return LogMountResult(errCode);
}

#endif
