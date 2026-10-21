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
//        c. Initialise the external flash holding the secondary slots (mcuboot_ext_flash_init),
//           retried a few times; a failure is fatal and halts the bootloader
//        d. Check recovery button and run SMP serial recovery if pressed (mcuboot_serial_recovery_try),
//           unless an update is in flight (mcuboot_update_in_progress)
//           If recovery is triggered, this never returns (device resets after image upload)
//        e. Sweep removable update media (SD card / USB MSD) for signed images and stage
//           them into the secondary slots (mcuboot_media_import_run), when enabled
//        f. Run MCUboot (two boot_go_for_image_id passes, nanoCLR then deployment) — only if
//           recovery was not triggered
//        g. Launch the selected image (do_boot)
//
// do_boot() performs the low-level Cortex image launch:
//   - Stop SysTick so it cannot fire during handoff
//   - Set SCB->VTOR to the application vector table address
//   - Load the application's initial MSP from the vector table
//   - Disable all interrupts and clear pending flags
//   - Jump to the application's reset handler
//
// The bootloader never returns from do_boot().  If the nanoCLR pass fails (no valid
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
#include "bootutil/bootutil_log.h"

#include "mcuboot_board_iface.h"
#include "mcuboot_serial_port.h"

#include <MCUboot_UpdateGuard.h>

#if (defined(CONFIG_NF_FEATURE_MCUBOOT_HAS_SDCARD) && CONFIG_NF_FEATURE_MCUBOOT_HAS_SDCARD) ||                         \
    (defined(CONFIG_NF_FEATURE_MCUBOOT_HAS_USB_MSD) && CONFIG_NF_FEATURE_MCUBOOT_HAS_USB_MSD)
#define NF_MCUBOOT_MEDIA_IMPORT 1
#include <mcuboot_media_import.h>
#endif

// Image indices (convention: 0 = nanoCLR, 1 = deployment).
// Mirrors MCUboot_RuntimeInterface.h, which is a nanoCLR-side header the bootloader
// does not include.
#define NF_MCUBOOT_IMAGE_CLR    0
#define NF_MCUBOOT_IMAGE_DEPLOY 1

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
// external flash init                                                      //
// ----------------------------------------------------------------------- //

// Attempts and pause between them: rides out a transient fault on the bus or the supply
#define NF_MCUBOOT_EXT_FLASH_INIT_ATTEMPTS 3
#define NF_MCUBOOT_EXT_FLASH_RETRY_MS      20

// Quiescent fatal stop: never touches flash, never recovers, never boots.
// The main thread only sleeps, so the heartbeat keeps running in the fatal cadence. The watchdog
// (when enabled) is not fed here, so it resets the device and the init is tried again.
static __attribute__((noreturn)) void fatal_halt(const char *reason)
{
    // the heartbeat is only started after a successful init
    mcuboot_heartbeat_start();
    mcuboot_heartbeat_set_pattern(MCUBOOT_HEARTBEAT_FATAL);

    BOOT_LOG_ERR("FATAL: %s", reason);

    while (1)
    {
        chThdSleepMilliseconds(500);
    }
}

// Returns true when the external flash is ready, or the board has none by design.
static bool ext_flash_init_with_retry(void)
{
    for (int attempt = 0; attempt < NF_MCUBOOT_EXT_FLASH_INIT_ATTEMPTS; attempt++)
    {
        int result = mcuboot_ext_flash_init();

        if (result == MCUBOOT_EXT_FLASH_OK || result == MCUBOOT_EXT_FLASH_NOT_FITTED)
        {
            return true;
        }

        BOOT_LOG_ERR("External flash init failed (%d), attempt %d", result, attempt + 1);

        chThdSleepMilliseconds(NF_MCUBOOT_EXT_FLASH_RETRY_MS);
    }

    return false;
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

    // Initialise the board's external flash device, which holds the secondary slots.
    // Fatal on failure: without them an interrupted swap can be neither resumed nor reverted
    if (!ext_flash_init_with_retry())
    {
        fatal_halt("external flash init failed");
    }

    // Start the LED heartbeat
    mcuboot_heartbeat_start();

#if defined(MCUBOOT_SERIAL)
    // Check recovery button and - if held - run the SMP serial recovery loop, unless an update is
    // in flight: an upload would rewrite the secondary slot under a swap that has to be finished.
    // If the button is not pressed, returns immediately and boot continues.
    if (mcuboot_update_in_progress())
    {
        BOOT_LOG_INF("update in flight, serial recovery disabled for this boot");
    }
    else
    {
        mcuboot_serial_recovery_try();
    }
#endif

#if defined(NF_MCUBOOT_MEDIA_IMPORT)
    // Look for signed update images on removable media
    mcuboot_media_import_run();
#endif

    // Run MCUboot image validation and upgrade logic as two boot_go_for_image_id() passes -
    // image 0 (nanoCLR) first for the actual boot decision, then image 1 (deployment) so a
    // staged swap can complete, masked so a never-provisioned deploy_0 can't veto the boot.
    // The CLR goes first so that a reset during the deployment swap reverts the CLR and leaves
    // the resumed deployment unconfirmed: the old CLR rejects it and it is reverted too. See
    // "Two-Pass Boot Sequence" in MCUboot/docs/multi-image-management.md for the full reasoning.
    struct boot_loader_state *bootState = boot_get_loader_state();
    struct boot_rsp rsp;

    boot_state_init(bootState);
    FIH_DECLARE(bootResult, FIH_FAILURE);
    FIH_CALL(boot_go_for_image_id, bootResult, &rsp, NF_MCUBOOT_IMAGE_CLR);

    if (FIH_NOT_EQ(bootResult, FIH_SUCCESS))
    {
        BOOT_LOG_ERR("Image %d boot pass failed (%d); no bootable nanoCLR", NF_MCUBOOT_IMAGE_CLR, (int)bootResult);

#if defined(MCUBOOT_SERIAL)
        // No valid image found - enter SMP serial recovery. This is deliberately not gated: with no
        // bootable nanoCLR, recovery is the only way out. The nanoCLR pass has already completed
        // any swap of image 0; flag anything still in flight for the other images.
#if (MCUBOOT_IMAGE_NUMBER > 1)
        for (uint8_t image = NF_MCUBOOT_IMAGE_DEPLOY; image < MCUBOOT_IMAGE_NUMBER; image++)
        {
            if (mcuboot_update_state(image) != MCUBOOT_UPDATE_NONE)
            {
                BOOT_LOG_ERR("Image %u has an update in flight; entering serial recovery anyway", (unsigned)image);
            }
        }
#endif
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

#if (MCUBOOT_IMAGE_NUMBER > 1)
    // rsp.br_hdr points into the loader state, which the next boot_state_init() clears
    struct image_header clrHeader = *rsp.br_hdr;
    rsp.br_hdr = &clrHeader;

    struct boot_rsp deployRsp;

    boot_state_init(bootState);
    FIH_DECLARE(deployResult, FIH_FAILURE);
    FIH_CALL(boot_go_for_image_id, deployResult, &deployRsp, NF_MCUBOOT_IMAGE_DEPLOY);

    // Deployment pass is not fatal: empty or invalid deploy_0 must not prevent nanoCLR from booting
    if (FIH_NOT_EQ(deployResult, FIH_SUCCESS))
    {
        BOOT_LOG_ERR("Image %d boot pass failed (%d); booting nanoCLR", NF_MCUBOOT_IMAGE_DEPLOY, (int)deployResult);
    }
#endif

    // Launch the selected image
    do_boot(&rsp);

    // Unreachable.
    while (1)
    {
    }
}
