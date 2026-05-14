//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef MCUBOOT_RUNTIME_INTERFACE_H
#define MCUBOOT_RUNTIME_INTERFACE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef NF_FEATURE_HAS_MCUBOOT

#include <bootutil/bootutil_public.h>

// Image index constants (convention: 0 = CLR, 1 = deployment)
#define NF_MCUBOOT_IMAGE_CLR    0
#define NF_MCUBOOT_IMAGE_DEPLOY 1

    /**
     * @brief MCUboot runtime error codes.
     *
     * These map MCUboot bootutil API return codes to a stable error interface
     * for decoupled integration with Wire Protocol and other subsystems.
     */
    typedef enum
    {
        NF_MCUBOOT_SUCCESS = 0,           ///< Operation succeeded
        NF_MCUBOOT_INVALID_IMAGE = -1,    ///< Invalid image index (not 0 or 1)
        NF_MCUBOOT_INVALID_STATE = -2,    ///< Invalid state for operation
        NF_MCUBOOT_SWAP_TYPE_ERROR = -3,  ///< Error reading/interpreting swap type
        NF_MCUBOOT_FLASH_MAP_ERROR = -4,  ///< Flash area mapping error
        NF_MCUBOOT_SYSTEM_ERROR = -5,     ///< Underlying system (HAL/PAL) error
        NF_MCUBOOT_NOT_IMPLEMENTED = -99, ///< Feature not implemented for this target
    } nf_mcuboot_error_t;

    /**
     * @brief Query the current image state and boot swap type.
     *
     * @param image_index Image index (0=CLR, 1=deployment)
     * @param out_swap_type Pointer to store the swap type (BOOT_SWAP_TYPE_* constants)
     *
     * @return NF_MCUBOOT_SUCCESS on success; NF_MCUBOOT_* error code on failure
     *
     * @note This function is cross-platform and works independently for each image.
     *       Image 0 and image 1 states are not coupled.
     *
     * Usage:
     *   int swap_type;
     *   int result = nf_mcuboot_image_state(0, &swap_type);
     *   if (result == NF_MCUBOOT_SUCCESS && swap_type == BOOT_SWAP_TYPE_TEST) {
     *       // CLR image is running from secondary (unconfirmed)
     *   }
     */
    nf_mcuboot_error_t nf_mcuboot_image_state(int image_index, int *out_swap_type);

    /**
     * @brief Confirm (finalize) an image upgrade for the given image.
     *
     * Marks the image in the secondary slot as permanent. After confirmation,
     * the image will not be reverted to primary on next boot even if not marked ok.
     *
     * @param image_index Image index (0=CLR, 1=deployment)
     *
     * @return NF_MCUBOOT_SUCCESS on success; NF_MCUBOOT_* error code on failure
     *
     * @note Per-image operation: confirming image 0 does not affect image 1.
     *       Should be called only after validation/health checks pass.
     *
     * Usage:
     *   int swap_type;
     *   if (nf_mcuboot_image_state(0, &swap_type) == NF_MCUBOOT_SUCCESS &&
     *       swap_type == BOOT_SWAP_TYPE_TEST) {
     *       // Perform CLR health checks...
     *       if (clr_startup_ok) {
     *           nf_mcuboot_confirm_image(0);
     *       }
     *   }
     */
    nf_mcuboot_error_t nf_mcuboot_confirm_image(int image_index);

    /**
     * @brief Set an image to pending (test) state without confirming it.
     *
     * Prepares the image for revert on next boot if not explicitly confirmed.
     * Used during testing phase to enable rollback if validation fails.
     *
     * @param image_index Image index (0=CLR, 1=deployment)
     * @param permanent If true, marks the swap as permanent (no revert possible).
     *                  If false, keeps the swap in test state (revertible).
     *
     * @return NF_MCUBOOT_SUCCESS on success; NF_MCUBOOT_* error code on failure
     *
     * @note Per-image operation: setting image 0 pending does not affect image 1.
     *       Typically called before deployment to enable validation/rollback.
     *
     * Usage:
     *   // Before deploying a new managed application:
     *   nf_mcuboot_set_image_pending(1, false);
     *   // ... validate deployment ...
     *   // If validation passes:
     *   nf_mcuboot_confirm_image(1);
     *   // If validation fails, reboot without confirming and MCUboot reverts.
     */
    nf_mcuboot_error_t nf_mcuboot_set_image_pending(int image_index, int permanent);

    /**
     * @brief Confirm the CLR image after startup health checks pass.
     *
     * Uses the boot-time state snapshot captured by nf_mcuboot_startup_init().
     * If the CLR image is running under an unconfirmed swap, this finalizes the
     * upgrade so it persists across the next reboot.
     * Falls back to a live flash read if the boot snapshot was not available.
     *
     * @return NF_MCUBOOT_SUCCESS on success; NF_MCUBOOT_* error code on failure
     *
     * @note This is a no-op if the CLR image is already confirmed or was not swapped.
     */
    nf_mcuboot_error_t nf_mcuboot_startup_ok(void);

#endif // NF_FEATURE_HAS_MCUBOOT

#ifdef __cplusplus
}
#endif

#endif // MCUBOOT_RUNTIME_INTERFACE_H
