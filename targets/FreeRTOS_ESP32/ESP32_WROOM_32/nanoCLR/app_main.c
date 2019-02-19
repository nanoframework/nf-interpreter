//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <Esp32_os.h>

#include <targetHAL.h>
#include <nanoCLR_Application.h>
#include <WireProtocol_ReceiverThread.h>
#include <LaunchCLR.h>
#include <string.h>

extern void CLRStartupThread(void const * argument);

// Mutex for GLOBAL_LOCK / GLOBAL_UNLOCK
portMUX_TYPE globalLockMutex = portMUX_INITIALIZER_UNLOCKED;

void receiver_task(void *pvParameter)
{
  (void)pvParameter;

  ReceiverThread( 0);
   
  vTaskDelete(NULL);
}

// Main task start point
void main_task(void *pvParameter)
{
  (void)pvParameter;
  
  // CLR settings to launch CLR thread
  CLR_SETTINGS clrSettings;
  (void)memset(&clrSettings, 0, sizeof(CLR_SETTINGS));

  clrSettings.MaxContextSwitches         = 50;
  clrSettings.WaitForDebugger            = false;
  clrSettings.EnterDebuggerLoopAfterExit = true;

  CLRStartupThread(&clrSettings);
 
  vTaskDelete(NULL);
}

// App_main 
// Called from Esp32 IDF start up code before scheduler starts
//
void  app_main()
{
  // Switch off logging so as not to interfere with WireProtocol over Uart0
  esp_log_level_set("*", ESP_LOG_NONE);         

  ESP_ERROR_CHECK( nvs_flash_init() );
 
 	xTaskCreatePinnedToCore(&receiver_task, "ReceiverThread", 2048, NULL, 5, NULL, 1);
  
  // Start the main task pinned to 2nd core
	xTaskCreatePinnedToCore(&main_task, "main_task", 15000, NULL, 5, NULL, 1);
}
