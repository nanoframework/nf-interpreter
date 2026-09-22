//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// MCUboot update media for ORGPAL_PALTHREE (STM32F769ZI).
//
// Lists the storage volumes the bootloader sweeps for update files
// (nano-clr-update-*.bin, nano-deployment-update-*.bin) before boot_go() — see
// MCUboot/common/mcuboot_media_import.c — and brings the block devices up/down around the
// sweep. They are the same SD card / USB stick the application uses as regular storage;
// nanoCLR re-initialises them after hand-off as it always did.
//   - USB mass storage on OTG_HS (USBHD2), FatFs volume "E:"   NF_FEATURE_MCUBOOT_HAS_USB_MSD
//   - SD card on SDMMC1 (SDCD1), FatFs volume "D:"             NF_FEATURE_MCUBOOT_HAS_SDCARD
//
// Sweep order is the table order below: USB first, because a plugged-in stick is the
// deliberate operator action while the SD card is usually installed permanently for
// application data. Flip the table to change it.
//
// Compiled only when at least one of the two media switches is enabled.

#if (defined(CONFIG_NF_FEATURE_MCUBOOT_HAS_SDCARD) && CONFIG_NF_FEATURE_MCUBOOT_HAS_SDCARD) ||                         \
    (defined(CONFIG_NF_FEATURE_MCUBOOT_HAS_USB_MSD) && CONFIG_NF_FEATURE_MCUBOOT_HAS_USB_MSD)

#include <ch.h>
#include <hal.h>
#include <stdbool.h>

#include <mcuboot_media_fatfs.h>
#include <mcuboot_board_iface.h>
#include <bootutil/bootutil_log.h>

#if defined(CONFIG_NF_FEATURE_MCUBOOT_HAS_USB_MSD) && CONFIG_NF_FEATURE_MCUBOOT_HAS_USB_MSD

#include <usbh/dev/msd.h>

// USB host controller carrying the MSD port (OTG_HS, see mcuconf_community.h).
#define MCUBOOT_USB_MSD_HOST USBHD2

// Wall-clock budget for a device to show up on the port after the host is started. With
// VBUS supplied by the board the stick is already powered, but the OTG core needs the
// forced-host-mode switch plus PHY settle before it reports the pull-up; the CLR never
// bounds this (it polls forever), so keep it generous.
#ifndef MCUBOOT_USB_MSD_ATTACH_TIMEOUT_MS
#define MCUBOOT_USB_MSD_ATTACH_TIMEOUT_MS 1000U
#endif

// Wall-clock budget for the whole bring-up: attach + debounce (200 ms) + reset + recovery
// (100 ms) + enumeration + MSD inquiry/test-unit-ready (up to 10 x 200 ms on slow sticks).
#ifndef MCUBOOT_USB_MSD_ENUM_TIMEOUT_MS
#define MCUBOOT_USB_MSD_ENUM_TIMEOUT_MS 6000U
#endif

#define MCUBOOT_USB_MSD_POLL_MS 10U

// Live "device attached" status straight from the OTG core (HPRT.PCSTS). Unlike
// rootport.status, which the USBH stack only refreshes while it processes a port change,
// this reflects the pull-up the moment the PHY sees it.
static inline bool usb_msd_port_connected(void)
{
    return (MCUBOOT_USB_MSD_HOST.otg->HPRT & HPRT_PCSTS) != 0U;
}

