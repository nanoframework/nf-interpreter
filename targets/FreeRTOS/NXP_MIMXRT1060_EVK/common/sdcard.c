
//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#include "fsl_sd.h"
#include "fsl_debug_console.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/


/*******************************************************************************
 * Prototypes
 ******************************************************************************/

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
static SemaphoreHandle_t s_CardAccessSemaphore = NULL;
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
        if (s_cardInserted)
        {
            //PRINTF("\r\nCard inserted.\r\n");
            /* power on the card */
            SD_PowerOnCard(g_sd.host.base, g_sd.usrParam.pwr);
            /* Init card. */
            if (SD_CardInit(&g_sd))
            {
                //PRINTF("\r\nSD card init failed.\r\n");
                return;
            }
            xSemaphoreGive(s_CardAccessSemaphore);
        }
        else
        {
            //PRINTF("\r\nPlease insert a card into board.\r\n");
        }
    }
}

#include <stdio.h>

/*! @brief Data block count accessed in card */
#define DATA_BLOCK_COUNT (5U)
/*! @brief Start data block number accessed in card */
#define DATA_BLOCK_START (2U)
/*! @brief Data buffer size. */
#define DATA_BUFFER_SIZE (FSL_SDMMC_DEFAULT_BLOCK_SIZE * DATA_BLOCK_COUNT)

/*! @brief Data written to the card */
SDK_ALIGN(uint8_t g_dataWrite[SDK_SIZEALIGN(DATA_BUFFER_SIZE, SDMMC_DATA_BUFFER_ALIGN_CACHE)],
          MAX(SDMMC_DATA_BUFFER_ALIGN_CACHE, SDMMCHOST_DMA_BUFFER_ADDR_ALIGN));
/*! @brief Data read from the card */
SDK_ALIGN(uint8_t g_dataRead[SDK_SIZEALIGN(DATA_BUFFER_SIZE, SDMMC_DATA_BUFFER_ALIGN_CACHE)],
          MAX(SDMMC_DATA_BUFFER_ALIGN_CACHE, SDMMCHOST_DMA_BUFFER_ADDR_ALIGN));

static status_t SDCARD_ReadWrite(sd_card_t *card, bool isReadOnly)
{
    if (isReadOnly)
    {
        //PRINTF("\r\nRead one data block......\r\n");
        if (kStatus_Success != SD_ReadBlocks(card, g_dataRead, DATA_BLOCK_START, 1U))
        {
            //PRINTF("Read one data block failed.\r\n");
            return kStatus_Fail;
        }

        //PRINTF("Read multiple data blocks......\r\n");
        if (kStatus_Success != SD_ReadBlocks(card, g_dataRead, DATA_BLOCK_START, DATA_BLOCK_COUNT))
        {
            //PRINTF("Read multiple data blocks failed.\r\n");
            return kStatus_Fail;
        }
    }
    else
    {
        memset(g_dataWrite, 0x67U, sizeof(g_dataWrite));
        snprintf((char *) g_dataWrite, sizeof(g_dataWrite), "Karta SD dziala :)");

        //PRINTF("\r\nWrite/read one data block......\r\n");
        if (kStatus_Success != SD_WriteBlocks(card, g_dataWrite, DATA_BLOCK_START, 1U))
        {
            //PRINTF("Write one data block failed.\r\n");
            return kStatus_Fail;
        }

        memset(g_dataRead, 0U, sizeof(g_dataRead));
        if (kStatus_Success != SD_ReadBlocks(card, g_dataRead, DATA_BLOCK_START, 1U))
        {
            //PRINTF("Read one data block failed.\r\n");
            return kStatus_Fail;
        }

        //PRINTF("Compare the read/write content......\r\n");
        if (memcmp(g_dataRead, g_dataWrite, FSL_SDMMC_DEFAULT_BLOCK_SIZE))
        {
            //PRINTF("The read/write content isn't consistent.\r\n");
            return kStatus_Fail;
        }
        //PRINTF("The read/write content is consistent.\r\n");

        //PRINTF("Write/read multiple data blocks......\r\n");
        if (kStatus_Success != SD_WriteBlocks(card, g_dataWrite, DATA_BLOCK_START, DATA_BLOCK_COUNT))
        {
            //PRINTF("Write multiple data blocks failed.\r\n");
            return kStatus_Fail;
        }

        memset(g_dataRead, 0U, sizeof(g_dataRead));

        if (kStatus_Success != SD_ReadBlocks(card, g_dataRead, DATA_BLOCK_START, DATA_BLOCK_COUNT))
        {
            //PRINTF("Read multiple data blocks failed.\r\n");
            return kStatus_Fail;
        }

        //PRINTF("Compare the read/write content......\r\n");
        if (memcmp(g_dataRead, g_dataWrite, FSL_SDMMC_DEFAULT_BLOCK_SIZE))
        {
            //PRINTF("The read/write content isn't consistent.\r\n");
            return kStatus_Fail;
        }
        //PRINTF("The read/write content is consistent.\r\n");

        //PRINTF("Erase multiple data blocks......\r\n");
        if (kStatus_Success != SD_EraseBlocks(card, DATA_BLOCK_START, DATA_BLOCK_COUNT))
        {
            //PRINTF("Erase multiple data blocks failed.\r\n");
            return kStatus_Fail;
        }
    }

    return kStatus_Success;
}

static void AccessCardTask(void *pvParameters)
{
    (void)pvParameters;

    sd_card_t *card = &g_sd;
    bool isReadOnly;
    char ch = '0';

    /* take card access semaphore */
    xSemaphoreTake(s_CardAccessSemaphore, portMAX_DELAY);

    while (ch != 'q')
    {
        if (SD_IsCardPresent(card) == false)
        {
            /* take card access semaphore */
            xSemaphoreTake(s_CardAccessSemaphore, portMAX_DELAY);
        }

        //PRINTF("\r\nRead/Write/Erase the card continuously until encounter error......\r\n");
        /* Check if card is readonly. */
        isReadOnly = SD_CheckReadOnly(card);

        SDCARD_ReadWrite(card, isReadOnly);

        //PRINTF("\r\nInput 'q' to quit card access task. \r\nInput other char to read/write/erase data blocks again.\r\n");
        // ch = GETCHAR();
        // PUTCHAR(ch);
    }

    //PRINTF("\r\nThe card access task will not access card again.\r\n");

    SD_Deinit(card);

    vTaskSuspend(NULL);
}

void SDCardInit()
{
    s_CardAccessSemaphore = xSemaphoreCreateBinary();
    s_CardDetectSemaphore = xSemaphoreCreateBinary();

    g_sd.host.base = SD_HOST_BASEADDR;
    g_sd.host.sourceClock_Hz = SD_HOST_CLK_FREQ;
    g_sd.usrParam.cd = &s_sdCardDetect;

    NVIC_SetPriority(SD_HOST_IRQ, 5U);

    /* SD host init function */
    if (SD_HostInit(&g_sd) != kStatus_Success)
    {
        //PRINTF("\r\nSD host init fail\r\n");
        return;
    }

    xTaskCreate(AccessCardTask, "AccessCardTask", configMINIMAL_STACK_SIZE + 500, NULL, configMAX_PRIORITIES - 1, NULL);
    xTaskCreate(CardDetectTask, "CardDetectTask", configMINIMAL_STACK_SIZE + 500, NULL, configMAX_PRIORITIES - 2, NULL);
    
}