//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef MCUBOOT_IMAGE_H
#define MCUBOOT_IMAGE_H

#include <stdbool.h>
#include <stdint.h>
#include <MCUboot_RuntimeInterface.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef NF_FEATURE_HAS_MCUBOOT

    /**
     * @brief Combined image state returned by nf_mcuboot_get_image_details().
     *
     * Captures both the MCUboot swap type and the primary-slot trailer flags
     * in a single flash read sequence, avoiding redundant reads by callers.
     *
     * swap_type values: BOOT_SWAP_TYPE_NONE / TEST / PERM / REVERT / FAIL
     * copy_done / image_ok / magic values: BOOT_FLAG_SET / BOOT_FLAG_UNSET / BOOT_FLAG_BAD
     */
    typedef struct
    {
        int swap_type;         ///< BOOT_SWAP_TYPE_* value; valid only when rc == NF_MCUBOOT_SUCCESS
        uint8_t copy_done;     ///< BOOT_FLAG_* — whether MCUboot completed a swap into primary
        uint8_t image_ok;      ///< BOOT_FLAG_* — whether the app confirmed the image
        uint8_t magic;         ///< BOOT_MAGIC_* — trailer magic validity
        nf_mcuboot_error_t rc; ///< NF_MCUBOOT_SUCCESS or error code; check before using other fields
    } nf_mcuboot_image_details_t;

    /**
     * @brief Query combined image state in a single call.
     *
     * Reads the primary-slot trailer (copy_done, image_ok, magic) and the swap
     * type for the given image, returning them together to avoid duplicate flash
     * reads by the caller.
     *
     * @param image_index Image index (NF_MCUBOOT_IMAGE_CLR=0 or NF_MCUBOOT_IMAGE_DEPLOY=1)
     *
     * @return nf_mcuboot_image_details_t — always check the .rc field first.
     *
     * Usage (Wire Protocol image-info handler):
     *   nf_mcuboot_image_details_t d = nf_mcuboot_get_image_details(NF_MCUBOOT_IMAGE_DEPLOY);
     *   if (d.rc == NF_MCUBOOT_SUCCESS) {
     *       // d.swap_type, d.copy_done, d.image_ok, d.magic are all valid
     *   }
     */
    nf_mcuboot_image_details_t nf_mcuboot_get_image_details(int image_index);

    /**
     * @brief Signal that an image should be reverted on the next reboot.
     *
     * Validates that the given image is in an unconfirmed test state
     * (copy_done == SET, image_ok != SET). If so, returns NF_MCUBOOT_SUCCESS —
     * no flash write is needed because MCUboot reverts automatically when image_ok
     * is not set at boot time.
     *
     * The caller must trigger a system reboot after this call to complete the revert.
     *
     * @param image_index Image index (NF_MCUBOOT_IMAGE_CLR=0 or NF_MCUBOOT_IMAGE_DEPLOY=1)
     *
     * @return NF_MCUBOOT_SUCCESS      — image is unconfirmed; revert will occur on next reboot
     *         NF_MCUBOOT_INVALID_IMAGE — image_index is not 0 or 1
     *         NF_MCUBOOT_INVALID_STATE — image is already confirmed or was not swapped
     *         NF_MCUBOOT_FLASH_MAP_ERROR — could not read primary slot trailer
     *
     * Usage (Wire Protocol "cancel deployment" handler):
     *   nf_mcuboot_error_t rc = nf_mcuboot_set_image_revert(NF_MCUBOOT_IMAGE_DEPLOY);
     *   if (rc == NF_MCUBOOT_SUCCESS) {
     *       // Trigger system reboot — MCUboot will revert the deploy image
     *       HAL_REBOOT();
     *   }
     */
    nf_mcuboot_error_t nf_mcuboot_set_image_revert(int image_index);

    /**
     * @brief Check whether an image is in an unconfirmed test state and needs app confirmation.
     *
     * Returns true when copy_done == BOOT_FLAG_SET and image_ok != BOOT_FLAG_SET,
     * meaning a swap completed but the app has not yet confirmed the image.
     * This is the authoritative pre-confirmation check for Wire Protocol handlers.
     *
     * @param image_index  Image index (NF_MCUBOOT_IMAGE_CLR=0 or NF_MCUBOOT_IMAGE_DEPLOY=1)
     * @param[out] out_error  Set to NF_MCUBOOT_SUCCESS on success, or an error code on failure.
     *                        May be NULL if the caller does not need the error code.
     *
     * @return true  — image needs confirmation (will revert if not confirmed before reboot)
     *         false — image is confirmed, not swapped, or an error occurred (check out_error)
     *
     * Usage (Wire Protocol deployment validation):
     *   nf_mcuboot_error_t err;
     *   bool needs = nf_mcuboot_image_needs_confirm(NF_MCUBOOT_IMAGE_DEPLOY, &err);
     *   if (err != NF_MCUBOOT_SUCCESS) { // handle error }
     *   if (needs) { // safe to call nf_mcuboot_confirm_image() }
     */
    bool nf_mcuboot_image_needs_confirm(int image_index, nf_mcuboot_error_t *out_error);

#endif // NF_FEATURE_HAS_MCUBOOT

#ifdef __cplusplus
}
#endif

#endif // MCUBOOT_IMAGE_H
