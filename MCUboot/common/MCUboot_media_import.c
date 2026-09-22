//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Implements the algorithm and port interface described in mcuboot_media_import.h —
// see that file for the six-step per-image flow, the layering and the "used" marker
// format. This file is the platform-neutral engine: it touches only flash_area_*,
// bootutil_public.h and the mcuboot_media_ops callbacks, never a filesystem directly.

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include <mcuboot_config/mcuboot_config.h>
#include <flash_map_backend/flash_map_backend.h>
#include <sysflash/sysflash.h>
#include <bootutil/bootutil_public.h>
#include <bootutil/image.h>
#include <bootutil/bootutil_log.h>

#include <mcuboot_media_import.h>

// Copy buffer shared by all media (the sweep is strictly sequential).
static uint8_t s_chunk[MCUBOOT_IMPORT_CHUNK_SIZE];

// Best candidate found for an image on a medium.
typedef struct
{
    char name[MCUBOOT_MEDIA_NAME_MAX];
    struct image_header hdr;
    uint32_t size;
} import_candidate;

// Weak default: no watchdog to feed. Ports with a hardware watchdog override this.
__attribute__((weak)) void mcuboot_media_watchdog_feed(void)
{
}

const char *mcuboot_media_pattern_for_image(uint8_t image)
{
    switch (image)
    {
        case 0:
            return MCUBOOT_IMPORT_IMG0_PATTERN;

        case 1:
            return MCUBOOT_IMPORT_IMG1_PATTERN;

        default:
            return NULL;
    }
}

// Offset inside the secondary slot where the upgrade image must start.
// swap-using-offset keeps the first sector of the secondary slot free (MCUboot reads the
// header at boot_img_sector_size(secondary, 0) and rejects a magic at offset 0); every
// other swap mode expects the image at the start of the slot.
static uint32_t image_offset(const struct flash_area *fa)
{
#if defined(MCUBOOT_SWAP_USING_OFFSET)
#if defined(MCUBOOT_LOGICAL_SECTOR_SIZE) && (MCUBOOT_LOGICAL_SECTOR_SIZE != 0)
    (void)fa;
    return MCUBOOT_LOGICAL_SECTOR_SIZE;
#else
    struct flash_sector sector;

    if (flash_area_get_sector(fa, 0, &sector) != 0)
    {
        return 0;
    }

    return sector.fs_size;
#endif
#else
    (void)fa;
    return 0;
#endif
}

// Largest image the slot pair accepts. Under swap-using-offset the secondary slot is one
// sector larger than the primary: one sector is skipped at the start (imageOff) and the
// primary reserves one sector for the swap trailer, so two sectors come off the top.
// MCUboot performs the definitive check; this only avoids copying something hopeless.
static uint32_t usable_size(const struct flash_area *fa, uint32_t imageOff)
{
    uint32_t reserved = 2U * imageOff;

    return (fa->fa_size > reserved) ? (fa->fa_size - reserved) : 0U;
}

static bool header_is_sane(const struct image_header *hdr, uint32_t fileSize, uint32_t usable)
{
    if (hdr->ih_magic != IMAGE_MAGIC)
    {
        return false;
    }

    // header + payload + protected TLVs must fit in the file (unprotected TLVs follow)
    uint32_t span = (uint32_t)hdr->ih_hdr_size + hdr->ih_img_size + hdr->ih_protect_tlv_size;

    if (span < hdr->ih_img_size || span > fileSize)
    {
        return false;
    }

    return fileSize <= usable;
}

// major.minor.revision+build, all four components (MCUBOOT_VERSION_CMP_USE_BUILD_NUMBER).
static int version_cmp(const struct image_version *a, const struct image_version *b)
{
    if (a->iv_major != b->iv_major)
    {
        return (a->iv_major > b->iv_major) ? 1 : -1;
    }
    if (a->iv_minor != b->iv_minor)
    {
        return (a->iv_minor > b->iv_minor) ? 1 : -1;
    }
    if (a->iv_revision != b->iv_revision)
    {
        return (a->iv_revision > b->iv_revision) ? 1 : -1;
    }
    if (a->iv_build_num != b->iv_build_num)
    {
        return (a->iv_build_num > b->iv_build_num) ? 1 : -1;
    }
    return 0;
}

