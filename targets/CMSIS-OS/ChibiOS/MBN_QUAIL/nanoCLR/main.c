//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

// Using Devantech LCD03 display in I2C mode @ address 0xC8

#include <ch.h>
#include <hal.h>
#include <hal_nf_community.h>
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
// need to declare the Receiver thread here
osThreadDef(ReceiverThread, osPriorityHigh, 4096, "ReceiverThread");
// declare CLRStartup thread here 
osThreadDef(CLRStartupThread, osPriorityNormal, 4096, "CLRStartupThread"); 

#if HAL_USE_SDC
// declare SD Card working thread here 
osThreadDef(SdCardWorkingThread, osPriorityNormal, 1024, "SDCWT"); 
#endif
//  Application entry point.
int main(void) {

  // find out wakeup reason
  if((RTC->ISR & RTC_ISR_ALRAF) == RTC_ISR_ALRAF)
  {
    // standby, match WakeupReason_FromStandby enum
    WakeupReasonStore = 1;
  }
  else if((PWR->CSR & PWR_CSR_WUF) == PWR_CSR_WUF)
  {
    // wake from pin, match WakeupReason_FromPin enum
    WakeupReasonStore = 2;
  }
  else
  {
    // undetermined reason, match WakeupReason_Undetermined enum
    WakeupReasonStore = 0;
  }

  // first things first: need to clear any possible wakeup flags
  // if this is not done here the next standby -> wakeup sequence won't work
  CLEAR_BIT(RTC->CR, RTC_CR_ALRAIE);
  CLEAR_BIT(RTC->ISR, RTC_ISR_ALRAF);
  SET_BIT(PWR->CR, PWR_CR_CWUF);

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
  ///////////////////////////////////////////////////////////////////////////////////////////////////////
  // for STM32F4 family if watchdog is enabled can't use standby mode because the IWDG can't be stoped //
  ///////////////////////////////////////////////////////////////////////////////////////////////////////
  Watchdog_Init();

  #if (HAL_NF_USE_STM32_CRC == TRUE)
  // startup crc
  crcStart(NULL);
  #endif

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

  #if HAL_USE_SDC
  // creates the SD card working thread 
  osThreadCreate(osThread(SdCardWorkingThread), NULL);
  #endif
  // start kernel, after this main() will behave like a thread with priority osPriorityNormal
  osKernelStart();
  
  while (true) { 
    osDelay(100);
  }
}
