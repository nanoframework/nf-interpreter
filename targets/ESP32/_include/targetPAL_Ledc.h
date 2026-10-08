//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#pragma once

#include <driver/ledc.h>

enum class Esp32LedcOwner
{
    None,
    Pwm,
    Camera
};

bool Esp32_Ledc_Reserve(ledc_mode_t mode, ledc_timer_t timer, ledc_channel_t channel, Esp32LedcOwner owner);
void Esp32_Ledc_Release(ledc_mode_t mode, ledc_channel_t channel, Esp32LedcOwner owner);
