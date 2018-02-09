//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <cmsis_os.h>

#include <nanoCLR_Application.h>
#include <nanoHAL_v2.h>


// This thread needs to be implemented at ChibiOS level because it has to include a call to chThdShouldTerminateX()
// in case the thread is requested to terminate by the CMSIS call osThreadTerminate()

__attribute__((noreturn))
void CLRStartupThread(void const * argument)
{
  // initialize nanoHAL
  nanoHAL_Initialize_C();

  CLR_SETTINGS clrSettings;
  memset(&clrSettings, 0, sizeof(CLR_SETTINGS));

  clrSettings.MaxContextSwitches         = 50;
  clrSettings.WaitForDebugger            = false;
  clrSettings.EnterDebuggerLoopAfterExit = true;

  ClrStartup(clrSettings);

  // loop until thread receives a request to terminate
  while (1) {
    osDelay(500);
  }

  // this function never returns
}
