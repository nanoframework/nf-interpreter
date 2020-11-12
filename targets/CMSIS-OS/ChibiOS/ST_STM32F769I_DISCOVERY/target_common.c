//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_v2.h>
#include "target_board.h"
#include "target_common.h"
#include <platform_target_capabilities.h>

HAL_SYSTEM_CONFIG HalSystemConfig = {
    {true}, // HAL_DRIVER_CONFIG_HEADER Header;

    1, // ConvertCOM_DebugHandle(1),
    0, // ConvertCOM_DebugHandle(0),
    921600,
    0, // STDIO = COM2 or COM1

    {RAM1_MEMORY_StartAddress, RAM1_MEMORY_Size},
    {FLASH1_MEMORY_StartAddress, FLASH1_MEMORY_Size}};

HAL_TARGET_CONFIGURATION g_TargetConfiguration;

// this target can use both JTAG and DFU for updates
inline GET_TARGET_CAPABILITIES(TargetCapabilities_JtagUpdate | TargetCapabilities_DfuUpdate);
inline TARGET_HAS_PROPRITARY_BOOTER(true);

bool LaunchProprietaryBootloader()
{
    //volatile uint32_t DFU_BOOTLOADER_ADDRESS = 0x1FFF0000;

    // disable all interrupts in ChibiOS
    chSysDisable();

    // // Disable RCC, set it to default (after reset) settings
    // // Internal clock, no PLL, etc
    // RCC_DeInit();

    // // enable SYSCFG clock
    // RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    // // disable systick timer and reset it to default values
    // SysTick->CTRL = 0;
    // SysTick->LOAD = 0;
    // SysTick->VAL = 0;

    // SYSCFG->MEMRMP = SYSCFG_MemoryRemap_SystemFlash;

    // // clear any pending interrupts to make sure we are jumping straight to nanoCLR ResetHandler
    // SCB->ICSR &= SCB_ICSR_PENDSVCLR_Msk;

    // StartApplication(SYSMEM_ADDR);

    return true;
}
