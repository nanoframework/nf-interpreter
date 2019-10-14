//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

/**
 * @file    GPTM/hal_st_lld.h
 * @brief   ST Driver subsystem low level driver header.
 * @details This header is designed to be include-able without having to
 *          include other files from the HAL.
 *
 * @addtogroup ST
 * @{
 */

#ifndef HAL_ST_LLD_H
#define HAL_ST_LLD_H

#include "mcuconf.h"
#include "ticc3200_registry.h"

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @name    Configuration options
 * @{
 */

/**
 * @brief   SysTick timer IRQ priority.
 */
#if !defined(TICC3200_ST_IRQ_PRIORITY) || defined(__DOXYGEN__)
#define TICC3200_ST_IRQ_PRIORITY                2
#endif

/**
 * @brief   GPTx unit (by number) to be used for free running operations.
 * @note    You must select a 32 bits timer if a 32 bits @p systick_t type
 *          is required.
 */
#if !defined(TICC3200_ST_TIMER_NUMBER) || defined(__DOXYGEN__)
#define TICC3200_ST_TIMER_NUMBER                0
#endif

/**
 * @brief   When set to @p TRUE a wide timer is used. When set to @p FALSE a
 *          normal timer is used.
 */
#if !defined(TICC3200_ST_USE_WIDE_TIMER) || defined(__DOXYGEN__)
#define TICC3200_ST_USE_WIDE_TIMER              TRUE
#endif

/**
 * @}
 */

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

#if TICC3200_ST_TIMER_NUMBER == 0
#if !TICC3200_HAS_GPT0
#error "GPT0 not present"
#endif
#define TICC3200_ST_TIM                         TIMERA0_BASE

#elif TICC3200_ST_TIMER_NUMBER == 1
#if !TICC3200_HAS_GPT1
#error "GPT1 not present"
#endif
#define TICC3200_ST_TIM                         TIMERA1_BASE

#elif TICC3200_ST_TIMER_NUMBER == 2
#if !TICC3200_HAS_GPT2
#error "GPT2 not present"
#endif
#define TICC3200_ST_TIM                         TIMERA2_BASE

#elif TICC3200_ST_TIMER_NUMBER == 3
#if !TICC3200_HAS_GPT3
#error "GPT3 not present"
#endif
#define TICC3200_ST_TIM                         TIMERA3_BASE

#else
#error "TICC3200_ST_TIMER_NUMBER specifies an unsupported timer"
#endif

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
  void st_lld_init(void);
#ifdef __cplusplus
}
#endif

/*===========================================================================*/
/* Driver inline functions.                                                  */
/*===========================================================================*/

/**
 * @brief   Returns the time counter value.
 *
 * @return              The counter value.
 *
 * @notapi
 */
static inline systime_t st_lld_get_counter(void)
{
  return (systime_t) (((systime_t) 0xffffffff) - HWREG(TICC3200_ST_TIM + TIMER_O_TAV));
}

/**
 * @brief   Starts the alarm.
 * @note    Makes sure that no spurious alarms are triggered after
 *          this call.
 *
 * @param[in] time      the time to be set for the first alarm
 *
 * @notapi
 */
static inline void st_lld_start_alarm(systime_t time)
{
  HWREG(TICC3200_ST_TIM + TIMER_O_TAMATCHR) = (systime_t) (((systime_t) 0xffffffff) - time);
  HWREG(TICC3200_ST_TIM + TIMER_O_ICR) = HWREG(TICC3200_ST_TIM + TIMER_O_MIS);
  HWREG(TICC3200_ST_TIM + TIMER_O_IMR) = TIMER_IMR_TAMIM;
}

/**
 * @brief   Stops the alarm interrupt.
 *
 * @notapi
 */
static inline void st_lld_stop_alarm(void)
{
  HWREG(TICC3200_ST_TIM + TIMER_O_IMR) = 0;
}

/**
 * @brief   Sets the alarm time.
 *
 * @param[in] time      the time to be set for the next alarm
 *
 * @notapi
 */
static inline void st_lld_set_alarm(systime_t time)
{
  HWREG(TICC3200_ST_TIM + TIMER_O_TAMATCHR) = (systime_t) (((systime_t) 0xffffffff) - time);
}

/**
 * @brief   Returns the current alarm time.
 *
 * @return              The currently set alarm time.
 *
 * @notapi
 */
static inline systime_t st_lld_get_alarm(void)
{
  return (systime_t) (((systime_t)0xffffffff) - HWREG(TICC3200_ST_TIM + TIMER_O_TAMATCHR));
}

/**
 * @brief   Determines if the alarm is active.
 *
 * @return              The alarm status.
 * @retval false        if the alarm is not active.
 * @retval true         is the alarm is active
 *
 * @notapi
 */
static inline bool st_lld_is_alarm_active(void)
{
  return (bool) ((HWREG(TICC3200_ST_TIM + TIMER_O_IMR) & TIMER_IMR_TAMIM) !=0);
}

#endif /* HAL_ST_LLD_H */

/**
 * @}
 */
