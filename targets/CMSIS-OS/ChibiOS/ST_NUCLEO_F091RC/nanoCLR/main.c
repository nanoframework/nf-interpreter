//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>
#include <cmsis_os.h>

#include <targetHAL.h>
#include <WireProtocol_ReceiverThread.h>
#include <CLR_Startup_Thread.h>

///////////////////////////////////////////////////////////////////////////////////////////
// RAM vector table declaration (valid for GCC only)
__IO uint32_t vectorTable[48] __attribute__((section(".RAMVectorTable")));

#define SYSCFG_MemoryRemap_SRAM     ((uint8_t)0x03)

///////////////////////////////////////////////////////////////////////////////////////////


void BlinkerThread(void const * argument)
{
  (void)argument;

  palSetPad(GPIOA, GPIOA_LED_GREEN);
  osDelay(1000);

  palClearPad(GPIOA, GPIOA_LED_GREEN);
  osDelay(250);

  // loop until thread receives a request to terminate
  while (!chThdShouldTerminateX()) {

    palSetPad(GPIOA, GPIOA_LED_GREEN);
    osDelay(125);
  
    palClearPad(GPIOA, GPIOA_LED_GREEN);
    osDelay(125);
  }
}
osThreadDef(BlinkerThread, osPriorityNormal, 128);

// need to declare the Receiver thread here
osThreadDef(ReceiverThread, osPriorityNormal, 1024);

// declare CLRStartup thread here
osThreadDef(CLRStartupThread, osPriorityNormal, 1024);

//  Application entry point.
int main(void) {

  // HAL initialization, this also initializes the configured device drivers
  // and performs the board-specific initializations.
  halInit();

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

  // Prepares the serial driver 2 using UART2
  sdStart(&SD2, NULL);
  palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(1)); // USART2 TX
  palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(1)); // USART2 RX

  // Creates the blinker thread, it does not start immediately.
  osThreadCreate(osThread(BlinkerThread), NULL);

  // create the receiver thread
  osThreadCreate(osThread(ReceiverThread), NULL);
  
  // create the CLR Startup thread
  osThreadCreate(osThread(CLRStartupThread), NULL);

  // start kernel, after this the main() thread has priority osPriorityNormal by default
  osKernelStart();

  //  Normal main() thread activity it does nothing except sleeping in a loop 

  // CLR_SETTINGS clrSettings;

  // memset(&clrSettings, 0, sizeof(CLR_SETTINGS));

  // clrSettings.MaxContextSwitches         = 50;
  // clrSettings.WaitForDebugger            = false;
  // clrSettings.EnterDebuggerLoopAfterExit = true;

  // ClrStartup(clrSettings);

  while (true) {
    osDelay(1000);
  }
}
