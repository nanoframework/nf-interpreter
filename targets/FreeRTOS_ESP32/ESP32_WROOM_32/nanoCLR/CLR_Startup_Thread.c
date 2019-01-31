//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <Esp32_os.h>

#include <nanoCLR_Application.h>
#include <nanoHAL_v2.h>
#include <string.h>

// This thread needs to be implemented at ChibiOS level because it has to include a call to chThdShouldTerminateX()
// in case the thread is requested to terminate by the CMSIS call osThreadTerminate()

void CLRStartupThread(void const * argument)
{
  CLR_SETTINGS* clrSettings = (CLR_SETTINGS*)argument;
  
  nanoHAL_Initialize_C();

  ClrStartup(*clrSettings);

  // nothing to deinitialize or cleanup, so it's safe to return
}
