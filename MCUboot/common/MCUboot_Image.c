//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <string.h>
#include <stdbool.h>
#include <MCUboot_Image.h>

#ifdef NF_FEATURE_HAS_MCUBOOT

#include <sysflash/sysflash.h>
#include <bootutil/bootutil_public.h>

nf_mcuboot_image_details_t nf_mcuboot_get_image_details(int image_index)
{
    nf_mcuboot_image_details_t details;
    memset(&details, 0, sizeof(details));

    if (image_index != NF_MCUBOOT_IMAGE_CLR && image_index != NF_MCUBOOT_IMAGE_DEPLOY)
    {
        details.rc = NF_MCUBOOT_INVALID_IMAGE;
        return details;
    }

    struct boot_swap_state state;
    int rc = boot_read_swap_state_by_id(FLASH_AREA_IMAGE_PRIMARY(image_index), &state);
    if (rc != 0)
    {
        details.rc = NF_MCUBOOT_FLASH_MAP_ERROR;
        return details;
    }

    int swap_type = boot_swap_type_multi(image_index);
    if (swap_type < 0)
    {
        details.rc = NF_MCUBOOT_SWAP_TYPE_ERROR;
        return details;
    }

    details.swap_type = swap_type;
    details.copy_done = state.copy_done;
    details.image_ok = state.image_ok;
    details.magic = state.magic;
    details.rc = NF_MCUBOOT_SUCCESS;
    return details;
}

nf_mcuboot_error_t nf_mcuboot_set_image_revert(int image_index)
{
    if (image_index != NF_MCUBOOT_IMAGE_CLR && image_index != NF_MCUBOOT_IMAGE_DEPLOY)
    {
        return NF_MCUBOOT_INVALID_IMAGE;
    }

    struct boot_swap_state state;
    int rc = boot_read_swap_state_by_id(FLASH_AREA_IMAGE_PRIMARY(image_index), &state);
    if (rc != 0)
    {
        return NF_MCUBOOT_FLASH_MAP_ERROR;
    }

    // Revert is only possible when a swap has completed (copy_done) but the
    // app has not yet confirmed the image (image_ok not set). In this state
    // MCUboot will automatically revert on the next reboot without any flash
    // write being necessary.
    if (state.copy_done != BOOT_FLAG_SET || state.image_ok == BOOT_FLAG_SET)
    {
        return NF_MCUBOOT_INVALID_STATE;
    }

    // Image is unconfirmed — revert will occur on next reboot automatically.
    return NF_MCUBOOT_SUCCESS;
}

bool nf_mcuboot_image_needs_confirm(int image_index, nf_mcuboot_error_t *out_error)
{
    if (image_index != NF_MCUBOOT_IMAGE_CLR && image_index != NF_MCUBOOT_IMAGE_DEPLOY)
    {
        if (out_error != NULL)
        {
            *out_error = NF_MCUBOOT_INVALID_IMAGE;
        }

        return false;
    }

    struct boot_swap_state state;
    int rc = boot_read_swap_state_by_id(FLASH_AREA_IMAGE_PRIMARY(image_index), &state);
    if (rc != 0)
    {
        if (out_error != NULL)
        {
            *out_error = NF_MCUBOOT_FLASH_MAP_ERROR;
        }

        return false;
    }

    if (out_error != NULL)
    {
        *out_error = NF_MCUBOOT_SUCCESS;
    }

    return (state.copy_done == BOOT_FLAG_SET) && (state.image_ok != BOOT_FLAG_SET);
}

#endif // NF_FEATURE_HAS_MCUBOOT
