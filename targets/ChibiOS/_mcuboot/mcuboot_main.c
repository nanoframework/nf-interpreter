//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// MCUboot bootloader entry point for STM32 ChibiOS targets
//
// Execution sequence after Reset_Handler:
//   1. set up stacks, copy .data, zero .bss, call __early_init, etc.
//   2. main():
//        a. halInit()  — clock, GPIO, enabled peripheral drivers, board.c
//        b. chSysInit() — start the ChibiOS RT kernel (OSAL for SPI/WSPI/SERIAL)
//        c. Initialise external storage (mcuboot_ext_flash_init or mcuboot_sdcard_init)
//        d. Check recovery button and run SMP serial recovery if pressed (mcuboot_serial_recovery_try)
//           If recovery is triggered, this never returns (device resets after image upload)
//        e. Run MCUboot (boot_go) — only if recovery was not triggered
//        f. Launch the selected image (do_boot)
//
// do_boot() performs the low-level Cortex image launch:
//   - Stop SysTick so it cannot fire during handoff
//   - Set SCB->VTOR to the application vector table address
//   - Load the application's initial MSP from the vector table
//   - Disable all interrupts and clear pending flags
//   - Jump to the application's reset handler
//
// The bootloader never returns from do_boot().  If boot_go() fails (no valid
// image found), the system enters SMP serial recovery (when MCUBOOT_SERIAL is
// defined) so firmware can be uploaded over SMP, or halts if serial recovery
// is unavailable.

#include <stdint.h>
#include <string.h>

// ChibiOS HAL and RT kernel headers.
#include <hal.h>
#include <ch.h>
#include <vectors.h>

#include "bootutil/bootutil.h"
#include "bootutil/image.h"

#include "mcuboot_board_iface.h"
#include "mcuboot_serial_port.h"

#if defined(CONFIG_NF_FEATURE_WATCHDOG) && CONFIG_NF_FEATURE_WATCHDOG
#include <targetHAL_Watchdog.h>
#endif

// ----------------------------------------------------------------------- //
// do_boot — hand off to the application image selected by MCUboot          //
// ----------------------------------------------------------------------- //

static void do_boot(struct boot_rsp *rsp)
{
    // Compute the absolute address of the application vector table.
    // br_image_off is the real flash offset of the image;
    uint32_t vtor_addr = rsp->br_image_off + rsp->br_hdr->ih_hdr_size;

    // function pointer to load nanoCLR ResetHandler address
    irq_vector_t JumpToNanoCLR;

    // load nanoCLR vector table
    const vectors_t *nanoCLRVectorTable = (vectors_t *)vtor_addr;

    // load the jump address with the nanoCLR ResetHandler address
    JumpToNanoCLR = nanoCLRVectorTable->reset_handler;

    // disable all interrupts in ChibiOS
    chSysDisable();

    // clear any pending interrupts to make sure we are jumping straight to nanoCLR ResetHandler
    SCB->ICSR = SCB_ICSR_PENDSTCLR_Msk | SCB_ICSR_PENDSVCLR_Msk;

    // set VTOR to point to nanoCLR vector table so interrupts use the correct handlers
    SCB->VTOR = vtor_addr;

    SCB->SHCSR |= SCB_SHCSR_USGFAULTENA_Msk | SCB_SHCSR_BUSFAULTENA_Msk | SCB_SHCSR_MEMFAULTENA_Msk;

    // need to set stack pointer from CLR vector table
    __set_MSP((uint32_t)nanoCLRVectorTable->init_stack);

    // make the jump to nanoCLR, at last
    JumpToNanoCLR();
}

// ----------------------------------------------------------------------- //
// main                                                                     //
// ----------------------------------------------------------------------- //

int main(void)
{
    // Initialise the ChibiOS HAL
    halInit();

    // Start the ChibiOS RT kernel.
    chSysInit();

#if defined(CONFIG_NF_FEATURE_WATCHDOG) && CONFIG_NF_FEATURE_WATCHDOG
    Watchdog_Init();
#endif

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

#if defined(MCUBOOT_SERIAL)
    // Check recovery button and - if held - run the SMP serial recovery loop.
    // If the button is not pressed, returns immediately and boot continues.
    mcuboot_serial_recovery_try();
#endif

    // Run MCUboot image validation and upgrade logic
    struct boot_rsp rsp;
    if (boot_go(&rsp) != 0)
    {
#if defined(MCUBOOT_SERIAL)
        // No valid image found - enter SMP serial recovery
        mcuboot_serial_recovery_start();
#else
        // No valid image and no serial recovery available: halt.
        while (1)
        {
            __BKPT(0);
            __NOP();
        }
#endif
    }

    // Launch the selected image
    do_boot(&rsp);

    // Unreachable.
    while (1)
    {
    }
}
