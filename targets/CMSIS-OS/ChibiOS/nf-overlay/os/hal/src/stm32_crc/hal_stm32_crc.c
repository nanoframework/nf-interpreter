//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

// This driver is slightly different from the usual ChibiOS drivers because the calls don't require an instance of the driver.
// The reason for this is that the hardware only has a single instance of the peripheral 
// so there is no point on wasting memory with calling functions with dummy arguments.


#include <hal.h>
#include <hal_nf_community.h>

#if (HAL_USE_STM32_CRC == TRUE)

///////////////////////////////////////////////////////////////////////////////
// Driver local definitions.                                                 //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Driver exported variables.                                                //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Driver local variables and types.                                         //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Driver local functions.                                                   //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Driver exported functions.                                                //
///////////////////////////////////////////////////////////////////////////////

// Initializes the CRC Driver
void crcInit(void) {
  crc_lld_init();
}

// Configures and activates the CRC peripheral
void crcStart(const crcConfig *config) {

  // better lock this to setup the driver and start the peripheral
  osalSysLock();

  crc_lld_start(config);

  osalSysUnlock();
}

// Deactivates the CRC peripheral
void crcStop() {

    // better lock this to stop the peripheral
    osalSysLock();
  
    crc_lld_stop();
    
    osalSysUnlock();
}

// Resets the current CRC calculation
void crcReset() {
  osalSysLock();
  crc_lld_reset();
  osalSysUnlock();
}

uint32_t crcCompute(const void* buffer, int size, uint32_t initialCrc) {

  return crc_lld_compute(buffer, size, initialCrc);
}

void crcAquireModule() {

  crc_lld_aquire();
}

void crcReleaseModule() {

  crc_lld_release();  
}

#endif
