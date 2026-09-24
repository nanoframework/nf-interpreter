//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Update session registry - see MCUboot_UpdateSession.h and MCUboot/docs/session-update.md.
//
// Concurrency model: every read or write of the registry happens between Ifu_SessionLock() and
// Ifu_SessionUnlock(). Flash I/O never runs under the lock - it is serialised by the token gate
// plus the WriteInFlight flag, which makes any other registry operation on that image fail with
// UpdateSessionResult_Busy while a flash operation is executing. No function holds a session
// pointer across an unlock: after the flash work each one looks the session up again by token, so
// a registry reset (CLR reboot) that happened meanwhile can never be written over.
//
// Resume soundness notes:
//  - The high-water mark is the last byte that differs from the erased value. Payload bytes that
//    happen to equal the erased value at the tail make it under-estimate, which is harmless: the
//    caller re-sends bytes that are already stored, and re-programming a NOR byte with the erased
//    value is a no-op.
//  - A page that was being programmed when power failed cannot be repaired by writing it again
//    (bits only ever clear), so the erase block containing the high-water mark is always erased
//    and the caller restarts from its beginning. This also keeps ECC / write-once internal flash
//    happy: the resumed range is always freshly erased.
//  - The scan is bounded by the declared total length, which is <= the usable size, so it never
//    reaches the swap trailer at the end of the slot.
//  - After a completed swap the secondary slot holds the *previous* primary image, with a valid
//    header at the same offset. Only the caller can tell that apart from a paused download - hence
//    the expectedHeader parameter.
//
// Every operation here touches the SECONDARY slot only. Whatever goes wrong - an interrupted
// write, a reset registry, a corrupt chunk - the running image in the primary slot is untouched,
// and a damaged staged image is rejected by the hash check at completion or by MCUboot's signature
// check at boot. A failed update can never brick the device.

#include <string.h>
// first: brings in the assembly header (managed enums) and, through it, nanoHAL
#include <MCUboot_UpdateSession.h>
#include <mcuboot_config/mcuboot_config.h>
#include <flash_map_backend/flash_map_backend.h>
#include <sysflash/sysflash.h>
#include <bootutil/bootutil_public.h>
#include <bootutil/image.h>
#include <MCUboot_ImageSlotInfo.h>

// scratch size for the resume scan
#define IFU_SCAN_CHUNK 256U

static Ifu_Session s_sessions[MCUBOOT_IMAGE_NUMBER];
// set while Ifu_EraseSecondary() is erasing an image's slot with no session open
static bool s_eraseInFlight[MCUBOOT_IMAGE_NUMBER];
static UpdateSessionResult s_lastStatus = UpdateSessionResult_Success;
// Not cleared by Ifu_SessionResetAll(): keeping it monotonic across a CLR-only reboot means a
// token can never be reused while anything might still be holding the previous one.
static uint32_t s_tokenCounter;

__attribute__((weak)) void Ifu_SessionLock(void)
{
}

__attribute__((weak)) void Ifu_SessionUnlock(void)
{
}

// Record the outcome of an operation and hand it back to the caller in one step.
static UpdateSessionResult set_status(UpdateSessionResult status)
{
    Ifu_SessionLock();

    s_lastStatus = status;

    Ifu_SessionUnlock();

    return status;
}

static bool image_valid(uint8_t image)
{
    return image < MCUBOOT_IMAGE_NUMBER;
}

static bool owner_valid(UpdateSessionOwner owner)
{
    return owner == UpdateSessionOwner_Managed || owner == UpdateSessionOwner_WireProtocol ||
           owner == UpdateSessionOwner_Native;
}

static uint32_t make_token(uint8_t image, uint32_t seed)
{
    uint32_t token = (++s_tokenCounter << 16) ^ seed ^ ((uint32_t)image << 8);

    return (token == 0U) ? 1U : token;
}

static UpdateSessionResult open_secondary(uint8_t image, const struct flash_area **fa)
{
    int faId = Ifu_FlashAreaId(image, IFU_SLOT_SECONDARY);

    if (faId == FLASH_SLOT_DOES_NOT_EXIST)
    {
        return UpdateSessionResult_NoSlot;
    }

    if (flash_area_open((uint8_t)faId, fa) != 0 || *fa == NULL)
    {
        return UpdateSessionResult_FlashError;
    }

    return UpdateSessionResult_Success;
}

