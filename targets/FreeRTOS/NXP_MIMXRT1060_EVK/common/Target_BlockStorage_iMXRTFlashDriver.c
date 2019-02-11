//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "MIMXRT1062.h"
#include "flexspi_nor_flash_ops.h"
#include "fsl_cache.h"
#include "fsl_flexspi.h"
#include "string.h"
#include <nanoPAL_BlockStorage.h>
#include "targetHAL.h"
#include <Target_BlockStorage_iMXRTFlashDriver.h>

#include "FreeRTOS.h"

__attribute__ ((section(".ramfunc.$RAM2")))
bool iMXRTFlexSPIDriver_InitializeDevice(void *context) {
  (void)context;

  flexspi_nor_flash_init(FLEXSPI);
  flexspi_nor_enable_quad_mode(FLEXSPI);

  return true;
}

bool iMXRTFlexSPIDriver_UninitializeDevice(void *context) {
  (void)context;

  return true;
}

DeviceBlockInfo *iMXRTFlexSPIDriver_GetDeviceInfo(void *context) {
  MEMORY_MAPPED_NOR_BLOCK_CONFIG *config = context;

  return config->BlockConfig.BlockDeviceInformation;
}

bool iMXRTFlexSPIDriver_Read(void *context, ByteAddress startAddress,
                             unsigned int numBytes, unsigned char *buffer) {
  (void)context;

  memcpy(buffer, (void *)startAddress, numBytes);

  return true;
}

bool iMXRTFlexSPIDriver_Write(void *context, ByteAddress startAddress,
                              unsigned int numBytes, unsigned char *buffer,
                              bool readModifyWrite) {
  (void)context;

  // Read-modify-write is used for FAT filesystems only
  if (readModifyWrite) {
    return false;
  }

  for (uint32_t i = 0; i < numBytes / FLASH_PAGE_SIZE; i++) {

    portENTER_CRITICAL();
    status_t status = flexspi_nor_flash_page_program(
        FLEXSPI, startAddress - (uint32_t)&__flash_start__ + i * FLASH_PAGE_SIZE,
        (void *)buffer + i * FLASH_PAGE_SIZE);
    portEXIT_CRITICAL();
    if (status != kStatus_Success) {
      return false;
    }

  }
  DCACHE_CleanInvalidateByRange(startAddress, numBytes);

  return true;
}

#define FLASH_ERASED_WORD 0xFFFFFFFF

bool iMXRTFlexSPIDriver_IsBlockErased(void *context, ByteAddress blockAddress,
                                      unsigned int length) {
  (void)context;
  uint32_t *cursor = (uint32_t *)blockAddress;
  uint32_t *endAddress = (uint32_t *)(blockAddress + length);
  // an erased flash address has to read FLASH_ERASED_WORD
  // OK to check by word (32 bits) because the erase is performed by 'page'
  // whose size is word multiple
  while (cursor < endAddress) {
    if (*cursor++ != FLASH_ERASED_WORD) {
      // found an address with something other than FLASH_ERASED_WORD!!
      return false;
    }
  }

  // reached here so the segment must be erased
  return true;
}

bool iMXRTFlexSPIDriver_EraseBlock(void *context, ByteAddress address) {
  (void)context;
  portENTER_CRITICAL();
  status_t status = flexspi_nor_flash_erase_sector(FLEXSPI, address - (uint32_t)&__flash_start__);
  portEXIT_CRITICAL();
  if (status != kStatus_Success) {
    return false;
  }

  DCACHE_CleanInvalidateByRange(address, SECTOR_SIZE);
  return true;
}

void iMXRTFlexSPIDriver_SetPowerState(void *context, unsigned int state) {
  (void)context;
  (void)state;
  // nothing to be done here
}
