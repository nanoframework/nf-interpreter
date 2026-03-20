//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// MbedTLS configuration overrides for Pico W.
// This file is included before the nanoFramework-wide mbedtls config.
// Keep empty to use platform defaults; add overrides as needed.

#ifndef MBEDTLS_CONFIG_H
#define MBEDTLS_CONFIG_H

// RP2040 does not have a hardware RNG, so MbedTLS will use
// the software entropy source configured in the platform config.

#endif // MBEDTLS_CONFIG_H
