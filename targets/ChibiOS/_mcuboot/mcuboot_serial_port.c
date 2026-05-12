// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// ChibiOS stream callbacks for MCUboot SMP serial recovery.
//
// Provides the two boot_uart_funcs function pointers (read/write) that
// MCUboot's boot_serial.c needs. The actual transport driver is returned by
// the board-specific mcuboot_serial_get_channel() (declared in
// mcuboot_serial_port.h and implemented in each board's mcuboot_detect_pin.c).
//
// mcuboot_serial_recovery_try() is called from mcuboot_main() after:
//   - halInit() and chSysInit() have set up the hardware and kernel.
//   - mcuboot_ext_flash_init() and mcuboot_sdcard_init() have initialized storage.
//   - But BEFORE boot_go() so recovery is available even if no valid image exists.
//
// It:
//   1. Sleeps MCUBOOT_SERIAL_DETECT_DELAY ms (hardware debounce).
//   2. Calls boot_serial_detect_pin() (board-specific, e.g. holds BOOT button).
//   3. If triggered:
//      a. Raises main thread to HIGHPRIO.
//      b. Calls mcuboot_target_init() — board USB/UART init from main at HIGHPRIO.
//      c. Spawns a dedicated NORMALPRIO thread that runs boot_serial_start().
//      d. Drops main back to NORMALPRIO.
//      e. Main enters infinite sleep loop.
//
//   boot_serial_start() never returns; it resets the device via hal_system_reset()
//   (= NVIC_SystemReset()) when recovery is complete. The device then restarts
//   and runs boot_go() with the newly-uploaded image ready in the secondary slot.
//
// If the button is not pressed, mcuboot_serial_recovery_try() returns immediately
// and boot_go() is called to validate the current image.

#include "mcuboot_serial_port.h"
#include "mcuboot_board_iface.h"

#if defined(MCUBOOT_SERIAL)

#include "hal.h"
#include "ch.h"

#include "boot_serial/boot_serial.h"
#include "mcuboot_config/mcuboot_config.h"

// ---------------------------------------------------------------------------
// ChibiOS stream I/O callbacks
// ---------------------------------------------------------------------------

static int nf_serial_read(char *buf, int cnt, int *newline)
{
    *newline = 0;
    int n = (int)chnReadTimeout(mcuboot_serial_get_channel(), (uint8_t *)buf, (size_t)cnt, TIME_MS2I(100));
    if (n > 0 && buf[n - 1] == '\n')
    {
        *newline = 1;
    }
    return n;
}

static void nf_serial_write(const char *buf, int cnt)
{
    chnWrite(mcuboot_serial_get_channel(), (const uint8_t *)buf, (size_t)cnt);
}

static const struct boot_uart_funcs nf_uart_funcs = {
    .read = nf_serial_read,
    .write = nf_serial_write,
};

// ---------------------------------------------------------------------------
// CPU idle hook
// ---------------------------------------------------------------------------

// Called by MCUBOOT_CPU_IDLE() in boot_serial_read_console() when no data is
// received. The serial recovery loop runs in its own thread (see below), so
// yielding here is enough — the scheduler will context-switch to the idle or
// main thread automatically.
void nf_mcuboot_cpu_idle(void)
{
    chThdSleepMilliseconds(10);
}

// ---------------------------------------------------------------------------
// Serial recovery thread
// ---------------------------------------------------------------------------

// Stack for the serial recovery thread. boot_serial.c has two 1 KB internal
// buffers (in_buf / dec_buf) plus zcbor/CBOR state — 4 KB is sufficient.
static THD_WORKING_AREA(wa_serial_recovery, 4096);

static THD_FUNCTION(serial_recovery_thd, arg)
{
    (void)arg;

    // Board-specific UART driver start (no-op for USB CDC boards — USB is
    // already active, initialised from the main thread before this thread
    // was spawned, mirroring the nanoBooter sequence).
    mcuboot_uart_init();

    // Run the upstream SMP/mcumgr receive loop. This function never returns:
    // when the host sends a reset command after image upload, boot_serial.c
    // calls hal_system_reset() which invokes NVIC_SystemReset().
    boot_serial_start(&nf_uart_funcs);

    // Unreachable.
    while (1)
    {
    }
}

// ---------------------------------------------------------------------------
// Weak default board hooks
// ---------------------------------------------------------------------------

// Weak no-op: USB CDC boards start their transport in mcuboot_target_init()
// and do not need additional initialisation here.
__attribute__((weak)) void mcuboot_uart_init(void)
{
}

// ---------------------------------------------------------------------------
// Serial recovery launch
// ---------------------------------------------------------------------------

// Start the SMP serial recovery loop unconditionally.
// Called either from mcuboot_serial_recovery_try() when the recovery button
// is detected, or directly from main() when boot_go() finds no valid image.
// This function never returns: the SMP thread handles device reset after upload.
__attribute__((noreturn)) void mcuboot_serial_recovery_start(void)
{
    chThdSetPriority(HIGHPRIO);

    // Initialise board hardware (USB CDC, UART, etc.) from the main thread
    mcuboot_target_init();

    // Spawn the SMP receive thread now that the transport is up.
    chThdCreateStatic(wa_serial_recovery, sizeof(wa_serial_recovery), NORMALPRIO, serial_recovery_thd, NULL);

    // Drop main back to NORMALPRIO.
    chThdSetPriority(NORMALPRIO);

    // Main thread idle loop - the SMP thread owns recovery from here.
    // boot_serial_start() resets the device via NVIC_SystemReset() when done.
    while (1)
    {
        chThdSleepMilliseconds(500);
    }
}

// ---------------------------------------------------------------------------
// Serial recovery entry point (button-triggered)
// ---------------------------------------------------------------------------

void mcuboot_serial_recovery_try(void)
{
    // Wait for hardware to settle (debounce the detect pin).
    chThdSleepMilliseconds(MCUBOOT_SERIAL_DETECT_DELAY);

    if (!boot_serial_detect_pin())
    {
        // Button not pressed - proceed with normal boot
        return;
    }

    mcuboot_serial_recovery_start();
}

#endif // MCUBOOT_SERIAL
