//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <stdbool.h>
#include <MCUboot_StartupPolicy.h>

#if defined(CONFIG_NF_FEATURE_HAS_MCUBOOT) && CONFIG_NF_FEATURE_HAS_MCUBOOT

#include <sysflash/sysflash.h>
#include <bootutil/bootutil_public.h>

// Boot-time snapshot of the MCUboot image trailer for each image.
// Read once in nf_mcuboot_startup_init() from the primary slot of each image
// so that subsequent confirmation decisions reflect the state at power-on,
// not the live flash state at the time the decision is made.
static struct boot_swap_state s_clr_boot_state;
static struct boot_swap_state s_deploy_boot_state;
static bool s_boot_state_valid = false;

// True when the primary slot holds a valid image (magic GOOD) that is not yet confirmed
// (image_ok not SET): the app must confirm it or MCUboot discards it on the next boot.
// copy_done is deliberately not required - a bootstrap copy writes only magic, and
// boot_set_next() confirms a magic=GOOD image without it.
static bool needs_confirmation(const struct boot_swap_state *state)
{
    return (state->magic == BOOT_MAGIC_GOOD) && (state->image_ok != BOOT_FLAG_SET);
}

void nf_mcuboot_startup_init(void)
{
    int rc0 = boot_read_swap_state_by_id(FLASH_AREA_IMAGE_PRIMARY(NF_MCUBOOT_IMAGE_CLR), &s_clr_boot_state);
    int rc1 = boot_read_swap_state_by_id(FLASH_AREA_IMAGE_PRIMARY(NF_MCUBOOT_IMAGE_DEPLOY), &s_deploy_boot_state);
    s_boot_state_valid = (rc0 == 0) && (rc1 == 0);
}

nf_mcuboot_error_t nf_mcuboot_startup_ok(void)
{
    if (s_boot_state_valid)
    {
        if (needs_confirmation(&s_clr_boot_state))
        {
            return nf_mcuboot_confirm_image(NF_MCUBOOT_IMAGE_CLR);
        }

        return NF_MCUBOOT_SUCCESS;
    }

    // Fallback: cache was not populated — read live state.
    int swap_type;
    nf_mcuboot_error_t result = nf_mcuboot_image_state(NF_MCUBOOT_IMAGE_CLR, &swap_type);
    if (result != NF_MCUBOOT_SUCCESS)
    {
        return result;
    }

    if (swap_type == BOOT_SWAP_TYPE_TEST)
    {
        return nf_mcuboot_confirm_image(NF_MCUBOOT_IMAGE_CLR);
    }

    return NF_MCUBOOT_SUCCESS;
}

nf_mcuboot_error_t nf_mcuboot_deploy_ok(void)
{
    if (s_boot_state_valid)
    {
        if (needs_confirmation(&s_deploy_boot_state))
        {
            return nf_mcuboot_confirm_image(NF_MCUBOOT_IMAGE_DEPLOY);
        }

        return NF_MCUBOOT_SUCCESS;
    }

    // Fallback: cache was not populated — read live state.
    int swap_type;
    nf_mcuboot_error_t result = nf_mcuboot_image_state(NF_MCUBOOT_IMAGE_DEPLOY, &swap_type);
    if (result != NF_MCUBOOT_SUCCESS)
    {
        return result;
    }

    if (swap_type == BOOT_SWAP_TYPE_TEST)
    {
        return nf_mcuboot_confirm_image(NF_MCUBOOT_IMAGE_DEPLOY);
    }

    return NF_MCUBOOT_SUCCESS;
}

nf_mcuboot_error_t nf_mcuboot_deploy_failed(void)
{
    // No action required: keeping the deploy image unconfirmed causes MCUboot
    // to revert to the previous image on the next reboot.
    return NF_MCUBOOT_SUCCESS;
}

#endif // CONFIG_NF_FEATURE_HAS_MCUBOOT
