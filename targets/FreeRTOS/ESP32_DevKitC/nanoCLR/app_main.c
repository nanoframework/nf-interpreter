//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <Esp32_os.h>

#include <targetHAL.h>
#include <WireProtocol_ReceiverThread.h>
#include <LaunchCLR.h>
#include <string.h>

#include <wifi_smart_config.h>

extern void CLRStartupThread(void const * argument);

// Mutex for GLOBAL_LOCK / GLOBAL_UNLOCK
portMUX_TYPE globalLockMutex = portMUX_INITIALIZER_UNLOCKED;

// These are offsets in the Deployment partition
uint32_t  __deployment_start__ = 0;
uint32_t  __deployment_end__ = 1024*1024 - 1;

// Not used, Well i couldn't find anywhere !
uint32_t __nanoImage_start__ = 0;
uint32_t __nanoImage_end__ = 0;

extern bool Esp32FlashDriver_InitializeDevice(void* context);


void  IRAM_ATTR receiver_task(void *pvParameter)
{
  ReceiverThread( 0);
   
  vTaskDelete(NULL);
}

// Main task start point
void  IRAM_ATTR main_task(void *pvParameter)
{
  CLRStartupThread( 0);
 
  vTaskDelete(NULL);
}

// App_main 
// Called from Esp32 IDF start up code before scheduler starts
//
void  app_main()
{
  // Switch off logging so as not to interfere with WireProtocol over Uart0
  esp_log_level_set("*", ESP_LOG_NONE);         

  initialise_wifi_smart_config();

  Esp32FlashDriver_InitializeDevice(0);
 
 	xTaskCreatePinnedToCore(&receiver_task, "ReceiverThread", 2048, NULL, 6, NULL, 1);
  
  // Start the main task pinned to 2nd core
	xTaskCreatePinnedToCore(&main_task, "main_task", 15000, NULL, 5, NULL, 1);
}
