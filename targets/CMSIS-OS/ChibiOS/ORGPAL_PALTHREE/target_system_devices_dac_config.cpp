//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <sys_dev_dac_native_target.h>

const NF_PAL_DAC_PORT_PIN_CHANNEL DacPortPinConfig[] = {

    // DAC1, channel 1, GPIO port A, pin 4 
    {1, 1, GPIOA, 4},
};

const int DacChannelCount = ARRAYSIZE(DacPortPinConfig);
