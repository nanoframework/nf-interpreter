//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "board.h"

#include "FreeRTOS.h"
#include "task.h"

#include <nanoCLR_Application.h>
#include <nanoHAL_v2.h>
#include <string.h>

void CLRStartupThread(void *argument)
{
  (void)argument;

  CLR_SETTINGS clrSettings;
  (void)memset(&clrSettings, 0, sizeof(CLR_SETTINGS));

  clrSettings.MaxContextSwitches = 50;
  clrSettings.WaitForDebugger = false;
  clrSettings.EnterDebuggerLoopAfterExit = true;

  // initialize nanoHAL
  nanoHAL_Initialize_C();

  ClrStartup(clrSettings);

  vTaskDelete(NULL);

  // this function never returns
}
