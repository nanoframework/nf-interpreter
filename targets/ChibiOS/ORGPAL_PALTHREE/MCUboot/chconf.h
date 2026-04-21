//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2006..2015 Giovanni Di Sirio. All rights reserved.
// See LICENSE file in the project root for full license information.
//

// clang-format off

/**
 * @file    chconf.h
 * @brief   ChibiOS RT configuration for the MCUboot bootloader (ORGPAL_PALTHREE).
 *
 * Minimal RT: one thread (main), SysTick for HAL driver timeouts, no RTOS
 * features beyond what the SPI HAL driver OSAL layer requires.
 */

#ifndef CHCONF_H
#define CHCONF_H

#define _CHIBIOS_RT_CONF_
#define _CHIBIOS_RT_CONF_VER_7_0_

/*===========================================================================*/
/* System timers settings.                                                   */
/*===========================================================================*/

#define CH_CFG_ST_RESOLUTION                32
#define CH_CFG_ST_FREQUENCY                 1000
#define CH_CFG_ST_TIMEDELTA                 2

/*===========================================================================*/
/* Kernel parameters and options.                                            */
/*===========================================================================*/

#define CH_CFG_TIME_QUANTUM                 0
#define CH_CFG_MEMCORE_SIZE                 0
#define CH_CFG_NO_IDLE_THREAD               FALSE

/*===========================================================================*/
/* Performance options.                                                      */
/*===========================================================================*/

#define CH_CFG_OPTIMIZE_SPEED               FALSE

/*===========================================================================*/
/* Subsystem options.                                                        */
/*===========================================================================*/

#define CH_CFG_USE_TM                       FALSE
#define CH_CFG_USE_REGISTRY                 FALSE
#define CH_CFG_USE_WAITEXIT                 FALSE
#define CH_CFG_USE_SEMAPHORES               FALSE
#define CH_CFG_USE_SEMAPHORES_PRIORITY      FALSE
#define CH_CFG_USE_MUTEXES                  FALSE
#define CH_CFG_USE_MUTEXES_RECURSIVE        FALSE
#define CH_CFG_USE_CONDVARS                 FALSE
#define CH_CFG_USE_CONDVARS_TIMEOUT         FALSE
#define CH_CFG_USE_EVENTS                   FALSE
#define CH_CFG_USE_EVENTS_TIMEOUT           FALSE
#define CH_CFG_USE_MESSAGES                 FALSE
#define CH_CFG_USE_MESSAGES_PRIORITY        FALSE
#define CH_CFG_USE_MAILBOXES                FALSE
#define CH_CFG_USE_PIPES                    FALSE
#define CH_CFG_USE_DYNAMIC                  FALSE

/*===========================================================================*/
/* Memory allocation options.                                                */
/*===========================================================================*/

#define CH_CFG_USE_MEMCORE                  TRUE
#define CH_CFG_USE_HEAP                     TRUE
#define CH_CFG_USE_MEMPOOLS                 FALSE
#define CH_CFG_USE_OBJ_FIFOS                FALSE
#define CH_CFG_USE_FACTORY                  FALSE
#define CH_CFG_FACTORY_MAX_NAMES_LENGTH     0
#define CH_CFG_FACTORY_OBJECTS_REGISTRY     FALSE
#define CH_CFG_FACTORY_GENERIC_BUFFERS      FALSE
#define CH_CFG_FACTORY_SEMAPHORES           FALSE
#define CH_CFG_FACTORY_MAILBOXES            FALSE
#define CH_CFG_FACTORY_OBJ_FIFOS            FALSE
#define CH_CFG_FACTORY_PIPES                FALSE

/*===========================================================================*/
/* Debug options.                                                            */
/*===========================================================================*/

#define CH_DBG_STATISTICS                   FALSE
#define CH_DBG_SYSTEM_STATE_CHECK           FALSE
#define CH_DBG_ENABLE_CHECKS                FALSE
#define CH_DBG_ENABLE_ASSERTS               FALSE
#define CH_DBG_TRACE_MASK                   CH_DBG_TRACE_MASK_DISABLED
#define CH_DBG_TRACE_BUFFER_SIZE            128
#define CH_DBG_ENABLE_STACK_CHECK           FALSE
#define CH_DBG_FILL_THREADS                 FALSE

/*===========================================================================*/
/* Kernel hooks.                                                             */
/*===========================================================================*/

#define CH_CFG_THREAD_EXTRA_FIELDS
#define CH_CFG_THREAD_INIT_HOOK(tp)         {}
#define CH_CFG_THREAD_EXIT_HOOK(tp)         {}
#define CH_CFG_CONTEXT_SWITCH_HOOK(ntp, otp) {}
#define CH_CFG_IRQ_PROLOGUE_HOOK()          {}
#define CH_CFG_IRQ_EPILOGUE_HOOK()          {}
#define CH_CFG_IDLE_ENTER_HOOK()            {}
#define CH_CFG_IDLE_LEAVE_HOOK()            {}
#define CH_CFG_IDLE_LOOP_HOOK()             {}
#define CH_CFG_SYSTEM_TICK_HOOK()           {}
#define CH_CFG_SYSTEM_HALT_HOOK(reason)     {}
#define CH_CFG_TRACE_HOOK(tep)              {}
#define CH_CFG_RUNTIME_BRANCHES_ENABLED     FALSE

#endif /* CHCONF_H */
