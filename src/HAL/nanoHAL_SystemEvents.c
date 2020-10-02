//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_v2.h>
#include <nanoWeak.h>
#include <targetHAL.h>

volatile int32_t SystemStates[SYSTEM_STATE_TOTAL_STATES];

// the functions below are declared as weak so they can be replaced at target/platform level if needed

__nfweak void SystemState_SetNoLock(SYSTEM_STATE_type state)
{
    SystemStates[state]++;
}

__nfweak void SystemState_ClearNoLock(SYSTEM_STATE_type state)
{
    SystemStates[state]--;
}

__nfweak bool SystemState_QueryNoLock(SYSTEM_STATE_type state)
{
    return (SystemStates[state] > 0) ? true : false;
}

__nfweak void SystemState_Set(SYSTEM_STATE_type state)
{
#ifdef __CM0_CMSIS_VERSION
    GLOBAL_LOCK();
    SystemState_SetNoLock(state);
    GLOBAL_UNLOCK();
#else
    __atomic_fetch_add(&SystemStates[state], 1, __ATOMIC_RELAXED);
#endif
}

__nfweak void SystemState_Clear(SYSTEM_STATE_type state)
{
#ifdef __CM0_CMSIS_VERSION
    GLOBAL_LOCK();
    SystemState_ClearNoLock(state);
    GLOBAL_UNLOCK();
#else
    __atomic_fetch_sub(&SystemStates[state], 1, __ATOMIC_RELAXED);
#endif
}

__nfweak bool SystemState_Query(SYSTEM_STATE_type state)
{
    return __atomic_load_n(&state, __ATOMIC_RELAXED) ? true : false;
}
