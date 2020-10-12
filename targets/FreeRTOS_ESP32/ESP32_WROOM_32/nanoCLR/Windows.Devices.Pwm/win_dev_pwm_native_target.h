//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _WIN_DEV_PWM_NATIVE_TARGET_H_
#define _WIN_DEV_PWM_NATIVE_TARGET_H_

#include <win_dev_pwm_native.h>

enum PwmPulsePolarity
{
        /// <summary>
        /// Configures the PWM signal to start in the active high state.
        /// </summary>
        ActiveHigh,
        /// <summary>
        /// Configures the PWM signal to start in the active low state.
        /// </summary>
        ActiveLow
};

#endif  //_WIN_DEV_PWM_NATIVE_TARGET_H_
