//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <hal.h>
#include <target_windows_storage_config.h>
#include <Target_Windows_Storage.h>

// FS mounted and ready
static bool fileSystemReady = false;

///////////////////////////////////////////
// code specific to USB MSD

#if HAL_USE_SDC

static FATFS SDC_FS;
static SDCConfig SDC_CFG;

// Card monitor timer
static virtual_timer_t sdCardMonitorTimer;
// Debounce counter
static unsigned sdDebounceCounter;
// Card event sources
static event_source_t sdInsertedEvent, sdRemovedEvent;

// Insertion monitor timer callback function.
static void SdcardInsertionMonitorCallback(void *p) 
{
    BaseBlockDevice *bbdp = p;

    chSysLockFromISR();

    if (sdDebounceCounter > 0) 
    {
        if (blkIsInserted(bbdp))
        {
            if (--sdDebounceCounter == 0)
            {
                chEvtBroadcastI(&sdInsertedEvent);
            }
        }
        else
        {
            sdDebounceCounter = SDCARD_POLLING_INTERVAL;
        }
    }
    else 
    {
        if (!blkIsInserted(bbdp))
        {
            sdDebounceCounter = SDCARD_POLLING_INTERVAL;
            chEvtBroadcastI(&sdRemovedEvent);
        }
    }

    chVTSetI(&sdCardMonitorTimer, TIME_MS2I(SDCARD_POLLING_DELAY), SdcardInsertionMonitorCallback, bbdp);
    chSysUnlockFromISR();
}

// Card monitor start
static void StartCardMonitor(void *p)
{
    chEvtObjectInit(&sdInsertedEvent);
    chEvtObjectInit(&sdRemovedEvent);

    chSysLock();

    sdDebounceCounter = SDCARD_POLLING_INTERVAL;
    chVTSetI(&sdCardMonitorTimer, TIME_MS2I(SDCARD_POLLING_DELAY), SdcardInsertionMonitorCallback, p);

    chSysUnlock();
}

// Card insertion event handler
static void SdcardInsertHandler(eventid_t id)
{
    FRESULT err;

    (void)id;

    // On insertion SDC initialization and FS  mount
    if (sdcConnect(&SD_CARD_DRIVER))
    {
        return;
    }

    err = f_mount(&SDC_FS, SDCARD_DRIVE_LETTER, 1);

    if (err != FR_OK)
    {
        osDelay(1000);

        sdcDisconnect(&SD_CARD_DRIVER);
        return;
    }
    else
    {
        fileSystemReady = TRUE;
    }
}

// Card removal event handler
static void SdcardRemoveHandler(eventid_t id)
{
    (void)id;

    sdcDisconnect(&SD_CARD_DRIVER);

    fileSystemReady = FALSE;
}

const evhandler_t sdcardEventHandler[] = { SdcardInsertHandler, SdcardRemoveHandler };
event_listener_t sdEventListener0, sdEventListener1;

#endif

///////////////////////////////////////////

///////////////////////////////////////////
// code specific to USB MSD
#if HAL_USBH_USE_MSD

// Card event sources
static event_source_t usbInsertedEvent;

// Insertion monitor timer callback function.
static void UsbMsdInsertionMonitorCallback(void *p) 
{
    BaseBlockDevice *bbdp = p;

    chSysLockFromISR();

    if (blkGetDriverState(bbdp) == BLK_ACTIVE)
    {
        // BLK: Active, connect
        usbhmsdLUNConnect(bbdp);
    }

    if (blkGetDriverState(bbdp) != BLK_READY)
    {
        // BLK: Ready
        chEvtBroadcastI(&usbInsertedEvent);
    }

    chVTSetI(&usbCardMonitorTimer, TIME_MS2I(USB_MSD_POLLING_DELAY), UsbMsdInsertionMonitorCallback, bbdp);
    chSysUnlockFromISR();
}

// USB Mass storage device monitor
static void StartUsbMsdMonitor(void *p)
{
    chEvtObjectInit(&usbInsertedEvent);

    chSysLock();

    usbDebounceCounter = USB_MSD_POLLING_INTERVAL;
    chVTSetI(&usbCardMonitorTimer, TIME_MS2I(USB_MSD_POLLING_DELAY), UsbMsdInsertionMonitorCallback, p);

    chSysUnlock();
}

// USB MSD insertion event handler
static void UsbMsdInsertHandler(eventid_t id)
{
    (void)id;

    // TODO
}

const evhandler_t usbEventHandler[] = { UsbMsdInsertHandler };
event_listener_t usbEventListener0;

#endif

void Target_FileSystemInit()
{
  #if HAL_USE_SDC
    // activates the SDC driver using default configuration
    sdcStart(&SD_CARD_DRIVER, &SDC_CFG);

    // activates the card insertion monitor
    StartCardMonitor(&SD_CARD_DRIVER);

    chEvtRegister(&sdInsertedEvent, &sdEventListener0, 0);
    chEvtRegister(&sdRemovedEvent, &sdEventListener1, 1);
  #endif

  #if HAL_USBH_USE_MSD

    // activates the USB insertion monitor
    StartUsbMsdMonitor(&MSBLKD[0]);

    chEvtRegister(&usbInsertedEvent, &usbEventListener0, 0);

  #endif

}
