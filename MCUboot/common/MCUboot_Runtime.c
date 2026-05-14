//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <MCUboot_RuntimeInterface.h>

#ifdef NF_FEATURE_HAS_MCUBOOT

#include <bootutil/bootutil_public.h>

/**
 * @brief Query the current image state and boot swap type.
 *
 * Wrapper around MCUboot's boot_swap_type_multi() API.
 * Provides stable error interface for cross-platform integration.
 */
nf_mcuboot_error_t nf_mcuboot_image_state(int image_index, int *out_swap_type)
{
    // Validate image index (only 0 and 1 are supported)
    if (image_index != NF_MCUBOOT_IMAGE_CLR && image_index != NF_MCUBOOT_IMAGE_DEPLOY)
    {
        return NF_MCUBOOT_INVALID_IMAGE;
    }

    if (out_swap_type == NULL)
    {
        return NF_MCUBOOT_INVALID_STATE;
    }

    // Call MCUboot API to get swap type for this image
    int swap_type = boot_swap_type_multi((int)image_index);

    // Return MCUboot's swap type directly (BOOT_SWAP_TYPE_* values)
    if (swap_type < 0)
    {
        return NF_MCUBOOT_SWAP_TYPE_ERROR;
    }

    *out_swap_type = swap_type;
    return NF_MCUBOOT_SUCCESS;
}

/**
 * @brief Confirm (finalize) an image upgrade for the given image.
 *
 * Wrapper around MCUboot's boot_set_confirmed_multi() API.
 * Per-image confirmation: confirming image 0 does not affect image 1.
 */
nf_mcuboot_error_t nf_mcuboot_confirm_image(int image_index)
{
    // Validate image index
    if (image_index != NF_MCUBOOT_IMAGE_CLR && image_index != NF_MCUBOOT_IMAGE_DEPLOY)
    {
        return NF_MCUBOOT_INVALID_IMAGE;
    }

    // Call MCUboot API to confirm this image
    int result = boot_set_confirmed_multi((int)image_index);

    if (result != 0)
    {
        // MCUboot error (could be flash error, invalid state, etc.)
        return NF_MCUBOOT_SYSTEM_ERROR;
    }

    return NF_MCUBOOT_SUCCESS;
}

/**
 * @brief Set an image to pending (test) state without confirming it.
 *
 * Wrapper around MCUboot's boot_set_pending_multi() API.
 * Per-image operation: setting image 0 pending does not affect image 1.
 */
nf_mcuboot_error_t nf_mcuboot_set_image_pending(int image_index, int permanent)
{
    // Validate image index
    if (image_index != NF_MCUBOOT_IMAGE_CLR && image_index != NF_MCUBOOT_IMAGE_DEPLOY)
    {
        return NF_MCUBOOT_INVALID_IMAGE;
    }

    // Call MCUboot API to set pending state for this image
    // second parameter: permanent (non-zero = permanent, 0 = test/revertible)
    int result = boot_set_pending_multi((int)image_index, permanent);

    if (result != 0)
    {
        // MCUboot error
        return NF_MCUBOOT_SYSTEM_ERROR;
    }

    return NF_MCUBOOT_SUCCESS;
}

#endif // NF_FEATURE_HAS_MCUBOOT