// header + payload + protected TLVs; false when the arithmetic overflows
static bool header_span(const struct image_header *hdr, uint32_t *span)
{
    uint32_t s = (uint32_t)hdr->ih_hdr_size + hdr->ih_img_size + hdr->ih_protect_tlv_size;

    if (s < hdr->ih_img_size)
    {
        return false;
    }

    *span = s;
    return true;
}

// Locate the session an operation refers to. Must be called under the lock.
static UpdateSessionResult find_session(uint8_t image, uint32_t token, Ifu_Session **session)
{
    if (!image_valid(image))
    {
        return UpdateSessionResult_BadArgument;
    }

    Ifu_Session *s = &s_sessions[image];

    if (!s->Open || s->Token != token)
    {
        return UpdateSessionResult_BadToken;
    }

    if (s->WriteInFlight)
    {
        return UpdateSessionResult_Busy;
    }

    *session = s;
    return UpdateSessionResult_Success;
}

// Release a session claimed with WriteInFlight after its flash work finished. The session is
// looked up by token again, so a registry reset in the meantime simply wins: nothing is written
// back over the cleared state and the caller is told its token is gone.
static UpdateSessionResult finish_claim(uint8_t image, uint32_t token, bool keepOpen, Ifu_Session *out)
{
    UpdateSessionResult status = UpdateSessionResult_Success;

    Ifu_SessionLock();

    Ifu_Session *s = &s_sessions[image];

    if (!s->Open || s->Token != token)
    {
        // the registry was reset (or taken over) while the flash work was running
        status = UpdateSessionResult_BadToken;
    }
    else
    {
        s->WriteInFlight = false;

        if (out != NULL)
        {
            *out = *s;
        }

        if (!keepOpen)
        {
            memset(s, 0, sizeof(*s));
        }
    }

    Ifu_SessionUnlock();

    return status;
}

// Common front half of Start/Resume: argument checks, slot geometry, swap state, ownership.
// On success the session is claimed with WriteInFlight set, so the caller can run its flash work
// without the lock, and *token carries the identity to finish with.
static UpdateSessionResult claim_session(
    uint8_t image,
    UpdateSessionOwner owner,
    uint32_t totalLength,
    uint32_t seed,
    const struct flash_area **fa,
    uint32_t *token,
    uint32_t *imageOffsetOut)
{
    if (!image_valid(image) || !owner_valid(owner) || totalLength < sizeof(struct image_header))
    {
        return UpdateSessionResult_BadArgument;
    }

    UpdateSessionResult status = open_secondary(image, fa);
    if (status != UpdateSessionResult_Success)
    {
        return status;
    }

    uint32_t imageOffset = Ifu_ImageOffset(*fa);
    uint32_t usable = Ifu_UsableSize(*fa);

    if (totalLength > usable)
    {
        return UpdateSessionResult_TooLarge;
    }

    if (boot_swap_type_multi(image) != BOOT_SWAP_TYPE_NONE)
    {
        return UpdateSessionResult_SwapInFlight;
    }

    Ifu_SessionLock();

    Ifu_Session *s = &s_sessions[image];

    // a session held by someone else, a flash operation in progress, or an erase in progress all
    // block us; the same owner re-opening replaces its own (possibly abandoned) session
    if ((s->Open && (s->Owner != owner || s->WriteInFlight)) || s_eraseInFlight[image])
    {
        Ifu_SessionUnlock();
        return UpdateSessionResult_Busy;
    }

    memset(s, 0, sizeof(*s));
    s->Open = true;
    s->WriteInFlight = true;
    s->Image = image;
    s->Owner = owner;
    s->Token = make_token(image, seed);
    s->ImageOffset = imageOffset;
    s->UsableSize = usable;
    s->TotalLength = totalLength;

    *token = s->Token;
    *imageOffsetOut = imageOffset;

    Ifu_SessionUnlock();

    return UpdateSessionResult_Success;
}

