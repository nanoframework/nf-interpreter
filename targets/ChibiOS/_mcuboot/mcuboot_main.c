//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// MCUboot standalone bootloader entry point for STM32 ChibiOS targets
//
// Execution sequence after Reset_Handler:
//   1. startup_stm32f7.s: initialise stack, copy .data, zero .bss, call SystemInit
//   2. SystemInit() (system_stm32f7xx.c): enable FPU, set VTOR, enable caches
//   3. main():
//        a. Initialise internal flash driver
//        b. Initialise board external flash (mcuboot_ext_flash_init)
//        c. Run MCUboot (boot_go)
//        d. Launch the selected image (do_boot)
//
// do_boot() performs the low-level Cortex-M7 image launch:
//   - Set SCB->VTOR to the application vector table address
//   - Load the application's initial MSP from the vector table
//   - Disable all interrupts and clear pending flags
//   - Jump to the application's reset handler
//
// The bootloader never returns from do_boot().  If boot_go() fails (no valid
// image found), the system enters a safe infinite loop

#include <stdint.h>
#include <string.h>

// clang-format off
// Series-specific device header, core header and internal flash driver.
// Add a new elif block when porting to a new STM32 series.
#if defined(STM32F745xx) || defined(STM32F746xx) || defined(STM32F756xx) || \
    defined(STM32F765xx) || defined(STM32F767xx) || defined(STM32F769xx) || \
    defined(STM32F777xx) || defined(STM32F779xx)
#include <stm32f7xx.h>
#include <core_cm7.h>
#include "stm32f7_flash_bare.h"
#else
#error "mcuboot_main.c: unsupported STM32 series — add device header includes for the new series"
#endif
// clang-format on

#include "bootutil/bootutil.h"
#include "bootutil/image.h"

#include "mcuboot_board_iface.h"

// ----------------------------------------------------------------------- //
// do_boot — hand off to the application image selected by MCUboot          //
// ----------------------------------------------------------------------- //

// Structure of the Cortex-M ARM vector table (first two entries)
typedef struct
{
    uint32_t msp;   // Initial Main Stack Pointer
    uint32_t reset; // Reset Handler address
} VectorTable_t;

// clang-format off
// do_boot is series-specific: VTOR relocation, cache disable and interrupt
// clearing differ between Cortex-M cores. Add an elif block for each new series.
#if defined(STM32F745xx) || defined(STM32F746xx) || defined(STM32F756xx) || \
    defined(STM32F765xx) || defined(STM32F767xx) || defined(STM32F769xx) || \
    defined(STM32F777xx) || defined(STM32F779xx)
// clang-format on

__attribute__((noreturn)) static void do_boot(struct boot_rsp *rsp)
{
    // Compute the absolute address of the application vector table
    // boot_rsp.br_hdr is a pointer to the MCUboot image header at the start
    // of the primary slot — the vector table follows the header
    uint32_t img_base = (uint32_t)(uintptr_t)rsp->br_hdr;
    uint32_t vtor_addr = img_base + rsp->br_hdr->ih_hdr_size;

    const VectorTable_t *vt = (const VectorTable_t *)vtor_addr;

    // Relocate the vector table to the application's location
    SCB->VTOR = vtor_addr;
    __DSB();
    __ISB();

    // Load the application's initial MSP
    __set_MSP(vt->msp);
    __DSB();

    // Disable all interrupts and clear any pending NVIC flags so the
    // application starts with a clean interrupt state
    __disable_irq();
    for (uint32_t i = 0; i < (sizeof(NVIC->ICER) / sizeof(NVIC->ICER[0])); i++)
    {
        NVIC->ICER[i] = 0xFFFFFFFFU;
        NVIC->ICPR[i] = 0xFFFFFFFFU;
    }
    __DSB();
    __ISB();

    // Cast reset handler address to a function pointer and jump
    void (*reset_handler)(void) = (void (*)(void))(vt->reset);
    reset_handler();

    // Unreachable — suppress compiler warning.
    while (1)
    {
    }
}

// clang-format off
#endif
// clang-format on

// ----------------------------------------------------------------------- //
// main                                                                     //
// ----------------------------------------------------------------------- //

int main(void)
{
    // Initialise the internal flash driver (series-specific)
    // clang-format off
#if defined(STM32F745xx) || defined(STM32F746xx) || defined(STM32F756xx) || \
    defined(STM32F765xx) || defined(STM32F767xx) || defined(STM32F769xx) || \
    defined(STM32F777xx) || defined(STM32F779xx)
    stm32f7_flash_init();
#endif
    // clang-format on

    // Initialise the board's external flash device
    // Non-fatal: if the external device fails to initialise the boot will still
    // proceed, but any upgrade requiring the secondary slot will fail gracefully
    (void)mcuboot_ext_flash_init();

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
