//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Implements the update guard described in MCUboot_UpdateGuard.h

#include <stdint.h>
#include <stdbool.h>

#include <mcuboot_config/mcuboot_config.h>
#include <flash_map_backend/flash_map_backend.h>
#include <sysflash/sysflash.h>
#include <bootutil/bootutil_public.h>
#include <bootutil/bootutil_log.h>

#include <MCUboot_UpdateGuard.h>

// Weak default: no platform specific veto.
__attribute__((weak)) bool mcuboot_update_guard_platform_veto(void)
{
    return false;
}

bool mcuboot_swap_interrupted(const struct boot_swap_state *primary)
{
    return primary->magic == BOOT_MAGIC_GOOD && primary->copy_done == BOOT_FLAG_UNSET &&
           primary->swap_type != BOOT_SWAP_TYPE_NONE;
}

mcuboot_update_state_t mcuboot_update_state(uint8_t image)
{
    struct boot_swap_state primary;

    if (boot_read_swap_state_by_id(FLASH_AREA_IMAGE_PRIMARY(image), &primary) != 0)
    {
        return MCUBOOT_UPDATE_STATE_UNKNOWN;
    }

    if (mcuboot_swap_interrupted(&primary))
    {
        return MCUBOOT_UPDATE_SWAP_INTERRUPTED;
    }

    // reads both trailers; an unreachable secondary counts as empty, any other failure is PANIC
    switch (boot_swap_type_multi(image))
    {
        case BOOT_SWAP_TYPE_NONE:
            return MCUBOOT_UPDATE_NONE;

        case BOOT_SWAP_TYPE_PANIC:
            return MCUBOOT_UPDATE_STATE_UNKNOWN;

        default:
            // TEST, PERM, REVERT or FAIL: anything other than NONE is not idle
            return MCUBOOT_UPDATE_SWAP_PENDING;
    }
}

bool mcuboot_update_in_progress(void)
{
    if (mcuboot_update_guard_platform_veto())
    {
        BOOT_LOG_INF("update guard: platform veto");
        return true;
    }

    for (uint8_t image = 0; image < MCUBOOT_IMAGE_NUMBER; image++)
    {
        mcuboot_update_state_t state = mcuboot_update_state(image);

        if (state != MCUBOOT_UPDATE_NONE)
        {
            BOOT_LOG_INF("update guard: image %u update in flight (state %d)", (unsigned)image, (int)state);
            return true;
        }
    }

    return false;
}
