//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef SYS_DEV_PWM_NATIVE_TARGET_H
#define SYS_DEV_PWM_NATIVE_TARGET_H

#include <sys_dev_pwm_native.h>

#include <targetPAL.h>
#include <sys_dev_pwm_native.h>

#include <em_device.h>
#include <em_cmu.h>
#include <em_emu.h>
#include <em_gpio.h>
#include <em_timer.h>

#define GET_ENCODED_PWM_PORT_PIN(pwmPPConfig) (int)pwmPPConfig.gpioPort * 16 + pwmPPConfig.portPin
#define PWM_CHANNEL_COUNT                     (3)
#define PWM_TIMERS_COUNT                      (3)

typedef struct
{
    bool IsRunning;
    bool IsConfigured;
    uint8_t ChannelArray[PWM_CHANNEL_COUNT];
} NF_PAL_PWM_INSTANCE;

typedef struct
{
    uint8_t wtimerIndex;
    uint8_t capCompIndex;
    GPIO_Port_TypeDef gpioPort;
    uint8_t portPin;
    uint8_t capCompLocation;
} NF_PAL_PWM_PORT_PIN_CONFIG;

extern const NF_PAL_PWM_PORT_PIN_CONFIG PwmPortPinConfig[];
extern const int PwmConfigCount;
// using the 1st three WTIMER instances for PWM
extern NF_PAL_PWM_INSTANCE PwmInstances[PWM_TIMERS_COUNT];

#endif // SYS_DEV_PWM_NATIVE_TARGET_H
