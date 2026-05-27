//
// Copyright (c) .NET Foundation and Contributors
// Copyright 2019 Silicon Laboratories Inc. www.silabs.com
// See LICENSE file in the project root for full license information.
//

#include "sl_device_init_clocks.h"

#include "em_cmu.h"
#include <nanoWeak.h>

// implemented as weak function to allow overriding at platform level
__nfweak sl_status_t sl_device_init_clocks(void)
{
    CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

    CMU_ClockEnable(cmuClock_HFLE, true);
    CMU_ClockEnable(cmuClock_HFPER, true);
    CMU_ClockEnable(cmuClock_GPIO, true);
    CMU_CLOCK_SELECT_SET(LFA, LFXO);
    CMU_CLOCK_SELECT_SET(LFB, LFXO);
#if defined(_CMU_LFCCLKSEL_MASK)
    CMU_CLOCK_SELECT_SET(LFC, LFXO);
#endif
#if defined(_CMU_LFECLKSEL_MASK)
    CMU_CLOCK_SELECT_SET(LFE, LFXO);
#endif

    return SL_STATUS_OK;
}
