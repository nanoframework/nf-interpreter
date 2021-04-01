//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#include "ff.h"
#include <nanoHAL_Windows_Storage.h>
#include <Target_Windows_Storage.h>
#include <nanoHAL_v2.h>

#include "fsl_sd.h"


// need to declare this here as extern
extern void PostManagedEvent(uint8_t category, uint8_t subCategory, uint16_t data1, uint32_t data2);

#define SD_CARD_DRIVE_INDEX             "0"
#define SD_CARD_DRIVE_INDEX_NUMERIC     (0)


///////////////////////////////////////////
// code specific to SD Card

// FS for SD Card mounted and ready
bool sdCardFileSystemReady;

static FATFS sdCard_FS;

/*!
* call back function for SD card detect.
*
* @param isInserted  true,  indicate the card is insert.
*                    false, indicate the card is remove.
* @param userData
*/
static void SDCARD_DetectCallBack(bool isInserted, void *userData);

/*! Card descriptor. */
sd_card_t g_sd;

/*! SDMMC host detect card configuration */
static const sdmmchost_detect_card_t s_sdCardDetect = {
#ifndef BOARD_SD_DETECT_TYPE
    .cdType = kSDMMCHOST_DetectCardByGpioCD,
#else
    .cdType = BOARD_SD_DETECT_TYPE,
#endif
    .cdTimeOut_ms = (~0U),
    .cardInserted = SDCARD_DetectCallBack,
    .cardRemoved = SDCARD_DetectCallBack,
};

/*! SD card detect flag  */
static bool s_cardInserted = false;
/*! Card semaphore  */
static SemaphoreHandle_t s_CardDetectSemaphore = NULL;

static void SDCARD_DetectCallBack(bool isInserted, void *userData)
{
    (void)userData;
    s_cardInserted = isInserted;
    xSemaphoreGiveFromISR(s_CardDetectSemaphore, NULL);
}

static void CardDetectTask(void *pvParameters)
{
    (void)pvParameters;

    while (true)
    {
        if (!s_cardInserted)
        {
            /* power off card */
            SD_PowerOffCard(g_sd.host.base, g_sd.usrParam.pwr);
        }

        /* take card detect semaphore */
        xSemaphoreTake(s_CardDetectSemaphore, portMAX_DELAY);
        vTaskDelay(100 / portTICK_PERIOD_MS);
        if (s_cardInserted)
        {
            //Card inserted.
            /* power on the card */
            SD_PowerOnCard(g_sd.host.base, g_sd.usrParam.pwr);
            /* Init card. */
            if (SD_CardInit(&g_sd))
            {
                //SD card init failed.
                continue;
            }

            FRESULT err = f_mount(&sdCard_FS, "0", 1U);
            if (err != FR_OK) {
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                SD_CardDeinit(&g_sd);
                continue;
            }
            sdCardFileSystemReady = true;

            // post event to managed app
            PostManagedEvent( EVENT_STORAGE, 0, StorageEventType_RemovableDeviceInsertion, SD_CARD_DRIVE_INDEX_NUMERIC );
        }
        else
        {
            SD_CardDeinit(&g_sd);

            //Card removed
            sdCardFileSystemReady = false;

            // post event to managed app
            PostManagedEvent( EVENT_STORAGE, 0, StorageEventType_RemovableDeviceRemoval, SD_CARD_DRIVE_INDEX_NUMERIC );
        }
    }
}

void SdCardThread(void * argument)
{
    (void)argument;
    
    s_CardDetectSemaphore = xSemaphoreCreateBinary();

    sdCardFileSystemReady = false;

    g_sd.host.base = SD_HOST_BASEADDR;
    g_sd.host.sourceClock_Hz = SD_HOST_CLK_FREQ;
    g_sd.usrParam.cd = &s_sdCardDetect;

    NVIC_SetPriority(SD_HOST_IRQ, 5U);

    /* SD host init function */
    if (SD_HostInit(&g_sd) != kStatus_Success)
    {
        //SD host init fail
        vTaskDelete(NULL);
        return;
    }

    xTaskCreate(CardDetectTask, "CardDetectTask", configMINIMAL_STACK_SIZE + 500, NULL, configMAX_PRIORITIES - 2, NULL);

    vTaskDelete(NULL);
}


///////////////////////////////////////////

