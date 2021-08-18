//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _WIN_DEV_PWM_NATIVE_TARGET_H_
#define _WIN_DEV_PWM_NATIVE_TARGET_H_

#include "Board.h"
#include <ti/drivers/PWM.h>

#include <target_windows_devices_pwm_config.h>
#include <win_dev_pwm_native.h>

/////////////////////////////////////
// Array of PWM handlers           //
// these live in the target folder //
/////////////////////////////////////
extern PWM_Handle PwmPins[PWM_PIN_COUNT];

#endif  //_WIN_DEV_PWM_NATIVE_TARGET_H_
