//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// FatFs implementation of the MCUboot media import interface (mcuboot_media_import.h)
// for ChibiOS targets. One instance serves any FatFs logical drive (SD card via SDC,
// USB mass storage via USBH MSD, ...) — the same volumes the application uses as regular
// storage; the bootloader only reads the update files from their root directory. The
// board supplies the drive letter and the block-device bring-up/teardown in a
// mcuboot_media_fatfs_ctx and lists the instances in mcuboot_media_table().

#ifndef MCUBOOT_MEDIA_FATFS_H
#define MCUBOOT_MEDIA_FATFS_H

#include <stdbool.h>
#include <ff.h>
#include <mcuboot_media_import.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        // FatFs logical drive prefix as spelled in FF_VOLUME_STRS, e.g. "D:" or "E:".
        const char *volume;

        // Bring the block device up so the FatFs diskio can use it. 0 on success,
        // <0 when the device is absent or failed (the medium is then skipped).
        int (*device_init)(void);

        // Release the block device after the sweep (before handing off to the application).
        void (*device_deinit)(void);

        // Private state, zero-initialised by the board. The FIL objects are here rather than
        // on the stack: each carries a sector buffer and the bootloader main stack is small.
        FATFS fs;
        FIL file;
        FIL marker;
        bool fileOpen;
    } mcuboot_media_fatfs_ctx;

    extern const mcuboot_media_ops mcuboot_media_fatfs_ops;

#ifdef __cplusplus
}
#endif

#endif // MCUBOOT_MEDIA_FATFS_H
