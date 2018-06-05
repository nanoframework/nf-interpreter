//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <Esp32_os.h>

#include <targetHAL.h>
#include <WireProtocol_ReceiverThread.h>
#include <LaunchCLR.h>

#include <wifi_smart_config.h>


#define BLINK_GPIO 4


// Blink GPIO port(Led) when in NanoBooter loader mode
void Blinker_task(void *pvParameter)
{
  ets_printf("blinker task running\n");
  // Configure the IOMUX register for GPIO on Blinker pin and set to OUTPUT
	gpio_pad_select_gpio(BLINK_GPIO);
	gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

	// TODO: loop until thread receives a request to terminate
	while (1) {
		// Blink off
  //  ets_printf("on");
    gpio_set_level(BLINK_GPIO, 0);
		vTaskDelay( 2000 / portTICK_PERIOD_MS);
		// Blink on
 //   ets_printf("off");
		gpio_set_level(BLINK_GPIO, 1);
		vTaskDelay( 2000 / portTICK_PERIOD_MS);
	}
}

// Main task start point
void main_task(void *pvParameter)
{
  // HAL initialization, this also initializes the configured device drivers
  // and performs the board-specific initializations.
//  halInit(); CMsis-os function

  // the following IF is not mandatory, it's just providing a way for a user to 'force'
  // the board to remain in nanoBooter and not launching nanoCLR

  // if the USER button (blue one) is pressed, skip the check for a valid CLR image and remain in booter
  // the user button in this board has a pull-up resistor so the check has to be inverted
 // if (!palReadPad(GPIOA, GPIOA_BUTTON))
 // {

    // check for valid CLR image in Flash 
    //if(CheckValidCLRImage((uint32_t)&__nanoImage_end__))
    //{
    //  // there seems to be a valid CLR image
    //  // launch nanoCLR
    //  LaunchCLR((uint32_t)&__nanoImage_end__);
    //}
 
 // }

/*
  //  Initializes a serial-over-USB CDC driver.
  sduObjectInit(&SDU1);
  sduStart(&SDU1, &serusbcfg);

  // Activates the USB driver and then the USB bus pull-up on D+.
  // Note, a delay is inserted in order to not have to disconnect the cable after a reset.
  usbDisconnectBus(serusbcfg.usbp);
  chThdSleepMilliseconds(1500);
  usbStart(serusbcfg.usbp, &usbcfg);
  usbConnectBus(serusbcfg.usbp);
  */
  ets_printf("main task started\n");
 
  // create the receiver thread
  ReceiverThread(0);
}

// App_main 
// Called from Esp32 IDF start up code before scheduler starts
//
void  app_main()
{
  initialise_wifi_smart_config();

  // Creates the blinker thread, it does not start immediately.
  xTaskCreatePinnedToCore(&Blinker_task, "Blinker_task", 1200, NULL, 5, NULL, 1);

 // Start the main task pinned to 2nd core
	xTaskCreatePinnedToCore(&main_task, "main_task", 2048, NULL, 5, NULL, 1);
}
