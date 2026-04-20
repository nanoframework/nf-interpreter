//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// mcuboot_serial_port.c — Shared serial recovery stub for the bare-metal
// MCUboot bootloader (STM32 ChibiOS targets).
//
// boot_serial_start() is a PLACEHOLDER. When MCUBOOT_SERIAL=1 and the boot
// button is held at power-on, MCUboot calls boot_serial_detect_pin() (board-
// specific) and then boot_serial_start(). This stub satisfies the linker while
// the full SMP serial stack is deferred to a later implementation step.

#include "mcuboot_serial_port.h"

#if defined(MCUBOOT_SERIAL)

#include <cmsis_compiler.h>

void boot_serial_start(const struct boot_uart_funcs *f)
{
    (void)f;
    mcuboot_uart_init();
    // PLACEHOLDER — full SMP serial recovery not yet implemented.
    // Hardware reset required to recover from this state.
    while (1)
    {
        __NOP();
    }
}

// Weak default no-op — overridden by boards that require hardware UART
// initialisation before entering the recovery loop (e.g. ST_STM32F769I_DISCOVERY).
__attribute__((weak)) void mcuboot_uart_init(void)
{
}

#endif // MCUBOOT_SERIAL
