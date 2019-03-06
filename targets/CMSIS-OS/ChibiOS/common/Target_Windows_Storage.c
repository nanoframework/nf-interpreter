//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <hal.h>
#include <cmsis_os.h>
#include <ff.h>
#include <target_windows_storage_config.h>
#include <nanoHAL_Windows_Storage.h>
#include <Target_Windows_Storage.h>

#if HAL_USBH_USE_MSD
#include "usbh/dev/msd.h"
#endif

///////////////////////////////////////////
// code specific to USB MSD

#if HAL_USE_SDC

// declare SD Card working thread here 
osThreadDef(SdCardWorkingThread, osPriorityNormal, 256, "SDCWT"); 

// FS for SD Card mounted and ready
static bool sdCardFileSystemReady = false;

static FATFS sdCard_FS;
static SDCConfig SDC_CFG;

// Card monitor timer
static virtual_timer_t sdCardMonitorTimer;
// Debounce counter
static unsigned sdDebounceCounter;
// Card event sources
static event_source_t sdInsertedEvent, sdRemovedEvent;

// Insertion monitor timer callback function.
static void SdCardInsertionMonitorCallback(void *p) 
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

    chVTSetI(&sdCardMonitorTimer, TIME_MS2I(SDCARD_POLLING_DELAY), SdCardInsertionMonitorCallback, bbdp);
    chSysUnlockFromISR();
}

// Card monitor start
static void StartCardMonitor(void *p)
{
    chEvtObjectInit(&sdInsertedEvent);
    chEvtObjectInit(&sdRemovedEvent);

    chSysLock();

    sdDebounceCounter = SDCARD_POLLING_INTERVAL;
    chVTSetI(&sdCardMonitorTimer, TIME_MS2I(SDCARD_POLLING_DELAY), SdCardInsertionMonitorCallback, p);

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

    err = f_mount(&sdCard_FS, SDCARD_DRIVE_LETTER, 1);

    if (err != FR_OK)
    {
        osDelay(1000);

        sdcDisconnect(&SD_CARD_DRIVER);
        return;
    }
    else
    {
        sdCardFileSystemReady = TRUE;
    }
}

// Card removal event handler
static void SdCardRemoveHandler(eventid_t id)
{
    (void)id;

    sdcDisconnect(&SD_CARD_DRIVER);

    sdCardFileSystemReady = FALSE;
}


__attribute__((noreturn))
void SdCardWorkingThread(void const * argument)
{
    (void)argument;

    static const evhandler_t sdcardEventHandler[] = 
    { 
        SdcardInsertHandler,
        SdCardRemoveHandler 
    };

    event_listener_t sdEventListener0, sdEventListener1;

    // activates the card insertion monitor
    StartCardMonitor(&SD_CARD_DRIVER);

    chEvtRegister(&sdInsertedEvent, &sdEventListener0, 0);
    chEvtRegister(&sdRemovedEvent, &sdEventListener1, 1);

    for(;;)
    {
        chEvtDispatch(sdcardEventHandler, chEvtWaitOneTimeout(ALL_EVENTS, TIME_MS2I(500)));
    }
}

#endif

///////////////////////////////////////////

///////////////////////////////////////////
// code specific to USB MSD
#if HAL_USBH_USE_MSD

// declare USB MSD thread here 
osThreadDef(UsbMsdWorkingThread, osPriorityNormal, 1024, "UsbMsdThread"); 

// FS for USB MSD mounted and ready
static bool usbCardFileSystemReady = false;

static FATFS usbMsd_FS;

// USB MSD monitor timer
//static virtual_timer_t usbMsdMonitorTimer;

// USB MSD event sources
//static event_source_t usbInsertedEvent;

// Insertion monitor timer callback function.
// static void UsbMsdMonitorCallback(void *p) 
// {
//     USBHMassStorageLUNDriver* msBlk = p;

//     chSysLockFromISR();

//     if (blkGetDriverState(msBlk) == BLK_ACTIVE)
//     {
//         // BLK: Active, connect
//         usbhmsdLUNConnect(msBlk);
//     }

//     if (blkGetDriverState(msBlk) == BLK_READY)
//     {
//         // BLK: Ready
//         chEvtBroadcastI(&usbInsertedEvent);
//     }

//     chVTSetI(&usbMsdMonitorTimer, TIME_MS2I(USB_MSD_POLLING_DELAY), UsbMsdMonitorCallback, msBlk);
//     chSysUnlockFromISR();
// }

// USB Mass storage device monitor
// static void StartUsbMsdMonitor(void *p)
// {
//     chEvtObjectInit(&usbInsertedEvent);

//     chSysLock();

//     chVTSetI(&usbMsdMonitorTimer, TIME_MS2I(USB_MSD_POLLING_DELAY), UsbMsdMonitorCallback, p);

//     chSysUnlock();
// }

// USB MSD insertion event handler
// static void UsbMsdInsertHandler(eventid_t id)
// {
//     (void)id;

//     // TODO
// }

__attribute__((noreturn))
void UsbMsdWorkingThread(void const * argument)
{
    FRESULT err;

    // static const evhandler_t usbEventHandler[] = 
    // { 
    //     UsbMsdInsertHandler 
    // };
    
    // event_listener_t usbEventListener0;

    USBHMassStorageLUNDriver* msBlk = (USBHMassStorageLUNDriver*)argument;

    for(;;)
    {
        osDelay(USB_MSD_POLLING_INTERVAL);

        if (blkGetDriverState(msBlk) == BLK_ACTIVE)
        {
            // file system can't be ready
            usbCardFileSystemReady = false;

            // BLK: Active, connect
            usbhmsdLUNConnect(msBlk);
        }

        if (blkGetDriverState(msBlk) == BLK_READY)
        {
            // BLK: Ready
            if(!usbCardFileSystemReady)
            {
                // USB MSD file system not ready
                // mount drive
                err = f_mount(&usbMsd_FS, USB_MSD_DRIVE_LETTER, 1);

                if (err != FR_OK)
                {
                    // mount operation failed, disconnect
                    usbhmsdLUNDisconnect(msBlk);
                }
                else
                {
                    usbCardFileSystemReady = true;
                }                
            }
            //chEvtBroadcastI(&usbInsertedEvent);
        }

        usbhMainLoop(&USB_MSD_DRIVER);
    }
}

#endif

void Target_FileSystemInit()
{
  #if HAL_USE_SDC
    // activates the SDC driver using default configuration
    sdcStart(&SD_CARD_DRIVER, &SDC_CFG);

    // creates the SD card working thread 
    osThreadCreate(osThread(SdCardWorkingThread), NULL);

  #endif

  #if HAL_USBH_USE_MSD

    // start USB host
    usbhStart(&USB_MSD_DRIVER);

    // create the USB MSD working thread 
    osThreadCreate(osThread(UsbMsdWorkingThread), &MSBLKD[0]);

  #endif

}