// Enumerate an attached USB mass storage device and bring its LUN to BLK_READY.
//
// Same call sequence as nanoCLR's UsbMsdWorkingThread (usbhStart, poll usbhMainLoop,
// usbhmsdLUNConnect once the class driver bound the LUN), just bounded in time: the USBH
// stack does not need a dedicated thread - usbhMainLoop() returns at once when nothing is
// pending and runs the complete debounce/reset/enumerate/driver-load sequence
// synchronously when the OTG interrupt flagged a connection.
static int usb_msd_init(void)
{
    USBHMassStorageLUNDriver *lun = &MSBLKD[0];
    systime_t start = chVTGetSystemTimeX();
    bool attached = false;

    usbhStart(&MCUBOOT_USB_MSD_HOST);

    for (;;)
    {
        mcuboot_media_watchdog_feed();

        // drive the host state machine; blocks through enumeration when a device is pending
        usbhMainLoop(&MCUBOOT_USB_MSD_HOST);

        uint32_t elapsed = (uint32_t)TIME_I2MS(chVTTimeElapsedSinceX(start));

        if (!attached && usb_msd_port_connected())
        {
            attached = true;
            BOOT_LOG_INF("USB MSD: device attached after %u ms, enumerating", (unsigned)elapsed);
        }

        blkstate_t state = blkGetDriverState(lun);

        if (state == BLK_READY)
        {
            return 0;
        }

        if (state == BLK_ACTIVE)
        {
            // MSD class driver bound to the device: bring the LUN up (inquiry, capacity).
            // ChibiOS convention: HAL_SUCCESS is false.
            if (usbhmsdLUNConnect(lun) == HAL_SUCCESS)
            {
                BOOT_LOG_INF("USB MSD: LUN ready after %u ms", (unsigned)elapsed);
                return 0;
            }

            BOOT_LOG_ERR("USB MSD: LUN connect failed (not a usable mass storage device)");
            break;
        }

        if (!attached && elapsed >= MCUBOOT_USB_MSD_ATTACH_TIMEOUT_MS)
        {
            BOOT_LOG_INF("USB MSD: no device attached within %u ms", (unsigned)elapsed);
            break;
        }

        if (elapsed >= MCUBOOT_USB_MSD_ENUM_TIMEOUT_MS)
        {
            BOOT_LOG_ERR("USB MSD: gave up after %u ms (LUN state %d)", (unsigned)elapsed, (int)state);
            break;
        }

        chThdSleepMilliseconds(MCUBOOT_USB_MSD_POLL_MS);
    }

    if (blkGetDriverState(lun) >= BLK_ACTIVE)
    {
        usbhmsdLUNDisconnect(lun);
    }

    usbhStop(&MCUBOOT_USB_MSD_HOST);

    return -1;
}

static void usb_msd_deinit(void)
{
    USBHMassStorageLUNDriver *lun = &MSBLKD[0];

    if (blkGetDriverState(lun) >= BLK_ACTIVE)
    {
        usbhmsdLUNDisconnect(lun);
    }

    // release the host controller so nanoCLR starts it from a clean state
    usbhStop(&MCUBOOT_USB_MSD_HOST);
}

static mcuboot_media_fatfs_ctx s_usbMsdCtx = {
    .volume = "E:",
    .device_init = usb_msd_init,
    .device_deinit = usb_msd_deinit,
};

#endif // CONFIG_NF_FEATURE_MCUBOOT_HAS_USB_MSD

#if defined(CONFIG_NF_FEATURE_MCUBOOT_HAS_SDCARD) && CONFIG_NF_FEATURE_MCUBOOT_HAS_SDCARD

static int sdcard_init(void)
{
    // Start the ChibiOS SDC driver (SDMMC1 peripheral clocked by halInit()).
    sdcStart(&SDCD1, NULL);

    // Connect to (identify) the inserted SD card: OCR, CID, CSD negotiation.
    if (sdcConnect(&SDCD1) != HAL_SUCCESS)
    {
        BOOT_LOG_INF("SD card: no card detected (sdcConnect failed)");
        sdcStop(&SDCD1);
        return -1;
    }

    BOOT_LOG_INF("SD card: connected");

    return 0;
}

static void sdcard_deinit(void)
{
    if (blkGetDriverState(&SDCD1) == BLK_READY)
    {
        (void)sdcDisconnect(&SDCD1);
    }

    sdcStop(&SDCD1);
}

static mcuboot_media_fatfs_ctx s_sdCardCtx = {
    .volume = "D:",
    .device_init = sdcard_init,
    .device_deinit = sdcard_deinit,
};

#endif // CONFIG_NF_FEATURE_MCUBOOT_HAS_SDCARD

// clang-format off
static const mcuboot_media s_mediaTable[] = {
#if defined(CONFIG_NF_FEATURE_MCUBOOT_HAS_USB_MSD) && CONFIG_NF_FEATURE_MCUBOOT_HAS_USB_MSD
    { .name = "USB MSD", .ops = &mcuboot_media_fatfs_ops, .ctx = &s_usbMsdCtx },
#endif
#if defined(CONFIG_NF_FEATURE_MCUBOOT_HAS_SDCARD) && CONFIG_NF_FEATURE_MCUBOOT_HAS_SDCARD
    { .name = "SD card", .ops = &mcuboot_media_fatfs_ops, .ctx = &s_sdCardCtx },
#endif
};
// clang-format on

const mcuboot_media *mcuboot_media_table(uint32_t *count)
{
    *count = sizeof(s_mediaTable) / sizeof(s_mediaTable[0]);
    return s_mediaTable;
}

#endif // CONFIG_NF_FEATURE_MCUBOOT_HAS_SDCARD || CONFIG_NF_FEATURE_MCUBOOT_HAS_USB_MSD
