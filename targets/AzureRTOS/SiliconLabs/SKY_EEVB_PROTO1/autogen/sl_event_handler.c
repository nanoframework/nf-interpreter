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

void sl_platform_init(void)
{
    CHIP_Init();
    sl_device_init_nvic();
    sl_board_preinit();
    sl_device_init_dcdc();
    //sl_device_init_hfxo();
    sl_device_init_hfrco();
    //sl_device_init_lfxo();
    sl_device_init_lfrco();
    sl_device_init_clocks();
    sl_device_init_emu();
    sl_board_init();
    sl_power_manager_init();
}
