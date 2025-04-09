//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright 2020 Silicon Laboratories Inc. All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef SL_SLEEPTIMER_CONFIG_H
#define SL_SLEEPTIMER_CONFIG_H

#include <target_platform.h>

#define SL_SLEEPTIMER_PERIPHERAL_DEFAULT 0
#define SL_SLEEPTIMER_PERIPHERAL_RTCC    1
#define SL_SLEEPTIMER_PERIPHERAL_PRORTC  2
#define SL_SLEEPTIMER_PERIPHERAL_RTC     3
#define SL_SLEEPTIMER_PERIPHERAL_SYSRTC  4
#define SL_SLEEPTIMER_PERIPHERAL_BURTC   5

// <o SL_SLEEPTIMER_PERIPHERAL> Timer Peripheral Used by Sleeptimer
//   <SL_SLEEPTIMER_PERIPHERAL_DEFAULT=> Default (auto select)
//   <SL_SLEEPTIMER_PERIPHERAL_RTCC=> RTCC
//   <SL_SLEEPTIMER_PERIPHERAL_PRORTC=> Radio internal RTC (PRORTC)
//   <SL_SLEEPTIMER_PERIPHERAL_RTC=> RTC
//   <SL_SLEEPTIMER_PERIPHERAL_SYSRTC=> SYSRTC
//   <SL_SLEEPTIMER_PERIPHERAL_BURTC=> Back-Up RTC (BURTC)
// <i> Selection of the Timer Peripheral Used by the Sleeptimer
#define SL_SLEEPTIMER_PERIPHERAL SL_SLEEPTIMER_PERIPHERAL_RTCC

// <q SL_SLEEPTIMER_WALLCLOCK_CONFIG> Enable wallclock functionality
// <i> Enable or disable wallclock functionalities (get_time, get_date, etc).
// this is dependent on the HAL_USE_RTC option in target platform
#if (HAL_USE_RTC == TRUE)
#define SL_SLEEPTIMER_WALLCLOCK_CONFIG 1
#else
#define SL_SLEEPTIMER_WALLCLOCK_CONFIG 0
#endif

// <o SL_SLEEPTIMER_FREQ_DIVIDER> Timer frequency divider
// <i> Default: 1
#define SL_SLEEPTIMER_FREQ_DIVIDER 1

// <q SL_SLEEPTIMER_PRORTC_HAL_OWNS_IRQ_HANDLER> If Radio internal RTC (PRORTC) HAL is used, determines if it owns the
// IRQ handler. Enable, if no wireless stack is used. <i> Default: 0
#define SL_SLEEPTIMER_PRORTC_HAL_OWNS_IRQ_HANDLER 0

// <q SL_SLEEPTIMER_DEBUGRUN> Enable DEBUGRUN functionality on hardware RTC.
// <i> Default: 0
#define SL_SLEEPTIMER_DEBUGRUN 0

#endif // SLEEPTIMER_CONFIG_H
