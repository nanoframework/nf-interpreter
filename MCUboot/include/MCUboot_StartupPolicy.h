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

#if defined(CONFIG_NF_FEATURE_HAS_MCUBOOT) && CONFIG_NF_FEATURE_HAS_MCUBOOT

#include <MCUboot_RuntimeInterface.h>

    /**
     * @brief Initialize MCUboot startup policies for both CLR and deploy images.
     *
     * Call this early in nanoCLR startup (for example in main() after HAL init).
     * It does not perform image confirmation; confirmation happens later via
     * nf_mcuboot_startup_ok() and nf_mcuboot_deploy_ok().
     */
    void nf_mcuboot_startup_init(void);

#endif // CONFIG_NF_FEATURE_HAS_MCUBOOT

#ifdef __cplusplus
}
#endif

#endif // MCUBOOT_STARTUP_POLICY_H
