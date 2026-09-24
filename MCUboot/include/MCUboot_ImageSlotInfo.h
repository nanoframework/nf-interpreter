//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef MCUBOOT_IMAGE_SLOT_INFO_H
#define MCUBOOT_IMAGE_SLOT_INFO_H

#include <stdint.h>
#include <stdbool.h>
#include <flash_map_backend/flash_map_backend.h>

#ifdef __cplusplus
extern "C"
{
#endif

// Slot indices - shared by the Wire Protocol (Monitor_Image_Slot_Primary/Secondary) and the
// managed IFU interop (SlotId_Primary/Secondary); both already use 0 = primary, 1 = secondary.
#define IFU_SLOT_PRIMARY 0
#define IFU_SLOT_SECONDARY 1

// Best-effort snapshot of a single (image, slot) flash area's MCUboot image header + TLV area.
typedef struct
{
    // true as soon as the image header magic alone parses - MajorVersion/MinorVersion/
    // RevisionNumber/BuildNumber/Bootable are then meaningful
    bool HeaderValid;
    // true once the TLV info magic also parses - Hash/HasHash are then meaningful
    bool TlvValid;
    bool Bootable;
    uint8_t MajorVersion;
    uint8_t MinorVersion;
    uint16_t RevisionNumber;
    uint32_t BuildNumber;
    bool HasHash;
    uint8_t Hash[32];
} Ifu_SlotSnapshot;

// Resolve the flash area ID for a (image index, slot) pair.
int Ifu_FlashAreaId(uint8_t imageIndex, uint8_t slotIndex);

// Offset inside a SECONDARY slot where the upgrade image must start. This is the single source
// of truth for every writer (managed IFU sessions, Wire Protocol, media import) and reader.
// swap-using-offset keeps the first sector of the secondary slot free (MCUboot reads the header
// at boot_img_sector_size(secondary, 0)); every other swap mode expects the image at offset 0.
// Primary slots always hold the image at offset 0.
uint32_t Ifu_ImageOffset(const struct flash_area *fa);

// Largest image a secondary slot accepts. Under swap-using-offset one sector is skipped at the
// start (Ifu_ImageOffset) and the primary reserves one sector for the swap trailer, so two
// sectors come off the top. MCUboot performs the definitive check at swap time.
uint32_t Ifu_UsableSize(const struct flash_area *fa);

// SHA-256 over [offset, offset + length) of a flash area, streamed in small reads.
// Returns 0 on success, non-zero when a flash read failed. Keeps the tinycrypt dependency inside
// the MCUboot porting library.
int Ifu_HashFlashRange(const struct flash_area *fa, uint32_t offset, uint32_t length, uint8_t digest[32]);

// Read the image header and (best-effort) SHA-256 digest from a slot into *snapshot.
// For the secondary slot the header is read at Ifu_ImageOffset(fa).
void Ifu_ReadSlotInfo(uint8_t imageIndex, uint8_t slotIndex, Ifu_SlotSnapshot *snapshot);

#ifdef __cplusplus
}
#endif

#endif // MCUBOOT_IMAGE_SLOT_INFO_H
