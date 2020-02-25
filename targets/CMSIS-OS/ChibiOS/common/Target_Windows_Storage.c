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
#include <nanoHAL_v2.h>

#if HAL_USBH_USE_MSD
#include "usbh/dev/msd.h"
#endif

// need to declare this here as extern
extern void PostManagedEvent(uint8_t category, uint8_t subCategory, uint16_t data1, uint32_t data2);

///////////////////////////////////////////
// code specific to SD Card

#if HAL_USE_SDC

// FS for SD Card mounted and ready
bool sdCardFileSystemReady;

static FATFS sdCard_FS;
static SDCConfig SDC_CFG;

// SD Card event sources
static event_source_t sdInsertedEvent, sdRemovedEvent;

// this timer is used to handle the debounce of SD card detect line
static virtual_timer_t sdCardDebounceTimer;
static bool sdCardPresent;

// Insertion monitor timer callback function.
static void SdCardInsertionMonitorCallback(void *p) 
{
    BaseBlockDevice *bbdp = p;
    bool currentStatus;

    chSysLockFromISR();
    
    // get current status
    currentStatus = blkIsInserted(bbdp);

    if(sdCardPresent == currentStatus)
    {
        // value hasn't change for debounce interval so this is a valid change
        if(currentStatus)
        {
            chEvtBroadcastI(&sdInsertedEvent);
        }
        else
        {
            chEvtBroadcastI(&sdRemovedEvent);
        }        
    }

    chSysUnlockFromISR();
}

static void SdCardDetectCallback(void *arg)
{
    BaseBlockDevice* bbdp = (BaseBlockDevice*)arg;

    if (port_is_isr_context()) {
    	chSysLockFromISR();
    	if(!chVTIsArmedI(&sdCardDebounceTimer))
		{
        	// save current status
        	sdCardPresent = blkIsInserted(bbdp);
        	// setup timer
        	chVTSetI(&sdCardDebounceTimer, TIME_MS2I(SDCARD_POLLING_DELAY), SdCardInsertionMonitorCallback, arg);
		}
    	chSysUnlockFromISR();

	} else {
		if(!chVTIsArmed(&sdCardDebounceTimer))
		{

			// save current status
			sdCardPresent = blkIsInserted(bbdp);

			// setup timer
			chVTSet(&sdCardDebounceTimer, TIME_MS2I(SDCARD_POLLING_DELAY), SdCardInsertionMonitorCallback, arg);
		}
	}
}

// Card insertion event handler
void SdcardInsertHandler(eventid_t id)
{
    FRESULT err;

    (void)id;

    // On insertion SDC initialization and FS  mount
    if (sdcConnect(&SD_CARD_DRIVER))
    {
        return;
    }

    err = f_mount(&sdCard_FS, SD_CARD_DRIVE_INDEX, 1);

    if (err != FR_OK)
    {
        osDelay(1000);

        sdcDisconnect(&SD_CARD_DRIVER);
        return;
    }
    else
    {
        sdCardFileSystemReady = true;

        // post event to managed app
        PostManagedEvent( EVENT_STORAGE, 0, StorageEventType_RemovableDeviceInsertion, SD_CARD_DRIVE_INDEX_NUMERIC );
    }
}

// Card removal event handler
void SdCardRemoveHandler(eventid_t id)
{
    (void)id;

    sdcDisconnect(&SD_CARD_DRIVER);

    sdCardFileSystemReady = false;

    // post event to managed app
    PostManagedEvent( EVENT_STORAGE, 0, StorageEventType_RemovableDeviceRemoval, SD_CARD_DRIVE_INDEX_NUMERIC );
}

__attribute__((noreturn))
void SdCardWorkingThread(void const * argument)
{
    (void)argument;
    
    event_listener_t sdEventListener0, sdEventListener1;

    const evhandler_t sdcardEventHandler[] = 
    { 
        SdcardInsertHandler,
        SdCardRemoveHandler 
    };

    sdCardFileSystemReady = false;

    // activates the SDC driver using default configuration
    sdcStart(&SD_CARD_DRIVER, &SDC_CFG);

    // setup line event in SD Card detect GPIO
    palEnableLineEvent(SDCARD_LINE_DETECT, PAL_EVENT_MODE_BOTH_EDGES);
    palSetLineCallback(SDCARD_LINE_DETECT, SdCardDetectCallback, &SD_CARD_DRIVER);

    // init timer
    chVTObjectInit(&sdCardDebounceTimer);

    // init events
    chEvtObjectInit(&sdInsertedEvent);
    chEvtObjectInit(&sdRemovedEvent);

    // register events
    chEvtRegister(&sdInsertedEvent, &sdEventListener0, 0);
    chEvtRegister(&sdRemovedEvent, &sdEventListener1, 1);

    // force initial check
    SdCardDetectCallback(&SD_CARD_DRIVER);

    for(;;)
    {
        chEvtDispatch(sdcardEventHandler, chEvtWaitOneTimeout(ALL_EVENTS, TIME_MS2I(SDCARD_POLLING_DELAY)));
    }
}

#endif

///////////////////////////////////////////

///////////////////////////////////////////
// code specific to USB MSD
#if HAL_USBH_USE_MSD

// FS for USB MSD mounted and ready
bool usbMsdFileSystemReady;

static FATFS usbMsd_FS;

__attribute__((noreturn))
void UsbMsdWorkingThread(void const * argument)
{
    (void)argument;

    FRESULT err;

    usbMsdFileSystemReady = false;

    // start USB host
    usbhStart(&USB_MSD_DRIVER);

    USBHMassStorageLUNDriver* msBlk = (USBHMassStorageLUNDriver*)&MSBLKD[0];

    for(;;)
    {
        osDelay(USB_MSD_POLLING_INTERVAL);

        if (blkGetDriverState(msBlk) == BLK_ACTIVE)
        {
            // file system can't be ready
            usbMsdFileSystemReady = false;

            // BLK: Active, connect
            usbhmsdLUNConnect(msBlk);
        }

        if (blkGetDriverState(msBlk) == BLK_READY)
        {
            // BLK: Ready
            if(!usbMsdFileSystemReady)
            {
                // USB MSD file system not ready
                // mount drive
                err = f_mount(&usbMsd_FS, USB_MSD_DRIVE_INDEX, 1);

                if (err != FR_OK)
                {
                    // mount operation failed, disconnect
                    usbhmsdLUNDisconnect(msBlk);
                }
                else
                {
                    usbMsdFileSystemReady = true;

                    // post event to managed app
                    PostManagedEvent( EVENT_STORAGE, 0, StorageEventType_RemovableDeviceInsertion, USB_MSD_DRIVE_INDEX_NUMERIC );
                }                
            }
        }

        if (blkGetDriverState(msBlk) == BLK_STOP)
        {
            if(usbMsdFileSystemReady)
            {
                usbMsdFileSystemReady = false;

                // post event to managed app
                PostManagedEvent( EVENT_STORAGE, 0, StorageEventType_RemovableDeviceRemoval, USB_MSD_DRIVE_INDEX_NUMERIC );
            }
        }

        usbhMainLoop(&USB_MSD_DRIVER);
    }
}

#endif
