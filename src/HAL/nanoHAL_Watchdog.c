//
// Copyright (c) 2018 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_Watchdog.h>

/**
* Method to initialize the watchdog. Watchdog must be initialized 
* only one (when the system starts, generally in main after system and HAL initialization).
*/
__nfweak void Watchdog_Init(){}

/**
 * Start the watchdog. 
 * @return  true when watchdog started successfully else false
 */ 
__nfweak bool Watchdog_Start(){return false;}

/**
 * Stop the watchdog
 */  
__nfweak void Watchdog_Stop(){}
/**
 * Reset the watchdog. If not done without the timout period,
 * the watchdog will trigger and MCU will reset
 */ 
__nfweak void Watchdog_Reset(){}
/**
 * Check if watchdog is running
 * @return bool
 */ 
__nfweak bool Watchdog_IsRunning() {return false;}
/**
 * Set the timeout required for the watchdog trigger.
 * This must be called before the watchdog is started
 * @param   timeoutMillis   The timeout in milliseconds
 */ 
__nfweak void Watchdog_SetTimeout(uint16_t timeoutMillis){};
/**
 * Set the maximum timeout for the platform
 */ 
__nfweak void Watchdog_SetMaxTimeout(){}; 