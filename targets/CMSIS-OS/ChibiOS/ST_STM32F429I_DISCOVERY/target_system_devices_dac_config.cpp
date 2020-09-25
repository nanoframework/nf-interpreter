//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <sys_dev_dac_native_target.h>

const NF_PAL_DAC_PORT_PIN_CHANNEL DacPortPinConfig[] = {

    // DAC1, channel 1, GPIO port A, pin 4
    {1, 1, GPIOA, 4},

    // DAC1, channel 2, GPIO port A, pin 5
    {1, 2, GPIOA, 5},
};

const int DacChannelCount = ARRAYSIZE(DacPortPinConfig);
