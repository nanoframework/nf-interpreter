//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef MCUBOOT_IMAGE_SLOT_INFO_H
#define MCUBOOT_IMAGE_SLOT_INFO_H

#include <stdint.h>
#include <stdbool.h>

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

// Read the image header and (best-effort) SHA-256 digest from a slot into *snapshot.
void Ifu_ReadSlotInfo(uint8_t imageIndex, uint8_t slotIndex, Ifu_SlotSnapshot *snapshot);

#ifdef __cplusplus
}
#endif

#endif // MCUBOOT_IMAGE_SLOT_INFO_H
