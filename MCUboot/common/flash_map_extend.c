//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <string.h>
#include <stdbool.h>

#include <sysflash/sysflash.h>

int flash_area_id_from_direct_image(int image_id)
{
    // Map the direct-image id (the "image" field carried by the SMP serial-recovery
    // upload command) to a flash area id. The numbering is fixed by mcuboot core -
    // bs_slot_info() (boot_serial.c) independently computes and advertises
    // upload_image_id = image_index * 2 + slot + 1 for every (image, slot) pair, and
    // the port's job is only to resolve an already-standardized id to a flash area:
    //   0 -> Image 0 (CLR)        primary (default/unspecified id)
    //   1 -> Image 0 (CLR)        primary       2 -> Image 0 (CLR)        secondary
    //   3 -> Image 1 (deployment) primary       4 -> Image 1 (deployment) secondary
    //
    // The FLASH_AREA_IMAGE_PRIMARY/SECONDARY macros (sysflash.h) already resolve to
    // the correct flash area id, so return them directly. This mapping must stay in
    // sync with GetImageIndex() in the nanoFirmwareFlasher (McubootManager.cs).
    switch (image_id)
    {
        case 0:
        case 1:
            return FLASH_AREA_IMAGE_PRIMARY(0);

        case 2:
            return FLASH_AREA_IMAGE_SECONDARY(0);

        case 3:
            return FLASH_AREA_IMAGE_PRIMARY(1);

        case 4:
            return FLASH_AREA_IMAGE_SECONDARY(1);

        default:
            return -1;
    }
}
