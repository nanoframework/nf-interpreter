//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// MbedTLS configuration overrides for Pico 2 W (RP2350).
// This file is included before the nanoFramework-wide mbedtls config.
// Keep empty to use platform defaults; add overrides as needed.

#ifndef MBEDTLS_CONFIG_H
#define MBEDTLS_CONFIG_H

// RP2350 has 520 KB SRAM vs 264 KB on RP2040, so we can afford larger buffers.
// Still constrained compared to larger MCUs; limit to what TLS 1.2 needs.
#define MBEDTLS_SSL_IN_CONTENT_LEN  8192
#define MBEDTLS_SSL_OUT_CONTENT_LEN 4096

#endif // MBEDTLS_CONFIG_H
