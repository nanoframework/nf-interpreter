//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _NANOHAL_POWER_H_
#define _NANOHAL_POWER_H_ 1

#include <stdbool.h>

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

bool CPU_IsSoftRebootSupported();

#ifdef __cplusplus
}
#endif

#endif //_NANOHAL_POWER_H_
