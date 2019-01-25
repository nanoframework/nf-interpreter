//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <Target_Windows_Storage.h>
#include <target_windows_storage_config.h>

static FATFS SDC_FS;
static SDCConfig SDC_CFG;

// Card monitor timer
static virtual_timer_t cardMonitorTimer;

// Debounce counter
static unsigned debounceCounter;

// FS mounted and ready
static bool fileSystemReady = FALSE;

// Card event sources
static event_source_t insertedEvent, removedEvent;

/**
 * @brief   Insertion monitor timer callback function.
 *
 * @param[in] p         pointer to the @p BaseBlockDevice object
 *
 * @notapi
 */
static void InsertionMonitorCallback(void *p) 
{
  BaseBlockDevice *bbdp = p;

  chSysLockFromISR();

  if (debounceCounter > 0) 
  {
    if (blkIsInserted(bbdp))
    {
      if (--debounceCounter == 0)
      {
        chEvtBroadcastI(&insertedEvent);
      }
    }
    else
    {
      debounceCounter = POLLING_INTERVAL;
    }
  }
  else 
  {
    if (!blkIsInserted(bbdp))
    {
      debounceCounter = POLLING_INTERVAL;
      chEvtBroadcastI(&removedEvent);
    }
  }

  chVTSetI(&cardMonitorTimer, TIME_MS2I(POLLING_DELAY), InsertionMonitorCallback, bbdp);
  chSysUnlockFromISR();
}

// Card monitor start
static void StartCardMonitor(void *p)
{
  chEvtObjectInit(&insertedEvent);
  chEvtObjectInit(&removedEvent);

  chSysLock();

  debounceCounter = POLLING_INTERVAL;
  chVTSetI(&cardMonitorTimer, TIME_MS2I(POLLING_DELAY), InsertionMonitorCallback, p);

  chSysUnlock();
}

// Card insertion event handler
static void InsertHandler(eventid_t id)
{
  FRESULT err;

  (void)id;

  /*
   * On insertion SDC initialization and FS  mount.
   */
  if (sdcConnect(&SD_CARD_DRIVER))
  {
    return;
  }

  err = f_mount(&SDC_FS, "D:", 1);

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

  if (fileSystemReady)
  {
    //****** Test - Create a file!
    FIL fileObj;
    err = f_open(&fileObj, "TestMessage.txt", FA_CREATE_ALWAYS);
    f_close(&fileObj);
      
    if (err != FR_OK)
    {
      osDelay(1000);
    }
    else
    {

    }
    //******* End Test
  }
}

// Card removal event handler
static void RemoveHandler(eventid_t id)
{
  (void)id;
  
  sdcDisconnect(&SD_CARD_DRIVER);

  fileSystemReady = FALSE;
}

const evhandler_t sdcardEventHandler[] = { InsertHandler, RemoveHandler };
event_listener_t eventListener0, eventListener1;

void Target_FileSystemInit()
{
  // activates the  SDC driver 2 using default configuration
  sdcStart(&SD_CARD_DRIVER, &SDC_CFG);

  // activates the card insertion monitor
  StartCardMonitor(&SD_CARD_DRIVER);

  chEvtRegister(&insertedEvent, &eventListener0, 0);
  chEvtRegister(&removedEvent, &eventListener1, 1);
}
