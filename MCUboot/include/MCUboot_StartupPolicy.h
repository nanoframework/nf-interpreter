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

#include <stdbool.h>
#include <stdint.h>
#include <MCUboot_RuntimeInterface.h>

    /**
     * @brief Initialize MCUboot startup policies for both CLR and deploy images.
     *
     * Call this early in nanoCLR startup (for example in main() after HAL init).
     * It does not perform image confirmation; confirmation happens later via
     * nf_mcuboot_startup_ok() (image 0) and nf_mcuboot_deploy_ok() (image 1).
     */
    void nf_mcuboot_startup_init(void);

    /**
     * @brief True when the deployment image (image 1) in the primary slot is valid but not yet confirmed.
     *
     * Reads the live trailer, so a confirmation made after power-on is taken into account.
     * Returns false when the trailer can't be read.
     */
    bool nf_mcuboot_deploy_needs_confirmation(void);

    /**
     * @brief Arm the deployment confirmation watchdog.
     *
     * When it expires the CPU is reset and MCUboot reverts the unconfirmed deployment.
     * Only arms when seconds is not 0 and the deployment needs confirmation. It arms at most once per
     * power cycle: a CLR soft reboot neither re-arms nor extends it, and nothing re-arms it once it
     * was cancelled.
     *
     * @param seconds Timeout in seconds, 0 to leave it disarmed.
     *
     * @return true when this call armed the watchdog
     */
    bool nf_mcuboot_deploy_watchdog_arm(uint32_t seconds);

    /**
     * @brief Cancel the deployment confirmation watchdog for the rest of this power cycle.
     *
     * Used when a debugger session starts. Safe to call when it isn't armed.
     */
    void nf_mcuboot_deploy_watchdog_cancel(void);

    /**
     * @brief Confirm the deployment image (image 1).
     *
     * Single confirmation path for image 1: stops the confirmation watchdog before the trailer write
     * and re-arms it when the confirmation fails. The trailer is read back to check image_ok.
     *
     * @return NF_MCUBOOT_SUCCESS when image 1 is confirmed; NF_MCUBOOT_* error code otherwise
     */
    nf_mcuboot_error_t nf_mcuboot_deploy_ok(void);

    /**
     * @brief Platform hook: start a one-shot timer that resets the CPU when it expires.
     *
     * Weak default returns false (not supported). RTOS targets override it.
     *
     * @return true when the timer was started
     */
    bool nf_mcuboot_confirm_timer_start(uint32_t seconds);

    /**
     * @brief Platform hook: stop and release the timer started by nf_mcuboot_confirm_timer_start().
     *
     * Only called from thread context and only after a successful start.
     */
    void nf_mcuboot_confirm_timer_stop(void);

#endif // CONFIG_NF_FEATURE_HAS_MCUBOOT

#ifdef __cplusplus
}
#endif

#endif // MCUBOOT_STARTUP_POLICY_H
