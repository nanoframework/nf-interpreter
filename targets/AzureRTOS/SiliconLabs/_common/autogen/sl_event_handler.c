//
// Copyright (c) .NET Foundation and Contributors
// Copyright 2020 Silicon Laboratories Inc. www.silabs.com
// See LICENSE file in the project root for full license information.
//

#include "sl_event_handler.h"

#include "em_chip.h"
#include "sl_device_init_nvic.h"
#include "sl_board_init.h"
#include "sl_device_init_dcdc.h"
#include "sl_device_init_hfxo.h"
#include "sl_device_init_lfxo.h"
#include "sl_device_init_clocks.h"
#include "sl_device_init_emu.h"
#include "sl_board_control.h"
#include "sl_sleeptimer.h"
#include "gpiointerrupt.h"
#include "sl_uartdrv_instances.h"
#include "sl_iostream_init_usart_instances.h"
#include "sl_iostream_init_instances.h"
#include "sl_simple_led_instances.h"
#include "sl_i2cspm_instances.h"
#include "sl_power_manager.h"

#include <tx_api.h>

void sl_platform_init(void)
{
    CHIP_Init();
    sl_device_init_nvic();
    sl_board_preinit();
    sl_device_init_dcdc();
    sl_device_init_hfxo();
    sl_device_init_lfxo();
    sl_device_init_clocks();
    sl_device_init_emu();
    sl_board_init();
    sl_power_manager_init();
}

void sl_kernel_start(void)
{
    // Enter the ThreadX kernel.
    tx_kernel_enter();
}

void sl_driver_init(void)
{
    GPIOINT_Init();
    // sl_i2cspm_init_instances();
    sl_uartdrv_init_instances();
    sl_simple_led_init_instances();
}

void sl_service_init(void)
{
    sl_board_configure_vcom();
    sl_sleeptimer_init();
    sl_iostream_init_instances();
}

void sl_stack_init(void)
{
}

void sl_internal_app_init(void)
{
}

void sl_platform_process_action(void)
{
}

void sl_service_process_action(void)
{
}

void sl_stack_process_action(void)
{
}

void sl_internal_app_process_action(void)
{
}

void sl_iostream_init_instances(void)
{
    sl_iostream_usart_init_instances();
}
