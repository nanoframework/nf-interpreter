//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_Power.h>
#include <nanoHAL_v2.h>
#include <target_platform.h>
#include <target_common.h>

#include <mxc_device.h>

#ifdef HAL_RTC_MODULE_ENABLED
extern RTC_HandleTypeDef RtcHandle;
#endif

uint32_t WakeupReasonStore;

inline void CPU_Reset()
{
    NVIC_SystemReset();
};

inline bool CPU_IsSoftRebootSupported()
{
    return true;
};

// CPU sleep is not currently implemented in this target
inline void CPU_Sleep(SLEEP_LEVEL_type level, uint64_t wakeEvents)
{
    (void)level;
    (void)wakeEvents;
};

void CPU_SetPowerMode(PowerLevel_type powerLevel)
{
    // default to false
    // bool success = false;

    switch (powerLevel)
    {
        case PowerLevel__Off:
            // stop watchdog
            // TODO
            // wdgStop(&WDGD1);

            // gracefully shutdown everything
            nanoHAL_Uninitialize_C(true);

            // chSysDisable();
            // disable interrupts

            /////////////////////////////////////////////////////////////////////////
            // stop the idependent watchdog, for series where the option is available

            /////////////////////////////////////////////////////
            // set alarm interrupt enable
            // set power control register to: power down deep sleep
            /////////////////////////////////////////////////////

            // TODO
            // need review here to use ST HAL HAL_PWREx_EnterSTOP2Mode

            // set SLEEPDEEP bit of Cortex SCR
            SCB->SCR |= (uint32_t)SCB_SCR_SLEEPDEEP_Msk;

            // wait for interrupt, and the execution dies here
            __WFI();

            break;

        default:
            // all the other power modes are unsupported here
            break;
    }
}
