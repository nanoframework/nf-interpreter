//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_v2.h>

uint32_t WakeupReasonStore;

// CPU sleep is not currently implemented in this target
inline void CPU_Sleep(SLEEP_LEVEL_type level, uint64_t wakeEvents)
{
    (void)level;
    (void)wakeEvents;
};

inline bool CPU_IsSoftRebootSupported()
{
    return true;
};

void CPU_SetPowerMode(PowerLevel_type powerLevel)
{
    switch (powerLevel)
    {
        case PowerLevel__Off:
            // gracefully shutdown everything
            nanoHAL_Uninitialize_C(true);

            // take care of platform and target specific taks for powering down
            CPU_SetPowerModePlatform(powerLevel);

            // now let's go with shutdown
            Power_shutdown(0, 0);

            break;

        default:
            // all the other power modes are unsupported here
            break;
    }
}

void CPU_SetPowerModePlatform(PowerLevel_type powerLevel)
{
    // call target specific implementation
    CPU_SetPowerModeTarget(powerLevel);
}

// implemented as weak symbol to be overridden by target implementation
__nfweak void CPU_SetPowerModeTarget(PowerLevel_type powerLevel)
{
    (void)powerLevel;
}
