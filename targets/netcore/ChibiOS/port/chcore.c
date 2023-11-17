/*
    ChibiOS - Copyright (C) 2006,2007,2008,2009,2010,2011,2012,2013,2014,
              2015,2016,2017,2018,2019,2020,2021 Giovanni Di Sirio.

    This file is part of ChibiOS.

    ChibiOS is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation version 3 of the License.

    ChibiOS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    SIMIA32/chcore.c
 * @brief   Simulator on IA32 port code.
 *
 * @addtogroup SIMIA32_GCC_CORE
 * @{
 */

#if defined(_WIN32)
#include <windows.h>
#else
#include <sys/time.h>
#endif

#include <ch.h>
#include <intrin.h>

/*===========================================================================*/
/* Module local definitions.                                                 */
/*===========================================================================*/

/*
 * RTOS-specific context offset.
 */
#if defined(_CHIBIOS_RT_CONF_)
#define CONTEXT_OFFSET "32"

#elif defined(_CHIBIOS_NIL_CONF_)
#define CONTEXT_OFFSET "0"

#else
#error "invalid chconf.h"
#endif

/*===========================================================================*/
/* Module exported variables.                                                */
/*===========================================================================*/

bool port_isr_context_flag;
syssts_t port_irq_sts;

/*===========================================================================*/
/* Module local types.                                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Module local variables.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Module local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Module exported functions.                                                */
/*===========================================================================*/

// port_switch has to be implemented on asm file, because of MSVC for x64 not supporting "inline" asm

/**
 * @brief   Start a thread by invoking its work function.
 * @details If the work function returns @p chThdExit() is automatically
 *          invoked.
 */
__declspec(noreturn) void __cdecl _port_thread_start(msg_t (*pf)(void *), void *p)
{
    chSysUnlock();
    pf(p);
    chThdExit(0);
    while (1)
        ;
}

/**
 * @brief   Returns the current value of the realtime counter.
 *
 * @return              The realtime counter value.
 */
rtcnt_t port_rt_get_counter_value(void)
{
#if defined(WIN32)
    LARGE_INTEGER n;

    QueryPerformanceCounter(&n);

    return (rtcnt_t)(n.QuadPart / 1000LL);
#else
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return ((rtcnt_t)tv.tv_sec * (rtcnt_t)1000000) + (rtcnt_t)tv.tv_usec;
#endif
}

/** @} */
