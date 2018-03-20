//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>
#include <cmsis_os.h>

#include "usbcfg.h"
#include <swo.h>
#include <CLR_Startup_Thread.h>
#include <WireProtocol_ReceiverThread.h>
#include <nanoCLR_Application.h>
#include <nanoPAL_BlockStorage.h>
#include <targetPAL.h>

#define FSMC_BASE             ((uint32_t)0x60000000) /*!< FSMC base address */

#define FSMC_R_BASE           ((uint32_t)0xA0000000) /*!< FSMC registers base address */

// SDRAM driver configuration structure
// need this definition here because it depends on the specifics of the target
#if (HAL_USE_FSMC == TRUE)
static const SDRAMConfig sdram_cfg = 
{
  .sdcr = (uint32_t) FMC_SDRAM_COLUMN_BITS_NUM_8 |
                     FMC_SDRAM_ROW_BITS_NUM_12 |
                     FMC_SDRAM_MEM_BUS_WIDTH_16 |
                     FMC_SDRAM_INTERN_BANKS_NUM_4 |
                     FMC_SDRAM_CAS_LATENCY_2 |
                     FMC_SDRAM_WRITE_PROTECTION_DISABLE |
                     FMC_SDRAM_CLOCK_PERIOD_2 |
                     FMC_SDRAM_RBURST_ENABLE |
                     FMC_SDRAM_RPIPE_DELAY_0,
  .sdtr = (uint32_t) ((2 - 1) | // FSMC_LoadToActiveDelay = 2 (TMRD: 2 Clock cycles)
                     ((6 - 1) <<  4) | // FSMC_ExitSelfRefreshDelay = 6 (TXSR: min=70ns (6x11.11ns))
                     ((4 - 1) <<  8) | // FSMC_SelfRefreshTime = 4 (TRAS: min=42ns (4x11.11ns) max=120k (ns))
                     ((6 - 1) << 12) | // FSMC_RowCycleDelay = 6 (TRC:  min=70 (6x11.11ns))
                     ((2 - 1) << 16) | // FSMC_WriteRecoveryTime = 2 (TWR:  min=1+ 7ns (1+1x11.11ns))
                     ((2 - 1) << 20) | // FSMC_RPDelay = 2 (TRP:  20ns => 2x11.11ns)
                     ((2 - 1) << 24)),  // FSMC_RCDDelay = 2 (TRCD: 20ns => 2x11.11ns)
  /* NRFS = 4-1*/
  .sdcmr = (SDRAM_MODEREG_BURST_LENGTH_1 |
                       SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL |
                       SDRAM_MODEREG_CAS_LATENCY_2 |
                       SDRAM_MODEREG_OPERATING_MODE_STANDARD |
                       SDRAM_MODEREG_WRITEBURST_MODE_SINGLE) << 9,

  // Set the refresh rate counter
  // (15.62 us x Freq) - 20
  .sdrtr = (uint32_t)(1292 << 1),
};

#define SDRAM_BANK_ADDR                 ((uint32_t)0xC0000000)
#define WRITE_READ_ADDR     ((uint32_t)0x0800)
#endif

uint32_t __attribute__((section (".myBufSection"))) aTxBuffer[255];
uint32_t aRxBuffer[255];

// need to declare the Receiver thread here
osThreadDef(ReceiverThread, osPriorityNormal, 2048, "ReceiverThread");
// declare CLRStartup thread here 
osThreadDef(CLRStartupThread, osPriorityNormal, 2048, "CLRStartupThread"); 

//  Application entry point.
int main(void) {

  // HAL initialization, this also initializes the configured device drivers
  // and performs the board-specific initializations.
  halInit();

  // init SWO as soon as possible to make it available to output ASAP
  #if (SWO_OUTPUT == TRUE)  
  SwoInit();
  #endif

  // The kernel is initialized but not started yet, this means that
  // main() is executing with absolute priority but interrupts are already enabled.
  osKernelInitialize();

  #if (HAL_USE_FSMC == TRUE)

  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();


  fsmcSdramInit();
  fsmcSdramStart(&SDRAMD, &sdram_cfg);

  uint32_t tmpIndex = 0;

  /* Put in global buffer different values */
  for (tmpIndex = 0; tmpIndex < 255; tmpIndex++ )
  {
    aTxBuffer[tmpIndex] = tmpIndex + 0xA244250F;
  }

uint32_t uwIndex = 0;
  // /* Write data to the SDRAM memory */
  // for (uwIndex = 0; uwIndex < 255; uwIndex++)
  // {
  //   (*((volatile unsigned long *) (SDRAM_BANK_ADDR + 4*uwIndex))) = aTxBuffer[uwIndex];
  // }

  /* Read back data from the SDRAM memory */
  for (uwIndex = 0; uwIndex < 255; uwIndex++)
  {
    aRxBuffer[uwIndex] = (*((volatile unsigned long *) (SDRAM_BANK_ADDR + 4*uwIndex)));
  }


  //*(__IO uint16_t*) (SDRAM_BANK_ADDR ) = 0xDEAD;


volatile int testValue = *(__IO uint16_t*) (SDRAM_BANK_ADDR);

  #endif

  //  Initializes a serial-over-USB CDC driver.
  sduObjectInit(&SDU1);
  sduStart(&SDU1, &serusbcfg);

  // Activates the USB driver and then the USB bus pull-up on D+.
  // Note, a delay is inserted in order to not have to disconnect the cable after a reset
  usbDisconnectBus(serusbcfg.usbp);
  chThdSleepMilliseconds(1500);
  usbStart(serusbcfg.usbp, &usbcfg);
  usbConnectBus(serusbcfg.usbp);

  // create the receiver thread
  osThreadCreate(osThread(ReceiverThread), NULL);
  // create the CLR Startup thread 
  osThreadCreate(osThread(CLRStartupThread), NULL); 


  // start kernel, after this main() will behave like a thread with priority osPriorityNormal
  osKernelStart();

  while (true) { 
    osDelay(100);
  }
}
