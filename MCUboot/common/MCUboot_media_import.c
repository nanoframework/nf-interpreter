//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Implements the algorithm and port interface described in mcuboot_media_import.h

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
#include <MCUboot_ImageSlotInfo.h>
#include <MCUboot_UpdateGuard.h>

// Copy buffer shared by all media (the sweep is strictly sequential).
static uint8_t s_chunk[MCUBOOT_IMPORT_CHUNK_SIZE];

// Bytes of the SHA-256 shown in log output and stored in the "used" stamp.
#define IMPORT_HASH_PREFIX_BYTES 8U

// Best candidate found for an image on a medium: a loose file, or an entry of a bundle.
typedef struct
{
    char name[MCUBOOT_MEDIA_NAME_MAX];
    // size of the file `name`, checked again when the file is reopened for the copy
    uint32_t fileSize;
    // where the signed image starts in the file (0 for a loose file) and its size
    uint32_t base;
    struct image_header hdr;
    uint32_t size;
    uint8_t hash[32];
} import_candidate;

// Best update bundle found on a medium. present[i] tells whether it carries image i.
typedef struct
{
    bool present[MCUBOOT_IMAGE_NUMBER];
    import_candidate entry[MCUBOOT_IMAGE_NUMBER];
} import_bundle;

// What to do with the candidate once the slots have been looked at.
typedef enum
{
    // stage it into the secondary slot
    IMPORT_STAGE,
    // leave everything as is, record the file as consumed by this device
    IMPORT_SKIP_MARK,
    // leave everything as is, do not record (already recorded)
    IMPORT_SKIP,
    // leave everything as is, do not record: retried on a later boot
    IMPORT_DEFER,
} import_action;

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

static void hash_prefix(const uint8_t *hash, char out[2 * IMPORT_HASH_PREFIX_BYTES + 1])
{
    static const char hex[] = "0123456789abcdef";

    for (uint32_t i = 0; i < IMPORT_HASH_PREFIX_BYTES; i++)
    {
        out[2 * i] = hex[hash[i] >> 4];
        out[2 * i + 1] = hex[hash[i] & 0x0F];
    }

    out[2 * IMPORT_HASH_PREFIX_BYTES] = '\0';
}

static bool header_is_sane(const struct image_header *hdr, uint32_t fileSize, uint32_t usable)
{
    if (hdr->ih_magic != IMAGE_MAGIC)
    {
        return false;
    }

    // header + payload + protected TLVs must fit in the file (unprotected TLVs follow)
    uint32_t span = Ifu_ImageHashedSize(hdr);

    if (span == 0 || span > fileSize)
    {
        return false;
    }

    return fileSize <= usable;
}

