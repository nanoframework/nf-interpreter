//
// Copyright (c) .NET Foundation and Contributors
// Copyright 2019 Silicon Laboratories Inc. www.silabs.com
// See LICENSE file in the project root for full license information.
//

#include "sl_device_init_clocks.h"

#include "em_cmu.h"

sl_status_t sl_device_init_clocks(void)
{
    CMU_CLOCK_SELECT_SET(HF, USHFRCO);

    CMU_ClockEnable(cmuClock_HFLE, true);
    CMU_ClockEnable(cmuClock_HFPER, true);
    CMU_ClockEnable(cmuClock_GPIO, true);
    CMU_CLOCK_SELECT_SET(LFA, LFRCO);
    CMU_CLOCK_SELECT_SET(LFB, LFRCO);
#if defined(_CMU_LFCCLKSEL_MASK)
    CMU_CLOCK_SELECT_SET(LFC, LFRCO);
#endif
#if defined(_CMU_LFECLKSEL_MASK)
    CMU_CLOCK_SELECT_SET(LFE, LFRCO);
#endif

    return SL_STATUS_OK;
}
