//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>
#include <cmsis_os.h>

#include "usbcfg.h"
#include <swo.h>
#include <CLR_Startup_Thread.h>
#include <WireProtocol_ReceiverThread.h>
#include <nanoCLR_Application.h>
#include <nanoPAL_BlockStorage.h>
#include <nanoHAL_v2.h>
#include <targetPAL.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ff.h>

#define FATFS_HAL_DEVICE SDCD2

// need to declare the Receiver thread here
osThreadDef(ReceiverThread, osPriorityHigh, 2048, "ReceiverThread");
// declare CLRStartup thread here 
osThreadDef(CLRStartupThread, osPriorityNormal, 4096, "CLRStartupThread"); 

// Temporary for debug of file system
extern int tiny_sprintf(char *out, const char *format, ...);
char buffer[255];

/**
 * @brief FS object.
 */
static FATFS SDC_FS;
static SDCConfig SDC_CFG;

/* FS mounted and ready.*/
static bool fs_ready = FALSE;



//  Application entry point.
int main(void) {

  // HAL initialization, this also initializes the configured device drivers
  // and performs the board-specific initializations.
  halInit();

  // init SWO as soon as possible to make it available to output ASAP
  #if (SWO_OUTPUT == TRUE)  
  SwoInit();
  #endif

  // The kernel is initialized but not started yet, this means that
  // main() is executing with absolute priority but interrupts are already enabled.
  osKernelInitialize();

  // start watchdog
  Watchdog_Init();

  // config and init external memory
  // this has to be called after osKernelInitialize, otherwise an hard fault will occur
  Target_ExternalMemoryInit();

  //  Initializes a serial-over-USB CDC driver.
  sduObjectInit(&SDU1);
  sduStart(&SDU1, &serusbcfg);

  // Activates the USB driver and then the USB bus pull-up on D+.
  // Note, a delay is inserted in order to not have to disconnect the cable after a reset
  usbDisconnectBus(serusbcfg.usbp);
  chThdSleepMilliseconds(1500);
  usbStart(serusbcfg.usbp, &usbcfg);
  usbConnectBus(serusbcfg.usbp);

  // create the receiver thread
  osThreadCreate(osThread(ReceiverThread), NULL);

  // CLR settings to launch CLR thread
  CLR_SETTINGS clrSettings;
  (void)memset(&clrSettings, 0, sizeof(CLR_SETTINGS));

  clrSettings.MaxContextSwitches         = 50;
  clrSettings.WaitForDebugger            = false;
  clrSettings.EnterDebuggerLoopAfterExit = true;

  // create the CLR Startup thread 
  osThreadCreate(osThread(CLRStartupThread), &clrSettings);

  // start kernel, after this main() will behave like a thread with priority osPriorityNormal
  osKernelStart();


  // *************** DEMO CODE FOR FATFS *******************
  /*
   * Activates the  SDC driver 2 using default configuration.
   */
      FRESULT err;

  if (!fs_ready)
  {
    
    sdcStart(&SDCD2, &SDC_CFG);

    // Temporary to indicate this event being fired
    palSetLine(LINE_LED1_RED);
    


    bool res = sdcConnect(&SDCD2);

    // Temporary to indicate this event being fired
    SwoPrintString("\r\nFatFs: Initializing SD completed.\r\n");
    tiny_sprintf(buffer, "with result %d", res);
    SwoPrintString(buffer);

    osDelay(1000);

    err = f_mount(&SDC_FS, "/", 1);

    if (err != FR_OK)
    {
      tiny_sprintf(buffer, "Error Mounting Drive %d", err);
      SwoPrintString(buffer);
      osDelay(1000);

      sdcDisconnect(&SDCD2);
    }
    else
    {
      SwoPrintString("\r\nFatFs: Mount completed...\r\n");

      fs_ready = TRUE;
      // Temporary: arriving here means we have an initialized and mounted SD Card
      // Indicated by a green LED
      palSetLine(LINE_LED2_GREEN);
    }
  }

  if (fs_ready)
  {
    osDelay(1000);

    //****** Test - Create a file!
    FIL fileObj;
    err = f_open(&fileObj, "TestMessage.txt", FA_CREATE_ALWAYS);
    f_close(&fileObj);
      
      if (err != FR_OK)
    {
      tiny_sprintf(buffer, "Error Creating File %d", err);
      SwoPrintString(buffer);
    }
    else
    {
      SwoPrintString("\r\nFatFs: file created...\r\n");
    }
  }

  // *******************************************************
  while (true) { 
    osDelay(100);
  }
}
