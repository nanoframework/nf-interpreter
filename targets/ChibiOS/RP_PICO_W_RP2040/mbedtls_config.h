//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// MbedTLS configuration overrides for Pico W.
// This file is included before the nanoFramework-wide mbedtls config.
// Keep empty to use platform defaults; add overrides as needed.

#ifndef MBEDTLS_CONFIG_H
#define MBEDTLS_CONFIG_H

// RP2040 uses ROSC random-bit sampling for entropy (see
// mbedtls_entropy_hardware_pool.c).  PLATFORM_HAS_RNG is set in
// target_common.h.in so that MBEDTLS_ENTROPY_HARDWARE_ALT is enabled.

// Reduce SSL record buffers for RAM-constrained RP2040.
// Default is 16384 each (33 KB total) which exhausts the 64 KB CLR heap.
// 4096 is sufficient for TLS 1.2 with most servers.
#define MBEDTLS_SSL_IN_CONTENT_LEN   6144 
#define MBEDTLS_SSL_OUT_CONTENT_LEN  4096

#endif // MBEDTLS_CONFIG_H
