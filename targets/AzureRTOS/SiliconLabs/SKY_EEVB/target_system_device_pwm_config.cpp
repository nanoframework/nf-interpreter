//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <sys_dev_pwm_native_target.h>

const NF_PAL_PWM_PORT_PIN_CONFIG PwmPortPinConfig[] = {

    // using WTIMER0, CC0, PC1, location 7
    {0, 0, gpioPortC, 1, 7},
    // using WTIMER1, CC2, PI1, location 5
    {1, 2, gpioPortI, 1, 5},

};

const int PwmConfigCount = ARRAYSIZE(PwmPortPinConfig);