// Build the per-device stamp: "<device id> <major>.<minor>.<revision>+<build> <size>".
// Returns false when the device id is unavailable (marking disabled).
static bool build_stamp(char *out, size_t cap, const struct image_header *hdr, uint32_t fileSize)
{
    char id[MCUBOOT_MEDIA_STAMP_MAX / 2];

    if (mcuboot_media_device_id(id, sizeof(id)) == 0)
    {
        return false;
    }

    int n = snprintf(
        out,
        cap,
        "%s %u.%u.%u+%u %u",
        id,
        (unsigned)hdr->ih_ver.iv_major,
        (unsigned)hdr->ih_ver.iv_minor,
        (unsigned)hdr->ih_ver.iv_revision,
        (unsigned)hdr->ih_ver.iv_build_num,
        (unsigned)fileSize);

    return (n > 0) && ((size_t)n < cap);
}

static bool primary_runs_version(uint8_t image, const struct image_version *ver)
{
    const struct flash_area *fa = NULL;
    struct image_header hdr;
    bool same = false;

    if (flash_area_open(FLASH_AREA_IMAGE_PRIMARY(image), &fa) != 0 || fa == NULL)
    {
        return false;
    }

    if (flash_area_read(fa, 0, &hdr, sizeof(hdr)) == 0 && hdr.ih_magic == IMAGE_MAGIC)
    {
        same = (memcmp(&hdr.ih_ver, ver, sizeof(*ver)) == 0);
    }

    flash_area_close(fa);

    return same;
}

// Enumerate the files matching the image's pattern and keep the valid one with the
// highest version. Returns true when a candidate was found.
static bool find_candidate(const mcuboot_media *media, uint8_t image, uint32_t usable, import_candidate *best)
{
    bool found = false;
    char name[MCUBOOT_MEDIA_NAME_MAX];

    for (uint32_t n = 0; media->ops->find(media->ctx, image, n, name, sizeof(name)) == 0; n++)
    {
        uint32_t size = 0;
        struct image_header hdr;

        if (media->ops->open(media->ctx, name, &size) != 0)
        {
            BOOT_LOG_WRN("media import: %s: cannot open %s", media->name, name);
            continue;
        }

        bool ok = (size >= sizeof(hdr)) && (media->ops->read(media->ctx, 0, &hdr, sizeof(hdr)) == 0);

        media->ops->close(media->ctx);

        if (!ok || !header_is_sane(&hdr, size, usable))
        {
            BOOT_LOG_WRN(
                "media import: %s: %s ignored (not a valid image %u file or larger than %u bytes)",
                media->name,
                name,
                (unsigned)image,
                (unsigned)usable);
            continue;
        }

        if (!found || version_cmp(&hdr.ih_ver, &best->hdr.ih_ver) > 0)
        {
            strcpy(best->name, name);
            best->hdr = hdr;
            best->size = size;
            found = true;
        }
    }

    return found;
}

// Copy the candidate into the secondary slot of `image` and mark it pending.
// Returns 0 on success; on failure the slot is left erased.
static int stage_image(
    const mcuboot_media *media,
    uint8_t image,
    const struct flash_area *fa,
    uint32_t imageOff,
    const import_candidate *cand)
{
    uint32_t size = 0;

    if (media->ops->open(media->ctx, cand->name, &size) != 0 || size != cand->size)
    {
        BOOT_LOG_ERR("media import: image %u: cannot reopen %s", (unsigned)image, cand->name);
        media->ops->close(media->ctx);
        return -1;
    }

    mcuboot_media_watchdog_feed();

    if (flash_area_erase(fa, 0, fa->fa_size) != 0)
    {
        BOOT_LOG_ERR("media import: image %u: secondary slot erase failed", (unsigned)image);
        media->ops->close(media->ctx);
        return -1;
    }

    uint32_t pos = 0;

    while (pos < size)
    {
        uint32_t len = size - pos;

        if (len > sizeof(s_chunk))
        {
            len = sizeof(s_chunk);
        }

        if (media->ops->read(media->ctx, pos, s_chunk, len) != 0)
        {
            BOOT_LOG_ERR("media import: image %u: read failed at %u", (unsigned)image, (unsigned)pos);
            goto fail;
        }

        if (flash_area_write(fa, imageOff + pos, s_chunk, len) != 0)
        {
            BOOT_LOG_ERR("media import: image %u: flash write failed at %u", (unsigned)image, (unsigned)pos);
            goto fail;
        }

        pos += len;

        mcuboot_media_watchdog_feed();
    }

    media->ops->close(media->ctx);

    // read the header back: catches a silently failing flash device
    struct image_header check;

    if (flash_area_read(fa, imageOff, &check, sizeof(check)) != 0 || memcmp(&check, &cand->hdr, sizeof(check)) != 0)
    {
        BOOT_LOG_ERR("media import: image %u: header read-back mismatch", (unsigned)image);
        (void)flash_area_erase(fa, 0, fa->fa_size);
        return -1;
    }

    // schedule a one-time test swap; the application confirms it after booting
    if (boot_set_pending_multi(image, 0) != 0)
    {
        BOOT_LOG_ERR("media import: image %u: set pending failed", (unsigned)image);
        (void)flash_area_erase(fa, 0, fa->fa_size);
        return -1;
    }

    return 0;

fail:
    media->ops->close(media->ctx);
    (void)flash_area_erase(fa, 0, fa->fa_size);
    return -1;
}

