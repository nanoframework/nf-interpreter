//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <string.h>
#include <flash_map_backend/flash_map_backend.h>
#include <sysflash/sysflash.h>
#include <bootutil/image.h>
#include <MCUboot_ImageSlotInfo.h>

int Ifu_FlashAreaId(uint8_t imageIndex, uint8_t slotIndex)
{
    if (slotIndex == IFU_SLOT_SECONDARY)
    {
        return FLASH_AREA_IMAGE_SECONDARY(imageIndex);
    }

    return FLASH_AREA_IMAGE_PRIMARY(imageIndex);
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

    if (flash_area_read(fa, 0, &hdr, sizeof(hdr)) == 0 && hdr.ih_magic == IMAGE_MAGIC)
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
        uint32_t tlvOff = (uint32_t)hdr.ih_hdr_size + hdr.ih_img_size + hdr.ih_protect_tlv_size;
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
