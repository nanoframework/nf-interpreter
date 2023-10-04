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
#include "sl_device_init_hfrco.h"
#include "sl_device_init_lfrco.h"
#include "sl_device_init_clocks.h"
#include "sl_device_init_emu.h"
#include "sl_board_control.h"
#include "sl_sleeptimer.h"
#include "gpiointerrupt.h"
#include "sl_uartdrv_instances.h"
#include "sl_iostream_init_usart_instances.h"
#include "sl_iostream_init_instances.h"
#include "sl_i2cspm_instances.h"
#include "sl_power_manager.h"

#include <tx_api.h>
#include <nanoCLR_Headers.h>

extern void InitGpCrc(void);

// implemented as weak function to allow overriding at platform level
// DEVELOPER NOTE: when implementing this function, make sure to call ALL the functions there
// except for the ones that aren't required for your target
__nfweak void sl_platform_init(void)
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
    InitGpCrc();
}

// implemented as weak function to allow overriding at platform level
__nfweak void sl_kernel_start(void)
{
    // Enter the ThreadX kernel.
    tx_kernel_enter();
}

// implemented as weak function to allow overriding at platform level
__nfweak void sl_driver_init(void)
{
    sl_i2cspm_init_instances();

#if HAL_WP_USE_SERIAL == TRUE
    sl_uartdrv_init_instances();
#endif

    sl_driver_init_target();
}

// implemented as weak function to allow overriding at platform level
__nfweak void sl_service_init(void)
{
    sl_board_configure_vcom();
    sl_sleeptimer_init();

#if HAL_WP_USE_SERIAL == TRUE
    sl_iostream_init_instances();
#endif
}

// implemented as weak function to allow overriding at platform level
__nfweak void sl_stack_init(void)
{
    sl_stack_init_target();
}

// implemented as weak function to allow overriding at platform level
__nfweak void sl_internal_app_init(void)
{
}

// implemented as weak function to allow overriding at platform level
__nfweak void sl_platform_process_action(void)
{
}

// implemented as weak function to allow overriding at platform level
__nfweak void sl_service_process_action(void)
{
}

void sl_stack_process_action(void)
{
}

// implemented as weak function to allow overriding at platform level
__nfweak void sl_internal_app_process_action(void)
{
}

// implemented as weak function to allow overriding at platform level
__nfweak void sl_iostream_init_instances(void)
{
#if HAL_WP_USE_SERIAL == TRUE
    sl_iostream_usart_init_instances();
#endif
}

// provided as weak so it can be replaced at target level
__nfweak void sl_stack_init_target(void)
{
}

// provided as weak so it can be replaced at target level
__nfweak void sl_driver_init_target(void)
{
}
