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

// Copy buffer shared by all media (the sweep is strictly sequential).
static uint8_t s_chunk[MCUBOOT_IMPORT_CHUNK_SIZE];

// Bytes of the SHA-256 shown in log output and stored in the "used" stamp.
#define IMPORT_HASH_PREFIX_BYTES 8U

// Best candidate found for an image on a medium.
typedef struct
{
    char name[MCUBOOT_MEDIA_NAME_MAX];
    struct image_header hdr;
    uint32_t size;
    uint8_t hash[32];
} import_candidate;

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

// Read the SHA-256 TLV from the unprotected TLV area of the open file. 0 on success.
static int file_hash_tlv(const mcuboot_media *media, uint32_t size, const struct image_header *hdr, uint8_t hash[32])
{
    uint32_t tlvOff = Ifu_ImageHashedSize(hdr);
    struct image_tlv_info info;

    if (tlvOff == 0 || tlvOff > size || size - tlvOff < sizeof(info) ||
        media->ops->read(media->ctx, tlvOff, &info, sizeof(info)) != 0 || info.it_magic != IMAGE_TLV_INFO_MAGIC)
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

        if (media->ops->read(media->ctx, pos, &tlv, sizeof(tlv)) != 0)
        {
            return -1;
        }

        pos += sizeof(tlv);

        if (tlv.it_type == IMAGE_TLV_SHA256 && tlv.it_len == 32U && pos + 32U <= end)
        {
            return media->ops->read(media->ctx, pos, hash, 32U);
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
        uint8_t hash[32];

        if (media->ops->open(media->ctx, name, &size) != 0)
        {
            BOOT_LOG_WRN("media import: %s: cannot open %s", media->name, name);
            continue;
        }

        bool ok = (size >= sizeof(hdr)) && (media->ops->read(media->ctx, 0, &hdr, sizeof(hdr)) == 0);
        bool sane = ok && header_is_sane(&hdr, size, usable);
        bool hashed = sane && (file_hash_tlv(media, size, &hdr, hash) == 0);

        media->ops->close(media->ctx);

        if (!sane)
        {
            BOOT_LOG_WRN(
                "media import: %s: %s ignored (not a valid image %u file or larger than %u bytes)",
                media->name,
                name,
                (unsigned)image,
                (unsigned)usable);
            continue;
        }

        if (!hashed)
        {
            BOOT_LOG_WRN("media import: %s: %s ignored (no SHA-256 TLV)", media->name, name);
            continue;
        }

        if (!found || version_cmp(&hdr.ih_ver, &best->hdr.ih_ver) > 0)
        {
            strcpy(best->name, name);
            best->hdr = hdr;
            best->size = size;
            memcpy(best->hash, hash, sizeof(best->hash));
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

    if (primaryState.magic == BOOT_MAGIC_GOOD && primaryState.copy_done == BOOT_FLAG_UNSET &&
        primaryState.swap_type != BOOT_SWAP_TYPE_NONE)
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

// Handle one image index on one medium. Returns true when the image was staged.
static bool import_image(const mcuboot_media *media, uint8_t image)
{
    static import_candidate s_cand;
    const struct flash_area *fa = NULL;
    bool staged = false;
    char stamp[MCUBOOT_MEDIA_STAMP_MAX];
    char hash[2 * IMPORT_HASH_PREFIX_BYTES + 1];
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

    hash_prefix(s_cand.hash, hash);

    BOOT_LOG_INF(
        "media import: %s: image %u candidate %s (%u.%u.%u+%u, %u bytes, sha256 %s)",
        media->name,
        (unsigned)image,
        s_cand.name,
        (unsigned)s_cand.hdr.ih_ver.iv_major,
        (unsigned)s_cand.hdr.ih_ver.iv_minor,
        (unsigned)s_cand.hdr.ih_ver.iv_revision,
        (unsigned)s_cand.hdr.ih_ver.iv_build_num,
        (unsigned)s_cand.size,
        hash);

    bool haveStamp = build_stamp(stamp, sizeof(stamp), &s_cand);

    switch (decide(media, image, fa, imageOff, &s_cand, haveStamp ? stamp : NULL, &reason))
    {
        case IMPORT_SKIP_MARK:
            BOOT_LOG_INF("media import: image %u: skipped (%s)", (unsigned)image, reason);

            if (haveStamp && media->ops->mark_used(media->ctx, s_cand.name, stamp) != 0)
            {
                BOOT_LOG_WRN("media import: image %u: could not write used marker", (unsigned)image);
            }
            goto done;

        case IMPORT_SKIP:
            BOOT_LOG_INF("media import: image %u: skipped (%s)", (unsigned)image, reason);
            goto done;

        case IMPORT_DEFER:
            BOOT_LOG_INF("media import: image %u: deferred to a later boot (%s)", (unsigned)image, reason);
            goto done;

        case IMPORT_STAGE:
        default:
            break;
    }

    BOOT_LOG_INF(
        "media import: image %u: staging into secondary slot at +0x%x (%s)",
        (unsigned)image,
        (unsigned)imageOff,
        reason);

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