UpdateSessionResult Ifu_SessionStart(
    uint8_t image,
    UpdateSessionOwner owner,
    uint32_t totalLength,
    uint32_t seed,
    Ifu_Session *out)
{
    const struct flash_area *fa = NULL;
    uint32_t token = 0U;
    uint32_t imageOffset = 0U;

    if (out == NULL)
    {
        return set_status(UpdateSessionResult_BadArgument);
    }

    UpdateSessionResult status = claim_session(image, owner, totalLength, seed, &fa, &token, &imageOffset);
    if (status != UpdateSessionResult_Success)
    {
        return set_status(status);
    }

    // whole slot, including the swap trailer, so no stale pending marker can survive
    bool erased = (flash_area_erase(fa, 0, fa->fa_size) == 0);

    flash_area_close(fa);

    status = finish_claim(image, token, erased, out);

    if (status == UpdateSessionResult_Success && !erased)
    {
        status = UpdateSessionResult_FlashError;
    }

    return set_status(status);
}

// Find the last byte in [0, length) of the image that is not the erased value. Returns 0 when the
// whole range reads as erased.
static UpdateSessionResult high_water_mark(
    const struct flash_area *fa,
    uint32_t imageOffset,
    uint32_t length,
    uint32_t *hwm)
{
    uint8_t buffer[IFU_SCAN_CHUNK];
    uint8_t erased = flash_area_erased_val(fa);
    uint32_t pos = length;

    while (pos > 0U)
    {
        uint32_t chunk = (pos < IFU_SCAN_CHUNK) ? pos : IFU_SCAN_CHUNK;
        uint32_t start = pos - chunk;

        if (flash_area_read(fa, imageOffset + start, buffer, chunk) != 0)
        {
            return UpdateSessionResult_FlashError;
        }

        for (uint32_t i = chunk; i > 0U; i--)
        {
            if (buffer[i - 1U] != erased)
            {
                *hwm = start + i;
                return UpdateSessionResult_Success;
            }
        }

        pos = start;
    }

    *hwm = 0U;
    return UpdateSessionResult_Success;
}

UpdateSessionResult Ifu_SessionResume(
    uint8_t image,
    UpdateSessionOwner owner,
    uint32_t totalLength,
    const uint8_t *expectedHeader,
    uint32_t expectedHeaderLen,
    uint32_t seed,
    Ifu_Session *out)
{
    const struct flash_area *fa = NULL;
    struct image_header hdr;
    struct image_tlv_info tlvInfo;
    uint32_t token = 0U;
    uint32_t imageOffset = 0U;
    uint32_t span = 0U;
    uint32_t hwm = 0U;
    uint32_t resumeOffset = 0U;
    struct flash_sector sector;

    if (out == NULL)
    {
        return set_status(UpdateSessionResult_BadArgument);
    }

    UpdateSessionResult status = claim_session(image, owner, totalLength, seed, &fa, &token, &imageOffset);
    if (status != UpdateSessionResult_Success)
    {
        return set_status(status);
    }

    // 1. the stored header identifies the image
    if (flash_area_read(fa, imageOffset, &hdr, sizeof(hdr)) != 0)
    {
        status = UpdateSessionResult_FlashError;
        goto done;
    }

    if (hdr.ih_magic != IMAGE_MAGIC)
    {
        status = UpdateSessionResult_NoImage;
        goto done;
    }

    if (expectedHeader != NULL)
    {
        uint32_t cmp = (expectedHeaderLen < sizeof(hdr)) ? expectedHeaderLen : (uint32_t)sizeof(hdr);

        if (memcmp(&hdr, expectedHeader, cmp) != 0)
        {
            status = UpdateSessionResult_HeaderMismatch;
            goto done;
        }
    }

    if (!header_span(&hdr, &span) || span > totalLength)
    {
        status = UpdateSessionResult_HeaderMismatch;
        goto done;
    }

    // 2. when the unprotected TLV info is already stored, the exact image length is known
    if (span + sizeof(tlvInfo) <= totalLength)
    {
        if (flash_area_read(fa, imageOffset + span, &tlvInfo, sizeof(tlvInfo)) != 0)
        {
            status = UpdateSessionResult_FlashError;
            goto done;
        }

        if (tlvInfo.it_magic == IMAGE_TLV_INFO_MAGIC && span + tlvInfo.it_tlv_tot > totalLength)
        {
            status = UpdateSessionResult_HeaderMismatch;
            goto done;
        }
    }

    // 3. how far did the previous session get
    status = high_water_mark(fa, imageOffset, totalLength, &hwm);
    if (status != UpdateSessionResult_Success)
    {
        goto done;
    }

    if (hwm == 0U)
    {
        status = UpdateSessionResult_NoImage;
        goto done;
    }

    // 4. rewind to the start of the erase block holding the high-water mark and erase it: the
    //    tail of that block may be partially programmed and cannot be repaired by rewriting
    if (flash_area_get_sector(fa, imageOffset + hwm - 1U, &sector) != 0)
    {
        status = UpdateSessionResult_FlashError;
        goto done;
    }

    if (sector.fs_off <= imageOffset)
    {
        // the header itself lives in that block: nothing worth keeping, start over
        status = UpdateSessionResult_NoImage;
        goto done;
    }

    resumeOffset = sector.fs_off - imageOffset;

    if (resumeOffset < hwm && flash_area_erase(fa, sector.fs_off, sector.fs_size) != 0)
    {
        status = UpdateSessionResult_FlashError;
        goto done;
    }

    status = UpdateSessionResult_Success;

done:
    flash_area_close(fa);

    if (status != UpdateSessionResult_Success)
    {
        // nothing resumable was found: drop the claim and report why
        (void)finish_claim(image, token, false, NULL);

        return set_status(status);
    }

    Ifu_SessionLock();

    Ifu_Session *s = &s_sessions[image];

    if (s->Open && s->Token == token)
    {
        s->NextOffset = resumeOffset;
        s->HeaderKnown = true;
        s->Header = hdr;
    }

    Ifu_SessionUnlock();

    return set_status(finish_claim(image, token, true, out));
}

