//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <string.h>
#include <stdbool.h>

#include <sysflash/sysflash.h>

int flash_area_id_from_direct_image(int image_id)
{
    // load with index 1 which is the first updatable image (after the bootloader)
    int imageIndex = 1;

    switch (image_id)
    {
        case 0:
            imageIndex += FLASH_AREA_IMAGE_PRIMARY(0);
            break;

        case 1:
            imageIndex += FLASH_AREA_IMAGE_PRIMARY(1);
            break;

        case 2:
            imageIndex += FLASH_AREA_IMAGE_SECONDARY(0);
            break;

        case 3:
            imageIndex += FLASH_AREA_IMAGE_SECONDARY(1);
            break;

        default:
            return -1;
    }

    return imageIndex;
}
