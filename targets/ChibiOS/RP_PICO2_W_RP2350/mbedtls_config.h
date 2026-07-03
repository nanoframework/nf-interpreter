//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// MbedTLS configuration overrides for Pico 2 W (RP2350).
// This file is included before the nanoFramework-wide mbedtls config.
// Keep empty to use platform defaults; add overrides as needed.

#ifndef MBEDTLS_CONFIG_H
#define MBEDTLS_CONFIG_H

// RP2350 uses ROSC random-bit sampling for entropy (see
// mbedtls_entropy_hardware_pool.c).  PLATFORM_HAS_RNG is set in
// target_common.h.in so that MBEDTLS_ENTROPY_HARDWARE_ALT is enabled.

// RP2350 has 520 KB SRAM vs 264 KB on RP2040, so we can afford larger buffers.
// Still constrained compared to larger MCUs; limit to what TLS 1.2 needs.
#define MBEDTLS_SSL_IN_CONTENT_LEN  8192
#define MBEDTLS_SSL_OUT_CONTENT_LEN 4096

// Disable hardware acceleration alternative implementations on RP2350
// (x86 PadLock, etc. are not available on ARM Cortex-M33)
#undef MBEDTLS_PADLOCK_C

// Disable PSA_CRYPTO on RP2350. PSA operations (key import, key derivation)
// fail without proper platform support, causing SSL_HW_ACCEL_FAILED errors.
// RP2350 can run TLS 1.2 with legacy mbedTLS crypto primitives.
#undef MBEDTLS_USE_PSA_CRYPTO

#endif // MBEDTLS_CONFIG_H
