//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <cmsis_os.h>

#include <nanoCLR_Application.h>
#include <nanoPAL_BlockStorage.h>


// need to declare this here because this is implemented as a C++ function
// and the declaration is in nanoHAL.h which is including C++ declarations
void nanoHAL_Initialize_C();

// This thread needs to be implemented at ChibiOS level because it has to include a call to chThdShouldTerminateX()
// in case the thread is requested to terminate by the CMSIS call osThreadTerminate()

void CLRStartupThread(void const * argument)
{
  (void)argument;

  // initialize block storage devices
  BlockStorage_AddDevices();

  // initialize nanoHAL
  nanoHAL_Initialize_C();

  CLR_SETTINGS clrSettings;

  memset(&clrSettings, 0, sizeof(CLR_SETTINGS));

  clrSettings.MaxContextSwitches         = 50;
  clrSettings.WaitForDebugger            = false;
  clrSettings.EnterDebuggerLoopAfterExit = true;

  ClrStartup(clrSettings);

  // loop until thread receives a request to terminate
  while (!chThdShouldTerminateX()) {
    osDelay(500);
  }

  // nothing to deinitialize or cleanup, so it's safe to return
}
