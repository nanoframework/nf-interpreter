//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>
#include <cmsis_os.h>

#ifdef _USB_
#include "usbcfg.h"
#include <WireProtocol_ReceiverThread.h>
#endif

void BlinkerThread(void const * argument)
{
	(void)argument;
	
	palSetPad(GPIOE, GPIOE_LED1);
	palClearPad(GPIOE, GPIOE_LED2);
	palClearPad(GPIOC, GPIOC_LED3);
	palClearPad(GPIOA, GPIOA_INT1);
	
	// loop until thread receives a request to terminate
	while (!chThdShouldTerminateX()) 
	{
		palClearPad(GPIOC, GPIOC_LED3);
		palSetPad(GPIOE, GPIOE_LED1);
		osDelay(200);
		palClearPad(GPIOE, GPIOE_LED1);
		palSetPad(GPIOE, GPIOE_LED2);
		osDelay(200);
		palClearPad(GPIOE, GPIOE_LED2);
		palSetPad(GPIOC, GPIOC_LED3);
		osDelay(200);
	}
}

osThreadDef(BlinkerThread, osPriorityNormal, 128);

#ifdef _USB_
osThreadDef(ReceiverThread, osPriorityNormal, 1024);
#endif

static PWMConfig pwmTimer1 = 
{
	200000,		/* 200Khz PWM clock frequency*/
	1024,		/* PWM period of 1024 ticks ~ 0.005 second */
	NULL,		/* No callback */
	/* Only channel 1 enabled */
	{
		{PWM_OUTPUT_ACTIVE_HIGH, NULL},
		{PWM_OUTPUT_DISABLED, NULL},
		{PWM_OUTPUT_DISABLED, NULL},
		{PWM_OUTPUT_DISABLED, NULL}
	}
};

static PWMConfig pwmTimer4 = 
{
	500000,		/* 500Khz PWM clock frequency*/
	1024,		/* PWM period of 1024 ticks ~ 0.005 second */
	NULL,		/* No callback */
	/* Channels 2,3 & 4 enabled */
	{
		{PWM_OUTPUT_DISABLED, NULL},
		{PWM_OUTPUT_ACTIVE_HIGH, NULL},
		{PWM_OUTPUT_ACTIVE_HIGH, NULL},
		{PWM_OUTPUT_ACTIVE_HIGH, NULL}
	}
};

void TestPWM()
{
	pwmStart(&PWMD1, &pwmTimer1);
	pwmEnableChannel(&PWMD1, 0, 512);

	pwmStart(&PWMD4, &pwmTimer4);
	pwmEnableChannel(&PWMD4, 1, 512);
	pwmEnableChannel(&PWMD4, 2, 1024);
	pwmEnableChannel(&PWMD4, 3, 256);
}

static const SPIConfig spi1_cfg =
{
	NULL,
	GPIOA,                                          /*   port of CS on Socket #1  */
	GPIOA_CS1,                                      /*   pin of CS    */
	SPI_CR1_BR_0,                                   /*   CR1 register, default value */
};

static const SPIConfig spi3_cfg =
{
	NULL,
	GPIOD,                                          /*   port of CS on Socket #4   */
	GPIOD_CS4,                                      /*   pin of CS    */
	SPI_CR1_BR_0,                                   /*   CR1 register, default value */
};

void DisplayBars(uint8_t nb, bool fill, uint8_t spiNum)
{
	uint8_t data[2];
	uint16_t value = (uint16_t)((1 << (fill ? nb : nb - 1)) - (fill ? 1 : 0));
	
	data[1] = value & 0xFF;
	data[0] = (value >> 8) & 0xFF;
	
	if (spiNum == 1)
	{
		spiSelect(&SPID1);
		spiSend(&SPID1, 2, &data);
		spiUnselect(&SPID1);
	}
	else
	{
		spiSelect(&SPID3);
		spiSend(&SPID3, 2, &data);
		spiUnselect(&SPID3);
	}
}


void TestSPI(uint8_t spiNum)
{
	if (spiNum == 1) spiStart(&SPID1, &spi1_cfg);
	else spiStart(&SPID3, &spi3_cfg);

	while (true)
	{
		for (uint8_t i = 0; i <= 10; i++)
		{
			DisplayBars(i, true, spiNum);
			chThdSleepMilliseconds(50);
		}
		for (uint8_t i = 0; i <= 10; i++)
		{
			DisplayBars(10 - i, true, spiNum);
			chThdSleepMilliseconds(50);
		}
		for (uint8_t i = 0; i <= 10; i++)
		{
			DisplayBars(i, false, spiNum);
			chThdSleepMilliseconds(50);
		}
		for (uint8_t i = 0; i <= 10; i++)
		{
			DisplayBars(10 - i, false, spiNum);
			chThdSleepMilliseconds(50);
		}
	}
}


//  Application entry point.
int main(void)
{
	// HAL initialization, this also initializes the configured device drivers
	// and performs the board-specific initializations.
	halInit();
	
	// The kernel is initialized but not started yet, this means that
	// main() is executing with absolute priority but interrupts are already enabled.
	osKernelInitialize();

#ifdef _USB_
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
#endif

	// Creates the blinker thread, it does not start immediately.
	osThreadCreate(osThread(BlinkerThread), NULL);
	
	// start kernel, after this the main() thread has priority osPriorityNormal by default
	osKernelStart();
	osDelay(200);
	
	TestSPI(3);
	
	//  Normal main() thread activity it does nothing except sleeping in a loop 
	while (true)
	{
		osDelay(1000);
	}
}

