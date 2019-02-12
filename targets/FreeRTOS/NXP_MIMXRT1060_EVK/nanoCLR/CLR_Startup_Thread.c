//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "board.h"

#include "FreeRTOS.h"
#include "task.h"

#include <nanoCLR_Application.h>
#include <nanoHAL_v2.h>
#include <string.h>


void CLRStartupThread(void * argument)
{
  CLR_SETTINGS* clrSettings = (CLR_SETTINGS*)argument;

  // initialize nanoHAL
  nanoHAL_Initialize_C();

  ClrStartup(*clrSettings);

  vTaskDelete(NULL);

  // this function never returns
}
