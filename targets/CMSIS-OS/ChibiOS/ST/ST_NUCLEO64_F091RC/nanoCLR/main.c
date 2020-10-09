//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>
#include <hal_nf_community.h>
#include <cmsis_os.h>

#include <serialcfg.h>
#include <swo.h>
#include <targetHAL.h>
#include <CLR_Startup_Thread.h>
#include <WireProtocol_ReceiverThread.h>
#include <nanoCLR_Application.h>
#include <nanoPAL_BlockStorage.h>
#include <nanoHAL_v2.h>
#include <targetPAL.h>

///////////////////////////////////////////////////////////////////////////////////////////
// RAM vector table declaration (valid for GCC only)
__IO uint32_t vectorTable[48] __attribute__((section(".RAMVectorTable")));

#define SYSCFG_MemoryRemap_SRAM     ((uint8_t)0x03)

///////////////////////////////////////////////////////////////////////////////////////////

// need to declare the Receiver thread here
osThreadDef(ReceiverThread, osPriorityHigh, 512, "ReceiverThread");
// declare CLRStartup thread here 
osThreadDef(CLRStartupThread, osPriorityNormal, 4096, "CLRStartupThread"); 

// configuration for debugger serial port
// dev notes:
// conservative baud rate value as 921600 has a high error percentage on baud rate clocking
// OVER8 bit on CR1 to further decrease baud rate clocking error 
static const SerialConfig uartConfig =
{
  460800,
  USART_CR1_OVER8,
  USART_CR2_STOP1_BITS,
  0
};

//  Application entry point.
int main(void) {

  // HAL initialization, this also initializes the configured device drivers
  // and performs the board-specific initializations.
  halInit();

  // init SWO as soon as possible to make it available to output ASAP
  #if (SWO_OUTPUT == TRUE)  
  SwoInit();
  #endif
  
  // relocate the vector table to RAM
  // Copy the vector table from the Flash (mapped at the base of the application
  // load address) to the base address of the SRAM at 0x20000000.
  for(int i = 0; i < 48; i++)
  {
    vectorTable[i] = *(__IO uint32_t*)((uint32_t)&__nanoImage_start__ + (i<<2));
  } 

  // set CFGR1 register MEM_MODE bits value as "memory remap to SRAM"
  SYSCFG->CFGR1 |= SYSCFG_MemoryRemap_SRAM;

  // The kernel is initialized but not started yet, this means that
  // main() is executing with absolute priority but interrupts are already enabled.
  osKernelInitialize();

  // start watchdog
  Watchdog_Init();

  #if (HAL_NF_USE_STM32_CRC == TRUE)
  // startup crc
  crcStart(NULL);
  #endif

  // starts the serial driver
  sdStart(&SERIAL_DRIVER, &uartConfig);

  // create the receiver thread
  osThreadCreate(osThread(ReceiverThread), NULL);

  // CLR settings to launch CLR thread
  CLR_SETTINGS clrSettings;
  (void)memset(&clrSettings, 0, sizeof(CLR_SETTINGS));

  clrSettings.MaxContextSwitches         = 50;
  clrSettings.WaitForDebugger            = false;
  clrSettings.EnterDebuggerLoopAfterExit = true;

  // create the CLR Startup thread 
  osThreadCreate(osThread(CLRStartupThread), &clrSettings);

  // start kernel, after this main() will behave like a thread with priority osPriorityNormal
  osKernelStart();

  while (true) { 
    osDelay(100);
  }
}
