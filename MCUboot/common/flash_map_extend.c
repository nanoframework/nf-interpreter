//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <string.h>
#include <stdbool.h>

#include <sysflash/sysflash.h>

int flash_area_id_from_direct_image(int image_id)
{
    switch (image_id)
    {
        case 0:
            return FLASH_AREA_IMAGE_PRIMARY(0);

        case 1:
            return FLASH_AREA_IMAGE_PRIMARY(1);

        case 2:
            return FLASH_AREA_IMAGE_SECONDARY(0);

        case 3:
            return FLASH_AREA_IMAGE_SECONDARY(1);
    }

    return -1;
}