UpdateSessionResult Ifu_SessionWrite(
    uint8_t image,
    uint32_t token,
    const uint8_t *data,
    uint32_t len,
    uint32_t *nextOffset)
{
    const struct flash_area *fa = NULL;
    Ifu_Session *s = NULL;
    struct image_header hdr;
    uint32_t imageOffset = 0U;
    uint32_t writeAt = 0U;
    bool firstChunk = false;

    if (data == NULL && len != 0U)
    {
        return set_status(UpdateSessionResult_BadArgument);
    }

    Ifu_SessionLock();

    UpdateSessionResult status = find_session(image, token, &s);
    if (status != UpdateSessionResult_Success)
    {
        Ifu_SessionUnlock();
        return set_status(status);
    }

    if (len == 0U)
    {
        if (nextOffset != NULL)
        {
            *nextOffset = s->NextOffset;
        }

        Ifu_SessionUnlock();
        return set_status(UpdateSessionResult_Success);
    }

    if (s->NextOffset + len > s->TotalLength || s->NextOffset + len < s->NextOffset)
    {
        Ifu_SessionUnlock();
        return set_status(UpdateSessionResult_TooLarge);
    }

    if (s->NextOffset == 0U)
    {
        uint32_t span = 0U;

        firstChunk = true;

        if (len < sizeof(hdr))
        {
            Ifu_SessionUnlock();
            return set_status(UpdateSessionResult_BadMagic);
        }

        memcpy(&hdr, data, sizeof(hdr));

        if (hdr.ih_magic != IMAGE_MAGIC || !header_span(&hdr, &span) || span > s->TotalLength)
        {
            Ifu_SessionUnlock();
            return set_status(UpdateSessionResult_BadMagic);
        }
    }

    imageOffset = s->ImageOffset;
    writeAt = s->NextOffset;
    s->WriteInFlight = true;

    Ifu_SessionUnlock();

    status = open_secondary(image, &fa);

    if (status == UpdateSessionResult_Success)
    {
        if (flash_area_write(fa, imageOffset + writeAt, data, len) != 0)
        {
            status = UpdateSessionResult_FlashError;
        }

        flash_area_close(fa);
    }

    Ifu_SessionLock();

    s = &s_sessions[image];

    if (!s->Open || s->Token != token)
    {
        // the registry was reset while the write was running; the chunk landed in the staging
        // slot but nobody owns it any more, so the caller must start over
        status = UpdateSessionResult_BadToken;
    }
    else
    {
        s->WriteInFlight = false;

        if (status == UpdateSessionResult_Success)
        {
            s->NextOffset += len;

            if (firstChunk)
            {
                s->HeaderKnown = true;
                s->Header = hdr;
            }
        }

        if (nextOffset != NULL)
        {
            *nextOffset = s->NextOffset;
        }
    }

    Ifu_SessionUnlock();

    return set_status(status);
}

