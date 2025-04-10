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
#include <driver/sdmmc_host.h>
#include <driver/sdspi_host.h>
#include <sdmmc_cmd.h>

#include <target_platform.h>

#include <nanoHAL_System_IO_FileSystem.h>
#include <Esp32_DeviceMapping.h>

#if (HAL_USE_SDC == TRUE)

static const char *TAG = "SDCard";

sdmmc_card_t *card;

//
//  Unmount SD card ( MMC/SDIO or SPI)
//
bool Storage_UnMountSDCard(int driveIndex)
{
    char mountPoint[] = INDEX0_DRIVE_LETTER;

    // Change fatfs drive letter to mount point  D: -> /D for ESP32 VFS
    mountPoint[1] = mountPoint[0] + driveIndex;
    mountPoint[0] = '/';

    if (esp_vfs_fat_sdcard_unmount(mountPoint, card) != ESP_OK)
    {
        return false;
    }

    card = NULL;

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

#if SOC_SDMMC_HOST_SUPPORTED
//
// Mount SDcard on MMC/SDIO bus
//
//  bit1Mode- true to use 1 bit MMC interface
//  driveIndex =  0 = first drive, 1 = 2nd drive
//
bool Storage_MountMMC(bool bit1Mode, int driveIndex)
{
    esp_err_t errCode;
    char mountPoint[] = INDEX0_DRIVE_LETTER;

    // Change fatfs drive letter to mount point  D: -> /D for ESP32 VFS or /E
    mountPoint[1] = mountPoint[0] + driveIndex;
    mountPoint[0] = '/';

    ESP_LOGI(TAG, "Initializing SDMMC%d SD card", driveIndex + 1);

    sdmmc_host_t host = SDMMC_HOST_DEFAULT();

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

    // Set bus width and pins to use
#if (defined(CONFIG_IDF_TARGET_ESP32S3) || defined(CONFIG_IDF_TARGET_ESP32P4))
    slot_config.clk = (gpio_num_t)Esp32_GetSDmmcDevicePins_C(driveIndex, Esp32SdmmcPin_Clock);
    slot_config.cmd = (gpio_num_t)Esp32_GetSDmmcDevicePins_C(driveIndex, Esp32SdmmcPin_Command);
    slot_config.d0 = (gpio_num_t)Esp32_GetSDmmcDevicePins_C(driveIndex, Esp32SdmmcPin_D0);
#endif

    if (bit1Mode)
    {
        slot_config.width = 1;
    }
    else
    {
        slot_config.width = 4;
#if (defined(CONFIG_IDF_TARGET_ESP32S3) || defined(CONFIG_IDF_TARGET_ESP32P4))
        slot_config.d1 = (gpio_num_t)Esp32_GetSDmmcDevicePins_C(driveIndex, Esp32SdmmcPin_D1);
        slot_config.d2 = (gpio_num_t)Esp32_GetSDmmcDevicePins_C(driveIndex, Esp32SdmmcPin_D2);
        slot_config.d3 = (gpio_num_t)Esp32_GetSDmmcDevicePins_C(driveIndex, Esp32SdmmcPin_D3);
#endif
    }

    // from IDF readme on SDMMC
    //
    ///////////////////////////////
    // Pin assignments for ESP32 //
    ///////////////////////////////
    // On ESP32, SDMMC peripheral is connected to specific GPIO pins using the IO MUX.
    // GPIO pins cannot be customized. Please see the table below for the pin connections.

    // ESP32 pin     | SD card pin | Notes
    // --------------|-------------|------------
    // GPIO14 (MTMS) | CLK         | 10k pullup in SD mode
    // GPIO15 (MTDO) | CMD         | 10k pullup in SD mode
    // GPIO2         | D0          | 10k pullup in SD mode, pull low to go into download mode
    // GPIO4         | D1          | not used in 1-line SD mode; 10k pullup in 4-line SD mode
    // GPIO12 (MTDI) | D2          | not used in 1-line SD mode; 10k pullup in 4-line SD mode
    // GPIO13 (MTCK) | D3          | not used in 1-line SD mode, but card's D3 pin must have a 10k pullup

    //////////////////////////////////
    // Pin assignments for ESP32-S3 //
    //////////////////////////////////
    // On ESP32-S3, SDMMC peripheral is connected to GPIO pins using GPIO matrix.
    // This allows arbitrary GPIOs to be used to connect an SD card. In this example, GPIOs can be configured in two
    // ways:

    // The table below lists the default pin assignments.

    // ESP32-S3 pin  | SD card pin | Notes
    // --------------|-------------|------------
    // GPIO36        | CLK         | 10k pullup
    // GPIO35        | CMD         | 10k pullup
    // GPIO37        | D0          | 10k pullup
    // GPIO38        | D1          | not used in 1-line SD mode; 10k pullup in 4-line mode
    // GPIO33        | D2          | not used in 1-line SD mode; 10k pullup in 4-line mode
    // GPIO34        | D3          | not used in 1-line SD mode, but card's D3 pin must have a 10k pullup

    // Enable internal pullups on enabled pins. The internal pullups
    // are insufficient however, please make sure 10k external pullups are
    // connected on the bus.
    slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

    //	Mount the SDCard device as a FAT device on the VFS
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = SDC_MAX_OPEN_FILES,
        .allocation_unit_size = 16 * 1024};

    errCode = esp_vfs_fat_sdmmc_mount(mountPoint, &host, &slot_config, &mount_config, &card);
    if (errCode == ESP_ERR_INVALID_STATE)
    {
        // Invalid state means its already mounted, this can happen if you are trying to debug mount from managed code
        // and the code has already run & mounted
        Storage_UnMountSDCard(driveIndex);
        errCode = esp_vfs_fat_sdmmc_mount(mountPoint, &host, &slot_config, &mount_config, &card);
    }

    return LogMountResult(errCode);
}
#endif

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
#if defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32S3) || defined(CONFIG_IDF_TARGET_ESP32C6) ||  \
    defined(CONFIG_IDF_TARGET_ESP32H2)
    // First available bus on ESP32_C3/S3/C6/H2 is SPI2_HOST
    host.slot = spiBus + SPI2_HOST;
#else
    // First available bus on ESP32 is HSPI_HOST(1)
    host.slot = spiBus + HSPI_HOST;
#endif

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024};

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
        Storage_UnMountSDCard(driveIndex);
        errCode = esp_vfs_fat_sdspi_mount(mountPoint, &host, &slot_config, &mount_config, &card);
    }

    return LogMountResult(errCode);
}

#endif