// Read the SHA-256 TLV from the unprotected TLV area of the image that starts at `base` in
// the open file and spans `size` bytes. 0 on success.
static int file_hash_tlv(
    const mcuboot_media *media,
    uint32_t base,
    uint32_t size,
    const struct image_header *hdr,
    uint8_t hash[32])
{
    uint32_t tlvOff = Ifu_ImageHashedSize(hdr);
    struct image_tlv_info info;

    if (tlvOff == 0 || tlvOff > size || size - tlvOff < sizeof(info) ||
        media->ops->read(media->ctx, base + tlvOff, &info, sizeof(info)) != 0 || info.it_magic != IMAGE_TLV_INFO_MAGIC)
    {
        return -1;
    }

    uint32_t pos = tlvOff + sizeof(info);
    uint32_t end = tlvOff + info.it_tlv_tot;

    if (end < tlvOff || end > size)
    {
        return -1;
    }

    while (pos + sizeof(struct image_tlv) <= end)
    {
        struct image_tlv tlv;

        if (media->ops->read(media->ctx, base + pos, &tlv, sizeof(tlv)) != 0)
        {
            return -1;
        }

        pos += sizeof(tlv);

        if (tlv.it_type == IMAGE_TLV_SHA256 && tlv.it_len == 32U && pos + 32U <= end)
        {
            return media->ops->read(media->ctx, base + pos, hash, 32U);
        }

        pos += tlv.it_len;
    }

    return -1;
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

// Build the per-device stamp: "<device id> <major>.<minor>.<revision>+<build> <size> <hash prefix>".
// The hash prefix makes a rebuilt file with the same name, version and size a different file.
// Returns false when the device id is unavailable (marking disabled).
static bool build_stamp(char *out, size_t cap, const import_candidate *cand)
{
    char id[MCUBOOT_MEDIA_STAMP_MAX / 2];
    char hash[2 * IMPORT_HASH_PREFIX_BYTES + 1];

    if (mcuboot_media_device_id(id, sizeof(id)) == 0)
    {
        return false;
    }

    hash_prefix(cand->hash, hash);

    int n = snprintf(
        out,
        cap,
        "%s %u.%u.%u+%u %u %s",
        id,
        (unsigned)cand->hdr.ih_ver.iv_major,
        (unsigned)cand->hdr.ih_ver.iv_minor,
        (unsigned)cand->hdr.ih_ver.iv_revision,
        (unsigned)cand->hdr.ih_ver.iv_build_num,
        (unsigned)cand->size,
        hash);

    return (n > 0) && ((size_t)n < cap);
}

// Secondary slot geometry of `image`: where the image goes in the slot and how large it may be.
// Returns false when the image has no secondary slot.
static bool slot_geometry(uint8_t image, uint32_t *imageOff, uint32_t *usable)
{
    const struct flash_area *fa = NULL;

    if (flash_area_open(FLASH_AREA_IMAGE_SECONDARY(image), &fa) != 0 || fa == NULL)
    {
        return false;
    }

    *imageOff = Ifu_ImageOffset(fa);
    *usable = Ifu_UsableSize(fa);

    flash_area_close(fa);

    return true;
}

// Check the signed image that starts at `base` and spans `size` bytes of the open file `name`
// and fill `cand` from it. Logs and returns false when it is not usable for `image`.
static bool probe_image(
    const mcuboot_media *media,
    const char *name,
    uint32_t fileSize,
    uint32_t base,
    uint32_t size,
    uint8_t image,
    uint32_t usable,
    import_candidate *cand)
{
    bool ok = (size >= sizeof(cand->hdr)) && (media->ops->read(media->ctx, base, &cand->hdr, sizeof(cand->hdr)) == 0);

    if (!ok || !header_is_sane(&cand->hdr, size, usable))
    {
        BOOT_LOG_WRN(
            "media import: %s: %s ignored (not a valid image %u at +0x%x or larger than %u bytes)",
            media->name,
            name,
            (unsigned)image,
            (unsigned)base,
            (unsigned)usable);
        return false;
    }

    if (file_hash_tlv(media, base, size, &cand->hdr, cand->hash) != 0)
    {
        BOOT_LOG_WRN("media import: %s: %s ignored (image %u has no SHA-256 TLV)", media->name, name, (unsigned)image);
        return false;
    }

    strcpy(cand->name, name);
    cand->fileSize = fileSize;
    cand->base = base;
    cand->size = size;

    return true;
}

// Enumerate the loose files matching the image's pattern and keep the valid one with the
// highest version. Returns true when a candidate was found.
static bool find_candidate(const mcuboot_media *media, uint8_t image, uint32_t usable, import_candidate *best)
{
    static import_candidate s_probe;
    bool found = false;
    char name[MCUBOOT_MEDIA_NAME_MAX];
    const char *pattern = mcuboot_media_pattern_for_image(image);

    for (uint32_t n = 0; pattern != NULL && media->ops->find(media->ctx, pattern, n, name, sizeof(name)) == 0; n++)
    {
        uint32_t size = 0;

        if (media->ops->open(media->ctx, name, &size) != 0)
        {
            BOOT_LOG_WRN("media import: %s: cannot open %s", media->name, name);
            continue;
        }

        bool valid = probe_image(media, name, size, 0, size, image, usable, &s_probe);

        media->ops->close(media->ctx);

        if (valid && (!found || version_cmp(&s_probe.hdr.ih_ver, &best->hdr.ih_ver) > 0))
        {
            *best = s_probe;
            found = true;
        }
    }

    return found;
}

// Parse and check the manifest and every entry of the open bundle file `name`.
// Returns false (and logs why) when the bundle cannot be used as a whole.
static bool probe_bundle(const mcuboot_media *media, const char *name, uint32_t fileSize, import_bundle *bundle)
{
    mcuboot_bundle_header hdr;
    mcuboot_bundle_entry entries[MCUBOOT_IMAGE_NUMBER];
    const char *why = NULL;

    memset(bundle, 0, sizeof(*bundle));

    if (fileSize < sizeof(hdr) || media->ops->read(media->ctx, 0, &hdr, sizeof(hdr)) != 0 ||
        memcmp(hdr.marker, MCUBOOT_BUNDLE_MARKER, MCUBOOT_BUNDLE_MARKER_SIZE) != 0)
    {
        why = "no bundle marker";
    }
    else if (hdr.format_version != MCUBOOT_BUNDLE_FORMAT_VERSION)
    {
        why = "unsupported format version";
    }
    else if (hdr.entry_count == 0 || hdr.entry_count > MCUBOOT_IMAGE_NUMBER)
    {
        why = "bad entry count";
    }
    else if (
        hdr.header_size < sizeof(hdr) + hdr.entry_count * sizeof(mcuboot_bundle_entry) || hdr.header_size > fileSize ||
        media->ops->read(media->ctx, sizeof(hdr), entries, hdr.entry_count * sizeof(mcuboot_bundle_entry)) != 0)
    {
        why = "bad header size";
    }

    for (uint32_t i = 0; why == NULL && i < hdr.entry_count; i++)
    {
        const mcuboot_bundle_entry *e = &entries[i];

        if (e->image >= MCUBOOT_IMAGE_NUMBER || bundle->present[e->image])
        {
            why = "bad or duplicate image index";
        }
        else if (
            e->offset < hdr.header_size || (e->offset % 4U) != 0 || e->size == 0 || e->offset > fileSize ||
            e->size > fileSize - e->offset)
        {
            why = "entry out of bounds or misaligned";
        }
        else
        {
            for (uint32_t j = 0; j < i; j++)
            {
                if (e->offset < entries[j].offset + entries[j].size && entries[j].offset < e->offset + e->size)
                {
                    why = "entries overlap";
                    break;
                }
            }
        }

        if (why != NULL)
        {
            break;
        }

        uint32_t imageOff = 0;
        uint32_t usable = 0;

        if (!slot_geometry(e->image, &imageOff, &usable))
        {
            why = "image has no secondary slot";
            break;
        }

        if (!probe_image(media, name, fileSize, e->offset, e->size, e->image, usable, &bundle->entry[e->image]))
        {
            why = "invalid image entry";
            break;
        }

        bundle->present[e->image] = true;
    }

    if (why != NULL)
    {
        BOOT_LOG_WRN("media import: %s: bundle %s ignored (%s)", media->name, name, why);
        return false;
    }

    return true;
}

// Order bundles by the versions of their images, image 0 first. A missing image counts as 0.0.0+0.
static int bundle_cmp(const import_bundle *a, const import_bundle *b)
{
    static const struct image_version none = {0};

    for (uint8_t image = 0; image < MCUBOOT_IMAGE_NUMBER; image++)
    {
        int c = version_cmp(
            a->present[image] ? &a->entry[image].hdr.ih_ver : &none,
            b->present[image] ? &b->entry[image].hdr.ih_ver : &none);

        if (c != 0)
        {
            return c;
        }
    }

    return 0;
}

// Enumerate the bundle files and keep the valid one with the highest versions.
// Returns true when a bundle was found.
static bool find_bundle(const mcuboot_media *media, import_bundle *best)
{
    static import_bundle s_probe;
    bool found = false;
    char name[MCUBOOT_MEDIA_NAME_MAX];

    for (uint32_t n = 0; media->ops->find(media->ctx, MCUBOOT_IMPORT_BUNDLE_PATTERN, n, name, sizeof(name)) == 0; n++)
    {
        uint32_t size = 0;

        if (media->ops->open(media->ctx, name, &size) != 0)
        {
            BOOT_LOG_WRN("media import: %s: cannot open %s", media->name, name);
            continue;
        }

        bool valid = probe_bundle(media, name, size, &s_probe);

        media->ops->close(media->ctx);

        if (valid && (!found || bundle_cmp(&s_probe, best) > 0))
        {
            *best = s_probe;
            found = true;
        }
    }

    return found;
}

// Copy the candidate into the secondary slot of `image` and check the copy against the
// candidate's SHA-256. Does not mark it pending. Returns 0 on success; on failure the slot
// is left erased.
static int copy_image(
    const mcuboot_media *media,
    uint8_t image,
    const struct flash_area *fa,
    uint32_t imageOff,
    const import_candidate *cand)
{
    uint32_t fileSize = 0;
    uint32_t size = cand->size;

    if (media->ops->open(media->ctx, cand->name, &fileSize) != 0 || fileSize != cand->fileSize)
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

        if (media->ops->read(media->ctx, cand->base + pos, s_chunk, len) != 0)
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

    // Hash what actually landed in flash and compare it with the file's own SHA-256 TLV:
    // catches a corrupt file, a short/garbled media read and a silently failing flash device.
    struct image_header check;
    uint8_t digest[32];
    Ifu_ImageCheck result = Ifu_VerifyImageAt(fa, imageOff, &check, digest);

    mcuboot_media_watchdog_feed();

    if (result != IFU_IMAGE_CHECK_OK || memcmp(digest, cand->hash, sizeof(digest)) != 0)
    {
        BOOT_LOG_ERR(
            "media import: image %u: staged copy rejected (%s), file corrupt or flash write failed",
            (unsigned)image,
            (result != IFU_IMAGE_CHECK_OK) ? Ifu_ImageCheckText(result) : "SHA-256 differs from the file");
        (void)flash_area_erase(fa, 0, fa->fa_size);
        return -1;
    }

    return 0;

fail:
    media->ops->close(media->ctx);
    (void)flash_area_erase(fa, 0, fa->fa_size);
    return -1;
}

// Schedule a one-time test swap of the copied image; the application confirms it after
// booting. Returns 0 on success; on failure the slot is left erased.
static int set_pending(uint8_t image, const struct flash_area *fa)
{
    if (boot_set_pending_multi(image, 0) != 0)
    {
        BOOT_LOG_ERR("media import: image %u: set pending failed", (unsigned)image);
        (void)flash_area_erase(fa, 0, fa->fa_size);
        return -1;
    }

    return 0;
}

// Check an image area and report whether it holds an intact image identical to the candidate.
static Ifu_ImageCheck check_slot(
    const struct flash_area *fa,
    uint32_t base,
    const import_candidate *cand,
    struct image_header *hdr,
    bool *identical)
{
    uint8_t digest[32];
    Ifu_ImageCheck result = Ifu_VerifyImageAt(fa, base, hdr, digest);

    *identical = (result == IFU_IMAGE_CHECK_OK) && (memcmp(digest, cand->hash, sizeof(digest)) == 0);

    mcuboot_media_watchdog_feed();

    return result;
}

// Decide what to do with the candidate given the state of both slots of `image`.
// `reason` receives a static string for the log.
static import_action decide(
    const mcuboot_media *media,
    uint8_t image,
    const struct flash_area *secondary,
    uint32_t imageOff,
    const import_candidate *cand,
    const char *stamp,
    const char **reason)
{
    struct boot_swap_state primaryState;
    struct boot_swap_state secondaryState;

    if (boot_read_swap_state_by_id(FLASH_AREA_IMAGE_PRIMARY(image), &primaryState) != 0)
    {
        *reason = "cannot read the primary slot trailer";
        return IMPORT_DEFER;
    }

    if (boot_read_swap_state_by_id(FLASH_AREA_IMAGE_SECONDARY(image), &secondaryState) != 0)
    {
        // no usable secondary slot (device not initialised, or no backend on this board):
        // nothing could be staged anyway
        *reason = "secondary slot cannot be read, no storage behind it";
        return IMPORT_SKIP;
    }

    if (mcuboot_swap_interrupted(&primaryState))
    {
        *reason = "interrupted swap, MCUboot has to finish it first";
        return IMPORT_DEFER;
    }

    // check what does the primary run
    const struct flash_area *primary = NULL;
    struct image_header primaryHdr;
    bool primaryIdentical = false;
    Ifu_ImageCheck primaryCheck = IFU_IMAGE_CHECK_READ_ERROR;

    if (flash_area_open(FLASH_AREA_IMAGE_PRIMARY(image), &primary) == 0 && primary != NULL)
    {
        primaryCheck = check_slot(primary, 0, cand, &primaryHdr, &primaryIdentical);
        flash_area_close(primary);
    }

    if (primaryIdentical)
    {
        *reason = "identical image (same SHA-256) already in primary";
        return IMPORT_SKIP_MARK;
    }

    bool primaryValid = (primaryCheck == IFU_IMAGE_CHECK_OK);

    if (stamp != NULL && media->ops->is_used(media->ctx, cand->name, stamp))
    {
        if (primaryValid)
        {
            *reason = "already imported by this device";
            return IMPORT_SKIP;
        }

        // erased/reflashed/corrupt device: the marker refers to a past life of this device
        BOOT_LOG_INF(
            "media import: image %u: used marker ignored, primary has no valid image (%s)",
            (unsigned)image,
            Ifu_ImageCheckText(primaryCheck));
    }

    int swapType = boot_swap_type_multi(image);
    struct image_header secondaryHdr;
    bool secondaryIdentical = false;

    switch (swapType)
    {
        case BOOT_SWAP_TYPE_NONE:
            *reason = primaryValid ? "different image in primary" : "primary has no valid image";
            return IMPORT_STAGE;

        case BOOT_SWAP_TYPE_REVERT:
            if (secondaryState.magic == BOOT_MAGIC_GOOD)
            {
                // swap-using-offset revert row keyed on the secondary trailer: revert mechanics
                // in progress
                *reason = "revert in progress";
                return IMPORT_DEFER;
            }

            if (check_slot(secondary, 0, cand, &secondaryHdr, &secondaryIdentical) == IFU_IMAGE_CHECK_OK)
            {
                *reason = "unconfirmed image in primary will be reverted first";
                return IMPORT_DEFER;
            }

            *reason = "unconfirmed image in primary and no valid image to revert to";
            return IMPORT_STAGE;

        case BOOT_SWAP_TYPE_TEST:
        case BOOT_SWAP_TYPE_PERM:
        {
            Ifu_ImageCheck pending = check_slot(secondary, imageOff, cand, &secondaryHdr, &secondaryIdentical);

            if (pending == IFU_IMAGE_CHECK_NO_HEADER && imageOff != 0)
            {
                pending = check_slot(secondary, 0, cand, &secondaryHdr, &secondaryIdentical);
            }

            if (secondaryIdentical)
            {
                *reason = "identical image already pending";
                return IMPORT_SKIP_MARK;
            }

            if (pending != IFU_IMAGE_CHECK_OK)
            {
                *reason = "pending image is not valid, replacing it";
                return IMPORT_STAGE;
            }

            BOOT_LOG_INF(
                "media import: image %u: replacing pending image %u.%u.%u+%u",
                (unsigned)image,
                (unsigned)secondaryHdr.ih_ver.iv_major,
                (unsigned)secondaryHdr.ih_ver.iv_minor,
                (unsigned)secondaryHdr.ih_ver.iv_revision,
                (unsigned)secondaryHdr.ih_ver.iv_build_num);

            *reason = "a different image was pending, the medium takes precedence";
            return IMPORT_STAGE;
        }

        default:
            *reason = "cannot determine the swap state";
            return IMPORT_DEFER;
    }
}

static void log_candidate(const mcuboot_media *media, uint8_t image, const import_candidate *cand)
{
    char hash[2 * IMPORT_HASH_PREFIX_BYTES + 1];

    hash_prefix(cand->hash, hash);

    BOOT_LOG_INF(
        "media import: %s: image %u candidate %s+0x%x (%u.%u.%u+%u, %u bytes, sha256 %s)",
        media->name,
        (unsigned)image,
        cand->name,
        (unsigned)cand->base,
        (unsigned)cand->hdr.ih_ver.iv_major,
        (unsigned)cand->hdr.ih_ver.iv_minor,
        (unsigned)cand->hdr.ih_ver.iv_revision,
        (unsigned)cand->hdr.ih_ver.iv_build_num,
        (unsigned)cand->size,
        hash);
}

// Log a decision that leaves the slots untouched and record the file when asked to.
static void apply_skip(
    const mcuboot_media *media,
    uint8_t image,
    import_action action,
    const char *reason,
    const import_candidate *cand,
    const char *stamp)
{
    if (action == IMPORT_DEFER)
    {
        BOOT_LOG_INF("media import: image %u: deferred to a later boot (%s)", (unsigned)image, reason);
        return;
    }

    BOOT_LOG_INF("media import: image %u: skipped (%s)", (unsigned)image, reason);

    if (action == IMPORT_SKIP_MARK && stamp != NULL && media->ops->mark_used(media->ctx, cand->name, stamp) != 0)
    {
        BOOT_LOG_WRN("media import: image %u: could not write used marker", (unsigned)image);
    }
}

// Handle one image index on one medium from loose files. Returns true when the image was staged.
static bool import_image(const mcuboot_media *media, uint8_t image)
{
    static import_candidate s_cand;
    const struct flash_area *fa = NULL;
    bool staged = false;
    char stamp[MCUBOOT_MEDIA_STAMP_MAX];
    const char *reason = "";

    if (flash_area_open(FLASH_AREA_IMAGE_SECONDARY(image), &fa) != 0 || fa == NULL)
    {
        BOOT_LOG_ERR("media import: image %u: no secondary slot", (unsigned)image);
        return false;
    }

    uint32_t imageOff = Ifu_ImageOffset(fa);
    uint32_t usable = Ifu_UsableSize(fa);

    if (!find_candidate(media, image, usable, &s_cand))
    {
        // no update file for this image on this medium
        goto done;
    }

    log_candidate(media, image, &s_cand);

    bool haveStamp = build_stamp(stamp, sizeof(stamp), &s_cand);
    import_action action = decide(media, image, fa, imageOff, &s_cand, haveStamp ? stamp : NULL, &reason);

    if (action != IMPORT_STAGE)
    {
        apply_skip(media, image, action, reason, &s_cand, haveStamp ? stamp : NULL);
        goto done;
    }

    BOOT_LOG_INF(
        "media import: image %u: staging into secondary slot at +0x%x (%s)",
        (unsigned)image,
        (unsigned)imageOff,
        reason);

    if (copy_image(media, image, fa, imageOff, &s_cand) != 0 || set_pending(image, fa) != 0)
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

// Handle the images carried by a bundle as one unit (see mcuboot_media_import.h).
// Sets staged[image] for every image that was staged.
static void import_bundle_images(const mcuboot_media *media, const import_bundle *bundle, bool staged[])
{
    const struct flash_area *fa[MCUBOOT_IMAGE_NUMBER] = {NULL};
    uint32_t imageOff[MCUBOOT_IMAGE_NUMBER] = {0};
    import_action action[MCUBOOT_IMAGE_NUMBER];
    const char *reason[MCUBOOT_IMAGE_NUMBER] = {NULL};
    static char s_stamp[MCUBOOT_IMAGE_NUMBER][MCUBOOT_MEDIA_STAMP_MAX];
    bool haveStamp[MCUBOOT_IMAGE_NUMBER] = {false};
    bool copied[MCUBOOT_IMAGE_NUMBER] = {false};
    bool defer = false;
    bool failed = false;
    uint8_t image;

    // 1. decide for every entry
    for (image = 0; image < MCUBOOT_IMAGE_NUMBER; image++)
    {
        if (!bundle->present[image])
        {
            continue;
        }

        const import_candidate *cand = &bundle->entry[image];

        log_candidate(media, image, cand);

        if (flash_area_open(FLASH_AREA_IMAGE_SECONDARY(image), &fa[image]) != 0 || fa[image] == NULL)
        {
            // probe_bundle checked the slot exists, so this is a flash backend failure
            fa[image] = NULL;
            action[image] = IMPORT_DEFER;
            reason[image] = "cannot open the secondary slot";
            defer = true;
            continue;
        }

        imageOff[image] = Ifu_ImageOffset(fa[image]);
        haveStamp[image] = build_stamp(s_stamp[image], sizeof(s_stamp[image]), cand);
        action[image] = decide(
            media,
            image,
            fa[image],
            imageOff[image],
            cand,
            haveStamp[image] ? s_stamp[image] : NULL,
            &reason[image]);

        if (action[image] == IMPORT_DEFER)
        {
            defer = true;
        }
    }

    // 2. half a bundle is what a bundle exists to avoid: defer all of it
    if (defer)
    {
        for (image = 0; image < MCUBOOT_IMAGE_NUMBER; image++)
        {
            if (bundle->present[image])
            {
                BOOT_LOG_INF(
                    "media import: image %u: deferred to a later boot with its bundle (%s)",
                    (unsigned)image,
                    (action[image] == IMPORT_DEFER) ? reason[image] : "another image of the bundle is deferred");
            }
        }

        goto done;
    }

    // 3. copy and check every image to stage before any of them is marked pending
    for (image = 0; image < MCUBOOT_IMAGE_NUMBER && !failed; image++)
    {
        if (!bundle->present[image] || action[image] != IMPORT_STAGE)
        {
            continue;
        }

        BOOT_LOG_INF(
            "media import: image %u: staging into secondary slot at +0x%x (%s)",
            (unsigned)image,
            (unsigned)imageOff[image],
            reason[image]);

        if (copy_image(media, image, fa[image], imageOff[image], &bundle->entry[image]) != 0)
        {
            failed = true;
            break;
        }

        copied[image] = true;
    }

    // 4. all landed intact: mark them pending, highest image index first (the deployment before the
    // nanoCLR). If power fails half way, a deployment pending alone is rejected by the nanoCLR it does
    // not match and reverts; a nanoCLR pending alone would be kept with a deployment it can't load.
    for (int i = MCUBOOT_IMAGE_NUMBER - 1; i >= 0 && !failed; i--)
    {
        if (copied[i] && set_pending((uint8_t)i, fa[i]) != 0)
        {
            failed = true;
        }
    }

    if (failed)
    {
        // erasing a secondary drops its trailer too, so nothing of the bundle is left pending
        for (image = 0; image < MCUBOOT_IMAGE_NUMBER; image++)
        {
            if (copied[image])
            {
                (void)flash_area_erase(fa[image], 0, fa[image]->fa_size);
            }
        }

        BOOT_LOG_ERR("media import: %s: bundle not staged, slots left erased", media->name);
        goto done;
    }

    // 5. report and record every entry
    for (image = 0; image < MCUBOOT_IMAGE_NUMBER; image++)
    {
        if (!bundle->present[image])
        {
            continue;
        }

        const import_candidate *cand = &bundle->entry[image];
        const char *stamp = haveStamp[image] ? s_stamp[image] : NULL;

        if (!copied[image])
        {
            apply_skip(media, image, action[image], reason[image], cand, stamp);
            continue;
        }

        staged[image] = true;

        BOOT_LOG_INF("media import: image %u: staged, pending test swap", (unsigned)image);

        if (stamp != NULL && media->ops->mark_used(media->ctx, cand->name, stamp) != 0)
        {
            BOOT_LOG_WRN("media import: image %u: could not write used marker (read-only medium?)", (unsigned)image);
        }
    }

done:
    for (image = 0; image < MCUBOOT_IMAGE_NUMBER; image++)
    {
        if (fa[image] != NULL)
        {
            flash_area_close(fa[image]);
        }
    }
}

void mcuboot_media_import_run(void)
{
    static import_bundle s_bundle;
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
        bool haveBundle = false;

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

        // a bundle takes precedence over loose files for the images it carries; it is only
        // considered while none of its images was staged from an earlier medium
        if (find_bundle(media, &s_bundle))
        {
            haveBundle = true;

            for (uint8_t image = 0; image < MCUBOOT_IMAGE_NUMBER; image++)
            {
                if (s_bundle.present[image] && imageDone[image])
                {
                    BOOT_LOG_INF(
                        "media import: %s: bundle %s ignored (image %u already staged from another medium)",
                        media->name,
                        s_bundle.entry[image].name,
                        (unsigned)image);
                    haveBundle = false;
                    break;
                }
            }
        }

        if (haveBundle)
        {
            bool staged[MCUBOOT_IMAGE_NUMBER] = {false};

            import_bundle_images(media, &s_bundle, staged);

            for (uint8_t image = 0; image < MCUBOOT_IMAGE_NUMBER; image++)
            {
                if (staged[image])
                {
                    imageDone[image] = true;
                    pending--;
                }
            }
        }

        for (uint8_t image = 0; image < MCUBOOT_IMAGE_NUMBER; image++)
        {
            if (imageDone[image] || (haveBundle && s_bundle.present[image]))
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
