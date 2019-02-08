//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoCLR_Application.h>
#include <nanoHAL_v2.h>
#include <string.h>

void* CLRStartupThread(void* argument)
{
  CLR_SETTINGS* clrSettings = (CLR_SETTINGS*)argument;
  
  nanoHAL_Initialize_C();

  ClrStartup(*clrSettings);

  // nothing to deinitialize or cleanup, so it's safe to return
}
