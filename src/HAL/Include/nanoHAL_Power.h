//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _NANOHAL_POWER_H_
#define _NANOHAL_POWER_H_ 1

#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum PowerLevel
{
    PowerLevel__Active,
    PowerLevel__SelectiveOff,
    PowerLevel__Sleep,
    PowerLevel__DeepSleep,
    PowerLevel__Off
}PowerLevel_type;


// this is used to store the CPU wakeup reason
// a target implementation can use it or not
// if it's used suggest to add the variable at targetHAL_Power.c 
extern uint32_t WakeupReasonStore;

bool CPU_IsSoftRebootSupported();

#ifdef __cplusplus
}
#endif

#endif //_NANOHAL_POWER_H_
