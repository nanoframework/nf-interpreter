//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <string.h>
#include <mcuboot_config/mcuboot_config.h>
#include <flash_map_backend/flash_map_backend.h>
#include <sysflash/sysflash.h>
#include <bootutil/image.h>
#include <tinycrypt/sha256.h>
#include <MCUboot_ImageSlotInfo.h>

// read granularity of the streaming hash
#define IFU_HASH_CHUNK 256U

int Ifu_FlashAreaId(uint8_t imageIndex, uint8_t slotIndex)
{
    if (slotIndex == IFU_SLOT_SECONDARY)
    {
        return FLASH_AREA_IMAGE_SECONDARY(imageIndex);
    }

    return FLASH_AREA_IMAGE_PRIMARY(imageIndex);
}

int Ifu_HashFlashRange(const struct flash_area *fa, uint32_t offset, uint32_t length, uint8_t digest[32])
{
    struct tc_sha256_state_struct sha;
    uint8_t buffer[IFU_HASH_CHUNK];
    uint32_t pos;

    tc_sha256_init(&sha);

    for (pos = 0U; pos < length; pos += IFU_HASH_CHUNK)
    {
        uint32_t chunk = (length - pos < IFU_HASH_CHUNK) ? (length - pos) : IFU_HASH_CHUNK;

        if (flash_area_read(fa, offset + pos, buffer, chunk) != 0)
        {
            return -1;
        }

        tc_sha256_update(&sha, buffer, chunk);
    }

    tc_sha256_final(digest, &sha);

    return 0;
}

uint32_t Ifu_ImageOffset(const struct flash_area *fa)
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

uint32_t Ifu_UsableSize(const struct flash_area *fa)
{
    uint32_t reserved = 2U * Ifu_ImageOffset(fa);

    return (fa->fa_size > reserved) ? (fa->fa_size - reserved) : 0U;
}

void Ifu_ReadSlotInfo(uint8_t imageIndex, uint8_t slotIndex, Ifu_SlotSnapshot *snapshot)
{
    memset(snapshot, 0, sizeof(*snapshot));

    int faId = Ifu_FlashAreaId(imageIndex, slotIndex);
    if (faId == FLASH_SLOT_DOES_NOT_EXIST)
    {
        return;
    }

    const struct flash_area *fa = NULL;
    if (flash_area_open((uint8_t)faId, &fa) != 0 || fa == NULL)
    {
        return;
    }

    struct image_header hdr;

    // the secondary slot holds the image at the offset the swap mode expects; primary at 0
    uint32_t base = (slotIndex == IFU_SLOT_SECONDARY) ? Ifu_ImageOffset(fa) : 0U;

    if (flash_area_read(fa, base, &hdr, sizeof(hdr)) == 0 && hdr.ih_magic == IMAGE_MAGIC)
    {
        snapshot->HeaderValid = true;

        snapshot->MajorVersion = hdr.ih_ver.iv_major;
        snapshot->MinorVersion = hdr.ih_ver.iv_minor;
        snapshot->RevisionNumber = hdr.ih_ver.iv_revision;
        snapshot->BuildNumber = hdr.ih_ver.iv_build_num;

        if (!(hdr.ih_flags & IMAGE_F_NON_BOOTABLE))
        {
            snapshot->Bootable = true;
        }

        // Locate the unprotected TLV area and extract the SHA-256 digest (best-effort).
        uint32_t tlvOff = base + (uint32_t)hdr.ih_hdr_size + hdr.ih_img_size + hdr.ih_protect_tlv_size;
        struct image_tlv_info tlvInfo;

        if (flash_area_read(fa, tlvOff, &tlvInfo, sizeof(tlvInfo)) == 0 && tlvInfo.it_magic == IMAGE_TLV_INFO_MAGIC)
        {
            snapshot->TlvValid = true;

            uint32_t pos = tlvOff + sizeof(tlvInfo);
            uint32_t end = tlvOff + tlvInfo.it_tlv_tot;

            while (pos + sizeof(struct image_tlv) <= end)
            {
                struct image_tlv tlv;
                if (flash_area_read(fa, pos, &tlv, sizeof(tlv)) != 0)
                {
                    break;
                }

                pos += sizeof(tlv);

                if (tlv.it_type == IMAGE_TLV_SHA256 && tlv.it_len == sizeof(snapshot->Hash))
                {
                    if (flash_area_read(fa, pos, snapshot->Hash, sizeof(snapshot->Hash)) == 0)
                    {
                        snapshot->HasHash = true;
                    }
                    break;
                }

                pos += tlv.it_len;
            }
        }
    }

    flash_area_close(fa);
}
