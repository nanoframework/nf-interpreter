//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// MCUboot bootloader entry point for STM32 ChibiOS targets
//
// Execution sequence after Reset_Handler (ChibiOS crt0_v7m.s):
//   1. crt0_v7m.s: set up stacks, copy .data, zero .bss, call __early_init
//   2. main():
//        a. halInit()  — clock, GPIO, enabled peripheral drivers, board.c
//        b. chSysInit() — start the ChibiOS RT kernel (OSAL for SPI/WSPI/SERIAL)
//        c. Initialise external storage (mcuboot_ext_flash_init or mcuboot_sdcard_init)
//        d. Run MCUboot (boot_go)
//        e. Launch the selected image (do_boot)
//
// do_boot() performs the low-level Cortex-M7 image launch:
//   - Stop SysTick so it cannot fire during handoff
//   - Set SCB->VTOR to the application vector table address
//   - Load the application's initial MSP from the vector table
//   - Disable all interrupts and clear pending flags
//   - Jump to the application's reset handler
//
// The bootloader never returns from do_boot().  If boot_go() fails (no valid
// image found), the system enters a safe infinite loop.

#include <stdint.h>
#include <string.h>

// ChibiOS HAL and RT kernel headers.
// The series device header (stm32f7xx.h / stm32f769xx.h) and CMSIS core
// header (core_cm7.h) are included transitively by hal.h.
#include "hal.h"
#include "ch.h"

#include "bootutil/bootutil.h"
#include "bootutil/image.h"

#include "mcuboot_board_iface.h"

// ----------------------------------------------------------------------- //
// do_boot — hand off to the application image selected by MCUboot          //
// ----------------------------------------------------------------------- //

// Structure of the Cortex-M ARM vector table (first two entries)
typedef struct
{
    uint32_t msp;    // Initial Main Stack Pointer
    uint32_t reset;  // Reset Handler address
} VectorTable_t;

__attribute__((noreturn)) static void do_boot(struct boot_rsp *rsp)
{
    // Compute the absolute address of the application vector table.
    // boot_rsp.br_hdr points to the MCUboot image header; the vector table
    // follows immediately after the header.
    uint32_t img_base = (uint32_t)(uintptr_t)rsp->br_hdr;
    uint32_t vtor_addr = img_base + rsp->br_hdr->ih_hdr_size;

    const VectorTable_t *vt = (const VectorTable_t *)vtor_addr;

    // Stop the SysTick timer before tearing down the ChibiOS RT state so
    // no SysTick interrupt fires during the handoff sequence.
    SysTick->CTRL = 0U;

    // Relocate the vector table to the application's location.
    SCB->VTOR = vtor_addr;
    __DSB();
    __ISB();

    // Load the application's initial MSP.
    __set_MSP(vt->msp);
    __DSB();

    // Disable all interrupts and clear any pending NVIC flags so the
    // application starts with a clean interrupt state.
    __disable_irq();
    for (uint32_t i = 0; i < (sizeof(NVIC->ICER) / sizeof(NVIC->ICER[0])); i++)
    {
        NVIC->ICER[i] = 0xFFFFFFFFU;
        NVIC->ICPR[i] = 0xFFFFFFFFU;
    }
    __DSB();
    __ISB();

    // Cast reset handler address to a function pointer and jump.
    void (*reset_handler)(void) = (void (*)(void))(vt->reset);
    reset_handler();

    // Unreachable — suppress compiler warning.
    while (1)
    {
    }
}

// ----------------------------------------------------------------------- //
// main                                                                     //
// ----------------------------------------------------------------------- //

int main(void)
{
    // Initialise the ChibiOS HAL: configures clocks (via mcuconf.h PLL settings),
    // enables GPIO clocks, initialises each enabled peripheral driver, and calls
    // boardInit() (board.c) to apply the full pin-mux configuration.
    halInit();

    // Start the ChibiOS RT kernel.  The SPI/WSPI/SERIAL HAL drivers rely on the
    // RT OSAL (hal_osal_rt_nil.h) for mutexes and condition-variable primitives;
    // those are available only after chSysInit().
    chSysInit();

    // Initialise the board's external flash device
    // Non-fatal: if the external device fails to initialise the boot will still
    // proceed, but any upgrade requiring the secondary slot will fail gracefully
    (void)mcuboot_ext_flash_init();

#if defined(NF_FEATURE_MCUBOOT_HAS_SDCARD)
    // Initialise the SD card and mount the FatFs filesystem for the secondary slot.
    // Non-fatal: a failed SD card init causes boot_go() to skip external slots
    // and boot the primary slot directly.
    (void)mcuboot_sdcard_init();
#endif

    // Run MCUboot image validation and upgrade logic
    struct boot_rsp rsp;
    if (boot_go(&rsp) != 0)
    {
        // No valid image found — enter a safe infinite loop
        // A debugger or JTAG reset is required to recover
        while (1)
        {
            __BKPT(0);
            __NOP();
        }
    }

    // Launch the selected image
    do_boot(&rsp);

    // Unreachable.
    while (1)
    {
    }
}