// Structural + hash verification of the staged image. Runs without the lock (WriteInFlight set).
static UpdateSessionResult verify_image(
    const struct flash_area *fa,
    uint32_t imageOffset,
    uint32_t totalLength,
    struct image_header *hdr)
{
    struct image_tlv_info tlvInfo;
    struct image_tlv tlv;
    uint8_t expected[32];
    uint8_t computed[32];
    uint32_t span = 0U;
    uint32_t pos;
    uint32_t end;
    bool haveHash = false;

    if (flash_area_read(fa, imageOffset, hdr, sizeof(*hdr)) != 0)
    {
        return UpdateSessionResult_FlashError;
    }

    if (hdr->ih_magic != IMAGE_MAGIC || !header_span(hdr, &span))
    {
        return UpdateSessionResult_BadMagic;
    }

    if (span + sizeof(tlvInfo) > totalLength)
    {
        return UpdateSessionResult_BadTlv;
    }

    if (flash_area_read(fa, imageOffset + span, &tlvInfo, sizeof(tlvInfo)) != 0)
    {
        return UpdateSessionResult_FlashError;
    }

    if (tlvInfo.it_magic != IMAGE_TLV_INFO_MAGIC || tlvInfo.it_tlv_tot < sizeof(tlvInfo) ||
        span + tlvInfo.it_tlv_tot > totalLength)
    {
        return UpdateSessionResult_BadTlv;
    }

    // walk the unprotected TLVs looking for the SHA-256 digest
    pos = span + sizeof(tlvInfo);
    end = span + tlvInfo.it_tlv_tot;

    while (pos + sizeof(tlv) <= end)
    {
        if (flash_area_read(fa, imageOffset + pos, &tlv, sizeof(tlv)) != 0)
        {
            return UpdateSessionResult_FlashError;
        }

        pos += sizeof(tlv);

        if (pos + tlv.it_len > end)
        {
            return UpdateSessionResult_BadTlv;
        }

        if (tlv.it_type == IMAGE_TLV_SHA256 && tlv.it_len == sizeof(expected))
        {
            if (flash_area_read(fa, imageOffset + pos, expected, sizeof(expected)) != 0)
            {
                return UpdateSessionResult_FlashError;
            }

            haveHash = true;
            break;
        }

        pos += tlv.it_len;
    }

    if (!haveHash)
    {
        return UpdateSessionResult_BadTlv;
    }

    // SHA-256 over header + payload + protected TLVs, exactly what imgtool hashed
    if (Ifu_HashFlashRange(fa, imageOffset, span, computed) != 0)
    {
        return UpdateSessionResult_FlashError;
    }

    if (memcmp(computed, expected, sizeof(expected)) != 0)
    {
        return UpdateSessionResult_HashMismatch;
    }

    return UpdateSessionResult_Success;
}

UpdateSessionResult Ifu_SessionComplete(uint8_t image, uint32_t token, Ifu_Session *outSnapshot)
{
    const struct flash_area *fa = NULL;
    Ifu_Session *s = NULL;
    struct image_header hdr;
    uint32_t imageOffset = 0U;
    uint32_t totalLength = 0U;

    Ifu_SessionLock();

    UpdateSessionResult status = find_session(image, token, &s);
    if (status != UpdateSessionResult_Success)
    {
        Ifu_SessionUnlock();
        return set_status(status);
    }

    if (s->NextOffset != s->TotalLength)
    {
        if (outSnapshot != NULL)
        {
            *outSnapshot = *s;
        }

        Ifu_SessionUnlock();
        return set_status(UpdateSessionResult_Incomplete);
    }

    imageOffset = s->ImageOffset;
    totalLength = s->TotalLength;
    s->WriteInFlight = true;

    Ifu_SessionUnlock();

    status = open_secondary(image, &fa);

    if (status == UpdateSessionResult_Success)
    {
        status = verify_image(fa, imageOffset, totalLength, &hdr);

        flash_area_close(fa);
    }

    // schedule a revertible test swap; the application confirms after it has validated itself
    if (status == UpdateSessionResult_Success && boot_set_pending_multi(image, 0) != 0)
    {
        status = UpdateSessionResult_FlashError;
    }

    Ifu_SessionLock();

    s = &s_sessions[image];

    if (s->Open && s->Token == token)
    {
        if (status == UpdateSessionResult_Success)
        {
            s->HeaderKnown = true;
            s->Header = hdr;
        }

        s->WriteInFlight = false;

        if (outSnapshot != NULL)
        {
            *outSnapshot = *s;
        }

        // the session is done either way: a verification failure leaves the slot as it is and the
        // caller decides between aborting with an erase and starting over
        memset(s, 0, sizeof(*s));
    }
    else if (status == UpdateSessionResult_Success)
    {
        status = UpdateSessionResult_BadToken;
    }

    Ifu_SessionUnlock();

    return set_status(status);
}

