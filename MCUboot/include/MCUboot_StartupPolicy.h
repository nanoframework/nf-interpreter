//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef MCUBOOT_STARTUP_POLICY_H
#define MCUBOOT_STARTUP_POLICY_H

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef NF_FEATURE_HAS_MCUBOOT

#include <MCUboot_RuntimeInterface.h>

    /**
     * @brief Initialize MCUboot startup policies for both CLR and deploy images.
     *
     * Call this early in nanoCLR startup (for example in main() after HAL init).
     * It does not perform image confirmation; confirmation happens later via
     * nf_mcuboot_startup_ok() and nf_mcuboot_deploy_ok().
     */
    void nf_mcuboot_startup_init(void);

    /**
     * @brief Confirm deploy image after deployment validation passes.
     *
     * Called after managed deployment logic verifies the new application code.
     * If deploy image is in test state (unconfirmed), this finalizes the upgrade.
     *
     * @return NF_MCUBOOT_SUCCESS on success; NF_MCUBOOT_* error code on failure
     */
    nf_mcuboot_error_t nf_mcuboot_deploy_ok(void);

    /**
     * @brief Handle deployment validation failure by triggering revert on reboot.
     *
     * Called when deployment validation fails. Leaves the deploy image unconfirmed
     * so MCUboot reverts to the previous version on next boot.
     *
     * @return Always returns NF_MCUBOOT_SUCCESS
     */
    nf_mcuboot_error_t nf_mcuboot_deploy_failed(void);

#endif // NF_FEATURE_HAS_MCUBOOT

#ifdef __cplusplus
}
#endif

#endif // MCUBOOT_STARTUP_POLICY_H
