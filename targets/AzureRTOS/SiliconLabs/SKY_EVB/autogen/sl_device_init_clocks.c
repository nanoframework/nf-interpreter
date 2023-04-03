//
// Copyright (c) .NET Foundation and Contributors
// Copyright 2019 Silicon Laboratories Inc. www.silabs.com
// See LICENSE file in the project root for full license information.
//

#include "sl_device_init_clocks.h"

#include "em_cmu.h"

sl_status_t sl_device_init_clocks(void)
{
    CMU_ClockSelectSet(cmuClock_HF, cmuSelect_USHFRCO);

    CMU_ClockEnable(cmuClock_HFLE, true);
    CMU_ClockEnable(cmuClock_HFPER, true);
    CMU_ClockEnable(cmuClock_GPIO, true);
    CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);
    CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFRCO);
#if defined(_CMU_LFCCLKSEL_MASK)
    CMU_ClockSelectSet(cmuClock_LFC, cmuSelect_LFRCO);
#endif
#if defined(_CMU_LFECLKSEL_MASK)
    CMU_ClockSelectSet(cmuClock_LFE, cmuSelect_LFRCO);
#endif

    return SL_STATUS_OK;
}