UpdateSessionResult Ifu_SessionAbort(uint8_t image, uint32_t token, bool eraseSlot)
{
    const struct flash_area *fa = NULL;
    Ifu_Session *s = NULL;

    Ifu_SessionLock();

    UpdateSessionResult status = find_session(image, token, &s);
    if (status != UpdateSessionResult_Success)
    {
        Ifu_SessionUnlock();
        return set_status(status);
    }

    // keep the session claimed while erasing so nobody can open on top of a half-erased slot
    s->WriteInFlight = true;

    Ifu_SessionUnlock();

    if (eraseSlot)
    {
        status = open_secondary(image, &fa);

        if (status == UpdateSessionResult_Success)
        {
            if (flash_area_erase(fa, 0, fa->fa_size) != 0)
            {
                status = UpdateSessionResult_FlashError;
            }

            flash_area_close(fa);
        }
    }

    // close it whatever the erase did: the caller asked to let go of the session
    (void)finish_claim(image, token, false, NULL);

    return set_status(status);
}

UpdateSessionResult Ifu_EraseSecondary(uint8_t image)
{
    const struct flash_area *fa = NULL;

    if (!image_valid(image))
    {
        return set_status(UpdateSessionResult_BadArgument);
    }

    Ifu_SessionLock();

    if (s_sessions[image].Open || s_eraseInFlight[image])
    {
        Ifu_SessionUnlock();
        return set_status(UpdateSessionResult_Busy);
    }

    s_eraseInFlight[image] = true;

    Ifu_SessionUnlock();

    UpdateSessionResult status = open_secondary(image, &fa);

    if (status == UpdateSessionResult_Success)
    {
        if (flash_area_erase(fa, 0, fa->fa_size) != 0)
        {
            status = UpdateSessionResult_FlashError;
        }

        flash_area_close(fa);
    }

    Ifu_SessionLock();

    s_eraseInFlight[image] = false;

    Ifu_SessionUnlock();

    return set_status(status);
}

UpdateSessionOwner Ifu_SessionOwner(uint8_t image)
{
    UpdateSessionOwner owner = UpdateSessionOwner_None;

    if (!image_valid(image))
    {
        return owner;
    }

    Ifu_SessionLock();

    if (s_sessions[image].Open)
    {
        owner = s_sessions[image].Owner;
    }

    Ifu_SessionUnlock();

    return owner;
}

uint32_t Ifu_SessionToken(uint8_t image, UpdateSessionOwner owner)
{
    uint32_t token = 0U;

    if (!image_valid(image))
    {
        return token;
    }

    Ifu_SessionLock();

    if (s_sessions[image].Open && s_sessions[image].Owner == owner)
    {
        token = s_sessions[image].Token;
    }

    Ifu_SessionUnlock();

    return token;
}

UpdateSessionResult Ifu_SessionLastStatus(void)
{
    UpdateSessionResult status;

    Ifu_SessionLock();

    status = s_lastStatus;

    Ifu_SessionUnlock();

    return status;
}

void Ifu_SessionResetAll(void)
{
    Ifu_SessionLock();

    // RAM state only. Flash is deliberately left alone: a paused download stays in the staging
    // slot and Ifu_SessionResume() picks it up again after the reboot.
    memset(s_sessions, 0, sizeof(s_sessions));
    memset(s_eraseInFlight, 0, sizeof(s_eraseInFlight));

    s_lastStatus = UpdateSessionResult_Success;

    Ifu_SessionUnlock();
}
