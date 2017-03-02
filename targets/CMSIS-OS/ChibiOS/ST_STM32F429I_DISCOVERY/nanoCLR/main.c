//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//
#define STM32_SPI_USE_SPI5 TRUE

#include <ch.h>
#include <hal.h>
#include <cmsis_os.h>

#include "usbcfg.h"
#include <WireProtocol_ReceiverThread.h>



#include "/dev/ChibiOS_16.1.7/community/os/various/devices_lib/lcd/ili9341.h"


const SPIConfig spi_cfg5 = {
  NULL,
  GPIOC,
  GPIOC_SPI5_LCD_CS,
  (((1 << 3) & SPI_CR1_BR) | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_MSTR),
  0
};

extern SPIDriver SPID5;

const ILI9341Config ili9341_cfg = {
  &SPID5,
  GPIOD,
  GPIOD_LCD_WRX
};

static void initialize_lcd(void) {

  static const uint8_t pgamma[15] = {
    0x0F, 0x29, 0x24, 0x0C, 0x0E, 0x09, 0x4E, 0x78,
    0x3C, 0x09, 0x13, 0x05, 0x17, 0x11, 0x00
  };
  static const uint8_t ngamma[15] = {
    0x00, 0x16, 0x1B, 0x04, 0x11, 0x07, 0x31, 0x33,
    0x42, 0x05, 0x0C, 0x0A, 0x28, 0x2F, 0x0F
  };

  ILI9341Driver *const lcdp = &ILI9341D1;

  ili9341AcquireBus(lcdp);
  ili9341Select(lcdp);

  ili9341WriteCommand(lcdp, ILI9341_CMD_RESET);
  chThdSleepMilliseconds(10);

  ili9341WriteCommand(lcdp, ILI9341_CMD_DISPLAY_OFF);
  ili9341WriteCommand(lcdp, ILI9341_SET_MEM_ACS_CTL);
  ili9341WriteByte(lcdp, 0x08 | 0x40);

  ili9341WriteCommand(lcdp, ILI9341_SET_PIX_FMT);
  ili9341WriteByte(lcdp, 0x55);

  ili9341WriteCommand(lcdp, ILI9341_SET_FRAME_CTL_NORMAL);
  ili9341WriteByte(lcdp, 0x00);
  ili9341WriteByte(lcdp, 0x1B);

  ili9341WriteCommand(lcdp, ILI9341_SET_GAMMA);
  ili9341WriteByte(lcdp, 0x01);

  ili9341WriteCommand(lcdp, ILI9341_SET_COL_ADDR);
  ili9341WriteByte(lcdp, 0x00);
  ili9341WriteByte(lcdp, 0x00);
  ili9341WriteByte(lcdp, 0x00);
  ili9341WriteByte(lcdp, 0xEF);

  ili9341WriteCommand(lcdp, ILI9341_SET_PAGE_ADDR);
  ili9341WriteByte(lcdp, 0x00);
  ili9341WriteByte(lcdp, 0x00);
  ili9341WriteByte(lcdp, 0x01);
  ili9341WriteByte(lcdp, 0x3F);

  ili9341WriteCommand(lcdp, ILI9341_SET_ENTRY_MODE);
  ili9341WriteByte(lcdp, 0x07);

  ili9341WriteCommand(lcdp, ILI9341_SET_FUNCTION_CTL);
  ili9341WriteByte(lcdp, 0x0A);
  ili9341WriteByte(lcdp, 0x82);
  ili9341WriteByte(lcdp, 0x27);
  ili9341WriteByte(lcdp, 0x00);

  ili9341WriteCommand(lcdp, ILI9341_CMD_SLEEP_OFF);
  chThdSleepMilliseconds(120);

  ili9341WriteCommand(lcdp, ILI9341_CMD_DISPLAY_ON);
  chThdSleepMilliseconds(100);

  ili9341Unselect(lcdp);
  ili9341ReleaseBus(lcdp);
}

void ILI9341_ClearScreen()
{
	uint8_t buff[240];
  ILI9341Driver *const lcdp = &ILI9341D1;
	
	memset(buff,0x0A,sizeof(buff));

  ili9341AcquireBus(lcdp);
  ili9341Select(lcdp);
  ili9341WriteCommand(lcdp, ILI9341_SET_MEM);
	for (int i=0; 320; i++)
	{
    ili9341WriteChunk(lcdp, buff, 240);
	}

  ili9341Unselect(lcdp);
  ili9341ReleaseBus(lcdp);
}

void BlinkerThread(void const * argument)
{
  (void)argument;

  palSetPad(GPIOG, GPIOG_LED3_GREEN);
  palSetPad(GPIOG, GPIOG_LED4_RED);
  osDelay(1000);

  palClearPad(GPIOG, GPIOG_LED3_GREEN);
  palClearPad(GPIOG, GPIOG_LED4_RED);
  osDelay(250);

  while (true) {

      palSetPad(GPIOG, GPIOG_LED3_GREEN);
	  palClearPad(GPIOG, GPIOG_LED4_RED);
      osDelay(125);

      palClearPad(GPIOG, GPIOG_LED3_GREEN);
      palSetPad(GPIOG, GPIOG_LED4_RED);
      osDelay(125);

      
  }
}
osThreadDef(BlinkerThread, osPriorityNormal, 128);

// need to declare the Receiver thread here
osThreadDef(ReceiverThread, osPriorityNormal, 1024);

//  Application entry point.
int main(void) {

  // HAL initialization, this also initializes the configured device drivers
  // and performs the board-specific initializations.
  halInit();

  // The kernel is initialized but not started yet, this means that
  // main() is executing with absolute priority but interrupts are already enabled.
  osKernelInitialize();

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

  // start kernel, after this the main() thread has priority osPriorityNormal by default
  osKernelStart();
  
  osThreadCreate(osThread(BlinkerThread), NULL);

  /*
   * Activates the LCD-related drivers.
   */
  spiStart(&SPID5, &spi_cfg5);
  ili9341ObjectInit(&ILI9341D1);
  ili9341Start(&ILI9341D1, &ili9341_cfg);
  initialize_lcd();

  ILI9341_ClearScreen();

  //  Normal main() thread activity it does nothing except sleeping in a loop 
  while (true) {
    osDelay(1000);
  }
}