// Handle one image index on one medium. Returns true when the image was staged.
static bool import_image(const mcuboot_media *media, uint8_t image)
{
    static import_candidate s_cand;
    const struct flash_area *fa = NULL;
    bool staged = false;
    char stamp[MCUBOOT_MEDIA_STAMP_MAX];
    bool haveStamp = false;

    if (flash_area_open(FLASH_AREA_IMAGE_SECONDARY(image), &fa) != 0 || fa == NULL)
    {
        BOOT_LOG_ERR("media import: image %u: no secondary slot", (unsigned)image);
        return false;
    }

    uint32_t imageOff = image_offset(fa);
    uint32_t usable = usable_size(fa, imageOff);

    if (!find_candidate(media, image, usable, &s_cand))
    {
        // no update file for this image on this medium
        goto done;
    }

    BOOT_LOG_INF(
        "media import: %s: image %u candidate %s (%u.%u.%u+%u, %u bytes)",
        media->name,
        (unsigned)image,
        s_cand.name,
        (unsigned)s_cand.hdr.ih_ver.iv_major,
        (unsigned)s_cand.hdr.ih_ver.iv_minor,
        (unsigned)s_cand.hdr.ih_ver.iv_revision,
        (unsigned)s_cand.hdr.ih_ver.iv_build_num,
        (unsigned)s_cand.size);

    haveStamp = build_stamp(stamp, sizeof(stamp), &s_cand.hdr, s_cand.size);

    if (haveStamp && media->ops->is_used(media->ctx, s_cand.name, stamp))
    {
        BOOT_LOG_INF("media import: image %u: already imported by this device, skipping", (unsigned)image);
        goto done;
    }

    int swapType = boot_swap_type_multi(image);

    if (swapType != BOOT_SWAP_TYPE_NONE)
    {
        // the secondary slot is part of an in-flight test/permanent/revert cycle: leave it alone,
        // the file stays unmarked and is picked up on a later boot
        BOOT_LOG_INF("media import: image %u: swap in flight (type %d), deferring import", (unsigned)image, swapType);
        goto done;
    }

    if (primary_runs_version(image, &s_cand.hdr.ih_ver))
    {
        BOOT_LOG_INF("media import: image %u: primary already runs this version, skipping", (unsigned)image);

        if (haveStamp && media->ops->mark_used(media->ctx, s_cand.name, stamp) != 0)
        {
            BOOT_LOG_WRN("media import: image %u: could not write used marker", (unsigned)image);
        }

        goto done;
    }

    BOOT_LOG_INF("media import: image %u: staging into secondary slot at +0x%x", (unsigned)image, (unsigned)imageOff);

    if (stage_image(media, image, fa, imageOff, &s_cand) != 0)
    {
        goto done;
    }

    staged = true;

    BOOT_LOG_INF("media import: image %u: staged, pending test swap", (unsigned)image);

    if (haveStamp && media->ops->mark_used(media->ctx, s_cand.name, stamp) != 0)
    {
        BOOT_LOG_WRN("media import: image %u: could not write used marker (read-only medium?)", (unsigned)image);
    }

done:
    flash_area_close(fa);

    return staged;
}

void mcuboot_media_import_run(void)
{
    uint32_t mediaCount = 0;
    const mcuboot_media *table = mcuboot_media_table(&mediaCount);

    if (table == NULL || mediaCount == 0)
    {
        return;
    }

    // first medium that supplies a file for an image index wins
    bool imageDone[MCUBOOT_IMAGE_NUMBER] = {false};
    uint32_t pending = MCUBOOT_IMAGE_NUMBER;

    for (uint32_t m = 0; m < mediaCount && pending > 0; m++)
    {
        const mcuboot_media *media = &table[m];

        if (media->ops == NULL)
        {
            continue;
        }

        if (media->ops->mount(media->ctx) != 0)
        {
            BOOT_LOG_INF("media import: %s: not present", media->name);
            continue;
        }

        BOOT_LOG_INF("media import: %s: mounted, looking for update files", media->name);

        for (uint8_t image = 0; image < MCUBOOT_IMAGE_NUMBER; image++)
        {
            if (imageDone[image])
            {
                continue;
            }

            if (import_image(media, image))
            {
                imageDone[image] = true;
                pending--;
            }
        }

        media->ops->unmount(media->ctx);
    }
}
