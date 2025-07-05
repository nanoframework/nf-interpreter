//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <hal.h>
#include <cmsis_os.h>
#include <target_storage_config.h>
#include <nanoHAL_System_IO_FileSystem.h>
#include <Target_System_IO_FileSystem.h>
#include <nanoHAL_v2.h>

#if defined(HAL_USBH_USE_MSD) && (HAL_USBH_USE_MSD == TRUE)
#include "usbh/dev/msd.h"
#endif

// need to declare these here as extern because they are C++ functions
extern bool FS_MountVolume(const char *rootName, uint32_t deviceFlags, char *fileSystemDriver);
extern void FS_UnmountVolume(const char *rootName);

// need to declare this here as extern
extern void PostManagedEvent(uint8_t category, uint8_t subCategory, uint16_t data1, uint32_t data2);

///////////////////////////////////////////
// code specific to SD Card

#if HAL_USE_SDC

// SD Card event sources
static event_source_t sdInsertedEvent, sdRemovedEvent;

// this timer is used to handle the debounce of SD card detect line
static virtual_timer_t sdCardDebounceTimer;
static bool sdCardPresent;

// Insertion monitor timer callback function.
static void SdCardInsertionMonitorCallback(virtual_timer_t *vtp, void *p)
{
    (void)vtp;
    BaseBlockDevice *bbdp = p;
    bool currentStatus;

    chSysLockFromISR();

    // get current status
    currentStatus = blkIsInserted(bbdp);

    if (sdCardPresent == currentStatus)
    {
        // value hasn't change for debounce interval so this is a valid change
        if (currentStatus)
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

void SdCardDetectCallback(void *arg)
{
    BaseBlockDevice *bbdp = (BaseBlockDevice *)arg;

    if (port_is_isr_context())
    {
        chSysLockFromISR();
        if (!chVTIsArmedI(&sdCardDebounceTimer))
        {
            // save current status
            sdCardPresent = blkIsInserted(bbdp);
            // setup timer
            chVTSetI(&sdCardDebounceTimer, TIME_MS2I(SDCARD_POLLING_DELAY), SdCardInsertionMonitorCallback, arg);
        }
        chSysUnlockFromISR();
    }
    else
    {
        if (!chVTIsArmed(&sdCardDebounceTimer))
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
    (void)id;

    // On insertion SDC initialization and FS  mount
    if (sdcConnect(&SD_CARD_DRIVER))
    {
        return;
    }

    if (!FS_MountVolume(INDEX0_DRIVE_LETTER, 0, "FATFS"))
    {
        sdcDisconnect(&SD_CARD_DRIVER);
    }
}

// Card removal event handler
void SdCardRemoveHandler(eventid_t id)
{
    (void)id;

    sdcDisconnect(&SD_CARD_DRIVER);

    FS_UnmountVolume(INDEX0_DRIVE_LETTER);
}

__attribute__((noreturn)) void SdCardWorkingThread(void const *argument)
{
    (void)argument;

    event_listener_t sdEventListener0, sdEventListener1;

    const evhandler_t sdcardEventHandler[] = {SdcardInsertHandler, SdCardRemoveHandler};

    // activates the SDC driver using default configuration
    sdcStart(&SD_CARD_DRIVER, NULL);

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

    for (;;)
    {
        chEvtDispatch(sdcardEventHandler, chEvtWaitOneTimeout(ALL_EVENTS, TIME_MS2I(SDCARD_POLLING_DELAY)));
    }
}

#endif

///////////////////////////////////////////

///////////////////////////////////////////
// code specific to USB MSD
#if HAL_USBH_USE_MSD

__attribute__((noreturn)) void UsbMsdWorkingThread(void const *argument)
{
    (void)argument;

    bool usbMsdFileSystemReady = false;

    // start USB host
    usbhStart(&USB_MSD_DRIVER);

    USBHMassStorageLUNDriver *msBlk = (USBHMassStorageLUNDriver *)&MSBLKD[0];

    for (;;)
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
            if (!usbMsdFileSystemReady)
            {
                if (FS_MountVolume(INDEX1_DRIVE_LETTER, 0, "FATFS"))
                {
                    usbMsdFileSystemReady = true;
                }
                else
                {
                    // mount operation failed, disconnect
                    usbhmsdLUNDisconnect(msBlk);
                }
            }
        }

        if (blkGetDriverState(msBlk) == BLK_STOP)
        {
            if (usbMsdFileSystemReady)
            {
                usbMsdFileSystemReady = false;

                FS_UnmountVolume(INDEX1_DRIVE_LETTER);
            }
        }

        usbhMainLoop(&USB_MSD_DRIVER);
    }
}

// Handler to force USB MSD mount
void UsbMsdForceMount()
{
    USBHMassStorageLUNDriver *msBlk = (USBHMassStorageLUNDriver *)&MSBLKD[0];

    // force unmount, just in case
    FS_UnmountVolume(INDEX1_DRIVE_LETTER);

    if (blkGetDriverState(msBlk) >= BLK_ACTIVE)
    {
        // if the driver is active, force disconnect to reset the USB stack
        usbhmsdLUNDisconnect((USBHMassStorageLUNDriver *)&MSBLKD[0]);
    }
}

#endif
