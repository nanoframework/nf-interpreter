//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// mcuboot_media_import.h — import signed update images from removable storage.
//
// MCUboot only knows two slots per image (primary + secondary); it cannot boot or
// swap from an SD card or a USB stick. Those remain what they are for the application:
// ordinary storage volumes exposed through the file system, mounted read/write as usual
// once the bootloader hands off. All the bootloader does, before boot_go() runs, is
// mount each medium in mcuboot_media_table() order and, per image index, run:
//
//   1. enumerate the medium's root directory for files matching the image's pattern
//      (nanoCLR-*.bin / nanoDeployment-*.bin); among those whose header is sane, that
//      carry a SHA-256 TLV and that fit the usable slot size, keep the highest version
//   2. defer if MCUboot left a swap unfinished (reset mid-swap): boot_go() must resume it
//   3. skip, and mark used, if the primary slot holds an intact image with the same
//      SHA-256 (identity is the image hash, not the version number)
//   4. skip if this device already consumed that exact file (stamp in its "used" marker),
//      unless the primary holds no valid image (erased, random content, corrupt): then
//      the marker belongs to a past life of the device and is ignored
//   5. per swap state of the image:
//        none          -> stage
//        revert        -> defer while the image to revert to is intact (let MCUboot
//                         restore it); stage when it is not (a revert would install garbage)
//        test/perm     -> skip, and mark used, if the identical image is already pending;
//                         otherwise stage (replaces an invalid or a different pending image)
//   6. erase the secondary slot, copy the file in at the offset the swap mode expects,
//      hash the copy in flash against the file's SHA-256 TLV, mark the slot pending for
//      a one-time test swap
//   7. mark the file as consumed by this device
//
// Every outcome is logged with its reason. Any failure along the way is logged and the
// slot is left erased; boot continues with whatever the slots already held. MCUboot then
// performs its regular swap/validate/revert cycle from the secondary slot, exactly as it
// does for images uploaded over SMP or the Wire Protocol — this module only ever stages
// the file, it never edits an existing trailer.
//
// Layering:
//   MCUboot/common/MCUboot_media_import.c                platform-neutral engine (this API)
//   targets/<RTOS>/.../mcuboot_media_<fs>.c               filesystem implementation of mcuboot_media_ops
//   targets/<RTOS>/<BOARD>/MCUboot/mcuboot_media_boot.c   device bring-up + media table (sweep order)
//
// The engine depends only on the MCUboot flash_area_* API, bootutil_public.h and the
// mcuboot_media_ops callbacks below. A port has to provide:
//   - one mcuboot_media_ops implementation per filesystem/media type,
//   - mcuboot_media_table(),
//   - mcuboot_media_device_id(),
//   - optionally mcuboot_media_watchdog_feed() (weak no-op by default).
//
// "Used" marking (steps 3/4/5/7 above): the engine builds a stamp string that identifies the
// file (image version + size + SHA-256 prefix) *and this device* (unique id). The ops
// store/look up that stamp next to the file, so a device imports a given file once while
// the same medium remains usable to update other devices; a rebuilt file with an unchanged
// version is a different file. Deferred imports and failures are never marked. Marking is
// best effort: on read-only media the same-hash check against the primary slot (step 3)
// still prevents a re-import loop once the update has been applied.

#ifndef MCUBOOT_MEDIA_IMPORT_H
#define MCUBOOT_MEDIA_IMPORT_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

// File name patterns, one per image index, matched against the root directory of every
// medium ('*' = any sequence, '?' = any single character, case-insensitive). When several
// files match, the one carrying the highest image version is imported.
// Override per target from mcuboot_config.h if needed.
#ifndef MCUBOOT_IMPORT_IMG0_PATTERN
#define MCUBOOT_IMPORT_IMG0_PATTERN "nanoCLR-*.bin"
#endif
#ifndef MCUBOOT_IMPORT_IMG1_PATTERN
#define MCUBOOT_IMPORT_IMG1_PATTERN "nanoDeployment-*.bin"
#endif

