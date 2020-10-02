//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_Watchdog.h>

/**
* Method to initialize the watchdog. Watchdog must be initialized 
* only once (when the system starts, generally in main after system and HAL initialization).
*/
__nfweak void Watchdog_Init(){}


/**
 * Reset the watchdog. If not done within the timout period,
 * the watchdog will trigger and MCU will reset. Specific implementation
 * must provide required thread-safety (e.g. whether called from thread or an 
 * ISR, reentrant/non-reentrant)
 */ 
__nfweak void Watchdog_Reset(){}

