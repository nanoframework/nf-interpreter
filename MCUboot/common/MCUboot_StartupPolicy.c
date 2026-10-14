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

// Both images were swapped in this boot and are still unconfirmed: they are kept or reverted as a
// pair, so image 0 is confirmed by nf_mcuboot_deploy_ok() together with image 1. A deployment that
// never confirms then reverts the CLR it came with, instead of leaving it with the old deployment.
// See "Paired confirmation" in MCUboot/docs/confirmation-timeout.md.
bool nf_mcuboot_clr_confirm_paired(void)
{
    return s_boot_state_valid && needs_confirmation(&s_clr_boot_state) && needs_confirmation(&s_deploy_boot_state);
}

nf_mcuboot_error_t nf_mcuboot_startup_ok(void)
{
    if (s_boot_state_valid)
    {
        if (needs_confirmation(&s_clr_boot_state) && !nf_mcuboot_clr_confirm_paired())
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

// Deployment confirmation watchdog state. See MCUboot/docs/confirmation-timeout.md.
// set once the watchdog was armed or cancelled in this power cycle: it never arms again after that
static bool s_deploy_watchdog_settled = false;
static bool s_deploy_watchdog_running = false;
static uint32_t s_deploy_watchdog_seconds = 0;

__attribute__((weak)) bool nf_mcuboot_confirm_timer_start(uint32_t seconds)
{
    (void)seconds;
    return false;
}

__attribute__((weak)) void nf_mcuboot_confirm_timer_stop(void)
{
}

static bool read_deploy_state(struct boot_swap_state *state)
{
    return boot_read_swap_state_by_id(FLASH_AREA_IMAGE_PRIMARY(NF_MCUBOOT_IMAGE_DEPLOY), state) == 0;
}

bool nf_mcuboot_deploy_needs_confirmation(void)
{
    struct boot_swap_state state;

    return read_deploy_state(&state) && needs_confirmation(&state);
}

static void stop_deploy_watchdog(void)
{
    if (s_deploy_watchdog_running)
    {
        nf_mcuboot_confirm_timer_stop();
        s_deploy_watchdog_running = false;
    }
}

bool nf_mcuboot_deploy_watchdog_arm(uint32_t seconds)
{
    if (s_deploy_watchdog_settled || seconds == 0 || !nf_mcuboot_deploy_needs_confirmation())
    {
        return false;
    }

    s_deploy_watchdog_settled = true;
    s_deploy_watchdog_seconds = seconds;
    s_deploy_watchdog_running = nf_mcuboot_confirm_timer_start(seconds);

    return s_deploy_watchdog_running;
}

void nf_mcuboot_deploy_watchdog_cancel(void)
{
    s_deploy_watchdog_settled = true;
    stop_deploy_watchdog();
}

nf_mcuboot_error_t nf_mcuboot_deploy_ok(void)
{
    bool wasRunning = s_deploy_watchdog_running;
    struct boot_swap_state state;

    // stop before the trailer write so the reset can't land in the middle of it
    stop_deploy_watchdog();

    nf_mcuboot_error_t result = nf_mcuboot_confirm_image(NF_MCUBOOT_IMAGE_DEPLOY);

    // a zero return from MCUboot alone doesn't mean image_ok is set
    if (result == NF_MCUBOOT_SUCCESS && (!read_deploy_state(&state) || state.image_ok != BOOT_FLAG_SET))
    {
        result = NF_MCUBOOT_SYSTEM_ERROR;
    }

    if (result != NF_MCUBOOT_SUCCESS)
    {
        if (wasRunning)
        {
            // keep the failsafe in place: the image is still unconfirmed
            s_deploy_watchdog_running = nf_mcuboot_confirm_timer_start(s_deploy_watchdog_seconds);
        }

        return result;
    }

    // image 0 was held back by nf_mcuboot_startup_ok() to be kept together with this deployment
    if (nf_mcuboot_clr_confirm_paired())
    {
        if (boot_read_swap_state_by_id(FLASH_AREA_IMAGE_PRIMARY(NF_MCUBOOT_IMAGE_CLR), &state) != 0)
        {
            return NF_MCUBOOT_FLASH_MAP_ERROR;
        }

        if (needs_confirmation(&state))
        {
            result = nf_mcuboot_confirm_image(NF_MCUBOOT_IMAGE_CLR);
        }
    }

    return result;
}

#endif // CONFIG_NF_FEATURE_HAS_MCUBOOT
