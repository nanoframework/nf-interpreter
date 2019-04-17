//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>
#include <hal_nf_community.h>
#include <cmsis_os.h>
#include <string.h>

#include <targetHAL.h>
#include <nanoHAL_v2.h>
#include <nanoCLR_Application.h>
#include <nanoPAL_BlockStorage.h>

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

  ///////////////////////////////////////////
  // sanity check of block storage addresses

  #if !defined(BUILD_RTM)

  // if the target Device_BlockStorageConfig (and collections) and the linker file have an address mismatch
  // the assertions bellow will cause an exception
  // these checks are only included when RTM build option isn't specified
  BlockStorageStream stream;
  memset(&stream, 0, sizeof(BlockStorageStream));

  // DEPLOYMENT block
  // this one lives in a single block so we are OK with checking start, end and length
  BlockStorageStream_Initialize(&stream, BlockUsage_DEPLOYMENT);
  ASSERT(stream.BaseAddress == (uint32_t)&__deployment_start__);
  //ASSERT(stream.Length == ((uint32_t)&__deployment_end__) - ((uint32_t)&__deployment_start__));

  // CODE block (CLR)
  // this one can be spread accross several blocks so we can only check the start address
  memset(&stream, 0, sizeof(BlockStorageStream));
  BlockStorageStream_Initialize(&stream, BlockUsage_CODE);
  ASSERT(stream.BaseAddress == (uint32_t)&__nanoImage_start__);
  //ASSERT(stream.Length == ((uint32_t)&__nanoImage_end__) - ((uint32_t)&__nanoImage_start__));

  // CONFIG block
  // this one lives in a single block so we are OK with checking start, end and length
  memset(&stream, 0, sizeof(BlockStorageStream));
  BlockStorageStream_Initialize(&stream, BlockUsage_CONFIG);
  ASSERT(stream.BaseAddress == (uint32_t)&__nanoConfig_start__);
  ASSERT(stream.Length == ((uint32_t)&__nanoConfig_end__) - ((uint32_t)&__nanoConfig_start__));

  #endif
  ///////////////////////////////////////////

#if LWIP_NETCONN_SEM_PER_THREAD
  // need to create a semaphore for lwIP
  semaphore_t *semaphore = chPoolAlloc(NULL);
  chSemObjectInit(semaphore, (cnt_t)1);
  chSemWaitTimeout(semaphore, 0);
  thread_t* currentThread = chThdGetSelfX();
  currentThread->localStorage = semaphore;
#endif

  ClrStartup(*clrSettings);

  // loop until thread receives a request to terminate
  while (1) {
    osDelay(500);
  }

  // this function never returns
}
