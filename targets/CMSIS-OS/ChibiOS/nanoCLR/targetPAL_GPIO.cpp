//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL_GPIO.h>
#include <nanoPAL.h>
#include <target_platform.h>
#include <targetPAL.h>
#include <hal.h>

#if (HAL_USE_EXT == TRUE)
EXTConfig extInterruptsConfiguration;
#endif

void GPIO_Initialize()
{

    // the initialization for this structure has to be made here and it's part of the target because it can have different sizes according to the device
#if (HAL_USE_EXT == TRUE)
    extInterruptsConfiguration = { .channels = { {EXT_CH_MODE_DISABLED, NULL} }};
#endif

}

void GPIO_Uninitialize()
{

}