// Suffix appended to the update file name for the per-device "used" marker.
#ifndef MCUBOOT_IMPORT_USED_SUFFIX
#define MCUBOOT_IMPORT_USED_SUFFIX ".used"
#endif

// Longest file name (without directory) the engine handles; longer matches are ignored.
#ifndef MCUBOOT_MEDIA_NAME_MAX
#define MCUBOOT_MEDIA_NAME_MAX 96U
#endif

// Size of the file -> flash copy buffer. Keep it a multiple of the media sector size
// (512 bytes for SD/USB) and of the flash write granularity.
#ifndef MCUBOOT_IMPORT_CHUNK_SIZE
#define MCUBOOT_IMPORT_CHUNK_SIZE 4096U
#endif

// Longest stamp string the engine produces (device id + version + size + hash prefix + NUL).
#define MCUBOOT_MEDIA_STAMP_MAX 128U

    // Pattern the update files of an image index must match (see the defines above).
    const char *mcuboot_media_pattern_for_image(uint8_t image);

    // Callbacks implemented per filesystem/media type. ctx is the opaque per-medium
    // context from the mcuboot_media entry. All calls are made from the bootloader
    // main context, strictly in the order mount -> (find* / open -> read* -> close)* ->
    // is_used/mark_used -> unmount. File names are relative to the medium's root.
    typedef struct
    {
        // Bring the medium up and mount its filesystem. 0 = ready, <0 = absent/failed.
        int (*mount)(void *ctx);

        // Unmount and release the medium/device before the bootloader hands off.
        void (*unmount)(void *ctx);

        // Return in `name` the n-th (0-based) file in the root directory matching
        // mcuboot_media_pattern_for_image(image). <0 when there is no n-th match
        // (this ends the enumeration); a name that does not fit `cap` is skipped.
        int (*find)(void *ctx, uint8_t image, uint32_t n, char *name, size_t cap);

        // Open `name` for reading and return its size. <0 if it cannot be opened.
        int (*open)(void *ctx, const char *name, uint32_t *size);

        // Read `len` bytes at absolute file offset `off` into dst. 0 on success.
        int (*read)(void *ctx, uint32_t off, void *dst, uint32_t len);

        // Close the file opened by open().
        void (*close)(void *ctx);

        // true if this exact stamp was already recorded for the file `name`.
        bool (*is_used)(void *ctx, const char *name, const char *stamp);

        // Record the stamp for the file `name`. <0 if the medium is read-only or the
        // marker cannot be written (the import itself is still valid).
        int (*mark_used)(void *ctx, const char *name, const char *stamp);
    } mcuboot_media_ops;

    // One removable medium.
    typedef struct
    {
        const char *name;             // for log output, e.g. "SD card"
        const mcuboot_media_ops *ops; // filesystem implementation
        void *ctx;                    // opaque, passed to every ops callback
    } mcuboot_media;

    // Port-provided: the media to sweep. Array order is the sweep order; the first medium
    // that supplies a file for a given image index wins. Return NULL / *count = 0 for none.
    const mcuboot_media *mcuboot_media_table(uint32_t *count);

    // Port-provided: this device's unique id as lowercase hex (no separators), NUL
    // terminated. Returns the number of characters written (0 if unavailable, in
    // which case marking is disabled and only the same-hash check applies).
    size_t mcuboot_media_device_id(char *hex, size_t cap);

    // Port-provided, optional (weak no-op in the engine): kick the watchdog. Called once per
    // chunk while copying so a long import does not trip a hardware watchdog.
    void mcuboot_media_watchdog_feed(void);

    // Engine entry point. Sweeps every medium for every image index, stages what it finds.
    // Never fatal: any failure is logged and boot continues with whatever is in the slots.
    // Call once from main() after the flash devices are up and before boot_go().
    void mcuboot_media_import_run(void);

#ifdef __cplusplus
}
#endif

#endif // MCUBOOT_MEDIA_IMPORT_H
