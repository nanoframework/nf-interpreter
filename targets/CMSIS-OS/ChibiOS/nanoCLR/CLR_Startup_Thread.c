//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>
#include <hal_nf_community.h>
#include <cmsis_os.h>

#include <nanoCLR_Application.h>
#include <nanoHAL_v2.h>
#include <string.h>


// This thread needs to be implemented at ChibiOS level because it has to include a call to chThdShouldTerminateX()
// in case the thread is requested to terminate by the CMSIS call osThreadTerminate()

__attribute__((noreturn))
void CLRStartupThread(void const * argument)
{
  CLR_SETTINGS* clrSettings = (CLR_SETTINGS*)argument;

  #if (HAL_USE_STM32_CRC == TRUE)
  // startup crc
  crcStart(NULL);
  #endif

  #if (HAL_USE_STM32_ONEWIRE == TRUE)
  // startup 1-Wire driver
  oneWireStart();
  #endif

  // initialize nanoHAL
  nanoHAL_Initialize_C();

  ClrStartup(*clrSettings);

  // loop until thread receives a request to terminate
  while (1) {
    osDelay(500);
  }

  // this function never returns
}
