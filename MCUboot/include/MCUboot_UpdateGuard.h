//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
//
// Update guard: tells the bootloader whether an update is in flight, so that operations which
// rewrite the secondary slots (serial recovery upload, ...) are not started on top of it.
//
// What counts as "in flight", per image:
//   - an interrupted swap (primary trailer: magic good, copy not done, swap type set)
//   - a swap scheduled for the next boot (TEST / PERM staged), or an unconfirmed image in the
//     primary slot that will be reverted (the revert copy lives in the secondary slot)
//   - a slot state that cannot be read or classified
//
// The guard fails closed: whatever it cannot prove idle is reported as in flight.
//
// It is built only on MCUboot public APIs (boot_read_swap_state_by_id, boot_swap_type_multi),
// so it needs no bootloader internals and does not touch MCUboot's global loader state.
// Call it before boot_go(), after the flash devices behind the flash_area_* API are ready.
//
// Platforms can veto on top of the generic checks by providing mcuboot_update_guard_platform_veto().

#ifndef MCUBOOT_UPDATEGUARD_H
#define MCUBOOT_UPDATEGUARD_H

#include <stdbool.h>
#include <stdint.h>

#include <bootutil/bootutil_public.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum
    {
        // nothing in flight for the image
        MCUBOOT_UPDATE_NONE = 0,

        // a swap was started and not completed; MCUboot has to finish it
        MCUBOOT_UPDATE_SWAP_INTERRUPTED,

        // a swap is scheduled for the next boot, or an unconfirmed image will be reverted
        MCUBOOT_UPDATE_SWAP_PENDING,

        // the slot state could not be read or classified
        MCUBOOT_UPDATE_STATE_UNKNOWN,
    } mcuboot_update_state_t;

    // True when the primary slot trailer says a swap was started and has not completed.
    bool mcuboot_swap_interrupted(const struct boot_swap_state *primary);

    // Classify what is going on for one image.
    mcuboot_update_state_t mcuboot_update_state(uint8_t image);

    // True when any image has an update in flight, or the platform vetoes.
    bool mcuboot_update_in_progress(void);

    // Platform hook, return true to report an update in flight regardless of the slot state.
    // The default (weak) implementation returns false.
    bool mcuboot_update_guard_platform_veto(void);

#ifdef __cplusplus
}
#endif

#endif // MCUBOOT_UPDATEGUARD_H
