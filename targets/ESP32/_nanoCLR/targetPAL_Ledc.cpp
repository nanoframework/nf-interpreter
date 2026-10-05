//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <targetPAL_Ledc.h>
#include <freertos/FreeRTOS.h>

struct LedcReservation
{
    Esp32LedcOwner owner;
    ledc_timer_t timer;
};

static LedcReservation s_ledcReservations[LEDC_SPEED_MODE_MAX][LEDC_CHANNEL_MAX] = {};
static portMUX_TYPE s_ledcLock = portMUX_INITIALIZER_UNLOCKED;

bool Esp32_Ledc_Reserve(ledc_mode_t mode, ledc_timer_t timer, ledc_channel_t channel, Esp32LedcOwner owner)
{
    if (mode < 0 || mode >= LEDC_SPEED_MODE_MAX || timer < 0 || timer >= LEDC_TIMER_MAX || channel < 0 ||
        channel >= LEDC_CHANNEL_MAX || owner == Esp32LedcOwner::None)
    {
        return false;
    }

    portENTER_CRITICAL(&s_ledcLock);

    LedcReservation &reservation = s_ledcReservations[mode][channel];
    bool available = reservation.owner == Esp32LedcOwner::None ||
                     (owner == Esp32LedcOwner::Pwm && reservation.owner == owner && reservation.timer == timer);

    // PWM channels may share a timer, but the camera must own its timer exclusively.
    for (int index = 0; available && index < LEDC_CHANNEL_MAX; index++)
    {
        const LedcReservation &other = s_ledcReservations[mode][index];
        if (other.owner != Esp32LedcOwner::None && other.timer == timer &&
            (owner == Esp32LedcOwner::Camera || other.owner == Esp32LedcOwner::Camera))
        {
            available = false;
        }
    }

    if (available)
    {
        reservation.owner = owner;
        reservation.timer = timer;
    }

    portEXIT_CRITICAL(&s_ledcLock);
    return available;
}

void Esp32_Ledc_Release(ledc_mode_t mode, ledc_channel_t channel, Esp32LedcOwner owner)
{
    if (mode < 0 || mode >= LEDC_SPEED_MODE_MAX || channel < 0 || channel >= LEDC_CHANNEL_MAX)
    {
        return;
    }

    portENTER_CRITICAL(&s_ledcLock);
    LedcReservation &reservation = s_ledcReservations[mode][channel];
    if (reservation.owner == owner)
    {
        reservation = {};
    }
    portEXIT_CRITICAL(&s_ledcLock);
}
